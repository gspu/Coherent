////////
/
/ Display XT/AT Disk Parameter Bootblock.
/ Copy to a boot block on hard disk partition, then reboot to display info.
/
////////

NSTK	=	256			/ # of bytes of stack.
CR	=	0x0D			/ CR character.
LF	=	0x0A			/ LF character.
BOOTLC	=	0x7C00			/ Boot location (ROM).
BOOTS	=	0			/ boot segment (ROM).
RBOOTS	=	0x2060			/ Relocated boot segment.
BSHIFT	=	9			/ Shift, bytes to blocks.
BUFSIZE =	512			/ Block size.
BUFMSK2 =	0x00FF			/ Block [word] mask, for reads.
DISK	=	0x13			/ Disk Interrupt
KEYBD	=	0x16			/ Keyboard Interrupt
READ1	=	0x0201			/ read one sector
JMPF	=	0xEA			/ Jump far, direct.
FIRST	=	8			/ relative start of partition

////////
/
/
////////

	.blkb	0x0100			/ PC-DOS base page.
entry:
	mov	ax, es
	mov	ds, ax

restart:
	call	login			/ print signon message
///
	xorb	bl, bl			/ zero the drive count
2:
	movb	drive, bl
	movb	dl, $0x80		/ get drive number
	addb	dl, bl
	movb	ah, $8			/ get drive parameters
	push	bx
	int	DISK

	push	dx
	movb	al, ch			/ fetch cyl(lo)
	movb	ah, cl			/ move cyl(hi), sects
	rolb	ah, $1			/ shift cylinder high to
	rolb	ah, $1			/ the least sig bits
	andb	ah, $3			/ mask out cylinder bits
	inc	ax
	mov	cyls, ax
	
	movb	al, $0x3F		/ sector mask
	andb	al,cl			/ mask sector
	movb	sects, al

	incb	dh			/ change to # of heads
	movb	heads, dh
///
	mov	si, $msgdrive
	call	print
	sub	ax, ax
	movb	al, drive
	call	putu
	mov	si, $msgcyls
	call	print
	mov	ax, cyls
	call	putu
	mov	si, $msgheads
	call	print
	sub	ax, ax
	movb	al, heads
	call	putu
	mov	si, $msgsects
	call	print
	sub	ax, ax
	movb	al, sects
	call	putu

	mov	si, $crlf
	call	print

	pop	dx		/ recall # of drives in dl
	pop	bx		/ recall # of previous drive in bl
	incb	bl
	cmpb	bl, dl		/ more drives to check?
	jb	2b
0:	
	movb	al, $'?		/ set prompt
	call	putc

	movb	ah, $0		/ Get ASCII opcode.
	int	KEYBD		/ Read keyboard ROM call.

	mov	si, $crlf	/ Echo LF followed by CR.
	call	print

	int	0x19		/ reboot, in real mode

////////
/
/ Print signon message
/
////////

login:	mov	si, $msg00		/ point si to message

print:	movb	al, (si)
	inc	si
	orb	al, al			/
	je	0f
	call	putc			/ Type it and
	jne	print			/ go back for more.
0:	ret

////////
/
/ Write the character in "al" out to
/ the display, using routines in the ROM.
/ Like most calls to the ROM, this routine spends
/ most of its time saving and restoring the
/ registers.
/
/ Note: The ZF in the PSW word must be preserved.
/
////////

putc:	push	si		/ Save registers.
	push	di
	push	bp
	pushf

	cmpb	lz, $1		/ zero suppress?
	jne	2f		/ skip if not
	cmpb	al, $'0
	je	3f		/ skip if we're suppressing a zero now
	movb	lz, $0		/ else turn off zero suppression and print char
2:
	mov	bx, $0x0007	/ Page 0, white on black
	movb	ah, $0x0E	/ Write TTY.
	int	0x10		/ Call video I/O in ROM.
3:
	popf
	pop	bp		/ Restore registers.
	pop	di
	pop	si
return: ret			/ Return

////////
/
/ Output the unsigned integer in AX.
/
////////

putu:				/ putu( i )
	push	si		/ int i;
	push	di		/
	push	bp		/

	sub	dx, dx		/
	movb	lz, $1		/ suppress leading zeroes
	mov	bx, $10000	/
	div	bx		/
	push	dx		/
	addb	al, $'0		/
	call	putc		/
	pop	ax		/

	sub	dx, dx		/
	mov	bx, $1000	/
	div	bx		/
	push	dx		/
	addb	al, $'0		/
	call	putc		/
	pop	ax		/

	sub	dx, dx		/
	mov	bx, $100	/
	div	bx		/
	push	dx		/
	addb	al, $'0		/
	call	putc		/
	pop	ax		/

	sub	dx, dx		/
	mov	bx, $10		/
	div	bx		/
	push	dx		/
	addb	al, $'0		/
	call	putc		/
	pop	ax		/

	movb	lz, $0		/ don't suppress final leading zero
	addb	al, $'0		/
	call	putc		/

	pop	bp		/
	pop	di
	pop	si
	ret

////////
/
/ Data. There is some pure data in
/ the "prvd" segment. This data is moved to the
/ new memory when the boot is relocated. All buffers
/ are in the BSS, and are not actually moved, or even
/ saved in the boot block.
/
////////

NHD = 1
NSPT = 9
NTRK = 40

	.prvd

cyls:	.word	0
sects:	.byte	0
heads:	.byte	0

drive:	.byte	0
first:	.word	0
	.word	0

lz:	.byte	0	/ nonzero if we are doing zero suppression

msgdrive:.ascii	"Drive "
	.byte	0
msgheads:.ascii "  Heads="
	.byte	0
msgcyls:.ascii ":  Cylinders="
	.byte	0
msgsects:.ascii	"  Sectors per track="
	.byte	0
msg00:
	.byte	CR, LF
	.ascii	"Here are the BIOS parameters for your hard drive(s)."
	.byte	CR, LF
	.ascii	"Write them down for use during installation."
	.byte	CR, LF
	.ascii	"Then press <Enter> to reboot."
	.byte	CR, LF
crlf:	.byte	CR, LF
	.byte	0
