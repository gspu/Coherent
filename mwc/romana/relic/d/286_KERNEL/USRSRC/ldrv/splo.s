/ $Header: /usr/src/sys/ldrv/RCS/splo.s,v 1.1 88/03/24 16:31:19 src Exp $
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
/ $Log:	/usr/src/sys/ldrv/RCS/splo.s,v $
/ Revision 1.1	88/03/24  16:31:19	src
/ Initial revision
/ 
/
////////

////////
/
/ Enable interrupts.  Previous value is returned.
/
////////

	.globl	splo_

splo_:
	pushf
	pop	ax
	sti
	ret
