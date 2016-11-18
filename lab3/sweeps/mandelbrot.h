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
#define X_POS    0.43
#define Y_POS   -0.2166393884377127

/* The mandelbrot kernel.
 *
 * Calculates the number of iterations of Z_i = Z_{i+1}^2 + C
 * where Z and C are complex numbers until |Z_i| > 2.0 or the number
 * of iterations is higher than MAX_ITERATIONS.
 *
 * Z_0 is equal to 0+0i.
 * C   is equal to the scaled x/y coordinate of the complex plane.
 *
 * The Mandelbrot set lies within -2.5..1 for the real part
 * and within -1..1 for the imaginary part.
 *
 * c, ci  The real/imaginary coordinate.
 *
 * Returns the number of iterations.
 */
int mandelbrot_sweep(Field* field, SimData* simdata);

#endif
