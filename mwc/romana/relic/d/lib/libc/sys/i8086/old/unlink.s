/
/ C interface for unlink system call.
/ unlink(name);

.globl	unlink_

unlink_:
	sys	012		/10
	ret
