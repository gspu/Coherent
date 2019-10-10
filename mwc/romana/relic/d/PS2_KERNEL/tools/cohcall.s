	.globl	cohcall
	.globl	errno
	.set	COHCALL,500

cohcall:
	movl	$COHCALL,%eax
	lcall	$7,$0
	jc	x
	ret
x:	movl	%eax,errno
	movl	$-1,%eax
	ret
