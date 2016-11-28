/* High performance computing - Simulation data
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "field.h"
#include "simdata.h"

void SimData_init(SimData* simdata, int argc, char** argv) {
  size_t len;

  if (argc != 8) {
    fprintf(stderr, "ERROR: not enough arguments given.\n\n");
#ifndef MPI
    printf("usage: %s <sweep> <x blocks> <y blocks> <x block size> <y block size> <number of timesteps> <output>\n", argv[0]);
    printf("example: %s 0 2 2 512 512 2  out\n", argv[0]);
#else
    printf("usage: mpirun -np <number> %s <sweep> <x blocks> <y blocks> <x block size> <y block size> <number of timesteps> <output>\n", argv[0]);
    printf("example: mpirun -np 4 %s 0 2 2 512 512 2 out\n", argv[0]);
#endif
    printf("- produces two timesteps of size 1024x1024 with sweep 0 and writes the result to \"out_<frame>.vtk\".\n");

    exit(EXIT_FAILURE);
  }

  simdata->sweep_num = atoi(argv[1]);
  simdata->blocks[X] = atoi(argv[2]);
  simdata->blocks[Y] = atoi(argv[3]);
  simdata->lsizes[X] = atoi(argv[4]); // local array
  simdata->lsizes[Y] = atoi(argv[5]);
  simdata->timesteps = atoi(argv[6]);
  simdata->timestep  = 0;

  simdata->gsizes[X] = simdata->lsizes[X] * simdata->blocks[X];
  simdata->gsizes[Y] = simdata->lsizes[Y] * simdata->blocks[Y];

  printf(" in simdata.c: simdata->lsizes[X] = %i\n", simdata->lsizes[X]);
  printf(" in simdata.c: simdata->gsizes[X] = %i\n", simdata->gsizes[X]);


  // Copy the filename
  len = strlen(argv[7]) + 1;
  simdata->output    = calloc(len, sizeof(char));
  strncpy(simdata->output, argv[7], len);

#ifndef MPI
  VTK_init(&simdata->vtk, simdata->output);
  VTK_setDimensions(&simdata->vtk, simdata->gsizes);
#endif
}

void SimData_deinit(SimData* simdata) {
  free(simdata->output);
#ifndef MPI
  VTK_deinit(&simdata->vtk);
#endif
}

#ifndef MPI
void SimData_writeTimeStep(SimData* simdata, Field* field, bool float_values) {
  VTK_open(&simdata->vtk, simdata->timestep, "w");
  printf("Writing timestep %ld...\n", simdata->timestep);
  VTK_writeHeader(&simdata->vtk, simdata->timestep, simdata->timesteps, float_values);
  VTK_writeData(&simdata->vtk, Field_getTimeStepData(field, OLD));
  VTK_close(&simdata->vtk);
}
#endif
