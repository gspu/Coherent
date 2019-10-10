#ifndef	__SYS_POLL_H__
#define	__SYS_POLL_H__

/*
 * This header defines prototypes for functions that manipulate the structures
 * used to support device polling in the kernel. definitions from this file
 * are imported by <poll.h>
 */
/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__PROTO ()
 */

#if	_DDI_DKI

#include <common/feature.h>
#include <common/ccompat.h>


/*
 * The contents of the "pollhead" structure used in System V polling are not
 * specified here, or indeed anywhere else outside of the file which
 * implements the routines whose prototypes are given below.
 *
 * The "pollhead" structure is intended to be totally opaque. Instances must
 * be allocated and freed with the functions given, and no access to the
 * internal structure of this item is permissible.
 *
 * It is worthwhile for implementations to note several facts about System V
 * polling, however; consider the extra difficulties involved in preventing
 * race conditions in polling when the system is multithreaded. The chpoll ()
 * entry point is required to never sleep, or call a function such that it
 * might sleep (which means that calling "phalloc (KM_SLEEP)" is not permitted
 * in that context), implying that an implementation may hold a basic lock to
 * limit race conditions. Essentially, implementations should spend as little
 * time as possible in the chpoll () entry point.
 */

__EXTERN_C_BEGIN__

struct pollhead	* phalloc	__PROTO ((int _flag));
void		phfree		__PROTO ((struct pollhead * _php));
void		pollwakeup	__PROTO ((struct pollhead * _php,
					  short _event));

__EXTERN_C_END__

#endif	/* _DDI_DKI */

#if	_OLD_COH_POLLING || (__COHERENT__ && __KERNEL__)

#include <common/_poll.h>


/*
 * The following structure is defined by the implementation code in the
 * Coherent kernel, so we define it so that this header can replace that one.
 *
 * We keep the old structure name, and also preserve the name of one of the
 * structure members. That's about it, though. The *only* field that old-style
 * Coherent drivers were allowed documented access to is e_procp, and then
 * only as a test to see whether pollwake () needed to be called at all, which
 * was unnecessary.
 */

typedef struct event	event_t;

struct event {
#if	! _OLD_COH_POLLING
	event_t	      *	e_pnext;
	event_t	      *	e_dnext;
	event_t	      *	e_dlast;
	struct proc   *	e_procp;
#else
	poll_t		e_node;		/* for circular list management */
	__VOID__      *	e_reserved;	/* unused, for binary compatibility */
	__VOID__      *	e_procp;	/* drivers can test this for != 0 */
#endif
};


__EXTERN_C_BEGIN__

void	pollopen		__PROTO ((event_t * _elistp));
void	pollwake		__PROTO ((event_t * _elistp));

__EXTERN_C_END__

/*
 * Declare the existence of the 'event_t' hack to code that wants to work
 * either with or without it.
 */

#ifndef	_OLD_COH_POLLING
#define	_OLD_COH_POLLING	1
#endif

#endif	/* _OLD_COH_POLLING || (__COHERENT__ && __KERNEL__) */


/*
 * Here we put an enumeration which defines the common elements of the event
 * masks used in <poll.h> for poll (), and <stropts.h> for the I_SETSIG
 * ioctl (). Mostly, the two masks match; the bits where they differ do not
 * conflict in key routines because the events they match in <stropts.h>
 * cannot be polled for.
 *
 * The exact numeric values used here can be found in the System V ABI.
 */

enum {
	__POLL_INPUT	= 0x0001,	/* any message other than M_PCPROTO */
	__POLL_HIPRI	= 0x0002,	/* a high priority message has arrived */
	__POLL_OUTPUT	= 0x0004,	/* the write queue is no longer full */
	__POLL_008	= 0x0008,	/* a SIGPOLL signal message has arrived */
					/* -or- fatal error on stream */
	__POLL_010	= 0x0010,	/* an M_ERROR message has arrived */
					/* -or- stream is hung up */
	__POLL_020	= 0x0020,	/* an M_HANGUP message has arrived */
					/* -or- file descriptor invalid */
	__POLL_RDNORM	= 0x0040,	/* a non-priority message has arrived */
	__POLL_WRNORM	= __POLL_OUTPUT,/* alias for S_OUTPUT */
	__POLL_RDBAND	= 0x0080,	/* a priority band message has arrived */
	__POLL_WRBAND	= 0x0100,	/* a priority band is writeable */
	__POLL_BANDURG	= 0x0200,	/* request SIGURG rather than SIGPOLL */
	__POLL_MASK	= 0x02FF	/* MASK FOR I_SETSIG */
};

enum {
	/*
	 * Definitions for events that may be polled for.
	 */

	POLLIN	       	= __POLL_INPUT,	/* ready message, not M_PCPROTO */
	POLLPRI		= __POLL_HIPRI,	/* ready message type M_PCPROTO */
	POLLOUT		= __POLL_OUTPUT,/* normal band is writeable */
	POLLRDNORM	= __POLL_RDNORM,/* normal message is ready */
	POLLWRNORM	= __POLL_OUTPUT,/* same as POLLOUT */
	POLLRDBAND	= __POLL_RDBAND,/* ready message in band > 0 */
	POLLWRBAND	= __POLL_WRBAND,/* band > 0 downstream is writeable */

	/*
	 * Definitions for additional events that may not be polled for but which may
	 * be returned anyway.
	 */

	POLLERR		= __POLL_008,	/* fatal error on stream */
	POLLHUP		= __POLL_010,	/* stream is hung up */
	POLLNVAL	= __POLL_020	/* fd not associated with a stream */

	/* value 0x0200 is reserved for BANDURG flag in <stropts.h> */
};

#endif	/* ! defined (__SYS_POLL_H__) */
