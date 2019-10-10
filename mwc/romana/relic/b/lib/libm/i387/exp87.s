//////////
/ /usr/src/libm/i387/exp87.s
/ i387 mathematics library
/ exp(d), _two()
//////////

RASIZE	=	4

	.globl	exp
	.globl	_exp
	.globl	_two
	.globl	_floor
	.globl	_cfcc
	.globl	_erange

//////////
/ double
/ exp(d)
/ double d;
/
/ _exp() takes d on the NDP stacktop %st and returns exp(d).
/ Uses: exp(x) = 2 ^ (x * log base 2 of e).
/
/ _two() takes d on the NDP stacktop %st and returns 2^d.
//////////

d	=	RASIZE		/ arg offset

exp:
	fldl	d(%esp)		/ Load argument d.

_exp:				/ d
	fldl2e			/ Load log base 2 of e.
	fmul			/ d * log base 2 of e

_two:
	ficoms	maxexp
	call	_cfcc		/ Compare d to max exponent.
	jbe	?1		/ d is not too big.
	fstp	%st		/ Too big, pop d.
	call	_erange		/ Set errno to indicate out of range
	fldl	infin		/ and return infinity.
	ret

?1:
	ficoms	minexp
	call	_cfcc		/ Compare d to min exponent.
	jae	?2		/ d is not too small.
	fstp	%st		/ Too small, pop d
	fldz			/ and return 0.
	ret

?2:
	fld	%st		/ d, d
	call	_floor		/ Round d down to int(d), d
	fst	%st(2)		/ int(d), d, int(d)
	fsubr			/ d-int(d) = fraction(d), int(d)
	fcoml	half
	call	_cfcc		/ Compare fraction(d) to .5
	jbe	?3		/ fraction(d) <= .5, ok.
	fsubl	half		/ Set fraction(d) -= .5.

?3:
	f2xm1			/ 2^fraction(d) - 1
	fld1
	fadd			/ 2^fraction(d)
	jbe	?4		/ Was not >.5, ok as is.
	fmull	sqrt2		/ 2^fr(d) = 2^(fr(d)-.5) * 2^.5

?4:
	fscale			/ Scale in the integer part
	fstp	%st(1)		/ and pop, leaving result.
	ret

//////////
/ Data.
//////////
sqrt2:	.byte	0xCD, 0x3B, 0x7F, 0x66, 0x9E, 0xA0, 0xF6, 0x3F	/ sqrt(2.)
infin:	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F	/ infinity
half:	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F	/ .5
maxexp:	.word	1023		/ maximum exponent
minexp:	.word	-1022		/ minimum exponent

/ end of exp87.s
