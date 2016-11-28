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
 */
void MPIData_init(MPIData* self, SimData* simdata, int px, int py) {

  // px : nProcesses in x direction, y analogous

    /* NOTE: Define additional variables in the MPIData structure as needed (mpidata.h), so it can be used in other functions.
      */
    /* Number of processes in the vertical and horizontal dimensions of process grid.
     */

    self->reorder = 0;

    self->ndims = 2; // x and y dimension

    self->dims[X] = px;
    self->dims[Y] = py;
    self->num_tasks = px*py;

    /* Number of rows and colums in the local array.
     */
    //
    simdata->lsizes[X] = simdata->gsizes[X] / self->dims[X];
    simdata->lsizes[Y] = simdata->gsizes[Y] / self->dims[Y];
    /* Set periodic behavior.
     */
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

    printf("Hello from process with rank %i\n", self->rank_global);

    /* Create two new groups, one group of all processes and one without the master
     * process.
     *
     * After that a new communicator of all workers is created.
     */
     if (self->rank_global == 0) {
       self->is_master = true;
       //return; // returning immediately leads to Unexpected behavior
     } else {
       self->is_master = false;
     }


    MPI_Comm_group(MPI_COMM_WORLD, &self->all_group);
    static int ranks[] = {0};
    MPI_Group_excl(self->all_group, 1, ranks, &self->allworkers_group); // exclude master process

    int groupsize_worker, groupsize_all;
    MPI_Group_size(self->all_group, &groupsize_all);
    MPI_Group_size(self->allworkers_group, &groupsize_worker);

    printf("worker group size = %i, all group size = %i\n",groupsize_worker,groupsize_all);

    MPI_Comm_create(MPI_COMM_WORLD, self->allworkers_group, &self->commslave);



    if (!self->is_master){ // only non-Master processes

      /* Create a new cartesian communicator of the worker communicator and get informations.
       */
      MPI_Comm_rank(self->commslave,&self->rank);


      MPI_Cart_create(self->commslave,self->ndims, self->dims,self->periods, self->reorder, &self->comm_cart);
      MPI_Cart_coords(self->comm_cart, self->rank, 2, &self->coords);
      MPI_Cart_rank(self->comm_cart, self->coords, &self->rank);

      printf("my Carthesian Communicator rank is %i and my coords are %i,%i\n", self->rank,self->coords[0],self->coords[1]);


      /* Global indices of the first element of the local array.
       */
      self->start_indices[0] = self->coords[0] * simdata->lsizes[0];
      self->start_indices[1] = self->coords[1] * simdata->lsizes[1];

      /* Create a derived datatype that describes the layout of the local array in the memory buffer that includes the ghost area. This is another subarray datatype!
       */
      printf("creating subarray with start_indices = %i,%i... gsizes = %i %i .... lsizes = %i %i .... \n",self->start_indices[0],self->start_indices[1],simdata->gsizes[0],simdata->gsizes[1],simdata->lsizes[0],simdata->lsizes[1] );


      MPI_Type_create_subarray(2, simdata->gsizes, simdata->lsizes, self->start_indices,MPI_ORDER_C, MPI_INT, &self->localarray_type);
      MPI_Type_commit(&self->localarray_type);


      /* Set the position of the start indices to (1 1)^T
       */ //TODO: korrekt???
      // self->start_indices[0] = 0; // aus Folien S. 187ff
      // self->start_indices[1] = 0;

      MPI_Type_create_subarray(2, simdata->gsizes, simdata->lsizes, self->start_indices,MPI_ORDER_C, MPI_INT, &self->printoutarray_type);
      MPI_Type_commit(&self->printoutarray_type);


      /* Indices of the first element of the local array in the allocated array.
       */
      // implement: ??

      // veronika: Leer lassen


      /* Init VTK image data.
       */
      VTK_init(&self->vtk, simdata->output);
      VTK_setDimensions(&self->vtk, simdata->gsizes);

      /* Init all ghost layers.
       */
      // TODO: implement mit 8x ghost layer oder 4x?
      // mit create subarray und dann start_indices richtig verwenden





    }

}

static MPI_Offset MPIData_writeFileHeader(MPIData* self, SimData* simdata, bool float_values) {
  char   header[2048];
  size_t len;

  len = float_values ? VTK_getHeader_f(&self->vtk, simdata->timestep, header, simdata->timesteps)
                     : VTK_getHeader(&self->vtk, simdata->timestep, header, simdata->timesteps);


  // Write the header to the file.
  printf("writing file header with process %i \n", self->rank);
  MPI_File_write(self->fh,header,len,MPI_FLOAT,&self->status);

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
  printf("filename: %s\n", filename);

  /* Create a new file handle for collective I/O
   */
  MPI_File_open(self->commslave, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &self->fh);

  // /* Write the file header with ONE process
  //  */
  MPI_Barrier(self->commslave);
  offset = MPIData_writeFileHeader(self, simdata, float_values);

  /* Set the file view for the file handle using collective I/O
   */
   offset = 188;
  //  offset = 0;

   int local_array_size = simdata->lsizes[X] * simdata->lsizes[Y];
   printf("I am process %i and my offset is %i\n",self->rank,offset + self->rank*local_array_size*sizeof(int) );
  //
  rc = MPI_File_set_view(self->fh, offset ,MPI_INT,self->localarray_type,"native", MPI_INFO_NULL); // vers 02

  if (rc != MPI_SUCCESS) {
    MPI_Error_string(rc, buffer, &len);
    printf("ERROR in line %d: %s\n", __LINE__, buffer);
  }


  int x,y;

  int local_array_int[4*simdata->gsizes[X]*simdata->gsizes[Y]];
  int hh = field->size[Y];
  int ww = field->size[X];


  int ii = 0;
  printf("local_array_int[i] == \n" );
  for (y = -1; y < (hh+1); y++) {
    for (x = -1; x < (ww+1); x++) {
      for(int i = 0; i < self->num_tasks; i++){
        local_array_int[ii] = Field_getCell(field,x,y);
        printf("%i ",local_array_int[ii] );
        ii++;
      }
      printf("\n");

    }
    printf("\n\n\n\n");
  }




  /* Write the data using collective I/O
   */
  // pointer auf feld statt buffer variable
  printf("writing file with process %i\n", self->rank);

  rc = MPI_File_write_all(self->fh, local_array_int,1,self->printoutarray_type,&self->status); // vers 02

  if (rc != MPI_SUCCESS) {
    MPI_Error_string(rc, buffer, &len);
    printf("ERROR in %d: %s\n", __LINE__, buffer);
  }

  /* Close the file handle.
   */
   MPI_File_close( &self->fh ); // close with file handle



   // write to console for debugging purposes:

  //  printf("field for process with rank %i:\n", self->rank);
//    for (y = -1; y < (hh+1); y++) {
//      for (x = -1; x < (ww+1); x++) {
//        printf("%i  ",Field_getCell(field,x,y));
//      }
//      printf("\n");
//    }
//
//    printf("\n\n\n\n");
}

void MPIData_exchangeGhostLayer(MPIData* self, Field* field) {
  // TODO: implement ghost layer exchange.
  // point to point communication to and from the eight neighbors?

  // MPI_sendrecv ?
  // use MPI_Cart_shift (direction ...) to get neighbor process ranks

  // benutze vorgefertigte filetypes zum Austauschen der Randlayer

}

void MPIData_deinit(MPIData* self) {
  // Deinit all objects and free the memory.
  VTK_deinit(&self->vtk);
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
