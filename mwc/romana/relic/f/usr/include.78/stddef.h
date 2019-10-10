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

#ifndef	_SIZE_T
#define	_SIZE_T
typedef	unsigned int	size_t;		/* sizeof result type		*/
#endif

#ifndef	_WCHAR_T
#define	_WCHAR_T
typedef	char		wchar_t;	/* extended character set type	*/
#endif

/* Macros. */
#ifndef	NULL
#define	NULL	((char *)0)		/* null pointer constant	*/
#endif
#ifndef	Void
#define	Void	char		/* Use "Void *" as generic pointer type	*/
#endif

#define offsetof(s,id)	((size_t)&(((s *)0)->id))
				/* offset of member id in struct s	*/

#endif

/* end of stddef.h */
