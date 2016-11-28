/* High performance computing - Game of Life implementation
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>


#include "gameoflife.h"

void gameoflife_sweep(Field* field, SimData* simdata,MPIData* mpidata) {


  // printf("Hello from gameoflife_sweep\n");


  int x, y, ii, jj;
  int boundarysizeX = field->boundarysize[X];
  int boundarysizeY = field->boundarysize[Y];
  int counter, index_x, index_y;

  int w = field->size[X];
  int h = field->size[Y];

  // printf("local field size is %i x %i cells\n", w,h);


  // for (y = boundarysizeY; y < simdata->lsizes[Y]-boundarysizeY; y++) {
  //   for (x = boundarysizeX; x < simdata->lsizes[X]-boundarysizeX; x++) {

  for (y = -1; y < (h+1); y++) {
    for (x = -1; x < (w+1); x++) {

      Field_setCell(field, x,y, 1+mpidata->rank);

	    //  counter = 0;
       //
      //   // get neighboring cells
      //   for (ii = -1; ii <= 1; ii++){
      //     for (jj = -1; jj <= 1; jj++){
      //       if((ii) == 0 && (jj == 0)) continue; // dont check cell itself
       //
      //       index_x = x + jj;
      //       index_y = y + ii;
       //
      //       // apply periodic boundary
      //       if (index_x < 0){ // x direction
      //         index_x += w;
      //       } else if(index_x >= w){
      //         index_x -= w;
      //       }
       //
      //       if (index_y < 0){ // y direction
      //         index_y += h;
      //       } else if(index_y >= h){
      //         index_y -= h;
      //       }
       //
      //       if (Field_getCell(field, index_x, index_y) == ALIVE){
      //         counter += 1;
      //       }
      //     }
      //   }
       //
      //   if (Field_getCell(field, x, y)  == ALIVE){ // cell ALIVE
      //     if((counter == 2) || (counter == 3) ){
      //       Field_setCell(field, x, y, ALIVE);
      //     } else{
      //       Field_setCell(field, x, y, DEAD);
      //     } // cell ALIVE
       //
      //   } else if (Field_getCell(field, x, y)  == DEAD){ // cell DEAD
      //     if(counter == 3) {
      //       Field_setCell(field, x, y, ALIVE);
      //     }else{
      //       Field_setCell(field, x, y, DEAD);
      //     }
      //   } // cell DEAD
    }
  }

  // printf("bye from gameoflife_sweep\n");


}
