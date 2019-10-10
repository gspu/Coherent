/ (-lgl
/ 	COHERENT Version 4.0
/ 	Copyright (c) 1982, 1992 by Mark Williams Company.
/	Copyright (c) Ciaran O'Donnell, Bievres, France, 1991.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
//////////
/ csu/i386/mcrts0.s
/ C run-time start-off with profiling.
/ i386 COHERENT native version.
//////////

	.globl	_start
	.globl	environ
	.globl	main
	.globl	exit
	.globl	_exit
	.globl	_profon
	.globl	_profoff

	.bss	environ, 4

	.text
_start:	
	popl	%eax				/ argc to EAX
	mov	%esp, %ebp			/ argv to EBP
	lea	4(%esp,%eax,4), %edx		/ envp to EDX
	movl	%edx, environ			/ initialize environ
	pushl	%edx
	pushl	%ebp
	pushl	%eax
	call	_profon				/ enable profiling
	call	main				/ main(argc, argv, envp)
	addl	$12,%esp
	pushl	%eax
	call	exit				/ exit(status)
_exit:
	call	_profoff			/ disable profiling
	movl	$1,%eax
	lcall	$0x7,$0
	hlt					/ NOTREACHED

/ end of csu/i386/mcrts0.s
