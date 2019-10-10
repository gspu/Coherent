//////////
/ /usr/src/libm/i387/fabs87.s
/ i387 mathematics library
/ fabs(d)
//////////

RASIZE	=	4

	.globl  fabs

//////////
/ double
/ fabs(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

fabs:
	fldl	d(%esp)		/ Load argument d
	fabs			/ and perform abs.
	ret

/ end of fabs87.s
