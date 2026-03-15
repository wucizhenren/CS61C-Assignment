.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:

    # Error checks
    li t0,1
    blt a1,t0,fuck1
    blt a2,t0,fuck1
    blt a4,t0,fuck2
    blt a5,t0,fuck2
    bne a2,a4,fuck3


    # Prologue
    #s0 outer index
    #s1 inner index
    addi sp,sp,-12
    sw s0,0(sp)
    sw s1,4(sp)
    sw ra,8(sp)
    mv s0,x0
    

outer_loop_start:
    beq s0,a1,done
    mv s1,x0
inner_loop_start:
    beq s1,a5,outer_loop_end
   
    addi sp,sp,-28
    sw a0,0(sp)
    sw a1,4(sp)
    sw a2,8(sp)
    sw a3,12(sp)
    sw a4,16(sp)
    sw a5,20(sp)
    sw a6,24(sp)
    #t0 to v0
    #t1 to v1
    mul t0,a2,s0
    slli t0,t0,2
    add t0,t0,a0
    add t1,x0,s1
    slli t1,t1,2
    add t1,t1,a3
   
    mv a2,a4
    addi a3,x0,1
    mv a0,t0
    mv a1,t1
    mv a4,a5
    jal dot
    #t1 save return value
    mv t1,a0
    lw a0,0(sp)
    lw a1,4(sp)
    lw a2,8(sp)
    lw a3,12(sp)
    lw a4,16(sp)
    lw a5,20(sp)
    lw a6,24(sp)
    addi sp,sp,28
 #t0 as the pointer to c[i][j]
    mul t0,a5,s0
    add t0,t0,s1
    slli t0,t0,2
    add t0,t0,a6
    sw  t1,0(t0)

inner_loop_end:
    addi s1,s1,1
    jal x0,inner_loop_start

outer_loop_end:
    addi s0,s0,1
    jal x0,outer_loop_start

    # Epilogue
done:
    lw s0,0(sp)
    lw s1,4(sp)
    lw ra,8(sp)
    addi sp,sp,12
    ret

fuck1:
    li a1,72
    jal exit2

fuck2:
    li a1,73
    jal exit2

fuck3:
    li a1,74
    jal exit2