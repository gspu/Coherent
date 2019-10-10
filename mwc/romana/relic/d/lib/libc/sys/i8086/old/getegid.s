/
/ C interface for getegid system call
/ gid = getegid();

.globl	getegid_

getegid_:
	sys	070		/56
	ret
