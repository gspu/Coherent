/* (-lgl
 * 	COHERENT Version 4.0.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * stdlib.h
 * C general utilities library header.
 * Draft Proposed ANSI C Standard, Section 4.10, 12/7/88 draft.
 */

#ifndef	__STDLIB_H__
#define	__STDLIB_H__

/* Macros. */
#define	_ATEXITN	32		/* number of atexit()-registered fns */
#define	EXIT_FAILURE	1
#define	EXIT_SUCCESS	0
#define	MB_CUR_MAX	1
#define	RAND_MAX	32767

/* Types. */
typedef	struct { int quot; int rem; } div_t;	/* div result type	*/
typedef	struct { long quot; long rem; } ldiv_t;	/* ldiv result type	*/
#ifndef	_SIZE_T
#define	_SIZE_T
typedef	unsigned int	size_t;		/* sizeof result type		*/
#endif
#ifndef	_WCHAR_T
#define	_WCHAR_T
typedef	char		wchar_t;	/* extended character set type	*/
#endif
#define	Void	char		/* Use "Void *" as generic pointer type	*/
#define	const			/* Ignore type modifier "const"		*/
#ifndef	NULL
#define	NULL	((char *)0)		/* null pointer constant	*/
#endif

/*
 * Functions in /lib/libc.a corresponding to ANSI <stdlib.h>.
 * N.B. The behavior of these functions is not guaranteed to
 * comply with the ANSI standard;
 * see the COHERENT documentation for details on each function.
 */
extern	void	abort(		/* void */				);
extern	int	abs(		/* int j */				);
extern	int	atexit(		/* void (*func)(void) */		);
extern	double	atof(		/* const char *nptr */			);
extern	int	atoi(		/* const char *nptr */			);
extern	long int atol(		/* const char *nptr */			);
extern	Void	*bsearch(	/* const void *key,
				   const void *base,
				   size_t nmemb,
				   size_t size,
				   int (*compar)(const void *, const void *) */ );
extern	Void	*calloc(	/* size_t nmemb, size_t size */		);
extern	div_t	div(		/* int numer, int denom */		);
extern	void	exit(		/* int status */			);
extern	void	free(		/* Void *ptr */				);
extern	char	*getenv(	/* const char *name */			);
extern	long int labs(		/* long int j */			);
extern	ldiv_t	ldiv(		/* long int numer, long int denom */	);
extern	Void	*malloc(	/* size_t size */			);
extern	void	qsort(		/* void *base, size_t nmemb, size_t size,
				int (*compar)(const void *, const void *) */ );
extern	int	rand(		/* void */				);
extern	Void	*realloc(	/* void *ptr, size_t size */		);
extern	void	srand(		/* unsigned int seed */			);
extern	double	strtod(		/* const char *nptr, char **endptr */	);
extern	long int strtol(	/* const char *nptr, char **endptr, int base */ );
extern	unsigned long strtoul(	/* const char *nptr, char **endptr, int base */ );
extern	int	system(		/* const char *string */		);

#if	0
/*
 * Functions in ANSI <stdlib.h> not currently implemented in COHERENT libc.a.
 */
extern	int	mblen(	/* const char *s, size_t n */			);
extern	int	mbtowc(	/* wchar_t *pwc, const char *s, size_t n */	);
extern	size_t	mbstowcs(/* wchar_t *pwcs, const char *s, size_t n */	);
extern	size_t	wcstombs(/* char *s, const wchar_t *pwcs, size_t n */	);
extern	int	wctomb(	/* char *s, wchar_t wchar */			);
#endif

/* Internal data and functions. */
extern	int		_atexitn;
extern	void		(**_atexitfp)();
extern	double		_pow10	   ();

#endif

/* end of stdlib.h */
