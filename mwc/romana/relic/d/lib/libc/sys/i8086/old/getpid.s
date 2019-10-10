/
/ C interface for getpid system call.
/ pid = getpid();

.globl	getpid_

getpid_:
	sys	024		/20
	ret
