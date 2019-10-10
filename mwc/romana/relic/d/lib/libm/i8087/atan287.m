//////////
/ libm 8087
/ atan2(y, x)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	atan2_
	.globl	atan
	.globl	cfcc87
	.globl	tstcc

//////////
/ double
/ atan2(y, x)
/ double y, x;
//////////

y	=	RASIZE		/ y arg offset
x	=	RASIZE+8	/ x arg offset

atan2_:
	ifno8087(_atan2_)
	mov	bx, sp
	fdld	Pss y(bx)	/ Load argument y.
	fdld	Pss x(bx)	/ Load argument x.

atan2:				/ x, y
	Gcall	tstcc
	jne	1f		/ Jump if x nonzero.
	fcompp			/ x = 0, compare 0 to y and pop x and y.
	Gcall	cfcc87
	fild	cs:minus1	/ -1
	fldpi			/ pi, -1
	fscale			/ pi/2, -1
	fstp	st1		/ pi/2
	jbe	0f		/ 0 <= y, return pi/2.
	fchs			/ 0 > y, return -pi/2.

0:	Gret

1:	pushf			/ Save flags with sign of x.
	fld	st1		/ y, x, y
	fdivr			/ y/x, y
	Gcall	atan		/ atan(y/x), y
	popf			/ Restore flags.
	jb	2f		/ x < 0, must adjust by pi.
	fstp	st1		/ atan(y/x)
	Gret

2:	fxch			/ y, atan(y/x)
	Gcall	tstcc
	fstp	st		/ atan(y/x)
	fldpi			/ pi, atan(y/x)
	jae	3f		/ y >= 0, add pi.
	fchs			/ y < 0, subtract pi.

3:	fadd			/ atan(y/x) + pi
	Gret

/ Data.
minus1:	.word	-1

/ end of atan287.m
