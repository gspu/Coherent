/
/ C interface for sync system call
/ sync();

.globl	sync_

sync_:
	sys	044		/36
	ret
