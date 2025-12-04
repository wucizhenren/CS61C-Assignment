/************************************************************************
**
** NAME:        imageloader.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**              Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-15
**
**************************************************************************/

#include "imageloader.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Opens a .ppm P3 image file, and constructs an Image object.
// You may find the function fscanf useful.
// Make sure that you close the file with fclose before returning.
Image *readData(char *filename) {
  // YOUR CODE HERE
  Image *res = (Image *)malloc(sizeof(Image));
  FILE *fp = fopen(filename, "r");
  char *tmp = (char *)malloc(sizeof(char) * 5);
  // P3
  fscanf(fp, "%s", tmp);

  // 读取行和列
  int row, col;
  fscanf(fp, "%d %d", &col, &row);
  res->cols = col;
  res->rows = row;

  // 225
  fscanf(fp, "%s", tmp);

  free(tmp);
  // color
  // 这里有一个关于多级指针和多维数组的区别
  // 多维数组的内存是连续存放的，但是多级指针是指向了一个数组，数组里存放的是指向不同位置的数组的指针，也就是都是分开存放的
  // 这里因为头文件定义是多级指针，所以按照多级指针的写法去写
  // 还有一点理解错了，就是关于数组的类型，以及[]操作符。
  // []操作符实际上的操作还是*(a+x)，不过之前关于多维数组的内容让我以为[i][j]在数组中变成了*((a+i*?)+j)，但实际上还是原来那样
  // 因为这里的误会就是关于数组的类型，一维数组char
  // a[3]的类型是char*[3],(虽然单独a的类型是char*) 到了多维数组，char
  // a[3][3]的类型是char*
  // [3][3],然后a的类型是char*[3],只有这样在运行*(*(a+i)+j)的时候才可以根据类型定指针走的步长
  // 而多级指针就不一样了，哪里不一样根据类型来看就可以，在多维指针里，关于步长的地方就得我们自己操作了。
  res->image = (Color **)malloc(row * sizeof(Color *));
  for (int i = 0; i < row; i++) {
    res->image[i] = malloc(col * sizeof(Color));
  }
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      fscanf(fp, "%hhd", &(res->image[i][j].R));
      fscanf(fp, "%hhd", &(res->image[i][j].G));
      fscanf(fp, "%hhd", &(res->image[i][j].B));
    }
  }
  fclose(fp);
  return res;
}

// Given an image, prints to stdout (e.g. with printf) a .ppm P3 file with the
// image's data.
void writeData(Image *image) {
  // YOUR CODE HERE
  printf("P3\n%d %d\n255\n", image->cols, image->rows);
  for (int i = 0; i < image->rows; i++) {
    for (int j = 0; j < image->cols; j++) {
      if (j == image->cols - 1) {
        printf("%3d %3d %3d", (image->image[i])[j].R, (image->image[i])[j].G,
               (image->image[i])[j].B);
        continue;
      }
      printf("%3d %3d %3d   ", (image->image[i])[j].R, (image->image[i])[j].G,
             (image->image[i])[j].B);
    }
    printf("\n");
  }
}

// Frees an image
void freeImage(Image *image) {
  // YOUR CODE HERE

  for (int i = 0; i < image->rows; i++) {
    free(image->image[i]);
  }
  free(image->image);
  free(image);
}