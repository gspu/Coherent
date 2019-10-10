#ifndef __SYS_STREAM_H__
#define __SYS_STREAM_H__

/*
 * This ^^^^^^^^^^^^^^^^ symbol is used in the DDI/DKI header <sys/ddi.h> to
 * determine which #undef directives it is required to perform, on the basis
 * that it should avoid touching namespaces unless they have been reserved
 * by the inclusion of a header which reserves such classes of names.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__PROTO ()
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__VOID__
 *	<common/_cred.h>
 *		cred_t
 *	<kernel/_toid.h>
 *		toid_t
 *	<kernel/x86lock.h>
 *		atomic_uchar_t
 *	<sys/types.h>
 *		caddr_t
 */

#include <common/ccompat.h>
#include <common/_cred.h>
#include <kernel/_toid.h>
#include <kernel/x86lock.h>
#include <sys/types.h>

#include <common/_stream.h>

/*
 * Public section of the <sys/stream.h> file, for consumption by user
 * application code using access to the STREAMS messaging system, and
 * by STREAMS drivers.
 *
 * The information in this header is directly related to the information
 * published in the System V, Release 4 STREAMS Programmer's Guide,
 * Appendices A and B.
 */


/*
 * Message types. The particular values here may have some significance
 * to some macros defined later, particularly datamsg ().
 */

#define M_PRI           0x80
#define QPCTL           M_PRI

/* regular messages */

#define M_DATA          0
#define M_PROTO         1
#define M_DELAY         2

#define M_IOCTL         3
#define M_SETOPTS       4
#define M_SIG           5
#define M_CTL           6
#define M_BREAK         7
#define M_PASSFP        8
#define	M_RSE		9

/* priority messages */

#define M_PCPROTO       (1 + M_PRI)
#define M_IOCACK        (3 + M_PRI)
#define M_IOCNAK        (4 + M_PRI)
#define M_PCSIG         (5 + M_PRI)
#define M_ERROR         (6 + M_PRI)
#define M_FLUSH         (7 + M_PRI)
#define M_HANGUP        (8 + M_PRI)
#define M_START         (9 + M_PRI)
#define M_STOP          (10 + M_PRI)
#define	M_COPYIN	(11 + M_PRI)
#define	M_COPYOUT	(12 + M_PRI)
#define	M_IOCDATA	(13 + M_PRI)
#define	M_PCRSE		(14 + M_PRI)
#define	M_READ		(15 + M_PRI)
#define	M_STARTI	(16 + M_PRI)
#define	M_STOPI		(17 + M_PRI)


struct streamtab {
	struct qinit  *	st_rdinit;	/* defines read QUEUE */
	struct qinit  *	st_wrinit;	/* defines write QUEUE */
	struct qinit  *	st_muxrinit;	/* for multiplexing drivers only */
	struct qinit  *	st_muxwinit;	/* for multiplexing drivers only */
};


/*
 * Private type definitions.
 */

typedef	atomic_uchar_t	__str_lock_t;


/*
 * QUEUE structures - these structures are used to build streams.
 *
 * Note that "queue_t" structures occur in pairs, first the write queue, then
 * the read queue. The QREADR flag distinguishes them for the RD (q), WR (q)
 * and OTHERQ (q) macros.
 *
 * The System V DDI/DKI adds multiprocessor support to STREAMS via the notion
 * of "freezing" a stream, something mostly analagous to a basic lock. We can
 * implement this facility either in terms of the primitive machine-specific
 * atomic operations, or in terms of the DDI/DKI high-level lock operations.
 *
 * For now, we choose the latter.
 */

struct queue {
	struct qinit  *	q_qinfo;	/* procedures and limits for queue */
	mblk_t	      *	q_first;	/* head of message queue for QUEUE */
	mblk_t	      *	q_last;		/* tail of message queue for QUEUE */
	queue_t	      *	q_next;		/* next QUEUE in stream */
	queue_t	      *	q_link;		/* next on STEAMS scheduling list */
	__VOID__      * q_ptr;		/* to private data structure */
	unsigned long	q_count;	/* number of characters in queue */
	unsigned long	q_flag;		/* QUEUE state */
	long		q_minpsz;	/* minimum packet size accepted */
	long            q_maxpsz;	/* maximum packet size accepted */
	unsigned long	q_hiwat;	/* message queue high water mark */
	unsigned long	q_lowat;	/* message queue low water mark */
	qband_t	      *	q_bandp;	/* separate flow information */
	unsigned char	q_nband;	/* number of priority bands */
	unsigned char	q_pad1 [3];	/* reserved for future use */
	long		q_pad2 [2];	/* reserved for future use */

	/*
	 * Data from this point are MWC extensions for multiprocessing. The
	 * DDI/DKI revokes user's right to know about the size of the
	 * structure, among other things.
	 */

	unsigned char	q_active;	/* count of active put/srv routines */
	unsigned char	q_lastband;	/* band of last retrieved message */
	__str_lock_t	q_locked;	/* hack implementation of freezing */
};

/*
 * Hack around the union above. This field should only be referred to by some
 * layering macros in <sys/strmlib.h> anyway, but we try and keep it clean.
 */

#define	q_lock	x.q_lock


/*
 * Queue flag definitions : these are internal to the system, and the values
 * bear no relation to any definitions in System V. Drivers use these flags at
 * their own risk...
 */

#define QENAB		0x0001		/* queue is on qenable () schedule */
#define	QWANTR		0x0002		/* someone wants to read from the queue */
#define	QWANTW		0x0004		/* someone wants to write to the queue */
#define	QFULL		0x0008		/* queue is full */
#define	QREADR		0x0010		/* set for read queues */
#define QNOENB		0x0020		/* prevent queue from being enabled */
#define	QBACK		0x0040		/* queue has been back-enabled */
#define	QOLD		0x0080		/* pre-SVR4 open/close interface */
#define	QPROCSOFF	0x0100		/* put/service routines disabled */
#define	QDRAIN		0x0200		/* waiting for queue to drain */
#define	QSRVACTIVE	0x0400		/* service routine is active */

/* #define	QUSE		?????		 queue has been allocated */
/* #define	QHLIST		?????		 SVR4, not used by this implementation */


/*
 * queue priority-band information.
 */

struct qband {
	struct qband  *	qb_next;	/* next band's info */
	unsigned long	qb_count;	/* count of bytes in band */
	struct msgb   *	qb_first;	/* beginning of band's data */
	struct msgb   *	qb_last;	/* end of band's data */
	unsigned long	qb_hiwat;	/* high water mark for band */
	unsigned long	qb_lowat;	/* low water mark for band */
	unsigned long	qb_flag;	/* flags, see below */
	unsigned long	qb_pad1;	/* reserved for future use */
};



/* Flags used in band structure */

#define	QB_FULL		0x0001		/* band is considered full */
#define	QB_WANTW	0x0002		/* someone wants to write to band */
#define	QB_BACK		0x0004		/* queue has been back-enabled */

/* non-SVR4 flag */
#define	QB_FIRST	0x0100		/* first entry in allocation unit */


/*
 * Message structures - the format of STREAMS messages and message data
 * blocks.
 */

struct msgb {
	struct msgb   * b_next;		/* next message on queue */
	struct msgb   *	b_prev;		/* previous message on queue */
	struct msgb   *	b_cont;		/* next message block of message */
	unsigned char *	b_rptr;		/* first unread data byte in buffer */
	unsigned char *	b_wptr;		/* first unwritten data byte in buffer */
	struct datab  *	b_datap;	/* data block */

	unsigned char	b_band;		/* message priority */
	unsigned char	b_pad1;
	unsigned short	b_flag;		/* message flags */
	long		b_pad2;
};


/* Flag definitions for message blocks */

#define	MSGMARK		0x0001		/* last byte of message is "marked" */
#define	MSGNOLOOP	0x0002		/* don't loop message to write-side of stream */
#define	MSGDELIM	0x0004		/* message is delimited */

/* Extra flag definitions, for STREAMS private use only */

#define	MSGTRIPLE	0x0100		/* message block is part of triple */
#define	MSGFREE		0x0200		/* message block is free */

#define	MSGMASK_SYSTEM	0x0300		/* system-private flags */


/*
 * Data structure used to control freeing of user-defined STREAMS buffer
 * memory (such as user memory that is temporarily mapped to kernel space
 * to avoid copying, or driver-dependent shared memory from an I/O space).
 */

struct free_rtn {
	void         (*	free_func)		/* driver-dependent free routine */
			__PROTO ((__VOID__ * _arg));
	__VOID__      *	free_arg;	/* argument for free_rtn */
};

typedef struct free_rtn frtn_t;


/*
 * Data block descriptors - several mblk_t's may refer to the same section
 * of STREAMS buffer memory to avoid copying.
 *
 * Note that this data structure is NOT identical to it's System V
 * counterpart.
 */

struct datab {
	frtn_t	      * db_frtnp;	/* internal use */
	unsigned char *	db_base;	/* first byte of buffer */
	unsigned char *	db_lim;		/* last+1 byte of buffer */
	unsigned char	db_ref;		/* count of messages pointing here */
	unsigned char	db_type;	/* message type */
	short		db_pad1;	/* reserved for future use */
	long		db_pad2;	/* reserved for future use */
};


/*
 * Define one of these structures with external visibility to publish the
 * entry points to your STREAMS driver.
 */

typedef	void (*	__putp_t)		/* put procedure */
			__PROTO ((queue_t * _q, mblk_t * _mp));
typedef	void (*	__srvp_t)		/* service procedure */
			__PROTO ((queue_t * _q));
typedef	int  (*	__qopen_t)		/* open procedure */
			__PROTO ((queue_t * _q, n_dev_t * _devp, int _flag,
				  int _sflag, cred_t * _credp));
typedef	int  (*	__qclose_t)		/* called on last close/pop */
			__PROTO ((queue_t * _q, int _flag, cred_t * _credp));
typedef	int  (*	__qadmin_t)		/* reserved for future use */
			__PROTO ((void));

struct qinit {
	__putp_t	qi_putp;	/* put procedure */
	__srvp_t	qi_srvp;	/* service procedure */
	__qopen_t	qi_qopen;	/* called on each open */
	__qclose_t	qi_qclose;	/* called on last close/pop */
	__qadmin_t	qu_qadmin;	/* reserved for future use */
	struct module_info
		      *	qi_minfo;  	/* information structure */
	struct module_stat
		      *	qi_mstat;  	/* statistics - optional */
};


/*
 * General STREAMS "module information", mainly used by SysV configuration
 * utilities, although it also provides defaults for the queues at open time.
 */

struct module_info {
	unsigned short	mi_idnum;	/* module ID number */
	char	      *	mi_idname;	/* module name */
	long		mi_minpsz;	/* min packet size accepted */
	long		mi_maxpsz;	/* max packet size accepted */
	unsigned long	mi_hiwat;	/* hi-water mark, for flow control */
	unsigned long	mi_lowat;	/* lo-water mark, for flow control */
};


/* Infinite message size, for use in module_info mi_minpsz/mi_maxpsz */

#define INFPSZ 		0x7FFFFFFF


/*
 * General statistics structure, useful for profiling module/driver
 * activity.
 */

struct module_stat {
	long		ms_pcnt;	/* count of calls to put proc */
	long		ms_scnt;	/* count of calls to service proc */
	long		ms_ocnt;	/* count of calls to open proc */
	long		ms_ccnt;	/* count of calls to close proc */
	long		ms_acnt;	/* count of calls to admin proc */
	char	      *	ms_xptr;	/* pointer to private statistics */
	short		ms_xsize;	/* length of private stats buffer */
};


/*
 * link block for multiplexed streams... this structure appears in the data
 * portion of I_LINK and I_UNLINK M_IOCTL messages.
 *
 * Note that the STREAMS manual documents the type of "l_index" as an "int",
 * but for our purposes an "n_dev_t" (which is a long) is more appropriate.
 */

typedef	n_dev_t		__muxid_t;

struct linkblk {
	queue_t	      *	l_qtop;		/* lowest write queue of upper stream */
	queue_t	      *	l_qbot;		/* highest write queue of lower stream */
	__muxid_t	l_index;	/* unique index for lower stream */
	long		l_pad [5];	/* reserved for future use */
};


/*
 * Stream head options control message format, used as the data part of an
 * M_SETOPTS message set from a driver or module to the stream head. The
 * values for the "so_readopt" member can be found in <stropts.h> as well
 * as below.
 */

struct stroptions {
	unsigned long	so_flags;       /* options to set */
	short		so_readopt;     /* read option */
	unsigned short	so_wroff;       /* write offset */
	long		so_minpsz;      /* min packet size */
	long		so_maxpsz;      /* max packet size */
	unsigned long	so_hiwat;       /* read queue high-water mark */
	unsigned long	so_lowat;       /* read queue low-water mark */
	unsigned char	so_band;	/* band for water marks */
};


/*
 * Flag values for the "so_flags" member,
 */

#define	SO_ALL		0x3F		/* set all options */
#define SO_READOPT      0x0001		/* set read option */
#define SO_WROFF        0x0002		/* set write offset */
#define SO_MINPSZ       0x0004		/* set min packet size */
#define SO_MAXPSZ       0x0008		/* set max packet size */
#define SO_HIWAT        0x0010		/* set read queue high-water mark */
#define SO_LOWAT        0x0020		/* set read queue low-water mark */

/* Flags above control which members to look at, below are R4 extensions */

#define	SO_MREADON	0x0040		/* set read notification on */
#define	SO_MREADOFF	0x0080		/* set read notification off */
#define	SO_NDELON	0x0100		/* old TTY semantics for NDELAY */
#define	SO_NDELOFF	0x0200		/* STREAMS semantics for NDELAY */
#define	SO_ISTTY	0x0400		/* stream is acting as a terminal */
#define	SO_ISNTTY	0x0800		/* stream is not acting as a terminal */
#define	SO_TOSTOP	0x1000		/* stop on background writes to stream */
#define	SO_TONSTOP	0x2000		/* don't stop on background writes */
#define	SO_BAND		0x4000		/* water marks that affect band */

/*
 * Other flags relevant to the above structure are defined in <sys/stropts.h>.
 */


/*
 * Structure of data block found at head of M_IOCTL message, usually reused
 * for replying to M_IOCTL with an M_IOCACK or M_IOCNAK message.
 */

struct iocblk {
	int             ioc_cmd;	/* ioctl () command type */
	cred_t	      * ioc_cr;		/* user's full credentials */
	unsigned int	ioc_id;         /* M_IOCTL sequence number */
	unsigned int	ioc_count;	/* bytes in data field */
	int             ioc_error;	/* error code */
	int             ioc_rval;	/* return value */

	long		ioc_filler [4];	/* reserved for future use */
};


/*
 * For compatibility with SVR3 sources
 */

#define	ioc_uid		ioc_cr->cr_uid
#define	ioc_gid		ioc_cr->cr_gid


/*
 * This special value of "ioc_count" is used to indicate that the stream
 * head has passed us a "transparent" ioctl rather than an I_STR ioctl.
 */

#define	TRANSPARENT     ((unsigned int) -1)


/*
 * This structure defines the data format used in an M_COPYIN/M_COPYOUT
 * message (used to request data copying from user process space during
 * an ioctl ()). Note that the first three members of the "copyreq" structure
 * match the first three fields of the "iocblk" structure. It is intended
 * that drivers be able to use this property to simply use the intial message
 * block from M_IOCTL for all the transparent IOCTL messages.
 */

struct copyreq {
	int		cq_cmd;		/* ioctl command (from ioc_cmd) */
	cred_t	      *	cq_cr;		/* full credentials (from ioc_cr) */
	unsigned int	cq_id;		/* ioctl id (from ioc_id) */
	caddr_t		cq_addr;	/* user address to copy data to/from */
	unsigned int	cq_size;	/* number of bytes to copy */
	int		cq_flag;	/* flags, see below */
	mblk_t	      *	cq_private;	/* private state information */
	long		cq_filler [4];	/* reserved for future use */
};

/* For compatibility with old sources. */

#define	cq_uid		cq_cr->cr_uid
#define	cq_gid		cq_cr->cr_gid

/* cq_flag values */

#define	STRCANON	0x0001		/* b_cont data block contains canonical format specifier */
#define	RECOPY		0x0002		/* perform I_STR copyin again using canonical format specifier */


/*
 * This structure is used in the M_IOCDATA response to an M_COPYIN/M_COPYOUT,
 * and is laid out so that equivalent fields from the "struct copyreq"
 * structure remain at the same offset, ie we expect the stream head to
 * simply turn around our request, although it is not required to do so.
 */

struct copyresp {
	int		cp_cmd;		/* ioctl command (from cq_cmd) */
	cred_t	      *	cp_cr;		/* full credentials (from cq_cr) */
	unsigned int	cp_id;		/* ioctl id (from cq_id) */
	caddr_t		cp_rval;	/* request status: 0 success, non-zero failure */
	unsigned int	cp_pad1;	/* reserved */
	unsigned int	cp_pad2;	/* reserved */
	mblk_t	      * cp_private;	/* private state information */
	long		cp_filler [4];	/* reserved for future use */
};

/* for compatibility with old sources */

#define	cp_uid		cp_cr->cr_uid
#define	cp_gid		cp_cr->cr_gid


/*
 * LOCAL EXTENSIONS : The methods used in the multiplexing examples in the
 * STREAMS V.2 manual are slightly inefficient. The following general STREAMS
 * queue schedling routines were defined for multiplexing drivers and device
 * drivers (such as Ethernet drivers) that must consume data from several
 * queues.
 *
 * In the STREAMS code supplied, these routines are also used for the main
 * streams queue scheduling, so we define them here. A consequence of this is
 * that a given queue may not be queued for both STREAMS execution and
 * multiplexing at the same time.
 *
 * The basic idea is to use qschedule () and qunschedule () to add and remove
 * a queue from the scheduling list. Lower queues may then getq () from the
 * "s_head" of the scheduling list to consume the data. Since by default this
 * will simply attempt to drain each scheduled queue of data before moving to
 * the next, the muxrobin () call adjusts the head/tail pointers to effect
 * a round-robin of the queues that have data.
 */

struct strsched {
	queue_t		* s_head;	/* next queue to read data from */
	queue_t		* s_tail;	/* last queue to read data from */
};


/*
 * Message allocation priorities, used with allocb () and bufcall ()
 */

enum {
	BPRI_LO,
	BPRI_MED,
	BPRI_HI
};


/*
 * Flag values used with flushq () and flushband ().
 */

enum {
	FLUSHDATA,			/* flush M_DATA, M_PROTO, M_PCPROTO, M_DELAY */
	FLUSHALL			/* flush all messages from queue */
};


/*
 * queue field numbers to be used with strqget ()/strqset ().
 */

typedef enum qfields {
	QHIWAT		= 0,  		/* q_hiwat or qb_hiwat */
	QLOWAT  	= 1,		/* q_lowat or qb_lowat */
	QMAXPSZ		= 2,		/* q_maxpsz */
	QMINPSZ		= 3, 		/* q_minpsz */
	QCOUNT		= 4,		/* q_count or qb_count */
	QFIRST		= 5,		/* q_first or qb_first */
	QLAST	        = 6,		/* q_last or qb_last */
	QFLAG		= 7,		/* q_flag or qb_flag */
	QBAD		= 8
} qfields_t;


/*
 * External function definitions for the STREAMS library.
 *
 * Note that prototypes are also provided for those STREAMS functions that
 * are available as macros. This is since this header is a more appropriate
 * place to encode this knowledge than the DDI/DKI required header
 * <sys/ddi.h>
 *
 * The System V Release 4 Multi-Processor DDI/DKI defines some extra versions
 * of some common STREAMS library functions so that STREAMS driver code does
 * not reference the q->q_next member, since this may not be safe in a multi-
 * processor environment. These functions are included in this implementation
 * and are prototyped below to help device drivers anticipate the multi-
 * processor environment.
 */

__EXTERN_C_BEGIN__

int		adjmsg		__PROTO ((mblk_t * _mp, int _len));
mblk_t	      *	allocb		__PROTO ((int _size, unsigned int _pri));
queue_t	      *	backq		__PROTO ((queue_t * _q));
int		bcanput		__PROTO ((queue_t * _q, unsigned char _pri));
int		bcanputnext	__PROTO ((queue_t * _q, unsigned char _pri));
/* see <sys/bufcall.h> for a discussion of this declaration for bufcall () */
#define	__STDARG_BUFCALL__
toid_t		bufcall		__PROTO ((unsigned int _size, int _pri, ...));
int		canput		__PROTO ((queue_t * _q));
int		canputnext	__PROTO ((queue_t * _q));
mblk_t 	      *	copyb		__PROTO ((mblk_t * _bp));
mblk_t	      *	copymsg		__PROTO ((mblk_t * _mp));
int     	datamsg		__PROTO ((unsigned char _type));
mblk_t	      *	dupb		__PROTO ((mblk_t * _bp));
mblk_t	      *	dupmsg		__PROTO ((mblk_t * _mp));
void		enableok	__PROTO ((queue_t * _q));
mblk_t	      *	esballoc	__PROTO ((unsigned char * _base, int _size,
					  int _pri, frtn_t * _fr_rtn));
/* The following assumes __STDARG_BUFCALL__ as above */
toid_t		esbbcall	__PROTO ((int _pri, ...));
void		flushband	__PROTO ((queue_t * _q, unsigned char _pri,
					  int _flag));
void		flushq		__PROTO ((queue_t * _q, int _flag));
pl_t		freezestr	__PROTO ((queue_t * _q));
void		freeb		__PROTO ((mblk_t * _bp));
void		freemsg		__PROTO ((mblk_t * _mp));
int	     (*	getadmin	__PROTO ((unsigned short _mid)))
			__PROTO ((void));
unsigned short	getmid		__PROTO ((char * _name));
mblk_t	      *	getq		__PROTO ((queue_t * _q));
int		insq		__PROTO ((queue_t * _q, mblk_t * _emp,
					  mblk_t * _nmp));
void		linkb		__PROTO ((mblk_t * _mp, mblk_t * _bp));
int		msgdsize	__PROTO ((mblk_t * _mp));
mblk_t	      *	msgpullup	__PROTO ((mblk_t * _mp, int _len));
void		noenable	__PROTO ((queue_t * _q));
queue_t	      *	OTHERQ		__PROTO ((queue_t * _q));
int		pcmsg		__PROTO ((uchar_t _type));
int		pullupmsg	__PROTO ((mblk_t * _mp, int _len));
void		put		__PROTO ((queue_t * _q, mblk_t * _mp));
int		putbq		__PROTO ((queue_t * _q, mblk_t * _mp));
int		putctl		__PROTO ((queue_t * _q, int _type));
int		putctl1		__PROTO ((queue_t * _q, int _type,
					  int _param));
int		putnext		__PROTO ((queue_t * _q, mblk_t * _mp));
int		putnextctl	__PROTO ((queue_t * _q, int _type));
int		putnextctl1	__PROTO ((queue_t * _q, int _type,
					  int _param));
int		putq		__PROTO ((queue_t * _q, mblk_t * _mp));
void		qenable		__PROTO ((queue_t * _q));
void		qprocsoff	__PROTO ((queue_t * _rq));
void		qprocson	__PROTO ((queue_t * _rq));
void		qreply		__PROTO ((queue_t * _q, mblk_t * _mp));
int		qsize		__PROTO ((queue_t * _q));
queue_t	      *	RD		__PROTO ((queue_t * _q));
mblk_t	      *	rmvb		__PROTO ((mblk_t * _mp, mblk_t * _bp));
void		rmvq		__PROTO ((queue_t * _q, mblk_t * _mp));
int		strlog		__PROTO ((short _mid, short _sid, char _level,
					  unsigned short _flags, char * _fmt,
					  ...));
int		strqget		__PROTO ((queue_t * _q, qfields_t _what,
					  unsigned char _pri, long * _valp));
int		strqset		__PROTO ((queue_t * _q, qfields_t _what,
					  unsigned char _pri, long _val));
int		testb		__PROTO ((int _size, int _pri));
void		unbufcall	__PROTO ((toid_t _id));
void		unfreezestr	__PROTO ((queue_t * _q, pl_t _pl));
mblk_t	      *	unlinkb		__PROTO ((mblk_t * _mp));
queue_t	      *	WR		__PROTO ((queue_t * _q));

__EXTERN_C_END__


/*
 * Possible values for the stream open flag "sflag" in a module or driver
 * open (0 implies a normal driver open)
 */

enum {
	MODOPEN		= 1,		/* normal module open */
	CLONEOPEN			/* clone device open */
};


/*
 * Special value for the two-byte form of the M_ERROR message.
 */

enum {
	NOERROR		= 0xFF
};


/*
 * Functions that are normally implemented as macros (although the function
 * versions which are prototyped above are available by including the
 * <sys/ddi.h> header.
 */
/*
 * NB: Borland C++ 3.1 in 'C' mode insists that if a pointer is derived from
 * a ternary where the arms are calculated from lvalues then dereferencing
 * that pointer does not yield a valid lvalue, e.g.
 *	WR (q)->q_ptr = NULL;
 * it thinks is invalid, which is untrue. To fix this, the pointer value from
 * the ternary seems to need massaging to turn it into a proper rvalue... the
 * easiest way to do this is add 0 to the *result* of the ternary. This is
 * optimized away by basic constant-folding in even simple compilers.
 */

#define	__HACK(exp)	((exp) + 0)

#define	datamsg(type)	((unsigned char) ((type) & (M_PRI - 1)) <= M_DELAY)
/* There is no multiprocessor macro implementation of enableok () */
/* There is no multiprocessor macro implementation of noenable () */
#define	OTHERQ(q)     __HACK (((q)->q_flag & QREADR) != 0 ? (q) + 1 : (q) - 1)
#define	pcmsg(type)	((type) >= QPCTL)
#define	RD(q)	      __HACK (((q)->q_flag & QREADR) != 0 ? (q) : (q) - 1)
#define	WR(q)	      __HACK (((q)->q_flag & QREADR) != 0 ? (q) + 1 : (q))

#endif /* ! defined (__SYS_STREAM_H__) */
