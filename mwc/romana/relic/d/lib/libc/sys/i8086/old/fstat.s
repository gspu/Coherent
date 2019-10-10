/
/ C interface for fstat system call.
/ fstat(fd, sbp);

.globl	fstat_

fstat_:
	sys	034		/28
	ret
