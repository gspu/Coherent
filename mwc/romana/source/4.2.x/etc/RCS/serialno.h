head	1.1;
access
	hal
	udo
	ljg
	jrd
	nigel
	fred;
symbols
	r12:1.1;
locks; strict;
comment	@ *@;


1.1
date	93.09.01.20.03.48;	author rcsadmin;	state Exp;
branches;
next	;


desc
@@


1.1
log
@COHERENT r12
@
text
@/*
 * serialno.h
 * COHERENT serial number encryption.
 */

#define VERSIONS	9
#define	NSERIAL		(9+1)		/* Length of serial number + NUL */

/* numtab.c */
extern int 	magic1[];
extern int 	magic2[];
extern int 	magic3[];

/* serialno.c */
extern	char	serialno[NSERIAL];
extern	int	isserial();

/* end of serialno.h */
@
