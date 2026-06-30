#include "page_table.h"
#include "config.h"

/* ---------------------------------------------------------------
 * Tabela de páginas global — uma entrada por página lógica.
 * --------------------------------------------------------------- */
static page_table_entry_t page_table[PAGE_TABLE_SIZE];

/* ---------------------------------------------------------------
 * page_table_init
 * Inicializa todas as entradas como inválidas.
 * --------------------------------------------------------------- */
void page_table_init(void) {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i].frame          = -1;
        page_table[i].valid          = 0;
        page_table[i].reference_bit  = 0;
        page_table[i].aging_counter  = 0;
    }
}

/* ---------------------------------------------------------------
 * page_table_lookup
 * Retorna o frame mapeado à página, ou -1 se inválida.
 * --------------------------------------------------------------- */
int page_table_lookup(int page) {
    if (page_table[page].valid == 1) {
        return page_table[page].frame;
    }
    return -1;
}

/* ---------------------------------------------------------------
 * page_table_update
 * Registra o mapeamento página → frame e marca como válida.
 * Zera reference_bit e aging_counter (página recém-carregada).
 * --------------------------------------------------------------- */
void page_table_update(int page, int frame) {
    page_table[page].frame         = frame;
    page_table[page].valid         = 1;
    page_table[page].reference_bit = 0;
    page_table[page].aging_counter = 0;
}

/* ---------------------------------------------------------------
 * page_table_invalidate
 * Marca a página como não-presente (page out).
 * --------------------------------------------------------------- */
void page_table_invalidate(int page) {
    page_table[page].valid = 0;
    page_table[page].frame = -1;
}

/* ---------------------------------------------------------------
 * page_table_set_reference
 * Seta reference_bit = 1 (chamado a cada acesso à página).
 * --------------------------------------------------------------- */
void page_table_set_reference(int page) {
    page_table[page].reference_bit = 1;
}

/* ---------------------------------------------------------------
 * page_table_update_aging
 * Atualiza contadores de aging de todas as entradas válidas.
 * Algoritmo (LRU aproximado):
 *   1. counter >>= 1  (envelhece)
 *   2. se reference_bit == 1: counter |= 0x80 (registra acesso)
 *   3. reference_bit = 0
 * Quanto maior o counter, mais recente o último acesso.
 * Vítima = página com MENOR counter.
 * --------------------------------------------------------------- */
void page_table_update_aging(void) {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        if (page_table[i].valid == 1) {
            page_table[i].aging_counter >>= 1;

            if (page_table[i].reference_bit == 1) {
                page_table[i].aging_counter |= 0x80;
                page_table[i].reference_bit  = 0;
            }
        }
    }
}

/* ---------------------------------------------------------------
 * page_table_get_frame
 * Retorna o frame da página (sem checar validade).
 * --------------------------------------------------------------- */
int page_table_get_frame(int page) {
    if (page < 0 || page >= PAGE_TABLE_SIZE) {
        return -1;
    }
    return page_table[page].frame;
}

/* ---------------------------------------------------------------
 * page_table_is_valid
 * Retorna 1 se a entrada for válida, 0 caso contrário.
 * --------------------------------------------------------------- */
int page_table_is_valid(int page) {
    if (page < 0 || page >= PAGE_TABLE_SIZE) {
        return 0;
    }
    return page_table[page].valid;
}

/* ---------------------------------------------------------------
 * page_table_get_aging_counter
 * Retorna o aging_counter de uma página.
 * Usado por select_victim_page em memory.c.
 * --------------------------------------------------------------- */
unsigned char page_table_get_aging_counter(int page) {
    if (page < 0 || page >= PAGE_TABLE_SIZE) {
        return 0;
    }
    return page_table[page].aging_counter;
}
