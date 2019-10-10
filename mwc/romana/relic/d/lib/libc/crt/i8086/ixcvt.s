////////
/
/ Intel 8086 floating point.
/ double to int and unsigned int.
/ small model.
/
////////

	.globl	idcvt
	.globl	udcvt
	.globl	_fpac_

////////
/
/ idcvt - double to integer.
/ udcvt - double to unsigned integer.
/
/ compiler calling sequence:
/	push	<double argument>
/	call	[iu]dcvt
/	add	sp,8
/
/ outputs:
/	ax=int or unsigned int result.
/
/ uses:
/	ax, bx, cx, dx
/
////////

n	=	8			/ double argument

idcvt:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage

	mov	si,$15			/ si=largest exponent.
	jmp	l0			/ go to common code.

udcvt:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage

	testb	n+7(bp),$0x80		/ double < 0 ??
	jnz	l3			/ (error) yes, return 0.
	mov	si,$16			/ si=largest exponant.

l0:	mov	dx,n+6(bp)		/ scoop up the front
	mov	bx,n+4(bp)		/ of the double.

	shl	bx,$1			/ slide
	rcl	dx,$1			/ sign.

	movb	cl,dh			/ cl=biased exponant.
	and	cx,$0xFF		/ extract, test for all zeros.
	jz	l3			/ if exp=0, it's 0.
	sub	cx,$0x81		/ unbias.
	jl	l3			/ if small, it's 0.
	cmp	cx,si			/ legal range ??
	jge	l3			/ (error) nope, return 0.

	mov	ax,$0x01		/ hidden bit.
	jcxz	l2			/ skip if done.

l1:	shlb	bh,$1			/ shift
	rclb	dl,$1			/ up
	rcl	ax,$1			/ int.
	loop	l1			/ do all bits.

l2:	testb	n+7(bp),$0x80		/ check sign of double.
	jz	l4			/ jump if pos.
	neg	ax			/ fix.
	jmp	l4			/ done.

l3:	sub	ax,ax			/ return 0.

l4:	pop	bp			/ standard
	pop	di			/ c
	pop	si			/ function
	ret				/ return.
