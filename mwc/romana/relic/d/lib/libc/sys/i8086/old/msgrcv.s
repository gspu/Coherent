////////
/
/ C interface for msgrcv system call
/ int msgrcv(msgid, msgp, sgsz, msgtyp, msgflg)
/ int msgid, msgsz, msgflg; struct msgbuf *msgp; long msgtyp;
/
////////
	.globl	msgrcv_

msgrcv_:
	sys	70
	ret
