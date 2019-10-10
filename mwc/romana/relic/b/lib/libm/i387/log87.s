//////////
/ /usr/src/libm/i387/log87.s
/ i387 mathematics library
/ log(x), log10(x), ylog2x
//////////

RASIZE	=	4

	.globl  log10
	.globl	log
	.globl	_ylog2x
/	.globl	_cfcc
	.globl	_tstcc
	.globl	_edomain

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
/ Passed args x, y on the NDP stack,
/ it pops both and returns y * log base 2 of x on the NDP stacktop %st.
/
/ Useful identity:
/	 log base n of x = log base n of 2 * log base 2 of x.
/ Uses fyl2x, which requires x > 0.
//////////

x	=	RASIZE		/ arg offset

log10:
	fldlg2			/ Load y = log base 10 of 2.
	jmp	.L1

log:
	fldln2			/ Load y = log base e of 2.

.L1:
	fldl	x(%esp)		/ Load argument x.

_ylog2x:			/ x, y
	call	_tstcc		/ Check sign of x.
	ja	?2		/ x > 0., ok.
	fstp	%st		/ x <= 0, pop y.
	jmp	_edomain	/ Issue EDOM error and return 0.

?2:
//////////
/ The code below uses
/	fyl2x	if d <= 1 or d >= 2-sqrt(2)/2,
/	fyl2xp1	if 1 < d < 2-sqrt(2)/2.
/ Since the precision fyl2xp1 purports to save is already lost,
/ this appears to make no difference in the result.
/ The code is commented out for now and should be removed eventually.
/	fdcom	near1
/	call	_cfcc		/ Compare d to 2+sqrt(2)/2.
/	jae	?4		/ d >= 2+sqrt(2)/2, use fyl2x.
/	fld1			/ Load 1.
/	fcom
/	call	_cfcc		/ Compare 1 to d.
/	jae	?3		/ 1 >= d, use fyl2x.
/	fsub			/ d = d-1 and pop off the 1.
/	fyl2xp1			/ result = y * log base 2 of d+1.
/	ret
/
/?3:	fstp	%st		/ Pop off the 1.
/
/?4:
//////////
	fyl2x			/ result = y * log base 2 of x.
	ret

//////////
/ Commented out as above...
/near?1:	.byte	0x1A, 0x62, 0xC0, 0xCC, 0xB0, 0xAF, 0xF4, 0x3F	/ 2 - sqrt(2)/2
//////////

/ end of log87.s
