/
/ C interface for chroot system call.
/ chroot(directory);

.globl	chroot_

chroot_:
	sys	075		/61
	ret
