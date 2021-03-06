/* High performance computing exercise course main function
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

#ifdef MPI
#include "mpidata.h"
#endif

#include "sweeps/gameoflife.h"
#include "sweeps/add.h"

typedef void (*Sweep_func)(Field*, SimData*, MPIData*);

typedef struct Sweep_s {
  Sweep_func   exec;
  char*        name;
  char*        description;
} Sweep;


Sweep sweeps[] = {
    {add_sweep,        "Add",         "Adds just the center cell"},
    {gameoflife_sweep, "Gameoflife",  "runs game of life"},

};

/* The solver main loop in two variants, with and without MPI.
 */
int main(int argc, char** argv) {
  SimData simdata;
  MPIData mpidata;
  Field   field;
  struct timeval t1, t2;
  double elapsedTime;

  MPI_Init(&argc, &argv);// initialize MPI

  // 1. Initialize the simulation data. (see simdata.h)
  SimData_init(&simdata, argc, argv);


  // 2. Initialize the mpi data. (see mpidata.h/mpidata.c)
  // number of processes in x and y direction
  int px = 2;
  int py = 2;
  MPIData_init(&mpidata, &simdata, px, py);

  if (!mpidata.is_master) {

    printf("Hello from slave process with rank %i\n",mpidata.rank_global);
    // 3. Initialize the field. (see field.h)
    int boundarysize[2] = {1,1};


    Field_init(&field, simdata.lsizes, boundarysize);
    // printf("simdata.lsizes = %i %i\n",simdata.lsizes[X],simdata.lsizes[Y] );
    // printf("field.sizes = %i %i\n",field.size[X],field.size[Y] );


    srand(mpidata.rank);

    // 4. Use a filling.
    if(mpidata.rank == 0) Filling_produceRunner(&field, ALIVE  , 2,2);

    // 5. Exchange the ghost layers.
    MPIData_exchangeGhostLayer(&mpidata, &field);

    // 6. Swap the fields.
    Field_swap(&field);

    // Start timer.
    gettimeofday(&t1, NULL);


    // 7. For each timestep
    SIMDATA_FOR_EACH_TIMESTEP(simdata)
    {
      // 5.1 Call the iterator
      sweeps[simdata.sweep_num].exec(&field, &simdata, &mpidata);

        // 7.2 Exchange the ghost layers.
        MPIData_exchangeGhostLayer(&mpidata, &field);

        // 7.3 Swap the fields
        Field_swap(&field);

        // 7.4 Write the data. (see mpidata.h/mpidata.c)
        MPIData_writeTimeStep(&mpidata, &simdata, &field, false);

        MPI_Barrier(mpidata.commslave);
        fflush(stdout);
    }

    // printf("AFTER SIMLOOP with rank %i\n",mpidata.rank_global);

    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms


    MPI_Barrier(mpidata.commslave);fflush(stdout);


    if (1 == mpidata.rank_global) {
      printf("Time elapsed in ms is: %f\n", elapsedTime);
    }

    // 8. Call the deinit functions and free all allocated memory.
    Field_deinit(&field);

    /* Send abort message
     */
    if (1 == mpidata.rank_global) {
      long buffer = 0;
      MPI_Send(&buffer, 1, MPI_LONG, 0, 2, MPI_COMM_WORLD);
    }
  } else { // master process
    printf("[MASTER] with rank: %d is waiting...\n", mpidata.rank_global);
    MPI_Status status;
    char       buffer[4096];
    int        count;
    bool       running = true;

#ifdef __SSE__
    printf("\033[1;31mUsing SSE\033[0m\n");
#endif

    while (running) { // wait for all processes to terminate (?)
      // probe and wait for any MPI message
      MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      switch (status.MPI_TAG) {
        case 1:
          MPI_Get_count(&status, MPI_CHAR, &count);
          MPI_Recv(buffer, count, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
          break;
        case 2:
          running = false;
          break;
        default:
          fprintf(stderr, "[MASTER]: Unexpected MPI_TAG received (%d).\n", status.MPI_TAG);
          running = false;
      }
    }


  // 6. Call the deinit functions and free all allocated memory.
    SimData_deinit(&simdata);
    MPIData_deinit(&mpidata);

  }

#ifdef MPI
  MPI_Finalize();
#endif

  return EXIT_SUCCESS;
}
