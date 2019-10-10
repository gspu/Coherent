/ $Header: /usr/src/sys/ldrv/RCS/sfbyte.s,v 1.1 88/03/24 16:31:05 src Exp $
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
/ $Log:	/usr/src/sys/ldrv/RCS/sfbyte.s,v $
/ Revision 1.1	88/03/24  16:31:05	src
/ Initial revision
/ 
/
////////

////////
/
/ sfbyte( fp, b )	-- set far byte
/ char far * fp;
/ char b;
/
////////

	.globl	sfbyte_

sfbyte_:push	es		/ sfbyte( fp, c )
	push	di		/ register char far * fp;	/* ES:DI */
	push	bp		/ register char c;		/* AX */
	mov	bp, sp		/ {
	les	di, 8(bp)	/
	mov	ax, 12(bp)	/
				/
	movb	es:(di), al	/	*fp = c;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret
