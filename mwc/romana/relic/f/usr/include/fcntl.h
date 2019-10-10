/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/fcntl.h
 * Cf. Intel iBCS2, pp. 6-35, 6-36.
 */

#ifndef	__FCNTL_H__
#define	__FCNTL_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/_uid.h>

/*
 * Flag values accessible to open () and fcntl ()
 * The first three can only be set by open ().
 */

#define	O_RDONLY	0x00
#define	O_WRONLY	0x01
#define	O_RDWR		0x02
#define	O_NDELAY	0x04	/* non-blocking i/o			*/
#define	O_APPEND	0x08	/* append (writes guaranteed at the end	*/

#if	_I386

#define O_SYNC		0x10	/* sync on every write			*/
#define	O_NONBLOCK	0x80	/* non-blocking	i/o			*/

/* Flag values accessible only to 3 argument open (). */

#define	O_CREAT		0x100	/* open with file create (third open arg) */
#define	O_TRUNC		0x200	/* open with truncation			*/
#define	O_EXCL		0x400	/* exclusive open			*/
#define O_NOCTTY	0x800	/* don't assign a controlling tty	*/

#endif /* _I386 */


/* fcntl () requests. */

#define	F_DUPFD		0	/* duplicate file desriptor		*/
#define	F_GETFD		1	/* get file descriptor flags		*/
#define	F_SETFD		2	/* set file descriptor flags		*/
#define	F_GETFL		3	/* get file flags			*/
#define	F_SETFL		4	/* set file flags			*/
#define	F_GETLK		5	/* return blocking lock			*/
#define	F_SETLK		6	/* set or remove lock, return if blocked */
#define	F_SETLKW	7	/* set or remove lock, sleep if blocked	*/


/* Locking command/return structure. */

struct flock {
	short		l_type;		/* F_RDLCK, F_WRLCK, or F_UNLCK	*/
	short		l_whence;	/* SEEK_SET, SEEK_CUR, SEEK_END	*/
	long		l_start;	/* location			*/
	long		l_len;		/* 0 is through EOF		*/
	short		__NON_POSIX (l_sysid);
					/* system id of lock (GETLK) ?	*/
	short		l_pid;		/* process id of owner (GETLK)	*/
};

#if	! _POSIX_SOURCE

typedef	struct flock	flock_t;

#endif


/* lock () command types. */

#define	F_RDLCK	1		/* read lock; more than 1 allowed	*/
#define	F_WRLCK	2		/* write lock; blocks any lock		*/
#define	F_UNLCK 3		/* unlock				*/


/* Close on exec bit */

#define	FD_CLOEXEC	1	/* close fd on exec			*/


__EXTERN_C_BEGIN__

int		creat		__PROTO ((__CONST__ char * _path,
					  __mode_t _mode));
int		fcntl		__PROTO ((int _fildes, int _cmd, ...));
int		open		__PROTO ((__CONST__ char * _path, int _oflag,
					  ...));

__EXTERN_C_END__

#endif	/* ! defined (__FCNTL_H__) */
