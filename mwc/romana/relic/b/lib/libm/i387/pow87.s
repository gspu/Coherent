//////////
/ /usr/src/libm/i387/pow87.s
/ i387 mathematics library
/ pow(x, y)
//////////

RASIZE	=	4

	.globl	pow
	.globl	_pow
	.globl	_two
	.globl	_ylog2x
	.globl	_cfcc
	.globl	_tstcc
	.globl	_edomain

//////////
/ double
/ pow(x, y)
/ double x, y;
/
/ _pow takes x, y on the  NDP stack.
//////////
x	=	RASIZE		/ x arg offset
y	=	RASIZE+8	/ y arg offset

pow:
	fldl	y(%esp)		/ Load argument y.
	fldl	x(%esp)		/ Load argument x.

_pow:				/ x, y
	call	_tstcc		/ Test if x == 0.
	je	?0		/ x is zero.
	jb	?1		/ x is negative.
	subl	%eax, %eax	/ x is positive, clear ZF flag
	jmp	?4		/ to indicate result sign positive.

?0:
	fxch			/ y, 0
	call	_tstcc		/ Compare y to 0.
	fstp	%st		/ Pop y, leaving 0.
	jbe	?2		/ y <= 0, issue EDOM error and return 0.
	ret			/ y > 0, return 0.

?1:
	fld	%st(1)		/ y, x, y with x negative.
	call	tstintp		/ Test if y is integral and pop.
	je	?3		/ y is integral.
	fstp	%st		/ y is nonintegral, pop x
?2:
	jmp	_edomain	/ and issue EDOM error and return.

?3:
	fchs			/ x is negative, force it positive.
	fld1			/ 1, x, y
	fadd	%st, %st	/ 2, x, y
	fdivr	%st, %st(2)	/ y/2, x, y
	call	tstintp		/ Test if y/2 is integral.

/ At 4:, ZF flag contains the desired result sign.
/ The NDP stack contains abs(x), y.
?4:
	pushf			/ Save the flags.
	call	_ylog2x		/ y * log base 2 of x
	call	_two		/ 2 ^ (y * log base 2 of x) = x ^ y
	popf			/ Restore the flags.
	je	?5		/ No sign change, done.
	fchs			/ Flip the result sign.

?5:
	ret

//////////
/ tstintp tests whether the NDP stacktop %st contains
/ an integral value and sets ZF flag accordingly.
/ It pops the NDP stack before returning.
/ This works with any of the four NDP rounding modes.
//////////
tstintp:			/ x
	fld	%st		/ x, x
	frndint			/ int(x), x
	fcompp			/ Compare int(x) to x and pop both.
	jmp	_cfcc		/ Load condition codes and return.

/ end of pow87.s
