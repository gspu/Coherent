/ $Header: /usr/src/sys/ldrv/RCS/sphi.s,v 1.1 88/03/24 16:31:13 src Exp $
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
/ $Log:	/usr/src/sys/ldrv/RCS/sphi.s,v $
/ Revision 1.1	88/03/24  16:31:13	src
/ Initial revision
/ 
/
////////

////////
/
/ Disable interrupts.  Previous value is returned.
/
////////

	.globl	sphi_

sphi_:
	pushf				/ Save flags
	pop	ax			/ Return current value
	cli				/ Disable interrupts
	ret				/ And return
