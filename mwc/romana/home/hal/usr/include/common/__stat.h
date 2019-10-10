/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON___STAT_H__
#define	__COMMON___STAT_H__

/*
 * This internal header file assigns the structure tags used for the various
 * forms of the POSIX.1 file-statistics structure "stat". This structure comes
 * in various incompatible forms as defined in such documents as the System V,
 * release 4 ABI, and the iBCS2 specification; however some code needs to
 * access both forms in a uniform way.
 *
 * Here, we create incomplete type declarations for types that can be used to
 * access either structure uniformly, and assign the structure tags based on
 * the features that are set in the system.
 */

#include <common/feature.h>

#if	_DDI_DKI

#define	__R4STAT_TAG	__r4stat
#define	__R3STAT_TAG	__r3stat

#elif	_SYSV4

#define	__R4STAT_TAG	stat
#define	__R4STAT_TAG	__r3stat

#else

/*
 * Suppress the R4 stat structure to avoid causing problems with auto-config
 * scripts that use 'grep' to determine what members the stat structure has.
 */
/* #define	__R4STAT_TAG	__r4stat */
#define	__R3STAT_TAG	stat

#endif

typedef	struct __R4STAT_TAG	__stat_t;
 
#define	_ST_FSTYPSZ	16

#endif	/* ! defined (__COMMON___STAT_H__) */
