/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * stddef.h
 * C common definition header.
 * Draft Proposed ANSI C Standard, Section 4.1.5, 5/13/88 draft.
 */

#ifndef	__STDDEF_H__
#define	__STDDEF_H__

/* Types. */
#ifndef	_PTRDIFF_T
#define	_PTRDIFF_T
#if	_LARGE
typedef	long		ptrdiff_t;	/* pointer difference type	*/
#else
typedef	int		ptrdiff_t;	/* pointer difference type	*/
#endif
#endif

#include <common/_size.h>
#include <common/_wchar.h>
#include <common/_null.h>

#define offsetof(s,id)	((size_t)&(((s *)0)->id))
				/* offset of member id in struct s	*/

#endif	/* ! defined (__STDDEF_H__) */
