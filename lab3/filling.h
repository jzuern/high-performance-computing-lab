/* High performance computing - Filling class declaration
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Johannes Hötzer, Constantin Heisler
 */

#ifndef FILLING_H_
#define FILLING_H_

#include "field.h"

/* Produces a pattern of a runner.
 * 
 * value: use ALIVE for this parameter.
 */
void Filling_produceRunner(Field* field, int value, int xpos, int ypos);

/* Produces a pattern of a gun.
 * 
 * value: use ALIVE for this parameter.
 */
void Filling_produceGun(Field* field, int value, int xpos, int ypos);

/* Fills an area with value or 0 using a random function.
 * The value is set as an integer.
 * 
 * value: use ALIVE for this parameter.
 */
void Filling_binaryRandomizeFrame(Field* field, int value);

/* Fills an area with value or 0 using a random function.
 * The value is set as a float.
 * 
 * value: use ALIVE for this parameter.
 */
void Filling_binaryRandomizeFrame_f(Field* field, int value);

/* Fills an area with a horizontal Line of length len at given position.
 * 
 * value: use ALIVE for this parameter.
 */
void Filling_produceLine(Field* field, int value, int xpos, int ypos);

#endif

