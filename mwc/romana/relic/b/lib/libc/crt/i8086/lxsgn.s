////////
/
/ Intel 8086 C runtime.
/ SMALL model.
/
////////

	.globl	lrmod
	.globl	lrsgn
	.globl	llmod
	.globl	llsgn

////////
/
/ These four routines transform an unsigned operator into
/ a signed operator. They are called with the arguments on the stack
/ as pushed by the compiler, and a pointer to the "r" format unsigned
/ operator routine in "ax". The C style call frame for the signed
/ operator is created here.
/
////////

alow	= 	8
ahigh	=	10
bptr	=	12
blow	=	12
bhigh	=	14
fptr	=	-4

lrmod:	mov	dx,$1			/ set remainder flag.
	jmp	l0

lrsgn:	sub	dx,dx			/ clear remainder flag.
	jmp	l0

llmod:	mov	dx,$0x8001		/ set lvalue, remainder flags.
	jmp	l0

llsgn:	mov	dx,$0x8000		/ set lvalue flag.

l0:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage.

	push	bx			/ save bx.
	mov	bx,ax			/ function adr to bx.
	mov	di,dx			/ flags to di.
	push	bx			/ save function in fptr.
	sub	si,si			/ clear sign flag.
	or	di,di			/ check lvalue flag.
	jns	l1			/ rvalue passed.
	mov	bx,bptr(bp)		/ grab address of operand abd
	mov	dx,2(bx)		/ grab the b
	mov	ax,(bx)			/ operand.
	jmp	l2

l1:	mov	dx,bhigh(bp)
	mov	ax,blow(bp)

l2:	or	dx,dx			/ check sign.
	jns	l3			/ jump if operand positive.
	neg	dx			/ negate
	neg	ax			/ the
	sbb	dx,$0			/ operand.
	shr	di,$1			/ check remainder flag.
	jc	l3			/ jump if doing remainder.
	inc	si			/ flip result sign.

l3:	push	dx			/ push onto
	push	ax			/ stack as function arg.

	mov	dx,ahigh(bp)		/ grab the
	mov	ax,alow(bp)		/ other operand.
	or	dx,dx			/ check the operand sign.
	jns	l4			/ jump if positive.
	neg	dx			/ negate
	neg	ax			/ the
	sbb	dx,$0			/ operand.
	inc	si			/ and flip the sign.

l4:	push	dx			/ push other operand as
	push	ax			/ an argument.

	icall	fptr(bp)		/ call unsigned op.
	add	sp,$10			/ pop off 5 words.

	shr	si,$1			/ do we need to negate ??
	jnc	l5			/ nope.
	neg	dx			/ negate 
	neg	ax			/ the
	sbb	dx,$0			/ result.

l5:	pop	bx			/ restore saved bx.
	mov	sp,bp			/ standard
	pop	bp			/ c
	pop	di			/ function
	pop	si			/ return
	ret				/ sequence.
