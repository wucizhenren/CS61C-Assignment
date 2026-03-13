.globl relu
.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
# 就是对int数组实现原地的算法
# 第一个参数指向的数组是一维的
# 将小于零的元素设置为零就可以了
# t0作为index
# 循环中就是对每个变量做一遍操作就可以了
relu:
lw t1,x0
loop_start:
# Prologue
    #t0作为存储a0当前值的寄存器
    #t1作为index
    lw t0,0(a0)
    beq a1,t1,done

loop_continue:
    # 对数组中每个元素进行检测，小于零则置零，大于零则直接跳到end
    bge t0,zero,loop_end
    mv t0,x0
    sw t0,0(a0)

   
loop_end:
# a0向后走,t1++
    addi a0,a0,1
    addi t1,t1,1
    jal x0,loop_start
    # Epilogue
done:
# 跳出循环
    
	ret