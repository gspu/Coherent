////////
/
/ Intel 8086 C runtime.
/ Split a double into the fraction and whole integer parts.
/ SMALL model.
/
////////

	.globl	modf_
	.globl	_fpac_
	.globl	dpush
	.globl	dladd
	.globl	dlsub
	.globl	dzero

////////
/
/ double
/ modf(d, *ip);
/ double d;
/ double *ip;
/ The "integer part" of the double "d" is stored
/ indirectly through "ip". The remaining fractional part
/ is returned. The return value is always positive.
/
////////

d	=	8			/ Double argument
ip	=	16			/ Whole part pointer.

EXP	=	-2			/ Exponant.
SIGN	=	-4			/ Sign flag.
CLAIM	=	4			/ # of bytes of autos.

modf_:	push	si			/ Standard
	push	di			/ C
	push	bp			/ calling
	mov	bp,sp			/ sequence.

	sub	sp,$CLAIM		/ Claim auto space
	cld				/ Incrementing.

	lea	si,d(bp)		/ Copy the "d" argument
	mov	di,ip(bp)		/ into the
	movsw				/ integer return
	movsw				/ value
	movsw				/ output
	movsw				/ area

	mov	ax,d+6(bp)		/ Get the first word.
	shl	ax,$1			/ Slide off the sign and
	rclb	SIGN(bp),$1		/ save it.
	movb	al,ah			/ Save the exponant
	subb	ah,ah			/ in the ax,
	sub	ax,$0x80		/ unbias it, and
	mov	EXP(bp),ax		/ save it away.

/ If the exponant is <= 0, then there is no
/ integer part. Set the integer part to 0.0, and get set
/ to return the argument as the fractional part.

	jg	0f			/ Jump if exponant > 0

	sub	ax,ax			/ Set 
	mov	di,ip(bp)		/ the
	stosw				/ integer
	stosw				/ part
	stosw				/ to
	stosw				/ zero.

	lea	si,d(bp)		/ Copy
	mov	di,$_fpac_		/ the
	movsw				/ argument
	movsw				/ to
	movsw				/ the
	movsw				/ return area.
	jmp	1f			/ Go check the sign.

/ If the exponant is > 56 then there are no fractional bits
/ at all. The integer part is correct. Zero out the floating point
/ return value.

0:	cmp	ax,$56			/ Any fractional bits ?
	jl	0f			/ Yes.

	call	dzero			/ Zero the _fpac_.
	jmp	1f			/ Go check the sign.

/ Clear 56-exp bits, starting at the right hand end of the
/ integer value. Clear as many full bytes as you can, then build a
/ mask and get the rest.

0:	neg	ax			/ Figure out
	add	ax,$56			/ 56-exp.
	mov	di,ip(bp)		/ Point di at low end.

0:	sub	ax,$8			/ Is there a full byte ?
	jl	0f			/ Nope.
	movb	(di),$0			/ Clear 8 bits.
	inc	di			/ Move to next byte and
	jmp	0b			/ try again.

0:	add	ax,$8			/ Figure out
	movb	cl,al			/ the
	movb	al,$0xFF		/ correct mask to
	shlb	al,cl			/ clear the rest, and
	andb	(di),al			/ do it.

/ Scoop up the number. Shift the binary point to just above
/ the hidden bit location, then shift it additional times to get
/ it normalized.

	mov	cx,EXP(bp)		/ Get exponant.

	movb	dl,d+6(bp)		/ Scoop
	mov	ax,d+4(bp)		/ up
	mov	si,d+2(bp)		/ the
	mov	di,d+0(bp)		/ number.

0:	shl	di,$1			/ Slide
	rcl	si,$1			/ up
	rcl	ax,$1			/ one
	rclb	dl,$1			/ bit, and
	loop	0b			/ loop until done.

	mov	cx,di			/ Check
	or	cx,si			/ if
	or	cx,ax			/ we
	orb	cl,dl			/ have a zero.
	jnz	0f			/ Nope.
	subb	dh,dh			/ Make all zeros and
	jmp	2f			/ return.

0:	mov	cx,$0x8000		/ ch=exp, cl=0

0:	orb	dl,dl			/ Normalized ?
	js	0f			/ Yes.
	shl	di,$1			/ Shift
	rcl	si,$1			/ up
	rcl	ax,$1			/ one
	rclb	dl,$1			/ place.
	decb	ch			/ Adjust exponant and
	jmp	0b			/ loop.

0:	and	dx,$0x7F		/ dh=0, dl=no hidden bit
	shrb	SIGN(bp),$1		/ carry=sign
	rcr	cx,$1			/ Pull into correct place and
	or	dx,cx			/ pack up double.

2:	mov	_fpac_+6,dx		/ Save
	mov	_fpac_+4,ax		/ into the
	mov	_fpac_+2,si		/ return
	mov	_fpac_+0,di		/ location.

/ Final sign checks. The fractional part is always a
/ positive number. If it is less than zero, add one to it and
/ subtract one from the integer part.

1:	test	_fpac_+6,$0x7F80	/ Is the return value zero ?
	jz	0f			/ Yup.
	testb	_fpac_+7,$0x80		/ Is the return value negative ?
	jz	0f			/ Nope.

	mov	si,$one			/ Get address of "1.0"
	push	si			/ Push for "dladd"
	call	dpush			/ Push fraction for "dlsub"
	push	si			/ Push for "dlsub"
	mov	di,ip(bp)		/ di=pointer to the integer part.
	push	6(di)			/ Push
	push	4(di)			/ onto the
	push	2(di)			/ stack for
	push	0(di)			/ "dlsub"
	call	dlsub			/ Subtract 1 from integer part.
	add	sp,$10			/ Pop args.
	mov	si,$_fpac_		/ Copy
	movsw				/ result
	movsw				/ to
	movsw				/ the integer
	movsw				/ part
	call	dladd			/ Add one to the fraction
	add	sp,$10			/ and clear args.

0:	mov	sp,bp			/ Standard
	pop	bp			/ C
	pop	di			/ function
	pop	si			/ return
	ret				/ code.

	.shrd
one:	.byte	0x00, 0x00, 0x00, 0x00
	.byte	0x00, 0x00, 0x80, 0x40
