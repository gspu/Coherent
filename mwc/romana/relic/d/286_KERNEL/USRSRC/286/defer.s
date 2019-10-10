/ $Header: /usr/src/sys/i8086/src/RCS/defer.s,v 1.2 88/04/04 17:05:05 src Exp $
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

////////
/
/ Defer a function [from interrupt level] for later execution
/
/	defer( f, a )	- defer a function [usually from interrupt level]
/	defend()	- execute deferred functions
/
/ $Log:	/usr/src/sys/i8086/src/RCS/defer.s,v $
/ Revision 1.2	88/04/04  17:05:05	src
/ ldefer/ldefend functions now allow deferred functions from loadable drivers.
/ 
/ Revision 1.1	88/03/24  17:39:20	src
/ Initial revision
/ 
/ 86/11/19	Allan Cornish		/usr/src/sys/i8086/src/defer.s
/ defer(func,arg) and defend() functions ported to Coherent from RTX.
/
////////

	.globl	defend_
	.globl	defer_
	.globl	ldefer

	.bssd
defunc:	.blkw	128		/ static void (*defunc[128])();
defarg:	.blkw	128		/ static char * defarg[128];
defqix:	.blkw	1		/ static int defqix;
defqox:	.blkw	1		/ static int defqox;
ldcseg:	.blkw	128		/ static saddr_t ldcseg[128];
ldfunc:	.blkw	128		/ static void (*ldfunc[128])();
	.shri

////////
/
/ void
/ defer( f, a )		-- defer a function [usually from interrupt level]
/ int (*f)();
/ char *a;
/
/	Input:	f = pointer to function to be deferred.
/		a = argument to pass to function when it is invoked.
/
/	Action:	Schedule function 'f' to be invoked with argument 'a'
/		during the transition from interrupt service level back
/		to user mode.
/
/	Return:	None.
/
/	Notes:	13 instructions executed.  Interrupt latency = 7 instructions.
/		Only 127 functions can be deferred at any one time.
/		Exceeding this limit will cause loss of ALL deferred functions.
/
////////

defer_:				/
	pop	ax		/ Convert IP into PSW,CS,IP to allow iret.
	pushf			/
	push	cs		/
	push	ax		/
	mov	bx, sp		/ defer( f, a )
	mov	ax, ss:6(bx)	/ register int (*f)();		/* AX */
	mov	dx, ss:8(bx)	/ register char *a;		/* DX */
				/ {
				/	register int x;		/* BX */
				/
	cli			/	sphi();
	mov	bx, defqix	/	x = defqix;
	mov	defunc(bx), ax	/	defunc[x] = f;
	mov	defarg(bx), dx	/	defarg[x] = a;
	addb	defqix, $2	/	defqix++;
//	sti			/	splo();
	iret			/ }

////////
/
/ void
/ defend( )		-- evaluate deferred functions
/
/	Action:	Evaluate all deferred functions.
/
/	Notes:	Should be called periodically by busy-wait device drivers.
/		4 + (n * 7) instructions executed, where n = # deferred func.
////////

defend_:			/ defend()
				/ {
				/
	mov	bx, defqox	/	register int x = defqox;    /* BX */
				/
	cmp	bx, defqix	/	if ( x != defqix ) {
	je	1f		/
				/		do {
0:	addb	defqox, $2	/			defqox++;
				/
	push	defarg(bx)	/			(*defunc[x])
	icall	defunc(bx)	/				(defarg[x]);
	add	sp, $2		/
				/
	mov	bx, defqox	/			x = defqox;
	cmp	bx, defqix	/
	jne	0b		/		} while ( x != defqix );
				/	}
1:	ret			/ }

////////
/
/ void
/ ldefer( f, a )	-- defer a far function [usually from interrupt level]
/ int (far*f)();
/ char *a;
/
/	Input:	f = pointer to loadable driver function to be deferred.
/		a = argument to pass to function when it is invoked.
/
/	Action:	Schedule loadable driver function 'f' to be invoked with
/		argument 'a' during the transition from interrupt service
/		level back to user mode.
/
/	Return:	None.
/
/	Notes:	16 instructions executed.  Interrupt latency = 8 instructions.
/		Only 127 functions can be deferred at any one time.
/		Exceeding this limit will cause loss of ALL deferred functions.
/
////////

ldefer:				/
	pop	ax		/ Convert IP into PSW,CS,IP to allow iret.
	pushf			/
	push	cs		/
	push	ax		/
	mov	bx, sp		/ defer( f, a )
	mov	ax, ss:6(bx)	/ register int (*f)();		/* CX:AX */
	mov	cx, ss:8(bx)	/
	mov	dx, ss:10(bx)	/ register char *a;		/* DX */
				/ {
				/	register int x;		/* BX */
				/
	cli			/	sphi();
	mov	bx, defqix	/	x = defqix;
	mov	ldfunc(bx), ax	/	ldfunc[x] = FP_OFF(f);
	mov	ldcseg(bx), cx	/	ldcseg[x] = FP_SEL(f);
	mov	defunc(bx),$ldefend/	defunc[x] = ldefend;
	mov	defarg(bx), dx	/	defarg[x] = a;
	addb	defqix, $2	/	defqix++;
//	sti			/	splo();
	iret			/ }

////////
/
/ static void
/ ldefend( )		-- evaluate deferred far function
/
/	Action:	Evaluate deferred far function.
/
/	Notes:	Only called by defend().  Register BX contains driver's defqox.
/
////////

ldefend:			/ PARAMETERS MUST REMAIN AT 4(BP).
	push	bp		/ DRIVER RESIDENT CODE RELIES ON THIS.
	mov	bp, sp		/
				/
	mov	ax, ldfunc(bx)	/ AX = Driver function to be invoked.
				/
	push	ldcseg(bx)	/ Define driver entry point.
	push	four		/
				/
	xcall	-4(bp)		/ Invoke driver entry point, which will
				/	in turn invoke the deferred function.
				/
	mov	sp, bp		/ Return to caller.
	pop	bp		/
	ret			/

	.prvd
four:	.word	4
	.shri
