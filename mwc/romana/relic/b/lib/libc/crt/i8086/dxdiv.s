////////
/
/ Intel 8086 floating point.
/ Double divide.
/ SMALL model.
/
////////

	.globl	drdiv
	.globl	dldiv
	.globl	_fpac_
	.globl	dzero

oerror_:	ret

////////
/
/ drdiv - double divide (rvalue)
/ dldiv - double divide (lvalue)
/
/ compiler calling sequences:
/	push	<right double argument>
/	push	<left  double argument>
/	call	drdiv
/	add	sp,16
/
/	mov	ax,offset <right double argument>
/	push	ax
/	push	<left  double argument>
/	call	dldiv
/	add	sp,10
/
/ outputs:
/	_fpac_ = result.
/
/ see Knuth "Art of Computer Programming" vol 2  for explanation of
/ multiprecision divide.
/
////////

l	=	8	/ left argument
r	=	16	/ right argument (rvalue)
rp	=	16	/ right argument (lvalue)

sign	=	-2
exp	=	-4
q0	=	-6
q1	=	-8
q2	=	-10
q3	=	-12

claim	=	-q3	/ stack claim size

drdiv:
	push	si
	push	di
	push	bp
	mov	bp, sp

	lea	si, r(bp)	/ si = pointer to right op.
	jmp	0f

dldiv:
	push	si
	push	di
	push	bp
	mov	bp, sp

	mov	si, rp(bp)	/ si = pointer to right op.
0:
	sub	sp, $claim

	mov	ax, l+6(bp)
	shl	ax, $1		/ l exp in ah
	jne	0f
retz:
	call	dzero
	jmp	done
0:
	rclb	dl, $1		/ form sign
	mov	bx, 6(si)
	shl	bx, $1		/ r exp in bh
	adcb	dl, $0
	movb	sign(bp), dl
	orb	bh, bh
	je	retinf

	movb	al, ah
	movb	bl, bh
	subb	ah, ah
	movb	bh, ah
	sub	ax, bx
	add	ax, $129
	js	retz		/ exp will only get smaller

	cmp	ax, $257	/ by at most 2
	ja	retinf

	mov	exp(bp), ax
	mov	bx, l+5(bp)	/ set up l fraction (u) for divide loop
	orb	bh, $128	/ implicit bit of hi order word
	mov	cx, l+3(bp)
	mov	di, l+1(bp)
	movb	ah, l(bp)
	subb	al, al
	shr	bx, $1		/ shift r to guarantee valid divide
	rcr	cx, $1
	rcr	di, $1
	rcr	ax, $1
	add	bp, $q0 + 2	/ quotient pushed from here to top
	jmp	0f
overf:
	mov	(bp), $-1	/ q^ = word size - 1
	sub	dx, cx		/ do multiply and subtract using adds
	mov	bx, ax		/ u1
	sub	ax, ax
	add	bx, cx
	adcb	dl, al		/ must keep track of u0 for a while

	sub	bx, 3(si)	/ u1 u2 -= q^ * v2
	sbbb	dl, al
	add	di, 3(si)
	adc	bx, ax
	adcb	dl, al
	js	2f		/ adjust q^ and fall in

	mov	cx, di		/ u2
	mov	di, 1(si)	/ v3
	sub	cx, di		/ u2 u3 -= q^ * v3
	sbb	bx, ax
	sbbb	dl, al
	add	di, _fpac_	/ pick up u3
	adc	cx, ax
	adc	bx, ax
	adcb	dl, al

	movb	ah, (si)	/ u3 u4 -= q^ * v4
	sub	di, ax
	jmp	3f		/ fall in

0:
	sub	bp, $2
	mov	_fpac_, ax	/ u3

	mov	dx, bx		/ u0
	mov	ax, cx		/ u1
	mov	cx, 5(si)
	orb	ch, $128	/ v1
	cmp	dx, cx
	je	overf
	jb	9f
	call	oerror_
9:

	div	cx
	mov	(bp), ax	/ q+2 >= a = q^ >= q
	mov	bx, dx		/ bx = dx = remainder = new u1 <= v1

	mul	3(si)		/ dx ax  =  q^ * v2
	sub	di, ax		/ subtract from  new u1 new u2
	sbb	bx, dx		/          =     new u1 old u3
	jnc	1f		/ ok - no prelim q^ adjust
2:
	dec	(bp)		/ q^ too big
	add	di, 3(si)	/ adjust u1 u2
	adc	bx, cx
	jnc	2b		/ until positive ( at most twice)
1:				/ now q+1 >= q^ >= q
	mov	cx, di		/ shift new u2 = old u3
	mov	di, _fpac_	/ shift new u3 = old u4
				/ (     new u4 = 0)
	mov	ax, 1(si)
	mul	(bp)		/ dx ax = v3 * q^
	sub	di, ax		/ subtract from u2 u3
	sbb	cx, dx
	mov	ax, $0
	sbb	bx, ax
	sbbb	ah, ah
	movb	_fpac_+2, ah	/ save carry into u0

	movb	ah, (si)	/ last word is short 8 bits
	mul	(bp)		/ dx ax = v4 * q^
	neg	ax		/ ax := u4 - ax = 0 - ax
	sbb	di, dx
	movb	dl, _fpac_+2
3:
	sbb	cx, $0
	sbb	bx, $0
	sbbb	dl, $0
	je	1f		/ q = q^ ok ( this is usually the case)

	dec	(bp)		/ else q = q^ - 1
	addb	ah, (si)	/ add v1 v2 v3 v4  to  u1 u2 u3 u4
	adc	di, 1(si)
	adc	cx, 3(si)
	adc	bx, $0
	mov	dx, 5(si)
	orb	dh, $128
	add	bx, dx
1:
	cmp	bp, sp
	jne	0b

	add	bp, $claim
	mov	ax, exp(bp)
	mov	dx, q3(bp)
	mov	di, q2(bp)
	mov	cx, q1(bp)
	mov	bx, q0(bp)
	orb	bh, bh
	js	0f		/ normalized

	dec	ax		/ only need to shift once
	shl	dx, $1
	rcl	di, $1
	rcl	cx, $1
	rcl	bx, $1
0:
	shlb	dl, $1		/ round
	adcb	dh, $0
	adc	di, $0
	adc	cx, $0
	adc	bx, $0
	jnc	0f

	rcr	bx, $1		/ exactly 1/2
	dec	ax
0:
	orb	ah, ah		/ check exp
	js	retz		/ too small
	jne	retinf		/ too big

	movb	_fpac_, dh	/ ok store it
	mov	_fpac_+1, di
	mov	_fpac_+3, cx
	shlb	bh, $1
	movb	ah, sign(bp)
	shr	ax, $1
	rcrb	bh, $1
	mov	_fpac_+5, bx
	movb	_fpac_+7, al
done:
	mov	sp, bp
	pop	bp
	pop	di
	pop	si
	ret
retinf:
	mov	ax, $-1
	mov	_fpac_, ax
	mov	_fpac_+2, ax
	mov	_fpac_+4, ax
	movb	dl, sign(bp)
	shrb	dl, $1
	rcrb	ah, $1
	mov	_fpac_+6, ax
	jmp	done
