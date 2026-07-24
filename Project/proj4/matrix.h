#include <Python.h>

typedef struct matrix {
    int rows;      	// number of rows
    int cols;      	// number of columns
    double **data; 	// each element is a pointer to a row of data
    int is_1d;     	// Whether this matrix is a 1d matrix,就是col或者row有一个为1这个就是1.
    // For 1D matrix, shape is (rows * cols)
    int ref_cnt;
    //当前与这个 matrix 结构体共享全部或部分数据数组（data array）的 matrix 结构体数量（包括它自己）。
    struct matrix *parent;//如果是一个矩阵的切割就指向那个矩阵，反之则为null
} matrix;


void rand_matrix(matrix *result, unsigned int seed, double low, double high);
int allocate_matrix(matrix **mat, int rows, int cols);
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset,
                        int col_offset, int rows, int cols);
void deallocate_matrix(matrix *mat);
double get(matrix *mat, int row, int col);
void set(matrix *mat, int row, int col, double val);
void fill_matrix(matrix *mat, double val);
int add_matrix(matrix *result, matrix *mat1, matrix *mat2);
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2);
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2);
int pow_matrix(matrix *result, matrix *mat, int pow);
int neg_matrix(matrix *result, matrix *mat);
int abs_matrix(matrix *result, matrix *mat);
