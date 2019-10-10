/
/ C interface for alarm system call.
/ alarm(nsec);

.globl	alarm_

alarm_:
	sys	033		/27
	ret
