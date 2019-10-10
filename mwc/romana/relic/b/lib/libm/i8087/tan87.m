//////////
/ libm 8087
/ tan(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	tan_
	.globl	tan
	.globl	cfcc87
	.globl	tstcc

//////////
/ double
/ tan(angle)
/ double angle;
//////////

d	=	RASIZE		/ arg offset

tan_:
	ifno8087(_tan_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

	Gcall	tan		/ Get x, y.
	fdiv			/ tan  = y / x.
	Gret

//////////
/ tan() is passed an argument d on the 8087 stack
/ and returns x and y on 8087 stack.
/
/ tan() reduces angle d to octant 0,
/ uses fptan to get a sine y and cosine x,
/ and puts x and y back to the correct octant.
/
/ Uses fptan, which requires 0 <= d < pi/4.
//////////

tan:				/ d
	fild	cs:minus2	/ -2, d
	fldpi			/ pi, -2, d
	fscale			/ pi/4, -2, d
	fstp	st1		/ pi/4, d
	fxch			/ d, pi/4

0:	Gcall	tstcc		/ Is d < 0.?
	jnb	1f		/ No, ok.
	fldpi
	fld	st
	fadd
	fadd			/ Add 2 * pi.
	jmp	0b		/ Repeat until d is positive.

1:	fprem			/ Reduce angle to octant 0.
	Gcall	cfcc87		/ Set condition codes.
	jpe	1b		/ Repeat until fully reduced.
	fstp	st1		/ Pop pi/4, retain reduced arg.
	fptan			/ x = r*cos(arg), y=r*sin(arg)

/ cos(pi+d) = -cos(d), sin(pi+d) = -sin(d).
	jnc	2f		/ Test for c0 set by cfcc87 above.
	fchs			/ Octants 4, 5, 6, 7
	fxch			/ need both
	fchs			/ signs
	fxch			/ changed.

/ cos(pi/2+d) = -sin(d), sin(pi/2+d) = cos(d).
2:	jnz	3f		/ Test for c3 set by cfcc87 above.
	fxch			/ Octants 2, 3, 6, 7
	fchs			/ need switch and sign change.

/ cos(pi/4+d) = (cos(d)-sin(d))*k, sin(pi/4+d) = (cos(d)+sin(d))*k.
3:	testb	ah, $2		/ Test for c1 set by cfcc87 above.
	jz	4f		/ Done.
	fld	st		/ Octants 1, 3, 5, 7
	fld	st2		/ need x-y and x+y.
	fadd			/ x+y, x, y
	fxch	st2		/ y, x, x+y
	fsub			/ x-y, x+y

4:	Gret

//////////
/ data
//////////
minus2:	.word	-2
