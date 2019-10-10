/ $Header: /kernel/kersrc/ldrv/RCS/sfword.s,v 1.1 92/07/17 15:28:08 bin Exp Locker: bin $
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
/ $Log:	sfword.s,v $
/ Revision 1.1  92/07/17  15:28:08  bin
/ Initial revision
/
/ Revision 1.1	88/03/24  16:31:11	src
/ Initial revision
/ 
/
////////

////////
/
/ sfword( fp, w )	-- set far word
/ int far * fp;
/ int w;
/
////////

	.globl	sfword_

sfword_:push	es		/ sfword( fp, w )
	push	di		/ register int far * fp;	/* ES:DI */
	push	bp		/ register int w;		/* AX */
	mov	bp, sp		/ {
	les	di, 8(bp)	/
	mov	ax, 12(bp)	/
				/
	mov	es:(di), ax	/	*fp = w;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret
