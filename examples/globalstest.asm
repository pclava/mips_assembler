# Defines globals for globalstestmain.asm

.data
.globl str num
str: .asciiz "Hello,\n World!"
num: .word 463

.text

# func(a0, a1) prints the string at a0, a1 times
.globl func
func:
li $v0 4
li $t0 0
func_loop:
    bge $t0 $a1 end_func_loop
    syscall
    addi $t0 $t0 1
    j func_loop
end_func_loop:
jr $ra