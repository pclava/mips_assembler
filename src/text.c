#include "text.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"

/* Text

Used by the preprocessor to store the preprocessed input file
Implemented as a dynamic array of Line structures,
itself a dynamic array of characters.

Lines also contain information for error handling (filename and number)
*/

// Initializes and allocates memory
int line_init(Line *line, const char *fileName) {
    line->len = 0;
    line->cap = 64;
    line->number = -1;

    char *text = malloc(line->cap);
    if (text == NULL) {
        general_error(MEM, __FILE__, NULL);
    }
    line->text = text;

    char *fname = malloc(strlen(fileName)+1);
    if (fname == NULL) {
        free(text);
        general_error(MEM, __FILE__, NULL);
        return 0;
    }

    strcpy(fname, fileName);
    line->filename = fname;
    return 1;
}

// Adds a character to the end of the array
int line_add_char(Line * line, const char c) {
    if (line->len >= line->cap) {
        line->cap *= 2;
        char *new = realloc(line->text, line->cap * sizeof(char));
        if (new == NULL) {
            general_error(MEM, __FILE__, NULL);
            return 0;
        }
        line->text = new;
    }
    line->text[line->len] = c;
    line->len++;
    return 1;
}

// Frees resources (text and filename)
void line_destroy(Line *line) {
    if (line->filename != NULL) {
        free(line->filename);
    }
    if (line->text != NULL) {
        free(line->text);
    }
}

// Initializes and allocates memory
int text_init(Text *text) {
    text->len = 0;
    text->cap = 64;
    Line *lines = malloc(text->cap * sizeof(Line));
    if (lines == NULL) {
        general_error(MEM, __FILE__, NULL);
        return 0;
    }
    text->items = lines;
    return 1;
}

// Adds a Line to the end of the array
int text_add_line(Text * text, const Line line) {
    if (text->len >= text->cap) {
        text->cap *= 2;
        Line *new = realloc(text->items, text->cap * sizeof(Line));
        if (new == NULL) {
            general_error(MEM, __FILE__, NULL);
            return 0;
        }
        text->items = new;
    }

    text->items[text->len] = line;
    text->len++;
    return 1;
}

// Frees resources (internal array and all Line resources)
void text_destroy(const Text *text) {
    if (text->items != NULL) {
        for (size_t i = 0; i < text->len; i++) {
            if (text->items[i].text != NULL) {
                line_destroy(&text->items[i]);
            }
        }
        free(text->items);
    }
}

void text_debug(const Text * text) {
    for (size_t i = 0; i < text->len; i++) {
        printf("%s\n", text->items[i].text);
    }
}