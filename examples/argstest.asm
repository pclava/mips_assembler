# Prints the strings passed as arguments
.text
.macro printstr %addr
        move    $4,%addr
        li      $2,4
        syscall
.end_macro

# int main(int argc, char *argv[])
.globl main
main:
        addiu   $sp,$sp,-40  # Adjust stack
        sw      $31,36($sp)  # $ra
        sw      $fp,32($sp)  # $fp
        move    $fp,$sp      # Load new frame pointer
        sw      $4,40($fp)   # Write $a0 to stack
        sw      $5,44($fp)   # Write $a1 to stack
        sw      $0,24($fp)   # Begin loop (set int i = 0)
        b       $L2          # Jump to condition

$L3:
        lw      $2,24($fp)   # Retrieve i
        sll     $2,$2,2      # Multiply by four for offset
        lw      $3,44($fp)   # Retrieve $a1 (base address)
        addu    $2,$3,$2     # Add offset to base
        lw      $2,0($2)     # Get pointer to string
        printstr $2         # Print string
        lw      $2,24($fp)   # Retrieve original i
        addiu   $2,$2,1      # Increment by 1
        sw      $2,24($fp)   # Update i
$L2:
        lw      $3,24($fp)   # Retrieve i
        lw      $2,40($fp)   # Retrieve $a0
        blt     $3,$2,$L3    # Check condition and jump

        li      $2,1         # Prepare return value (1)
        move    $sp,$fp      # Recover stack pointer
        lw      $31,36($sp)  # Restore $ra
        lw      $fp,32($sp)  # Restore $fp
        addiu   $sp,$sp,40   # Restore stack
        jr      $31          # Jump back