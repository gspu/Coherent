/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1983, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * setjmp.h
 * Structure for a setjmp environment.
 * i8086 SMALL model:	saves 3 words (SP, BP, return PC).
 * i8086 LARGE model:	saves 4 words (SP, BP, return PC segment:offset).
 * i386:		saves 6 dwords (EBP, ESP, return PC, ESI, EDI, EBX).
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

#define	_JBLEN		4

#endif

typedef	int	jmp_buf [_JBLEN];

#if	_SIGJBLEN
typedef	int	sigjmp_buf [_SIGJBLEN];
#endif

__EXTERN_C_BEGIN__

int		setjmp		__PROTO ((jmp_buf _env));
__NO_RETURN__ void
		longjmp		__PROTO ((jmp_buf _env, int _val));

#if	! _STDC_SOURCE && _SIGJBLEN

int		sigsetjmp	__PROTO ((sigjmp_buf _env, int _savemask));
__NO_RETURN__ int
		siglongjmp	__PROTO ((sigjmp_buf _env, int _val));

#endif

__EXTERN_C_END__

#endif	/* ! defined (__SETJMP_H__) */
