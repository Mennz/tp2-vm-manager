#include "tlb.h"
#include "config.h"

/* ---------------------------------------------------------------
 * TLB global — array fixo de TLB_SIZE entradas.
 * fifo_next aponta para a próxima entrada a ser substituída (FIFO).
 * --------------------------------------------------------------- */
static tlb_entry_t tlb[TLB_SIZE];
static int fifo_next = 0;

/* ---------------------------------------------------------------
 * tlb_init
 * Invalida todas as entradas e reseta o ponteiro FIFO.
 * --------------------------------------------------------------- */
void tlb_init(void) {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page  = -1;
        tlb[i].frame = -1;
        tlb[i].valid = 0;
    }
    fifo_next = 0;
}

/* ---------------------------------------------------------------
 * tlb_lookup
 * Retorna o frame se encontrar entrada válida com .page == page.
 * Retorna -1 caso contrário (TLB miss).
 * --------------------------------------------------------------- */
int tlb_lookup(int page) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid == 1 && tlb[i].page == page) {
            return tlb[i].frame;
        }
    }
    return -1;
}

/* ---------------------------------------------------------------
 * tlb_insert
 * Insere mapeamento página → frame com política FIFO.
 * Passo 1: página já existe → só atualiza o frame.
 * Passo 2: slot livre disponível → usa ele.
 * Passo 3: TLB cheia → substitui fifo_next (circular).
 * --------------------------------------------------------------- */
void tlb_insert(int page, int frame) {
    /* Passo 1: página já está na TLB? */
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid == 1 && tlb[i].page == page) {
            tlb[i].frame = frame;
            return;
        }
    }

    /* Passo 2: slot inválido disponível? */
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid == 0) {
            tlb[i].page  = page;
            tlb[i].frame = frame;
            tlb[i].valid = 1;
            return;
        }
    }

    /* Passo 3: TLB cheia — substituição FIFO */
    tlb[fifo_next].page  = page;
    tlb[fifo_next].frame = frame;
    tlb[fifo_next].valid = 1;
    fifo_next = (fifo_next + 1) % TLB_SIZE;
}

/* ---------------------------------------------------------------
 * tlb_remove
 * Invalida a entrada com .page == page.
 * --------------------------------------------------------------- */
void tlb_remove(int page) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid == 1 && tlb[i].page == page) {
            tlb[i].valid = 0;
            tlb[i].page  = -1;
            tlb[i].frame = -1;
            return;
        }
    }
}

/* ---------------------------------------------------------------
 * tlb_clear
 * Invalida todas as entradas (equivale a um tlb_init sem resetar
 * o fifo_next — útil para flush completo).
 * --------------------------------------------------------------- */
void tlb_clear(void) {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = 0;
        tlb[i].page  = -1;
        tlb[i].frame = -1;
    }
}
