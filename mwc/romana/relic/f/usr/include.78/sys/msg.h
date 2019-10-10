/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef __SYS_MSG_H__
#define	__SYS_MSG_H__
/*
**	IPC Message Facility.
*/
#include <sys/ipc.h>
#include <sys/_time.h>

/*
**	Permission Definitions.
*/

#define	MSG_R	0400		/* read permission */
#define	MSG_W	0200		/* write permission */

/*
**	ipc_perm Mode Definitions.
*/

#define	MSG_RWAIT	01000	/* a reader is waiting for a message */
#define	MSG_WWAIT	02000	/* a writer is waiting to send */

/*
**	Message Operation Flags.
*/

#define	MSG_NOERROR	010000	/* no error if big message */

/*
**	Structure Definitions.
*/

/*
**	There is one msg structure for each message that may be in the system.
*/

struct msg {

	struct	msg	*msg_next;	/* pointer to next message on q */
	long		msg_type;	/* message type */
	short		msg_ts;		/* message text size */
	short		msg_spot;	/* message text map address */
};

/*
**	There is one msg queue id data structure for each q in the system.
*/

struct msqid_ds {

	struct ipc_perm	msg_perm;	/* operation permission struct */
	struct msg	*msg_first;	/* ptr to first message on q */
	struct msg	*msg_last;	/* ptr to last message on q */
	unsigned short	msg_cbytes;	/* current # bytes on q */
	unsigned short	msg_qnum;	/* # of messages on q */
	unsigned short	msg_qbytes;	/* max # of bytes on q */
	unsigned short	msg_lspid;	/* pid of last msgsnd */
	unsigned short	msg_lrpid;	/* pid of last msgrcv */
	time_t		msg_stime;	/* last msgsnd time */
	time_t		msg_rtime;	/* last msgrcv time */
	time_t		msg_ctime;	/* last change time */
};

/*
**	User message buffer template for msgsnd and msgrcv system calls.
*/

struct msgbuf {

	long		mtype;		/* message type */
	char		mtext[];	/* message text */
};

/*
**	Ioctl commands issued to Message device driver (Coherent specific).
*/

#define	MSGIOC		('M'<<8)
#define	MSGCTL		(MSGIOC|0)
#define	MSGGET		(MSGIOC|1)
#define	MSGSND		(MSGIOC|2)
#define	MSGRCV		(MSGIOC|3)

#endif
