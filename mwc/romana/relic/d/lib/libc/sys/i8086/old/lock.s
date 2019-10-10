/
/ C interface for lock system call
/ lock(flag);

.globl	lock_

lock_:
	sys	065		/53
	ret
