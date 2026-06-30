Trabalho Prático 2 — Gerenciador de Memória Virtual

Sistemas Operacionais — PUC Minas

Integrantes


Davi Mendoza Rezende Couto
Matheus Henrique Moreira
Pedro Nunes Cruz
Cauã Moreira


Descrição

Simulador de gerência de memória virtual em C que traduz endereços lógicos
para endereços físicos utilizando TLB (16 entradas, política FIFO) e tabela
de páginas (256 entradas), com paginação por demanda e substituição de
páginas por LRU aproximado (aging).

Estrutura

vm_manager/
├── Makefile
├── README.md
├── include/
│   ├── config.h
│   ├── tlb.h
│   ├── page_table.h
│   ├── memory.h
│   └── statistics.h
├── src/
│   ├── main.c
│   ├── tlb.c
│   ├── page_table.c
│   ├── memory.c
│   └── statistics.c
├── data/
│   └── generate_data.py
└── report/
    └── relatorio_tp2.pdf

Como executar

1. Gerar os arquivos de entrada

Entre no diretório data e execute o script Python:

cd data
python3 generate_data.py
cd ..

Isso criará:


BACKING_STORE.bin
addresses_random.txt
addresses_location.txt


2. Compilar

Linux / macOS / WSL:

make

Windows (PowerShell), caso make não esteja disponível:

gcc -Wall -Wextra -O2 -std=c11 -Iinclude src/main.c src/memory.c src/page_table.c src/statistics.c src/tlb.c -o vm.exe

3. Executar

Linux / macOS / WSL:

./vm < data/addresses_random.txt
./vm < data/addresses_location.txt

Windows (PowerShell):

Get-Content data\addresses_random.txt | .\vm.exe
Get-Content data\addresses_location.txt | .\vm.exe

Resultados obtidos

Métricaaddresses_random.txtaddresses_location.txtEndereços traduzidos10.00010.000Page Faults4.9981.031Page Fault Rate0.5000.103TLB Hits6278.021TLB Hit Rate0.0630.802

Divisão de trabalho

MóduloResponsávelsrc/main.c, src/statistics.cDavisrc/tlb.c, include/tlb.hMatheussrc/page_table.c, include/page_table.hNunessrc/memory.c, include/memory.hCauãRelatório técnicoDavi

Uso de IA

Foi utilizada a ferramenta Claude (Anthropic) durante o desenvolvimento,
seguindo a metodologia Spec-Driven Development (SDD). Os prompts utilizados
por cada integrante estão documentados na Seção 7 do relatório técnico
(report/relatorio_tp2.pdf).