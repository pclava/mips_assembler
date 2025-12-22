# Optionally inserted by the linker at the end of the program
# Calls "main"

.text

.globl _start
_start:
jal main
jal _exit

_exit:
li $v0 10
syscall