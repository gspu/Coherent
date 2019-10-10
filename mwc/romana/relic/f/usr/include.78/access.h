/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Modes for the access
 * system call.
 */

#ifndef __ACCESS_H__
#define	__ACCESS_H__

#define	AREAD	04		/* Test for read */
#define	AWRITE	02		/* Test for write */
#define	AEXEC	01		/* Test for execute */
#define	AEXISTS	00		/* Test for existence */
#define	AAPPND	AWRITE		/* Test for append */

/* Dummy directory modes */
#define	ALIST	AREAD		/* List directory */
#define	ADEL	AWRITE		/* Delete directory entry */
#define	ASRCH	AEXEC		/* Search directory */
#define	ACREAT	AAPPND		/* Create directory entry */

#endif
