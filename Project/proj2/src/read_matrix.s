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
    #s4 save file descirptor
    mv s4,a0
    #fread
    mv a1,s4
    mv a2,s1
    li a3,4
    jal fread
    #fread
    mv a1,s4
    mv a2,s2
    li a3,4
    jal fread
    
    #malloc
    lw t0,0(s0)
    lw t1,0(s1)
    mul a0,t0,t1
    slli a0,a0,2
    jal malloc
    mv s3,a0
    # fread
    mv a1,s4
    mv a2,s3
    lw t0,0(s0)
    lw t1,0(s1)
    mul a3,t0,t1
    slli a3,a0,2
    jal fread
    # Epilogue
    #fclose
    mv a1,s4
    jal fclose
    lw s0,0(sp)
    lw s1,4(sp)
    lw s2,8(sp)
    lw s3,12(sp)
    lw s4,16(sp)
    lw ra,20(sp)
    addi sp,sp,24
    ret