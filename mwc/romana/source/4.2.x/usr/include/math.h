/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * Definitions for math library.
 * ANSI C Standard, section 4.5.
 */

#ifndef	__MATH_H__
#define	__MATH_H__

#include <common/feature.h>
#include <common/ccompat.h>

#if	! _STDC_SOURCE && ! _POSIX_C_SOURCE

#if	__DECVAX__
#define L2HUGE_VAL	127.0		/* log2(infinity)	*/
#define L10P		17		/* log10(precision)	*/
#define L2L2P		6		/* log2(log2(precision)) */
#elif	__IEEE__
#define L2HUGE_VAL	1023.0		/* log2(infinity)	*/
#define L10P		16		/* log10(precision)	*/
#define L2L2P		6		/* log2(log2(precision)) */
#endif

/* Constants. */
#define PI		0.31415926535897932e+01
#define SQRT2		0.14142135623730950e+01
#define LOG2B10		0.30102999566398119e+00
#define LOG10BE		0.23025850929940456e+01
#define LOG10B2		0.33219280948873623e+01
#define LOGEB2		0.14426950408889634e+01

/* Complex variables. */
typedef struct cpx {
	double	z_r;
	double	z_i;
} CPX;

#endif


#if	__DECVAX__

#define HUGE_VAL	1e+37		/* Infinity		*/

#elif	__IEEE__

#define HUGE_VAL	1e+308		/* Infinity		*/

#endif

__EXTERN_C_BEGIN__

double		acos		__PROTO ((double _x));
double		asin		__PROTO ((double _x));
double		atan		__PROTO ((double _x));
double		atan2		__PROTO ((double _y, double _x));
double		cos		__PROTO ((double _x));
double		sin		__PROTO ((double _x));
double		tan		__PROTO ((double _x));
double		cosh		__PROTO ((double _x));
double		sinh		__PROTO ((double _x));
double		tanh		__PROTO ((double _x));
double		exp		__PROTO ((double _x));
double		frexp		__PROTO ((double _x, int * _iptr));
double		ldexp		__PROTO ((double _x, int _i));
double		log		__PROTO ((double _x));
double		log10		__PROTO ((double _x));
double		modf		__PROTO ((double _value, double * _iptr));
double		pow		__PROTO ((double _x, double _y));
double		sqrt		__PROTO ((double _x));
double		ceil		__PROTO ((double _x));
double		fabs		__PROTO ((double _x));
double		floor		__PROTO ((double _x));
double		fmod		__PROTO ((double _x, double _y));

#if	! _STDC_SOURCE && ! _POSIX_C_SOURCE

/* Math library functions in /lib/libm.a. */
double		cabs		__PROTO ((struct cpx _z));
double		hypot		__PROTO ((double _x, double _y));
double		j0		__PROTO ((double _x));
double		j1		__PROTO ((double _x));
double		jn		__PROTO ((int _n, double _x));

double		atof		__PROTO ((__CONST__ char * _nptr));

#endif	/* ! _STDC_SOURCE && ! _POSIX_C_SOURCE */

__EXTERN_C_END__

#if	1

/* Internal functions. */
extern	double	_pol();
extern	double	_two();

#endif	/* not permitted in this header */

#endif	/* ! defined (__MATH__H__) */

