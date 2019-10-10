//////////
/ /usr/src/libm/i387/tan87.s
/ i387 mathematics library
/ tan(d)
//////////

RASIZE	=	4

	.globl	tan

//////////
/ double
/ tan(angle)
/ double angle;
//////////

d	=	RASIZE		/ arg offset

tan:
	fldl	d(%esp)		/ Load argument d.
	fptan			/ tan(d) in %st1, 1.0 in %st
	fstp	%st		/ tan(d)
	ret

/ end of tan87.s
