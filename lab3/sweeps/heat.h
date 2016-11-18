/* High performance computing - 2D heat equation kernel
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#ifndef HEAT_H_
#define HEAT_H_

#include "../field.h"
#include "../simdata.h"

#define HOT 20.0

/* Calculates one step of a 2D heat equation.
 */
void heat_sweep(Field* field, SimData* simdata);

#endif
