////////
/
/ C interface for tick system call
/ long tick();
/
////////
	.globl	tick_
tick_:
	sys	73
	ret
