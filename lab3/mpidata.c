/* High performance computing - Implementation of MPI specific parts
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Constantin Heisler
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <endian.h>
#include <string.h>
#include <mpi.h>
#include "mpidata.h"

/* Initialisation of the MPIData structure.
 *
 * TODO: implement this function.
 */
void MPIData_init(MPIData* self, SimData* simdata, int px, int py) {
  // px : nProcesses in x direction, y analogous

  /* NOTE: Define additional variables in the MPIData structure as needed (mpidata.h), so it can be used in other functions.
    */

  /* Number of processes in the vertical and horizontal dimensions of process grid.
   */
  //
  self->dims[X] = px;
  self->dims[Y] = py;

  self->num_tasks = px*py;

  /* Number of rows and colums in the local array.
   */
  //
  simdata->lsizes[X] = simdata->gsizes[X] / dims[X];
  simdata->lsizes[Y] = simdata->gsizes[Y] / dims[Y];


  /* Set periodic behavior.
   */
  //
  self->periods[X] = 1;
  self->periods[Y] = 1;


  /* Get the global rank
   */
  MPI_Comm_rank(MPI_COMM_WORLD, &self->rank_global);
  MPI_Comm_size(MPI_COMM_WORLD, &self->num_tasks_global);

  /* Abort if the number of processes does not match with the given configuration.
   */
  if (self->num_tasks_global != (px*py+1)) {
    if (self->rank_global == 0) {
      fprintf(stderr, "ERROR: %d MPI processes needed.\n", px*py+1);
    }
    MPI_Finalize();
    exit(EXIT_FAILURE);
  }

  /* Create two new groups, one group of all processes and one without the master
   * process.
   *
   * After that a new communicator of all workers is created.
   */
  // TODO

  if (self->rank_global == 0) {
    self->is_master = true;
    return;
  } else {
    self->is_master = false;
  }


    MPI_Group_incl(MPI_GROUP_WORLD, self->num_tasks_global , , self->all_group)
    MPI_Group_excl(self->all_group,)

  /* Create a new cartesian communicator of the worker communicator and get informations.
   */
  // TODO

  /* Global indices of the first element of the local array.
   */
  // TODO

  /* Create a derived datatype that describes the layout of the local array in the memory buffer that includes
   * the ghost area. This is another subarray datatype!
   */
  // TODO

  /* Set the position of the start indices to (1 1)^T
   */
  // TODO

  /* Indices of the first element of the local array in the allocated array.
   */
  // TODO

  /* Init VTK image data.
   */
  VTK_init(&self->vtk, simdata->output);
  VTK_setDimensions(&self->vtk, simdata->gsizes);

  /* Init all ghost layers.
   */
  // TODO
}

static MPI_Offset MPIData_writeFileHeader(MPIData* self, SimData* simdata, bool float_values) {
  char   header[2048];
  size_t len;

  len = float_values ? VTK_getHeader_f(&self->vtk, simdata->timestep, header, simdata->timesteps)
                     : VTK_getHeader(&self->vtk, simdata->timestep, header, simdata->timesteps);

  // TODO: Write the header to the file.

  return (MPI_Offset)len;
}

void MPIData_writeTimeStep(MPIData* self, SimData* simdata, Field* field, bool float_values) {
  char filename[2048];  // The generated file name
  MPI_Offset offset;
  // The following declarations are for error handling
  int        rc;
  char       buffer[MPI_MAX_ERROR_STRING];
  int        len;

  VTK_getFileName(&self->vtk, simdata->timestep, filename, sizeof(filename));

  /* Create a new file handle for collective I/O
   */
  // TODO

  /* Write the file header
   */
  offset = MPIData_writeFileHeader(self, simdata, float_values);

  /* Set the file view for the file handle using collective I/O
   * TODO
   */
  // rc = ...

  if (rc != MPI_SUCCESS) {
    MPI_Error_string(rc, buffer, &len);
    printf("ERROR in line %d: %s\n", __LINE__, buffer);
  }

  /* Write the data using collective I/O
   * TODO
   */
  // rc = ...

  if (rc != MPI_SUCCESS) {
    MPI_Error_string(rc, buffer, &len);
    printf("ERROR in %d: %s\n", __LINE__, buffer);
  }

  /* Close the file handle.
   * TODO
   */
}

void MPIData_exchangeGhostLayer(MPIData* self, Field* field) {
  // TODO: implement ghost layer exchange.
}

void MPIData_deinit(MPIData* self) {
  // Deinit all objects and free the memory.
  VTK_deinit(&self->vtk);

  // TODO
}

/* Use the master in order to write console output.
 *
 * NOTE: By convention the master has to have the rank 0.
 */
void MPI_printf(const char *fmt, ...) {
  // Guess we need no more than 4096 bytes.
  int      n, size = 4096;
  char    *p, *np;
  va_list  ap;
  int      rc;
  char     buffer[MPI_MAX_ERROR_STRING];
  int      len;

  if ((p = malloc(size * sizeof(char))) == NULL) {
    MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TAG);
  }

  while (true) {
    // Try to print in the allocated space.
    va_start(ap, fmt);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    // If that worked, send the string.
    if (n > -1 && n < size) {
      if ((rc = MPI_Send( p, strlen(p)+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD)) != MPI_SUCCESS) {
        MPI_Error_string(rc, buffer, &len);
        printf("ERROR in %s: %s\n", __FUNCTION__, buffer);
      }
      break;
    }

    // Else try again with more space.
    if (n > -1) {    // glibc 2.1
      size = n+1;    // precisely what is needed
    } else {         // glibc 2.0
      size *= 2;     // twice the old size
    }

    if ((np = realloc(p, size)) == NULL) {
      free(p);
      MPI_Abort(MPI_COMM_WORLD, MPI_ERR_TAG);
    } else {
      p = np;
    }
  }
}