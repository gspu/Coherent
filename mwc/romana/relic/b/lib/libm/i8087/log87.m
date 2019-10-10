//////////
/ libm 8087
/ log(x), log10(x), ylog2x
//////////

#include "larges.h"
#include "ifno8087.h"

	.globl  log10_
	.globl	log_
	.globl	ylog2x
	.globl	cfcc87
	.globl	tstcc
	.globl	edomain

//////////
/ double
/ log(x)
/ double x;
/
/ double
/ log10(x)
/ double x;
/
/ ylog2x does the work for log functions and pow().
/ Passed args x, y on the 8087 stack,
/ it pops both and returns y * log base 2 of x on the 8087 stack.
/
/ Useful identity:
/	 log base n of x = log base n of 2 * log base 2 of x.
/ Uses fyl2x, which requires x > 0.
//////////

x	=	RASIZE		/ arg offset

log10_:
	ifno8087(_log10_)
	fldlg2			/ Load y = log base 10 of 2.
	jmp	1f

log_:
	ifno8087(_log_)
	fldln2			/ Load y = log base e of 2.

1:	mov	bx, sp
	fdld	Pss x(bx)	/ Load argument x.

ylog2x:				/ x, y
	Gcall	tstcc		/ Check sign of x.
	ja	2f		/ x > 0., ok.
	fstp	st		/ x <= 0, pop y.
	Gjmp	edomain		/ Issue EDOM error and return 0.

2:
//////////
/ The code below uses
/	fyl2x	if d <= 1 or d >= 2-sqrt(2)/2,
/	fyl2xp1	if 1 < d < 2-sqrt(2)/2.
/ Since the precision fyl2xp1 purports to save is already lost,
/ this appears to make no difference in the result.
/ The code is commented out for now and should be removed eventually.
/	fdcom	cs:near1
/	Gcall	cfcc87		/ Compare d to 2+sqrt(2)/2.
/	jae	4f		/ d >= 2+sqrt(2)/2, use fyl2x.
/	fld1			/ Load 1.
/	fcom
/	Gcall	cfcc87		/ Compare 1 to d.
/	jae	3f		/ 1 >= d, use fyl2x.
/	fsub			/ d = d-1 and pop off the 1.
/	fyl2xp1			/ result = y * log base 2 of d+1.
/	Gret
/
/3:	fstp	st		/ Pop off the 1.
/
/4:
//////////
	fyl2x			/ result = y * log base 2 of x.
	Gret

//////////
/ Commented out as above...
/near1:	.byte	0x1A, 0x62, 0xC0, 0xCC, 0xB0, 0xAF, 0xF4, 0x3F	/ 2 - sqrt(2)/2
//////////
