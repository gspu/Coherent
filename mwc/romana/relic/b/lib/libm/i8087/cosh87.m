//////////
/ libm 8087
/ cosh(d), sinh(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	cosh_
	.globl  sinh_
	.globl	exp

//////////
/ double
/ cosh(d)
/ double d;
/
/ double
/ sinh(d)
/ double d;
/
/ cosh(d) = (exp(d) + 1/exp(d))/2
/ sinh(d) = (exp(d) - 1/exp(d))/2
//////////

d	=	RASIZE		/ arg offset

cosh_:
	ifno8087(_cosh_)
	clc			/ Clear Carry to indicate cosh.
	jmp	0f		/ Go to common code.

sinh_:
	ifno8087(_sinh_)
	stc			/ Set Carry to indicate sinh.

0:
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

	pushf			/ Save the Carry flag.
	Gcall	exp		/ exp(d)
	fld	st		/ exp(d), exp(d)
	fld1			/ 1, exp(d), exp(d)
	fdiv	st,st1		/ 1/exp(d), exp(d)
	popf			/ Restore the flags.
	jnc	1f		/ Jump if cosh.
	fchs			/ -1/exp(d), exp(d) for cosh

1:	fadd			/ exp(d) +- 1/exp(d)
	fidiv	cs:two		/ (exp(d) +- 1/exp(d)) / 2
	Gret

//////////
/ data
//////////
two:	.word	2
