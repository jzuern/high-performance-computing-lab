/* High performance computing exercise course solver loop.
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>

#include "field.h"
#include "filling.h"
#include "simdata.h"

#include "sweeps/gameoflife.h"
#include "sweeps/add.h"

typedef void (*Sweep_func)(Field*, SimData*);

typedef struct Sweep_s {
  Sweep_func   exec;
  char*        name;
  char*        description;
} Sweep;


Sweep sweeps[] = {
    {add_sweep,        "Add",         "Adds just the center cell"},
    {gameoflife_sweep, "Gameoflife",  "runs game of life"},
//TODO add other sweeps and optimizaion verisons
};

/* The solver main loop.
 */
int main(int argc, char** argv) {


  SimData  simdata;
  Field    field;
  struct timeval t1, t2;
  double elapsedTime;

  // 1. Initialize the simulation data. (see simdata.h)
  SimData_init(&simdata, argc, argv);

  // 2. Initialize the simulation field. (see field.h)
  //    Note: use the sizes from simdata structure
  //    Note2: boundary sizes: (x,y) (1,1)
  int boundary_size[2] = {1,1};
  Field_init(&field, simdata.gsizes, boundary_size);

  // 3. Use a filling. (see filling.h)
  Filling_binaryRandomizeFrame(&field, ALIVE);

  // 4. Swap the fields.
  Field_swap(&field);

  Field_printField(&field); // print the field


#ifdef _OPENMP
  printf("using: \033[1;31mOpenMP\033[0m\n");
#endif
  printf("using sweep: \033[1;31m%s\033[0m\ndescription: %s\n", sweeps[simdata.sweep_num].name , sweeps[simdata.sweep_num].description);
  printf("blocksX: %u, blocksY: %u\n", simdata.blocks[X], simdata.blocks[X]);
  printf("blockSizeX: %u, blockSizeY: %u\n", simdata.lsizes[X], simdata.lsizes[X]);
  printf("globalSizeX: %u, globalSizeY: %u\n", simdata.gsizes[X], simdata.gsizes[X]);

  // Start timer.
  gettimeofday(&t1, NULL);



  // 3. execute each timestep
  SIMDATA_FOR_EACH_TIMESTEP(simdata) // dont write anything but the loop hereafter
  // 5. For each timestep
  {
    // 5.1 Call the iterator
    sweeps[simdata.sweep_num].exec(&field, &simdata);

    // 5.2 Swap the fields.
    Field_swap(&field);

    // 5.3 Write the data with the SimData class.
    Field_write_vtk_custom(&simdata, &field); // custom VTK write function


    // 5.4 add periodic boundary conditions for each side
    // apply_periodic_boundary(&field); //jzuern: this is in gameoflife_sweep



    Field_printField(&field); //debugging

  }

  gettimeofday(&t2, NULL);
  elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
  elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
  printf("Time elapsed in ms is: %f\n", elapsedTime);


  // 6. Call the deinit functions and free all allocated memory.
  Field_deinit(&field);

  return EXIT_SUCCESS;
}
