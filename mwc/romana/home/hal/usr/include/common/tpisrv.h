/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON_TPISRV_H__
#define	__COMMON_TPISRV_H__

/*
 * This internal header defines some TPI-related constants that are used by
 * various network-related subsystems.
 */

/*
 * Names for TPI service classes for use with the netconfig and socket
 * interfaces.
 */

enum {
	__NC_TPI_CLTS	= 1,	/* connectionless service class */
	__NC_TPI_COTS,		/* connection-oriented service class */
	__NC_TPI_COTS_ORD,	/* as COTS with orderly connection release */
	__NC_TPI_RAW
};


/*
 * For contrast, the TLI names for the service classes.
 */

enum {
	__T_COTS	= 1,
	__T_COTS_ORD,
	__T_CLTS
};

#endif	/* ! defined (__COMMON_TPISRV_H__) */

