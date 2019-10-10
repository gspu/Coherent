/
/ C interface for stat system call.
/ stat(file, sbp);

.globl	stat_

stat_:
	sys	022		/18
	ret
