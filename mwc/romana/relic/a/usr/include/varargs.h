/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * varargs.h
 * Variable arguments.
 */

#ifndef	_VARARGS_H
#define	_VARARGS_H	_VARARGS_H

typedef	char *va_list;
#define	va_dcl			int va_alist;
#define	va_start(ap)		ap = (va_list)&va_alist
#define	va_arg(ap, type)	(((type *)(ap += sizeof(type)))[-1])
#define	va_end(ap)

#endif

/* end of varargs.h */
