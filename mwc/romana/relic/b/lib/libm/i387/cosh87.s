//////////
/ /usr/src/libm/i387/cosh87.s
/ i387 mathematics library
/ cosh(d), sinh(d)
//////////

RASIZE	=	4

	.globl	cosh
	.globl  sinh
	.globl	_exp

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

cosh:
	clc			/ Clear Carry to indicate cosh.
	jmp	.L0		/ Go to common code.

sinh:
	stc			/ Set Carry to indicate sinh.

.L0:
	fldl	d(%esp)		/ Load argument d.

	pushf			/ Save the Carry flag.
	call	_exp		/ exp(d)
	fld	%st		/ exp(d), exp(d)
	fld1			/ 1, exp(d), exp(d)
	fdiv	%st, %st(1)	/ 1/exp(d), exp(d)
	popf			/ Restore the flags.
	jnc	?1		/ Jump if cosh.
	fchs			/ -1/exp(d), exp(d) for cosh

?1:
	fadd			/ exp(d) +- 1/exp(d)
	fld1			/ 1, exp(d) +- 1/exp(d)
	fchs			/ -1, exp(d) +- 1/exp(d)
	fxch			/ exp(d) +- 1/exp(d), -1
	fscale			/ (exp(d) +- 1/exp(d)) / 2, -1
	fstp	%st(1)		/ (exp(d) +- 1/exp(d)) / 2
	ret

/ end of cosh87.s
