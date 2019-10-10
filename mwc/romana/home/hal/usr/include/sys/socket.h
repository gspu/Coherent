/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_SOCKET_H__
#define	__SYS_SOCKET_H__

/*
 * As of this writing, a kernel-level socket implementation is not available
 * for COHERENT.  This header defines constants, structures and prototypes
 * for a full socket emulation, nonetheless.
 *
 * Support is also contained within this file for a user-level emulation of a
 * simple subset of sockets over named pipes.
 */

#include <common/ccompat.h>
#include <common/__caddr.h>
#include <common/__iovec.h>
#include <common/_imode.h>
#include <common/tpisrv.h>

/*
 * Socket types. Note that other types exist historically, but these socket
 * types are not available in emulation.
 *
 * Note that the values for these constants under STREAMS-based emlation
 * correspond to the equivalent TPI service class name as known via the SVR4
 * "netconfig" interface.
 */

#define	SOCK_STREAM	__NC_TPI_COTS	/* connection-oriented stream */
#define	SOCK_DGRAM	__NC_TPI_CLTS	/* datagram-oriented stream */


/*
 * Socket options. The form of the option codes is historical practice and
 * thus required for binary compatibility; otherwise we would use normal
 * programming practice and hide the implementation details.
 *
 * Many of these options are only meaningful for the original BSD
 * implementation.
 */

#define	SO_DEBUG	0x0001
#define	SO_ACCEPTCONN	0x0002
#define	SO_REUSEADDR	0x0004
#define	SO_KEEPALIVE	0x0008
#define	SO_DONTROUTE	0x0010
#define	SO_BROADCAST	0x0020
#define	SO_USELOOPBACK	0x0040
#define	SO_LINGER	0x0080
#define	SO_OOBINLINE	0x0100

#define	SO_SNDBUF	0x1001
#define	SO_RCVBUF	0x1002
#define	SO_SNDLOWAT	0x1003
#define	SO_RCVLOWAT	0x1004
#define	SO_SNDTIMEO	0x1005
#define	SO_RCVTIMEO	0x1006
#define	SO_ERROR	0x1007
#define	SO_TYPE		0x1008


/*
 * The various options above can be directed at either a protocol or at a
 * socket. Use this as the "level" code in getsockopt ()/setsockopt () to
 * deal with the socket.
 */

#define	SOL_SOCKET	0xFFFF


/*
 * Address families.
 *
 * Many socket address families have been defined; our emulation is only
 * targeted at the most basic few.
 */

#define	AF_UNSPEC	0
#define	AF_UNIX		1
#define	AF_INET		2


/*
 * Protocol families.
 */

#define	PF_UNSPEC	AF_UNSPEC
#define	PF_UNIX		AF_UNIX
#define	PF_INET		AF_INET


/*
 * BSD systems define a symbol SOMAXCONN to describe a kernel internal limit
 * on the number of outstanding connection indications. TLI/TPI systems have
 * no fixed limit. We define SOMAXCONN anyway since any software that uses
 * sockets will have assumed a hard limit exists...
 */

#define	SOMAXCONN	5


/*
 * Type for use as a generic socket address holder. Ideally, this should be
 * left as an incomplete type since any attempt to instantiate such an item
 * is almost certainly wrong. However, historical usage is that this contains
 * an address family descriptor and then a character array (with size 14, but
 * the actual amount of address data is unlimited).
 */

typedef	struct sockaddr	__sockaddr_t;

struct sockaddr {
	unsigned short	sa_family;	/* address family */
};


/*
 * Message descriptor structure for recvmsg ().
 */

typedef struct msghdr	__msghdr_t;

struct msghdr {
	__caddr_t	msg_name;	/* optional address */
	int		msg_namelen;	/* size of address */
	__iovec_t     *	msg_iov;	/* fragment vector */
	int		msg_iovlen;	/* number of fragments */
	__caddr_t	msg_accrights;	/* optional access rights */
	int		msg_accrightslen;
};


/*
 * Flags for passing to the recv... () and send... () routines.
 */

#define	MSG_OOB		1
#define	MSG_PEEK	2
#define	MSG_DONTROUTE	4


__EXTERN_C_BEGIN__

int		accept		__PROTO ((int _s, __sockaddr_t * _addr,
					  int * _addrlen));
int		bind		__PROTO ((int _s, __sockaddr_t * _addr,
					  int _addrlen));
int		connect		__PROTO ((int _s, __sockaddr_t * _name,
					  int _namelen));
int		getsockname	__PROTO ((int _s, __sockaddr_t * _name,
					  int * _namelen));
int		getsockopt	__PROTO ((int _s, int _level, int _optname,
					  char * _optval, int * _optlen));
int		listen		__PROTO ((int _s, int _backlog));
int		recv		__PROTO ((int _s, char * _buf, int _len,
					  int _flags));
int		recvfrom	__PROTO ((int _s, char * _buf, int _len,
					  int _flags, __sockaddr_t * _from,
					  int _fromlen));
int		recvmsg		__PROTO ((int _s, __msghdr_t * _msg,
					  int _flags));
int		send		__PROTO ((int _s, __CONST__ char * _buf,
					  int _len, int _flags));
int		sendto		__PROTO ((int _s, __CONST__ char * _buf,
					  int _len, int _flags,
					  __CONST__ __sockaddr_t * _to,
					  int _tolen));
int		sendmsg		__PROTO ((int _s, __msghdr_t * _msg,
					  int _flags));
int		setsockopt	__PROTO ((int _s, int _level, int _optname,
					  char * _optval, int _optlen));
int		shutdown	__PROTO ((int _s, int _how));
int		socket		__PROTO ((int _domain, int _type,
					  int _protocol));
int		socketpair	__PROTO ((int _domain, int _type,
					  int _protovol, int _sv [2]));

#if	_SOCKET_EMULATION

int		soclose		__PROTO ((int _s));
int		soioctl		__PROTO ((int _s, int _cmd, ...));
int		soread		__PROTO ((int _s, __VOID__ * _buf, int _len));
int		sowrite		__PROTO ((int _s, __CONST__ __VOID__ * _buf,
					  int _len));
#endif

/*
 * Yet another sleazy BSD-ism, normally a completely unhygenic macro.
 */

int		SOCKADDRLEN	__PROTO ((__sockaddr_t * _addr));

__EXTERN_C_END__


#if	_SOCKET_EMULATION

/*
 * The socket emulation deals with users of SOCK_STREAM sockets by taking
 * control over calls such as read () and write ().
 */

#define	FIOSNBIO	(('f' << 8) | 1)
#define	FIONREAD	(('f' << 8) | 2)

#if	! _SOCKET_LIBRARY_BUILD

/*
 * Redirect I/O calls to emulation functions; note that we use function-like
 * macros to avoid causing conflicts with prototype declarations. This can't
 * be done for ioctl () because it has variable arguments, but then there also
 * isn't any standard place where it is prototyped...
 */

#define	read(f,b,n)	soread (f, b, n)
#define	write(f,b,n)	sowrite (f, b, n)
#define	close(f)	soclose (f)
#define	ioctl		soioctl

#endif

#endif	/* _SOCKET_EMULATION */

#endif	/* ! defined (__SYS_SOCKET_H__) */
