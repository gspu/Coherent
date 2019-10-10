//////////
/ libm 8087
/ asin(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	asin_
	.globl	asin
	.globl	atan
	.globl	cfcc87
	.globl	edomain

//////////
/ double
/ asin(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

asin_:
	ifno8087(_asin_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

asin:				/ d
	fld	st		/ d, d
	fabs			/ abs(d), d
	fld1			/ 1, abs(d), d
	fcompp			/ Compare 1 to abs(d) and pop twice.
	Gcall	cfcc87
	ja	1f		/ -1 < d < 1, use atan.
	je	0f		/ d = 1 or d = -1
	Gjmp	edomain		/ d < -1 or 1 < d, issue EDOM error and return.

0:
	fild	cs:minus1	/ -1, d
	fldpi			/ pi, -1, d
	fscale			/ pi/2, -1, d
	fstp	st1		/ pi/2, d
	fmul			/ For d = 1 or d = -1, asin(d) = d * pi/2.
	Gret

1:	fld	st		/ d, d
	fmul	st, st		/ d*d, d
	fld1			/ 1, d*d, d
	fsubr			/ 1 - d*d, d
	fsqrt			/ sqrt(1-d*d), d
	fdiv			/ d / sqrt(1-d*d)
	Gjmp	atan		/ asin(d) = atan(d/sqrt(1-d*d))

/ Data.
minus1:	.word	-1

/ end of asin87.m
