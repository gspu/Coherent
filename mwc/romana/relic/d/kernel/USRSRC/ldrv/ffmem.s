/ $Header: /usr/src/sys/ldrv/RCS/ffmem.s,v 1.1 88/03/24 16:30:31 src Exp $
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
/ $Log:	/usr/src/sys/ldrv/RCS/ffmem.s,v $
/ Revision 1.1	88/03/24  16:30:31	src
/ Initial revision
/ 
/
////////

////////
/
/ void
/ ffmem( fp, m, n )	-- fetch far memory
/ faddr_t fp;
/ char * m;
/ int n;
/
/	Input:	fp = far pointer [32 bit selector:offset] to source
/		m  = destination
/		n  = number of bytes to transfer.
/
/	Action:	Transfer 'n' bytes from far address 'fp' to offset 'm'
/		in the current data space.
/
/	Return:	None.
/
////////

	.globl	ffmem_

ffmem_:	push	si		/ void
	push	di		/ ffmem( fp, m, n )
	push	bp		/
	mov	bp, sp		/ register faddr_t fp;		/* DS:SI */
	push	ds		/ register char * m;		/* DI */
	lds	si, 8(bp)	/ register int n;		/* CX */
	mov	di, 12(bp)	/
	mov	cx, 14(bp)	/ {
				/
	cld			/
	clc			/	for ( ; n != 0; --n )
	rcr	cx, $1		/
	rep			/		*m++ = *fp++;
	movsw			/
	rcl	cx, $1		/
	rep			/
	movsb			/
				/
	pop	ds		/ }
	pop	bp
	pop	di
	pop	si
	ret
