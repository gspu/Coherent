//////////
/ /usr/src/libm/i387/asin87.s
/ i387 mathematics library
/ asin(d)
//////////

RASIZE	=	4

	.globl	asin
	.globl	_asin
	.globl	_cfcc
	.globl	_edomain

//////////
/ double
/ asin(d)
/ double d;
/
/ _asin() returns asin(%st).
/ It sets the Carry flag iff a domain error occurs.
//////////

d	=	RASIZE		/ arg offset

asin:
	fldl	d(%esp)		/ Load argument d.

_asin:				/ d
	fld	%st		/ d, d
	fabs			/ abs(d), d
	fld1			/ 1, abs(d), d
	fcompp			/ Compare 1 to abs(d) and pop twice.
	call	_cfcc
	ja	?1		/ -1 < d < 1, use atan.
	je	?0		/ d = 1 or d = -1
	call	_edomain	/ d < -1 or 1 < d, issue EDOM error
	stc			/ and set the Carry flag
	ret			/ and return.

?0:
	fld1			/ 1, d
	fchs			/ -1, d
	fldpi			/ pi, -1, d
	fscale			/ pi/2, -1, d
	fstp	%st(1)		/ pi/2, d
	fmul			/ For d = 1 or d = -1, asin(d) = d * pi/2.
	clc			/ Clear the Carry flag.
	ret

?1:
	fld	%st		/ d, d
	fmul	%st, %st	/ d*d, d
	fld1			/ 1, d*d, d
	fsub			/ 1 - d*d, d
	fsqrt			/ sqrt(1-d*d), d
	fpatan			/ asin(d) = atan(d/sqrt(1-d*d))
	clc			/ Clear the Carry flag.
	ret

/ end of asin87.s
