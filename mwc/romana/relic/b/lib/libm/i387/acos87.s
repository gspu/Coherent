//////////
/ /usr/src/libm/i387/acos87.s
/ i387 mathematics library
/ acos(d)
//////////

RASIZE	=	4

	.globl	acos
	.globl	_asin

//////////
/ double
/ acos(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

acos:
	fldl	d(%esp)		/ Load argument d.

	call	_asin		/ asin(d)
	jc	?0		/ d < -1 or 1 < d, domain error.
	fld1			/ 1, asin(d)
	fchs			/ -1, asin(d)
	fldpi			/ pi, -1, asin(d)
	fscale			/ pi/2, -1, asin(d)
	fstp	%st(1)		/ pi/2, asin(d)
	fsub			/ acos(d) = pi/2 - asin(d)

?0:
	ret

/ end of acos87.s
