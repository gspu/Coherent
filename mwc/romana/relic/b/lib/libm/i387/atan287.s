//////////
/ /usr/src/libm/i387/atan287.s
/ i387 mathematics library
/ atan2(y, x)
//////////

RASIZE	=	4

	.globl	atan2
	.globl	_cfcc
	.globl	_tstcc

//////////
/ double
/ atan2(y, x)
/ double y, x;
//////////

y	=	RASIZE		/ y arg offset
x	=	RASIZE+8	/ x arg offset

atan2:
	fldl	y(%esp)		/ Load argument y.
	fldl	x(%esp)		/ Load argument x.

	call	_tstcc
	jne	?1		/ Jump if x nonzero.
	fcompp			/ x = 0, compare 0 to y and pop x and y.
	call	_cfcc
	fld1			/ 1
	fchs			/ -1
	fldpi			/ pi, -1
	fscale			/ pi/2, -1
	fstp	%st(1)		/ pi/2
	jbe	?0		/ 0 <= y, return pi/2.
	fchs			/ 0 > y, return -pi/2.

?0:
	ret

?1:
	fpatan			/ atan(y/x)
	ret

/ end of atan287.s
