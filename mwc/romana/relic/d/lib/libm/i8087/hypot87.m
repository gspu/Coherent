//////////
/ libm 8087
/ cabs(z), hypot(x, y)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl	cabs_
	.globl	hypot_

//////////
/ double
/ cabs(z)
/ CPX z;
/
/ hypot(x, y)
/ double x, y;
//////////

x	=	RASIZE		/ x arg offset
y	=	RASIZE+8	/ y arg offset

cabs_:
hypot_:
	ifno8087(_cabs_)
	mov	bx, sp

	fdld	Pss x(bx)	/ x
	fmul	st, st		/ x*x
	fdld	Pss y(bx)	/ y, x*x
	fmul	st, st		/ y*y, x*x
	fadd			/ x*x + y*y
	fsqrt			/ result = sqrt(x*x + y*y)

	Gret
