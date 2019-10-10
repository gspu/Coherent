////////
/
/ Intel 8086 C runtime.
/ Store binary exponent, return mantissa.
/ SMALL model.
/
////////

	.globl	_fpac_
	.globl	frexp_
	.globl	dzero

EXPBIAS	= 0x80

////////
/
/ double frexp(dd, xp) double dd; int *xp;
/ Store binary exponent via xp, return mantissa.
/
////////

frexp_:
	push	si
	push	di
	push	bp
	mov	bp, sp

	mov	bx, 16(bp)	/ xp to BX
	mov	ax, 14(bp)	/ Get most significant bits
	rol	ax, $1		/ Get exponent in 1 reg
	orb	ah, ah		/ Is exponent 0?
	jnz	L0		/ No
	call	dzero		/ Yes, zero _fpac_
	mov	(bx), ax	/ and store 0 through xp
	jmp	L1

L0:	movb	cl, ah
	subb	ch, ch
	sub	cx, $EXPBIAS	/ signed
	mov	(bx), cx	/ *xp = exp

	movb	ah, $EXPBIAS	/ zero exponent
	ror	ax, $1		/ back in place
	mov	_fpac_+6, ax
	mov	ax, 12(bp)
	mov	_fpac_+4, ax
	mov	ax, 10(bp)
	mov	_fpac_+2, ax
	mov	ax, 8(bp)
L1:	mov	_fpac_, ax

	pop	bp
	pop	di
	pop	si
	ret
