////////
/
/ C interface for alarm2 system call
/ long alarm2(n)
/ long n;
/
////////
	.globl	alarm2_
alarm2_:
	sys	72
	ret
