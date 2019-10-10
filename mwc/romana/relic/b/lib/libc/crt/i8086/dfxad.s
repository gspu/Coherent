////////
/
/ Intel 8086 floating point.
/ assigned add to a single float.
/ small model.
/
////////

	.globl	dfladd
	.globl	dfradd
	.globl	_fpac_
	.globl	dladd

////////
/
/ ** dfladd -- assigned single add (lvalue)
/ ** dfradd -- assigned single add (rvalue)
/
/ these two routines are called directly by the compiler to do single
/ floating assigned addition. these small routines make the  compiler
/ simpler, and get rid of a very bulky code sequence that would  have
/ to be generated for a fairly uncommon operation.
/
/ compiler calling sequences:
/	push	<right single rvalue>
/	lea	ax,<left single lvalue>
/	push	ax
/	call	ddradd
/	add	sp,10
/
/	lea	ax,<right single lvalue>
/	push	ax
/	lea	ax,<left single lvalue>
/	push	ax
/	call	ddladd
/	add	sp,4
/
/ outputs:
/	_fpac_=result.
/
/ uses:
/	ax, bx, cx, dx
/
////////

pa	=	8			/ pointer to left
pb	=	10			/ pointer to right
b	=	10			/ right

dfladd:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ entry

	mov	ax,pb(bp)		/ ax=pointer to right operand
	jmp	l0			/ finish in common code

dfradd:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ entry

	lea	ax,b(bp)		/ ax=pointer to right operand

l0:	push	ax			/ second arg is right lvalue.
	mov	si,pa(bp)		/ si=pointer to left.
	push	2(si)			/ push
	push	0(si)			/ the
	sub	ax,ax			/ double
	push	ax			/ left
	push	ax			/ rvalue
	call	dladd			/ do the addition.
	add	sp,$10			/ pop args

	mov	ax,_fpac_+6		/ copy
	mov	2(si),ax		/ the
	mov	ax,_fpac_+4		/ result
	mov	0(si),ax		/ back.

	pop	bp			/ standard
	pop	di			/ c
	pop	si			/ function
	ret				/ return.
