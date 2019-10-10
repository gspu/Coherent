/ $Header: /kernel/kersrc/i286/RCS/as2.s,v 1.1 92/07/17 15:21:20 bin Exp Locker: bin $
/
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
/ Coherent on the IBM personal computer.
/
/ $Log:	as2.s,v $
/ Revision 1.1  92/07/17  15:21:20  bin
/ Initial revision
/
/ Revision 1.3  92/01/21  16:10:37  hal
/ Use read_cmos_ routine.
/ Allows merged 386 C code compatibility.
/ 
/ Revision 1.2  91/06/06  18:14:46  norm
/ Get memory size by reading CMOS.

/ Revision 1.3	88/08/05  15:37:32	src
/ AMD 286 hardware specific fixes removed - hardware now correct.
/ Virtual Selector F000 initialized to access ROM at F0000.
/ Normal kernel stack now used during initialization.
/ 
/ Revision 1.2	88/06/29  19:05:31	src
/ AT Coherent can now come up in real-mode by patching 'realmode' variable.
/ 
/ Revision 1.1	88/03/24  17:33:18	src
/ Initial revision
/ 
/ 88/03/10	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ Numerous temporary fixes due to AMD 286 chip being buggy in protected mode.
/ These partial fixes will be removed once all CPU's are replaced.
/
/ 88/03/07	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ Obsolete video() function deleted - not used, or usable in protected mode.
/ Auto-increment mode no longer assumed, but enforced on block moves.
/
/ 88/03/04	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ Memory sizing now flushes instruction pipeline before read-verify.
/ Otherwise bus capacitance on some machines gives invalid memory indication.
/ plrcopy, prlcopy, pclear, upcopy, kpcopy, pucopy, and pkcopy now ensure
/ registers DS and ES refer to kernel data before calling ptov() or vrelse().
/ MAXMEM variable added to specify maximum low memory in clicks.
/
/ 87/11/22	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ Added check for extended memory in protected mode.
/
/ 87/11/14	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ boot() now requests 8042 controller to initiate processor reset.
/
/ 87/11/05	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ slrcopy/srlcopy/sclear renamed plrcopy/prlcopy/pclear and moved here.
/
/ 87/10/27	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ System stack/data segments now setup here rather than in as1.s
/ System stack/data moved to next 128 byte boundary for protected mode.
/
/ 87/08/31	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ Timer channel 1 now reprogrammed for memory refresh.
/
/ 87/07/08	Allan Cornish		/usr/src/sys/i8086/ibm_at/as2.s
/ Timer chip now programmed for 100 hz clock interrupt rather than 20 hz.
/
////////

EFAULT	=	14			/ Bad argument
EXTMEML =	0x17			/ Ext. mem size (low) offset in CMOS
EXTMEMH = 	0x18			/               (high)
PFLAGS	=	0x22			/ Offset int PROC.
PFKERN	=	0x80			/ Kernel process flag bit.
PIC	=	0x20			/ 8259 CSR  I/O port.
PICM	=	0x21			/ 8259 IMR  I/O port.
PIT	=	0x40			/ 8253 base I/O port.
KBDATA	=	0x60			/ 8042 keyboard mpu data I/O port.
KBCTRL	=	0x64			/ 8042 keyboard mpu ctrl I/O port.
CMOSA	=	0x70			/ Real-time Clock/CMOS addr I/O port.
CMOSD	=	0x71			/ Real-time Clock/CMOS data I/O port.
SPIC	=	0xA0			/ Slave 8259 CSR I/O port.
SPICM	=	0xA1			/ Slave 8259 IMR I/O port.
UPASIZE	=	1024			/ Size of uproc and stack

////////
/
/ System entry point. Under PC-DOS,
/ which thinks that Coherent is just a large
/ user program, the code is offset by 0x100
/ to allow space for the base page.
/
////////

	.blkb	0x0100			/ PC-DOS base page. | MLK --\
	call	iabios_r_   / Before we go into protected mode, init the abios

	cli				/ No interrupts, please.
	int	0x11			/ Obtain int 11 value before printf().
	mov	cs:val11, ax		/Use boot block's stack for last time.
/
/ Enable the A20 address line, which is normally disabled by the ROM BIOS.
/ This line is under the control of the 8042 keyboard interface controller.
/
	sub	cx, cx			/
0:	inb	al, KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	al, $2			/
	loopne	0b			/
	jmp	.+2		/ DELAY /
					/
	movb	al, $0xD1		/ Request next output byte to be
	outb	KBCTRL, al		/	sent to the 8042 output port.
					/
	sub	cx, cx			/
0:	inb	al, KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	al, $2			/
	loopne	0b			/
	jmp	.+2		/ DELAY /
					/
	movb	al, $0xDF		/ Enable A20 address line.
	outb	KBDATA, al		/ See Page 1-44, IBM-AT Tech Ref.
					/
	sub	cx, cx			/
0:	inb	al, KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	al, $2			/ NOTE: A20 not enabled for up to 20 us.
	loopne	0b			/

/
/ Reprogram the 8253 timer so that channel 0, 
/ which is used as the clock, interrupts at exactly
/ 100 HZ, instead of 18.2 HZ.
/
	movb	al, $0x36		/ Timer 0, LSB, MSB, mode 3
	outb	PIT+3, al
	jmp	.+2		/ DELAY /
	jmp	.+2		/ DELAY /
	movb	al, $0x9C		/ Lsb of 59659/5 = 11932
	outb	PIT, al
	jmp	.+2		/ DELAY /
	jmp	.+2		/ DELAY /
	movb	al, $0x2E		/ Msb of 59659/5 = 11932
	outb	PIT, al
	jmp	.+2		/ DELAY	/
	jmp	.+2		/ DELAY	/

/ Reprogram channel 1 on the 8253 timer which is used for memory refresh.
/	movb	al, $0x54		/ Timer 1, LSB, mode 2
/	outb	PIT+3, al
/	jmp	.+2		/ DELAY	/
/	jmp	.+2		/ DELAY	/
/	movb	al, $18			/ LSB of 18.
/	outb	PIT+1, al
/	jmp	.+2		/ DELAY	/
/	jmp	.+2		/ DELAY	/

/ Reprogram the 1st programmable interrupt controller.
/ It's default vector table collides with iAPX 286 protection vectors.

/	movb	al, $0x11		/ ICW1 - edge, master, ICW4
	movb	al, $0x19		/ ICW1 - level, master, ICW4 MLK
	outb	PIC, al
	jmp	.+2		/ DELAY	/
	jmp	.+2		/ DELAY	/
	movb	al, $0x20		/ ICW2 - Reserve 1st 32 vectors for 286
	outb	PICM, al
	jmp	.+2		/ DELAY	/
	jmp	.+2		/ DELAY	/
	movb	al, $0x04		/ ICW3 - master level 2
	outb	PICM, al
	jmp	.+2		/ DELAY	/
	jmp	.+2		/ DELAY	/
	movb	al, $0x01		/ ICW4 - 8086 mode, master.
	outb	PICM, al
	jmp	.+2		/ DELAY	/
	jmp	.+2		/ DELAY	/

	movb	al, $0xFE		/ Disable interrupts from master PIC.
	outb	PICM, al		/ (except for clock interrupt).

	movb	al, $0xFF
	outb	SPICM, al		/ Disable interrupts from slave PIC.


/ Set up all trap vectors.
/ The machine traps all have their own
/ linkages. We have to steal the clock from
/ the ROM, because the stacks might get switched
/ during the INT 1C, and the EOI would get sent
/ to the 8259 at a strange time.

	sub	ax, ax			/ Map DS over the 8088
	mov	ds, ax			/ vector area.

	mov	0x0000, $trap0		/ Divide error vector
	mov	0x0002, cs
	mov	0x0004, $trap1		/ Single step.
	mov	0x0006, cs
	mov	0x0008, $trap2		/ NMI
	mov	0x000A, cs
	mov	0x000C, $trap3		/ INT 3 (break)
	mov	0x000E, cs
	mov	0x0010, $trap4		/ Overflow.
	mov	0x0012, cs
	mov	0x0014, $trap5		/ Bound range exceeded.
	mov	0x0016, cs
	mov	0x0018, $trap6		/ Invalid Opcode
	mov	0x001A, cs
	mov	0x001C, $trap7		/ Processor extension not available
	mov	0x001E, cs
	mov	0x0020, $trap8		/ Double exception detected.
	mov	0x0022, cs
	mov	0x0024, $trap9		/ Processor extension segment overrun.
	mov	0x0026, cs
	mov	0x0028, $trap10		/ Invalid task state segment.
	mov	0x002A, cs
	mov	0x002C, $trap11		/ Segment not present.
	mov	0x002E, cs
	mov	0x0030, $trap12		/ Stack segment overrun or not present.
	mov	0x0032, cs
	mov	0x0034, $trap13		/ General protection.
	mov	0x0036, cs

	mov	0x0080, $clk_ps		/ Clock for PS2  MLK
/	mov	0x0080, $clk		/ Clock.
	mov	0x0082, cs
	mov	0x0084, $dev1		/ Device 1
	mov	0x0086, cs
	mov	0x0088, $dev9		/ Device 2 maps into Device 9
	mov	0x008A, cs
	mov	0x008C, $dev3		/ Device 3
	mov	0x008E, cs
	mov	0x0090, $dev4		/ Device 4
	mov	0x0092, cs
	mov	0x0094, $dev5		/ Device 5
	mov	0x0096, cs
	mov	0x0098, $dev6		/ Device 6
	mov	0x009A, cs
	mov	0x009C, $dev7		/ Device 7
	mov	0x009E, cs

	mov	0x01C0, $dev8		/ Device 8
	mov	0x01C2, cs
	mov	0x01C4, $dev9		/ Device 9
	mov	0x01C6, cs
	mov	0x01C8, $dev10		/ Device 10
	mov	0x01CA, cs
	mov	0x01CC, $dev11		/ Device 11
	mov	0x01CE, cs
	mov	0x01D0, $dev12		/ Device 12
	mov	0x01D2, cs
	mov	0x01D4, $dev13		/ Device 13
	mov	0x01D6, cs
	mov	0x01D8, $dev14		/ Device 14
	mov	0x01DA, cs
	mov	0x01DC, $dev15		/ Device 15
	mov	0x01DE, cs

	mov	bx, $0x0200		/ INT 80 (sys 0)
0:	mov	(bx), $syc		/ Set up the system call
	mov	2(bx), cs		/ trap vector.
	add	bx, $4			/ Move to next vector and
	cmp	bx, $0x0400		/ loop until all
	jb	0b			/ vectors are reset.

/ Set up the system stack and data segments, by looking at the size of
/ the text and adding this to the base address already in the CS.
/ Relocate the stack and data to a 128 byte boundary.

	mov	ax, $etext_+15		/ End of text segment
	shr	ax, $4			/ Convert to paragraphs.
	mov	cx, cs			/ Get code segment base.
	add	ax, cx			/
	mov	ds, ax			/ Current data segment
	add	ax, $31			/ Allow virtual-physical alignment
	and	ax, $~31		/	[use 512 byte, need 128 byte]
					/
	cmp	realmode_, $0		/ Virtual Addressing enabled?
	jne	0f			/
	mov	idtsel_, ax		/ Interrupt descriptor table [2 Kbytes]
	add	ax, $0x0080		/	2K >> 4
	mov	gdtsel_, ax		/ Global descriptor table [64 Kbytes]
	add	ax, $0x1000		/	64K >> 4
0:					/
	mov	es, ax			/
	mov	si, $edata_-1		/ Copy data to new location, backwards.
	mov	di, $edata_-1		/
	mov	cx, $edata_		/
	std				/
	rep				/
	movsb				/
					/
	mov	ds, ax			/ Update data segment,
	mov	ss, ax			/ and stack segment.
	mov	sp, $u_+UPASIZE-32	/ Set up initial stack.
	mov	scs_, cs		/ Save code segment and
	mov	sds_, ds		/ data segment bases.
	mov	cs:cds, ds		/ For interrupts.

/ Size up memory, starting just above the system.
/ The memory is cleared, because somebody has to do a write
/ to set up the parity bits.

	mov	di, $edata_		/ Clear at edata...
	mov	cx, $512		/ for 1 Kbyte
	sub	ax, ax
	cld
	rep
	stosw

	mov	bp, $edata_+1023	/ Compute base.
	shr	bp, $4
	add	bp, sds_
	shr	bp, $6			/ Round down to a Kbyte boundary
	shl	bp, $6			/ so's we're in sync.

0:	sub	di, di			/ Destination.
	mov	es, bp			/ Set extra segment and
	mov	es:(di), ax		/ clear a word.
	jmp	.+2		/ FLUSH	/
	cmp	es:(di), ax		/ Should be zero now.
	jne	0f			/ Branch if memory end

	mov	cx, $512		/ 1K bytes, in words.
	cld
	rep
	stosw				/ Clear this 1K
	add	bp, $64			/ Move along by 1K
	cmp	bp, MAXMEM
	jb	0b			/ If not at video ram yet

0:	mov	es, sds_		/ Map extra.

	mov	ax, bp			/ Calculate top of low memory.
	rol	ax, $4			/
	mov	dx, ax			/
	and	ax, $0xFFF0		/
	xor	dx, ax			/

	cmp	realmode_, $0		/ Real Addressing Mode?
	je	0f			/
	mov	coretop_, ax		/ Yes, Record top of memory,
	mov	coretop_+2, dx		/
	jmp	start			/ and bring up system.
0:
	mov	holebot_, ax		/ Record bottom of I/O memory.
	mov	holebot_+2, dx		/

	mov	ax, gdtsel_		/ Format global descriptor table map.
	rol	ax, $4			/
	mov	dx, ax			/
	and	ax, $0xFFF0		/
	xor	dx, ax			/
	mov	gdtmap_+0, $0xFFFF	/ Limit: 64K bytes.
	mov	gdtmap_+2, ax		/
	mov	gdtmap_+4, dx		/
					/
	sub	ax, ax			/ Erase global descriptor table.
	mov	cx, $0x8000		/	[32K words = 64K bytes]
	mov	es, gdtsel_		/
	sub	di, di			/
	cld				/
	rep				/
	stosw				/
					/
	mov	ax, idtsel_		/ Format interrupt descriptor table map
	rol	ax, $4			/
	mov	dx, ax			/
	and	ax, $0xFFF0		/
	xor	dx, ax			/
	mov	idtmap_+0, $2047	/ Limit: 2K bytes.
	mov	idtmap_+2, ax		/
	mov	idtmap_+4, dx		/
					/
	sub	ax, ax			/ Erase interrupt descriptor table.
	mov	cx, $1024		/	[1K words = 2K bytes]
	mov	es, idtsel_		/
	sub	di, di			/
	cld				/
	rep				/
	stosw				/
					/
	mov	es, gdtsel_		/
	mov	di, cs			/ Define kernel code global selector.
	mov	ax, $etext_-1		/	Limit: etext.
	stosw				/
	mov	dx, $0x9A00		/	Flags: Present, executable.
	mov	ax, cs			/	Base:  cs << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/
					/
	mov	di, ss			/ Define kernel data global selector.
	mov	ax, $0xFFFF		/	Limit: 64K bytes.
	stosw				/
	mov	dx, $0x9200		/	Flags: Present, writable.
	mov	ax, ss			/	Base:  ss << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/
					/
	mov	di, $8			/ Define task state segment selector[8]
	mov	ax, $43			/	Limit: 44 bytes.
	stosw				/
	mov	dx, $0x8100		/	Flags: Present, avail tss seg.
	mov	ax, ss			/	Base:  (ss << 4) + &tss.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	xor	dx, ax			/
	add	ax, $tss_		/
	adc	dx, $0			/
	stosw				/
	mov	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/
					/
	mov	di, gdtsel_		/ Define gdt access global selector.
	mov	ax, $0xFFFF		/	Limit: 64K bytes.
	stosw				/
	mov	dx, $0x9200		/	Flags: Present, writable.
	mov	ax, gdtsel_		/	Base: gdtsel << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/
					/
	mov	di, idtsel_		/ Define idt access global selector.
	mov	ax, $2047		/	Limit: 2K bytes.
	stosw				/
	mov	dx, $0x9200		/	Flags: Present, writable.
	mov	ax, idtsel_		/	Base: idtsel << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/
					/
	mov	di, $0xB000		/ Define video access global selector.
	mov	ax, $0xFFFF		/	Limit: 64K bytes.
	stosw				/
	mov	dx, $0x9200		/	Flags: Present, writable.
	mov	ax, $0xB000		/	Base:  0xB000 << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/
					/
	mov	di, $0xB800		/ Define video access global selector.
	mov	ax, $0x7FFF		/	Limit: 32 Kbytes.
	stosw				/
	mov	dx, $0x9200		/	Flags: Present, writable.
	mov	ax, $0xB800		/	Base:  0xB800 << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/

	mov	di, $0xF000		/ Define ROM access global selector.
	mov	ax, $0xFFFF		/	Limit: 64 Kbytes.
	stosw				/
	mov	dx, $0x9000		/	Flags: Present, read only.
	mov	ax, $0xF000		/	Base:  0xF000 << 4.
	rol	ax, $4			/
	xor	dx, ax			/
	and	ax, $0xFFF0		/
	stosw				/
	xor	ax, dx			/
	stosw				/
	sub	ax, ax			/
	stosw				/

	mov	es, idtsel_		/ Map ES over the intr descr table.
	sub	ax, ax			/ Map DS over the 8088 vector area.
	mov	ds, ax			/
	sub	si, si			/
	sub	di, di			/
	mov	bx, cs			/ Make CS available for comparison.
	mov	cx, $256		/ Install 256 interrupt descriptors.
					/
0:	lodsw				/ Copy interrupt IP
	stosw				/
	lodsw				/ Copy interrupt CS
	stosw				/
					/
	cmp	ax, bx			/ Coherent interrupt handler?
	mov	ax, $0x8600		/
	je	1f			/
	sub	ax, ax			/ No, clear flags.
					/
1:	stosw				/ Define IDT flags.
	sub	ax, ax			/ Reserved IDT word.
	stosw				/
	loop	0b			/ Repeat for all 256 entries.
					/
	mov	ax, ss			/ Restore data and extra segments.
	mov	ds, ax			/
	mov	es, ax			/
					/
	clts				/ Clear task switched flag.
	lgdt	gdtmap_			/ Load global descriptor table map.
	lidt	idtmap_			/ Load interrupt descriptor table map.
					/
	smsw	ax			/ Enter protected mode.
	or	ax, $1			/
	lmsw	ax			/
	jmp	.+2			/ Clear pipeline.
					/
	mov	ax, $0x0008		/ Load task state segment register.
	ltr	ax			/
	sub	ax, ax			/ Load local descriptor table register.
	lldt	ax			/
					/
					/
					/ Register usage:
					/ DX:AX = extended mem physical addr.
					/ BX = scratch, then 0.
					/ SI = selector into extended memory.
					/ ES = selector into extended memory.
					/ DS = selector into global descr table
					/
	push	$EXTMEMH		/ high byte of pair
	call	read_cmos_		/ result in ax
	add	sp, $2			/ pop argument
	movb	bl, al			/ save al to bl
	push	$EXTMEML		/ low byte of pair
	call	read_cmos_		/ result in ax
	add	sp, $2			/ pop argument
	movb	ah, bl			/ restore high byte to ah
	shr	ax, $6			/ K -> 64K conversion
	add	ax, $0x0010		/ bias up to 1MB
	mov	CMOSmax_, ax		/ save count of 64K hunks
	sub	ax, ax			/
	mov	dx, $0x0010		/ Initial 64 Kbyte bank of extended mem.
	mov	holetop_, ax		/ Recorded extended memory bot in bytes.
	mov	holetop_+2, dx		/
					/
	mov	ds, gdtsel_		/ Map DS onto global descr table.
	mov	si, $0xFFF8		/ Define scratch access global selector.
	mov	0(si), $0xFFFF		/	Limit: 64K bytes.
	mov	2(si), $0x0000		/	Base:  1 Mbyte.
	mov	4(si), $0x9210		/	Flags: Present, writable.
	mov	6(si), $0x0000		/
					/
	sub	bx, bx			/
0:	sub	di, di			/ Destination.
	mov	cx, $0x8000		/ 64K bytes, in words.
	mov	2(si), ax		/ Adjust gdt to desired DX:AX mem locn.
	movb	4(si), dl		/
	mov	es, si			/ Map ES onto 64K bank of extended mem.
	mov	es:(di), bx		/ Write word of extended memory.
	jmp	.+2		/ FLUSH	/
	cmp	es:(di), bx		/ Verify word was correctly written.
	jne	0f			/ Branch if memory end.
					/
	cld				/
	rep				/
	stosw				/ Clear this 64K of extended memory.
					/
	inc	dx			/ Step to next 64K bank.
	cmp	dx, ss:CMOSmax_		/ See if we're beyond what the CMOS
	jge	0f			/    says we have.
	cmp	dx, $0x00F0		/ Stop at 15 Mbyte boundary; the last
	jl	0b			/    Mbyte is a dup of the 1st Mbyte.
					/
0:	movb	5(si), $0		/ Free the scratch selector.
					/
	mov	bx, ss			/ Restore data and extra segments.
	mov	ds, bx			/ NOTE: Do not modify DX:AX.
	mov	es, bx			/
					/
	mov	coretop_, ax		/ Recorded top of extended core memory.
	mov	coretop_+2, dx		/
	jmp	start			/ Bring up system.

////////
/
/ Trap an interrupt linkage.
/ Each of the machine traps has a special little
/ linkage, that sets up the type code and sends
/ control off to the common trap processor. Device
/ interrupts, other than the clock (IR0), are
/ done here.
/
////////

trap0:
	call	tsave
	mov	16(bx), $0x0000		/ Divide error.
	jmp	trap_

trap1:
	call	tsave
	mov	16(bx), $0x0100		/ Single step.
	jmp	trap_

trap2:
	call	tsave
	mov	16(bx), $0x0200		/ Non-maskable interrupt.
	jmp	trap_

trap3:
	call	tsave
	mov	16(bx), $0x0300		/ INT 3 (breakpoint).
	jmp	trap_

trap4:
	call	tsave
	mov	16(bx), $0x0400		/ Overflow.
	jmp	trap_

trap5:
	call	tsave
	mov	16(bx), $0x0500		/ Bound check.
	jmp	trap_

trap6:
	call	tsave
	mov	16(bx), $0x0600		/ Invalid opcode.
	jmp	trap_

trap7:
	call	tsave
	mov	16(bx), $0x0700		/ Processor Extension not available.
	jmp	trap_

trap8:
	pop	ax			/ Get error code from stack [always 0]
	call	tsave
	mov	16(bx), $0x0800		/ Double Exception detected
	jmp	trap_

trap9:
	call	tsave
	mov	16(bx), $0x0900		/ Processor extension segment overrun
	jmp	trap_

trap10:
	pop	ax			/ Get error code from stack
	call	tsave
	mov	16(bx), $0x0A00		/ Invalid task state segment
	jmp	trap_

trap11:
	pop	ax			/ Get error code from stack
	call	tsave
	mov	16(bx), $0x0B00		/ Segment not present
	jmp	trap_

trap12:
	pop	ax			/ Get error code from stack
	call	tsave
	mov	16(bx), $0x0C00		/ Stack segment overrun or not present
	jmp	trap_

trap13:
	pop	ax			/ Get error code from stack
	call	tsave
	mov	16(bx), $0x0D00		/ General protection
	jmp	trap_

	.globl	syc

syc:
	call	tsave
	mov	16(bx), $0x2000		/ System calls.
	jmp	trap_

ran:
	call	tsave
	mov	16(bx), $0x2100		/ Random trap.
	jmp	trap_

dev1:
	call	tsave
	mov	16(bx), $0x4001		/ Device 1: keyboard
	ijmp	vecs_+[2*1]

/dev2:	call	tsave			/ Device 2: mapped into device 9
/	mov	16(bx), $0x4002
/	ijmp	vecs_+[2*2]

dev3:
	call	tsave
	mov	16(bx), $0x4003		/ Device 3: al1
	ijmp	vecs_+[2*3]

dev4:
	call	tsave
	mov	16(bx), $0x4004		/ Device 4: al0
	ijmp	vecs_+[2*4]

dev5:
	call	tsave
	mov	16(bx), $0x4005		/ Device 5: hard disk
	ijmp	vecs_+[2*5]

dev6:
	call	tsave
	mov	16(bx), $0x4006		/ Device 6: floppy
	ijmp	vecs_+[2*6]

dev7:
	call	tsave
	mov	16(bx), $0x4007		/ Device 7: lp
	ijmp	vecs_+[2*7]

dev8:
	call	tsave
	mov	16(bx), $0x4008		/ Device 8:
	ijmp	vecs_+[2*8]

dev9:
	call	tsave
	mov	16(bx), $0x4009		/ Device 9:
	ijmp	vecs_+[2*9]

dev10:
	call	tsave
	mov	16(bx), $0x400A		/ Device 10:
	ijmp	vecs_+[2*10]

dev11:
	call	tsave
	mov	16(bx), $0x400B		/ Device 11:
	ijmp	vecs_+[2*11]

dev12:
	call	tsave
	mov	16(bx), $0x400C		/ Device 12:
	ijmp	vecs_+[2*12]

dev13:
	call	tsave
	mov	16(bx), $0x400D		/ Device 13:
	ijmp	vecs_+[2*13]

dev14:
	call	tsave
	mov	16(bx), $0x400E		/ Device 14:
	ijmp	vecs_+[2*14]

dev15:
	call	tsave
	mov	16(bx), $0x400F		/ Device 15:
	ijmp	vecs_+[2*15]

////////
/
/ Clock interrupt.
/ The clock interrupt is stolen from the ROM;
/ if you don't do this the EOI sequence for the 8259
/ may get mangled on context switches.
/
////////

clk_ps:					/ MLK
	push	ax
	inb	al, 0x61		/ PS/2 8259 needs to be told to	
	jmp	.+2			/ stop interrupting the CPU, or
	jmp	.+2
	orb	al,$0x80		/ we will hang.
	outb	0x61,al
	pop	ax			/ MLK

clk:
	call	tsave			/ Perform trap save.
	mov	16(bx), $0x4000

	sub	ax, ax			/ Assume system mode, push user flag
	push	ax
	push	18(bx)			/ IP at tick time

	cmpb	depth_, $0		/ Correct ?
	jne	0f			/ If ne, yes.
	mov	bx, cprocp_		/ User depth, check if the
	test	PFLAGS(bx), $PFKERN	/ current process is a kernel process.
	jne	0f			/ If ne, yes.
	mov	bx, sp			/ Load stack index
	inc	2(bx)			/ and set user mode.

0:	call	clock_			/ Call common clock and
	add	sp, $4			/ pop arguments.

	ret				/ Back to "tsave".

////////
/
/ This routine is called by "tsave" to dismiss an interrupt.
/ The interrupt code is in "ax".
/
////////

	.globl	eoi

eoi:
	cmpb	al, $8			/ Is this on the slave PIC?
	jb	0f			/ Jump if not.
	movb	al, $0x20		/ Send a non specific EOI
	outb	SPIC, al		/ to the slave PIC.
0:	movb	al, $0x20		/ Send a non specific EOI
	outb	PIC, al			/ to the master PIC.
	ret				/ Done.

////////
/
/ Block I/O to ports.
/ Mainly used to read and write the silo memories in the discs.
/ Delibrately only transfers 1 byte per loop to avoid
/ timing problems on AT I/O chips.
/
/ void	outcopy(port, off, seg, n);
/ int	port;			/* Port address */
/ char *off;			/* Offset in segment */
/ unsigned seg;			/* Segment register base */
/ int	n;			/* Byte count */
/ 
/ void	incopy(port, off, seg, n);
/ int	port;			/* Device */
/ char *off;			/* Offset */
/ unsigned seg;			/* Segment register base */
/ int	n;			/* Byte count */
/
////////

	.globl	incopy_
	.globl	outcopy_

incopy_:
	push	di
	push	es
	push	bp
	mov	bp, sp
	mov	dx, 8(bp)		/device port
	les	di, 10(bp)		/seg,off pair
	mov	cx, 14(bp)		/n bytes
	jcxz	1f

	cld				/auto-increment
0:	inb	al, dx
	stosb
	loop	0b

1:	pop	bp
	pop	es			/restore regs
	pop	di
	ret

outcopy_:
	push	si
	push	ds
	push	bp
	mov	bp, sp
	mov	dx, 8(bp)		/device port
	lds	si, 10(bp)		/offset
	mov	cx, 14(bp)		/count
	jcxz	1f

	cld				/auto-increment
0:	lodsb
	outb	dx, al
	loop	0b

1:	pop	bp
	pop	ds
	pop	si
	ret

////////
/
/ Copy "n" bytes of memory from base "p1" to base "p2".
/ The copy must be done from left to right.
/
/ plrcopy(p1, p2, n)
/ paddr_t p1, p2;
/ size_t n;
/
////////

	.globl	plrcopy_

plrcopy_:
	push	si			/ Save sequence
	push	di
	push	bp
	mov	bp, sp

	push	ds			/ Save ds
	push	es			/ Save es

	push	18(bp)			/ Map SI:DI at destination ptov(p2,n).
	push	16(bp)
	push	14(bp)
	push	12(bp)
	call	ptov_
	add	sp, $8
	mov	si, dx
	mov	di, ax

	push	18(bp)			/ Map DX:AX at source	ptov(p1,n);
	push	16(bp)
	push	10(bp)
	push	8(bp)
	call	ptov_
	add	sp, $8

	mov	es, si			/ Map ES:DI at destination.
	mov	ds, dx			/ Map DS:SI at source.
	mov	si, ax

	mov	cx, 16(bp)		/ Transfer count in bytes.
	cld				/ Auto Increment.
	clc				/
	rcr	cx, $1			/ Word count
	rep				/
	movsw				/ Move words
	rcl	cx, $1			/
	rep				/
	movsb				/ Move odd byte

	mov	si, es			/ Remember mapped selectors.
	mov	di, ds			/
	pop	es			/ Restore es
	pop	ds			/ Restore ds

	push	si			/ Release mapped selectors.
	push	ax			/ NOTE: Offset is ignored.
	call	vrelse_			/
	add	sp, $4			/
					/
	push	di			/
	push	ax			/
	call	vrelse_			/
	add	sp, $4			/

	pop	bp			/ Standard return
	pop	di
	pop	si
	ret				/ Return

////////
/
/ Copy "n" bytes of memory from base "p1" to base "p2".
/ The copy must be done from right to left.
/
/ prlcopy(p1, p2, n)
/ paddr_t p1, p2;
/ size_t n;
/
////////

	.globl	prlcopy_

prlcopy_:
	push	si			/ Save sequence
	push	di
	push	bp
	mov	bp, sp

	push	ds			/ Save ds
	push	es			/ Save es

	push	18(bp)			/ Map SI:DI at destination ptov(p2,n).
	push	16(bp)
	push	14(bp)
	push	12(bp)
	call	ptov_
	add	sp, $8
	mov	si, dx
	mov	di, ax

	push	18(bp)			/ Map DX:AX at source	ptov(p1,n);
	push	16(bp)
	push	10(bp)
	push	8(bp)
	call	ptov_
	add	sp, $8

	mov	es, si			/ Map ES:DI at destination.
	mov	ds, dx			/ Map DS:SI at source.
	mov	si, ax

	mov	cx, 16(bp)		/ Transfer count in bytes
	add	si, cx			/ Point DS:SI at the end
	dec	si			/ of the source.
	add	di, cx			/ Point ES:DI at the end
	dec	di			/ of the destination.

	std				/ Auto decrement
	clc				/
	rcr	cx, $1			/ Word Count
	rep				/
	movsw				/ Move words
	rcl	cx, $1			/
	rep				/
	movsb				/ Move odd byte
	cld				/ Auto increment

	mov	si, es			/ Remember mapped selectors.
	mov	di, ds			/
	pop	es			/ Restore es
	pop	ds			/ Restore ds

	push	si			/ Release mapped selectors.
	push	ax			/ NOTE: Offset is ignored.
	call	vrelse_			/
	add	sp, $4			/
					/
	push	di			/
	push	ax			/
	call	vrelse_			/
	add	sp, $4			/
					/
	mov	ax, 16(bp)		/ Return transfer count.
	pop	bp			/ Standard return
	pop	di
	pop	si
	ret

////////
/
/ Clear "n" bytes of memory starting at physical address "p".
/
/ pclear( p, n )
/ paddr_t p;
/ size_t n;
/
/
/	Notes:	At most 64K bytes of memory can be cleared.
/
////////

	.globl	pclear_

pclear_:
	push	si			/ Standard save
	push	di
	push	bp
	mov	bp, sp

	push	es			/ Save es

	push	14(bp)			/ Map ES:DI at ptov(p2,n).
	push	12(bp)
	push	10(bp)
	push	8(bp)
	call	ptov_
	add	sp, $8
	mov	es, dx
	mov	di, ax

	shr	14(bp), $1		/ Convert count from bytes to words.
	rcr	12(bp), $1
	mov	cx, 12(bp)		/ Count in words.

	sub	ax, ax			/ Get a 0.
	cld				/ Zero the block.
	rep
	stosw

	mov	ax, es			/ Remember mapped selector.
	pop	es			/ Restore es.

	push	ax			/ Release mapped selector.
	push	ax			/ NOTE: Offset is ignored.
	call	vrelse_			/
	add	sp, $4			/

	pop	bp			/ Standard return.
	pop	di
	pop	si
	ret

////////
/
/ Block copy chunks of memory to a physical
/ location from a location in either the system
/ or user data space.
/
/ upcopy(u, p, n)
/ char *u;
/ paddr_t p;
/ int n;
/
/ kpcopy(k, p, n);
/ char *k;
/ paddr_t p;
/ int n;
/
////////

	.globl	upcopy_

upcopy_:
	mov	bx, sp			/ Get set for stack index.
	mov	ax, 2(bx)		/ User address
	dec	ax			/ Don't wrap too soon
	add	ax, 8(bx)		/ Count
	jc	kuerr			/ Out of bounds
	cmp	ax, udl_		/ In range?
	ja	kuerr			/ No
	push	uds_			/ Mark transfer ds as being user ds.
	jmp	1f			/ Finish in common code.

	.globl	kpcopy_

kpcopy_:
	push	ds			/ Mark transfer ds as being kernel ds.

1:	push	si			/ Standard save
	push	di
	push	bp
	mov	bp, sp

	push	ds			/ Save ds, es
	push	es

	sub	ax, ax			/ ES:DI = ptov(p,n).
	push	ax			/
	push	16(bp)			/
	push	14(bp)			/
	push	12(bp)			/
	call	ptov_			/
	add	sp, $8			/
	mov	es, dx			/
	mov	di, ax			/
					/
	mov	ds, 6(bp)		/ DS:SI = source address.
	mov	si, 10(bp)		/
	mov	cx, 16(bp)		/ Byte count
					/
	cld				/ Auto Increment
	clc				/
	rcr	cx, $1			/ Word count
	rep				/
	movsw				/ Move words
	rcl	cx, $1			/ Move odd byte
	rep				/
	movsb				/
					/
	mov	ax, es			/ Remember mapped selector.
	pop	es			/ Restore es, ds.
	pop	ds			/
					/
	push	ax			/ Release mapped selector.
	push	ax			/ NOTE: Offset is ignored.
	call	vrelse_			/
	add	sp, $4			/
					/
	mov	ax, 16(bp)		/ Return transfer count.
					/
	pop	bp			/ Standard return.
	pop	di
	pop	si
	add	sp, $2			/ Discard marked transfer ds.
	ret

////////
/
/ Block copy memory from physical address "p"
/ to either user or system data space.
/
/ pucopy(p, u, n)
/ paddr_t p;
/ char *u;
/ int n;
/
/ pkcopy(p, k, n);
/ paddr_t p;
/ char *k;
/ int n;
/
////////

	.globl	pucopy_

pucopy_:
	mov	bx, sp			/ Stack index
	mov	ax, 6(bx)		/ User address
	dec	ax			/ Don't wrap too soon
	add	ax, 8(bx)		/ Count
	jc	kuerr			/ Out of bounds
	cmp	ax, udl_		/ In range?
	ja	kuerr			/ No
	push	uds_			/ Mark transfer es as being user ds.
	jmp	1f			/ Common code

	.globl	pkcopy_

pkcopy_:
	push	ds			/ Mark transfer es as being kernel ds.

1:	push	si			/ Standard save
	push	di
	push	bp
	mov	bp, sp

	push	ds			/ Save ds, es.
	push	es

	sub	ax, ax			/ DS:SI = ptov(p,n).
	push	ax
	push	16(bp)
	push	12(bp)
	push	10(bp)
	call	ptov_
	add	sp, $8
	mov	ds, dx
	mov	si, ax

	mov	es, 6(bp)		/ ES:DI = destination.
	mov	di, 14(bp)		/
	mov	cx, 16(bp)		/ Count

	cld				/ Incremental move
	clc				/
	rcr	cx, $1			/ Word count
	rep				/
	movsw				/ Move words.
	rcl	cx, $1			/ Move odd byte.
	rep				/
	movsb				/
					/
	mov	ax, ds			/ Rememember mapped selector.
	pop	es			/ Restore es, ds.
	pop	ds			/
					/
	push	ax			/ Release mapped selector.
	push	ax			/ NOTE: Offset is ignored.
	call	vrelse_			/
	add	sp, $4			/
					/
	mov	ax, 16(bp)		/ Return transfer count.
					/
	pop	bp			/ Restore registers.
	pop	di			/
	pop	si			/
	add	sp, $2			/ Discard marked transfer es.
	ret				/ Fin

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
/       Read a byte from the CMOS.  Takes one argument--the
/       CMOS address to read from as an int; returns the
/       value read in ax.
/
/	int read_cmos(addr)
/	int addr;
/
////////
        .globl	read_cmos_
read_cmos_:
        push    bp
        mov     bp, sp
        movb    al, 4(bp)	/ Fetch address from stack.
        outb    CMOSA, al	/ Send address to CMOS.
        jmp     .+2             / DELAY
	sub	ax, ax		/ Zero out everything we don't want.
        inb     al, CMOSD	/ Get Value from CMOS into al.
        pop     bp
        ret                     / Return from read_cmos().


/ Read timer channel 0 into int value.  
/ Clock counts down from 11932 to 0 with each clock tick.
	.globl	read_t0_
read_t0_:
	pushf
	cli
	xor	ax,ax	/ Counter latch timer 0 and clear return val
	outb	PIT+3,al
	jmp	.+2		/ DELAY /
	jmp	.+2		/ DELAY /
	inb	al,PIT		/ low byte of counter latch
	movb	ah,al
	jmp	.+2		/ DELAY /
	jmp	.+2		/ DELAY /
	inb	al,PIT		/ high byte of counter latch
	xchgb	al,ah
	jmp	.+2		/ DELAY	/
	jmp	.+2		/ DELAY	/
	popf
	ret

/ return current contents of psw
	.globl	read_psw_
read_psw_:
	pushf
	pop	ax
	ret

////////
/
/ Read the equipment description. Use
/ the "int 11" interface, so that the IBM
/ ROM will do all the details.
/
////////

	.globl	int11_

int11_:	mov	ax, cs:val11		/ Ask the ROM
	ret				/ to put stuff in AX

////////
/
/ Bootstrap.
/ Called by the keyboard driver on control-alt-del.
/ Requests the 8042 controller to initiate a processor reset,
/ which is the only way to terminate protected mode operation.
/
/	Reference: IBM-AT Technical Reference Manual,
/			Real-time Clock/CMOS RAM [Page 1-45]
/			Keyboard controller [Page 1-40]
/			Test 3, Page 5-68.
/
////////

	.globl	boot_
boot_:
	cli				/ Disable interrupts.
					/
	sub	cx, cx			/
0:	inb	al, KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	al, $2			/
	loopne	0b			/
	jmp	.+2		/ DELAY /
					/
	movb	al, $0xFE		/ Issue a shutdown command
	outb	KBCTRL, al		/ to the 8042 control port.
					/
0:	hlt				/ Halt until processor reset occurs.
	jmp	0b

////////
/
/ Data.
/
////////

	.globl	MAXMEM
	.globl	vecs_
	.globl	realmode_
	.globl	gdtsel_, gdtmap_
	.globl	idtsel_, idtmap_
	.globl	CMOSmax_

	.shri
val11:	.word	0			/ Value obtained from int11 [in code].

	.prvd
MAXMEM:	.word	0xA000			/ In paragraphs, must be mult. of 64
CMOSmax_:.word	0x0000			/ Max extended memory according ...
					/ ... to CMOS bytes 0x17 and 0x18 ...
					/ ... in 64K chunks.
realmode_:.word	0			/ Virtual Addressing Mode Enabled
gdtmap_:.blkw	3			/ Global descriptor table definition
idtmap_:.blkw	3			/ Interrupt descriptor table definition
gdtsel_:.word	0			/ Global descriptor table selector
idtsel_:.word	0			/ Interrupt descriptor table selector
vecs_:	.word	vret_			/ Interrupt vector table
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_
	.word	vret_

////////
/
/ Task State Segment - Coherent runs as a single protected mode task.
/
////////
	.globl	tss_

	.prvd
tss_:				/ Task State Segment.
tss_lnk:.word	0		/  0: Back link selector to TSS.
tss_sp0:.word	0		/  2: SP for CPL 0.
tss_ss0:.word	0		/  4: SS for CPL 0.
tss_sp1:.word	0		/  6: SP for CPL 1.
tss_ss1:.word	0		/  8: SS for CPL 1.
tss_sp2:.word	0		/ 10: SP for CPL 2.
tss_ss2:.word	0		/ 12: SS for CPL 2.
tss_ip:	.word	0		/ 14: IP (Entry point).
tss_psw:.word	0		/ 16: Flag word.
tss_ax:	.word	0		/ 18: Register AX.
tss_cx:	.word	0		/ 20: Register CX.
tss_dx:	.word	0		/ 22: Register DX.
tss_bx:	.word	0		/ 24: Register BX.
tss_bp:	.word	0		/ 26: Register BP.
tss_sp:	.word	0		/ 28: Register SP.
tss_si:	.word	0		/ 30: Register SI.
tss_di:	.word	0		/ 32: Register DI.
tss_es:	.word	0		/ 34: Register ES.
tss_cs:	.word	0		/ 36: Register CS.
tss_ss:	.word	0		/ 38: Register SS.
tss_ds:	.word	0		/ 40: Register DS.
tss_ldt:.word	0		/ 42: Task LDT Selector.
