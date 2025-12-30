# Optionally inserted by the linker at the end of the program
# Calls "main"

.text

.globl __start
__start:
# main(argc, argv)
lw $a0 0($sp)   # argc
addiu $a1 $sp 4 # argv
jal main
jal __exit

__exit:
li $v0 10
syscall