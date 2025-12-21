.data
.text
.globl main
main:

# func(str, num)
la $a0 str
la $t0 num
lw $a1 0($t0)
jal func
j exit

exit:
li $v0 10
syscall