/
/ C interface for setgid system call
/ setgid(gid);

.globl	setgid_

setgid_:
	sys	056		/46
	ret
