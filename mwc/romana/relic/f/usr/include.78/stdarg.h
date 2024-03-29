/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * stdarg.h
 * C variable arguments header.
 * Draft Proposed ANSI C Standard, Section 4.8, 12/7/88 draft.
 */

#ifndef	__STDARG_H__
#define	__STDARG_H__

/* Type. */
#if	__STDC__
typedef	void *va_list;
#else
typedef	char *va_list;
#endif

/* Macros. */
#define	va_start(ap, pN)	ap = (((va_list)&pN) + sizeof(pN))
#define	va_arg(ap, type)	(((type *)(ap += sizeof(type)))[-1])
#define	va_end(ap)

#endif

/* end of stdarg.h */
