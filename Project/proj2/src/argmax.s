.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the size of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:
    # Prologue
    # t1 as index
    # t3 as pointer
    # a0 save return value--the largest value
    # t4 save the index of a0
    # t0 as every loop tem value
    mv t1,x0
    addi t3,a0,0
    lw a0,0(t3)
    addi t4,x0,1
loop_start:
    bge t1,a1,done
    lw t0,0(t3)
    # less,negelect it
    blt t0,a0,loop_end
   
loop_continue:
    #get a larger value
    lw a0,0(t3)
    mv t4,t1

loop_end:
    addi t1,t1,1
    addi t3,t3,4
    jal x0,loop_start

    # Epilogue
done:
    ret