/
/ C interface for umount system call.
/ umount(special);

.globl	umount_

umount_:
	sys	026		/22
	ret
