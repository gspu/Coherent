//////////
/  void
/  ftime(tbp)
/  struct timeb * tbp;
//////////

	.unixorder
	.globl ftime, errno

ftime:
	movl	$0xB28, %eax
	lcall	$7, $0
	jb	foo
	ret

foo:
	movl	%eax, errno
	movl	$-1, %eax
	ret
