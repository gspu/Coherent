/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1983, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	__POLL_H__
#define	__POLL_H__
/*
 * This is a temporary file, and will NOT be binary compatible with System V.
 */

/*
 * Polling structure.
 */
struct pollfd {
	int	fd;		/* file descriptor	*/
	short	events;		/* requested events	*/
	short	revents;	/* returned events	*/
};

/*
 * Stream oriented events.
 */
#define	POLLIN	 000001		/* input data is available		*/
#define	POLLPRI	 000002		/* priority message is available	*/
#define	POLLOUT	 000004		/* output can be sent			*/
#define	POLLERR	 000010		/* a fatal error has occurred		*/
#define	POLLHUP	 000020		/* a hangup condition exists		*/
#define	POLLNVAL 000040		/* fd does not access an open stream	*/

/*
 * Each pollable event in the system has an associated event queue.
 * An polled event will be
 *	on a singularly-linked list throuch cprocp->p_polls, and
 *	on a circularly-linked list through an event queue on the device.
 */
typedef
struct event {
	struct event *	e_pnext;	/* next polled event on proc	*/
	struct event *	e_dnext;	/* next polled event on device	*/
	struct event *	e_dlast;	/* prev polled event on device	*/
	struct proc  *	e_procp;	/* pointer to polling process	*/
} event_t;

#endif
