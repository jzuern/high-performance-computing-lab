/* High performance computing - Field class implementation
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#include <stdlib.h>
#include <endian.h>
#include "field.h"
#include "simdata.h"


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


void Field_printField(Field *self){

  int nX = self->size[X];
  int nY = self->size[Y];

  printf("Field:\n\n\n\n");
  for (int i = 0; i < nX; i++){
    for (int j = 0; j < nY; j++){
      if(Field_getCell(self,i,j)) printf("X");
      else{
        printf("O");
      }
    }
    printf("\n");
  }
}



void Field_write_vtk_custom(SimData *simdata, Field *field) {

  printf("in write_vtk\n\n\n");

  char filename[2048];
  int x,y;

  long offsetX=0;
  long offsetY=0;
  float deltax=1.0;
  float deltay=1.0;

  int w = field->size[X];
  int h = field->size[Y];

  char prefix[] = "out";

  int  nxy = w * h * sizeof(int);

  snprintf(filename, sizeof(filename), "%s-%05ld%s", prefix, simdata->timestep, ".vti");
  FILE* fp = fopen(filename, "w");

  fprintf(fp, "<?xml version=\"1.0\"?>\n");
  fprintf(fp, "<VTKFile type=\"ImageData\" version=\"0.1\" byte_order=\"LittleEndian\" header_type=\"UInt64\">\n");
  fprintf(fp, "<ImageData WholeExtent=\"%d %d %d %d %d %d\" Origin=\"0 0 0\" Spacing=\"%le %le %le\">\n", offsetX, offsetX + w-1, offsetY, offsetY + h-1, 0, 0, deltax, deltax, 0.0);
  fprintf(fp, "<CellData Scalars=\"%s\">\n", prefix);
  // fprintf(fp, "<DataArray type=\"Float32\" Name=\"%s\" format=\"appended\" offset=\"0\"/>\n", prefix);
  fprintf(fp, "<DataArray type=\"Int32\" Name=\"%s\" format=\"appended\" offset=\"0\"/>\n", prefix);

  fprintf(fp, "</CellData>\n");
  fprintf(fp, "</ImageData>\n");
  fprintf(fp, "<AppendedData encoding=\"raw\">\n");
  fprintf(fp, "_");
  fwrite((unsigned char*)&nxy, sizeof(int), 1, fp);
  // fwrite(nxy, sizeof(int), 1, fp);



  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {
      int value = Field_getCell(field,y,x);
      fwrite((unsigned char*)&value, sizeof(int), 1, fp);
    }
  }

  fprintf(fp, "\n</AppendedData>\n");
  fprintf(fp, "</VTKFile>\n");
  fclose(fp);
}
