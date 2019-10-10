/
/ C interface for unique system call
/ l = unique();

.globl	unique_

unique_:
	sys	055		/45
	ret
