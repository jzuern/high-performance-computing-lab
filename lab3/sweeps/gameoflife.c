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


  int x, y;
  int counter, index_x, index_y;

  int w = field->size[X];
  int h = field->size[Y];


  for (y = 0; y < h; y++) {
    for (x = 0; x < w; x++) {

	     counter = 0;

        // get neighboring cells
        for (int ii = -1; ii <= 1; ii++){
          for (int jj = -1; jj <= 1; jj++){
            if((ii) == 0 && (jj == 0)) continue; // dont check cell itself

            index_x = x + jj;
            index_y = y + ii;

            if (Field_getCell(field, index_x, index_y) == ALIVE){
              counter += 1;
            }
          }
        }

        if (Field_getCell(field, x, y)  == ALIVE){ // cell ALIVE
          if((counter == 2) || (counter == 3) ){
            Field_setCell(field, x, y, ALIVE);
          } else{
            Field_setCell(field, x, y, DEAD);
          } // cell ALIVE end

        } else if (Field_getCell(field, x, y)  == DEAD){ // cell DEAD
          if(counter == 3) {
            Field_setCell(field, x, y, ALIVE);
          }else{
            Field_setCell(field, x, y, DEAD);
          }
        } // cell DEAD end


    }
  }

  // printf("bye from gameoflife_sweep\n");


}
