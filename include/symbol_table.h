#ifndef MIPS_ASSEMBLER_SYMBOL_TABLE_H
#define MIPS_ASSEMBLER_SYMBOL_TABLE_H
#include <stddef.h>
#include <stdint.h>
#include "utils.h"

#define SYMBOL_TABLE_SIZE 256

/* === TYPES === */

typedef struct {
    char name[SYMBOL_SIZE]; // LABELS CAN BE UP TO 31 CHARACTERS
    uint32_t addr;
} Symbol;

typedef struct {
    Symbol item;
    unsigned char inUse;
} SymbolBucket;

typedef struct {
    SymbolBucket *buckets;
    size_t size;
} SymbolTable;

/* === SYMBOL TABLE METHODS === */

int st_init(SymbolTable *table);

int st_add_symbol(SymbolTable *table, const char *name, uint32_t addr);

uint32_t st_get_symbol(const SymbolTable *table, const char *name);

void st_destroy(const SymbolTable *t);

void st_debug(const SymbolTable *t);

#endif //MIPS_ASSEMBLER_SYMBOL_TABLE_H