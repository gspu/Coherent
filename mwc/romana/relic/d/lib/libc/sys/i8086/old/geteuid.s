/
/ C interface for geteuid system call.
/ uid = geteuid();

.globl	geteuid_

geteuid_:
	sys	071		/57
	ret
