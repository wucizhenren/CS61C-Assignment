/************************************************************************
**
** NAME:        gameoflife.c
**
** DESCRIPTION: CS61C Fall 2020 Project 1
**
** AUTHOR:      Justin Yokota - Starter Code
**				YOUR NAME HERE
**
**
** DATE:        2020-08-23
**
**************************************************************************/

#include "imageloader.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Determines what color the cell at the given row/col should be. This function
// allocates space for a new Color. Note that you will need to read the eight
// neighbors of the cell in question. The grid "wraps", so we treat the top row
// as adjacent to the bottom row and the left column as adjacent to the right
// column.

Color *evaluateOneCell(Image *image, int row, int col, uint32_t rule) {
  // YOUR CODE HERE
  // 统计周围活细胞数目
  int aliveCellR = 0, aliveCellG = 0, aliveCellB = 0;
  Color *newColor = (Color *)malloc(sizeof(Color));
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) {
        continue;
      }
      if (image
              ->image[((row + i) + image->rows) % image->rows]
                     [((col + j) + image->cols) % image->cols]
              .R == 255) {
        aliveCellR++;
      }
      if (image
              ->image[((row + i) + image->rows) % image->rows]
                     [((col + j) + image->cols) % image->cols]
              .G == 255) {
        aliveCellG++;
      }
      if (image
              ->image[((row + i) + image->rows) % image->rows]
                     [((col + j) + image->cols) % image->cols]
              .B == 255) {
        aliveCellB++;
      }
    }
  }
  // 如果是活细胞
  // 和rule的前9位&
  if (image->image[row][col].R == 255) {
    if ((1 << aliveCellR << 9) & rule) {
      newColor->R = 255;
    } else {
      newColor->R = 0;
    }
  }
  // 如果是死细胞
  // 和rule的后9位&
  else {
    if ((1 << aliveCellR) & rule) {
      newColor->R = 255;
    } else {
      newColor->R = 0;
    }
  }

  // G
  if (image->image[row][col].G == 255) {
    if ((1 << aliveCellR << 9) & rule) {
      newColor->G = 255;
    } else {
      newColor->G = 0;
    }
  }
  // 如果是死细胞
  // 和rule的后9位&
  else {
    if ((1 << aliveCellG) & rule) {
      newColor->G = 255;
    } else {
      newColor->G = 0;
    }
  }
  // B
  if (image->image[row][col].B == 255) {
    if ((1 << aliveCellB << 9) & rule) {
      newColor->B = 255;
    } else {
      newColor->B = 0;
    }
  }
  // 如果是死细胞
  // 和rule的后9位&
  else {
    if ((1 << aliveCellB) & rule) {
      newColor->B = 255;
    } else {
      newColor->B = 0;
    }
  }
  return newColor;
}

// The main body of Life; given an image and a rule, computes one iteration of
// the Game of Life. You should be able to copy most of this from
// steganography.c
Image *life(Image *image, uint32_t rule) {
  // YOUR CODE HERE
  Image *res = (Image *)malloc(sizeof(Image));
  res->cols = image->cols;
  res->rows = image->rows;
  res->image = (Color **)malloc(sizeof(Color *) * res->rows);
  for (int i = 0; i < res->rows; i++) {
    res->image[i] = (Color *)malloc(sizeof(Color) * res->cols);
  }
  for (int i = 0; i < res->rows; i++) {
    for (int j = 0; j < res->cols; j++) {
      Color *tmp = evaluateOneCell(image, i, j, rule);
      res->image[i][j] = *tmp;
      free(tmp);
    }
  }
  return res;
}

/*
Loads a .ppm from a file, computes the next iteration of the game of life, then
prints to stdout the new image.

argc stores the number of arguments.
argv stores a list of arguments. Here is the expected input:
argv[0] will store the name of the program (this happens automatically).
argv[1] should contain a filename, containing a .ppm.
argv[2] should contain a hexadecimal number (such as 0x1808). Note that this
will be a string. You may find the function strtol useful for this conversion.
If the input is not correct, a malloc fails, or any other error occurs, you
should exit with code -1. Otherwise, you should return from main with code 0.
Make sure to free all memory before returning!

You may find it useful to copy the code from steganography.c, to start.
*/

void processCLI(int argc, char **argv, char **filename) {
  if (argc != 3) {
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
  char **tmp;
  char *tmp2 = argv[2];
  long rule = strtol(tmp2, tmp, 16);
  Image *newImage = life(image, rule);
  writeData(newImage);
  freeImage(image);
  freeImage(newImage);
  return 0;
}
