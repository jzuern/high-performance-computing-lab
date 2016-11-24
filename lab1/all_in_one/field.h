/* High performance computing - Field class declararion
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#ifndef FIELD_H_
#define FIELD_H_

#define X   0 // The x direction.
#define Y   1 // The y direction.

#define OLD 0 // timestep i
#define NEW 1 // timestep i+1

/* The field class.
 *
 * In general it is possible to define an inner start-vector
 * in order to define the position of the inner subfield.
 *
 * +---------+
 * |         |
 * | +-----+ |
 * | |     | |
 * | |     | |
 * | |     | |
 * | +-----+ |
 * |         |
 * +---------+
 *
 * The data attribute holds two fields and aligns the value bytes
 * in big endian 32 bit.
 */
typedef struct Field {
  int  size[2];  // The size of the field in X/Y direction
  int  boundarysize[2]; // size of the boundary
  int *data[2];  // Fields for timestep i and timestep i+1
} Field;

/* Initialize the field object.
 */
void Field_init(Field* self, int size[2], int boundarysize[2]);

/* Deinitialize the field object.
 */
void Field_deinit(Field* self);

/* Set a value at the given position.
 */
void Field_setCell(Field* self, int x, int y, int value);

/* Get a value from the given position.
 */
int Field_getCell(Field* self, int x, int y);

/* Set a float value at the given position.
 */
void Field_setCell_f(Field* self, int x, int y, float value);

/* Get a float value from the given position.
 */
float Field_getCell_f(Field* self, int x, int y);

/* Swap timestep i with timestep i+1.
 */
void Field_swap(Field* self);

/* Get the size of the field in given direction.
 * The direction has to be either X or Y.
 *
 * see: The X, Y macro
 */
int Field_getSizeInDim(Field* self, int dim);

/* Get the start position in the given direction.
 */
int Field_getBoundarysizeInDim(Field* self, int dim);

/* Get inner size in the given direction.
 */
int Field_getInnerSizeInDim(Field* self, int dim);

/* Get the data of the current timestep.
 */
int* Field_getTimeStepData(Field* self, int step);

void Field_printField(Field *self);

#endif
