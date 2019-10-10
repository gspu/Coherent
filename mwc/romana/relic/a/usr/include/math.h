/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Definitions for math library.
 */
#ifndef	MATH_H
#define	MATH_H

#if	_DECVAX
#define HUGE_VAL	1e+37		/* Infinity		*/
#define L2HUGE_VAL	127.0		/* log2(infinity)	*/
#define L10P		17		/* log10(precision)	*/
#define L2L2P		6		/* log2(log2(precision)) */
#elif	_IEEE
#define HUGE_VAL	1e+308		/* Infinity		*/
#define L2HUGE_VAL	1023.0		/* log2(infinity)	*/
#define L10P		16		/* log10(precision)	*/
#define L2L2P		6		/* log2(log2(precision)) */
#endif
/*
 * Error return values.
 */
#define	EDOM	33			/* Domain error */
#define	ERANGE	34			/* Result too large */

/*
 * Constants.
 */
#define PI	0.31415926535897932e+01
#define SQRT2	0.14142135623730950e+01
#define LOG2B10	0.30102999566398119e+00
#define LOG10BE	0.23025850929940456e+01
#define LOG10B2	0.33219280948873623e+01
#define LOGEB2	0.14426950408889634e+01

/*
 * Complex variables.
 */
typedef struct cpx {
	double	z_r;
	double	z_i;
} CPX;

/*
 * Status from routines.
 */
extern	int	errno;

/*
 * Internal functions.
 */
double	_pol();
double	_two();

/*
 * Math library functions.
 */
double	acos();
double	asin();
double	atan();
double	atan2();
double	cabs();
double	ceil();
double	cos();
double	cosh();
double	exp();
double	fabs();
double	floor();
double	hypot();
double	j0();
double	j1();
double	jn();
double	log();
double	log10();
double	pow();
double	sin();
double	sinh();
double	sqrt();
double	tan();
double	tanh();

/*
 * C library floating point functions.
 */
double	atof();
double	frexp();
double	ldexp();
double	modf();

#endif
