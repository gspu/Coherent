//////////
/ libm 8087
/ sqrt(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl  sqrt_
	.globl	tstcc
	.globl	edomain

//////////
/ double
/ sqrt(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

sqrt_:
	ifno8087(_sqrt_)
	mov	bx, sp
	fdld	Pss d(bx)	/ Load argument d.

	Gcall	tstcc		/ Test if d < 0.
	jae	0f		/ d >= 0.
	Gjmp	edomain		/ Issue EDOM error and return 0.

0:	fsqrt			/ result = sqrt(d)
	Gret
