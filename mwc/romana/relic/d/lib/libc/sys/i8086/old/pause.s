/
/ C interface for pause system call.
/ pause();

.globl	pause_

pause_:
	sys	035		/29
	ret
