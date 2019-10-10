/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__POLL_H__
#define	__POLL_H__

#include <common/ccompat.h>
#include <common/__size.h>
#include <sys/poll.h>

/*
 * Definitions for use with the poll (2) system call.
 */

struct pollfd {
	int		fd;		/* file descriptor to check */
	short		events;		/* events to check for */
	short		revents;	/* events detected */
};

__EXTERN_C_BEGIN__

int		poll		__PROTO ((struct pollfd * _fds,
					  __size_t _nfds, int _timeout));

__EXTERN_C_END__

#endif	/* ! defined (__POLL_H__) */
