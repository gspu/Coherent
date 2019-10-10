//////////
/ libm 8087
/ atan(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	atan_
	.globl	atan
	.globl	cfcc87
	.globl	tstcc

//////////
/ double
/ atan(d)
/ double d;
/
/ Uses fpatan, which requires 0 < st1 < st.
//////////

d	=	RASIZE		/ arg offset

atan_:
	ifno8087(_atan_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

atan:				/ d
	Gcall	tstcc
	je	3f		/ d = 0, just return it.
	pushf			/ Save the flags.
	ja	0f		/ d is positive.
	fchs			/ d is negative, negate it.

0:	fld1			/ Load a 1.
	fcom
	Gcall	cfcc87		/ Compare 1 to d.
	jae	1f		/ 1 >= d, ok.
	fdivr	st1, st		/ and set d = 1/d.

1:	fpatan			/ Result = atan(d/1).
	jae	2f		/ 1 >= d, OK as is.
	fild	cs:minus1	/ -1, result
	fldpi			/ pi, -1, result
	fscale			/ pi/2, -1, result
	fstp	st1		/ pi/2, result
	fsubr			/ Result = pi/2 - result.

2:	popf			/ Restore the sign flag.
	jae	3f		/ Positive, done.
	fchs			/ Result = -result.

3:	Gret

/ Data.
minus1:	.word	-1

/ end of atan87.m
