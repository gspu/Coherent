/
/ C interface for setuid system call.
/ setuid(uid);

.globl	setuid_

setuid_:
	sys	027		/23
	ret
