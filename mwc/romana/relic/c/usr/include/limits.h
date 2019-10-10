/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
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
 *	int	16 bits
 *	long	32 bits
 */

#ifndef	_LIMITS_H
#define	_LIMITS_H

#define	CHAR_BIT	8
#define	CHAR_MAX	SCHAR_MAX
#define	CHAR_MIN	SCHAR_MIN
#define	INT_MAX		0x7FFF
#define	INT_MIN		0x8000
#define	LONG_MAX	0x7FFFFFFFL
#define	LONG_MIN	0x80000000L
#define	MB_LEN_MAX	1
#define	SCHAR_MAX	127
#define	SCHAR_MIN	-128
#define	SHRT_MAX	((short)0x7FFF)
#define	SHRT_MIN	((short)0x8000)
#define	UCHAR_MAX	255
#define	UINT_MAX	((unsigned int)0xFFFF)
#define	USHRT_MAX	((unsigned short)0xFFFF)
#define	ULONG_MAX	((unsigned long)0xFFFFFFFFL)

#endif

/* end of limits.h */
