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

void gameoflife_sweep(Field* field, SimData* simdata) {

  int x, y, ii, jj;
  int boundarysizeX = field->boundarysize[X];
  int boundarysizeY = field->boundarysize[Y];
  int counter, index_x, index_y;

  int w = field->size[X];
  int h = field->size[Y];

  //#pragma omp for collapse(2) private(x,y)
  for (y = boundarysizeY; y < simdata->gsizes[Y]-boundarysizeY; y++) {
    for (x = boundarysizeX; x < simdata->gsizes[X]-boundarysizeX; x++) {

	counter = 0;

        // get neighboring cells
        for (ii = -1; ii <= 1; ii++){
          for (jj = -1; jj <= 1; jj++){
            if((ii) == 0 && (jj == 0)) continue; // dont check cell itself

            index_x = x + jj;
            index_y = y + ii;

            // apply periodic boundary
            if (index_x < 0){ // x direction
              index_x += w;
            } else if(index_x >= w){
              index_x -= w;
            }

            if (index_y < 0){ // y direction
              index_y += h;
            } else if(index_y >= h){
              index_y -= h;
            }

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
          }

        } else if (Field_getCell(field, x, y)  == DEAD){ // cell DEAD
          if(counter == 3) {
            Field_setCell(field, x, y, ALIVE);
          }else{
            Field_setCell(field, x, y, DEAD);
          }
        }
        else{
          printf("something is wrong\n");
          exit(0);
        }
    }
  }

}
