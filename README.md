# Trabalho Prático 2 — Gerenciador de Memória Virtual

**Sistemas Operacionais — PUC Minas**

## Integrantes

- Davi (Mennz)
- Matheus Moreira
- Nunes
- Cauã

## Descrição

Simulador de gerência de memória virtual em C que traduz endereços lógicos
para endereços físicos utilizando TLB (16 entradas, política FIFO) e tabela
de páginas (256 entradas), com paginação por demanda e substituição de
páginas por LRU aproximado (aging).

## Estrutura

```
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
```

## Como executar

### 1. Gerar os arquivos de entrada

Entre no diretório `data` e execute o script Python:

```
cd data
python3 generate_data.py
cd ..
```

Isso criará:

- `BACKING_STORE.bin`
- `addresses_random.txt`
- `addresses_location.txt`

### 2. Compilar

**Linux / macOS / WSL:**

```
make
```

**Windows (PowerShell), caso `make` não esteja disponível:**

```
gcc -Wall -Wextra -O2 -std=c11 -Iinclude src/main.c src/memory.c src/page_table.c src/statistics.c src/tlb.c -o vm.exe
```

### 3. Executar

**Linux / macOS / WSL:**

```
./vm < data/addresses_random.txt
./vm < data/addresses_location.txt
```

**Windows (PowerShell):**

```
Get-Content data\addresses_random.txt | .\vm.exe
Get-Content data\addresses_location.txt | .\vm.exe
```

## Resultados obtidos

| Métrica | addresses_random.txt | addresses_location.txt |
|---|---|---|
| Endereços traduzidos | 10.000 | 10.000 |
| Page Faults | 4.998 | 1.031 |
| Page Fault Rate | 0.500 | 0.103 |
| TLB Hits | 627 | 8.021 |
| TLB Hit Rate | 0.063 | 0.802 |

## Divisão de trabalho

| Módulo | Responsável |
|---|---|
| `src/main.c`, `src/statistics.c` | Davi |
| `src/tlb.c`, `include/tlb.h` | Matheus |
| `src/page_table.c`, `include/page_table.h` | Nunes |
| `src/memory.c`, `include/memory.h` | Cauã |
| Relatório técnico | Davi |

## Uso de IA

Foi utilizada a ferramenta Claude (Anthropic) durante o desenvolvimento,
seguindo a metodologia Spec-Driven Development (SDD). Os prompts utilizados
por cada integrante estão documentados na Seção 7 do relatório técnico
(`report/Relatorio.pdf`).
