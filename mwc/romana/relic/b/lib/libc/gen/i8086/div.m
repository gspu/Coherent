//////////
/ i8086 general utilities library.
/ div()
/ ANSI 4.10.6.2.
/ Division and remainder.
//////////

#include "larges.h"

//////////
/ #include <stdlib.h>
/ div_t div(int numer, int denom)
/
/ Compute quotient and remainder of 'numer' by 'denom'.
/ Return a pointer to a statically allocated div_t object.
//////////

	.prvd
quot:	.word	0
rem:	.word	0

numer	=	RASIZE
denom	=	2 + numer

	.shri
	.globl	div_

div_:
	mov	bx, sp
	mov	ax, Pss numer(bx)	/ Dividend to AX
	cwd				/ Sign-extend to DX:AX
	idiv	Pss denom(bx)		/ Divide by divisor
	Map	(ds, bx, $@quot)
	mov	bx, $quot		/ Pointer to result to DS:BX
	mov	(bx), ax		/ Store quotient
	mov	2(bx), dx		/ Store remainder
#if	LARGE
	mov	dx, ds
#endif
	mov	ax, bx			/ Return pointer in DX:AX
	Gret

/ end of div.m
