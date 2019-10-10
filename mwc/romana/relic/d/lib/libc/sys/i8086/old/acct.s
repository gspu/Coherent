/
/ C interface for acct system call
/ acct(file);
/ char *file;

.globl	acct_

acct_:
	sys	063		/51
	ret
