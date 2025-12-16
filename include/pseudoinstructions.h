#ifndef MIPS_ASSEMBLER_PSEUDOINSTRUCTIONS_H
#define MIPS_ASSEMBLER_PSEUDOINSTRUCTIONS_H
#include "instruction_parser.h"

int blt(Instruction, InstructionList*);
int bgt(Instruction, InstructionList*);
int ble(Instruction, InstructionList*);
int bge(Instruction, InstructionList*);
int li(Instruction, InstructionList*);
int la(Instruction, InstructionList*);
int move(Instruction, InstructionList*);

extern const size_t PSEUDOINSTRUCTIONS_COUNT;
extern const char *PSEUDOINSTRUCTIONS[7];

extern int (*PSEUDOINSTRUCTIONS_FUNCTS[7])(Instruction, InstructionList *);

int process_pseudo(Instruction instruction, InstructionList *instruction_list);

#endif //MIPS_ASSEMBLER_PSEUDOINSTRUCTIONS_H