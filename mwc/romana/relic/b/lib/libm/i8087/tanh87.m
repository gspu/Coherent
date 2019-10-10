//////////
/ libm 8087
/ tanh(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	tanh_
	.globl	exp
	.globl	tstcc

//////////
/ double
/ tanh(d)
/ double d;
/
/ tanh(d) = (exp(d) - exp(-d)) / (exp(d) + exp(-d))
/	  = (1 - exp(-2 * d)) / (1 + exp(-2 * d))	(used for d > 0)
/	  = - (1 - exp(2 * d)) / (1 + exp(2 * d))	(used for d < 0)
//////////

d	=	RASIZE		/ arg offset

tanh_:
	ifno8087(_tanh_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

	Gcall	tstcc		/ Check sign of d.
	je	1f		/ d is zero, just return it.
	jb	0f		/ Jump if negative.
	fchs			/ Change positive to negative.

0:	pushf			/ Save the flags.
	fadd	st, st		/ 2 * d
	Gcall	exp		/ exp(2 * d)
	fld1			/ 1, exp(2 * d)
	fsub	st, st1		/ 1 - exp(2 * d), exp(2 * d)
	fld1			/ 1, 1 - exp(2 * d), exp(2 * d)
	faddp	st2, st		/ 1 - exp(2 * d), 1 + exp(2 * d)
	fdivr			/ (1 -  exp(2 * d)) / (1 + exp(2 * d))
	popf			/ Restore the flags.
	ja	1f		/ d was positive, done.
	fchs			/ d was negative, flip result sign.

1:	Gret
