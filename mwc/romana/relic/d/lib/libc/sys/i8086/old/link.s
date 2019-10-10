/
/ C interface for link system call.
/ link(old, new);

.globl	link_

link_:
	sys	011		/9
	ret
