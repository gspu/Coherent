/
/ C interface for mknod system call.
/ mknod(file, mode, addr);

.globl	mknod_

mknod_:
	sys	016		/14
	ret
