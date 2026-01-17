# Calculates up to the 47th fibonacci number, storing in memory
.data
arr: .space 188

.text
.globl main
main:
la $s7 arr

li $s0 0  # f(n-2)
sw $s0 0($s7)

li $s1 1  # f(n-1)
sw $s1 4($s7)

li $s2 2  # n
li $s3 47 # limit

Loop:
    bgt $s2 $s3 EndLoop # loop until s2 > 47

    # Calculate
    addu $t0 $s0 $s1 # f(n) = f(n-2) + f(n-1)

    # Store in memory
    sll $t1 $s2 2    # offset
    addu $t1 $t1 $s7 # address
    sw $t0 0($t1)

    # Move
    move $s0 $s1     # f(n-2) = f(n-1)
    move $s1 $t0     # f(n-1) = f(n)

    addiu $s2 $s2 1
    j Loop

EndLoop:
li $v0 22
move $a0 $s1
syscall

li $v0 10
syscall