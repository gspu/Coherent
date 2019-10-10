/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

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
 * feature-test macro called _POSIX_SOURCE, no value for this macro was
 * recommended, and so users typically #define this symbol to have no value,
 * making the #if form of feature-test more complex. Conversely, the ISO C
 * standard mandates a value for __STDC__, but users have uniformly ignored
 * this fact.
 *
 * This file mutates definitions of well-known feature tests so that the #if
 * form of test can be used universally.
 */

#define	__EMPTY(x)		((1 - x - 1) == 2)

#define	__UNDEFINED_OR_EMPTY(x)	(! defined (x) || __EMPTY (x))


/*
 * The POSIX.2 standard introduces a new feature-test symbol, _POSIX_C_SOURCE.
 * When given the value 1 or 2, the effect is the same as if _POSIX_SOURCE had
 * been defined as specified in the POSIX.1 standard. When given the value 2,
 * definitions mandanted by the POSIX.2 standard shall also be made visible.
 *
 * Below, we set things up so that headers can uniformly use _POSIX_C_SOURCE
 * as a feature-test without worrying about the nasty _POSIX_SOURCE problems.
 */

#if	defined (_POSIX_SOURCE) && ! _POSIX_C_SOURCE
# define	_POSIX_C_SOURCE	1
#endif

#if	_POSIX_C_SOURCE && ! defined (_POSIX_SOURCE)
# define	_POSIX_SOURCE	1
#endif


/*
 * We have a feature-test _STDC_SOURCE analagous to _POSIX_SOURCE which
 * selects a minimal ISO C compilation environment. This is mutually exclusive
 * with _POSIX_SOURCE. We could either complain or do something tricky like
 * select the one with the highest value.
 */

#if	_STDC_SOUCE || _POSIX_C_SOURCE
# if	_STDC_SOURCE > _POSIX_C_SOURCE

#  undef	_POSIX_C_SOURCE

# elif	_STDC_SOURCE < _POSIX_C_SOURCE

#  undef	_STDC_SOURCE

# else

#  error You cannot select both _POSIX_SOURCE and _STDC_SOURCE in a compilation

# endif
#endif


/*
 * System V Drivers typically use -D_KERNEL to active driver-specific stuff in
 * headers. Since the only SVR4 kernel code we support is DDI/DKI, we make the
 * DDI/DKI stuff visible.
 */

#if	__EMPTY (_KERNEL)
#undef	_KERNEL
#define	_DDI_DKI	1
#endif


/*
 * For Coherent: avoid the use of COHERENT as a feature-test, use __COHERENT__
 * instead. Using COHERENT as a feature-test is not recommended as a future
 * release of the Coherent tools will not supply it.
 *
 * Note that if this *is* Coherent, then we are part of version 4.2 at the
 * earliest.
 */

#if	defined (KERNEL) || defined (__KERNEL__)

#undef	KERNEL
#undef	__KERNEL__
#define	_KERNEL		1

#endif

#ifdef	COHERENT
#define	__COHERENT__	1
/* #undef COHERENT */
#endif


#if	__COHERENT__ == 1
#undef	__COHERENT__
#define	__COHERENT__	0x0420
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
