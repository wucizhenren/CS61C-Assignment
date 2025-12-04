/************************************************************************
**
** NAME:        steganography.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Dan Garcia  -  University of California at Berkeley
**              Copyright (C) Dan Garcia, 2020. All rights reserved.
**				Justin Yokota - Starter Code
**				YOUR NAME HERE
**
** DATE:        2020-08-23
**
**************************************************************************/

#include "imageloader.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

// Determines what color the cell at the given row/col should be. This should
// not affect Image, and should allocate space for a new Color.
Color *evaluateOnePixel(Image *image, int row, int col) {
  // YOUR CODE HERE
  Color *newCol = (Color *)malloc(sizeof(Color));
  if (image->image[row][col].B & 0x1) {
    newCol->R = 255;
    newCol->G = 255;
    newCol->B = 255;
  } else {
    newCol->R = 0;
    newCol->G = 0;
    newCol->B = 0;
  }
  return newCol;
}

// Given an image, creates a new image extracting the LSB of the B channel.
Image *steganography(Image *image) {
  // YOUR CODE HERE
  Image *res = (Image *)malloc(sizeof(Image));
  res->cols = image->cols;
  res->rows = image->rows;
  res->image = (Color **)malloc(sizeof(Color *) * res->rows);
  for (int i = 0; i < res->cols; i++) {
    res->image[i] = (Color *)malloc(sizeof(Color) * res->cols);
  }
  for (int i = 0; i < res->rows; i++) {
    for (int j = 0; j < res->cols; j++) {
      Color *tmp = evaluateOnePixel(image, i, j);
      res->image[i][j] = *tmp;
      free(tmp);
    }
  }
  return res;
}

/*
Loads a file of ppm P3 format from a file, and prints to stdout (e.g. with
printf) a new image, where each pixel is black if the LSB of the B channel is 0,
and white if the LSB of the B channel is 1.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a file of ppm P3 format (not
necessarily with .ppm file extension). If the input is not correct, a malloc
fails, or any other error occurs, you should exit with code -1. Otherwise, you
should return from main with code 0. Make sure to free all memory before
returning!
*/
// 看上面，原来如此，我说之前我用gdb看的时候，怎么只输入了两个参数，argc就等于3了，原来还有一个默认的程序名称

void processCLI(int argc, char **argv, char **filename) {
  if (argc != 2) {
    exit(-1);
  }
  *filename = argv[1];
}

int main(int argc, char **argv) {
  // YOUR CODE HERE
  Image *image;
  char *filename;
  processCLI(argc, argv, &filename);
  image = readData(filename);
  Image *LSB = steganography(image);
  writeData(LSB);
  freeImage(image);
  freeImage(LSB);
  return 0;
}
