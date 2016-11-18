/* High performance computing - Field class implementation
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#include <stdlib.h>
#include <endian.h>
#include "field.h"

void Field_init(Field* self, int size[2], int boundarysize[2]) {
  self->size[X] = size[X];
  self->size[Y] = size[Y];
  self->boundarysize[X] = boundarysize[X];
  self->boundarysize[Y] = boundarysize[Y];
  self->data[OLD] = calloc(self->size[X]*self->size[Y], sizeof(int));
  self->data[NEW] = calloc(self->size[X]*self->size[Y], sizeof(int));
#ifdef MPI
  // transform memory size to inner size
  self->size[X] -= 2;
  self->size[Y] -= 2;
#endif
}

void Field_deinit(Field* self) {
  free(self->data[OLD]);
  free(self->data[NEW]);
}

void Field_swap(Field* self) {
  int *tmp;

  tmp             = self->data[OLD];
  self->data[OLD] = self->data[NEW];
  self->data[NEW] = tmp;
}

void Field_setCell(Field* self, int x, int y, int value) {
#ifndef MPI
  self->data[NEW][y * self->size[X] + x] = value;
#else
  self->data[NEW][(y + self->boundarysize[Y]) * (self->size[X] + 2*self->boundarysize[X]) + self->boundarysize[X] + x] = htobe32(value);
#endif
}

int Field_getCell(Field* self, int x, int y) {
#ifndef MPI
  return self->data[OLD][y * self->size[X] + x];
#else
  return be32toh(self->data[OLD][(y + self->boundarysize[Y]) * (self->size[X] + 2*self->boundarysize[X]) + self->boundarysize[X] + x]);
#endif
}

#ifdef MPI
/* Converts a host float to big endian 32bit.
 */
static int hftobe32(float f) {
  union  {
    float         f;
    unsigned int  i;
  } in;

  in.f = f;
  return htobe32(in.i);
}

/* Converts a big endian 32bit to host float.
 */
static float be32tohf(int f) {
  union  {
    float         f;
    unsigned int  i;
  } out;

  out.i = be32toh(f);
  return out.f;
}
#endif

void Field_setCell_f(Field* self, int x, int y, float value) {
#ifndef MPI
  ((float*)self->data[NEW])[y * self->size[X] + x] = value;
#else
  self->data[NEW][(y + self->boundarysize[Y]) * (self->size[X] + 2*self->boundarysize[X]) + self->boundarysize[X] + x] = value;
#endif
}

float Field_getCell_f(Field* self, int x, int y) {
#ifndef MPI
  return ((float*)self->data[OLD])[y * self->size[X] + x];
#else
  return self->data[OLD][(y + self->boundarysize[Y]) * (self->size[X] + 2*self->boundarysize[X]) + self->boundarysize[X] + x];
#endif
}

int Field_getSizeInDim(Field* self, int dim) {
  return self->size[dim];
}

int Field_getBoundarysizeInDim(Field* self, int dim) {
  return self->boundarysize[dim];
}

int Field_getInnerSizeInDim(Field* self, int dim) {
#ifndef MPI
  return Field_getSizeInDim(self, dim) - 2 * Field_getBoundarysizeInDim(self, dim);
#else
  return Field_getSizeInDim(self, dim) * Field_getBoundarysizeInDim(self, dim);
#endif
}

int* Field_getTimeStepData(Field* self, int step) {
  return self->data[step];
}
