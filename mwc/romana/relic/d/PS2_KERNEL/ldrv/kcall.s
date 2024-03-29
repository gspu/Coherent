/ $Header: /kernel/kersrc/ldrv/RCS/kcall.s,v 1.1 92/07/17 15:27:52 bin Exp Locker: bin $
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
/ $Log:	kcall.s,v $
/ Revision 1.1  92/07/17  15:27:52  bin
/ Initial revision
/
/ Revision 1.1	88/03/24  16:30:41	src
/ Initial revision
/ 
/
////////

////////
/
/ kcall( func, arg, ... )
/
/	Input:	func = kernel function to be invoked.
/		arg  = optional argument(s) to be passed to kernel function.
/
/	Action:	Invoke kernel function.
/
////////
	.globl	kcall_

kcall_:
	pop	bx		/ Convert stack from (retIP dstIP arg ...)
	pop	ax		/		  to (      retIP arg ...)
	push	bx		/ Leaving dstIP in AX register.
				/
	.byte 0x9A		/ Request kernel to perform far call.
	.word xcalled		/
	.word 0x0060		/
				/
	pop	bx		/ Convert stack from (      retIP arg ...)
	push	bx		/		  to (retIP retIP arg ...)
	push	bx		/ This allows caller to cleanup normally.
				/ NOTE: DO NOT MODIFY DX:AX RETURN VALUE.
				/
	ret			/ Return to caller.
