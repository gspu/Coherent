/ (-lgl
/ 	COHERENT Version 3.2.2
/ 	Copyright (c) 1982, 1992 by Mark Williams Company.
/	Copyright (c) Ciaran O'Donnell, Bievres, France, 1991.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
/ csu/i386/crts0.s
/ C run-time start-off.
/ i386 COHERENT native version.

	.globl	_start
	.globl	environ
	.globl	main
	.globl	exit
	.globl	_exit

	.bss	environ,4

	.text
_start:	
	popl	%eax				/ argc to EAX
	mov	%esp, %ebp			/ argv to EBP
	lea	4(%esp,%eax,4), %edx		/ envp to EDX
	movl	%edx, environ			/ initialize environ
	pushl	%edx
	pushl	%ebp
	pushl	%eax
	call	main				/ main(argc, argv, envp)
	addl	$12,%esp
	pushl	%eax
	call	exit				/ exit(status)
_exit:
	movl	$1,%eax
	lcall	$0x7,$0
	hlt					/ NOTREACHED

/ end of csu/i386/crts0.s
