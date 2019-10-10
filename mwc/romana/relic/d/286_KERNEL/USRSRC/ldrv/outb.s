/ $Header: /usr/src/sys/ldrv/RCS/outb.s,v 1.1 88/03/24 16:31:00 src Exp $
/
/	The  information  contained herein  is a trade secret  of INETCO
/	Systems, and is confidential information.   It is provided under
/	a license agreement,  and may be copied or disclosed  only under
/	the terms of that agreement.   Any reproduction or disclosure of
/	this  material  without  the express  written  authorization  of
/	INETCO Systems or persuant to the license agreement is unlawful.
/
/	Copyright (c) 1988
/	An unpublished work by INETCO Systems, Ltd.
/	All rights reserved.
/
/ $Log:	/usr/src/sys/ldrv/RCS/outb.s,v $
/ Revision 1.1	88/03/24  16:31:00	src
/ Initial revision
/ 
/
////////

////////
/
/ Basic port level I/O.
/
/ int	outb(port, data);
/
////////

	.globl	outb_

outb_:	mov	bx, sp
	mov	dx, 2(bx)
	mov	ax, 4(bx)
	outb	dx, al
	ret
