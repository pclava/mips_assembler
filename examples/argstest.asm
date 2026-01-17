.text
.macro test %a %b %c
addu %a %b %c
.end_macro
.globl main
main:
test $s1 $s2 $s3

bgt $s1 $s2 main

li $v0 10
syscall