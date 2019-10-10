/
/ C interface for chmod system call.
/ chmod(file, mode);

.globl	chmod_

chmod_:
	sys	017		/15
	ret
