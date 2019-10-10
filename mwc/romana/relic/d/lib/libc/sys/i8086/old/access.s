/
/ C interface for access system call.
/ access(file, mode);

.globl	access_

access_:
	sys	041		/33
	ret
