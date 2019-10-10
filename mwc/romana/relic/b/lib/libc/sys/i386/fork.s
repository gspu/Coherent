//////////
/ libc/sys/i386/fork.s
/ Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991.
//////////

	.globl	fork
	.globl	.cerror

fork:
	movl	$2,%eax
	lcall	$0x7,$0
	jc	.cerror
	orl	%edx,%edx
	jz	?L0				/ return pid	(parent)
	xorl	%eax,%eax			/ return 0	(child)
?L0:
	orl	%eax,%eax
	ret

/ end of libc/sys/i386/fork.s
