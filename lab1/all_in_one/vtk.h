/* High performance computing - VTK file format interface
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#ifndef VTK_H_
#define VTK_H_

#include <stdio.h>
#include <stdbool.h>


/* VTK image class
 */
typedef struct VTK {
  long  size[2];  // The size of the image in X/Y direction.
  char *prefix;   // The prefix of the image file name.
  FILE *fp;       // The current file handle.
} VTK;

/* Initialize the VTK image object
 */
VTK* VTK_init(VTK* self, char* prefix);

/* Free the VTK image object
 */
void VTK_deinit(VTK* self);

/* Generates the header for integer values and returns it in a new allocated memory.
 *
 * self       The VTK image object.
 * frame      The current image frame number.
 * header     A pointer which will contain the size of the new allocated memory.
 * framecount The number of frames.
 *
 * Returns the size of the header.
 */
size_t VTK_getHeader(VTK* self, long frame, char header[2048], long framecount);

/* Generates the header for float values and returns it in a new allocated memory.
 *
 * self       The VTK image object.
 * frame      The current image frame number.
 * header     A pointer which will contain the size of the new allocated memory.
 * framecount The number of frames.
 *
 * Returns the size of the header.
 */
size_t VTK_getHeader_f(VTK* self, long frame, char header[2048], long framecount);

/* Generates the filename for the given frame according to the following pattern: "%s-%05ld%s".
 *
 * The first part is the prefix, then the frame sequence (5 digits) is added and after that ".vtk" is added.
 * The size of the buffer should be at least equal to the size of the generated filename.
 *
 * self       The VTK image object.
 * frame      The current frame.
 * buffer     A properly sized buffer to hold the file name.
 * len        The length of the buffer to hold the file name.
 */
char* VTK_getFileName(VTK* self, long frame, char* buffer, size_t len);

/* Open a file by its frame-number and the initialized pattern
 *
 * frame      The frame to open.
 * mode       A file mode like "r", or "w".
 */
void VTK_open(VTK* self, long frame, const char* mode);

/* Open a file by its file name.
 *
 * filename   The file name to open.
 * mode       A file mode like "r", or "w".
 */
void VTK_openFileName(VTK* self, char* filename, const char* mode);

/* Close the current file handle.
 */
void VTK_close(VTK* self);

/* Check the file type by comparing the first line with the VTK file type.
 *
 * return true on success, false otherwise.
 */
bool VTK_checkFileType(VTK* self);

/* Getter for size in X/Y (width/height or colums/rows) direction.
 */
long* VTK_getDimensions(VTK* self);

/* Setter for size in X/Y (width/height or colums/rows) direction.
 */
void VTK_setDimensions(VTK* self, int size[2]);

/* Reads the dimensions of the file.
 */
void VTK_readDimensions(VTK* self);

/* Reads the number of values in the file.
 */
long VTK_readSize(VTK* self);

/* Reads the header of the VTK file until the data region has been reached.
 */
void VTK_finishHeader(VTK* self);

/* Reads a row from a given VTK file.
 *
 * data:     The buffer for exactly one row.
 */
void VTK_readDataRow(VTK* self, int* data);

/* Write a given buffer into a VTK file.
 *
 * data:     The buffer to write.
 * filename: The name of the file to write.
 */
void VTK_writeHeader(VTK* self, long frame, long framecount, bool float_values);

/* Writes a full frame.
 *
 * data:     The buffer to write.
 */
void VTK_writeData(VTK* self, int* data);


#endif
