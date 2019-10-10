//////////
/ libm 8087
/ pow(x, y)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	pow_
	.globl	two
	.globl	ylog2x
	.globl	cfcc87
	.globl	tstcc
	.globl	edomain

//////////
/ double
/ pow(x, y)
/ double x, y;
/
/ pow is passed x, y on 8087 stack.
//////////

x	=	RASIZE		/ x arg offset
y	=	RASIZE+8	/ y arg offset

pow_:
	ifno8087(_pow_)
	mov	bx, sp
	fdld	Pss y(bx)	/ Load argument y.
	fdld	Pss x(bx)	/ Load argument x.

pow:				/ x, y
	Gcall	tstcc		/ Test if x == 0.
	je	0f		/ x is zero.
	jb	1f		/ x is negative.
	sub	ax, ax		/ x is positive, clear ZF flag
	jmp	4f		/ to indicate result sign positive.

0:	fxch			/ y, 0
	Gcall	tstcc		/ Compare y to 0.
	fstp	st		/ Pop y, leaving 0.
	jbe	2f		/ y <= 0, issue EDOM error and return 0.
	Gret			/ y > 0, return 0.

1:	fld	st1		/ y, x, y with x negative.
	Gcall	tstintp		/ Test if y is integral and pop.
	je	3f		/ y is integral.
	fstp	st		/ y is nonintegral, pop x
2:	Gjmp	edomain		/ and issue EDOM error and return.

3:	fchs			/ x is negative, force it positive.
	fld1			/ 1, x, y
	fadd	st, st		/ 2, x, y
	fdivr	st, st2		/ y/2, x, y
	Gcall	tstintp		/ Test if y/2 is integral.

/ At 4:, ZF flag contains the desired result sign.
/ The 8087 stack contains x > 0, y.
4:	pushf			/ Save the flags.
	Gcall	ylog2x		/ y * log base 2 of x
	Gcall	two		/ 2 ^ (y * log base 2 of x) = x ^ y
	popf			/ Restore the flags.
	je	5f		/ No sign change, done.
	fchs			/ Flip the result sign.

5:	Gret

//////////
/ tstintp tests whether the 8087 stacktop contains
/ an integral value and sets ZF flag accordingly.
/ It pops the 8087 stack before returning.
/ This works with any of the four 8087 rounding modes.
//////////
tstintp:			/ x
	fld	st		/ x, x
	frndint			/ int(x), x
	fcompp			/ Compare int(x) to x and pop both.
	Gjmp	cfcc87		/ Load condition codes and return.
