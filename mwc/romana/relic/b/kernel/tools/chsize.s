//////////
/  chsize(fd, size)
/  int fd;
/  long size;
//////////

	.unixorder
	.globl chsize, errno

chsize:
	movl	$0xA28, %eax
	lcall	$7, $0
	jb	foo
	ret

foo:
	movl	%eax, errno
	movl	$-1, %eax
	ret
