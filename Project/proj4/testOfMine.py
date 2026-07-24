import numc as nc
# 索引部分测试1
# a=nc.Matrix(1,3,[1,2,3])
# print(a[1])
# print(a[0:1])
# a=nc.Matrix(2,2,[1,2,3,4])
# print(a[0])
# print(a[0:1])
# print(a[0:1,0:1])
# print(11111)
# print(a[0:0,0])
# print(a[0:1,0])
# print(a[0,0:1])
# print(a[0,1])

# # 索引部分测试2
a = nc.Matrix(3, 3)
a[0:1, 0:1] = 0.0
a[:, 0] = [1, 1, 1]
print(a)