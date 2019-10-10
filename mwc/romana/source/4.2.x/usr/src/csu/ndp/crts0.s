/ (-lgl
/ 	COHERENT Version 4.2
/ 	Copyright (c) 1982, 1993 by Mark Williams Company.
/	Copyright (c) Ciaran O'Donnell, Bievres, France, 1991.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
//////////
/ csu/i387/crts0.s
/ C run-time start-off.
/ i386 COHERENT native version with i80x87 NDP initialization.
//////////

	.unixorder

	.globl	_start
	.globl	environ
	.globl	main
	.globl	exit
	.globl	_exit

	.bss	environ, 4

	.text
_start:	
	fninit					/ initialize NDP
	fwait					/ and wait for it
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

/ end of csu/i387/crts0.s
