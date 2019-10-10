//////////
/ /usr/src/libm/i387/cos87.s
/ i387 mathematics library
/ cos(d)
//////////

RASIZE	=	4

	.globl	cos

//////////
/ double
/ cos(angle)
/ double angle;
//////////

d	=	RASIZE		/ arg offset

cos:
	fldl	d(%esp)		/ Load argument d.
	fcos			/ cos(d)
	ret

/ end of cos87.s
