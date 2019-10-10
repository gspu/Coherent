/* (-lgl
 * 	COHERENT Version 4.0.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/errno.h
 * Error codes.
 */

#ifndef	__ERRNO_H__
#define	__ERRNO_H__

#include <common/feature.h>

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
#if	_I386
#define	ENOMSG	35		/* No message of desired type */
#define	EIDRM	36		/* Identifier removed */
#else
#define	EKSPACE	35		/* Out of kernel space */
#define	ENOLOAD	36		/* Driver not loaded */
#endif
#define	EBADFMT	37		/* Bad format */
#if	_I386
#define EDATTN	199		/* Device needs attention do not use !! */
#else
#define EDATTN	38		/* Device needs attention */
#endif
#define	EDBUSY	39		/* Device busy */
#define	EDEADLK	40		/* Deadlock */
#define	ENOLCK	41		/* No lock available */


extern	int		errno;

#if	! _STDC_SOURCE && ! _POSIX_SOURCE

extern	int		sys_nerr;
extern	char	      *	sys_errlist [];

#endif

#endif	/* ! defined (__ERRNO_H__) */
