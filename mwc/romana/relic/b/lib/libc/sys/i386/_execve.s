//////////
/ libc/sys/i386/_execve.s
/ Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991.
//////////

	.globl	_execve
	.globl	.cerror

_execve:
	movl	$59,%eax
	lcall	$0x7,$0
	jmp	.cerror

/ end of libc/sys/i386/_execve.s
