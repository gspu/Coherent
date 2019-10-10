/ $Header: /kernel/kersrc/ldrv/RCS/inb.s,v 1.1 92/07/17 15:27:52 bin Exp Locker: bin $
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
/ $Log:	inb.s,v $
/ Revision 1.1  92/07/17  15:27:52  bin
/ Initial revision
/
/ Revision 1.1	88/03/24  16:30:39	src
/ Initial revision
/ 
/
////////

////////
/
/ Basic port level I/O.
/
/ int	inb(port);
/
////////

	.globl	inb_

inb_:	mov	bx, sp
	mov	dx, 2(bx)
	sub	ax, ax
	inb	al, dx
	ret
