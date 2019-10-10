/
/ C interface for utime system call
/ utime(file, timep);

.globl	utime_

utime_:
	sys	036		/30
	ret
