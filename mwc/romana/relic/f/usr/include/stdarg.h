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

#include <common/ccompat.h>
#include <common/__valist.h>

typedef	__va_list		va_list;

/*
 * On true ANSI/ISO C systems, we just use '...' to get the base address of
 * the argument space. In either system, we have several options as to how
 * we attempt to work around alignment issues (if we do at all).
 *
 * As a convenience, we round sizes up to a minimum size of "int". This may
 * help people who are confused about what the implementation is permitted
 * to do with enumeration types, for instance. This setup below will break
 * with systems that put the padding below the datum.
 */

#define	__size_to_int(type)	(sizeof (type) < sizeof (int) ? \
				 sizeof (int) : sizeof (type))

#if	__cplusplus

#define	va_start(ap, pN)	(ap = ...)

#else

#define	va_start(ap, pN)	(ap = (va_list) \
					((char *) & pN + __size_to_int (pN)))
#endif

#define	va_arg(ap, type)	\
	((* (type *) ((ap += __size_to_int (type)) - __size_to_int (type))))
#define	va_end(ap)		((void) 0)

#endif	/* ! defined (__STDARG_H__) */
