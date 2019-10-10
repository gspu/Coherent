//////////
/ libm 8087
/ acos(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	acos_
	.globl	asin
	.globl	cfcc87
	.globl	edomain

//////////
/ double
/ acos(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

acos_:
	ifno8087(_acos_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

	fld	st		/ d, d
	fabs			/ abs(d), d
	fld1			/ 1, abs(d), d
	fcompp			/ Compare 1 to abs(d) and pop twice.
	Gcall	cfcc87
	jae	0f
	Gjmp	edomain		/ d < -1 or 1 < d, issue EDOM error.
0:
	Gcall	asin		/ asin(d)
	fild	cs:minus1	/ -1, asin(d)
	fldpi			/ pi, -1, asin(d)
	fscale			/ pi/2, -1, asin(d)
	fstp	st1		/ pi/2, asin(d)
	fsubr			/ acos(d) = pi/2 - asin(d)
	Gret

/ Data.
minus1:	.word	-1

/ end of acos87.m
