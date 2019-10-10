////////
/
/ Intel 8086 floating point.
/ double to long and unsigned long.
/ small model.
/
////////

	.globl	ldcvt
	.globl	vdcvt
	.globl	_fpac_

////////
/
/ ldcvt - double to long integer.
/ vdcvt - double to unsigned long integer.
/
/ compiler calling sequence:
/	push	<double argument>
/	call	[lv]dcvt
/	add	sp,8
/
/ outputs:
/	dx:ax=long or unsigned long result.
/
/ uses:
/	ax, bx, cx, dx
/
////////

n	=	8			/ double argument.

ldcvt:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage

	mov	si,$31			/ si=largest exponent.
	jmp	l0			/ go to common code.

vdcvt:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage

	testb	n+7(bp),$0x80		/ double < 0 ??
	jnz	l3			/ (error) yes, return 0.
	mov	si,$32			/ si=largest exponant.

l0:	mov	bx,n+6(bp)		/ scoop
	mov	dx,n+4(bp)		/ up the front of
	mov	ax,n+2(bp)		/ the double.

	shl	ax,$1			/ slide
	rcl	dx,$1			/ off the
	rcl	bx,$1			/ sign.

	movb	cl,bh			/ cl=biased exponant.
	and	cx,$0xFF		/ extract, test for all zeros.
	jz	l3			/ if exp=0, it's 0.
	sub	cx,$0x81		/ unbias.
	jl	l3			/ if small, it's 0.
	cmp	cx,si			/ legal range ??
	jge	l3			/ (error) nope, return 0.

	movb	bh,dh			/ get
	xchgb	bh,bl			/ 32 bits
	movb	dh,ah			/ into
	xchgb	dh,dl			/ the
	mov	si,dx			/ bx:si pair.

	sub	dx,dx			/ set up dx:ax with the
	mov	ax,$0x01		/ hidden bit.
	jcxz	l2			/ skip if done.

l1:	shl	si,$1			/ shift
	rcl	bx,$1			/ up
	rcl	ax,$1			/ the
	rcl	dx,$1			/ long.
	loop	l1			/ do all bits.

l2:	testb	n+7(bp),$0x80		/ check sign.
	jz	l4			/ jump if pos.
	neg	dx			/ convert
	neg	ax			/ to
	sbb	dx,$0			/ correct sign.
	jmp	l4			/ done.

l3:	sub	dx,dx			/ make up a 0 to
	mov	ax,dx			/ pass back.

l4:	pop	bp			/ standard
	pop	di			/ c
	pop	si			/ function
	ret				/ return.
