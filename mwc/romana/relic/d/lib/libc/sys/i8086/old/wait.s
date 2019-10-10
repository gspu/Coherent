/
/ C interface for wait system call.
/ pid = wait(statusp);

.globl	wait_

wait_:
	sys	7
	ret
