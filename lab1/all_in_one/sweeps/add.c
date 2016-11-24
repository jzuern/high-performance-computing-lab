/* High performance computing exercise course main function
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                          Johannes Hötzer 
 */


#include "add.h"

void add_sweep(Field* field, SimData* simdata) {
  unsigned int x, y;
  unsigned int boundarysizeX = field->boundarysize[X];
  unsigned int boundarysizeY = field->boundarysize[Y];

  for (y = boundarysizeY; y < simdata->gsizes[Y]-boundarysizeY; y++) {
    for (x = boundarysizeX; x < simdata->gsizes[X]-boundarysizeX; x++) {
       float cell = Field_getCell(field, x, y);
       cell = (cell+cell);
       Field_setCell(field, x, y, cell);
    }
  }
}
