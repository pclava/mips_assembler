#include "linker.h"
#include "utils.h"
#include <stdlib.h>

void file_destroy(const SourceFile *file) {
    free(file->relocation_table);
    free(file->symbol_table);
}

int file_init(SourceFile *file, uint32_t text_offset, uint32_t data_offset, uint32_t text_size, uint32_t data_size) {
    file->text_offset = text_offset;
    file->data_offset = data_offset;
    file->text_size = text_size;
    file->data_size = data_size;
    uint32_t *text = malloc(text_size);
    if (text == NULL) {
        goto _init_failure;
    }
    file->text = text;
    uint8_t *data = malloc(data_size);
    if (data == NULL) {
        free(text);
        goto _init_failure;
    }
    file->data = data;

    RelocationTable *table = malloc(sizeof(RelocationTable));
    if (table == NULL) {
        free(text);
        free(data);
        goto _init_failure;
    }
    rt_init(table);
    file->relocation_table = table;

    SymbolTable *symbol_table = malloc(sizeof(SymbolTable));
    if (symbol_table == NULL) {
        free(text);
        free(data);
        free(table);
        goto _init_failure;
    }
    st_init(symbol_table);
    file->symbol_table = symbol_table;

    return 1;

    _init_failure:
    raise_error(MEM, NULL, __FILE__);
    return 0;

}

// Returns final memory address of a symbol, with the formula *section base + object file offset + symbol offset*
uint32_t get_final_address(const Symbol symbol, const uint32_t object_offset) {
    switch (symbol.segment) {
        case TEXT:
            return TEXT_START + object_offset + symbol.offset;
        case DATA:
            return DATA_START + object_offset + symbol.offset;
        default:
            return 0;
    }
}

int relocate(SourceFile file, RelocationEntry entry, uint32_t final_address) {
    if (entry.segment == UNDEF) return 0;
    const uint32_t instr_addr = TEXT_START + file.text_offset + entry.target_offset;
    switch (entry.reloc_type) {
        case R_32:
            // Check segment
            if (entry.segment != DATA) {
                fprintf(stderr, "Error linking %s: attempted R_32 relocation outside data segment\n", file.name);
                return 0;
            }
            // Replace bytes little-endian
            file.data[entry.target_offset] = final_address & 0xFF;
            file.data[entry.target_offset+1] = final_address >> 8 & 0xFF;
            file.data[entry.target_offset+2] = final_address >> 16 & 0xFF;
            file.data[entry.target_offset+3] = final_address >> 24 & 0xFF;
            return 1;
        case R_26:
            // Check segment
            if (entry.segment != TEXT) {
                fprintf(stderr, "Error linking %s: attempted R_26 relocation outside text segment\n", file.name);
                return 0;
            }
            // Check range (compare MSBs of instruction's real address and final address)
            if ((instr_addr & 0xF0000000) != (final_address & 0xF0000000)) {
                fprintf(stderr, "Error linking %s: jump target out of range\n", file.name);
                return 0;
            }
            // Replace 26 lower bits
            file.text[entry.target_offset] |= (final_address & 0x0FFFFFFFF) >> 2;
            return 1;
        case R_PC16:
            // Check segment
            if (entry.segment != TEXT) {
                fprintf(stderr, "Error linking %s: attempted R_PC16 relocation outside text segment\n", file.name);
                return 0;
            }
            // Check range (within 2^15 instructions)
            const int32_t dist = ((int32_t) instr_addr + 4 - (int32_t) final_address)/4;
            if (dist < INT16_MIN || dist > INT16_MAX) {
                fprintf(stderr, "Error linking %s: branch target out of range\n", file.name);
                return 0;
            }
            file.text[entry.target_offset] |= (int16_t) dist;
            return 1;
        case R_HI16:
            if (entry.segment != TEXT) {
                fprintf(stderr, "Error linking %s: attempted R_HI16 relocation outside text segment\n", file.name);
                return 0;
            }
            file.text[entry.target_offset] |= final_address >> 16;
            return 1;
        case R_LO16:
            if (entry.segment != TEXT) {
                fprintf(stderr, "Error linking %s: attempted R_LO16 relocation outside text segment\n", file.name);
                return 0;
            }
            file.text[entry.target_offset] |= final_address & 0x0000FFFF;
            return 1;
        default:
            fprintf(stderr, "Error linking %s: unrecognized relocation directive\n", file.name);
            return 0;
    }
}

int link(const char *out_path, char *object_files[], int file_count) {
    SourceFile source_files[file_count];

    SymbolTable global_symbols;
    st_init(&global_symbols);

    struct FileHeader final_header;
    final_header.text_size = 0;
    final_header.data_size = 0;

    /*
    For each file:
       - load its text segment
       - load its data segment
       - load its relocation table
       - load its symbol table and add global defined symbols to global symbol table
    */
    for (int file_index = 0; file_index < file_count; file_index++) {
        // Open file
        FILE *f = fopen(object_files[file_index], "rb");
        if (f == NULL) {
            raise_error(FILE_IO, object_files[file_index], __FILE__);
            return 0;
        }
        struct FileHeader header;
        fread(&header, sizeof(struct FileHeader), 1, f);
        uint32_t text_offset, data_offset;
        if (file_index == 0) {
            text_offset = 0;
            data_offset = 0;
        } else {
            text_offset = source_files[file_index - 1].text_size;
            data_offset = source_files[file_index - 1].data_size;
        }
        final_header.text_size += header.text_size;
        final_header.data_size += header.data_size;

        // Initialize file structure
        SourceFile file;
        file_init(&file, text_offset, data_offset, header.text_size, header.data_size);

        // Read text
        for (uint32_t i = 0; i < header.text_size/4; i++) {
            file.text[i] = read_word(f);
        }

        // Read data
        for (uint32_t i = 0; i < header.data_size; i++) {
            file.data[i] = read_byte(f);
        }

        // Read relocation table
        const uint32_t reloc_size = read_word(f);
        for (uint32_t i = 0; i < reloc_size; i++) {
            RelocationEntry entry;
            fread(&entry, sizeof(RelocationEntry), 1, f);
            rt_add(file.relocation_table, entry);
        }

        // Read symbol table
        const uint32_t symbol_table_size = read_word(f);
        for (uint32_t i = 0; i < symbol_table_size; i++) {
            Symbol symbol;
            fread(&symbol, sizeof(Symbol), 1, f);
            st_add_struct(file.symbol_table, symbol);
        }

        // Add global symbols to symbol table
        for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
            SymbolBucket bucket = file.symbol_table->buckets[i];
            if (bucket.inUse) {
                if (bucket.item.binding == GLOBAL && bucket.item.segment != UNDEF) {
                    // get final address
                    uint32_t final_address = 0;
                    if (bucket.item.segment == TEXT) {
                        final_address = get_final_address(bucket.item, text_offset);
                    } else if (bucket.item.segment == DATA) {
                        final_address = get_final_address(bucket.item, data_offset);
                    }
                    // add to global symbols
                    st_add_symbol(&global_symbols, bucket.item.name, final_address, bucket.item.segment, GLOBAL);
                }
            }
        }

        source_files[file_index] = file;
        fclose(f);
    }

    /*
    In each relocation table,
    resolve each relocation
    */
    for (int file_index = 0; file_index < file_count; file_index++) {
        SourceFile source_file = source_files[file_index];
        RelocationTable *reloc_table = source_file.relocation_table;
        uint32_t text_offset = source_file.text_offset;
        uint32_t data_offset = source_file.data_offset;
        for (size_t i = 0; i < reloc_table->len; i++) {
            RelocationEntry entry = reloc_table->list[i];
            Symbol *dependency = st_get_symbol(source_file.symbol_table, entry.dependency);

            // Get correct final address for symbol
            uint32_t final_address = 0;
            switch (entry.segment) {
                case TEXT:
                    final_address = get_final_address(*dependency, text_offset);
                    break;
                case DATA:
                    final_address = get_final_address(*dependency, data_offset);
                    break;
                case UNDEF:
                    if (dependency->binding != GLOBAL) {
                        fprintf(stderr, "Error linking %s: symbol undefined\n", source_file.name);
                        return 0; // TODO proper exit
                    }
                    dependency = st_get_symbol(&global_symbols, entry.dependency);
                    if (dependency == NULL) return 0;
                    final_address = dependency->offset;
                    break;
                default:
                    return 0;
            }

            // Resolve relocation
            if (relocate(source_file, entry, final_address) == 0) return 0;
        }
    }

    /*
    Build file by combining text and data segments
    */
    FILE *out = fopen(out_path, "wb");
    if (out == NULL) {
        raise_error(FILE_IO, out_path, __FILE__);
        return 0;
    }
    fwrite(&final_header, sizeof(struct FileHeader), 1, out);
    for (int file_index = 0; file_index < file_count; file_index++) {
        // Write text segment
        fwrite(source_files[file_index].text, sizeof(uint32_t), source_files[file_index].text_size/4, out);
    }
    for (int file_index = 0; file_index < file_count; file_index++) {
        // Write data segment
        fwrite(source_files[file_index].data, sizeof(uint8_t), source_files[file_index].data_size, out);
    }
    fclose(out);

    for (int file_index = 0; file_index < file_count; file_index++) {
        file_destroy(&source_files[file_index]);
    }

    return 1;
}
