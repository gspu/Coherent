/
/ C interface for getuid system call
/ uid = getuid();

.globl	getuid_

getuid_:
	sys	030		/24
	ret
