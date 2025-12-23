#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "preprocess.h"
#include "linker.h"

/*
 $ ./mips_assembler a.out src1 src2 [...src_i]            # assemble and link, linking _start.o and beginning execution there
 $ ./mips_assembler -c src1 src2 [...src_i]               # only assemble into object files
 $ ./mips_assembler -e. a.out src1 src2 [...src_i]        # -e. begins execution at the first instruction
 $ ./mips_assembler -e symbol a.out src1 src2 [...src_i]  # -e (arg) begins execution at arg
 */

int main(int argc, char *argv[]) {
    int performLinking = 1;
    int clean = 1;
    if (argc < 3) {
        fprintf(stderr, "error in %s: invalid arguments\n", __FILE__);
        return 1;
    }

    char *entry = "_start"; // symbol that execution should begin at; if null, begins at TEXT_START (0x00400000)
    const char *out_path = argv[1];
    int file_count = argc-2;

    // Handle options, determine entry and outpath
    if (argv[1][0] == '-') {
        switch (argv[1][1]) {
            case 'c':
                performLinking = 0;
                out_path = NULL;
                break;
            case 'e':
                if (argv[1][2] == '.') {
                    file_count = argc-3;
                    entry = NULL;
                    out_path = argv[2];
                }
                else {
                    if (argc < 4) {
                        fprintf(stderr, "error in %s: invalid arguments\n", __FILE__);
                        return 1;
                    }
                    file_count = argc-4;
                    entry = argv[2];
                    out_path = argv[3];
                }
                break;
            default:
                fprintf(stderr, "error in %s: unrecognized option %c\n", __FILE__, argv[1][1]);
                return 1;
        }
    }

    char *object_files[file_count];

    // Assemble each source file
    for (int i = argc-file_count; i < argc; i++) {
        char *inp_path = argv[i];

        // Strip suffix from input path and add .o
        char *object_path = malloc(strlen(inp_path)+3);
        size_t j;
        for (j = 0; j < strlen(inp_path); j++) {
            if (inp_path[j] == '.' || inp_path[j] == '0') break;
            object_path[j] = inp_path[j];
        }
        object_path[j++] = '.';
        object_path[j++] = 'o';
        object_path[j] = '\0';
        object_files[i-(argc-file_count)] = object_path;

        FILE *inp_file = open_file(inp_path);
        if (inp_file == NULL) return 1;

        Text text;
        text_init(&text);

        if (preprocess(inp_file, inp_path, &text) == 0) {
            fprintf(stderr, "Error in %s: could not preprocess file \"%s\"\n", __FILE__, inp_path);
            text_destroy(&text);
            for (int k = 0; k <= i-2; k++) {
                free(object_files[k]);
            }
            return 2;
        }

        // text_debug(&text);

        if (assemble(&text, object_path) == 0) {
            fprintf(stderr, "Error in %s: could not assemble file \"%s\"\n", __FILE__, inp_path);
            text_destroy(&text);
            for (int k = 0; k <= i-2; k++) {
                free(object_files[k]);
            }
            return 3;
        }

        // debug_binary(object_path);
        text_destroy(&text);
    }

    if (performLinking) {
        if (link(out_path, object_files, file_count, entry) == 0) {
            fprintf(stderr, "Error in %s: could not link files\n", __FILE__);
            for (int i = 0; i < file_count; i++) {
                free(object_files[i]);
            }
            return 4;
        }
    }

    for (int i = 0; i < file_count; i++) {
        if (performLinking && clean) remove(object_files[i]);
        free(object_files[i]);
    }

    return 0;
}