/ $Header: /kernel/kersrc/ldrv/RCS/defer.s,v 1.1 92/07/17 15:27:49 bin Exp Locker: bin $
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
/ $Log:	defer.s,v $
/ Revision 1.1  92/07/17  15:27:49  bin
/ Initial revision

/ Revision 1.1	88/04/04  16:40:21	src
/ Initial revision
/ 
/
////////

////////
/
/ void
/ defer( f, a )		- defer local function from loadable driver.
/ void (*f)();
/ int a;
/
/	Input:	f = pointer to function to be deferred.
/		a = argument to be passed to function.
/
/	Action:	Schedule local function 'f' to be invoked with argument 'a'
/		at next transition from kernel to user mode.
/
/	Return:	None.
/
////////

	.globl	defer_

defer_:	push	bp			/ defer( f, a )
	mov	bp, sp			/ void (*f)();
	push	6(bp)			/ int a;
	push	cs			/ {
	push	4(bp)			/	kcall( Kldefer, f, cs, a );
	mov	ax, $Kldefer		/
	push	ax			/
	call	kcall_			/
	mov	sp, bp			/
	pop	bp			/ }
	ret
