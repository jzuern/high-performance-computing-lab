/* High performance computing - Filling class implementation
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Constantin Heisler
 */

#include <stdlib.h>
#include <stdio.h>
#include "filling.h"

void Filling_produceRunner(Field* field, int value, int xpos, int ypos) {
  long x, y;
  //int boundary_x = (Field_getSizeInDim(field, X) - Field_getInnerSizeInDim(field, X)) / 2;
  //int boundary_y = (Field_getSizeInDim(field, Y) - Field_getInnerSizeInDim(field, Y)) / 2;

  if ( Field_getInnerSizeInDim(field, X) <= (xpos + 2)
    || Field_getInnerSizeInDim(field, Y) <= (ypos + 2)) {
    printf("Filling_produceRunner: X/Y Werte ausserhalb des Spielfeldes oder zu nah am Rand: xpos: %d \t ypos: %d\n", xpos, ypos);
    exit(EXIT_FAILURE);
  }

  x = xpos;
  y = ypos;

  Field_setCell(field, x+0, y+1, value);
  Field_setCell(field, x+1, y+2, value);
  Field_setCell(field, x+2, y+0, value);
  Field_setCell(field, x+2, y+1, value);
  Field_setCell(field, x+2, y+2, value);
}

static void Filling_produceGlider1(Field* field, long x, long y, int value) {
  Field_setCell(field, x+2, y+0, value);
  Field_setCell(field, x+3, y+0, value);
  Field_setCell(field, x+1, y+1, value);
  Field_setCell(field, x+5, y+1, value);
  Field_setCell(field, x+0, y+2, value);
  Field_setCell(field, x+6, y+2, value);
  Field_setCell(field, x+0, y+3, value);
  Field_setCell(field, x+4, y+3, value);
  Field_setCell(field, x+6, y+3, value);
  Field_setCell(field, x+7, y+3, value);
  Field_setCell(field, x+0, y+4, value);
  Field_setCell(field, x+6, y+4, value);
  Field_setCell(field, x+1, y+5, value);
  Field_setCell(field, x+5, y+5, value);
  Field_setCell(field, x+2, y+6, value);
  Field_setCell(field, x+3, y+6, value);
}

static void Filling_produceGlider2(Field* field, long x, long y, int value) {
  Field_setCell(field, x+4, y+0, value);
  Field_setCell(field, x+2, y+1, value);
  Field_setCell(field, x+4, y+1, value);
  Field_setCell(field, x+0, y+2, value);
  Field_setCell(field, x+1, y+2, value);
  Field_setCell(field, x+0, y+3, value);
  Field_setCell(field, x+1, y+3, value);
  Field_setCell(field, x+0, y+4, value);
  Field_setCell(field, x+1, y+4, value);
  Field_setCell(field, x+2, y+5, value);
  Field_setCell(field, x+4, y+5, value);
  Field_setCell(field, x+4, y+6, value);
}

void Filling_produceGun(Field* field, int value, int xpos, int ypos) {
  long x, y;

  if(Field_getInnerSizeInDim(field, X) <= (xpos + 35) || Field_getInnerSizeInDim(field, Y) <= (ypos + 8)) {
    printf("Filling_produceGun: X/Y Werte ausserhalb des Spielfeldes oder zu nah am Rand: xpos: %d \t ypos: %d\n", xpos, ypos);
    return;
  }

  x = xpos;
  y = ypos;

  // block left
  Field_setCell(field, x+0, y+4, value);
  Field_setCell(field, x+1, y+4, value);
  Field_setCell(field, x+0, y+5, value);
  Field_setCell(field, x+1, y+5, value);

  // glider left
  Filling_produceGlider1(field, x + 10, y + 2, value);

  // glider right
  Filling_produceGlider2(field, x + 20, y + 0, value);

  // block right
  Field_setCell(field, x+34, y+2, value);
  Field_setCell(field, x+35, y+2, value);
  Field_setCell(field, x+34, y+3, value);
  Field_setCell(field, x+35, y+3, value);
}

void Filling_binaryRandomizeFrame_f(Field* field, int value) {
  long x, y;
  const int offset = 1;

  for (y = offset; y < Field_getInnerSizeInDim(field, Y) - offset; y++) {
    for (x = offset; x < Field_getInnerSizeInDim(field, X) - offset; x++) {
      Field_setCell_f(field, x, y, value * (random() % 2));
    }
  }
}

void Filling_binaryRandomizeFrame(Field* field, int value) {
  long x, y;
  const int offset = 1;

  for (y = offset; y < Field_getInnerSizeInDim(field, Y) - offset; y++) {
    for (x = offset; x < Field_getInnerSizeInDim(field, X) - offset; x++) {
      Field_setCell(field, x, y, value * (random() % 2));
    }
  }
}
