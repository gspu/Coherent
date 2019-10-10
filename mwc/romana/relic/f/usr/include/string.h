/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * string.h
 * C string handling library header.
 * Draft Proposed ANSI C Standard, Section 4.11, 5/13/88 draft.
 */

#ifndef	__STRING_H__
#define	__STRING_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_size.h>
#include <common/_null.h>


__EXTERN_C_BEGIN__

__VOID__      *	memcpy		__PROTO ((__VOID__ * _s1,
					  __CONST__ __VOID__ * _s2,
					  size_t _n));
__VOID__      *	memmove		__PROTO ((__VOID__ * _s1,
					  __CONST__ __VOID__ * _s2,
					  size_t _n));
char	      *	strcpy		__PROTO ((char * _s1, __CONST__ char * _s2));
char	      *	strncpy		__PROTO ((char * _s1, __CONST__ char * _s2,
					  size_t _n));
char	      *	strcat		__PROTO ((char * _s1, __CONST__ char * _s2));
char	      *	strncat		__PROTO ((char * _s1, __CONST__ char * _s2,
					  size_t _n));
int		memcmp		__PROTO ((__CONST__ __VOID__ * _s1,
					  __CONST__ __VOID__ * _s2,
					  size_t _n));
int		strcmp		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2));
int		strcoll		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2));
int		strncmp		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2, size_t _n));
size_t		strxfrm		__PROTO ((char * _s1, __CONST__ char * _s2,
					  size_t _n));
__VOID__      *	memchr		__PROTO ((__CONST__ __VOID__ * _s1, int _c,
					  size_t _n));
char	      *	strchr		__PROTO ((__CONST__ char * _s, int _c));
size_t		strcspn		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2));
char	      *	strpbrk		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2));
char	      *	strrchr		__PROTO ((__CONST__ char * _s1, int _c));
size_t		strspn		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2));
char	      *	strstr		__PROTO ((__CONST__ char * _s1,
					  __CONST__ char * _s2));
char	      *	strtok		__PROTO ((char * _s1, __CONST__ char * _s2));
__VOID__      *	memset		__PROTO ((__VOID__ * _s, int _c, size_t _n));
char	      *	strerror	__PROTO ((int _errnum));
size_t		strlen		__PROTO ((__CONST__ char * _s));

#if	! _STDC_SOURCE && ! _POSIX_SOURCE

__VOID__      *	memccpy		__PROTO ((__VOID__ * _s1,
					  __CONST__ __VOID__ * _s2, int _c,
					  size_t _n));
char	      *	strdup		__PROTO ((__CONST__ char * _s));


/*
 * While we are here, change calls to BSD-style string and memory functions
 * into something more modern. Programs which attempt to take the address of
 * the functions, or which attempt to access an underlying function, or
 * which do not include this header *must* be changed.
 */

#define	index(s,c)		(strchr (s, c))
#define	rindex(s,c)		(strrchr (s, c))
#define	bcopy(s2,s1,c)		(memcpy (s1, s2, c))
#define	bcmp(s1,s2,c)		(memcmp (s2, s1, c))
#define	bzero(s,c)		(memset (s, 0, c))

#endif

__EXTERN_C_END__

#if	1

/* Internal function. */
extern	void	_memxchg();

#endif	/* not permitted in this header */

#endif	/* ! defined (__STRING_H__) */
