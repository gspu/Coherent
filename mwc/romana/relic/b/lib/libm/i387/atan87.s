//////////
/ /usr/src/libm/i387/atan87.s
/ i387 mathematics library
/ atan(d)
//////////

RASIZE	=	4

	.globl	atan

//////////
/ double
/ atan(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

atan:
	fldl	d(%esp)		/ Load argument d.
	fld1			/ 1.0, d.
	fpatan			/ Result = atan(d/1).
	ret

/ end of atan87.s
