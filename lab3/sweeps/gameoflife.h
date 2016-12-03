/* High performance computing - Game of Life interface
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#ifndef GAME_OF_LIFE_H_
#define GAME_OF_LIFE_H_

#include "../field.h"
#include "../simdata.h"
#include "../mpidata.h"

/* A cell which is alive.
 */
#define ALIVE 1

/* A cell which is dead.
 */
#define DEAD     0

/* Calulates one generation in Game Of Life.
 */
void gameoflife_sweep(Field* field, SimData* simdata, MPIData* mpidata);


#endif
