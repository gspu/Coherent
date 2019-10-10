/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
/*
 * Structure for a setjmp environment.
 */

#ifndef	__SETJMP_H__
#define	__SETJMP_H__

#include <common/feature.h>
#include <common/ccompat.h>

#if	_SYSV4

#define	_SIGJBLEN	128
#define	_JBLEN		10

#elif	_SYSV3 || _I386

#define	_SIGJBLEN	8
#define	_JBLEN		6

#else

#error	Do not know how much space a jmp_buf occupies

#endif

typedef	int	jmp_buf [_JBLEN];

#if	_SIGJBLEN
typedef	int	sigjmp_buf [_SIGJBLEN];
#endif

__EXTERN_C_BEGIN__

int		setjmp		__PROTO ((jmp_buf _env));
__NO_RETURN__	longjmp		__PROTO ((jmp_buf _env, int _val));

#if	! _STDC_SOURCE && _SIGJBLEN

int		sigsetjmp	__PROTO ((sigjmp_buf _env, int _savemask));
__NO_RETURN__	siglongjmp	__PROTO ((sigjmp_buf _env, int _val));

#endif

__EXTERN_C_END__

#endif	/* ! defined (__SETJMP_H__) */
