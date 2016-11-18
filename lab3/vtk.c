/* High performance computing - VTK file format interface
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <endian.h>
#include <math.h>

#include "vtk.h"

#define X   0 // The x direction.
#define Y   1 // The y direction.

VTK* VTK_init(VTK* self, char* prefix) {
  const size_t len = strlen(prefix);
  self->prefix = calloc(len + 1, sizeof(char));
  memcpy(self->prefix, prefix, len*sizeof(char));
  self->fp = NULL;
  return self;
}

void VTK_deinit(VTK* self) {
  free(self->prefix);
  VTK_close(self);
}

void VTK_setDimensions(VTK* self, int size[2]) {
  self->size[X] = size[X];
  self->size[Y] = size[Y];
}

long* VTK_getDimensions(VTK* self) {
  return self->size;
}

void VTK_readDimensions(VTK* self) {
  char        line[2048];
  const char  s[]        = "DIMENSIONS";
  char       *p;
  
  while (fgets(line, sizeof(line), self->fp) != NULL) {
    if (strncmp(line, s, sizeof(s) - 1) == 0) {
      self->size[0] = strtol(line + sizeof(s), &p, 10);
      self->size[1] = strtol(p, NULL, 10);
      break;
    }
  }
}

long VTK_readSize(VTK* self) {
  char       line[2048];
  const char s[]        = "POINT_DATA";
  long       result     = -1;
  
  while (fgets(line, sizeof(line), self->fp) != NULL) {
    if (strncmp(line, s, sizeof(s) - 1) == 0) {
      result = strtol(line + sizeof(s), NULL, 10);
      break;
    }
  }
  
  return result;
}

void VTK_finishHeader(VTK* self) {
  char       line[2048];
  const char s[] = "LOOKUP_TABLE default\n";

  // Jump over the file header
  while (fgets(line, sizeof(line), self->fp) != NULL) {
    if (strcmp(line, s) == 0) {
      break;
    }
  }
}

void VTK_readDataRow(VTK* self, int* data) {
  long i;
  
  // read the file content
  if (self->size[0] != fread(data, sizeof(int), self->size[0], self->fp)) {
    fprintf(stderr, "%s: Unable to read a full data row from file\n", __FUNCTION__);
    exit(EXIT_FAILURE);
  }

  // convert big endian to host
  for (i = 0; i < self->size[0]; i++) {
    data[i] = be32toh(data[i]);
  }
}

char* VTK_getFileName(VTK* self, long frame, char* buffer, size_t len) {
  snprintf(buffer, len, "%s-%05ld%s", self->prefix, frame, ".vtk");
  return buffer;
}

void VTK_open(VTK* self, long frame, const char* mode) {
  char filename[2048];
  
  VTK_getFileName(self, frame, filename, sizeof(filename));
  VTK_openFileName(self, filename, mode);
}

void VTK_openFileName(VTK* self, char* filename, const char* mode) {
  if ((self->fp = fopen(filename, mode)) == NULL) {
    fprintf(stderr, "%s: Unable to open file (%s).\n", __FUNCTION__, filename);
    exit(EXIT_FAILURE);
  }
}

bool VTK_checkFileType(VTK* self) {
  char line[2048];
  const char s[] = "# vtk DataFile Version 3.0\n";
  
  if (fgets(line, sizeof(line), self->fp) == NULL) {
    fprintf(stderr, "%s: Unable to read from current file handle.\n", __FUNCTION__);
    exit(EXIT_FAILURE);
  }
  
  return strcmp(line, s) == 0;
}

void VTK_close(VTK* self) {
  if (self->fp != NULL) {
    fclose(self->fp);
    self->fp = NULL;
  }
}

void VTK_writeHeader(VTK* self, long frame, long framecount, bool float_values) {
  char   header[2048];
  size_t len;

  len = float_values ? VTK_getHeader_f(self, frame, header, framecount)
                     : VTK_getHeader(self, frame, header, framecount);

  if (fwrite(header, sizeof(char), len, self->fp) != len) {
    fprintf(stderr, "%s: Unable to write header.\n", __FUNCTION__);
    exit(EXIT_FAILURE);
  }
}

void VTK_writeData(VTK* self, int* data) {
  int         value;
  long        i;
  const long  nxy = self->size[0] * self->size[1];
  
  for (i = 0; i < nxy; i++) {
    value = htobe32(data[i]);
    fwrite(&value, sizeof(int), 1, self->fp);
  }
}

static size_t VTK_getHeader_internal(VTK* self, long frame, char header[2048], long framecount, bool float_values) {
  char   buffer[1024];

  header[0] = '\0';
  strcat(header, "# vtk DataFile Version 3.0\n");
  snprintf(buffer, sizeof(buffer), "Simulation frame %ld of %ld\n", frame+1, framecount);
  strcat(header, buffer);
  strcat(header, "BINARY\n");
  strcat(header, "DATASET STRUCTURED_POINTS\n");
  snprintf(buffer, sizeof(buffer), "DIMENSIONS %ld %ld 1\n", self->size[0], self->size[1]);
  strcat(header, buffer);
  strcat(header, "SPACING 1.0 1.0 1.0\n");
  strcat(header, "ORIGIN 0 0 0\n");
  snprintf(buffer, sizeof(buffer), "POINT_DATA %ld\n", self->size[0] * self->size[1]);
  strcat(header, buffer);
  strcat(header, float_values ? "SCALARS data float 1\n" : "SCALARS data int 1\n");
  strcat(header, "LOOKUP_TABLE default\n");

  return strlen(header);
}

size_t VTK_getHeader(VTK* self, long frame, char header[2048], long framecount) {
  return VTK_getHeader_internal(self, frame, header, framecount, false);
}

size_t VTK_getHeader_f(VTK* self, long frame, char header[2048], long framecount) {
  return VTK_getHeader_internal(self, frame, header, framecount, true);
}

