/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * C general utilities library header.
 * Draft Proposed ANSI C Standard, Section 4.10, 12/7/88 draft.
 */

#ifndef	__STDLIB_H__
#define	__STDLIB_H__

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <common/_size.h>
#include <common/_wchar.h>
#include <common/_null.h>


/* Macros. */
#define	_ATEXITN	32		/* number of atexit()-registered fns */
#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0
#define	MB_CUR_MAX	1
#define	RAND_MAX	32767

/* Types. */
typedef	struct { int quot; int rem; } div_t;	/* div result type	*/
typedef	struct { long quot; long rem; } ldiv_t;	/* ldiv result type	*/


/*
 * Functions in /lib/libc.a corresponding to ANSI <stdlib.h>.
 * N.B. The behavior of these functions is not guaranteed to
 * comply with the ANSI standard;
 * see the COHERENT documentation for details on each function.
 */

__EXTERN_C_BEGIN__

double		atof		__PROTO ((__CONST__ char * _nptr));
int		atoi		__PROTO ((__CONST__ char * _nptr));
long		atol		__PROTO ((__CONST__ char * _nptr));
double		strtod		__PROTO ((__CONST__ char * _nptr,
					  char ** _endptr));
long		strtol		__PROTO ((__CONST__ char * _nptr,
					  char ** _endptr, int _base));
unsigned long	strtoul		__PROTO ((__CONST__ char * _nptr,
					  char ** _endptr, int _base));
int		rand		__PROTO ((void));
void		srand		__PROTO ((unsigned _seed));
__VOID__      *	calloc		__PROTO ((size_t _nmemb, size_t _size));
void		free		__PROTO ((__VOID__ * _ptr));
__VOID__      *	malloc		__PROTO ((size_t _size));
__VOID__      *	realloc		__PROTO ((__VOID__ * _ptr, size_t _size));
__NO_RETURN__	abort		__PROTO ((void));
int		atexit		__PROTO ((void (* _func) (void)));
__NO_RETURN__	exit		__PROTO ((int _status));
char	      *	getenv		__PROTO ((__CONST__ char * _name));
int		system		__PROTO ((__CONST__ char * _string));
__VOID__      *	bsearch		__PROTO ((__CONST__ __VOID__ * _key,
					  __CONST__ __VOID__ * _base,
					  size_t _nmemb, size_t _size,
					  int (* _compar)
						(__CONST__ __VOID__ *,
						 __CONST__ __VOID__ *)));
void		qsort		__PROTO ((__VOID__ * _base, size_t _nmemb,
					  size_t _size,
					  int (* _compar)
						(__CONST__ __VOID__ *,
						 __CONST__ __VOID__ *)));
int		abs		__PROTO ((int _j));
div_t		div		__PROTO ((int _numer, int _denom));
long		labs		__PROTO ((long _j));
ldiv_t		ldiv		__PROTO ((long _numer, long _denom));

#if	0		/* not implemented */

int		mblen		__PROTO ((__CONST__ char * _s, size_t _n));
int		mbtowc		__PROTO ((wchar_t * _pwc, __CONST__ char * _s,
					  size_t _n));
int		wctomb		__PROTO ((char * _s, wchar_t _wchar));
size_t		mbstowcs	__PROTO ((wchar_t * _pwcs,
					  __CONST__ char * _s, size_t _n));
size_t		wcstombs	__PROTO ((char * _s,
					  __CONST__ wchar_t * _pwcs,
					  size_t _n));

#endif	/* not implemented */

#if	! _POSIX_C_SOURCE && ! _STDC_SOURCE

int		putenv		__PROTO ((char * _name));

#endif	/* ! _POSIX_C_SOURCE && ! _STDC_SOURCE */

__EXTERN_C_END__

#if	1

/* Internal data and functions. */
extern	int		_atexitn;
extern	void		(**_atexitfp)();
extern	double		_pow10	   ();

#endif		/* not permitted in this header */


#if	__GNUC__ && _I386

__LOCAL__ __INLINE__ div_t __div (int _numerator, int _denominator) {
	div_t		_result;
	__NON_ISO (asm) ("cltd;idiv %3" :
			 "=a" (_result.quot), "=d" (_result.rem) :
			 "0" (_numerator), "c" (_denominator));
	return _result;
}

__LOCAL__ __INLINE__ ldiv_t __ldiv (long _numerator, long _denominator) {
	ldiv_t		_result;
	__NON_ISO (asm) ("cltd;idiv %3" :
			 "=a" (_result.quot), "=d" (_result.rem) :
			 "0" (_numerator), "c" (_denominator));
	return _result;
}

#if	__cplusplus

__LOCAL__ __INLINE__ div_t div (int _numerator, int _denominator) {
	return __div (_numerator, _denominator);
}
__LOCAL__ __INLINE__ ldiv_t ldiv (long _numerator, long _denominator) {
	return __ldiv (_numerator, _denominator);
}

#else

#define	div(num, den)		__div (num, den)
#define	ldiv(num, den)		__ldiv (num, den)

#endif	/* ! __cplusplus */
#endif	/* ! (__GNUC__ && __I386__) */

#endif	/* ! defined (__STDLIB_H__) */
