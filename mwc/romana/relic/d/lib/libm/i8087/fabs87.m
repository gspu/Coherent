//////////
/ libm 8087
/ fabs(d)
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl  fabs_

//////////
/ double
/ fabs(d)
/ double d;
//////////

d	=	RASIZE		/ arg offset

fabs_:
	ifno8087(_fabs_)
	mov	bx, sp

	fdld	Pss d(bx)	/ Load argument d
	fabs			/ and perform abs.

	Gret
