/
/ C interface for times system call.
/ times(tbp);

.globl	times_

times_:
	sys	053		/43
	ret
