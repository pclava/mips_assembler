# Functions for fibonacci.asm

.text
.globl print_num
print_num:
	li $v0 1
	syscall
	jr $ra

# int get_item(base, offset, array address)
.globl get_item
get_item:
	add $t1 $a0 $a1
	sll $t1 $t1 2
	add $t2 $a2 $t1
	lw $v0 0($t2)
	jr $ra