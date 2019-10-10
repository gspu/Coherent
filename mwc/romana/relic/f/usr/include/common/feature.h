#ifndef	__COMMON_FEATURE_H__
#define	__COMMON_FEATURE_H__

/*
 * This header file deals with some minor complications that exist in the
 * world of feature-tests. Standard style for new code is to use the simpler
 * and more powerful #if test rather than the cumbersome and less useful
 * #ifdef/#ifndef tests that untutored programmers gravitate to.
 *
 * However, this is made more difficult by historical problems, and some
 * latitude extended to users. For example, while POSIX.1 introduces a
 * feature-test macro such as _POSIX_SOURCE, no value for this macro was
 * recommended, and so users typically #define this symbol to have no value,
 * making the #if form of feature-test more complex. Conversely, the ISO C
 * standard mandates a value for __STDC__, but users have uniformly ignored
 * this fact.
 *
 * This file mutates definitions of well-known feature tests so that the #if
 * form of test can be used universally. This means using several tricks to
 * detect such things as symbols with empty values, but such tricks break down
 * if users are perverse and #define symbols like _POSIX_SOURCE with values
 * that are strings or which cannot be evaluated. For such symbols, we have
 * an option which will simply force the feature to value 1 rather than trying
 * to be clever. This means that some utility is lost (in the sense that
 * features cannot have graduated values for purposes such as selecting
 * versions) so this is off by default.
 */

#define	__EMPTY(x)		((1 - x - 1) == 2)

#define	__UNDEFINED_OR_EMPTY(x)	(! defined (x) || __EMPTY (x))

#ifdef	_FORCE_TO_ONE
# undef		_FORCE_TO_ONE
# define	_FORCE_TO_ONE	1
#endif

#if	defined (_POSIX_SOURCE)
# if	_FORCE_TO_ONE

#  undef	_POSIX_SOURCE
#  define	_POSIX_SOURCE	1

# elif	__EMPTY (_POSIX_SOURCE)

#  undef	_POSIX_SOURCE
#  define	_POSIX_SOURCE	1

#endif
#endif



/*
 * We have a feature-test _STDC_SOURCE analagous to _POSIX_SOURCE which
 * selects a minimal ISO C compilation environment. This is mutually exclusive
 * with _POSIX_SOURCE. We could either complain or do something tricky like
 * select the one with the highest value.
 */

#if	_STDC_SOUCE || _POSIX_SOURCE
# if	_STDC_SOURCE > _POSIX_SOURCE

#  undef	_POSIX_SOURCE

# elif	_STDC_SOURCE < _POSIX_SOURCE

#  undef	_STDC_SOURCE

# else

#  error You cannot select both _POSIX_SOURCE and _STDC_SOURCE in a compilation

# endif
#endif

/*
 * For Coherent: avoid the use of COHERENT as a feature-test, use __COHERENT__
 * instead.
 */

#ifdef	KERNEL

#undef	KERNEL
#undef	__KERNEL__
#define	__KERNEL__	1

#endif

#ifdef	COHERENT

#undef	__COHERENT__
#define	__COHERENT__	1

#endif

/*
 * The MWC port of GCC botches -ansi mode by not defining _I386; here we put
 * it back in again. This also serves to define _I386 for those systems that
 * traditionally just define "i386".
 */

#if	__GNUC__ && (defined (i386) || __COHERENT__) && ! defined (_I386)
# define	_I386	1
#endif

#endif	/* ! defined (__COMMON_FEATURE_H__) */
