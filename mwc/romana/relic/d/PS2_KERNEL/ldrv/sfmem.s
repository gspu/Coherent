/ $Header: /kernel/kersrc/ldrv/RCS/sfmem.s,v 1.1 92/07/17 15:28:07 bin Exp Locker: bin $
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
/ $Log:	sfmem.s,v $
/ Revision 1.1  92/07/17  15:28:07  bin
/ Initial revision
/
/ Revision 1.1	88/03/24  16:31:08	src
/ Initial revision
/ 
/
////////

////////
/
/ void
/ sfmem( fp, m, n )	-- set far memory
/ char far * fp;
/ char * m;
/ int n;
/
/	Input:	fp = far pointer [32 bit selector:offset] to destination
/		m  = source
/		n  = number of bytes to transfer.
/
/	Action:	Transfer 'n' bytes from offset 'm' in the current data space
/		to far address 'fp'.
/
/	Return:	None.
/
////////

	.globl	sfmem_

sfmem_:	push	si		/ void
	push	di		/ sfmem( fp, m, n )
	push	bp		/
	mov	bp, sp		/ register char * fp;		/* ES:DI */
	push	es		/ register char * m;		/* SI */
	les	di, 8(bp)	/ register int n;		/* CX */
	mov	si, 12(bp)	/
	mov	cx, 14(bp)	/ {
				/
	cld			/
	clc			/	for ( ; n != 0; --n )
	rcr	cx, $1		/
	rep			/		*fp++ = *m++;
	movsw			/
	rcl	cx, $1		/
	rep			/
	movsb			/
				/
	pop	es		/ }
	pop	bp
	pop	di
	pop	si
	ret
