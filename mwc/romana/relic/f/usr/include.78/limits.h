/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * limits.h
 * C numerical limits header.
 * Draft Proposed ANSI C Standard, 12/7/88 draft.
 * Sections 2.2.4.2, 4.1.4.
 *	2's complement arithmetic
 *	char	8 bits, sign-extended
 *	short	16 bits
 *	int	16 bits for i8086, 32 bits for i386
 *	long	32 bits
 */

#ifndef	__LIMITS_H__
#define	__LIMITS_H__

#define	CHAR_BIT	8
#define	CHAR_MAX	SCHAR_MAX
#define	CHAR_MIN	SCHAR_MIN
#if	IAPX86
#define	INT_MAX		0x7FFF
#define	INT_MIN		0x8000
#elif	_I386
#define	INT_MAX		0x7FFFFFFF
#define	INT_MIN		0x80000000
#endif
#define	LONG_MAX	0x7FFFFFFFL
#define	LONG_MIN	0x80000000L
#define	MB_LEN_MAX	1
#define	SCHAR_MAX	127
#define	SCHAR_MIN	-128
#define	SHRT_MAX	32767
#define	SHRT_MIN	-32768
#define	UCHAR_MAX	255
#if	IAPX86
#define	UINT_MAX	0xFFFFU
#elif	_I386
#define	UINT_MAX	0xFFFFFFFFU
#endif
#define	USHRT_MAX	0xFFFFU
#define	ULONG_MAX	0xFFFFFFFFUL

#endif

/* end of limits.h */
