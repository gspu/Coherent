/
/ C interface for chdir system call.
/ chdir(directory);

.globl	chdir_

chdir_:
	sys	014		/12
	ret
