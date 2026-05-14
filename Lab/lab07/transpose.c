#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src) {
  for (int x = 0; x < n; x++) {
    for (int y = 0; y < n; y++) {
      dst[y + x * n] = src[x + y * n];
    }
  }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src) {
  // YOUR CODE HERE
  for (int BlockCountByrow = 0; BlockCountByrow < n;
       BlockCountByrow += blocksize) {
    for (int BlockCountBycol = 0; BlockCountBycol < n;
         BlockCountBycol += blocksize) {
      for (int x = BlockCountByrow; x < (BlockCountByrow + blocksize) && x < n;
           x++) {
        for (int y = BlockCountBycol;
             y < (BlockCountBycol + blocksize) && y < n; y++) {
          dst[y + x * n] = src[x + y * n];
        }
      }
    }
  }
}
