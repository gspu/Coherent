/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__STDARG_H__
#define	__STDARG_H__

/*
 * Please note that passing a type to va_arg() that would be widened when
 * promoted to integer type causes undefined behavior. This includes char,
 * short and enumeration types (since many translators use the narrowest
 * possible width for storing an enumeration).
 */

#include <common/ccompat.h>
#include <common/__valist.h>

typedef	__va_list		va_list;

/*
 * As a convenience, we round sizes up to a minimum size of "int". This may
 * help people who are confused about what the implementation is permitted
 * to do with enumeration types, for instance. Note that this does introduce
 * some dependence on the endianness of the machine, however, because those
 * machines which are big-endian will have put padding below the datum.
 */

#define	__size_to_int(type)	(sizeof (type) < sizeof (int) ? \
				 sizeof (int) : sizeof (type))
#if	_I386
# define	__size_to_datum(type)	__size_to_int (type)
#else
# define	__size_to_datum(type)	sizeof (type)
#endif


#define	va_start(ap, pN)	(ap = (va_list) \
					((char *) & pN + __size_to_int (pN)))
#define	va_arg(ap, type)	\
	((* (type *) ((ap += __size_to_int (type)) - __size_to_datum (type))))
#define	va_end(ap)		((void) 0)

#endif	/* ! defined (__STDARG_H__) */
