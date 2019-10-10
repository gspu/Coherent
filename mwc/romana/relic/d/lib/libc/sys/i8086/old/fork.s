/
/ C interface for fork system call.
/ pid = fork();

.globl	fork_

fork_:
	sys	2
	ret
