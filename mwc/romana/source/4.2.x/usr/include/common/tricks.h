/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON_TRICKS_H__
#define	__COMMON_TRICKS_H__

/*
 * Export some of the definitions from <common/_tricks.h> into the user
 * namespace.
 */

#include <common/_tricks.h>

#define	IS_POWER_OF_TWO(i)		__IS_POWER_OF_TWO (i)
#define	DIVIDE_ROUNDUP_CONST(num,den)	__DIVIDE_ROUNDUP_CONST (num, den)
#define	DIVIDE_ROUNDUP(num,den)		__DIVIDE_ROUNDUP (num, den)
#define	ROUND_UP_TO_MULTIPLE_CONST(n,m)	__ROUND_UP_TO_MULTIPLE_CONST (n, m)
#define	ROUND_UP_TO_MULTIPLE(n,mult)	__ROUND_UP_TO_MULTIPLE (n, mult)
#define	LEAST_BIT_UCHAR(i)		__LEAST_BIT_UCHAR (i)
#define	LEAST_BIT_USHRT(i)		__LEAST_BIT_USHRT (i)
#define	LEAST_BIT_UINT(i)		__LEAST_BIT_UINT (i)
#define	LEAST_BIT_ULONG(i)		__LEAST_BIT_ULONG (i)
#define	MOST_BIT_UCHAR(i)		__MOST_BIT_UCHAR (i)
#define	MOST_BIT_USHRT(i)		__MOST_BIT_USHRT (i)
#define	MOST_BIT_UINT(i)		__MOST_BIT_UINT (i)
#define	MOST_BIT_ULONG(i)		__MOST_BIT_ULONG (i)
#define	LEAST_BIT_UCHAR_CONST(i)	__LEAST_BIT_UCHAR_CONST (i)
#define	LEAST_BIT_USHRT_CONST(i)	__LEAST_BIT_USHRT_CONST (i)
#define	LEAST_BIT_UINT_CONST(i)		__LEAST_BIT_UINT_CONST (i)
#define	LEAST_BIT_ULONG_CONST(i)	__LEAST_BIT_ULONG_CONST (i)
#define	MOST_BIT_UCHAR_CONST(i)		__MOST_BIT_UCHAR_CONST (i)
#define	MOST_BIT_USHRT_CONST(i)		__MOST_BIT_USHRT_CONST (i)
#define	MOST_BIT_UINT_CONST(i)		__MOST_BIT_UINT_CONST (i)
#define	MOST_BIT_ULONG_CONST(i)		__MOST_BIT_ULONG_CONST (i)
#define	ARRAY_LENGTH(a)			__ARRAY_LENGTH (a)
#define	ADD_UINT_WITH_MAX(add,aug)	__ADD_UINT_WITH_MAX (add, aug)
#define	SUB_UINT_WITH_MIN(min,sub)	__SUB_UINT_WITH_MIN (min, sub)

#endif	/* ! defined (__COMMON_TRICKS_H__) */

