#ifndef MIPS_ASSEMBLER_TEXT_H
#define MIPS_ASSEMBLER_TEXT_H

/* === TYPES === */

typedef struct {
    unsigned int len;
    unsigned int cap;

    char *text;
    unsigned int number;
    char *filename;
} Line;

typedef struct {
    Line *items;
    unsigned int len;
    unsigned int cap;
} Text;

/* === METHODS === */

int line_init(Line *line, const char *fileName);

int line_add_char(Line *line, char c);

void line_destroy(Line *line);

int text_init(Text *text);

int text_add_line(Text *text, Line line);

void text_destroy(const Text *text);

void text_debug(const Text *text);

#endif //MIPS_ASSEMBLER_TEXT_H