/* (-lgl
 * 	COHERENT Version 4.1
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/mdata.h
 * Magic machine numbers.
 * i80386.
 */

#ifndef	_MDATA_H
#define	_MDATA_H

/* Bits per type. */
#define	NBCHAR		8
#define	NBSHORT		16
#define	NBINT		32
#define	NBLONG		32
#define	NBFLOAT		32
#define	NBDOUBLE	64

/* Bits per pointer. */
#define	NBPCHAR		32
#define	NBPSHORT	32
#define	NBPINT		32
#define	NBPLONG		32
#define	NBPFLOAT	32
#define	NBPDOUBLE	32
#define	NBPSTRUCT	32
#define	NBPUNION	32

/*
 * Alignment.
 * The values listed below can be misleading.
 * Under iBCS2 alignment rules, struct or union alignment
 * depends on the member types rather than being a specific value.
 * #pragma directives may also change the alignment.
 */
/* Type alignment. */
#define	ALCHAR		1
#define	ALSHORT		2
#define	ALINT		4
#define	ALLONG		4
#define	ALFLOAT		4
#define	ALDOUBLE	4
#define	ALSTRUCT	4	/* actually max of alignments of members */
#define	ALUNION		4	/* actually max of alignments of members */

/* Pointer alignment. */
#define	ALPCHAR		4
#define	ALPSHORT	4
#define	ALPINT		4
#define	ALPLONG		4
#define	ALPFLOAT	4
#define	ALPDOUBLE	4
#define	ALPSTRUCT	4
#define	ALPUNION	4

/* Ranges. */
#define	MAXCHAR		127
#define	MINCHAR		(-128)
#define	MAXUCHAR	255
#define	MAXSHORT	32767
#define	MINSHORT	(-32768)
#define	MAXUSHORT	65535
#define	MAXINT		2147483647
#define	MININT		(-2147483648)
#define	MAXUINT		4294967295
#define	MAXLONG		2147483647L
#define	MINLONG		(-2147483648L)
#define	MAXULONG	4294967295L

/* Fixed point representation. */
#define	TCINT		1
#define	OCINT		0
#define	SMINT		0

/* Base2 logarithms of bits per type. */
#define	LOGCHAR		3
#define	LOGSHORT	4
#define	LOGINT		5
#define	LOGLONG		5

#endif

/* end of /usr/include/sys/mdata.h */
