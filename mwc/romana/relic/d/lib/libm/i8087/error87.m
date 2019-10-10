//////////
/ libm 8087
/ edomain, erange
//////////

#include "larges.h"

	.globl  edomain
	.globl	erange
	.globl	errno_

//////////
/ edomain replaces the 8087 stacktop with 0.0
/ and sets errno_ to the appropriate error code.
/ erange just sets the error code.
/ They clobber AX and DS in LARGE model.
//////////

EDOM	=	33		/ domain error code
ERANGE	=	34		/ range error code

edomain:
	Map	(ds, ax, $@errno_)
	mov	errno_, $EDOM	/ Set errno_.
	fstp	st		/ Pop the stack
	fldz			/ and load a 0.
	Gret

erange:
	Map	(ds, ax, $@errno_)
	mov	errno_, $ERANGE	/ Set errno_.
	Gret

/ end of error87.m
