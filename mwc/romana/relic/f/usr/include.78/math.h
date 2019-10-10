/* (-lgl
 * 	COHERENT Version 4.1.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/math.h
 * Definitions for math library.
 * ANSI C Standard, section 4.5.
 */

#ifndef	__MATH_H__
#define	__MATH_H__

/* Manifest constants. */
#if	__DECVAX__
#define HUGE_VAL	1e+37		/* Infinity		*/
#define L2HUGE_VAL	127.0		/* log2(infinity)	*/
#define L10P		17		/* log10(precision)	*/
#define L2L2P		6		/* log2(log2(precision)) */
#elif	__IEEE__
#define HUGE_VAL	1e+308		/* Infinity		*/
#define L2HUGE_VAL	1023.0		/* log2(infinity)	*/
#define L10P		16		/* log10(precision)	*/
#define L2L2P		6		/* log2(log2(precision)) */
#endif

/* Error return values. */
#define	EDOM	33			/* Domain error		*/
#define	ERANGE	34			/* Result too large	*/

/* Constants. */
#define PI	0.31415926535897932e+01
#define SQRT2	0.14142135623730950e+01
#define LOG2B10	0.30102999566398119e+00
#define LOG10BE	0.23025850929940456e+01
#define LOG10B2	0.33219280948873623e+01
#define LOGEB2	0.14426950408889634e+01

/* Complex variables. */
typedef struct cpx {
	double	z_r;
	double	z_i;
} CPX;

/* Error return status from routines. */
extern	int	errno;

/* Internal functions. */
extern	double	_pol();
extern	double	_two();

/* Math library functions in /lib/libm.a. */
extern	double	acos();			/* ANSI 4.5.2.1	*/
extern	double	asin();			/* ANSI 4.5.2.3	*/
extern	double	atan();			/* ANSI 4.5.2.3	*/
extern	double	atan2();		/* ANSI 4.5.2.4	*/
extern	double	cabs();			/* non-ANSI	*/
extern	double	ceil();			/* ANSI 4.5.6.1	*/
extern	double	cos();			/* ANSI 4.5.2.5	*/
extern	double	cosh();			/* ANSI 4.5.3.1	*/
extern	double	exp();			/* ANSI 4.5.4.1	*/
extern	double	fabs();			/* ANSI 4.5.6.2	*/
extern	double	floor();		/* ANSI 4.5.6.3	*/
extern	double	fmod();			/* ANSI 4.5.6.4	*/
extern	double	hypot();		/* non-ANSI	*/
extern	double	j0();			/* non-ANSI	*/
extern	double	j1();			/* non-ANSI	*/
extern	double	jn();			/* non-ANSI	*/
extern	double	log();			/* ANSI 4.5.4.4	*/
extern	double	log10();		/* ANSI 4.5.4.5	*/
extern	double	pow();			/* ANSI 4.5.5.1	*/
extern	double	sin();			/* ANSI 4.5.2.6	*/
extern	double	sinh();			/* ANSI 4.5.3.2	*/
extern	double	sqrt();			/* ANSI 4.5.5.2	*/
extern	double	tan();			/* ANSI 4.5.2.7	*/
extern	double	tanh();			/* ANSI 4.5.3.3	*/

/* C library floating point functions in /lib/libc.a. */
extern	double	atof();			/* ANSI 4.10.1.1 */
extern	double	frexp();		/* ANSI 4.5.4.2	*/
extern	double	ldexp();		/* ANSI 4.5.4.3	*/
extern	double	modf();			/* ANSI 4.5.4.6	*/

#endif

/* end of /usr/include/math.h */
