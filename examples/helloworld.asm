# Print the string a given number of times

.data
str: .asciiz "Hello, World!\n"
num: .word 245

.text
la $t0 num
lw $s0 0($t0)

la $a0 str
li $v0 4
li $t0 0
Loop:
	bge $t0 $s0 EndLoop

	syscall
	addi $t0 $t0 1

	j Loop
EndLoop:

li $v0 10
syscall