////////
/
/ Intel 8086 C runtime.
/ Build a double from its mantissa and exponent parts.
/ SMALL model.
/
////////

	.globl	_fpac_
	.globl	ldexp_
	.globl	dzero

////////
/
/ double ldexp(dd, exp) double dd; int exp;
/ returns dd * 2**exp
/
////////

ldexp_:
	push	si
	push	di
	push	bp
	mov	bp, sp

	mov	ax, 14(bp)	/ most significant word
	rol	ax, $1		/ get exp isolated in register
	orb	ah, ah		/ is exp 0?
	jnz	L0		/ no
	call	dzero		/ dd is 0, zero _fpac_
	jmp	L1		/ and return it
L0:
	addb	ah, 16(bp)	/ add exp
	ror	ax, $1		/ back in place

	mov	_fpac_+6, ax
	mov	ax, 12(bp)
	mov	_fpac_+4, ax
	mov	ax, 10(bp)
	mov	_fpac_+2, ax
	mov	ax, 8(bp)
	mov	_fpac_, ax
L1:
	pop	bp
	pop	di
	pop	si
	ret
