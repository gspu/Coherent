/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_CYRIX_H__
#define __SYS_CYRIX_H__

#include <common/ccompat.h>

/* cpu types - will autosense if initially unpatched */

enum {
	CYRIX_UNPATCHED=0,
	CYRIX_NOT=0xFFFF,	/* AMD or Intel, e.g. */
	CYRIX_OEM=0XFFFE,	/* Don't enable cache from software! */
	CYRIX_SRX=4, 		/* Cx486_SRx */
	CYRIX_DRX=5, 		/* Cx486_DRx */
	CYRIX_SRX2=6, 		/* Cx486_SRx2 */
	CYRIX_DRX2=7 		/* Cx486_DRx2 */
};

#if	_KERNEL

extern int CYRIX_CACHE;
extern int CYRIX_CPU;

/* From cyrix.c */
void cyrix_cache_on		__PROTO((void));
unsigned short cyrix_detect	__PROTO((void));

/* From cyrix0.s */
unsigned char cx_r		__PROTO((unsigned char index));
void cx_w			__PROTO((unsigned char index, unsigned char val));
void cx_invalidate_cache	__PROTO((void));
void cx_start_cache		__PROTO((void));
int iscyrix			__PROTO((void));

#endif /* _KERNEL */

#endif /* ! defined (__SYS_CYRIX_H__) */
