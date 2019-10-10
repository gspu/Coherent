////////
/
/ C interface for msgsnd system call
/ int msgsnd(msgid, msgp, msgsz, msgflg)
/ int msgid, msgsz, msgflg; struct msgbuf *msgp;
/
////////
	.globl	msgsnd_

msgsnd_:
	sys	71
	ret
