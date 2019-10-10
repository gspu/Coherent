//////////
/ libm 8087
/ cos(d), sin(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	cos_
	.globl	sin_
	.globl	tan

//////////
/ double
/ cos(angle)
/ double angle;
/
/ double
/ sin(angle)
/ double angle;
//////////

d	=	RASIZE		/ arg offset

cos_:
	ifno8087(_cos_)
	clc			/ Clear Carry for cos.
	jmp	0f

sin_:
	ifno8087(_sin_)
	stc			/ Set Carry for sin.

0:	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

	pushf			/ Save the flags.
	Gcall	tan		/ Get x, y.
	fld	st		/ x, x, y
	fmul	st,st		/ x*x, x, y
	fld	st2		/ y, x*x, x, y
	fmul	st,st		/ y*y, x*x, x, y
	fadd			/ x*x + y*y, x, y
	fsqrt			/ r = sqrt(x*x + y*y), x, y
	popf			/ Restore the flags.
	jc	1f		/ Carry set for sin.
	fdiv			/ cos(d) = x / r, y.
	fstp	st1		/ Pop the stack, return result.
	Gret

1:	fstp	st1		/ r, y.
	fdiv			/ sin(d) = y / r
	Gret
