#include "matrix.h"
#include <omp.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include SSE intrinsics
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <immintrin.h>
#include <x86intrin.h>
#endif

/* Below are some intel intrinsics that might be useful
 * void _mm256_storeu_pd (double * mem_addr, __m256d a)
 * __m256d _mm256_set1_pd (double a)
 * __m256d _mm256_set_pd (double e3, double e2, double e1, double e0)
 * __m256d _mm256_loadu_pd (double const * mem_addr)
 * __m256d _mm256_add_pd (__m256d a, __m256d b)
 * __m256d _mm256_sub_pd (__m256d a, __m256d b)
 * __m256d _mm256_fmadd_pd (__m256d a, __m256d b, __m256d c)
 * __m256d _mm256_mul_pd (__m256d a, __m256d b)
 * __m256d _mm256_cmp_pd (__m256d a, __m256d b, const int imm8)
 * __m256d _mm256_and_pd (__m256d a, __m256d b)
 * __m256d _mm256_max_pd (__m256d a, __m256d b)
 */

/*
 * Generates a random double between `low` and `high`.
 */
double rand_double(double low, double high) {
  double range = (high - low);
  double div = RAND_MAX / range;
  return low + (rand() / div);
}

/*
 * Generates a random matrix with `seed`.
 */
void rand_matrix(matrix *result, unsigned int seed, double low, double high) {
  srand(seed);
  for (int i = 0; i < result->rows; i++) {
    for (int j = 0; j < result->cols; j++) {
      set(result, i, j, rand_double(low, high));
    }
  }
}

void allocateJuge(void *a) {
  if (a == NULL) {
    printf("allocate error!");
    exit(-1);
  }
}

/*
 * Allocate space for a matrix struct pointed to by the double pointer mat with
 * `rows` rows and `cols` columns. You should also allocate memory for the data
 * array and initialize all entries to be zeros. Remember to set all fieds of
 * the matrix struct. `parent` should be set to NULL to indicate that this
 * matrix is not a slice. You should return -1 if either `rows` or `cols` or
 * both have invalid values, or if any call to allocate memory in this function
 * fails. If you don't set python error messages here upon failure, then
 * remember to set it in numc.c. Return 0 upon success and non-zero upon
 * failure.
 */
// 我明白了，用二级指针管理整个矩阵，用一级指针管理原始以及引用矩阵。
// 数组得用动态二维数组，不然的话创建数组引用的时候就没法搞了，不知道步长，除非修改结构体添加步长变量，不然只能这样。或者漫长地去引用原矩阵以得到参数
// 还是引用得到步长参数吧
// 不行，还是得用动态二维数组
int allocate_matrix(matrix **mat, int rows, int cols) {
  /* TODO: YOUR CODE HERE */
  // test
  if (rows <= 0 || cols <= 0) {
    printf("input error!");
    return -1;
  }
  // set
  matrix *rowMat = (matrix *)malloc(sizeof(matrix));
  allocateJuge(rowMat);
  *mat = rowMat;
  rowMat->cols = cols;
  rowMat->rows = rows;
  rowMat->data = (double **)malloc(rows * sizeof(double *));
  for (int i = 0; i < rows; i++) {
    rowMat->data[i] = (double *)malloc(sizeof(double) * cols);
  }
  allocateJuge(rowMat->data);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      rowMat->data[i][j] = 0;
    }
  }
  rowMat->parent = NULL;
  if (rows == 1 || cols == 1)
    rowMat->is_1d = 1;
  rowMat->ref_cnt = 1;
  return 0;
}

/*
 * Allocate space for a matrix struct pointed to by `mat` with `rows` rows and
 * `cols` columns. This is equivalent to setting the new matrix to be
 * from[row_offset:row_offset + rows, col_offset:col_offset + cols]
 * If you don't set python error messages here upon failure, then remember to
 * set it in numc.c. Return 0 upon success and non-zero upon failure.
 */
int allocate_matrix_ref(matrix **mat, matrix *from, int row_offset,
                        int col_offset, int rows, int cols) {
  /* TODO: YOUR CODE HERE */
  if (rows <= 0 || cols <= 0 || row_offset < 0 || col_offset < 0) {
    printf("input error!");
    return -1;
  }
  matrix *CopyMat = (matrix *)malloc(sizeof(matrix));
  allocateJuge(CopyMat);
  *mat = CopyMat;
  CopyMat->cols = cols;
  CopyMat->rows = rows;
  CopyMat->data = (double **)malloc(rows * sizeof(double *));
  for (int i = row_offset, j = 0; i < row_offset + rows; i++, j++) {
    CopyMat->data[j] = from->data[i] + col_offset;
  }
  CopyMat->parent = from;
  if (rows == 1 || cols == 1)
    CopyMat->is_1d = 1;
  from->ref_cnt++;
  return 0;
}

/*
 * This function will be called automatically by Python when a numc matrix loses
 * all of its reference pointers. You need to make sure that you only free
 * `mat->data` if no other existing matrices are also referring this data array.
 * See the spec for more information.
 */
void deallocate_matrix(matrix *mat) {
  /* TODO: YOUR CODE HERE */
  // 分两种情况吧，一种是初始矩阵，也就是parent项里只有null值的，另一种是parent项里存在parent的
  // 初始矩阵调用只能是所有矩阵都没有了，不然之后其他矩阵通过from找到这里不坏了
  // 那就这样，初始矩阵调用，就先deallocate data，再deallocate matrix。
  // 引用矩阵调用，就将parent里的引用值减一
  // 再free matrix(需要free吗？感觉不对劲呢)，把free去掉看看
  // 如果在初始矩阵在引用不为1的情况下调用，则报错。
  // 初始矩阵
  if (mat == NULL) {
    printf("deallocate null matrix!");
    return;
  }
  if (mat->parent == NULL) {
    if (mat->ref_cnt != 1) {
      printf("error!not the last call original matrix!");
    }
    for (int i = 0; i < mat->rows; i++) {
      free((mat->data)[i]);
    }
    free((mat->data));
    free(mat);
  } else {
    mat->parent->ref_cnt--;
    // free(mat);
  }
}

/*
 * Return the double value of the matrix at the given row and column.
 * You may assume `row` and `col` are valid.
 */
double get(matrix *mat, int row, int col) {
  /* TODO: YOUR CODE HERE */
  return mat->data[row][col];
}

/*
 * Set the value at the given row and column to val. You may assume `row` and
 * `col` are valid
 */
void set(matrix *mat, int row, int col, double val) {
  /* TODO: YOUR CODE HERE */
  mat->data[row][col] = val;
}

/*
 * Set all entries in mat to val
 */
void fill_matrix(matrix *mat, double val) {
  /* TODO: YOUR CODE HERE */
  for (int i = 0; i < mat->rows; i++) {
    for (int j = 0; j < mat->cols; j++) {
      mat->data[i][j] = val;
    }
  }
}

/*
 * Store the result of adding mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int add_matrix(matrix *result, matrix *mat1, matrix *mat2) {
  /* TODO: YOUR CODE HERE */
  // 判断参数是否正常，判断参数是否可以进行相加
  if (result == NULL || mat1 == NULL || mat2 == NULL ||
      result->cols != mat1->cols || result->rows != mat1->rows ||
      mat1->cols != mat2->cols || mat1->rows != mat2->rows) {
    return -1;
  }
  for (int i = 0; i < result->rows; i++) {
    for (int j = 0; j < result->cols; j++) {
      result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
    }
  }
  return 0;
}

/*
 * Store the result of subtracting mat2 from mat1 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int sub_matrix(matrix *result, matrix *mat1, matrix *mat2) {
  /* TODO: YOUR CODE HERE */
  if (result == NULL || mat1 == NULL || mat2 == NULL ||
      result->cols != mat1->cols || result->rows != mat1->rows ||
      mat1->cols != mat2->cols || mat1->rows != mat2->rows) {
    return -1;
  }
  for (int i = 0; i < result->rows; i++) {
    for (int j = 0; j < result->cols; j++) {
      result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
    }
  }
  return 0;
}

/*
 * Store the result of multiplying mat1 and mat2 to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that matrix multiplication is not the same as multiplying individual
 * elements.
 */
int mul_matrix(matrix *result, matrix *mat1, matrix *mat2) {
  /* TODO: YOUR CODE HERE */
  if (result == NULL || mat1 == NULL || mat2 == NULL ||
      mat1->rows != mat2->cols) {
    printf("mul wrong!");
    return -1;
  }
  fill_matrix(result, 0);
  for (int i = 0; i < mat1->rows; i++) {
    for (int k = 0; k < mat1->cols; k++) {
      for (int j = 0; j < mat2->cols; j++) {
        result->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
      }
    }
  }
  return 0;
}

/*
 * Store the result of raising mat to the (pow)th power to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 * Remember that pow is defined with matrix multiplication, not element-wise
 * multiplication.
 */
int pow_matrix(matrix *result, matrix *mat, int pow) {
  /* TODO: YOUR CODE HERE */
  if (pow <= 0 || result == NULL || mat == NULL) {
    return -1;
  }
  // copy
  for (int i = 0; i < result->rows; i++) {
    for (int j = 0; j < result->cols; j++) {
      result->data[i][j] = mat->data[i][j];
    }
  }
  matrix **resultCopy = (matrix **)malloc(sizeof(matrix *));
  allocate_matrix(resultCopy, result->rows, result->cols);
  for (int i = 1; i < pow; i++) {
    mul_matrix(*resultCopy, result, mat);
    for (int i = 0; i < result->rows; i++) {
      for (int j = 0; j < result->cols; j++) {
        result->data[i][j] = (*resultCopy)->data[i][j];
      }
    }
  }
  deallocate_matrix(*resultCopy);
  return 0;
}

/*
 * Store the result of element-wise negating mat's entries to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int neg_matrix(matrix *result, matrix *mat) {
  /* TODO: YOUR CODE HERE */
  if (result == NULL || mat == NULL) {
    return -1;
  }
  for (int i = 0; i < mat->rows; i++) {
    for (int j = 0; j < mat->cols; j++) {
      result->data[i][j] = -mat->data[i][j];
    }
  }
  return 0;
}

/*
 * Store the result of taking the absolute value element-wise to `result`.
 * Return 0 upon success and a nonzero value upon failure.
 */
int abs_matrix(matrix *result, matrix *mat) {
  /* TODO: YOUR CODE HERE */
  if (result == NULL || mat == NULL) {
    return -1;
  }
  double HelpJudge = 0;
  for (int i = 0; i < mat->rows; i++) {
    for (int j = 0; j < mat->cols; j++) {
      HelpJudge = mat->data[i][j];
      if (HelpJudge < 0) {
        HelpJudge = -HelpJudge;
      }
      result->data[i][j] = HelpJudge;
    }
  }
  return 0;
}
