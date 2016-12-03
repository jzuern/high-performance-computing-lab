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

    // printf("Hello from process with rank %i\n", self->rank_global);

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
      self->start_indices[0] = self->coords[0] * simdata->lsizes[X];
      self->start_indices[1] = self->coords[1] * simdata->lsizes[Y];

      /* Create a derived datatype that describes the layout of the local array in the memory buffer that includes the ghost area. This is another subarray datatype!
       */
      printf("creating localarray_type with start_indices = %i,%i... gsizes = %i %i .... lsizes = %i %i .... \n",self->start_indices[0],self->start_indices[1],simdata->gsizes[0],simdata->gsizes[1],simdata->lsizes[X],simdata->lsizes[X]);

      MPI_Type_create_subarray(2, simdata->gsizes, simdata->lsizes, self->start_indices,MPI_ORDER_C, MPI_INT, &self->localarray_type);
      MPI_Type_commit(&self->localarray_type);



      // VERSUCH: kopiere einfach alle benötigten einträge des Felds in neues Subarray und
      // schreibe dies in Datei. Zwar doof aber funktioniert immerhin?


      int fileout_lsizes[2];
      fileout_lsizes[0] = simdata->lsizes[0] - 2;
      fileout_lsizes[1] = simdata->lsizes[1] - 2;

      int gsizes_new[2];
      gsizes_new[X] = simdata->gsizes[X] - 2*px;
      gsizes_new[Y] = simdata->gsizes[Y] - 2*py;

      int startindices_[2];
      startindices_[X] = self->coords[X] * fileout_lsizes[X];
      startindices_[Y] = self->coords[Y] * fileout_lsizes[Y];

      MPI_Type_create_subarray(2, gsizes_new, fileout_lsizes, startindices_,MPI_ORDER_C, MPI_INT, &self->fileout_type);
      MPI_Type_commit(&self->fileout_type);



      /* Init VTK image data.
       */
      VTK_init(&self->vtk, simdata->output);
      VTK_setDimensions(&self->vtk, gsizes_new); // version fileout_type
      // VTK_setDimensions(&self->vtk, simdata->gsizes); // version localarray_type



      /* Init all ghost layers.
      /////////////////////////////////////////////////////////////////////////////////
       */

      int startindices[2];
      int f_sizes[2];

      startindices[X] = 0;
      startindices[Y] = 0;
      f_sizes[X] = 1;
      f_sizes[Y] = simdata->lsizes[Y];

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->ghostlayerLeft);
      MPI_Type_commit(&self->ghostlayerLeft);

      startindices[X] = simdata->lsizes[X]-1;
      startindices[Y] = 0;
      f_sizes[X] = 1;
      f_sizes[Y] = simdata->lsizes[Y];

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->ghostlayerRight);
      MPI_Type_commit(&self->ghostlayerRight);

      startindices[X] = 0;
      startindices[Y] = 0;
      f_sizes[X] = simdata->lsizes[X];
      f_sizes[Y] = 1;

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->ghostlayerBottom);
      MPI_Type_commit(&self->ghostlayerBottom);

      startindices[X] = 0;
      startindices[Y] = simdata->lsizes[Y]-1;
      f_sizes[X] = simdata->lsizes[X];
      f_sizes[Y] = 1;

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->ghostlayerTop);
      MPI_Type_commit(&self->ghostlayerTop);

      ////////////////////////////////////
      // 4 times outermost layer of process cell-domain that is not ghost layer
      ////////////////////////////////

      startindices[X] = 1;
      startindices[Y] = 0;
      f_sizes[X] = 1;
      f_sizes[Y] = simdata->lsizes[Y];

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->memlayerLeft);
      MPI_Type_commit(&self->memlayerLeft);

      startindices[X] = simdata->lsizes[X]-2;
      startindices[Y] = 0;
      f_sizes[X] = 1;
      f_sizes[Y] = simdata->lsizes[Y];

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->memlayerRight);
      MPI_Type_commit(&self->memlayerRight);

      startindices[X] = 0;
      startindices[Y] = 1;
      f_sizes[X] = simdata->lsizes[X];
      f_sizes[Y] = 1;

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->memlayerBottom);
      MPI_Type_commit(&self->memlayerBottom);

      startindices[X] = 0;
      startindices[Y] = simdata->lsizes[Y]-2;
      f_sizes[X] = simdata->lsizes[X];
      f_sizes[Y] = 1;

      MPI_Type_create_subarray(2, simdata->lsizes, f_sizes, startindices,MPI_ORDER_C, MPI_INT, &self->memlayerTop);
      MPI_Type_commit(&self->memlayerTop);




    }

}

static MPI_Offset MPIData_writeFileHeader(MPIData* self, SimData* simdata, bool float_values) {
  char   header[2048];
  size_t len;

  len = float_values ? VTK_getHeader_f(&self->vtk, simdata->timestep, header, simdata->timesteps)
                     : VTK_getHeader(&self->vtk, simdata->timestep, header, simdata->timesteps);


  // Write the header to the file.
  // printf("writing file header with process %i \n", self->rank);
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

  /* Create a new file handle for collective I/O
   */
  MPI_File_open(self->commslave, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &self->fh);

  // /* Write the file header with ONE process
  //  */
  MPI_Barrier(self->commslave);
  offset = MPIData_writeFileHeader(self, simdata, float_values);

  /* Set the file view for the file handle using collective I/O
   */

  int local_array_size = (simdata->lsizes[X]) * (simdata->lsizes[Y]);

  //  printf("I am process %i and my offset is %i\n",self->rank, offset );
  //
  // rc = MPI_File_set_view(self->fh, offset , MPI_INT , self->localarray_type,"native", MPI_INFO_NULL); // vers localarray_type
  rc = MPI_File_set_view(self->fh, offset , MPI_INT , self->fileout_type,"native", MPI_INFO_NULL); // vers fileout_type


  if (rc != MPI_SUCCESS) {
    MPI_Error_string(rc, buffer, &len);
    printf("ERROR in line %d: %s\n", __LINE__, buffer);
  }


  /* Write the data using collective I/O
   */
  printf("writing file with process %i\n", self->rank);

  // rc = MPI_File_write_all(self->fh, field->data[1] ,local_array_size, MPI_INT, &self->status); // version with localarray_type

  int w = simdata->lsizes[X] - 2;
  int h = simdata->lsizes[Y] - 2;
{
  int c = 0;
  for(int i = 0; i < w; i++){
    for(int j = 0; j < h; j++){
      field->data_tmp[j*w+i] = Field_getCell(field,i,j);;
    }
  }
}
  local_array_size = w*h;
  rc = MPI_File_write_all(self->fh, field->data_tmp ,local_array_size, MPI_INT, &self->status); // version with fileout_type

  if (rc != MPI_SUCCESS) {
    MPI_Error_string(rc, buffer, &len);
    printf("ERROR in %d: %s\n", __LINE__, buffer);
  }

  /* Close the file handle.
   */
   MPI_File_close( &self->fh ); // close with file handle


}

void MPIData_exchangeGhostLayer(MPIData* self, Field* field) {

  // get left and right neighbor
  int left, right, down , up;


  MPI_Cart_shift ( self->comm_cart , 0 , 1 , & left , & right );
  MPI_Cart_shift ( self->comm_cart , 1 , 1 , & down , & up );

  // printf("my rank is %i. left  is %i, right is %i.  top  is %i. down is %i\n", self->rank, right, left,up,down);


  // 1 - sende nach rechts und empfange das von links
  MPI_Sendrecv(&field->data[1][0] , 1, self->memlayerRight,  right, 1,
               &field->data[1][0] , 1, self->ghostlayerLeft, left,  1,
               self->comm_cart, &self->status);

  // 2 - sende nach links und empfange von rechts

  MPI_Sendrecv(&field->data[1][0] , 1, self->memlayerLeft,  left, 2,
              &field->data[1][0] , 1, self->ghostlayerRight, right,  2,
              self->comm_cart, &self->status);

  // 3 - sende nach unten und empfange von oben


  MPI_Sendrecv(&field->data[1][0] , 1, self->memlayerTop,  up, 3,
               &field->data[1][0] , 1, self->ghostlayerBottom, down,  3,
                self->comm_cart, &self->status);


    // 4 - sende nach oebn und empfange von unten

  MPI_Sendrecv(&field->data[1][0] , 1, self->memlayerBottom,  down, 4,
               &field->data[1][0] , 1, self->ghostlayerTop, up,  4,
                self->comm_cart, &self->status);


  // IMPORTANT: Zuerst Austauschen in X-Richtung, danach in Y Richtung (Konstistent!)



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
