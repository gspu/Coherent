////////
/
/ Intel 8086 C runtime.
/ SMALL model.
/
////////

	.globl	lrdiv
	.globl	lldiv
	.globl	vrdiv
	.globl	lrsgn
	.globl	llsgn

////////
/
/ long
/ lrdiv(a, b);
/ long a;
/ long b;
/
/ long
/ lldiv(a, p);
/ long a;
/ long *p;
/
/ these routines perform long division. they are called from
/ the generated code whenever a long division is found in the
/ user's program.
/
/ both of these routines are just interfaces to the unsigned long
/ divide routine via the long sign adjusters.
/
////////

lrdiv:	mov	ax,$vrdiv		/ get pointer to divide code
	jmp	lrsgn			/ and do the operation.

lldiv:	mov	ax,$vrdiv		/ get pointer to divide code
	jmp	llsgn			/ and do the operation.
