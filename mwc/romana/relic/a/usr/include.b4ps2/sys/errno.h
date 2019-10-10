/*
 * Error codes.
 *
 * /usr/include/sys/errno.h
 */

#ifndef	ERRNO_H
#define	ERRNO_H	ERRNO_H

#define	EPERM	1		/* Not super user */
#define	ENOENT	2		/* No such file or directory */
#define	ESRCH	3		/* Process not found */
#define	EINTR	4		/* Interrupted system call */
#define	EIO	5		/* I/O error */
#define	ENXIO	6		/* No such device or address */
#define	E2BIG	7		/* Argument list too long */
#define	ENOEXEC	8		/* Sys exec format error */
#define	EBADF	9		/* Bad file number */
#define	ECHILD	10		/* No children (wait) */
#define	EAGAIN	11		/* No more processes are available */
#define	ENOMEM	12		/* Cannot map process into memory */
#define	EACCES	13		/* Permission denied */
#define	EFAULT	14		/* Bad system call argument address */
#define	ENOTBLK	15		/* Block device required (mount) */
#define	EBUSY	16		/* Device busy (mount) */
#define EEXIST	17		/* File already exists */
#define	EXDEV	18		/* Cross device link */
#define	ENODEV	19		/* No such device */
#define ENOTDIR	20		/* Not a directory */
#define	EISDIR	21		/* Is a directory */
#define	EINVAL	22		/* Invalid argument */
#define	ENFILE	23		/* File table overflow */
#define	EMFILE	24		/* Too many open files for this process */
#define	ENOTTY	25		/* Not a terminal */
#define	ETXTBSY	26		/* Text file busy */
#define	EFBIG	27		/* File too big to map */
#define	ENOSPC	28		/* No space left on device */
#define	ESPIPE	29		/* Illegal seek on a pipe */
#define	EROFS	30		/* Read only filesystem */
#define	EMLINK	31		/* Too many links */
#define	EPIPE	32		/* Broken pipe */
#define	EDOM	33		/* Domain error */
#define	ERANGE	34		/* Result too large */
#define EDATTN	199		/* Device needs attention do not use !! */

#ifndef KERNEL
/*
 * Globals for user programs.
 */
extern	int	errno;
extern	int	sys_nerr;
extern	char	*sys_errlist[];

#endif

#endif
