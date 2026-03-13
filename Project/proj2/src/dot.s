.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:

    # Prologue
    # t0 as index
    # t1 as product
    # t2 as value in v0
    # t3 as value in v1
    mv t0,x0
    mv t1,x0
    # t4 is the stride of v0
    # t5……v1
    mv t4,a3
    mv t5,a4
    slli t4,t4,2
    slli t5,t5,2
loop_start:
    # get product in this time loop
    beq t0,a2,done
    lw t2,0(a0)
    lw t3,0(a1)
    mul t2,t2,t3
    add t1,t1,t2

loop_end:
    #next loop
    addi t0,t0,1
    add a0,a0,t4
    add a1,a1,t5
    jal x0,loop_start
    # Epilogue
done:
    # return product
    mv a0,t1
    
    ret