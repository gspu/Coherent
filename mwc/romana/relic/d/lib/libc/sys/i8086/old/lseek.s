/
/ C interface for lseek system call.
/ lseek(fd, offset, ptrname);

.globl	lseek_

lseek_:
	sys	023		/19
	ret
