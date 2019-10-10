//////////
/  long
/  nap(msec)
/  int msec;
//////////

	.unixorder
	.globl nap, errno

nap:
	movl	$0xC28, %eax
	lcall	$7, $0
	jb	foo
	ret

foo:
	movl	%eax, errno
	movl	$-1, %eax
	ret
