/
/ C interface for close system call.
/ close(fd);

.globl	close_

close_:
	sys	6
	ret
