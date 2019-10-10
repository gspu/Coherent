/ $Header: /usr/src/sys/ldrv/RCS/getcs.s,v 1.1 88/03/24 16:30:36 src Exp $
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
/ $Log:	/usr/src/sys/ldrv/RCS/getcs.s,v $
/ Revision 1.1	88/03/24  16:30:36	src
/ Initial revision
/ 
/ 87/12/08	Allan Cornish	/usr/src/sys/ldrv/getcs.s
/ Getcs() function obtains current code segment.
/
////////

////////
/
/ Get code selector.
/
////////

	.globl	getcs_

getcs_:	mov	ax, cs			/ Return code selector.
	ret
