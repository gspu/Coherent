////////
/
/ Master Boot Block
/
/	To be installed in first block of hard disk drives.
/	Supports 4 logical partitions per drive, with the
/	info layout specified by /usr/include/sys/fdisk.h.
/
/	Will execute the keyboard selected or active partition.
/	Keyboard chars 0-3 will select partitions 0, 1, 2, or 3 on 1st drive.
/	Keyboard chars 4-7 will select partitions 0, 1, 2, or 3 on 2nd drive.
/	In the event of an empty partition, a read error, or
/	invalid partition signature, prompts for new partition.
/
/	Partition parameters can be modified by fdisk(8).
/
/	Author: Allan Cornish, INETCO Systems Ltd, April 23 1985
/
/ $Log:	/newbits/conf/boot/RCS/mboot.s,v $
/ Revision 1.1	91/05/29  08:30:28 	bin
/ Initial revision
/ 
/ Revision 1.1	88/03/24  16:44:18	src
/ Initial revision
/ 
/ 86/12/11	Allan Cornish		/usr/src/sys/i8086/boot/mboot.s
/ Diagnostic messages modified.
/
/ 86/12/03	Allan Cornish		/usr/src/sys/i8086/boot/mboot.s
/ Extended to support booting on either hard disk.
/
////////

////////
/
/ Offsets within partition structure to data fields.
/
///////
	P_BOOT	= 0
	P_BHD	= 1
	P_BSEC	= 2
	P_BCYL	= 3
	P_SYS	= 4
	P_EHD	= 5
	P_ESEC	= 6
	P_ECYL	= 7
	P_BASE	= 8
	P_SIZE	= 12

////////
/
/ Magic constants.
/
////////

	VIDEO	= 0x10			/ video swi
	DISK	= 0x13			/ disk io swi
	KEYBD	= 0x16			/ keyboard swi
	READ1	= 0x0201		/ read 1 sector
	BOOTLC	= 0x7C00		/ boot location
	RBOOTLC	= 0x0600		/ relocated boot location
	JMPF	= 0xEA			/ jump far, direct

////////
/
/ Master boot block startup code.
/
/	Action:	Setup segmentation registers.
/		Generate a 'INETCO Systems' message on console.
/		Copy boot block to new location.
/		Transfer control to relocated boot block.
/
////////

	cli
	sub	ax, ax
	mov	es, ax
	mov	ds, ax
	mov	ss, ax
	mov	sp, $BOOTLC
	sti
	cld

	mov	si, $m_hello+BOOTLC
	call	puts

	mov	si, sp
	mov	di, $RBOOTLC
	mov	cx, $512
	rep
	movsb

	.byte	JMPF
	.word	entry+RBOOTLC
	.word	0

////////
/
/ entry
/
/	Action:	If input character is available from keyboard,
/			transfer control to partition selection routine.
/		Otherwise, transfer control to partition scan routine.
/
////////

entry:	movb	ah, $1			/ entry:
	int	KEYBD			/	if ( iskey() )
	jne	select			/		goto select;
	call	scan			/	scan();
	incb	m_drvnum+RBOOTLC	/	drvnum++;
	call	scan			/	scan();
	jmp	resel			/	goto resel;

////////
/
/ warn
/
/	Input:	SI = null terminated error message.
/
/	Action:	Output error message.
/		Tranfer control to partition reselection routine.
/
////////

warn:					/ warn:
	mov	sp, $BOOTLC		/
	call	puts			/	puts( s );
//	jmp	resel			/	goto resel;

////////
/
/ resel
/
/	Action:	Prompt for user input of partition #.
/		Wait until keyboard data is available.
/		Transfer control to partition selection routine.
/
////////

resel:					/ resel:
	mov	si, $m_select+RBOOTLC	/	puts("Select partn ... ? ");
	call	puts			/
					/
0:	movb	ah, $1			/	while (! iskey() )
	int	KEYBD			/		;
	je	0b			/
					/
	call	crnl			/	puts("\r\n");
//	jmp	select			/	goto select;

////////
/
/ select
/
/	Action:	Read character from keyboard.
/		Use chars 0..3 as index into partition table.
/		Select drive and partition.
/		Initiate read of selected drive's master boot block.
/
////////

select:					/ select:
	movb	ah, $0			/	ch = nextkey();
	int	KEYBD			/
					/
	movb	m_partnum+RBOOTLC, al	/	partnum = ch;
	movb	m_drvnum+RBOOTLC, $'1	/	drvnum  = '1';
					/
	subb	al, $'0			/	if ( (ch -= '0') < 0 )
	jl	resel			/		goto resel;
	cmpb	al, $7			/	if (ch > 7)
	jg	resel			/		goto resel;
					/
	cmpb	al, $4			/	if ( ch < 4 )
	sbbb	m_drvnum+RBOOTLC, $0	/		drvnum--;
					/
	andb	al, $~4			/	ch &= ~4;
					/
	cbw				/	pp = &partitions[ch];
	movb	cl, $4			/
	shl	ax, cl			/
	add	ax, $partitions+RBOOTLC	/
	mov	di, ax			/
					/
	call	rdinfo			/	rdinfo();
					/
//	jmp	rdboot			/	goto rdboot;

////////
/
/ rdboot - read partition boot block
/
/	Action:	Validate partition table.
/		Ensure booting partition exists.
/		Read the partition boot block into memory.
/
////////

rdboot:					/ rdboot:
	mov	si, $m_partn+RBOOTLC	/
	call	puts			/	puts("Partition #n");
					/
	mov	si, $m_empty+RBOOTLC	/	if (pp->p_size == 0) {
	mov	ax, P_SIZE(di)		/		s = " is empty\n";
	or	ax, P_SIZE+2(di)	/		goto warn;
	je	warn			/	}
					/
	movb	al, m_drvnum+RBOOTLC	/	pp->p_boot = drvnum-'0' + 0x80;
	addb	al, $0x80-0x30		/
	movb	P_BOOT(di), al		/
					/
	mov	cx, P_BSEC(di)		/	p_bsec -> cl, p_bcyl -> ch
	mov	dx, P_BOOT(di)		/	p_boot -> dl, p_bhd  -> dh
	call	rdblock			/	rdblock();
					/
//	jmp	exec			/	goto exec;

////////
/
/ exec()
/
/	Action:	Validate partition boot block at BOOTLC.
/		If valid, transfer control to partition boot block.
/
////////

exec:	call	crnl			/	puts("\r\n");
					/
	mov	bx, sp			/
	mov	cx, P_BSEC(di)		/
	mov	dx, P_BOOT(di)		/
	mov	si, di			/
	mov	bp, di			/
	jmp	BOOTLC-RBOOTLC		/ jump relative to new boot block

////////
/
/ scan
/	Action:	Read partition table from current drive.
/		Scan partition table for active partition.
/		If found, transfer control to rdboot routine.
/		Otherwise, allow partition reselection.
/
////////

scan:					/ scan:
	call	rdinfo			/	rdinfo();
					/
	mov	di, $partitions+RBOOTLC	/	pp = &partitions[0];
	mov	cx, $4			/	n  = 4;
					/
0:	testb	P_BOOT(di), $0x80	/	do {	if (pp->b_boot & 0x80)
	jne	rdboot			/			goto rdboot;
	add	di, $0x10		/		pp++;
	incb	m_partnum+RBOOTLC	/		partnum++;
	loop	0b			/	} while (--n != 0);
					/
	ret				/	return;

////////
/
/ rdblock - read disk block
/
/	Input:	CX/DX = sector/drive information
/
/	Action:	Read block from disk.
/		Reset drive and report error on read failure.
/		Report error if block lacks boot signature.
/
////////

rdblock:				/ rdblock:
	mov	ax, $READ1		/
	mov	bx, $BOOTLC		/
	int	DISK			/	READ FROM DISK
					/
	jc	rderr			/	if ( read ok ) {
	cmp	BOOTLC+510, $0xAA55	/		if ( bad signature ) {
	mov	si, $m_missing+RBOOTLC	/			s = "has no..";
	jne	warn2			/			goto warn;
	ret				/		}
					/		return;
					/	}
					/
rderr:	sub	ax, ax			/	RESET HARD DISK
	int	DISK			/
	mov	si, $m_rderr+RBOOTLC	/	s = " can't be read\n";
warn2:	jmp	warn			/	goto warn;

////////
/
/ rdinfo - read partition information
/
/	Action:	Read master boot block from current drive.
/		Extract partition table.
/
////////

rdinfo:					/ rdinfo:
	mov	si, $m_drive+RBOOTLC	/	puts( "Drive X " );
	call	puts			/
					/
	mov	cx, $1			/	1st sector
	mov	dx, $0x0080-0x30	/	drive
	addb	dl, m_drvnum+RBOOTLC	/
	call	rdblock			/	read master boot block
					/
	push	di			/
	mov	si, $partitions+BOOTLC	/	copy partition table
	mov	di, $partitions+RBOOTLC	/
	mov	cx, $64			/
	rep				/
	movsb				/
	pop	di			/
					/
	mov	si, $m_invalid+RBOOTLC	/
	mov	bx, $partitions+RBOOTLC	/	pp = &partitions[0];
	movb	al, P_BOOT+0x00(bx)	/	ch = pp[0].p_boot;
	orb	al, P_BOOT+0x10(bx)	/	for ( n=1; n < 4; ++n )
	orb	al, P_BOOT+0x20(bx)	/		ch |= pp[n].p_boot;
	orb	al, P_BOOT+0x30(bx)	/
	andb	al, $0x7F		/	if ( ch & 0x7F )
	jne	warn2			/		goto warn;
					/
//	call	crnl			/	puts("\r\n");
//	ret				/	return;

////////
/
/ crnl - print carriage return followed by line feed.
/
////////

crnl:
	mov	si, $m_crnl+RBOOTLC	/	puts("\r\n");
//	call	puts			/
//	ret				/	return;
					/
	lodsb				/

////////
/
/ puts( s )
/ char * s;
/
/	Input:	SI = null terminated character string to output.
/
/	Output:	DI, SP, and segment registers are preserved.
/		All others may be destroyed.
/
////////

0:	movb	ah, $14
	mov	bx, $7
	push	si
	push	di
	int	VIDEO
	pop	di
	pop	si

puts:					/ puts(s)
	lodsb				/ {
	orb	al, al			/	while ( ch = *s++ )
	jne	0b			/		putc( ch );
	ret				/ }

////////
/
/ Text messages.
/
////////

m_hello:	.ascii	"Mark Williams "
m_crnl:		.ascii	"\r\n\000"

m_drive:	.ascii  "Drive "
m_drvnum:	.ascii	"0 \000"

m_partn:	.ascii	"Partition "
m_partnum:	.ascii	"0 \000"

m_select:	.ascii	"Select partition [0-7]\000"
m_invalid:	.ascii	"has bad partition table\r\n\n\000"
m_empty:	.ascii	"is empty\r\n\n\000"
m_missing:	.ascii	"has no operating system\r\n\n\000"
m_rderr:	.ascii	"can't be read\r\n\n\000"

		.blkb	6		/ filler
partitions:	.blkb	64		/ partition tables
		.word	0xAA55		/ master boot signature
