/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON_WHENCE_H__
#define	__COMMON_WHENCE_H__

/*
 * This internal header file defines the ISO C and POSIX.1 manifest constants
 * SEEK_SET, SEEK_END, and SEEK_CUR, used as the "whence" parameters in
 * functions such as fseek () and lseek ().
 */

#define	SEEK_CUR	1		/* from current position */
#define	SEEK_END	2		/* from end		*/
#define	SEEK_SET	0		/* from beginning	*/

#endif	/* ! defined (__COMMON_WHENCE_H__) */

