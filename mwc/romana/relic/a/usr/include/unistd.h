/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/unistd.h
 * Cf. Intel iBSC2, pp. 6-82, 6-83.
 */

#ifndef	_UNISTD_H
#define	_UNISTD_H

/* Access modes. */
#define	F_OK	0
#define	X_OK	1
#define	W_OK	2
#define	R_OK	4

/* lockf() commands. */
#define	F_ULOCK	0		/* unlock region			*/
#define	F_LOCK	1		/* sleep until available and lock	*/
#define	F_TLOCK	2		/* lock if available, EAGAIN if not	*/
#define	F_TEST	3		/* return 0 if available, EAGAIN if not	*/

/* Seek positions. */
#define	SEEK_SET	0	/* from beginning			*/
#define	SEEK_CUR	1	/* from current position		*/
#define	SEEK_END	2	/* from end				*/

/* File descriptors for standard FILEs. */
#define	STDIN_FILENO	0
#define	STDOUT_FILENO	1
#define	STDERR_FILENO	2

#endif

/* end of unistd.h */
