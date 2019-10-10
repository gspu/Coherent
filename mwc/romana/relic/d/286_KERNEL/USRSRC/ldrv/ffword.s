/ $Header: /usr/src/sys/ldrv/RCS/ffword.s,v 1.1 88/03/24 16:30:33 src Exp $
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
/ $Log:	/usr/src/sys/ldrv/RCS/ffword.s,v $
/ Revision 1.1	88/03/24  16:30:33	src
/ Initial revision
/ 
/
////////

////////
/
/ ffword( fp )		-- fetch far word
/ int far * fp;
/
////////

	.globl	ffword_

ffword_:push	es		/ ffword( fp )
	push	di		/ register int far * fp;	/* ES:DI */
	push	bp		/ {
	mov	bp, sp		/
	les	di, 8(bp)	/
				/
	mov	ax, es:(di)	/	return *fp;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret
