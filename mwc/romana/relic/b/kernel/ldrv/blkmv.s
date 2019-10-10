/ $Header: /kernel/kersrc/ldrv/RCS/blkmv.s,v 1.1 92/07/17 15:27:48 bin Exp Locker: bin $
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
/ $Log:	blkmv.s,v $
Revision 1.1  92/07/17  15:27:48  bin
Initial revision

/ Revision 1.1	88/03/24  16:30:25	src
/ Initial revision
/ 
/
////////

	.globl	blkmv

blkmv:	push	si
	push	di
	push	bp
	mov	bp, sp

	mov	di, 8(bp)
	mov	si, 10(bp)
	mov	cx, 12(bp)
	cld
	rep
	movsb

	mov	ax, 8(bp)
	mov	sp, bp
	pop	bp
	pop	di
	pop	si
	ret
