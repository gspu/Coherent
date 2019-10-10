/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__TM_H__
#define	__COMMON__TM_H__

/*
 * This internal header file defines the ANSI standard structure "tm". This
 * structure is defined as being exported to the user in various headers,
 * according to documents such as the System V ABI.
 */

struct tm {
	int	tm_sec;
	int	tm_min;
	int	tm_hour;
	int	tm_mday;
	int	tm_mon;
	int	tm_year;
	int	tm_wday;
	int	tm_yday;
	int	tm_isdst;
};

#endif	/* ! defined (__COMMON__TM_H__) */
