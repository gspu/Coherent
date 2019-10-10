/* (-lgl
 * 	COHERENT Version 3.2.1
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * setjmp.h
 * Structure for a setjmp environment.
 * i8086 SMALL model:	saves 3 words (SP, BP, return PC).
 * i8086 LARGE model:	saves 4 words (SP, BP, return PC segment:offset).
 * i386:		saves 6 dwords (EBP, ESP, return PC, ESI, EDI, EBX).
 */

#ifndef	SETJMP_H
#define	SETJMP_H	SETJMP_H

#if	_I386
typedef	int	jmp_buf[6];
#else
typedef	int	jmp_buf[4];
#endif

#endif

/* end of setjmp.h */
