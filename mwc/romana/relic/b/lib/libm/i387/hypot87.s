//////////
/ /usr/src/libm/i387/hypot.s
/ i387 mathematics library
/ cabs(z), hypot(x, y)
//////////

RASIZE	=	4

	.globl	cabs
	.globl	hypot

//////////
/ double
/ cabs(z)
/ CPX z;
/
/ hypot(x, y)
/ double x, y;
//////////

x	=	RASIZE		/ x arg offset
y	=	RASIZE+8	/ y arg offset

cabs:
hypot:
	fldl	x(%esp)		/ x
	fmul	%st, %st	/ x*x
	fldl	y(%esp)		/ y, x*x
	fmul	%st, %st	/ y*y, x*x
	fadd			/ x*x + y*y
	fsqrt			/ result = sqrt(x*x + y*y)

	ret

/ end of hypot87.s
