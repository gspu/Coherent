/
/ C interface for getgid system call.
/ gid = getgid();

.globl	getgid_

getgid_:
	sys	057		/47
	ret
