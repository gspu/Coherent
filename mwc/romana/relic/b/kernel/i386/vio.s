/	vio.s

/ This code was dropped in favor of a simpler implementation.
/ I am keeping it around in case I need a similar call gate interface
/ some day. - hws

/ Ring 0 services for video
/ this routine is a stub for the ring 0 code
/
/	int vio(int a1, int a2, int a3)
	.globl	vio
vio:
	pushf			/ irpts off during trip to ring 0
	cli

	movl	16(%esp),%eax	/ fetch a3
	pushl	%eax
	movl	16(%esp),%eax	/ fetch a2
	pushl	%eax
	movl	16(%esp),%eax	/ fetch a1
	pushl	%eax

	lcall	$SEG_VIO,$0	/ gate to viofR0
	/ viofR0 will delete 12 bytes of stack args
	popf			/ re-enable interrupts, if they were on before
	ret

/ Ring 0 video services.  Called via a gate.
/ Want interrupts off when we arrive since the interrupt gates
/ lead into Ring 1.
/
/ I/O map manipulations affect bitmap in tss.
/ I/O map offsets are in bytes from start of map.
/
/ a1:  VIO_OR_IO	bitwise or a2 into 32-bit word at offset a3
/   return the new word value stored back into the io map.
/ a1:  VIO_AND_IO	bitwise and a2 into 32-bit word at offset a3
/   return the new word value stored back into the io map.
viofR0:
	enter	$0,$8	/ 8 bytes of auto storage

	pushl	12(%ebp)	/ print a1
	call	print32
	popl	%eax

	pushl	16(%ebp) / print a2
	call	print32
	popl	%eax

	pushl	20(%ebp) / print a3
	call	print32
	popl	%eax

	xorl	%eax,%eax
	movl	20(%ebp),%ecx	/ fetch a3
	cmpl	$0,%ecx		/ don't allow access to tss below iomap
	jl	vioNfg

	cmpl	$TSS_MAPLEN,%ecx / or above iomap
	jge	vioNfg

	movl	tss+TSS_IOMAP(%ecx),%eax
	pushl	%eax
	call	print32
	popl	%eax

vioNfg:
	cmpb	$0,12(%ebp)	/ look at a1
	jnz	vioAnd
	orl	16(%ebp),%eax	/ or a2 with %eax
	jmp	vioDone

vioAnd:
	andl	16(%ebp),%eax	/ and a2 with %eax

vioDone:
	leave
	/ make 12-byte arg list disappear
	lret	$12

