#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "config.h"
#include "page_table.h"
#include "tlb.h"

/* ---------------------------------------------------------------
 * Memória física simulada (estática — não exposta no .h).
 * frame_to_page: mapeamento inverso frame → página (-1 = livre).
 * backing: ponteiro para o arquivo BACKING_STORE.bin.
 * --------------------------------------------------------------- */
static signed char physical_memory[NUM_FRAMES][FRAME_SIZE];
static int         frame_to_page[NUM_FRAMES];
static FILE       *backing = NULL;

/* ---------------------------------------------------------------
 * memory_init
 * Armazena o ponteiro do backing store e zera tudo.
 * --------------------------------------------------------------- */
void memory_init(FILE *backing_store) {
    backing = backing_store;

    for (int i = 0; i < NUM_FRAMES; i++) {
        frame_to_page[i] = -1;

        for (int j = 0; j < FRAME_SIZE; j++) {
            physical_memory[i][j] = 0;
        }
    }
}

/* ---------------------------------------------------------------
 * find_free_frame (estática — uso interno)
 * Retorna índice do primeiro frame livre, ou -1 se todos ocupados.
 * --------------------------------------------------------------- */
static int find_free_frame(void) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (frame_to_page[i] == -1) {
            return i;
        }
    }
    return -1;
}

/* ---------------------------------------------------------------
 * select_victim_page
 * LRU aproximado por aging: expulsa a página com MENOR
 * aging_counter dentre os frames ocupados.
 * --------------------------------------------------------------- */
int select_victim_page(void) {
    int victim_page        = -1;
    unsigned char min_aging = 0xFF;

    for (int i = 0; i < NUM_FRAMES; i++) {
        int page = frame_to_page[i];
        if (page != -1) {
            unsigned char aging = page_table_get_aging_counter(page);
            if (aging < min_aging) {
                min_aging   = aging;
                victim_page = page;
            }
        }
    }

    return victim_page;
}

/* ---------------------------------------------------------------
 * handle_page_fault
 * Carrega a página ausente do backing store para a memória física.
 *
 * 1. Procura frame livre.
 * 2. Se não houver, seleciona vítima, invalida na tabela e TLB,
 *    e reutiliza o frame liberado.
 * 3. fseek para page * PAGE_SIZE; fread de PAGE_SIZE bytes.
 * 4. Atualiza frame_to_page e page_table.
 * Retorna o número do frame onde a página foi carregada.
 * --------------------------------------------------------------- */
int handle_page_fault(int page) {
    int frame = find_free_frame();

    if (frame == -1) {
        int victim = select_victim_page();

        /* frame da vítima será reutilizado */
        frame = page_table_get_frame(victim);

        /* invalida na tabela de páginas e na TLB */
        page_table_invalidate(victim);
        tlb_remove(victim);

        /* libera o slot no mapeamento inverso */
        frame_to_page[frame] = -1;
    }

    /* Lê a página do backing store */
    if (backing == NULL) {
        fprintf(stderr, "Erro interno: BACKING_STORE nao inicializado.\n");
        exit(EXIT_FAILURE);
    }

    if (fseek(backing, (long)page * PAGE_SIZE, SEEK_SET) != 0) {
        fprintf(stderr, "Erro: fseek falhou para a pagina %d\n", page);
        exit(EXIT_FAILURE);
    }

    if (fread(physical_memory[frame], sizeof(signed char), PAGE_SIZE, backing)
            != PAGE_SIZE) {
        fprintf(stderr, "Erro: fread falhou para a pagina %d\n", page);
        exit(EXIT_FAILURE);
    }

    /* Atualiza mapeamentos */
    frame_to_page[frame] = page;
    page_table_update(page, frame);

    return frame;
}

/* ---------------------------------------------------------------
 * read_memory
 * Retorna o byte (com sinal) em physical_memory[frame][offset].
 * --------------------------------------------------------------- */
signed char read_memory(int frame, int offset) {
    return physical_memory[frame][offset];
}

/* ---------------------------------------------------------------
 * get_page_loaded_in_frame
 * Retorna qual página está carregada no frame (-1 se livre).
 * --------------------------------------------------------------- */
int get_page_loaded_in_frame(int frame) {
    if (frame < 0 || frame >= NUM_FRAMES) {
        return -1;
    }
    return frame_to_page[frame];
}
