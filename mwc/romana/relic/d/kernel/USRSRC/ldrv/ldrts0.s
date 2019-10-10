/ $Header: /usr/src/sys/ldrv/RCS/ldrts0.s,v 1.1 88/03/24 16:30:47 src Exp $
/ 
/	The  information  contained herein  is a trade secret  of INETCO
/	Systems, and is confidential information.   It is provided under
/	a license agreement,  and may be copied or disclosed  only under
/	the terms of that agreement.   Any reproduction or disclosure of
/	this  material  without  the express  written  authorization  of
/	INETCO Systems or persuant to the license agreement is unlawful.
/ 
/	Copyright (c) 1987
/	An unpublished work by INETCO Systems, Ltd.
/	All rights reserved.
/

////////
/
/ Loadable Driver Run Time Startup
/
/ Notes:	This function MUST be at offset 0 in driver code segment.
/
/ $Log:	/usr/src/sys/ldrv/RCS/ldrts0.s,v $
/ Revision 1.1	88/03/24  16:30:47	src
/ Initial revision
/ 
/
////////

	.globl	main_
	call	main_
	xret

////////
/
/	Invocation mechanism for local driver functions by kernel code.
/
/	Input:	AX	= pointer to local function to be invoked.
/		4(BP)	= 1st parameter to be passed to local function.
/		6(BP)	= 2nd parameter to be passed to local function.
/		8(BP)	= 3rd parameter to be passed to local function.
/
/	Action:	Invoke local function whose address is given in register AX,
/		passing parameters at offset 4,6,8 relative to register BP.
/		Perform a far return to operating system.
/
/	Notes:	Parameter passing convention specified by kernel.
/		This function MUST be at offset 4 in driver code segment.
/
////////

	push	8(bp)
	push	6(bp)
	push	4(bp)
	icall	ax
	add	sp, $6
	xret
