.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    #verify the number of arg
    li t0,5
    beq a0,t0,load
    li a0,89
    jal exit
load:
    
    addi sp,sp,-32
    sw ra,0(sp)
    sw s0,4(sp)
    sw s1,8(sp)
    sw s2,12(sp)
    sw s3,16(sp)
    sw s4,20(sp)
    sw s5,24(sp)
    sw s6,28(sp)
    # save argv in s0,a2 in s1
    mv s0,a1
    mv s1,a2

	# =====================================
    # LOAD MATRICES
    # =====================================

  

    # Load pretrained m0
    # malloc for m0,save one pointer and two value
    li a0,12
    jal malloc
    # s2 save m0
    mv s2,a0
    #load
    lw t0,4(s0)
    sw t0,0(s2)
    lw a0,0(s2)
    addi a1,s2,4
    addi a2,s2,8
    jal read_matrix
    sw a0,0(s2)

   

    # Load pretrained m1
    # malloc for m1,save one pointer
    li a0,12
    jal malloc
    # s3 save m1
    mv s3,a0
    #load
    lw t0,8(s0)
    sw t0,0(s3)
    lw a0,0(s3)
    addi a1,s3,4
    addi a2,s3,8
    jal read_matrix
    sw a0,0(s3)



    # Load input matrix
    # malloc for input,save one pointer
    li a0,12
    jal malloc
    # s4 save m1
    mv s4,a0
    #load
    lw t0,12(s0)
    mv a0,t0
    addi a1,s4,4
    addi a2,s4,8
    jal read_matrix  
    sw a0,0(s4)



 

    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)
    
    #malloc as destination,s5 save
    lw t0,4(s2)
    lw t1,8(s4)
    mul a0,t0,t1
    slli a0,a0,2
    addi a0,a0,4
    jal malloc
    mv s5,a0
    #step 1
    lw a0,0(s2)
    lw a1,4(s2)
    lw a2,8(s2)
    lw a3,0(s4)
    lw a4,4(s4)
    lw a5,8(s4)
    mv a6,s5
    jal matmul


    #step 2
    mv a0,s5
    lw t0,4(s2)
    lw t1,8(s4)
    mul a1,t0,t1
    jal relu
    #step 3
    #malloc as destination,s6 save
    lw t0,4(s3)
    lw t1,8(s4)
    mul a0,t0,t1
    slli a0,a0,2
    jal malloc
    mv s6,a0
    
    lw a0,0(s3)
    lw a1,4(s3)
    lw a2,8(s3)
    mv a3,s5
    lw a4,4(s3)
    lw a5,8(s4)
    mv a6,s6
    jal matmul



    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix
    lw a0,16(s0)
    mv a1,s6
    lw a2,4(s3)
    lw a3,8(s4)
    jal write_matrix

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax
    mv a0,s6
    lw t0,4(s3)
    lw t1,8(s4)
    mul a1,t0,t1
    jal argmax

    
    #remember a2
    # Print classification
    bne s1,x0,done
    mv a1,a0
    jal print_int
    

    # Print newline afterwards for clarity
    #ascii 10
    li a1,10
    jal print_char  

done:
    
    lw ra,0(sp)
    lw s0,4(sp)
    lw s1,8(sp)
    lw s2,12(sp)
    lw s3,16(sp)
    lw s4,20(sp)
    lw s5,24(sp)
    lw s6,28(sp)
    addi sp,sp,32
    ret

fuck:
    li a1,46
    jal exit2