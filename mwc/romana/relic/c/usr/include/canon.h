/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Coherent.
 * Canonical conversion routines for the Intel 8086.
 */

#ifndef	CANON_H
#define	CANON_H	CANON_H

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
