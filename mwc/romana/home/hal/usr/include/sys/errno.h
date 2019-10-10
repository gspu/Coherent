/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_ERRNO_H__
#define	__SYS_ERRNO_H__

#include <common/feature.h>

/*
 * Error numbers for DDI/DKI drivers, as described in errnos (D5DK) in the
 * System V, Release 4 Multiprocessor "Device Driver Interface/Driver-Kernel
 * Interface Reference Manual".
 *
 * Actual numeric values of error constants are derived from the System V ABI
 * "Intel 386 Processor Supplement".
 *
 * If these definitions are to be imported by <errno.h>, they *must* be macros
 * rather than enumerations or other kinds of constant, as required by the ISO
 * C standard (ISO/IEC 9989-1990).
 *
 * User-level programs should not use this file directly.
 */

#define	EPERM		1	/* Permission denied. */
#define	ENOENT		2	/* No such file or directory. */
#define	ESRCH		3	/* No such process. */
#define	EINTR		4	/* Interrupted system call. */
#define	EIO		5	/* I/O error. */
#define	ENXIO		6	/* No such device or address. */
#define	E2BIG		7	/* Argument list too long. */
#define	ENOEXEC		8	/* Exec format error. */
#define	EBADF		9	/* Bad file number. */
#define	ECHILD		10	/* No child processes. */
#define	EAGAIN		11	/* No more processes. */
#define	ENOMEM		12	/* Not enough space. */
#define	EACCES		13	/* Permission denied. */
#define	EFAULT		14	/* Bad address. */
#define	ENOTBLK		15	/* Block device required. */
#define	EBUSY		16	/* Device busy. */
#define EEXIST		17	/* File exists. */
#define	EXDEV		18	/* Cross-device link. */
#define	ENODEV		19	/* No such device. */
#define ENOTDIR		20	/* Not a directory. */
#define	EISDIR		21	/* Is a directory. */
#define	EINVAL		22	/* Invalid argument. */
#define	ENFILE		23	/* File-table overflow. */
#define	EMFILE		24	/* Too many open files. */
#define	ENOTTY		25	/* Not a teletypewriter (tty) */
#define	ETXTBSY		26	/* Text file busy. */
#define	EFBIG		27	/* File too large. */
#define	ENOSPC		28	/* No space left on device. */
#define	ESPIPE		29	/* Tried to seek on a pipe */
#define	EROFS		30	/* Read-only file system. */
#define	EMLINK		31	/* Too many links. */
#define	EPIPE		32	/* Broken pipe. */
#define	EDOM		33	/* Math argument out of domain of function. */
#define	ERANGE		34	/* Math result not representable. */
#define	ENOMSG		35	/* No message of desired type. */
#define	EIDRM		36	/* Identifier removed. */
#define	EDEADLK		45	/* Deadlock condition. */
#define	ENOLCK		46	/* No record locks available. */
#define	ENOSTR		60	/* Device not a stream. */
#define	ENODATA		61	/* No data available. */
#define	ETIME		62	/* Timer expired. */
#define	ENOSR		63	/* Out of STREAMS resources. */
#define	ENOPKG		65	/* Package not installed. */
#define	EPROTO		71	/* Protocol error. */
#define EBADMSG		77	/* Not a data message. */
#define	ENAMETOOLONG	78	/* File name too long. */
#define	EOVERFLOW	79	/* Value too large for defined data type. */
#define	ENOTUNIQ	80	/* Name not unique on network. */
#define	EBADFD		81	/* File descriptor in bad state. */
#define	EREMCHG		82	/* Remote address changed. */
#define	ELIBACC		83	/* Cannot access a needed shared library. */
#define	ELIBBAD		84	/* Accessing a corrupted shared library. */
#define	ELIBSCN		85	/* ".lib" section in "a.out" corrupted. */
#define	ELIBMAX		86	/* Attempting to link in more shared libraries
				   than system limit. */
#define	ELIBEXEC	87	/* Cannot exec () a shared library directly. */
#define	EILSEQ		88	/* Illegal byte sequence. */
#define	ENOSYS		89	/* Operation not applicable. */
#define	ELOOP		90	/* Number of symbolic links encountered during
				   path name traversal exceeds MAXSYMLINKS. */
#define	EUSERS		94	/* Too many users. */
#define	ENOTSOCK	95	/* Socket operation on non-socket. */
#define	EDESTADDRREQ	96	/* Destination address required. */
#define	EMSGSIZE	97	/* Message too long. */
#define	EPROTOTYPE	98	/* Protocol wrong type for socket. */
#define	ENOPROTOOPT	99	/* Protocol not available. */
#define	EPROTONOSUPPORT	120	/* Protocol not supported. */
#define	ESOCKTNOSUPPORT	121	/* Socket type not supported. */
#define	EOPNOTSUPP	122	/* Operation not supported on transport
				   endpoint. */
#define	EPFNOSUPPORT	123	/* Protocol family not supported. */
#define	EAFNOSUPPORT	124	/* Address family not supported by protocol
				   family. */
#define	EADDRINUSE	125	/* Address already in use. */
#define	EADDRNOTAVAIL	126	/* Cannot assign requested address. */
#define	ENETDOWN	127	/* Network is down. */
#define	ENETUNREACH	128	/* Network is unreachable. */
#define	ENETRESET	129	/* Network dropped connection because of
				   reset. */
#define	ECONNABORTED	130	/* Software-caused connection abort. */
#define	ECONNRESET	131	/* Connection reset by peer. */
#define	ENOBUFS		132	/* No buffer space available. */
#define	EISCONN		133	/* Transport endpoint is already connected. */
#define	ENOTCONN	134	/* Transport endpoint is not connected. */
#define	ESHUTDOWN	143	/* Cannot send after transport endpoint
				   shutdown. */
#define	ETIMEDOUT	145	/* Connection timed out. */
#define	ECONNREFUSED	146	/* Connection refused. */
#define	EHOSTDOWN	147	/* Host is down. */
#define	EHOSTUNREACH	148	/* No route to host. */
#define	EALREADY	149	/* Operation already in progress. */
#define	EINPROGRESS	150	/* Operation now in progress. */
#define	ESTALE		151	/* Stale NFS file handle. */

/*
 * Alias for compatibility with socket-based network sources; not for use in
 * new code or driver code.
 */

#define	EWOULDBLOCK	EAGAIN

#if	_KERNEL || _DDI_DKI_IMPL

/*
 * Driver code can use the following instead of touching the U area. Note that
 * these functions are not part of the DDI/DKI, but the DDI/DKI implementation
 * wants them.
 */

#include <common/ccompat.h>

__EXTERN_C_BEGIN__

int		get_user_error	__PROTO ((void));
void		set_user_error	__PROTO ((int));

__EXTERN_C_END__

#endif

#endif	/* ! defined (__SYS_ERRNO_H__) */
