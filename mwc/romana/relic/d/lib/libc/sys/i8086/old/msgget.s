////////
/
/ C interface for msgget system call
/ int msgget(key, msgflg)
/ key_t key; int msgflg;
/
////////
	.globl	msgget_

msgget_:
	sys	69
	ret
