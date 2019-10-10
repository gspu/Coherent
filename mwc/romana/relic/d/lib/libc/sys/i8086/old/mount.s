/
/ C interface for mount system call.
/ mount(special, name, rwflag);

.globl	mount_

mount_:
	sys	025		/21
	ret
