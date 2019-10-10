/ (lgl-
/ 	COHERENT Driver Kit Version 1.1.0
/ 	Copyright (c) 1982, 1990 by Mark Williams Company.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
////////
/
/ AT Hard Disk Assembler Support
/
/ $Log:	atas.s,v $
/ Revision 1.1  92/07/17  15:24:03  bin
/ Initial revision
/
/ Revision 1.4  92/02/05  15:55:06  hal
/ Patchable ATSREG_.
/ 
/ Revision 1.3  91/10/24  12:37:44  hal
/ COH 3.2.03k.
/ Poll HF_REG (3F6) rather than CSR_REG (1F7).
/ 
/
/ atsend( off, seg ) - send 512 bytes from seg:off into hard disk buffer
/ atrecv( off, seg ) - receive 512 bytes from hard disk buffer into seg:off.
/ DRQ is not checked.  DRQ must be true before atsend/atrecv are called.
/
/ atbsyw()	     - wait while controller is busy
/ atdrqw()	     - wait for controller to request data transfer
/
////////

	.globl	atsend_
	.globl	atrecv_
	.globl	atbsyw_
	.globl	atdrqw_
	.globl	ATSREG_

	BSY_ST	= 0x80
	DRQ_ST	= 0x08

////////
/
/ void
/ atsend( fp )	-- send 512 bytes to AT disk controller.
/ faddr_t fp;
/
/	Input:	fp = far pointer [sel:off] to data buffer.
/
/	Action:	Transfer 512 bytes to AT disk controller from buffer.
/
////////

atsend_:
	push	si
	push	ds
	push	bp
	mov	bp, sp
	lds	si, 8(bp)
	mov	cx, $256
	mov	dx, $0x1F0
	cld
	rep
	outs
	pop	bp
	pop	ds
	pop	si
	ret

////////
/
/ void
/ atrecv( fp )	-- receive 512 bytes from AT disk controller.
/ faddr_t fp;
/
/	Input:	fp = far pointer [sel:off] to data buffer.
/
/	Action:	Transfer 512 bytes from AT disk controller to buffer.
/
////////

atrecv_:
	push	di
	push	es
	push	bp
	mov	bp, sp
	les	di, 8(bp)
	mov	cx, $256
	mov	dx, $0x1F0
	cld
	rep
	ins
	pop	bp
	pop	es
	pop	di
	ret

////////
/
/ atbsyw()	-- wait for AT disk controller to become not busy
/
/	Return:	0 = timeout
/		* = not busy
/
////////

atbsyw_:
	mov	dx, ATSREG_
	mov	bx, $4		/ add another layer of iteration for 486's
0:	mov	cx, $-1
1:	inb	al, dx
	testb	al, $BSY_ST
	loopne	1b
	je	2f		/ not busy - return nonzero value
	dec	bx
	jne	0b
2:	mov	ax, cx
	ret

////////
/
/ atdrqw()	-- wait for AT disk controller to initiate data request
/
/	Return:	0 = timeout
/		* = data requested
/
////////

atdrqw_:
	mov	dx, ATSREG_
	mov	bx, $4
0:	mov	cx, $-1
1:	inb	al, dx
	testb	al, $DRQ_ST
	loope	1b
	jne	2f		/ not busy - return nonzero value
	dec	bx
	jne	0b
2:	mov	ax, cx
	ret
