//////////
/ libm 8087
/ exp(d), two()
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	exp_
	.globl	exp
	.globl	two
	.globl	floor
	.globl	cfcc87
	.globl	erange

//////////
/ double
/ exp(d)
/ double d;
/
/ exp() is passed d on the 8087 stack and returns exp(d).
/ Uses: exp(x) = 2 ^ (x * log base 2 of e).
/
/ two() is passed d on the 8087 stack and returns 2^d.
//////////

d	=	RASIZE		/ arg offset

exp_:
	ifno8087(_exp_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

exp:				/ d
	fldl2e			/ Load log base 2 of e.
	fmul			/ d * log base 2 of e

two:
	ficom	cs:maxexp
	Gcall	cfcc87		/ Compare d to max exponent.
	jbe	1f		/ d is not too big.
	fstp	st		/ Too big, pop d.
	Gcall	erange		/ Set errno to indicate out of range
	fdld	cs:infin	/ and return infinity.
	Gret

1:	ficom	cs:minexp
	Gcall	cfcc87		/ Compare d to min exponent.
	jae	2f		/ d is not too small.
	fstp	st		/ Too small, pop d
	fldz			/ and return 0.
	Gret

2:	fld	st		/ d, d
	Gcall	floor		/ Round d down to int(d), d
	fst	st2		/ int(d), d, int(d)
	fsub			/ d-int(d) = fraction(d), int(d)
	fdcom	cs:half
	Gcall	cfcc87		/ Compare fraction(d) to .5
	jbe	3f		/ fraction(d) <= .5, ok.
	fdsub	cs:half		/ Set fraction(d) -= .5.

3:	f2xm1			/ 2^fraction(d) - 1
	fld1
	fadd			/ 2^fraction(d)
	jbe	4f		/ Was not >.5, ok as is.
	fdmul	cs:sqrt2	/ 2^fr(d) = 2^(fr(d)-.5) * 2^.5

4:	fscale			/ Scale in the integer part
	fstp	st1		/ and pop, leaving result.
	Gret

//////////
/ data
//////////
sqrt2:	.byte	0xCD, 0x3B, 0x7F, 0x66, 0x9E, 0xA0, 0xF6, 0x3F	/ sqrt(2.)
infin:	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7F	/ infinity
half:	.byte	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F	/ .5
maxexp:	.word	1023	/ maximum exponent
minexp:	.word	-1022	/ minimum exponent
