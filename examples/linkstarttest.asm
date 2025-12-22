.data
str: .asciiz "Hello World!"
.text

addi $t0 $t1 17

.globl main
main:
li $v0 10
syscall