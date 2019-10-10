//////////
/ libc/sys/i386/pipe.s
/ Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991.
//////////

//////////
/ pipe(fd) int fd[2];
//////////

        .text
	.globl	pipe
	.globl	.cerror

pipe:
	movl	$42,%eax
	lcall	$0x7,$0
	jc	.cerror

	movl	4(%esp),%ecx		
	movl	%eax,(%ecx)		/ fd[0] = fd1
	movl	%edx,4(%ecx)		/ fd[2] = fd2;
	subl	%eax,%eax
	ret				/ return 0

/ end of libc/sys/i386/pipe.s
