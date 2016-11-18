/* High performance computing MPI interface
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Constantin Heisler
 */

#ifndef MPI_DATA_H_
#define MPI_DATA_H_

#include <mpi.h>
#include "vtk.h"
#include "field.h"
#include "simdata.h"

// Use this macros for the orientation of the ghost layers.
#define TOP    0
#define BOTTOM 1
#define LEFT   2
#define RIGHT  3

/* The MPIData data structure
 */
typedef struct MPIData {
  int           rank_global;         // The current MPI rank in the global communicator.
  int           rank;                // The current MPI rank in the local communicator.
  int           num_tasks;           // The number of processes in the current local communicator
  int           num_tasks_global;    // The global number of processes
  VTK           vtk;                 // VTK writer
  bool          is_master;           // true, if the current rank indicates the master process. (Forced by convention to be rank 0)
  // TODO: Insert more needed class attributes here.

  int ndims; // number of dimensions
  int dims[2]; // number of processes in each dim
  int periods[2] // periodic boundary in each dim
  int reoder;  // ranking may be reordered

  // MPI Zeug

  MPI_Comm comm_old,comm_cart; // MPI communicator
  MPI_Group all_group, allworkers_group;

  int * ranks;



} MPIData;

/* Initialisation of the MPIData structure.
 *
 * self         The MPIData object.
 * simdata      The SimData object.
 * px,py        The number of processes in X-/Y-direction.
 */
void MPIData_init(MPIData* self, SimData* simdata, int px, int py);

/* Writes a time step with MPI parallel I/O to the output file.
 *
 * self         The MPIData object.
 * simdata      The SimData object.
 * field        The field to write.
 * float_values If true, the output is written as float, as integer otherwise.
 */
void MPIData_writeTimeStep(MPIData* self, SimData* simdata, Field* field, bool float_values);

/* Exchange ghost layers.
 *
 * self         The MPIData object.
 * field        The field to write.
 */
void MPIData_exchangeGhostLayer(MPIData* self, Field* field);

/* Deinitialize the MPIData object.
 *
 * self         The MPIData object.
 */
void MPIData_deinit(MPIData* self);

/* Use the master in order to write console output.
 */
void MPI_printf(const char *fmt, ...);

#endif
