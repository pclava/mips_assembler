#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "preprocess.h"

/*
 * ./mips_assembler a.out src1 src2 [...src_i]
 * ./mips_assembler -c src1 src2 [...src_i]
 *
 * First argument: either the path to the final executable, -c to assemble without linking, or -h for help
 */

int main(int argc, char *argv[]) {
    int performLinking = 1;
    const char *out_path = NULL;

    if (argc < 2) {
        fprintf(stderr, "error in %s: invalid arguments\n", __FILE__);
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        performLinking = 0;
    } else if (strcmp(argv[1], "-h") == 0) {
        printf("not implemented");
        return 0;
    } else {
        out_path = argv[1];
    }

    char *object_files[argc-2];

    // Assemble each source file
    for (int i = 2; i < argc; i++) {
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
        object_files[i-2] = object_path;

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

    for (int i = 0; i < argc-2; i++) {
        free(object_files[i]);
    }

    return 0;
}