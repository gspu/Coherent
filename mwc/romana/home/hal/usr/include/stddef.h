/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__STDDEF_H__
#define	__STDDEF_H__

typedef	int		ptrdiff_t;	/* pointer difference type */

#include <common/_size.h>
#include <common/_wchar.h>
#include <common/_null.h>

#define offsetof(s,id)	((size_t) & (((s *) 0)->id))
					/* offset of member id in struct s */

#endif	/* ! defined (__STDDEF_H__) */
