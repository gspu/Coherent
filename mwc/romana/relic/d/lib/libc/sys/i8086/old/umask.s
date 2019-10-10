/
/ C interface for umask system call
/ umask(complmode);

.globl	umask_

umask_:
	sys	074		/60
	ret
