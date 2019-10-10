/ $Header: /x/usr/src/sys/i8086/src/RCS/as1.s,v 1.2 91/06/20 14:07:20 hal Exp $

/ (lgl-
/	The information contained herein is a trade secret of Mark Williams
/	Company, and  is confidential information.  It is provided  under a
/	license agreement,  and may be  copied or disclosed  only under the
/	terms of  that agreement.  Any  reproduction or disclosure  of this
/	material without the express written authorization of Mark Williams
/	Company or persuant to the license agreement is unlawful.
/
/	COHERENT Version 2.3.37
/	Copyright (c) 1982, 1983, 1984.
/	An unpublished work by Mark Williams Company, Chicago.
/	All rights reserved.
/ -lgl)
////////
/
/ Machine language assist for
/ 8086/8088 Coherent. This contains the parts
/ that are common to all machines.
/
/ Note that several of the following constants can be invalidated
/ by changing the contents of ../h/*proc.h among others,
/ or by changing the number of automatic variables in the functions
/ called on the paths leading to consave or conrest.  Tread with caution.
/
/ $Log:	as1.s,v $
/ Revision 1.2  91/06/20  14:07:20  hal
/ I'm not sure what changed here.

/ Revision 1.3	88/08/05  08:32:09 	src
/ Kludges for AMD 286 bug have been removed.
/ 
/ Revision 1.2	88/06/24  16:02:08	src
/ Bug:	inb/outb did not work properly in split stack/data operation.
/ Fix:	inb/outb now explicitly reference the stack segment.
/ 
/ Revision 1.1	88/03/24  17:39:08	src
/ Initial revision
/ 
/ 88/03/10	Allan Cornish		/usr/src/sys/i8086/src/as1.s
/ Numerous temporary fixes due to AMD 286 chip being buggy in protected mode.
/ These partial fixes will be removed once all CPU's are replaced.
/
/ 88/03/04	Allan Cornish		/usr/src/sys/i8086/src/as1.s
/ tmpcs*/tmpds* temporary variables renamed to sav_*.
/ usave/conrest/etc now handle odd byte alignment on stack.
/ envrest/conrest now do interrupt return as last instruction.
/
/ 87/12/21	Allan Cornish		/usr/src/sys/i8086/src/as1.s
/ fclear(f,n) function added.
/
/ 87/12/04	Allan Cornish		/usr/src/sys/i8086/src/as1.s
/ kfcopy(k,f,n) and fkcopy(f,k,n) routines added.
/
/ 87/12/03	Allan Cornish		/usr/src/sys/i8086/src/as1.s
/ popf replaced by 'push cs; mov ax,$0f; push ax; iret; 0:' due to popf int bug.
/
/ 87/11/05	Allan Cornish		/usr/src/sys/i8086/src/as1.s
/ slrcopy/srlcopy/sclear renamed plrcopy/prlcopy/pclear and moved to ibm*/as2.s
/
/ 87/10/27	Allan Cornish	/usr/src/sys/i8086/src/as1.s
/ System stack/data segments now setup in ibm/ibm_at as2.s
/
/ 87/02/06	Allan Cornish
/ Functions ffword and sfword added to fetch and set far memory.
/
////////

UPASIZE	=	1024			/ Size of uproc and stack
USIZE	=	0x114			/ sizeof(UPROC)
USZ1	=	140			/ Word count for usave, uproc size
UMCSP	=	0x06			/ offset of sp in u_syscon
EFAULT	=	14			/ Bad argument
PFLAGS	=	0x22			/ Offset into PROC.
PFKERN	=	0x80			/ Kernel process flag bit.
SIDEV	=	0x40			/ Device interrupt trap code.

////////
/
/ After performing machine specific
/ trap vector setup, the startup code jumps
/ here. The DS maps the vectors.
/
////////

	.globl	start

start:
/ Call the machine setup code.
/ Call Coherent main.
/ On return, send control off to the user
/ at its entry point.

	mov	sp, $u_+UPASIZE-32	/ Stack pointer for init
	call	i8086_			/ Do it.
	sti				/ Interrupts on, and
	call	main_			/ call Coherent mainline.
	cli				/ Interrupts off.
	incb	depth_			/ Set stack depth to user.
	sub	ax, ax			/ User mode IP.
	mov	bx, uds_		/ User mode DS, ES, SS
	mov	cx, ucs_		/ User mode CS.
	mov	dx, $0x0200		/ User mode FL.

	mov	ds, bx			/ Map data segment
	mov	es, bx			/ Map extra segment
	mov	ss, bx			/ Map stack segment

	mov	sp, $sb-aicodep_	/ User's stack
	push	dx			/ Flags
	push	cx			/ CS
	push	ax			/ IP
	iret				/ Go to user state.

////////
/
/ Trap and interrupt save.
/ These routines will be very familiar to any
/ RSX-11M hackers out there; it is just the ever
/ common co-routine call. The caller is called back,
/ with "bx" pointing to the saved "ax" on the stack,
/ with interrupts enabled. The called routine must
/ set 16(bx), which was initially the call back address,
/ to something non zero in the high byte if it does
/ not want an EOI sent to the 8259.
/
/ ttsave, tksave, tisave, and tusave could be folded into a single routine
/ with many tests and branches, but the frequency of passage through
/ this code warrants a minimally branched execution, and special casing
/ the interrupted context allows minimal memory references.
/
////////

	.globl	tsave

tsave:				/ What level of interrupt ?
	push	ds			/ Save current data base
	mov	ds, cs:cds		/ remap to system data space.
	pop	sav_ds			/
	decb	depth_			/ Adjust stack depth.
	je	tkusave			/ If e, stack switch may be needed.
ttsave:				/ Interrupted within interrupt
	cmp	sp,$u_+USIZE+50		/ Check for stack
	jbe	tabort			/ overflow
	push	ss			/ Fake save ss
	push	sp			/ Fake save sp
	push	sav_ds			/ Save ds
	push	bx			/ Save bx

	sti				/ More interrupts ok

	push	ax			/ Save ax
	push	dx			/ and remainder
	push	cx			/ of machine
	push	es			/ state
	mov	ax,ds			/ Map es
	mov	es,ax			/ to system ds
	mov	bx,sp			/ Load index
	icall	16(bx)			/ and call the caller
	mov	bx,sp			/ Load index
	mov	ax,16(bx)		/ fetch trap type
	cmpb	ah, $SIDEV		/ see if eoi
	jne	ttkdone			/ can be skipped

	cli				/ don't let eoi swamp us
	call	eoi			/ Dismiss interrupt
ttkdone:			/ Common ttsave/tksave finish
	pop	es			/ Restore
	pop	cx			/ the
	pop	dx			/ machine state
	pop	ax			/ state

	cli				/ No more interrupts

	incb	depth_			/ Reset stack level
	pop	bx			/ Restore the
	pop	ds			/ last parts
	add	sp,$6			/ forget ss, sp, and ra.
	iret				/ Done.

tabort:				/ Uarea stack overflowed
	add	sp,$300			/ Make room for death.
	mov	ax,$oops		/ Load
	push	ax			/ message
	call	panic_			/ and die.

tkusave:			/ Kernel or user process interrupted
	mov	sav_bx,bx		/ Save bx in temp
	mov	bx,cprocp_		/ Load proc pointer
	test	PFLAGS(bx),$PFKERN	/ Kernel process ?
	je	tusave			/ Sorry, go do it all.
tksave:				/ Kernel process interrupted
	push	ss			/ Fake save ss
	push	sp			/ Fake save sp
	push	sav_ds			/ Save ds
	push	sav_bx			/ Save bx

	sti				/ More interrupts ok.

	cmp	bx,iprocp_		/ Is this the idle process ?
	je	tisave			/ Yes, very easy
	push	ax			/ Save the
	push	dx			/ rest of
	push	cx			/ the machine
	push	es			/ state
	mov	ax,ds			/ And map
	mov	es,ax			/ extra to system data
	mov	bx,sp			/ Load index
	icall	16(bx)			/ and call the caller back.
	mov	bx,sp			/ Load index
	mov	ax,16(bx)		/ and pull trap type.
	cmpb	ah, $SIDEV		/ Machine trap?
	jne	ttkdone			/ Yes, skip dismiss
	call	eoi			/ Dismiss interrupt
	jmp	ttkdone			/ Finish above

tisave:				/ Idle process interrupted, nothing to save
	sub	sp,$8			/ Push junk
	mov	bx,sp			/ Load index
	icall	16(bx)			/ and call the caller back.
	mov	bx,sp			/ Load index
	mov	ax,16(bx)		/ and pull trap type.
	cmpb	ah, $SIDEV		/ Machine trap ?
	jne	0f			/ Yes, skip dismiss.
	call	eoi			/ Dismiss interrupt
0:
	call	stand_			/ Clock, part 2
	cli				/ No more interrupts

	add	sp,$18			/ Pop everything
	incb	depth_			/ Reset level
	iret				/ Done

tusave:				/ User process interrupted
	mov	bx, $u_+UPASIZE		/ Get base of user area and
	pop	-8(bx)			/ pop the data that
	pop	-6(bx)			/ was pushed onto the user
	pop	-4(bx)			/ stack onto the new
	pop	-2(bx)			/ system stack.
	mov	sav_ss, ss		/ Save the old stack segment
	mov	sav_sp, sp		/ and stack pointer, then
	mov	ss, sds_		/ switch onto the
	lea	sp, -8(bx)		/ new stack in the user area.
	push	sav_ss			/ Push old ss
	push	sav_sp			/ Push old sp
	push	sav_ds			/ Push old ds and
	push	sav_bx			/ push old bx.

	sti				/ Allow more interrupts.

	push	ax			/ Save the
	push	dx			/ remainder of
	push	cx			/ the machine
	push	es			/ state.
	mov	ax, ds			/ Map extra
	mov	es, ax			/ segment to system data.
	mov	bx, sp			/ Load up an index into the stack
	icall	16(bx)			/ and call the caller back.
	mov	bx, sp			/ Load up stack index.
	mov	ax, 16(bx)		/ Pull trap type.
	cmpb	ah, $SIDEV		/ Is this a machine trap ?
	jne	0f			/ Yes, skip dismiss.
	call	eoi			/ Do the dismiss.
0:	mov	bx, cprocp_		/ Have we become kernel?
	test	PFLAGS(bx), $PFKERN	/ If so, do kernel return.
	jne	ttkdone			/

	call	stand_			/ Clock, part 2
	pop	es			/ Restore the
	pop	cx			/ easy part of the
	pop	dx			/ machine
	pop	ax			/ state.

	cli				/ Interrupts off.

	incb	depth_			/ Adjust stack depth and
	pop	sav_bx			/ Pop off the old bx and
	pop	sav_ds			/ ds into statics, then
	pop	bx			/ map DS:BX over top of what
	pop	ds			/ was the previous stack.
	add	sp, $2			/ Pop ra.
	pop	-6(bx)			/ Copy the IP,
	pop	-4(bx)			/ the CS and the old
	pop	-2(bx)			/ FW onto the user's stack, then
	lea	sp, -6(bx)		/ switch back
	mov	bx, ds			/ to the
	mov	ss, bx			/ user's stack.
	mov	ds, cs:cds		/
	mov	bx, sav_bx		/ Reload the bx and the
	mov	ds, sav_ds		/ ds, then
	iret				/ exit interrupt.

////////
/
/ This dummy routine is put in vector
/ table slots that are unused. All it does is
/ return to the caller.
/
////////

	.globl	vret_

vret_:	ret

////////
/
/ Fetch a word from the user's data space.
/
/ getuwd(u)
/ char *u;
/
////////

	.globl	getuwd_
	.globl	getupd_

getuwd_:
getupd_:
	mov	bx,sp			/ Base pointer
	mov	bx,2(bx)		/ Argument
	cmp	bx,udl_			/ In range?
	ja	kuerr			/ No

	push	es			/ Save extra map and
	mov	es, uds_		/ remap over the user's data.
	mov	ax,es:(bx)		/ Get word
	pop	es			/ Restore es.
	ret				/ Return

////////
/
/ Fetch a word from the user's code space.
/
/ getuwi(u)
/ char *u;
/
////////

	.globl	getuwi_

getuwi_:
	mov	bx,sp			/ Base pointer
	mov	bx,2(bx)		/ Argument
	cmp	bx,ucl_			/ In range?
	ja	kuerr			/ No

	push	es			/ Save extra.
	mov	es,ucs_			/ Users data segment
	mov	ax,es:(bx)		/ Get word
	pop	es			/ Restore extra.
	ret				/ Return

////////
/
/ Fetch a byte from the user's data space.
/
/ getubd(u)
/ char *u;
/
////////

	.globl	getubd_

getubd_:
	mov	bx,sp			/ Base pointer
	mov	bx,2(bx)		/ Argument
	cmp	bx,udl_			/ In range?
	ja	kuerr			/ No

	push	es			/ Save es.
	mov	es,uds_			/ Users data segment
	movb	al,es:(bx)		/ Get word
	pop	es			/ Restore es.
	subb	ah,ah			/ Clear upper half
	ret				/ Return

////////
/
/ Store a word into the user's data space.
/
/ putuwd(u, w)
/ char *u;
/ int w;
/
////////

	.globl	putuwd_

putuwd_:
	mov	bx,sp			/ Base pointer
	mov	ax,4(bx)		/ New value
	mov	bx,2(bx)		/ Argument
	cmp	bx,udl_			/ In range?
	ja	kuerr			/ No

	push	es			/ Save es.
	mov	es,uds_			/ Users data segment
	mov	es:(bx),ax		/ Set value
	pop	es			/ Restore es.
	sub	ax,ax			/ Succesful
	ret				/ Return

////////
/
/ Store a word into the user's code space.
/
/ putuwi(u, w)
/ char *u;
/ int w;
/
////////

	.globl	putuwi_

putuwi_:
	mov	bx,sp			/ Base pointer
	mov	ax,4(bx)		/ New value
	mov	bx,2(bx)		/ Argument
	cmp	bx,ucl_			/ In range?
	ja	kuerr			/ No

	push	ucs_			/ Get physical address.
	sub	ax, ax			/ DX:AX = phy = vtop( ucs:0 );
	push	ax			/
	call	vtop_			/
	add	sp, $4			/
					/
	sub	bx, bx			/ Get writable virtual address.
	push	bx			/ DX:AX = fp = ptov( phy, ucl );
	push	ucl_			/
	push	dx			/
	push	ax			/
	call	ptov_			/
	add	sp, $8			/
					/
	mov	bx, sp			/
	mov	ax, 4(bx)		/ New value
	mov	bx, 2(bx)		/ Argument
	push	es			/ Save ES
	mov	es, dx			/ Users (writable) code segment
	mov	es:(bx), ax		/ Set value
	pop	es			/ Restore es
					/
	push	dx			/ Release writable virtual address.
	sub	ax, ax			/ vrelse( fp );
	push	ax			/
	call	vrelse_			/
	add	sp, $4			/
					/
	sub	ax,ax			/ Succesful
	ret				/ Return

////////
/
/ Store a byte into the user's data space.
/
/ putubd(u, w)
/ char *u;
/ int w;
/
////////

	.globl	putubd_

putubd_:
	mov	bx,sp			/ Base pointer
	mov	ax,4(bx)		/ New value
	mov	bx,2(bx)		/ Argument
	cmp	bx,udl_			/ In range?
	ja	kuerr			/ No

	push	es			/ Save es.
	mov	es,uds_			/ Users data segment
	movb	es:(bx),al		/ Set value
	pop	es			/ Restore es.
	sub	ax,ax			/ Succesful
	ret				/ Return

////////
/
/ Block transfer "n" bytes from location
/ "k" in the system map to location "u" in the
/ user's data space. Return the number of bytes
/ transferred.
/
/ kucopy(k, u, n)
/ char *k;
/ char *u;
/ int n;
/
////////

	.globl	kucopy_

kucopy_:
	mov	bx,sp			/ Base pointer
	mov	ax,4(bx)		/ User address
	dec	ax			/ Don't wrap too soon
	add	ax,6(bx)		/ Add count
	jc	kuerr			/ Out of bounds
	cmp	ax,udl_			/ In range?
	ja	kuerr			/ No

	push	si			/ Save si
	push	di			/ Save di
	push	es			/ Save es.

	mov	si,2(bx)		/ Kernel address
	mov	di,4(bx)		/ User address
	mov	cx,6(bx)		/ Count
	mov	ax,cx			/ Move here to return
	mov	es,uds_			/ Map extra segment to user

	cld				/ Auto increment
	clc				/
	rcr	cx, $1			/ Calculate Word count
	rep				/
	movsw				/ Move words.
	rcl	cx, $1
	rep
	movsb				/ Move odd byte.

	pop	es			/ Restore es.
	pop	di			/ Restore di
	pop	si			/ Restore si
	ret				/ Return

////////
/
/ Block copy "n" bytes from location "u" in
/ the user data space to location "k" in the system
/ data space. Return the actual number of bytes
/ moved.
/
/ ukcopy(u, k, n)
/ char *u;
/ char *k;
/ int n;
/
////////

	.globl	ukcopy_

ukcopy_:
	mov	bx,sp			/ Base pointer
	mov	ax,2(bx)		/ User address
	dec	ax			/ Don't wrap too soon
	add	ax,6(bx)		/ Count
	jc	kuerr			/ Out of bounds
	cmp	ax,udl_			/ In range?
	ja	kuerr			/ No

	push	si			/ Save si
	push	di			/ Save di
	push	ds			/ Save ds

	mov	si,2(bx)		/ User address
	mov	di,4(bx)		/ Kernel address
	mov	cx,6(bx)		/ Count
	mov	ax,cx			/ Move here to return
	mov	bx, uds_		/ Map data segment
	mov	ds, bx			/ avoiding bug in 8088.

	cld				/ Auto increment
	clc				/
	rcr	cx, $1			/ Word count, odd byte in carry.
	rep				/
	movsw				/ Move words.
	rcl	cx, $1
	rep				/ Move odd byte.
	movsb

	pop	ds			/ Restore ds
	pop	di			/ Restore di
	pop	si			/ Restore si
	ret				/ Return

////////
/
/ All of the above copy routines jump to
/ "kuerr", with the stack untouched, if they detect
/ a bounds error on a user address.
/
////////

kuerr:
	mov	bx,$u_			/ Pointer to user area
	movb	(bx),$EFAULT		/ Bad parameter error
	sub	ax,ax			/ Didn't copy anything
	ret				/ Return

////////
/
/ sfbyte( fp, b )	-- set far byte
/ int far * fp;
/ int b;
/
////////

	.globl	sfbyte_

sfbyte_:push	es		/ sfbyte( fp, b )
	push	di		/ register int far * fp;	/* ES:DI */
	push	bp		/ register int b;		/* AX */
	mov	bp, sp		/ {
	les	di, 8(bp)	/
	mov	ax, 12(bp)	/
				/
	movb	es:(di), al	/	*fp = b;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret

////////
/
/ sfword( fp, w )	-- set far word
/ int far * fp;
/ int w;
/
////////

	.globl	sfword_

sfword_:push	es		/ sfword( fp, w )
	push	di		/ register int far * fp;	/* ES:DI */
	push	bp		/ register int w;		/* AX */
	mov	bp, sp		/ {
	les	di, 8(bp)	/
	mov	ax, 12(bp)	/
				/
	mov	es:(di), ax	/	*fp = w;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret

////////
/
/ ffbyte( fp )		-- fetch far byte
/ int far * fp;
/
////////

	.globl	ffbyte_

ffbyte_:push	es		/ ffbyte( fp )
	push	di		/ register int far * fp;	/* ES:DI */
	push	bp		/ {
	mov	bp, sp		/
	les	di, 8(bp)	/
				/
	sub	ax, ax		/
	movb	al, es:(di)	/	return *fp;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret

////////
/
/ ffword( fp )		-- fetch far word
/ int far * fp;
/
////////

	.globl	ffword_

ffword_:push	es		/ ffword( fp )
	push	di		/ register int far * fp;	/* ES:DI */
	push	bp		/ {
	mov	bp, sp		/
	les	di, 8(bp)	/
				/
	mov	ax, es:(di)	/	return *fp;
				/
	pop	bp		/ }
	pop	di
	pop	es
	ret

////////
/
/ Block transfer "n" bytes from location
/ "k" in the system map to location "f"
/ in the virtual address space.
/ Return the number of bytes / transferred.
/
/ kfcopy(k, f, n)
/ char *k;
/ faddr_t f;
/ int n;
/
////////

	.globl	kfcopy_

kfcopy_:
	push	si			/ Save si
	push	di			/ Save di
	push	bp			/ Save bp
	mov	bp, sp			/ Base pointer
	push	es			/ Save es.

	mov	si, 8(bp)		/ Kernel address
	les	di, 10(bp)		/ Far address
	mov	cx, 14(bp)		/ Count
	mov	ax, cx			/ Move here to return

	cld				/ Auto increment
	clc				/
	rcr	cx, $1			/ Calculate Word count.
	rep				/
	movsw				/ Move words.
	rcl	cx, $1			/
	rep				/
	movsb				/ Move odd byte.
					/
	pop	es			/ Restore es.
	pop	bp			/ Restore bp.
	pop	di			/ Restore di
	pop	si			/ Restore si
	ret				/ Return

////////
/
/ Block transfer "n" bytes from location
/ "f" in the virtual addres sspace to
/ location "f" in the system map.
/ Return the number of bytes / transferred.
/
/ fkcopy(f, k, n)
/ faddr_t f;
/ char *k;
/ int n;
/
////////

	.globl	fkcopy_

fkcopy_:
	push	si			/ Save si
	push	di			/ Save di
	push	bp			/ Save bp
	mov	bp, sp			/ Base pointer
	push	ds			/ Save ds.

	lds	si, 8(bp)		/ Far address
	mov	di, 12(bp)		/ Kernel address
	mov	cx, 14(bp)		/ Count
	mov	ax, cx			/ Move here to return

	cld				/ Auto increment
	clc				/
	rcr	cx, $1			/ Calculate Word count.
	rep				/
	movsw				/ Move words.
	rcl	cx, $1			/
	rep				/
	movsb				/ Move odd byte.
					/
	pop	ds			/ Restore ds.
	pop	bp			/ Restore bp.
	pop	di			/ Restore di
	pop	si			/ Restore si
	ret				/ Return

////////
/
/ fclear( fp, n )	- Erase far memory.
/ faddr_t fp;
/ unsigned n;
/
////////

	.globl	fclear_

fclear_:
	push	es			/ Save es
	push	di			/ Save di
	push	bp			/ Save bp
	mov	bp, sp			/ Base pointer

	les	di, 8(bp)		/ Far address
	mov	cx, 12(bp)		/ Count
	sub	ax, ax			/

	cld				/ Auto increment
	clc				/
	rcr	cx, $1			/ Calculate Word count.
	rep				/
	stosw				/ Clear words.
	rcl	cx, $1			/
	rep				/
	stosb				/ Clear odd byte.
					/
	pop	bp			/ Restore bp.
	pop	di			/ Restore di.
	pop	es			/ Restore es.
	ret				/ Return

////////
/
/ Profile scaling.
/
////////

	.globl	pscale_

pscale_:
	mov	bx,sp			/ Base pointer
	mov	ax,2(bx)		/ Multiply
	mul	4(bx)			/
	mov	ax,dx			/ Get high half
	ret				/ And return

////////
/
/ Save the environment of a process
/ envsave(p)
/ MENV *p;
/
/ Save the context of a process
/ consave(p)
/ MCON *p;
/
////////

	.globl	consave_
	.globl	envsave_

envsave_:
consave_:
	mov	cx, di			/ Hide di.
	mov	bx, sp			/ Point bx at the stack and
	mov	di, 2(bx)		/ di at the MCON block.
	cld				/ Ensure increment.
	mov	ax, cx			/ Save di
	stosw
	mov	ax, si			/ Save si
	stosw
	mov	ax, bp			/ Save bp
	stosw
	mov	ax, sp			/ Save sp
	stosw
	mov	ax, (bx)		/ Save ra as pc
	stosw
	pushf				/ Save fw
	pop	ax
	stosw
	movb	al, depth_		/ Save stack depth
	cbw
	stosw
	mov	di, cx			/ Put di back,
	sub	ax, ax			/ indicate a state save and
	ret				/ return to caller.

////////
/
/ Restore the environment of a process.
/ envrest(p)
/ MENV *p;
/
////////

	.globl	envrest_

envrest_:
	cli
	cld
	mov	bx,sp			/ Base pointer
	mov	si,2(bx)		/ Pointer to context
	lodsw				/ Restore di
	mov	di,ax			/
	lodsw				/ Restore si
	mov	cx,ax			/ Save for later
	lodsw				/ Restore bp
	mov	bp,ax			/
	lodsw				/ Restore sp
	mov	sp,ax			/
	mov	bx,ax			/ Our frame
	push	cs			/ Push current CS
	lodsw				/ Restore pc
	push	ax			/
	lodsw				/ Restore flags
	mov	(bx),ax			/ Stack now in form PSW,CS,IP.
	lodsw				/ Restore stack depth
	cli				/ No more interrupts
	movb	depth_, al
	mov	si,cx			/ Restore si
	mov	ax,$1			/ We are restoring
	iret				/ Return through PSW,CS,IP.

////////
/
/ Restore the context of a process.
/ Called with interrupts disabled from dispatch.
/ conrest(u, o)
/ saddr_t u;
/
////////

	.globl	conrest_

conrest_:
	decb	depth_			/ Falsify user/system state
	sti				/ Interrupts ok here
			/ Save current uarea
	call	usave_			/ Save the uarea in its segment.
			/ Copy in new uarea
	mov	bx,sp			/ Base pointer
	mov	ax, 2(bx)		/ Fetch uarea saddr_t
	mov	bx, 4(bx)		/ Fetch syscon offset
	mov	uasa_, ax		/ Save uarea saddr_t
	mov	cx,$USZ1		/ uproc size
/	mov	es,sds_			/ system data segment
	mov	di,$u_			/ system data uarea offset
	mov	ds,ax			/ uarea segment
	sub	si,si			/ uarea offset
	mov	sp,UMCSP(bx)		/ new stack
	cld				/ increment
	rep				/ repeat
	movsw				/ copy uproc
	mov	di,sp			/ stack offset in system data
	and	di,$~1			/ ensure word alignment
	mov	cx,$u_+UPASIZE		/ compute byte
	sub	cx,di			/ count
	mov	si,$UPASIZE		/ compute offset
	sub	si,cx			/ in segment
	shr	cx,$1			/ make word count
	rep				/ repeat
	movsw				/ copy stack
			/ Clean up
	mov	ax, es			/ Restore data
	mov	ds, ax			/ segment
			/ Now restore context
	add	bx, $u_			/ convert to address
	mov	si, bx			/ Get source index for restore
	lodsw				/ Restore di
	mov	di,ax			/
	lodsw				/ Restore si
	mov	cx,ax			/ Save for later
	lodsw				/ Restore bp
	mov	bp,ax			/
	lodsw				/ Restore sp
	mov	sp,ax			/
	mov	bx,ax			/ Our frame
	push	cs			/ Push current CS
	lodsw				/ Restore pc
	push	ax			/
	lodsw				/ Restore flags
	mov	(bx),ax			/ Stack now in form PSW,CS,IP.
	lodsw				/ Restore stack depth
	cli				/ No more interrupts
	movb	depth_, al
	mov	si,cx			/ Restore si
	mov	ax,$1			/ We are restoring
	iret				/ Return through PSW,CS,IP.

////////
/ usave()
/ Save uarea in segment.
/ Knowing that ds points to the system data segment
/ and that es should map there also.
/ And guaranteed not to step on ax or bx for conrest
/
	.globl	usave_
0:	ret
usave_:
	cmp	uasa_, $0		/
	je	0b
	push	es			/ Save es
	push	si			/ Save si
	push	di			/ Save di
	mov	cx,$USZ1		/ count
	sub	di,di			/ uarea segment offset
	mov	es,uasa_		/ uarea segment
	mov	si,$u_			/ system data offset
/	mov	ds,sds_			/ system data segment
	cld				/ increment
	rep				/ repeat
	movsw				/ copy uproc
	mov	si,sp			/ stack offset in system data
	and	si,$~1			/ ensure word alignment
	mov	cx,$u_+UPASIZE		/ compute byte
	sub	cx,si			/ count
	mov	di,$UPASIZE		/ compute offset
	sub	di,cx			/ in segment
	shr	cx,$1			/ make word count
	rep				/ repeat
	movsw				/ copy stack
	pop	di			/ Restore di
	pop	si			/ Restore si
	pop	es			/ Restore extra
	ret

/ Save useful registers.
/
	.globl	msysgen_
/
/ msysgen(p)
/ MGEN *p;
/
msysgen_:
	ret				/ Nothing useful to save

/ Disable interrupts.  Previous value is returned.
/
	.globl	sphi_

sphi_:
	pushf				/ Save flags
	pop	ax			/ Return current value
	cli				/ Disable interrupts
	ret				/ And return

/ Enable interrupts.  Previous value is returned.
/
	.globl	splo_

splo_:
	pushf
	pop	ax
	sti
	ret

/ Change interrupt flag.  Previous value is returned.
/
	.globl	spl_

spl_:
	pop	ax			/ ip
	pop	bx			/ psw
	push	bx
	push	bx			/ push psw, cs, ip for iret
	push	cs
	push	ax
	pushf				/ old psw
	pop	ax
	iret

////////
/
/ Idle routine.
/ Enable interupts, and wait for something to
/ happen. Does not do anything to the 8259, bacause
/ this will be set up correctly.
/
////////

	.globl	_idle_

_idle_:	sti				/ Interupts on.
	hlt				/ Wait for an interrupt
	ret				/ and return.

////////
/
/ The world is indeed grim.
/ Halt. Keep the interrupts on so that the
/ keyboard can get int.
/
////////

	.globl	halt_

halt_:	sti				/ Be safe,
0:
	hlt				/ and halt.
	jmp	0b			/ Paranoid, yes sir.

////////
/
/ Basic port level I/O.
/
/ int	inb(port);
/ int	outb(port, data);
/
////////

	.globl	inb_
	.globl	outb_

inb_:	mov	bx, sp
	mov	dx, ss:2(bx)
	sub	ax, ax
	inb	al, dx
	ret

outb_:	mov	bx, sp
	mov	dx, ss:2(bx)
	mov	ax, ss:4(bx)
	outb	dx, al
	ret

////////
/
/ Routines to move data to and from
/ the system auxiliary segment.
/
////////

	.globl	ageti_

ageti_:
	mov	bx,sp			/ Base pointer
	mov	bx,2(bx)		/ Pointer
	push	es			/ Save extra mapping and
	mov	es, sas_		/ remap.
	mov	ax,es:(bx)		/ Get value
	pop	es			/ Restore es and
	ret				/ Return

	.globl	aputp_
	.globl	aputi_

aputp_:
aputi_:
	mov	bx,sp			/ Base pointer
	mov	ax,4(bx)		/ Value
	mov	bx,2(bx)		/ Pointer
	push	es			/ Save extra base and
	mov	es, sas_		/ remap.
	mov	es:(bx),ax		/ Set value
	pop	es			/ Restore extra base
	ret				/ Return

	.globl	aputc_

aputc_:
	mov	bx,sp			/ Base pointer
	mov	ax,4(bx)		/ Value
	mov	bx,2(bx)		/ Pointer
	push	es			/ Save es and
	mov	es, sas_		/ remap.
	movb	es:(bx),al		/ Set value
	pop	es			/ Restore es and
	ret				/ Return

////////
/
/ Data. 
/ A small number of variables must be
/ in the code segment. All of these variables have
/ something to do with the interrupt linkage; when you
/ get an interrupt the only thing that is valid is
/ the code segment.
/
////////

	.globl	cds

	.shri
cds:	.blkw	1			/ Copy of "sds_".

	.globl	u_
	.globl	depth_,	sas_,	scs_,	sds_,	ucs_
	.globl	ucl_,	uds_,	udl_

	.bssd
	.even
u_:	.blkb	UPASIZE

	.prvd
oops:	.ascii	"stack overflow"
	.byte	0

depth_:	.byte	0			/ System state.

	.even
sas_:	.blkw	1			/ System auxiliary segment.
scs_:	.blkw	1			/ System code segment.
sds_:	.blkw	1			/ System data segment.
ucs_:	.blkw	1			/ User code segment.
ucl_:	.blkw	1			/ User code limit.
uds_:	.blkw	1			/ User data segment.
udl_:	.blkw	1			/ User data limit.
sav_ds:	.blkw	1			/ Four scratch words
sav_bx:	.blkw	1
sav_ss:	.blkw	1
sav_sp:	.blkw	1

////////
/
/ This is the image of the init process.
/ It gets copied into a user segment when the system
/ is first brought up. It must be in the data segment, because
/ that is how it is used, and it must be dephased in a funny
/ way because it is executed at location 0.
/
////////

	.globl	aicodep_		/ Position of code.
	.globl	aicodes_		/ Size of code.
	.globl	aidatap_		/ Position of data.
	.globl	aidatas_		/ Size of data.

	.shrd
aicodep_:
	sub	ax,ax			/ No environment
	push	ax
	mov	ax,$argl-aidatap_	/ Argument list
	push	ax
	mov	ax,$fn-aidatap_		/ File name
	push	ax
	sub	sp,$2			/ Dummy word for exec
	sys	11			/ Sys exec
	jmp	.			/ This should not return
aicodes_ = .-aicodep_

aidatap_:
	.word	0			/
	.word	0			/ Errno
	.word	0			/
	.word	0			/
	.word	0			/
	.word	0			/
	.word	0			/
	.word	0			/
argl:	.word	fn-aidatap_		/ argv[0] = "/etc/init";
	.word	a1-aidatap_		/ argv[1] = "";
	.word	0			/ argv[2] = NULL;

fn:	.ascii	"/etc/init\000"
a1:	.byte	0

	.even
	.blkb	64
sb:
aidatas_ = .-aidatap_
