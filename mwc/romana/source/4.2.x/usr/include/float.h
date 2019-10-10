/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * C floating point representation header.
 * Draft Proposed ANSI C Standard, 12/7/88 draft.
 * Sections 2.2.4.2, 4.1.4.
 */

#ifndef	__FLOAT_H__
#define	__FLOAT_H__

#if	_DECVAX
/*
 * DECVAX format.
 * Double:	1 sign +  8 exponent + 56 mantissa, bias  129.
 * Float:	1 sign +  8 exponent + 24 mantissa, bias  129.
 * Long double:	unimplemented, limits defined here are same as double.
 */
#define	DBL_DIG		16
#define	DBL_EPSILON	2.7755575615628914e-17
#define	DBL_MANT_DIG	56
#define	DBL_MAX		1.7014118346046923e+38
#define	DBL_MAX_EXP	127
#define	DBL_MAX_10_EXP	38
#define	DBL_MIN		2.9387358770557188e-39
#define	DBL_MIN_EXP	(-128)
#define DBL_MIN_10_EXP	(-38)
#define	FLT_DIG		6
#define	FLT_EPSILON	1.1920928955078125e-07
#define	FLT_MANT_DIG	24
#define	FLT_MAX		1.7014117331926443e+38
#define	FLT_MAX_EXP	127
#define	FLT_MAX_10_EXP	38
#define	FLT_MIN		2.9387358770557188e-39
#define	FLT_MIN_EXP	(-128)
#define	FLT_MIN_10_EXP	(-38)
#define	FLT_RADIX	2
#define	FLT_ROUNDS	1
#define	LDBL_DIG	DBL_DIG
#define	LDBL_EPSILON	DBL_EPSILON
#define	LDBL_MANT_DIG	DBL_MANT_DIG
#define	LDBL_MAX	DBL_MAX
#define	LDBL_MAX_EXP	DBL_MAX_EXP
#define	LDBL_MAX_10_EXP	DBL_MAX_10_EXP
#define	LDBL_MIN	DBL_MIN
#define	LDBL_MIN_EXP	DBL_MIN_EXP
#define	LDBL_MIN_10_EXP	DBL_MIN_10_EXP
#endif

#if	_IEEE
/*
 * IEEE format.
 * Double:	1 sign + 11 exponent + 53 mantissa, bias 1023.
 * Float:	1 sign +  8 exponent + 24 mantissa, bias  127.
 * Long double:	unimplemented, limits defined here are same as double.
 */
#define	DBL_DIG		15
#define	DBL_EPSILON	2.220446049250313e-16
#define	DBL_MANT_DIG	53
#define	DBL_MAX		1.7976931348623157e+308
#define	DBL_MAX_EXP	1024
#define	DBL_MAX_10_EXP	308
#define	DBL_MIN		2.2250738585072025e-308
#define	DBL_MIN_EXP	(-1021)
#define DBL_MIN_10_EXP	(-307)
#define	FLT_DIG		6
#define	FLT_EPSILON	1.192092895507813e-07
#define	FLT_MANT_DIG	24
#define	FLT_MAX		3.402823466385289e+38
#define	FLT_MAX_EXP	128
#define	FLT_MAX_10_EXP	38
#define	FLT_MIN		1.175494350822288e-38
#define	FLT_MIN_EXP	(-125)
#define	FLT_MIN_10_EXP	(-37)
#define	FLT_RADIX	2
#define	FLT_ROUNDS	1
#define	LDBL_DIG	DBL_DIG
#define	LDBL_EPSILON	DBL_EPSILON
#define	LDBL_MANT_DIG	DBL_MANT_DIG
#define	LDBL_MAX	DBL_MAX
#define	LDBL_MAX_EXP	DBL_MAX_EXP
#define	LDBL_MAX_10_EXP	DBL_MAX_10_EXP
#define	LDBL_MIN	DBL_MIN
#define	LDBL_MIN_EXP	DBL_MIN_EXP
#define	LDBL_MIN_10_EXP	DBL_MIN_10_EXP
#endif

#endif

/* end of float.h */
