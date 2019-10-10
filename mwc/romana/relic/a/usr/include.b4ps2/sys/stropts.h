/* $Header: /src386/usr/include/sys/RCS/stropts.h,v 1.2 92/09/29 09:27:28 bin Exp Locker: bin $
 *
 * System V Stream Operations.
 *
 * $Log:	stropts.h,v $
 * Revision 1.2  92/09/29  09:27:28  bin
 * updated with kernel 63 src
 * 
 */
#ifndef	STROPTS_H
#define	STROPTS_H

/*
 * Read options
 */
#define	RNORM	0		/* read msg norm */
#define	RMSGD	1		/* read msg discard */
#define	RMSGN	2		/* read msg no discard */

/*
 * Flush options
 */
#define	FLUSHR	1		/* flush read queue */
#define	FLUSHW	2		/* flush write queue */
#define	FLUSHRW	3		/* flush both queues */

/*
 * Events for which to be sent SIGPOLL signal
 */
#define	S_INPUT	 001		/* regular priority msg on read Q */
#define	S_HIPRI	 002		/* high priority msg on read Q */
#define	S_OUTPUT 004		/* write Q no longer full */
#define	S_MSG	 010		/* signal msg on front of read Q */

/*
 * Flags for recv() and send() syscall arguments
 */
#define	RS_HIPRI 1		/* send/recv high priority messages */

/*
 * Flags returned as value of recv() syscall
 */
#define	MORECTL	 1		/* more ctl info is left in message */
#define	MOREDATA 2		/* more data is left in message */

/*
 * Stream Ioctl defines
 */
#define	STR		('S'<<8)
#define	I_NREAD		(STR|01)
#define	I_PUSH		(STR|02)
#define	I_POP		(STR|03)
#define	I_LOOK		(STR|04)
#define	I_FLUSH		(STR|05)
#define	I_SRDOPT	(STR|06)
#define	I_GRDOPT	(STR|07)
#define	I_STR		(STR|010)
#define	I_SETSIG	(STR|011)
#define	I_GETSIG	(STR|012)
#define	I_FIND		(STR|013)
#define	I_LINK		(STR|014)
#define	I_UNLINK	(STR|015)
#define	I_PEEK		(STR|017)
#define	I_FDINSERT	(STR|020)
#define	I_SENDFD	(STR|021)
#define	I_RECVFD	(STR|022)

/*
 * User level ioctl format for ioctl that go downstream I_STR.
 */
struct strioctl {
	int	ic_cmd;		/* command */
	int	ic_timeout;	/* timeout value */
	int	ic_len;		/* length of data */
	char *	ic_dp;		/* pointer to data */
};

/*
 * Values for timeouts (ioctl,select) that denotes infinity.
 */
#define	INFTIM		(-1)

/*
 * Stream buffer structure for send and recv system calls.
 */
struct strbuf {
	int	maxlen;		/* number of bytes in buffer */
	int	len;		/* number of bytes returned */
	char *	buf;		/* pointer to data */
};

/*
 * Stream I_PEEK ioctl format.
 */
struct strpeek {
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	long		flags;
};

/*
 * Stream I_FDINSERT ioctl format.
 */
struct strfdinsert {
	struct strbuf	ctlbuf;
	struct strbuf	databuf;
	long		flags;
	int		fildes;
	int		off/* set */;
};

/*
 * Receive file descriptor structure.
 */
struct strrecvfd {
#ifdef	KERNEL
	union {
		struct file * fp;
		int fd;
	} f;
#else
	int fd;
#endif
	unsigned short uid;
	unsigned short gid;
	char fill[8];
};

#endif
