//////////
/ /usr/src/libm/i387/sin87.s
/ i387 mathematics library
/ sin(d)
//////////

RASIZE	=	4

	.globl	sin

//////////
/ double
/ sin(angle)
/ double angle;
//////////

d	=	RASIZE		/ arg offset

sin:
	fldl	d(%esp)		/ Load argument d.
	fsin			/ sin(d)
	ret

/ end of sin87.s
