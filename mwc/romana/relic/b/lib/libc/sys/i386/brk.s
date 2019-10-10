/	Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
	.globl	sbrk
	.globl	brk
	.globl	__end
	.globl	.cerror

sbrk:
	movl	landsend,%ecx
	addl	%ecx,4(%esp)
	movl	$17,%eax
	lcall	$7,$0
	jc	.cerror
	movl	4(%esp),%eax
	movl	%eax,landsend
	movl	%ecx,%eax
	orl	%eax,%eax
	ret
brk:
	movl	$17,%eax
	lcall	$0x7,$0
	jc	.cerror
	movl	4(%esp),%eax
	movl	%eax,landsend
	xorl	%eax,%eax
	ret
	.data
landsend:
	.long	__end
