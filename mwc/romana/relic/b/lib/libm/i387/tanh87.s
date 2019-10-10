//////////
/ /usr/src/libm/i387/tanh87.s
/ i387 mathematics library
/ tanh(d)
//////////

RASIZE	=	4

	.globl	tanh
	.globl	_exp
	.globl	_tstcc

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

tanh:
	fldl	d(%esp)		/ Load argument d.

	call	_tstcc		/ Check sign of d.
	je	?1		/ d is zero, just return it.
	jb	?0		/ Jump if negative.
	fchs			/ Change positive to negative.

?0:
	pushf			/ Save the flags.
	fadd	%st, %st	/ 2 * d
	call	_exp		/ exp(2 * d)
	fld1			/ 1, exp(2 * d)
	fsub	%st, %st(1)	/ 1 - exp(2 * d), exp(2 * d)
	fld1			/ 1, 1 - exp(2 * d), exp(2 * d)
	faddp	%st(2), %st	/ 1 - exp(2 * d), 1 + exp(2 * d)
	fdiv			/ (1 -  exp(2 * d)) / (1 + exp(2 * d))
	popf			/ Restore the flags.
	ja	?1		/ d was positive, done.
	fchs			/ d was negative, flip result sign.

?1:
	ret

/ end of tanh87.s
