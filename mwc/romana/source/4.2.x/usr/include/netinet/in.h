/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */
#ifndef	__NETINET_IN_H__
#define	__NETINET_IN_H__

#include <common/ccompat.h>
#include <common/_limits.h>
#include <common/_canon.h>

/*
 * Deal with conversions to and from network byte order for various types.
 *
 * We built these on top of the reversible conversion utilities in the
 * general canonicalization system.
 */

#define	htonl(host)	__CANONICALIZE (host, R, __NET, __LONG)
#define	htons(host)	__CANONICALIZE (host, R, __NET, __SHORT)
#define	ntohl(net)	__CANONICALIZE (net, R, __NET, __LONG)
#define	ntohs(net)	__CANONICALIZE (net, R, __NET, __SHORT)


/*
 * Internet protocol numbers (as included in the protocol field of IP
 * datagrams).
 */

#define	IPPROTO_IP		0
#define	IPPROTO_ICMP		1
#define	IPPROTO_TCP		6
#define	IPPROTO_UDP		17

#define	IPPROTO_MAX		255


/*
 * TCP and UDP multiplex connections based on "port numbers"; port numbers
 * less than IPPORT_RESERVED are reserved for standard servers, and binding
 * such a port address requires special privelege. Port numbers greater than
 * IPPORT_USERRESERVED are set aside for user-defined services.
 */

#define	IPPORT_RESERVED		1024
#define	IPPORT_USERRESERVED	5000


/*
 * Internet address definitions; the structure version is forced upon us by
 * the socket interface.
 */

typedef	unsigned short	__inport_t;
#define	__INPORT_T	__SHORT
typedef	unsigned long	__inaddr_t;
#define	__INADDR_T	__LONG

struct in_addr {
	__inaddr_t	s_addr;
};


/*
 * Internet addresses come in forms with 24, 16 and 8 hosts per network,
 * repectively called class A, B, and C addresses. The address classes are
 * identified by codes in the top bits of the address.
 */

#define	IN_CLASSA(i)		(((__inaddr_t) (i) & 0x80000000UL) == 0)
#define	IN_CLASSA_MAX		128UL
#define	IN_CLASSA_NSHIFT	24UL
#define	IN_CLASSA_NET		(__ULONG_MAX << IN_CLASSA_NSHIFT)
#define	IN_CLASSA_HOST		((1UL << IN_CLASSA_NSHIFT) - 1UL)

#define	IN_CLASSB(i)		(((__inaddr_t) (i) & 0xC0000000UL) == \
				 0x80000000UL)
#define	IN_CLASSB_MAX		16384UL
#define	IN_CLASSB_NSHIFT	16UL
#define	IN_CLASSB_NET		(__ULONG_MAX << IN_CLASSB_NSHIFT)
#define	IN_CLASSB_HOST		((1UL << IN_CLASSB_NSHIFT) - 1UL)

#define	IN_CLASSC(i)		(((__inaddr_t) (i) & 0xE0000000UL) == \
				 0xC0000000UL)
#define	IN_CLASSC_NSHIFT	8UL
#define	IN_CLASSC_NET		(__ULONG_MAX << IN_CLASSC_NSHIFT)
#define	IN_CLASSC_HOST		((1UL << IN_CLASSC_NSHIFT) - 1UL)

#define	IN_CLASSD(i)		(((__inaddr_t) (i) & 0xF0000000UL) == \
				 0xE0000000UL)

/*
 * Special cases.
 */

#define	INADDR_ANY		((__inaddr_t) 0)
#define	INADDR_BROADCAST	((__inaddr_t) __ULONG_MAX)


/*
 * Internet family socket address.
 */

struct sockaddr_in {
	short		sin_family;	/* AF_INET */
	__inport_t	sin_port;
	struct in_addr	sin_addr;
	char		sin_zero [8];	/* more socket brain damage */
};

#endif	/* ! defined (__NETINET_IN_H__) */

