/ $Header: /newbits/conf/boot/RCS/boot.m,v 1.4 91/11/08 13:18:38 bin Exp Locker: bin $
/ (lgl-
/	The information contained herein is a trade secret of Mark Williams
/	Company, and  is confidential information.  It is provided  under a
/	license agreement,  and may be  copied or disclosed  only under the
/	terms of  that agreement.  Any  reproduction or disclosure  of this
/	material without the express written authorization of Mark Williams
/	Company or persuant to the license agreement is unlawful.
/
/	COHERENT Version 3.2.1
/	Copyright (c) 1982, 1983, 1984, 1991.
/	An unpublished work by Mark Williams Company, Chicago.
/	All rights reserved.
/ -lgl)

////////
/
/ Bootstrap block for the IBM
/ PC. This code, which lives in sector 1,
/ track 0 of the floppy (or sector 1 of a hard disk partition)
/ gets read into location 0x7C00 by the ROM.
/ The code relocates itself to location 0x2060:0x0000
/ (128K above the fixed memory used by the ROM). The system image is read
/ in beginning at paragraph 0x0060.
/
/ $Log:	boot.m,v $
/ * Revision 1.4  91/11/08  13:18:38  bin
/ * Piggy update for building secondary boot for tboot
/ * 
/  Revision 1.7  91/11/08  11:31:13  piggy
/  Attempted to integrate pboot into these sources.  Still not working.
/  Updated secondary boot for use with tertiary boot code.
/  
/  Revision 1.4  91/09/17  10:35:59  piggy
/  Uses 4 byte disk blocks, so can boot from anywhere on disk.
/  Points ds:si at useful data for tertiary boot.
/  Loads /tboot by default.
/  
/ Revision 1.1	88/03/24  16:44:09	src
/ Initial revision
/ 
/ 87/10/04	Allan Cornish		/usr/src/sys/i8086/boot/boot.s
/ Number of heads now defined by NHD macro.  Should be 1 for f*d*, 2 for f*a*.
/
/ 85/10/04	Allan Cornish		/usr/src/sys/i8086/boot/boot.s
/ Bootstrap extended to increase max file size from 64 Kbytes to 128 Kbytes.
/ The private code segment is now loaded, where previously it had to be empty.
/ File name editing enhanced to help cursor stay in the 14 char entry field,
/ and to provide a destructive backspace.
/
/ 85/01/04	Allan Cornish
/ Added backspace editing of file name.
/
////////

#ifndef	NHD
#define	NHD	1			/ num of heads per drive [1 for f9d0].
#endif

#ifndef	NSPT
#define	NSPT	9			/ num of sectors per track on floppy.
#define	NTRK	40			/ num of tracks on floppy.
#endif


#ifndef	CNTRL
#define	CNTRL	8
#endif

#ifndef	WPCC
#define	WPCC	0xFFFF
#endif

NSTK	=	256			/ num of bytes of stack.
BS	=	0x08			/ BS character.
CR	=	0x0D			/ CR character.
LF	=	0x0A			/ LF character.
SP	=	0x20			/ Backspace character.
BOOTLC	=	0x7C00			/ Boot location (ROM).
BOOTS	=	0			/ boot segment (ROM).
RBOOTS	=	0x2060			/ Relocated boot segment.
BSHIFT	=	9			/ Shift, bytes to blocks.
ROOTINO =	2			/ Root inode #
INOORG	=	2			/ First inode block.
IBSHIFT =	3			/ Shift, inode to blocks
IOSHIFT =	6			/ Shift, inode to bytes
INOMASK =	0x0007			/ Mask, inode to offset
DIRSIZE =	14			/ Directory size.
DIRSIZ2 =	7			/ Directory size / 2.
BUFSIZE =	512			/ Block size.
HDRSIZE =	44			/ L.out header size.
HDRSIZ2 =	22			/ L.out header size / 2.
BUFMSK2 =	0x00FF			/ Block [word] mask, for reads.
DISK	=	0x13			/ Disk Interrupt
KEYBD	=	0x16			/ Keyboard Interrupt
READ1	=	0x0201			/ read one sector
DADDR	=	4			/ number of bytes in a disk block number
ND	=	10			/ num of direct blocks.
NIND	=	128			/ num of blocks in indirect block.
NINDX2	=	256			/ NIND * 2 (number of words in indirect)
ISIZE	=	8			/ Offset of "di_size".
IADDR	=	12			/ Offset of "di_addr".
JMPF	=	0xEA			/ Jump far, direct.
LMAGIC	=	0			/ Offset of "l_magic"
LFLAG	=	2			/ Offset of "l_flag"
LSHRI	=	10			/ Offset of "l_ssize[SHRI]"
LPRVI	=	14			/ Offset of "l_ssize[PRVI]"
LSHRD	=	22			/ Offset of "l_ssize[SHRD]"
LPRVD	=	26			/ Offset of "l_ssize[PRVD]"
LBSSD	=	30			/ Offset of "l_ssize[BSSD]"
LFMAG	=	0x0107			/ Magic number.
LFSEP	=	0x02			/ Sep I/D flag bit.
SYSBASE =	0x0060			/ System load base paragraph.
FIRST	=	8			/ relative start of partition

/ Hard disk controller port addresses.
HF_REG	=	0x3F6
NSEC_REG=	0x1F2
SEC_REG	=	0x1F3
HDRV_REG=	0x1F6
CSR_REG	=	0x1F7
BSY_ST	=	0x80
SETPARM_CMD=	0x91

////////
/ Overall structure of boot.s (the secondary boot):
/
/ boot: relocate secondary to high memory (0x2060:0x0000)
/	if needed, get hard disk parameters from bios
/	jump far to high memory (RBOOTS:entry)
/ entry: open directory "/"
/ search: while filename not found
/	walk through "/" looking for filename (in nbuf)
/	if not found, ask for another name (call input:), and loop to search
/ found: open file that we just found
/	read the l.out header (verify magic number, get partition sizes)
/	load the shared and private code segments as one (call load:)
/	load the initialized data segment (call load:)
/	jump far to the newly loaded program
/	
////////

////////
/
/ Bootstrap mainline. Relocate the
/ boot to high memory, so it will not be written
/ over by the system. Read in a file name. Look up
/ the file name by following out the directory
/ structure. Read in the image and jump to it.
/
////////

boot:	xor	di, di
	mov	ds, di
	mov	si, $BOOTLC		/ Make DS:SI point at the code
/ "mov es, 1f(si)" really means "mov es, $RBOOTS".  Clever Intel... :-(
	mov	es, 1f(si)		/ Make ES:DI point at where it goes
	mov	cx, $512		/ cx = # of bytes to move
	cld
	rep				/ Move the bootstrap
	movsb				/ to high memory.

/ Check to see if we were invoked by mboot.  (That means we are booting
/ off a hard disk and need to load disk parameters.  )

					/ mboot sets bx = sp
	cmp	bx, sp			/ trick to tell
	jne	0f			/ whether or not xt

/ Assertion: at this point we know we are booting off a hard disk.
	mov	si, bp			/ set si to partition table

	movb	dl, (si)		/ get drive number
	movb	ah, $8			/ get drive parameters
	int	DISK
	jc	0f			/ abort on error

	movb	al, ch			/ fetch cyl(lo)
	movb	ah, cl			/ move cyl(hi), sects
	rolb	ah, $1			/ shift cylinder high to
	rolb	ah, $1			/ the least sig bits
	andb	ah, $3			/ mask out cylinder bits
	
	mov	di, $traks		/ point to drive
	stosw				/ set number of tracks

	movb	al, $0x3F		/ sector mask
	andb	al,cl			/ mask sector
	stosb				/ set sector

	movb	al, dh			/ get max head
	inc	ax			/ change to # of heads [incb al]
	stosb				/ set number of heads

	movsb				/ set drive
	add	si, $FIRST-1		/ point to first block
	movsw
	movsw				/ fetch first block

/ Set registers.

0:	mov	bp, $stack+NSTK 	/ stack pointer value
	mov	ax, es
	mov	ds, ax
	mov	ss, ax
	mov	sp, bp

/ Jump to (relocated) boot.

	.byte	JMPF
	.word	entry
1:	.word	RBOOTS

////////
/
/ This is the equivalent of "open()",
/ but takes an inode instead of a pathname.
/
/ Read the inode specified by "ax"
/ into the external variable "iaddr".
/
/ Makes a list of block numbers in "iaddr"
/ consisting of the 10 direct block numbers, the 128
/ block numbers pointed to the singly indirect block,
/ and the first 128 block numbers pointed to by the
/ first doubly indirect block.
/
/ Note that "iread" can only access the first 256
/ block numbers in this list, so the last 10 are
/ ignored.
/
/ Note that block numbers are actually 32 bits on disk--
/ this routine only uses the lower 16 bits.
/ In fact, iaddr is loaded with 16 bit numbers, not 32 bit.
/
/ No checking is done to make sure that
/ the inumber is in range on the filesystem.
/ Sets dx and cx to 0.
/ Mungs si, di, ax.
/
////////

igrab:	dec	ax			/ Make origin 0 and
	push	ax			/ remember for later use.

	/ We assume that the inode table appears in the first 32 meg of disk.
	/ Look up the inode in the inode table.
	movb	cl, $IBSHIFT		/ Convert to
	shr	ax, cl			/ inode block number,
	inc	ax			/ then to physical block number,
	inc	ax
	sub	bx, bx			/ Assume inode within first 32 meg.
	call	bread			/ and read in the data.

	pop	ax			/ Get i-number back.
	and	ax, $INOMASK		/ Get inode within block
	movb	cl, $IOSHIFT		/ and convert to
	shl	ax, cl			/ a byte offset in the block.
	add	si, ax			/ si = inode pointer.

	/ Assertion: si now points at the start of the desired
	/ inode structure (see <sys/inode.h>).

	/ Copy out the direct block numbers to iaddr.
	add	si, $IADDR		/ Point at address field.
	mov	di, $iaddr		/ Copy out area.

	/ The block numbers in the inode on disk are stored as
	/ 3 bytes: msb, lsw.  lsw is lsb, msb.  We want to store them
	/ in memory as msw, lsw, just like the inodes in the indirect
	/ blocks on disk.  So we need to cast msb as msw.
	movb	cl, $ND			/ cx = # of direct blocks
	sub	ax, ax			/ cast byte in al to word in ax
0:	lodsb				/ al = msb for direct byte
	stosw				/ store msb cast as a word
	movsw				/ store lsw.
	loop	0b

	lodsb				/ Get the msb for the indirect block.
	xchg	bx, ax			/ (implicit cast of msb to msw)
	lodsw				/ Get the lsw for the indirect block.
	call	bread			/ Read (first) indirect block.

	/ Copy out the singly indirect block numbers to iaddr.
	/ These are stored on disk as 4 byte numbers: msw, lsw
	/ where each word is lsb, msb.  This is how we store them
	/ in memory.
	mov	cx, $NINDX2
	rep
	movsw


	/ Finish by reading the first block of the file into bbuf.
	sub	dx, dx			/ Set dh to first block number in iaddr.
/	jmp	iread			/ Done return through iread.

////////
/
/ This routine reads the virtual
/ block iaddr[dh] (the dhth block number
/ in iaddr) into the buffer "bbuf".
/ It uses the mapping data that
/ has been provided by a previous call to igrab
/ On return "si" points at "bbuf".
/ Holes in files (sparse blocks) are correctly done.
/ mungs ax, bx, si.
/ clears cx.
/
///////

iread:
	sub	bx, bx			/ Convert from words to blocks.
	movb	bl, dh			/

	/ Block numbers are 4 bytes long, stored msw, lsw.
	shl	bx, $2			/ Compute index into iaddr table.
	push	iaddr(bx)		/ fetch msw
	inc	bx
	inc	bx
	mov	ax, iaddr(bx)		/ fetch lsw
	pop	bx			/ put msw where it belongs
/	jmp	bread			/ Yes, return through "bread".

////////
/
/ Read a block from the floppy disk,
/ drive A:, using the code in the IBM firmware.
/ The physical block # is in ax and bx.
/ Register bx is the MSW, and ax is the LSW.
/
////////

bread:	push	es			/ Save registers
	push	di
	push	dx

	push	ds
	pop	es			/ set ES to the address of the buffer

	mov	di, bp			/ Blast the buffer contents.
	mov	cx, $BUFSIZE		/ For block 0, this fills the buffer
	rep				/ with zeros.
	stosb

	/ Block #0 is the sparse block--it means a block of all zeros.
	test	ax, ax			/ if block 0, return zeroed buffer
	jnz	3f
	test	bx, bx
	jz	2f

	/ Translate block number into cylinder, head, and sector.
3:	push	bx
	pop	dx
/	xor	dx, dx			/ extend block number
	add	ax, first		/ add first block
	adc	dx, first+2		/ add rest

	mov	bx, ax			/ save block number
	movb	al, heads		/ get number of heads
	movb	cl, sects		/ get number of sectors
	mulb	cl			/ calculate sectors per cylinder
	xchg	bx,ax			/ swap block/sectors
	div	bx			/ calculate track
	xchg	dx, ax			/ put track in DX
	divb	cl			/ calculate head/sector

	movb	cl, ah			/ set sector
	inc	cx			/ sectors start at 1 [incb cl]
	
	cmp	dx, traks		/ check for second side
	jb	0f
	sub	dx, traks		/ fold track
	inc	ax			/ next head [incb al]

0:	rorb	dh, $1			/ rotate track(low) into
	rorb	dh, $1			/  msbits of DX
	orb	cl, dh			/ set track(high)
	movb	ch, dl			/ set track(low)
	movb	dh, al			/ set head
	movb	dl, drive		/ set drive
	mov	bx, bp			/ set offset [bbuf]

	mov	ax, $READ1		/ Read, 1 sector.
	int	DISK			/ Disk I/O.
	jnc	2f			/ Jump if no error.
	mov	ax, $READ1		/ try again
	int	DISK
	jc	error

2:	mov	si, bp			/ set SI to point to buffer for return
	sub	cx, cx			/ clear CX here to save space

	pop	dx			/ restore registers.
	pop	di
	pop	es
	ret				/ return.

/ Prompt with a "?" and read the file name
/ into "nbuf".  No character editing facilites
/ are provided.

error:	mov	sp, bp			/ Reset stack

input:	mov	di, $nbuf		/ di=name buffer pointer
	mov	cx, $DIRSIZE		/ cx=size of
/ Preprocessor blows up if we use single-quote question mark.
	movb	al, $0077		/ set prompt to ?.
0:	call	putc

1:	movb	ah, $0			/ Get ASCII opcode.
	int	KEYBD			/ Read keyboard ROM call.

2:	cmpb	al, $CR 		/ CR ?
	je	3f			/ Yup, do next thing

	jcxz	1b			/ skip over too big name
	stosb				/ put it into the buffer,
	dec	cx			/ adjust char count
	jmp	0b			/ and continue.

3:	movb	al, $CR
	call	putc
	movb	al, $LF
	call	putc

	movb	al, $0			/ zero out
	rep
	stosb				/ rest of name

////////
/ This is where we jump after boot has been relocated.
/ 
/ Open "/"; we are going to search for "autoboot".
////////
entry:	mov	ax, $ROOTINO		/ ax = current inode
	call	igrab			/ Read in inode and set dx

////////
/ Search directory for filename in nbuf.
/ Assume directory size = 64 Kbytes.
/ This assumption works because block numbers
/ from the inode beyond end of file are zero.
////////

search: orb	dl, dl			/ On block boundry ?
	jnz	0f			/ Nope.
	call	iread			/ Read block, set si.

0:	movb	cl, $DIRSIZE		/ cx = count left
	mov	di, $nbuf		/ Point at name buffer and
	lodsw				/ ax = inumber, this entry.
	or	ax, ax			/ Empty directory slot ?
	je	1f			/ Yes, skip.
	repe				/ compare the
	cmpsb				/ two file names.
	je	found			/ If eq, go and get inode.

1:	add	si, cx			/ Advance by count remaining.
	add	dx, $DIRSIZ2+1		/ Bump [word] seek pointer
	jg	search			/ Scan up to 64 Kbytes of directory.

	jmp	input			/ File not found.


////////
/
/ Found the file so read it in.
/ ax = inode number of file
/
////////

found:	call	igrab			/ read in inode and first block
	cmp	LMAGIC(si),$LFMAG	/ Check the magic number.
	jne	error			/ not Ok.

/	push	LBSSD(si)		/ Push the uninitialized data size.

	/ Get the total size of shared and private data segments.
	/ They are contigious on disk.  see <l.out.h>
	mov	ax, LSHRD(si)		/ Push the sum
	add	ax, LPRVD(si)		/ of the shared and private
	push	ax			/ [initialized] data sizes.
	/ If this is not a sep I/D executable, then we
	/ do not want to load the non-existent data segment.
	andb	LFLAG(si), $LFSEP	/ check image flags.
	pushf				/ Push result of test.
	mov	ax, LSHRI(si)		/ Get the sum of the
	add	ax, LPRVI(si)		/ shared and private code sizes.

	/ Find the start of the first segment on disk.
	mov	dx, $HDRSIZ2		/ Seek after header and
	add	si, $HDRSIZE		/ set buffer pointer appropriately.
	mov	es, 1f			/ Set ES:DI to point to the load
	sub	di, di			/ base of the new system.

	/ Load the code segment.
	call	load			/ Load code.

	/ Skip loading the data segment for non-sep I/D executables.
	popf				/ Pop sep I/D flag test
	jz	0f			/ Not sep.

	/ Assertion: at this point we are dealing with a sep I/D
	/ l.out executable.

	/ Calculate load point for data segment.
	add	di, $15 		/ Round up the system code
	movb	cl, $4			/ size to 16 byte
	shr	di, cl			/ paragraphs.
	mov	ax, es			/ fetch program base
	add	ax, di			/ Compute the data base and
	mov	es, ax			/ set up ES:DI to point
	sub	di, di			/ at it.

0:	pop	ax			/ Pop off initialized data size and
	/ Load the initialized data segment.
	call	load			/ load the image.

	mov	si, $useful		/ Point ds:si at the useful data
					/ for the tertiary boot.

/	pop	cx			/ Pop off uninitialized data size and
/	rep				/ clear it.
/	stosb

	/ Run the program that we just loaded.
	.byte	JMPF			/ Jump to offset
	.word	0x0100			/ 0x0100 (after base) in system
1:	.word	SYSBASE 		/ code segment.

////////
/
/ Load a segment from an l.out file into memory.
/ The "dx" register holds the
/ seek pointer into the file. The "si" holds
/ a pointer into the "bbuf". The "es:di" pair
/ holds the target address. The "ax" holds
/ the number of bytes to load. On return the
/ "ax" must equal 0 (the caller assumes this
/ and uses "al" to clear the BSS).
/
/ Note that since the number of bytes to load is
/ stored in a 16 bit register, a segment may be no
/ longer than 64K.  For small model this is just fine.
////////

load:	or	ax, ax			/ Any left ?
	jz	return			/ Jump if all loaded.

	mov	cx, $bbuf+BUFSIZE	/ Compute the number of bytes
	sub	cx, si			/ remaining in the block.
	jnz	0f			/ Jump if some.

	push	ax			/ Save count registers, then
	call	iread			/ read the next block
	pop	ax			/ of the file.
	mov	cx, $BUFSIZE		/ We now have a full block.

0:	cmp	cx, ax			/ More than we need ?
	jbe	1f			/ Nope.
	mov	cx, ax			/ Only take what we need.

1:	sub	ax, cx			/ Fix up the count
	shr	cx, $1			/
	add	dx, cx			/ Fix up the seek [word] address, then
	rep				/ copy the words from the block
	movsw				/ buffer to the load point and
	jmp	load			/ loop until done.

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

putc:	push	si			/ Save registers.
	push	di
	push	bp

	mov	bx, $0x0007		/ Page 0, white on black
	movb	ah, $0x0E		/ Write TTY.
	int	0x10			/ Call video I/O in ROM.

	pop	bp			/ Restore registers.
	pop	di
	pop	si
return: ret				/ Return

////////
/
/ Data. There is some pure data in
/ the "prvd" segment. This data is moved to the
/ new memory when the boot is relocated. All buffers
/ are in the BSS, and are not actually moved, or even
/ saved in the boot block.
/
////////

	.prvd

/ Do not rearrange ANY of the following variables.  If you
/ need to add variables, they must come BEFORE or AFTER these.
/ If you want to teach the tertiary boot about any variables you
/ add, put them after double word "first".


	.even
useful:	/ Useful variables for the tertiary boot.
nbuf:	.ascii	"tboot"			/ Name buffer.
	.blkb	DIRSIZE-5		/ rest of buffer (5=sizeof("tboot"))

traks:	.word	NTRK	/ Number of cylinders on drive we are booting off of.
sects:	.byte	NSPT	/ Number of sectors per track for our drive.
heads:	.byte	NHD	/ Number of heads on drive we are booting off of.

drive:	.byte	0	/ Drive our partition resides upon.
first:	.word	0	/ First block of our partition (?)
	.word	0
cntrl:	.byte	CNTRL	/ Control byte
wpcc:	.word	WPCC	/ 

/ This magic pair of bytes must be the last two bytes of
/ the sector (address 0x1FE), otherwise mboot will refuse
/ to execute it.
/ If needed, uncomment the .blkb and adjust the number appropriately.

	.blkb	0x28	/ Padding needed to make magic byte line up

	.byte	0x55,0xAA

	.bssd
stack:	.blkb	NSTK			/ Local Stack and name buffer
bbuf:	.blkb	BUFSIZE 		/ Block buffer [must follow stack].
iaddr:	.blkw	ND+ND+NIND+NIND		/ Inode map words.
