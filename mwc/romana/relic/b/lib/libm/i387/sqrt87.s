//////////
/ /usr/src/libm/i387/sqrt87.s
/ i387 mathematics library
/ sqrt(d)
//////////

RASIZE	=	4

	.globl  sqrt
	.globl	_tstcc
	.globl	_edomain

//////////
/ double
/ sqrt(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

sqrt:
	fldl	d(%esp)		/ Load argument d.

	call	_tstcc		/ Test if d < 0.
	jae	?0		/ d >= 0.
	jmp	_edomain	/ Issue EDOM error and return 0.

?0:
	fsqrt			/ result = sqrt(d)
	ret

/ end of sqrt87.s
