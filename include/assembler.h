#ifndef MIPS_ASSEMBLER_ASSEMBLER_H
#define MIPS_ASSEMBLER_ASSEMBLER_H

#include "symbol_table.h"
#include "instruction_parser.h"
#include "instructions.h"
#include "data_parser.h"

/* === TYPES === */

typedef struct {
    const Text *preprocessed;
    DataList *data_list;
    InstructionList *instruction_list;
    SymbolTable *symbol_table;
    InstructionTable *instruction_table;
} Assembler;

/* === ASSEMBLER STRUCTURE METHODS === */

int assembler_init(Assembler *assembler, const Text *preprocessed);

void assembler_destroy(Assembler *assembler);

void assembler_debug(const Assembler *assembler);

/* === ASSEMBLY METHODS === */

int assembler_first_pass(Assembler *assembler);

int assembler_second_pass(Assembler *assembler, const char *output);

int assemble(const Text *preprocessed, const char *output);

#endif //MIPS_ASSEMBLER_ASSEMBLER_H