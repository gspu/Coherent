/
/ Common C interface for all forms of
/ exec system call.
/ execve(name, argp, envp);

.globl	execve_

execve_:
	sys	013		/11
	ret
