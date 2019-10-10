/
/ C interface for open system call.
/ fd = open(name, mode);

.globl	open_

open_:
	sys	5
	ret
