////////
/
/ Intel 8086 runtime.
/ SMALL model.
/
////////

	.globl	lrmul
	.globl	llmul
	.globl	vrmul
	.globl	lrsgn
	.globl	llsgn

////////
/
/ long
/ lrmul(a, b);
/ long a;
/ long b;
/
/ long
/ llmul(a, p);
/ long a;
/ long *p;
/
/ these routines perform long multiplication. they are called from
/ the generated code whenever a long multiplication is found in the
/ user's program.
/
/ both of these routines are just interfaces to the unsigned long
/ multiply routine via the long sign adjusters.
/
////////

lrmul:	mov	ax,$vrmul		/ get pointer to multiply code
	jmp	lrsgn			/ and do the operation.

llmul:	mov	ax,$vrmul		/ get pointer to multiply code
	jmp	llsgn			/ and do the operation.
