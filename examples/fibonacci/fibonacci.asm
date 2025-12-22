# Prints out the first "count" fibonacci numbers, saving them in "arr"

.data
count: .word 16
arr: .word 0 1

.text
.globl main
main:
li $a0 0
jal print_num

li $a0 1
jal print_num

la $s3 count
lw $s0 0($s3)

la $s2 arr

li $s3 2
Loop:
	bge $s3 $s0 EndLoop

	move $a0 $s3
	li $a1 -1
	move $a2 $s2
	jal get_item
	move $t3 $v0

	li $a1 -2
	jal get_item
	move $t4 $v0

	add $t5 $t3 $t4

	sll $t1 $s3 2
	add $t2 $s2 $t1
	sw $t5 0($t2)

	move $a0 $t5
	jal print_num

	addi $s3 $s3 1
	j Loop

EndLoop:

li $v0 10
syscall