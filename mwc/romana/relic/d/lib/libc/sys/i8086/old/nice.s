/
/ C interface for nice system call.
/ nice(prio);

.globl	nice_

nice_:
	sys	042		/34
	ret
