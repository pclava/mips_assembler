# Optionally inserted by the linker at the end of the program
# Calls "main"

.text

.globl __start
__start:
jal main
jal __exit

__exit:
li $v0 10
syscall