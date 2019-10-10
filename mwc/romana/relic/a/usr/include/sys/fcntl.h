/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef	FCNTL_H
#define	FCNTL_H
/*
 * Flag values accessible to open(2) and fcntl(2)
 * (The first three can only be set by open)
 */
#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		2
#define	O_NDELAY	04	/* Non-blocking I/O */
#define	O_APPEND	010	/* append (writes guaranteed at the end) */

/*
 * Flag values accessible only to open(2)
 */
#define	O_CREAT		00400	/* open with file create (third open arg) */
#define	O_TRUNC		01000	/* open with truncation */
#define	O_EXCL		02000	/* exclusive open */

/*
 * fcntl(2) requests
 */
#define	F_DUPFD		0	/* duplicate fildes */
#define	F_GETFD		1	/* get fildes flags */
#define	F_SETFD		2	/* set fildes flags */
#define	F_GETFL		3	/* get file flags */
#define	F_SETFL		4	/* set file flags */

#endif
