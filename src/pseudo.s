.macro li(%reg, %imm)
lui %reg %imm
ori %reg %imm
.end_macro

li($t0, 10)