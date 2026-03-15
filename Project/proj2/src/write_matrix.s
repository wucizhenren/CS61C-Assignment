.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
# - If you receive an fopen error or eof,
#   this function terminates the program with error code 93.
# - If you receive an fwrite error or eof,
#   this function terminates the program with error code 94.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 95.
# ==============================================================================
write_matrix:

    # Prologue
    addi sp,sp,-24
    sw s0,0(sp)
    sw s1,4(sp)
    sw s2,8(sp)
    sw s3,12(sp)
    sw s4,16(sp)
    sw ra,20(sp)



    #s0--s3 save argument
    mv s0,a0
    mv s1,a1
    mv s2,a2
    mv s3,a3


    # fopen
    mv a1,s0
    li a2,1
    jal fopen
    li t0,-1
    beq a0,t0,error1
    #s4 save file descriptor
    mv s4,a0


    #malloc
    li a0,4
    jal malloc
    sw s2,0(a0)


    #fwrite row
    mv a1,s4
    mv a2,a0
    li a3,1
    li a4,4
    addi sp,sp,-4
    sw a3,0(sp)
    jal fwrite
    lw a3,0(sp)
    addi sp,sp,4
    bne a0,a3,error2


    #malloc
    li a0,4
    jal malloc
    sw s3,0(a0)
    #fwrite col
    mv a1,s4
    mv a2,a0
    li a3,1
    li a4,4
    addi sp,sp,-4
    sw a3,0(sp)
    jal fwrite
    lw a3,0(sp)
    addi sp,sp,4
    bne a0,a3,error2
   

    #fwrite matrix
    mv a1,s4
    mv a2,s1
    #t0 number to send
    mul t0,s2,s3
    mv a3,t0
    li a4,4
    addi sp,sp,-4
    sw a3,0(sp)
    jal fwrite
    lw a3,0(sp)
    addi sp,sp,4
    bne a0,a3,error2

    
    #fclose
    mv a1,s4
    jal fclose
    li t0,-1
    beq a0,t0,error3
    # Epilogue
    lw s0,0(sp)
    lw s1,4(sp)
    lw s2,8(sp)
    lw s3,12(sp)
    lw s4,16(sp)
    lw ra,20(sp)
    addi sp,sp,24
    ret
    
error1:
    li a0,93
    jal exit
    
error2:
    li a0,94
    jal exit

error3:
    li a0,95
    jal exit