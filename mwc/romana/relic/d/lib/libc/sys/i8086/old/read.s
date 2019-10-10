/
/ C interface for read system call.
/ count = read(fd, bp, nb);

.globl	read_

read_:
	sys	3
	ret
