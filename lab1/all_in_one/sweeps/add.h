/* High performance computing - 2D heat equation kernel
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer
 */

#ifndef ADD_H
#define ADD_H

#include "../field.h"
#include "../simdata.h"
/* Calculates one step of a 2D heat equation with SIMD SSE operations.
 */
void add_sweep(Field* field, SimData* simdata);

#endif
