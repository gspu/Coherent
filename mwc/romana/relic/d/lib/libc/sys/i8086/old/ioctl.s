/
/ C interface for ioctl system call
/ ioctl(fd, req, vec);

.globl	ioctl_

ioctl_:
	sys	066		/54
	ret
