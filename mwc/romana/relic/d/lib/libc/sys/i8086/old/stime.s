/
/ C interface for stime system call
/ stime(time);

.globl	stime_

stime_:
	sys	031		/25
	ret
