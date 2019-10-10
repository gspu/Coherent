/
/ C interface for write system call.
/ count = write(fd, bp, nb);

.globl	write_

write_:
	sys	4
	ret
