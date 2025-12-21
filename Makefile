CC=gcc
IDIR=include
CFLAGS=-Wall -Wextra -I$(IDIR) -g

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

all: mips_assembler

mips_assembler: $(OBJ)
	$(CC) $(OBJ) -o mips_assembler

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f src/*.o