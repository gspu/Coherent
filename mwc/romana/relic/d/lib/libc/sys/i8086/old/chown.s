/
/ C interface for chown system call.
/ chown(file, owner, group);

.globl	chown_

chown_:
	sys	020		/16
	ret
