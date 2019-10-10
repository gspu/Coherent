/
/ C interface for execk system call.
/ execk(name, argp, envp);

.globl	execk_

execk_:
	sys	076		/62
	ret
