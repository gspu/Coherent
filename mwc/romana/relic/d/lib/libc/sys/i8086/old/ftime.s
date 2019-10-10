/
/ C interface for ftime system call
/ ftime(timebp);

.globl	ftime_

ftime_:
	sys	043		/35
	ret
