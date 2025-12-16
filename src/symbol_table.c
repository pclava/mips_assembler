#include "symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* SymbolTable

Uses a hash table to store declared symbols.
Symbols declarations are found and added in the assembler's first pass.
The second pass consults the symbol table when an instruction refers to a symbol.

Note the symbol table supports up to 256 symbols. I might make it dynamic one day.
*/

// Initializes and allocates memory for an empty symbol table
int st_init(SymbolTable *table) {
    table->buckets = malloc(SYMBOL_TABLE_SIZE * sizeof(SymbolBucket));
    if (table->buckets == NULL) {
        raise_error(MEM, NULL, __FILE__);
        return 0;
    }

    table->size = 0;

    // Fill with empty buckets
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        const SymbolBucket bucket = {.inUse = 0};
        table->buckets[i] = bucket;
    }

    return 1;
}

// Adds to symbol table. Does not check if the symbol is valid per MIPS guidelines (i.e., alphanumeric only).
int st_add_symbol(SymbolTable * table, const char *name, const uint32_t addr) {
    if (table->size >= SYMBOL_TABLE_SIZE) {
        raise_error(ST_SIZE_ERR, name, __FILE__);
        return 0;
    }

    Symbol s;
    if (strlen(name) >= SYMBOL_SIZE) {
        raise_error(SYMBOL_INV, name, __FILE__);
        return 0;
    }
    strcpy(s.name, name);
    s.addr = addr;

    unsigned long index = hash_key(s.name, SYMBOL_SIZE);
    while (table->buckets[index].inUse) {
        if (strcmp(table->buckets[index].item.name, name) == 0) {
            raise_error(DUPL_DEF, name, __FILE__);
            return 0;
        }
        index = (index + 1) % SYMBOL_TABLE_SIZE;
    }

    table->buckets[index].inUse = 1;
    table->buckets[index].item = s;
    table->size++;

    return 1;
}

// Looks up the name and returns the address, or 0 on failure
uint32_t st_get_symbol(const SymbolTable *table, const char *name) {
    unsigned long index = hash_key(name, SYMBOL_TABLE_SIZE);
    if (!table->buckets[index].inUse) {
        raise_error(TOKEN_ERR, name, __FILE__);
        return 0;
    }

    int indices_searched = 0;
    while (strcmp(name, table->buckets[index].item.name) != 0) {
        index = (index + 1) % SYMBOL_TABLE_SIZE;
        indices_searched++;
        if (indices_searched >= SYMBOL_TABLE_SIZE) {
            raise_error(ST_SIZE_ERR, name, __FILE__);
            return 0;
        }
    }

    return table->buckets[index].item.addr;
}

// Frees resources
void st_destroy(const SymbolTable *t) {
    free(t->buckets);
}

void st_debug(const SymbolTable *table) {
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        if (table->buckets[i].inUse) {
            printf("%s: 0x%.8x\n", table->buckets[i].item.name, table->buckets[i].item.addr);
        }
    }
}