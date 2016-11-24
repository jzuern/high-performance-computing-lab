/* High performance computing - Mandelbrot kernel interface
 *
 * Copyright (C) 2014 IMP - University of Applied Sciences Karlsruhe
 *                        - Constantin Heisler
 */

#ifndef MANDELBROT_H_
#define MANDELBROT_H_

#include "../field.h"
#include "../simdata.h"
/* The maximum number of iterations.
 */
#define MAX_ITERATIONS 10000

/* This is the central position of our mandelbrot window.
 */
#define X_POS   -6.828978e-01
#define Y_POS   0.3



/* The mandelbrot kernel.
 *
 * Calculates the number of iterations of Z_{i+1} = Z_i^2 + C
 * where Z and C are complex numbers until |Z_i| > 2.0 or the number
 * of iterations is higher than MAX_ITERATIONS.
 *
 * Z_0 is equal to 0+0i.
 * C   is equal to the scaled x,y coordinate of the complex plane.
 *      (to calculate the scale parameter for each timestep: scale=pow(1.1,-timestep))
 *
 * The Mandelbrot set lies within -2.5..1 for the real part
 * and within -1..1 for the imaginary part.
 *
 * c, ci  The real/imaginary coordinate.
 *
 * Returns the number of iterations.
 */
//int mandelbrot_sweep(Field* field, SimData* simdata);
void mandelbrot_sweep(Field* field, SimData* simdata);

#endif
