////////
/
/ C interface for fcntl system call
/
////////
	.globl	fcntl_

fcntl_:
	sys	66
	ret
