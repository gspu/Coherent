////////
/
/ C interface for poll system call
/
////////
	.globl	poll_

poll_:
	sys	67
	ret
