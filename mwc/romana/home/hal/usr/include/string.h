/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

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

#if	! _STDC_SOURCE && ! _POSIX_C_SOURCE

__VOID__      *	memccpy		__PROTO ((__VOID__ * _s1,
					  __CONST__ __VOID__ * _s2, int _c,
					  size_t _n));
char	      *	strdup		__PROTO ((__CONST__ char * _s));


/*
 * BSD-style string and memory functions
 *
 * We permit the suppression of the definitions below to avoid
 * conflicts; if you want to do so, set the symbol _SUPPRESS_BSD_DEFINITIONS
 * to 1.
 */

#if	! _SUPPRESS_BSD_DEFINITIONS

#ifndef	index
# define	index(s,c)		(strchr (s, c))
#endif
#ifndef	rindex
# define	rindex(s,c)		(strrchr (s, c))
#endif
#ifndef	bcopy
# define	bcopy(s2,s1,c)		(memcpy (s1, s2, c))
#endif
#ifndef	bcmp
# define	bcmp(s1,s2,c)		(memcmp (s2, s1, c))
#endif
#ifndef	bzero
# define	bzero(s,c)		(memset (s, 0, c))
#endif

#endif	/* ! _SUPPRESS_BSD_DEFINITIONS */


/*
 * There is no standard location for the alloca () prototype, so here it goes.
 */

__VOID__      *	alloca		__PROTO ((size_t _size));

#endif

__EXTERN_C_END__

#if	1

/* Internal function. */
extern	void	_memxchg();

#endif	/* not permitted in this header */

#endif	/* ! defined (__STRING_H__) */
