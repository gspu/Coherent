/
/ C interface for pipe system call.
/ pipe(pdes);
/ int pdes[2];

.globl	pipe_

pipe_:
	sys	052		/42
	ret
