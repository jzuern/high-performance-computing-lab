/* High performance computing - Simulation data
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#ifndef SIMDATA_H_
#define SIMDATA_H_

#include "vtk.h"

/* A loop macro in order to iterate over each time step.
 */
#define SIMDATA_FOR_EACH_TIMESTEP(sd) \
  for ((sd).timestep = 0; (sd).timestep < (sd).timesteps; (sd).timestep++)

/* The SimData class represents global simulation data.
 */
typedef struct SimData {
  int   blocks[2];  // number of blocks in X/Y-direction
  int   lsizes[2];  // local number of cells in X/Y-direction
  int   gsizes[2];  // global number of cells in X/Y-direction
  int  timesteps;  // number of frames
  int  timestep;   // The current timestep
  int  sweep_num;  // The selected sweep
  char *output;     // name of the output file
#ifndef MPI
  VTK   vtk;        // The VTK output image
#endif
} SimData;

/* Initializes the simdata object and reads the command line arguments.
 *
 * argc              The argument count from the main-function.
 * argv              The arguments from the main-function.
 */
void SimData_init(SimData* simdata, int argc, char** argv);

/* Deinitialize the simdata object.
 */
void SimData_deinit(SimData* simdata);

#ifndef MPI
/* Write a time step to the file system.
 *
 * simdata            The current SimData object.
 * field              The Field object to write
 * float_values       If true, the data is written as floats, otherwise as integer.
 */
void SimData_writeTimeStep(SimData* simdata, Field* field, bool float_values);
#endif

#endif
