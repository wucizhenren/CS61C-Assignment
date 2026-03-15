.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
# fopen
#row = fread
#col = fread
# int a[][]=new malloc();
#         a[][]=file();
# fclose
read_matrix:

    # Prologue
    addi sp,sp,-24
    sw s0,0(sp)
    sw s1,4(sp)
    sw s2,8(sp)
    sw s3,12(sp)
    sw s4,16(sp)
    sw ra,20(sp)
	#s0,s1,s2 save the arguments
    #s3 save the return pointer
    mv s0,a0
    mv s1,a1
    mv s2,a2
    #fopen
    mv a1,s0
    li a2,0
    jal fopen
    li t0,-1
    beq a0,t0,bug2
    #s4 save file descirptor
    mv s4,a0
    #fread
    mv a1,s4
    mv a2,s1
    li a3,4
    jal fread
    li t0,4
    bne a0,t0,bug3
    #fread
    mv a1,s4
    mv a2,s2
    li a3,4
    jal fread
    li t0,4
    bne a0,t0,bug3
    
    #malloc
    lw t0,0(s1)
    lw t1,0(s2)
    mul a0,t0,t1
    slli a0,a0,2
    jal malloc
    mv s3,a0
    # fread
    mv a1,s4
    mv a2,s3
    lw t0,0(s1)
    lw t1,0(s2)
    mul a3,t0,t1
    slli a3,a3,2
    addi sp,sp,-4
    sw a3,0(sp)
    jal fread
    lw a3,0(sp)
    addi sp,sp,4
    bne a3,a0,bug3
    # Epilogue
    #fclose
    mv a1,s4
    jal fclose
    mv a0,s3
    mv a1,s1
    mv a2,s2
    lw s0,0(sp)
    lw s1,4(sp)
    lw s2,8(sp)
    lw s3,12(sp)
    lw s4,16(sp)
    lw ra,20(sp)
    addi sp,sp,24
    ret

bug1:
    li a1,88
    jal exit2
bug2:
    li a1,90
    jal exit2
bug3:
    li a1,91
    jal exit2
bug4:
    li a1,92
    jal exit2

fuck:
    li a1,46
    jal exit2