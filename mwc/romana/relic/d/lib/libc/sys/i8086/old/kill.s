/
/ C interface for kill system call.
/ kill(pid, sig);

.globl	kill_

kill_:
	sys	045		/37
	ret
