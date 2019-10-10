////////
/
/ C interface for msgctl system call
/ int msgctl(msgid, cmd, buf)
/ int msgid; int cmd; struct msqid_ds *buf;
/
////////
	.globl	msgctl_

msgctl_:
	sys	68
	ret
