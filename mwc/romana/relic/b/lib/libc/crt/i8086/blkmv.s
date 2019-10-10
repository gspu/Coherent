////////
/
/ Intel 8086 C runtime.
/ SMALL model.
/
////////

	.globl	blkmv

////////
/
/ char *
/ blkmv(dptr, sptr, nbytes);
/ char *dptr;
/ char *sptr;
/
/ Copy a block of bytes of length "nbytes" from
/ the block at "sptr" to the block at "dptr".
/ This routine is called by generated code for structure assignment and
/ for functions that return structures.
/
/ The function returns the start address of the destination buffer.
/ This is used as the return value of a function returning a structure.
/
////////

blkmv:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ linkage.

	mov	cx,12(bp)		/ grab the byte count.
	jcxz	L0			/ duck if no bytes.
	mov	si,10(bp)		/ get source and
	mov	di, 8(bp)		/ destination pointers.
	cld				/ move forward
	rep				/ copy
	movsb				/ the data block.

L0:	mov	ax, 8(bp)		/ return dptr.

	mov	sp,bp			/ standard
	pop	bp			/ c
	pop	di			/ function
	pop	si			/ linkage.
	ret				/ done.
