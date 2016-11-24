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
  // TODO: implement this function.

  // NOTE: Use the getter/setter from field.h
  //       and the macros ALIVE and DEAD as needed.



  int x, y, ii, jj;
  int index_x, index_y;

  int boundarysizeX = field->boundarysize[X];
  int boundarysizeY = field->boundarysize[Y];

  int counter;


    printf("boundarysizeX = %i\n",boundarysizeX);
    printf("boundarysizeY = %i\n",boundarysizeY);

    printf("simdata->gsizes[X] = %i\n",simdata->gsizes[X]);
    printf("simdata->gsizes[Y] = %i\n",simdata->gsizes[Y]);



  // for (y = 0; y < simdata->gsizes[Y]; y++) {
  //   for (x = 0; x < simdata->gsizes[X]; x++) {


      for (y = 1; y < simdata->gsizes[Y]-1 ; y++) {
        for (x = 1; x < simdata->gsizes[X]-1 ; x++) {


      counter = 0;


      // get neighboring cells
      for (ii = -1; ii <= 1; ii++){
        for (jj = -1; jj <= 1; jj++){


          index_x = x + ii;
          index_y = y + jj;

          // printf("index: %i,%i\n",index_x,index_y);


          // // apply periodic boundary
          // if (index_x < 0){ // x direction
          //   index_x += simdata->gsizes[X];
          // } else if(index_x >= simdata->gsizes[X]){
          //   index_x -= simdata->gsizes[X];
          // }
          //
          // if (index_y < 0){ // y direction
          //   index_y += simdata->gsizes[Y];
          // } else if(index_y >= simdata->gsizes[Y]){
          //   index_y -= simdata->gsizes[Y];
          // }

          if (Field_getCell(field,index_x, index_y) == ALIVE){
            counter += 1;
          }

          // printf("counter = %i; ",counter);


        }
      }

      // Field_setCell(field,x,y,ALIVE);

      if (Field_getCell(field,x,y) == ALIVE){ // cell ALIVE
        if(counter < 2 || counter > 3 ) Field_setCell(field,x,y,DEAD);
        else{Field_setCell(field,x,y,ALIVE); }

        // otherwise it stays alive

      } else{ // cell DEAD
        if(counter == 3) Field_setCell(field,x,y,ALIVE);
        else{Field_setCell(field,x,y,DEAD); }
      }
    }
  }

} // gameoflife_sweep


// void apply_periodic_boundary(Field *field, SimData *simdata){ // very long und unnecessary... I imlemented this in gameoflife_sweep itself
//
//   unsigned int boundarysizeX = field->boundarysize[X];
//   unsigned int boundarysizeY = field->boundarysize[Y];
//
//   int gsizes_x = simdata->gsizes[X];
//   int gsizes_y = simdata->gsizes[Y];
//
//   // oberste Zeile inkl Ecken
//   for (int i = 0; i < gsizes_x; i++){
//
//       counter = 0;
//       // get neighboring cells
//       for (ii = -1; ii <= 1; ii++){
//         for (jj = -1; jj <= 1; jj++){
//
//           index_x = x + jj;
//           index_y = y + ii;
//
//           if (index_x < 0){ // x direction
//             index_x += gsizes_x;
//           } else if(index_x >= gsizes_x{
//             index_x -= gsizes_x;
//           }
//
//           if (index_y < 0){ // y direction
//             index_y += gsizes_y;
//           } else if(index_y >= gsizes_y){
//             index_y -= gsizes_y;
//           }
//
//           if (Field_getCell(field,index_x, index_y) == ALIVE) counter++;
//         }
//       }
//
//       if (Field_getCell(field,x,y) == ALIVE){ // cell ALIVE
//         if(counter < 2 || counter > 3 ) Field_setCell(field,x,y,DEAD);
//         // otherwise it stays alive
//       } else{ // cell DEAD
//         if(counter == 3) Field_setCell(field,x,y,ALIVE);
//         // otherwise stays dead
//       }
//
//   }
//
//
//   // unterste Zeile inkl Ecken
//
//   // linke Spalte (ohne Ecken)
//
//   // rechte Spalte (ohne Ecken)
//
//
// }
