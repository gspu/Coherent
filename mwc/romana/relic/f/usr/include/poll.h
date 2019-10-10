#ifndef	__POLL_H__
#define	__POLL_H__

#include <sys/poll.h>

/*
 * Definitions for use with the poll (2) system call.
 */

struct pollfd {
	int		fd;		/* file descriptor to check */
	short		events;		/* events to check for */
	short		revents;	/* events detected */
};

#endif	/* ! defined (__POLL_H__) */
