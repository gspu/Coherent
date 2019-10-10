//////////
/ MWC86 runtime.
/ Transform a long to canonical format.
//////////

////////
/ long _canl(l);
/ long l;
////////

#include <larges.h>

low	=	RASIZE
high	=	low+2

	.globl	_canl_

_canl_:
	mov	bx, sp
	mov	dx, Pss low(bx)		/ lo part of arg is hi return value
	mov	ax, Pss high(bx)	/ hi part of arg is lo return value
	Gret

/ end of canon.m
