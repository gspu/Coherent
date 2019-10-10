/
/ C interface for creat system call.
/ fd = creat(name, mode);

.globl	creat_

creat_:
	sys	010		/8
	ret
