/*
 * libc/sys/msgop.c
 * User Message Functions.
 * Note: msgget() must be first function called.
 */

#include <sys/msg.h>
#include <errno.h>

static int  msgfno   = -1;
static char msgdev[] = "/dev/msg";

/*
 * Message Control Operations.
 */
msgctl(msqid, cmd, buf) int msqid, cmd; struct msqid_ds * buf;
{
	int parm[4];

	if (msgfno < 0) {
		errno = ENODEV;
		return -1;
	}

	parm[0] = -1;
	parm[1] = msqid;
	parm[2] = cmd;
	parm[3] = (int) buf;

	ioctl(msgfno, MSGCTL, parm);
	return parm[0];
}

/*
 * Get Message Queue.
 */
msgget(key, msgflg) key_t key; int msgflg;
{
	int parm[4];

	if (msgfno < 0) {
		if ((msgfno = open(msgdev, 0)) < 0) {
			perror(msgdev);
			errno = ENODEV;
			return -1;
		}
	}

	parm[0] = -1;
	parm[1] = key;
	parm[2] = key >> 16;
	parm[3] = msgflg;

	ioctl(msgfno, MSGGET, parm);
	return parm[0];
}

/*
 * Send Message.
 */
msgsnd(msqid, msgp, msgsz, msgflg) int msqid; struct msgbuf *msgp; int msgsz, msgflg;
{
	int parm[5];

	if (msgfno < 0) {
		errno = ENODEV;
		return -1;
	}

	parm[0] = -1;
	parm[1] = msqid;
	parm[2] = (int) msgp;
	parm[3] = msgsz;
	parm[4] = msgflg;

	ioctl(msgfno, MSGSND, parm);
	return parm[0];
}

/*
 * Receive Message.
 */
msgrcv(msqid, msgp, msgsz, msgtyp, msgflg)
int msqid; struct msgbuf *msgp; int msgsz; long msgtyp; int msgflg;
{
	int parm[7];

	if (msgfno < 0) {
		errno = ENODEV;
		return -1;
	}

	parm[0] = -1;
	parm[1] = msqid;
	parm[2] = (int) msgp;
	parm[3] = msgsz;
	parm[4] = (int) msgtyp;
	parm[5] = (int) (msgtyp >> 16);
	parm[6] = msgflg;

	ioctl(msgfno, MSGRCV, parm);
	return parm[0];
}

/* end of msgop.c */
