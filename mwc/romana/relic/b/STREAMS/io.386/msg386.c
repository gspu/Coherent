/*
 * System V Compatible Messaging
 *
 * This module provides System V compatible messaging operations.
 */
#include <sys/coherent.h>
#include <sys/sched.h>
#include <sys/types.h>
#include <sys/uproc.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/con.h>
#include <sys/seg.h>
#include <sys/msg.h>

/*
 * Global Message Parameters. We want them to be patchable.
 */
unsigned NMSQID = 50;	/* maximum number of message queues */
unsigned NMSQB  = 4096;	/* default maximum queue size in bytes */
unsigned NMSG = 40;	/* maximum number of messages per queue */
unsigned NMSC = 2048;	/* message text size */

#ifdef	TRACER
int	dballoc = 0;	/* For debug only */
int	dbfree = 0;
#endif

/* Message Information */
struct msqid_ds *msqs = NULL;	/* Array of message queues */
GATE		*msg_gate;	/* Message gates */
char		**msg_map;	/* Memory map */

/* 
 * Msgctl - Message Control Operations.
 */
umsgctl(qid, cmd, buf)
int qid;
int cmd;
struct msqid_ds *buf;
{
	register struct msqid_ds 	*qp;	/* message queues */
	register struct msg 		*mp;	/* single message queue */
	unsigned short	 		n;	/* temporary variable */

	/* Validate qid */
	if (qid < 0)  {
		u.u_error = EINVAL;
		return -1;
	}
	qp = &msqs[qid % NMSQID];

	/* Validate queue existence.*/
	if (qp->msg_perm.seq != qid || (qp->msg_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return -1;
	}

	switch (cmd) {
	case IPC_STAT:
		/* Validate access authority. */
		if ((ipcaccess(&qp->msg_perm) & MSG_R) == 0) {
			u.u_error = EACCES;
			break;
		}

		 /* Copy queue info to user buffer */
		kucopy(qp, buf, sizeof(struct msqid_ds));
		break;
	case IPC_SET:
		 /* Validate modify authority. */
		if ((u.u_uid != 0) && (u.u_uid != qp->msg_perm.uid)) {
			u.u_error = EPERM;
			break;
		}

		/*
		 * Get desired queue size.
		 */
		n = getusd(&(buf->msg_qbytes));
		if (u.u_error)
			break;

		/*
		 * Only super-user can increase queue size.
		 */
		if ((u.u_uid != 0) && (n > qp->msg_qbytes)) {
			u.u_error = EPERM;
			break;
		}

		/*
		 * Set queue parameters.
		 */
		qp->msg_perm.uid = getusd(&(buf->msg_perm.uid));
		qp->msg_perm.gid = getusd(&(buf->msg_perm.gid));
		qp->msg_perm.mode &= ~0777;
		qp->msg_perm.mode |= getusd(&(buf->msg_perm.mode)) & 0777;
		/* We may want to change the max size of a single message too.
		 * It is not obvious how to do it. There is no
		 * description in SVID. So it is possible that at some point
		 * the size of the single message happens to be greater than
		 * the size of message queue ;-(
		 */
		qp->msg_qbytes = NMSQB = n;
		break;

	case IPC_RMID:
		 /* Validate removal authority. */
		if ((u.u_uid != 0) && (u.u_uid != qp->msg_perm.uid)) {
			u.u_error = EPERM;
			break;
		}
		/* Free all messages on the queue being removed. */
		while (mp = qp->msg_first) {
			qp->msg_first = mp->msg_next;
			T_MSGQ(0x01, dballoc -= sizeof(struct msg));
			msgfree(mp);
		}
		T_MSGQ(0x01, printf("F%d", dballoc));

		/* Reset queue parameters. */
		qp->msg_last = NULL;
		qp->msg_qnum = 0;
		qp->msg_cbytes = 0;
		qp->msg_perm.mode = 0;
		/* Set last change time */
		qp->msg_ctime  = timer.t_time; 
		/* We have to pick up a new unique sequence number.
		 * There is a "wrap around bug". But, it is BCS.
		 */
		qp->msg_perm.seq += (unsigned short) 50;
		break;
	default:
		u.u_error = EINVAL;
	}

	if (u.u_error)
		return -1;

	return 0;
}

/*
 * Msgget - Get set of messages
 */
umsgget(mykey, msgflg)
key_t mykey;
int msgflg;
{
	register struct msqid_ds *qp;		
	register struct msqid_ds *freeidp = NULL;	
	int rwmode;
	
	/* Init message queues on the first msgget */
	if (msqs == NULL)
		if (msginit()) {
			u.u_error = ENOSPC;
			return -1;
		}
	/* Extract desired access mode from flags. */
	rwmode = msgflg & 0777; /* 0666 ??? */
	T_MSGQ(0x02, printf("U%o", rwmode));
	/* Search for desired message queue [also for first free queue]. */
	for (qp = msqs; qp < msqs + NMSQID; qp++) {
		/* Look for an older free queue */
		if (!(qp->msg_perm.mode & IPC_ALLOC)) {
			if (freeidp == NULL 
			   		|| freeidp->msg_ctime > qp->msg_ctime)
				freeidp = qp;
			continue;
		}
		if (mykey == IPC_PRIVATE) {	/* creat a new queue */
			if (msgflg & IPC_EXCL)	/* unique new queue */
				if (mykey == qp->msg_perm.key) {
					u.u_error = EEXIST;/* We cannot creat */
					return -1;	   /* exclusive queue */
				}
			continue;
		}
		if (qp->msg_perm.key != mykey) 	
			continue;

		if (mykey == qp->msg_perm.key) {	/* found! */
			if ((msgflg & IPC_CREAT) && (msgflg & IPC_EXCL)) {
				u.u_error = EEXIST;	/* We cannot creat */
				return -1;		/* exclusive queue */
			}

			/* PERMISSIONS */
			/* For super-user or if mode is 0 */
			if (u.u_uid == 0 || !rwmode) 
				return qp->msg_perm.seq;

			/* For owner or creator */
			T_MSGQ(0x02, printf("Q%o", qp->msg_perm.mode));
			if (u.u_uid == qp->msg_perm.uid 
			  || u.u_uid == qp->msg_perm.cuid) {
				if ((rwmode & 0600) & (qp->msg_perm.mode & 0600))
					return qp->msg_perm.seq;
				else {
					u.u_error = EACCES;
					return -1;
				}
			}

			/* For group */		
			if (u.u_gid == qp->msg_perm.gid 
			  || u.u_gid == qp->msg_perm.cgid) {
				if ((rwmode & 060) & (qp->msg_perm.mode & 060))
					return qp->msg_perm.seq;
				else {
					u.u_error = EACCES;
					return -1;
				}
			}

			/* For the rest of the world */
			if ((rwmode & 06) & (qp->msg_perm.mode & 06)) 
				return qp->msg_perm.seq;
			else {
				u.u_error = EACCES;
				return -1;
			}
		}
	}

	/* Creat a new queue */
	if (!(msgflg & IPC_CREAT)) {
		u.u_error = ENOENT;
		return -1;
	}
	if ((qp = freeidp) == NULL) {
		u.u_error = ENOSPC;
		return -1;
	}
	/* Set new queue */
	qp->msg_ctime  = timer.t_time; 
	qp->msg_perm.cuid = qp->msg_perm.uid = u.u_uid;
	qp->msg_perm.cgid = qp->msg_perm.gid = u.u_gid;
	qp->msg_perm.mode = rwmode | IPC_ALLOC;
	qp->msg_qnum = qp->msg_lspid = qp->msg_lrpid = qp->msg_stime 
		= qp->msg_rtime = 0;
	qp->msg_perm.key  = mykey;

	return qp->msg_perm.seq;
}

/*
 * Allocate space for the message queues and gates.
 * Initialize message queue headers
 * Return -1 on error.
 */
msginit()
{
	struct msqid_ds	*qp;
	
	T_MSGQ(0x01, printf("A%d",dballoc += sizeof(struct msqid_ds) * NMSQID));

	/* Allocate space for message headers */
	if ((msqs = 
	  (struct msqid_ds *) kalloc(sizeof(struct msqid_ds) * NMSQID)) == NULL)
		return -1;

	T_MSGQ(0x01, printf("A%d", dballoc += sizeof(GATE) * NMSQID));

	/* Allocate space for message gates */
	if ((msg_gate = (GATE *) kalloc(sizeof(GATE) * NMSQID)) == NULL) {
		kfree(msqs);
		msqs = NULL;
		return -1;
	}

	T_MSGQ(0x01, printf("A%d", dballoc += sizeof(char *) * NMSQID * NMSG));

	/* Allocate space for the message map */
	if ((msg_map = kalloc(sizeof(char *) * NMSQID * NMSG)) == NULL) {
		kfree(msqs);
		msqs = NULL;
		kfree(msg_gate);
		return -1;
	}
	/* Clear gate and map areas */
	kclear(msg_gate, sizeof(GATE) * NMSQID);
	kclear(msg_map, sizeof(char *) * NMSQID * NMSG);
	/* Set initial queue values */
	for (qp = msqs; qp < msqs + NMSQID; qp++) {
		qp->msg_first  = NULL;	/* First and last pointers to 	*/
		qp->msg_last   = NULL;	/* message queue	      	*/
		qp->msg_cbytes = 0;	/* Number of bytes in queue	*/
		qp->msg_qnum   = 0;	/* Number of messages in queue	*/
		qp->msg_qbytes = NMSQB;	/* Max size of a queue		*/
		qp->msg_lspid  = 0;	/* Pid of last msgsnd		*/
		qp->msg_lrpid  = 0;	/* Pid of last msgrcv		*/
		qp->msg_stime  = 0;	/* Last msgsnd time		*/
		qp->msg_rtime  = 0;	/* Last msgrcv time		*/
		qp->msg_ctime  = timer.t_time; /* Last change time	*/
		qp->msg_perm.seq = qp - msqs;
		qp->msg_perm.mode = 0;
	}
	return 0;
}

/* 
 * Remove the message.	Clear message text and header. Reset values in 
 * message map.
 */
msgfree(mp)
struct msg	*mp;	/* Message header to be remove */
{

	kfree(msg_map[mp->msg_spot]);
	msg_map[mp->msg_spot] = NULL;
	kfree(mp);
	T_MSGQ(0x01, printf("F%d", dballoc-=(mp->msg_ts + sizeof(struct msg))));
}

/*
 * Send a Message
 */
umsgsnd(qid, bufp, msgsz, msgflg)
int 	qid;		/* queue id */
struct 	msgbuf *bufp;	/* message buffer */
int 	msgsz, 		/* message size */
	msgflg;		/* flags */
{
	register struct msqid_ds	*qp;	/* message queue */
	register struct msg      	*mp,	/* message struct */ 
					*tmp;
	int				q_num;	/* number of a queue */
	int				i_spot;	/* # of empty entry in map */

	/* Validate queue identifier. */
	for (qp = msqs; qp < msqs + NMSQID; qp++) 
		if (qp->msg_perm.seq == qid) 	/* found */
			break;

	q_num = qp - msqs;		
	/* qid is not a valid qid identifier */
	if (q_num >= NMSQID) {
		u.u_error = EINVAL;
		return -1;
	}

	if (!(ipcaccess(&qp->msg_perm) & MSG_W)) { /* can't send mesg */
		u.u_error = EACCES;
		return -1;
	}

	/* Check if message has a valid message type and size.
	 * The comparisson with NMSQB was done because user could
	 * reduce this value.
	 */
	if (bufp->mtype < 1 || msgsz < 0 || msgsz > NMSC || msgsz > NMSQB) {
		u.u_error = EACCES;
		return -1;
	}
	
	/* Now we have a valid message. Check if we can send it. */
	lock(msg_gate[q_num]);	/* Lock it to avoid race condition */
	while (qp->msg_qnum >= NMSG || 
	 		qp->msg_qbytes < (qp->msg_cbytes + msgsz)) {
		if (msgflg & IPC_NOWAIT) {
			u.u_error = EAGAIN;
			unlock(msg_gate[q_num]);
			return -1;
		}
		/* We have to wait here */
		qp->msg_perm.mode |= MSG_WWAIT;
		unlock(msg_gate[q_num]);

		if (x_sleep (qp, pritty, slpriSigCatch, "umsgsnd")
		    == PROCESS_SIGNALLED) {
			/* Abort if a signal was received */
			u.u_error = EINTR;
			return -1;
		}

		/* Abort if the message queue was removed. */
		if (qid != qp->msg_perm.seq) {
			u.u_error = EINVAL;
			return -1;
		}
		lock(msg_gate[q_num]);
	}

	/* Find empty entry in message map */
	for (i_spot = 0; i_spot < NMSQID * NMSG; i_spot++)
		if (msg_map[i_spot] == NULL)
			break;
	/* It cannot happen when we do not have empty entry in map,
	 * but let check it.
	 */
	if (i_spot >= NMSQID * NMSG) {
		u.u_error = ENOSPC;
		return -1;
	}

	T_MSGQ(0x01, printf("A%d", dballoc += sizeof(struct msg)));

	/* Get space for the message header */
	if ((mp = kalloc(sizeof(struct msg))) == NULL) {
		unlock(msg_gate[q_num]);
		u.u_error = ENOSPC;
		return -1;
	}

	T_MSGQ(0x01, printf("A%d", dballoc += msgsz));

	/* Alloc space for the message text */
	if ((msg_map[i_spot] = kalloc(msgsz)) == NULL) {
		kfree(mp);
		unlock(msg_gate[q_num]);
		u.u_error = ENOSPC;
		return -1;
	}

	mp->msg_next = NULL;
	mp->msg_ts = msgsz;
	/* The map address is a number of msg_map array element */
	mp->msg_spot = i_spot;

	/* Transfer the message type and text.*/
	if (ukcopy(&(bufp->mtype), &(mp->msg_type), sizeof(mp->msg_type)) !=
						sizeof(mp->msg_type))
		u.u_error = EFAULT;
	if (ukcopy(&bufp->mtext[0], msg_map[i_spot], msgsz) != msgsz)
		u.u_error = EFAULT;
	if (u.u_error) {
		msgfree(mp);
		unlock(msg_gate[q_num]);
		return -1;
	}

	/* Move the message to the desired queue. */
	if (qp->msg_first == NULL) /* This is the first message per queue */
		qp->msg_first = qp->msg_last = mp;
	else {	/* There are messages */
		/* Find last message in gueue */
		for (tmp = qp->msg_first; ; tmp = tmp->msg_next)
			if (tmp->msg_next == NULL)
				break;
		qp->msg_last = tmp->msg_next = mp;
	}
	mp->msg_next = NULL;

	/* Update queue statistics. */
	qp->msg_cbytes += msgsz;
	qp->msg_qnum++;
	qp->msg_lspid = SELF->p_pid;
	qp->msg_stime = timer.t_time;

	/* Unlock queue and wake processes waiting to receive. */
	unlock(msg_gate[q_num]);
	if (qp->msg_perm.mode & MSG_RWAIT) {
		qp->msg_perm.mode &= ~MSG_RWAIT;
		wakeup(qp);
	}
	return 0;
}

/*
 * Receive a Message
 */
umsgrcv(qid, bufp, msgsz, msgtyp, msgflg)
int		qid;	/* Message queue id	*/
struct msgbuf 	*bufp;	/* Message buffer	*/
int		msgsz;	/* Message text size	*/
long		msgtyp;	/* Message type		*/
int 		msgflg;	/* Message flag		*/
{
	register struct msqid_ds	*qp;	/* queue headers	*/
	register struct msg 		*mp,	/* message headers	*/
					*prev;
	int				q_num;	/* queue number 	*/
	int				i_spot;

	/* Validate queue identifier. */
	if (qid < 0 || msqs == NULL) {
		u.u_error = EINVAL;
		return -1;
	}
	q_num = qid % NMSQID;
	qp = &msqs[q_num];

	/* Validate queue existence.*/
	if (qp->msg_perm.seq != qid || (qp->msg_perm.mode & IPC_ALLOC) == 0) {
		u.u_error = EINVAL;
		return -1;
	}

	/* Permission denied */
	if ((ipcaccess(&qp->msg_perm) & MSG_R) == 0) {
		u.u_error = EACCES;
		return -1;
	}

	/* Wait for message */
	lock(msg_gate[q_num]);
	for (;;) {
		prev = NULL;
		mp = qp->msg_first;
		/* Find mesg of type <= abs(msgtyp) */
		if (msgtyp < 0) {
			struct msg *qmin;	/* Message with lowest mtype */
			struct msg *xprev;	/* Previous message */
			
			qmin = NULL;
			xprev = prev;
			msgtyp = -msgtyp;

			for (; mp != NULL; prev = mp, mp = mp->msg_next) {
				if (mp->msg_type > msgtyp)
					continue;
				if (qmin == NULL 
					   || mp->msg_type < qmin->msg_type) {
					xprev = prev;
					qmin = mp;
				}
			}
			prev = xprev;
			mp  = qmin;
			msgtyp = -msgtyp;
		} else if (msgtyp > 0) { /* Find message of type == msgtyp */
			while (mp != NULL && mp->msg_type != msgtyp) {
				prev = mp;
				mp = mp->msg_next;
			}
		} else 	/* Else take first message */
			mp = qp->msg_first;

		if (mp != NULL)	/* Found */
			break;
	
		/* Can't wait to receive mesg */
		if (msgflg & IPC_NOWAIT) {
			u.u_error = ENOMSG;
			unlock(msg_gate[q_num]);
			return -1;
		}

		/* We can go sleep now */
		qp->msg_perm.mode |= MSG_RWAIT;
		unlock(msg_gate[q_num]);
		if (x_sleep (qp, pritty, slpriSigCatch, "umsgrcv")
		    == PROCESS_SIGNALLED) {
			u.u_error = EINTR;
			return -1;
		}

		/* Not same q anymore */
		if (qid != qp->msg_perm.seq) {
			u.u_error = EINVAL;
			return -1;
		}
		lock(msg_gate[q_num]);
	}

	/* Ensure entire message can be transferred, or MSG_NOERROR asserted.*/
	if (msgsz < mp->msg_ts && (msgflg & MSG_NOERROR) == 0) {
		unlock(msg_gate[q_num]);
		u.u_error = E2BIG;
		return -1;
	}

	/* Transfer message data */
	if (msgsz > mp->msg_ts)
		msgsz = mp->msg_ts;

	kucopy(&(mp->msg_type), &(bufp->mtype), sizeof(mp->msg_type));

	i_spot = mp->msg_spot;
	if (kucopy(msg_map[i_spot], bufp->mtext, msgsz) != msgsz)
		u.u_error = EFAULT;

	/* Abort if address fault occurred during transfer. */
	if (u.u_error) {
		unlock(msg_gate[q_num]);
		return -1;
	}
	/* Remove message from queue */
	if (prev != NULL)
		prev->msg_next = mp->msg_next;
	else
		qp->msg_first = mp->msg_next;

	if (qp->msg_last == mp)
		qp->msg_last = prev;


	/* Update queue statistics */
	qp->msg_cbytes -= mp->msg_ts;
	qp->msg_qnum--;
	qp->msg_lrpid = SELF->p_pid;
	qp->msg_rtime = timer.t_time;

	/* free message */
	msgfree(mp);

	unlock(msg_gate[q_num]);
	/* Wakeup processes waiting to send. */
	if (qp->msg_perm.mode & MSG_WWAIT) {
		qp->msg_perm.mode &= ~MSG_WWAIT;
		wakeup(qp);
	}
	return msgsz;
}

