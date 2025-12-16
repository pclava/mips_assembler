#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "preprocess.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Error in %s: expected two arguments: source path and destination path\n", __FILE__);
        return 1;
    }
    const char *inp_path = argv[1];
    const char *out_path = argv[2];
    FILE *inp_file = open_file(argv[1]);
    if (inp_file == NULL) return 1;

    ERROR_HANDLER.file = __FILE__;

    Text text;
    text_init(&text);

    if (preprocess(inp_file, inp_path, &text) == 0) {
        fprintf(stderr, "Error in %s: could not preprocess\n", __FILE__);
        text_destroy(&text);
        return 2;
    }

    // text_debug(&text);

    if (assemble(&text, out_path) == 0) {
        fprintf(stderr, "Error in %s: could not assemble\n", __FILE__);
        text_destroy(&text);
        return 3;
    }

    text_destroy(&text);

    return 0;
}