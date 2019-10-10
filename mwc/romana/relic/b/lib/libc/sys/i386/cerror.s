//////////
/ libc/sys/i386/cerror.s
/ Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991.
//////////

//////////
/ Common error exit for system calls.
//////////

	.globl	.cerror
	.globl	errno
	.comm	errno,4

.cerror:
	movl	%eax,errno
	movl	$-1,%eax
	orl	%eax,%eax
	ret

/ end of libc/sys/i386/cerror.s
