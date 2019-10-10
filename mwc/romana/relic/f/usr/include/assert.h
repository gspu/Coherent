/* (-lgl
 * 	COHERENT Version 4.1.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef	__ASSERT_H__
#define	__ASSERT_H__

#include <common/ccompat.h>

__EXTERN_C__ __NO_RETURN__ void
		__assertfail	__PROTO ((__CONST__ char * _test,
					  __CONST__ char * _file, int _line));

#endif	/* ! defined (__ASSERT_H__) */

#undef	assert

#if	NDEBUG
#define	assert(p)	((void) 0)
#else
#define	assert(p)	((p) ? (void) 0 : \
			 __assertfail (__STRING (p), __FILE__, __LINE__))
#endif
