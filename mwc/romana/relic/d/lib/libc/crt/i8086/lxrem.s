////////
/
/ Intel 8086 C runtime.
/ SMALL model.
/
////////

	.globl	lrrem
	.globl	llrem
	.globl	vrrem
	.globl	lrmod
	.globl	llmod

////////
/
/ long
/ lrrem(a, b);
/ long a;
/ long b;
/
/ long
/ llrem(a, p);
/ long a;
/ long *p;
/
/ these routines perform long remainder. they are called from
/ the generated code whenever a long remainder is found in the
/ user's program.
/
/ both of these routines are just interfaces to the unsigned long
/ remainder routine via the long sign adjusters.
/
////////

lrrem:	mov	ax,$vrrem		/ get pointer to remainder code
	jmp	lrmod			/ and do the operation.

llrem:	mov	ax,$vrrem		/ get pointer to remainder code
	jmp	llmod			/ and do the operation.
