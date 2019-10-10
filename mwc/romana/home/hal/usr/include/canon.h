/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__CANON_H__
#define	__CANON_H__

/*
 * Coherent.
 * Canonical conversion routines for the Intel 8086.
 */

long	_canl();

#define	candaddr(d)	((d)=_canl(d))
#define	candev(d)
#define	canino(i)
#define	canint(i)
#define	canlong(l)	((l)=_canl(l))
#define	canshort(i)
#define	cansize(s)	((s)=_canl(s))
#define	cantime(t)	((t)=_canl(t))
#define	canvaddr(v)

#endif
