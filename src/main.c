#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "tlb.h"
#include "page_table.h"
#include "memory.h"
#include "statistics.h"

int main(void)
{
    FILE *backing = fopen(BACKING_STORE_PATH, "rb");

    if (backing == NULL) {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", BACKING_STORE_PATH);
        fprintf(stderr, "Execute antes: cd data && python3 generate_data.py\n");
        return 1;
    }

    page_table_init();
    tlb_init();
    memory_init(backing);
    statistics_init();

    int logical_address;

    while (scanf("%d", &logical_address) == 1) {
        count_address();

        /* Mascara os 16 bits menos significativos.
         * Bits [15:8] = número da página, bits [7:0] = offset. */
        logical_address &= 0xFFFF;
        int page   = (logical_address >> 8) & 0xFF;
        int offset =  logical_address       & 0xFF;

        /* Consulta TLB primeiro */
        int frame = tlb_lookup(page);

        if (frame != -1) {
            count_tlb_hit();
        } else {
            /* TLB miss: consulta tabela de páginas */
            frame = page_table_lookup(page);

            if (frame == -1) {
                /* Page fault: carrega do backing store */
                count_page_fault();
                frame = handle_page_fault(page);
            }

            tlb_insert(page, frame);
        }

        /* Atualiza LRU aproximado (aging) a cada acesso */
        page_table_set_reference(page);
        page_table_update_aging();

        /* Endereço físico = frame * tamanho_do_quadro + offset */
        int physical_address = frame * PAGE_SIZE + offset;
        signed char value    = read_memory(frame, offset);

        printf("Logical address: %d Physical address: %d Value: %d\n",
               logical_address,
               physical_address,
               value);
    }

    print_statistics();

    fclose(backing);

    return 0;
}
