/ $Header: /usr/src/sys/ldrv/RCS/ffbyte.s,v 1.1 88/03/24 16:30:28 src Exp $
/
/	The  information  contained herein  is a trade secret  of INETCO
/	Systems, and is confidential information.   It is provided under
/	a license agreement,  and may be copied or disclosed  only under
/	the terms of that agreement.   Any reproduction or disclosure of
/	this  material  without  the express  written  authorization  of
/	INETCO Systems or persuant to the license agreement is unlawful.
/
/	Copyright (c) 1986
/	An unpublished work by INETCO Systems, Ltd.
/	All rights reserved.
/
/ $Log:	/usr/src/sys/ldrv/RCS/ffbyte.s,v $
/ Revision 1.1	88/03/24  16:30:28	src
/ Initial revision
/ 
/
////////

////////
/
/ ffbyte( fp )		-- fetch far byte
/ int far * fp;
/
////////

	.globl	ffbyte_

ffbyte_:push	es		/ ffbyte( fp )
	push	di		/ register char far * fp;	/* ES:DI */
	push	bp		/ {
	mov	bp, sp		/
	les	di, 8(bp)	/
				/
	sub	ax, ax		/
	movb	al, es:(di)	/	return( *fp );
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret
