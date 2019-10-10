////////
/
/ Intel 8086 C runtime.
/ convert ascii string to double binary.
/ small model.
/
////////

	.globl	atof_
	.globl	_fpac_

////////
/
/ ** atof_ -- convert ascii string to double binary.
/
/ this routine converts a floating point number from ascii into double
/ precision floating point binary. it is c callable. conversion  stops
/ on the first illegal character. there is no error status passed back
/ for bad numbers or over/underflows. this version of atof does decvax
/ floating point; it can be changed to do ieee trivially (look at  the
/ conversion routine in n0/lex.c to see how to do this).
/
/ calling sequence:
/	double	atof();
/	double	d;
/	char	*p;
/	d = atof(p);
/
/ outputs:
/	_fpac_=result.
/
/ uses:
/	ax, bx, cx, dx
/
////////

string	=	8			/ the string argument
binexp	=	-2			/ binary exponant
decexp	=	-4			/ decimal exponant (byte)
flags	=	-3			/ flags (byte)
fldexp	=	-4			/ for clearing flags/decexp
num	=	-12			/ a scratch double.

claim	=	12			/ number of bytes of autos.
negnum	=	0x01			/ negative number (must be 01)
negexp	=	0x02			/ negative exponant
gotdot	=	0x04			/ got dot flag

ten:	.byte	10			/ a 10 in the code segment.

atof_:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage

	sub	sp,$claim		/ get space for autos
	cld				/ make lodsb increment si.

	sub	cx,cx			/ zero out the
	mov	bx,cx			/ big register used to
	mov	di,cx			/ hold doubles
	mov	dx,cx			/ during conversion (dx,di,bx,cx)
	mov	fldexp(bp),cx		/ clear flags, decexp
	mov	binexp(bp),$65+128	/ initialise binary exponant

	mov	si,string(bp)		/ get string argument.

l0:	lodsb				/ al=character.
	cmpb	al,$040			/ is it a space?
	je	l0			/ loop on white space
	cmpb	al,$011			/ is it a tab?
	je	l0			/ loop on white space

	cmpb	al,$'+			/ plus sign ??
	je	l1			/ yes, eat it up.
	cmpb	al,$'-			/ minus sign ??
	jne	l2			/ no, go get number.
	incb	flags(bp)		/ set "negnum" flag.

l1:	lodsb				/ al=character.

l2:	cmpb	al,$'.			/ decimal point character ??
	jne	l3			/ jump if not.
	testb	flags(bp),$gotdot	/ have we seen a dot ??
	jnz	l12			/ yes, quit.
	orb	flags(bp),$gotdot	/ we have seen one now.
	jmp	l1			/ go on.

l3:	subb	al,$'0			/ remove ascii bias.
	cmpb	al,$9			/ legal digit ??
	ja	l6			/ no (ja makes < '0' look huge)
	testb	dh,$0xF8		/ do we have 5 bits ??
	jz	l4			/ yes.
	incb	decexp(bp)		/ nope, toss significance.
	jmp	l5			/ done with this digit.

l4:	call	shlreg			/ multiply
	call	storeg			/ the big
	call	shlreg			/ register
	call	shlreg			/ by
	call	addreg			/ 10 (8+2 = 10).
	cbw				/ ax = new digit.
	add	cx,ax			/ add into the low word.
	call	ripple			/ ripple carries.

l5:	testb	flags(bp),$gotdot	/ are we after the "." ??
	jz	l1			/ jump if not.
	decb	decexp(bp)		/ yes, adjust scale.
	jmp	l1			/ go get another digit.

l6:	cmpb	al,$'e-'0		/ exponant ??
	jz	l7			/ yes.
	cmpb	al,$'E-'0		/ perhaps ??
	jnz	l12			/ nope, all done.

l7:	push	dx			/ get a work register.
	subb	dh,dh			/ dh=exponant.
	lodsb				/ al=character.
	cmpb	al,$'+			/ positive ??
	je	l8			/ yes, eat it up.
	cmpb	al,$'-			/ negative ??
	jne	l9			/ nope, its first character.
	orb	flags(bp),$negexp	/ set flag.

l8:	lodsb				/ al=character.

l9:	subb	al,$'0			/ remove any ascii bias.
	cmpb	al,$9			/ legal ??
	ja	l10			/ nope, all done.
	xchgb	dh,al			/ al=old exponant, dh=digit.
	mulb	cs:ten			/ old=10*old.
	addb	dh,al			/ exp=10*old+digit
	jmp	l8			/ loop until the end of exp.

l10:	testb	flags(bp),$negexp	/ was the exponant lt 0 ??
	jz	l11			/ nope.
	negb	dh			/ fix it up.

l11:	addb	decexp(bp),dh		/ fix up decimal exponant.
	pop	dx			/ recover work register.

l12:	mov	ax,di			/ quick
	or	ax,bx			/ check
	or	ax,cx			/ for
	or	ax,dx			/ zero.
	jnz	l13			/ jump if non zero.
	jmp	l25			/ zero is easy.

l13:	movb	al,decexp(bp)		/ al=decimal exponant.
	orb	al,al			/ check its sign.
	jle	l18			/ jump if not multiply scaling.

l14:	call	storeg			/ save big reg in "num".
	call	shlreg			/ can it take a * 5 ??
	jc	l15			/ nope
	call	shlreg			/ well ??
	jc	l15			/ nope
	call	addreg			/ last try ??
	jc	l15			/ nope
	inc	binexp(bp)		/ yes, 5*2 = 10
	jmp	l17			/ go test if all done.

l15:	call	lodreg			/ fetch number.
	call	shrreg			/ figure out
	call	shrreg			/ 5/4 of the old
	call	addreg			/ number.
	jnc	l16			/ harumph, overflow.
	call	rcrreg			/ easily
	inc	binexp(bp)		/ repaired, though.

l16:	add	binexp(bp),$3		/ 5/4 * 8 = 10

l17:	decb	decexp(bp)		/ are we done ??
	jnz	l14			/ nope
	jmp	l23			/ yup

l18:	jz	l23			/ jump if no scaling

l19:	or	dx,dx			/ first
	js	l20			/ we
	call	shlreg			/ left
	dec	binexp(bp)		/ justify the
	jmp	l19			/ fraction.

l20:	call	shrreg			/ position number
	call	storeg			/ get it in
	movb	al,$32			/ al=step counter.

l21:	testb	al,$0x01		/ odd iteration ??
	jz	l22			/ nope.
	call	shrreg			/ yes, 4/5 = 0.110011001100
	call	shrreg			/ so do two extra shifts.

l22:	call	shrreg			/ shift over and
	add	num+0(bp),cx		/ add it
	adc	num+2(bp),bx		/ into
	adc	num+4(bp),di		/ the
	adc	num+6(bp),dx		/ number.
	decb	al			/ any more loops ??
	jnz	l21			/ yes.

	call	lodreg			/ result in
	sub	binexp(bp),$3		/ 1/10 = 4/5 * 1/8
	incb	decexp(bp)		/ done ??
	jnz	l19			/ jump if not.

l23:	dec	binexp(bp)		/ shift left until
	call	shlreg			/ the hidden bit
	jnc	l23			/ appears.
	addb	ch,$1			/ round just below
	call	ripple			/ the last bit we keep.
	jnc	l24			/ no carry on rounding.
	call	shrreg			/ carry flips hidden bit and
	inc	binexp(bp)		/ fix exponant.

l24:	movb	al,$8			/ al=step counter.
l24a:	call	shrreg			/ slide the number down
	decb	al			/ to make room for
	jnz	l24a			/ the exponant.
	movb	dh,binexp(bp)		/ get binary exponant.
	rcrb	flags(bp),$1		/ sign in c.
	call	rcrreg			/ finish up the number.

l25:	mov	_fpac_+6,dx		/ store
	mov	_fpac_+4,di		/ the
	mov	_fpac_+2,bx		/ big
	mov	_fpac_+0,cx		/ number

	mov	sp,bp			/ do
	pop	bp			/ standard
	pop	di			/ c
	pop	si			/ function
	ret				/ return.

////////
/
/ ** shrreg -- shift big register right.
/ ** rcrreg -- shift big register right, using carry.
/
////////

shrreg:	shr	dx,$1			/ do first word and
	jmp	l30			/ finish up in common code.

rcrreg:	rcr	dx,$1			/ do first word.

l30:	rcr	di,$1			/ shift down the
	rcr	bx,$1			/ other three
	rcr	cx,$1			/ words and
	ret				/ return

////////
/
/ ** lodreg -- load big register from "num"
/
////////

lodreg:	mov	dx,num+6(bp)		/ load
	mov	di,num+4(bp)		/ the
	mov	bx,num+2(bp)		/ big
	mov	cx,num+0(bp)		/ register and
	ret				/ return

////////
/
/ ** storeg -- store big register into "num"
/
////////

storeg:	mov	num+6(bp),dx		/ store
	mov	num+4(bp),di		/ the
	mov	num+2(bp),bx		/ big
	mov	num+0(bp),cx		/ register and
	ret				/ return

////////
/
/ ** addreg -- add "num" to big register.
/
////////

addreg:	add	cx,num+0(bp)		/ add the
	adc	bx,num+2(bp)		/ big register to
	adc	di,num+4(bp)		/ the
	adc	dx,num+6(bp)		/ number and
	ret				/ return

////////
/
/ ** ripple -- ripple c bit into register
/
////////

ripple:	adc	bx,$0			/ ripple 
	adc	di,$0			/ carry
	adc	dx,$0			/ bits and
	ret				/ return

////////
/
/ ** shlreg -- shift big register left
/
////////

shlreg:	shl	cx,$1			/ shift
	rcl	bx,$1			/ big
	rcl	di,$1			/ register
	rcl	dx,$1			/ left and
	ret				/ return
