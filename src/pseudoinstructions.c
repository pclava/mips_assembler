#include "pseudoinstructions.h"
#include "utils.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/* Pseudoinstructions

This handles the conversion of pseudoinstructions to their real equivalents.
The functions below take the pseudoinstruction in the form of an Instruction structure
and add its real equivalents directly to the InstructionList
*/

#define PSEUDOINSTRUCTIONS_COUNT 7
const char *PSEUDOINSTRUCTIONS[PSEUDOINSTRUCTIONS_COUNT] = {
    "blt", "bgt", "ble", "bge", "li", "la", "move"
};

// Take an Instruction struct and write the 1 or 2 translated instructions to the InstructionList
// Return number of instructions successfully written on success, otherwise 0
int (*PSEUDOINSTRUCTIONS_FUNCTS[PSEUDOINSTRUCTIONS_COUNT])(Instruction, InstructionList *) = {
    &blt, &bgt, &ble, &bge, &li, &la, &move
};

// Find the corresponding function for the pseudoinstruction. Returns the number of instructions written, or -1 on failure
int process_pseudo(const Instruction instruction, InstructionList *instruction_list) {

    // find corresponding pseudoinstruction and call that function
    for (int i = 0; i < PSEUDOINSTRUCTIONS_COUNT; i++) {
        if (strcmp(instruction.mnemonic, PSEUDOINSTRUCTIONS[i]) == 0) {
            const int s = PSEUDOINSTRUCTIONS_FUNCTS[i](instruction, instruction_list);
            if (s == 0) return -1;
            return s;
        }
    }

    return 0; // no instructions were written; instr isn't pseudo
}

int blt(const Instruction instruction, InstructionList* instructions) {
    const unsigned char r1 = instruction.registers[0];
    const unsigned char r2 = instruction.registers[1];
    if (instruction.registers[2] != 255 || instruction.imm.type != SYMBOL) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }

    Instruction i1;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));
    const Immediate i1imm = {.type=NONE, .intValue=0, .modifier=0};
    i1.imm = i1imm;
    Instruction i2;
    memset(i2.mnemonic, '\0', sizeof(i2.mnemonic));

    strcpy(i1.mnemonic, "slt");
    i1.registers[0] = 1;
    i1.registers[1] = r1;
    i1.registers[2] = r2;
    i1.line = instruction.line;
    if (add_instruction(instructions, i1) == 0) return 0;

    strcpy(i2.mnemonic, "bne");
    i2.registers[0] = 1;
    i2.registers[1] = 0;
    i2.registers[2] = 255;
    i2.imm = instruction.imm;
    i2.line = instruction.line;
    if (add_instruction(instructions, i2) == 0) return 0;

    return 2;
}

int bgt(const Instruction instruction, InstructionList* instructions) {
    const unsigned char r1 = instruction.registers[0];
    const unsigned char r2 = instruction.registers[1];
    if (instruction.registers[2] != 255 || instruction.imm.type != SYMBOL) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }

    Instruction i1;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));
    const Immediate i1imm = {.type=NONE, .intValue=0, .modifier=0};
    i1.imm = i1imm;
    Instruction i2;
    memset(i2.mnemonic, '\0', sizeof(i2.mnemonic));

    strcpy(i1.mnemonic, "slt");
    i1.registers[0] = 1;
    i1.registers[1] = r2;
    i1.registers[2] = r1;
    i1.line = instruction.line;
    if (add_instruction(instructions, i1) == 0) return 0;

    strcpy(i2.mnemonic, "bne");
    i2.registers[0] = 1;
    i2.registers[1] = 0;
    i2.registers[2] = 255;
    i2.imm = instruction.imm;
    i2.line = instruction.line;
    if (add_instruction(instructions, i2) == 0) return 0;

    return 2;
}

int ble(const Instruction instruction, InstructionList* instructions) {
    const unsigned char r1 = instruction.registers[0];
    const unsigned char r2 = instruction.registers[1];
    if (instruction.registers[2] != 255 || instruction.imm.type != SYMBOL) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }

    Instruction i1;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));
    const Immediate i1imm = {.type=NONE, .intValue=0, .modifier=0};
    i1.imm = i1imm;
    Instruction i2;
    memset(i2.mnemonic, '\0', sizeof(i2.mnemonic));

    strcpy(i1.mnemonic, "slt");
    i1.registers[0] = 1;
    i1.registers[1] = r2;
    i1.registers[2] = r1;
    i1.line = instruction.line;
    if (add_instruction(instructions, i1) == 0) return 0;

    strcpy(i2.mnemonic, "beq");
    i2.registers[0] = 1;
    i2.registers[1] = 0;
    i2.registers[2] = 255;
    i2.imm = instruction.imm;
    i2.line = instruction.line;
    if (add_instruction(instructions, i2) == 0) return 0;

    return 2;
}

int bge(const Instruction instruction, InstructionList* instructions) {
    const unsigned char r1 = instruction.registers[0];
    const unsigned char r2 = instruction.registers[1];
    if (instruction.registers[2] != 255 || instruction.imm.type != SYMBOL) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }

    Instruction i1;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));
    const Immediate i1imm = {.type=NONE, .intValue=0, .modifier=0};
    i1.imm = i1imm;
    Instruction i2;
    memset(i2.mnemonic, '\0', sizeof(i2.mnemonic));

    strcpy(i1.mnemonic, "slt");
    i1.registers[0] = 1;
    i1.registers[1] = r1;
    i1.registers[2] = r2;
    i1.line = instruction.line;
    if (add_instruction(instructions, i1) == 0) return 0;

    strcpy(i2.mnemonic, "beq");
    i2.registers[0] = 1;
    i2.registers[1] = 0;
    i2.registers[2] = 255;
    i2.imm = instruction.imm;
    i2.line = instruction.line;
    if (add_instruction(instructions, i2) == 0) return 0;

    return 2;
}

int li(const Instruction instruction, InstructionList* instructions) {
    // li $R IMM
    if (instruction.registers[1] != 255 || instruction.registers[2] != 255 || instruction.imm.type != NUM) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }
    const unsigned char r1 = instruction.registers[0];
    const Immediate imm = instruction.imm;

    Instruction i1;
    i1.line = instruction.line;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));

    // Determine size
    if (instruction.imm.intValue >= SHRT_MIN && instruction.imm.intValue <= SHRT_MAX) {
        // 16-bit
        // addi $R $0 IMM
        strcpy(i1.mnemonic, "addiu");
        i1.registers[0] = r1;
        i1.registers[1] = 0;
        i1.registers[2] = 255;
        i1.imm = imm;
        if (add_instruction(instructions, i1) == 0) {
            return 0;
        }
        return 1;
    }
    // 32-bit
    const int32_t hi = imm.intValue >> 16;     // Sign extended upper 16-bits
    const int32_t lo = imm.intValue & 0x0000FFFF;  // Low 16-bits

    const Immediate hiImm = {NUM, .intValue = hi};
    const Immediate loImm = {NUM, .intValue = lo};

    // lui $1 %hi(IMM)
    strcpy(i1.mnemonic, "lui");
    i1.registers[0] = 1;
    i1.registers[1] = 255;
    i1.registers[2] = 255;
    i1.imm = hiImm;

    // ori $R $1 %lo(IMM)
    Instruction i2;
    memset(i2.mnemonic, '\0', sizeof(i2.mnemonic));
    strcpy(i2.mnemonic, "ori");
    i2.registers[0] = r1;
    i2.registers[1] = 1;
    i2.registers[2] = 255;
    i2.line = instruction.line;
    i2.imm = loImm;

    if (add_instruction(instructions, i1) == 0 || add_instruction(instructions, i2) == 0) {
        return 0;
    }
    return 2;
}

int la(const Instruction instruction, InstructionList* instructions) {
    // la $R LABEL
    // convert to SYMBOLIC IMMEDIATE

    const unsigned char r1 = instruction.registers[0];
    const Immediate imm = instruction.imm;
    if (instruction.registers[1] != 255 || instruction.registers[2] != 255 || imm.type != SYMBOL) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }

    // lui $at %hi(label)
    Instruction i1;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));
    strcpy(i1.mnemonic, "lui");
    i1.registers[0] = 1;
    i1.registers[1] = 255;
    i1.registers[2] = 255;
    i1.line = instruction.line;

    // Take the low bits of the immediate
    Immediate hiImm;
    hiImm.type = SYMBOL;
    strcpy(hiImm.symbol, instruction.imm.symbol);
    hiImm.modifier = 1;
    i1.imm = hiImm;

    // ori $R $at %lo(label)
    Instruction i2;
    memset(i2.mnemonic, '\0', sizeof(i2.mnemonic));
    strcpy(i2.mnemonic, "ori");
    i2.registers[0] = r1;
    i2.registers[1] = 1;
    i2.registers[2] = 255;
    i2.line = instruction.line;

    // Take the lo bits of the immediate
    Immediate loImm;
    loImm.type = SYMBOL;
    strcpy(loImm.symbol, instruction.imm.symbol);
    loImm.modifier = 2;

    i2.imm = loImm;

    if (add_instruction(instructions, i1) == 0 || add_instruction(instructions, i2) == 0) {
        return 0;
    }
    return 2;
}

int move(const Instruction instruction, InstructionList* instructions) {
    // move $R1 $R2
    const unsigned char r1 = instruction.registers[0];
    const unsigned char r2 = instruction.registers[1];

    if (instruction.registers[2] != 255 && instruction.imm.type != NONE) {
        raise_error(ARGS_INV, NULL, __FILE__);
        return 0;
    }

    // addu $R1 $0 $R2
    Instruction i1;
    memset(i1.mnemonic, '\0', sizeof(i1.mnemonic));
    strcpy(i1.mnemonic, "addu");
    i1.registers[0] = r1;
    i1.registers[1] = 0;
    i1.registers[2] = r2;
    i1.imm = instruction.imm;
    i1.line = instruction.line;

    if (add_instruction(instructions, i1) == 0) {
        return 0;
    }

    return 1;
}
