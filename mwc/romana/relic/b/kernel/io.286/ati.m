/ (lgl-
/ 	COHERENT Driver Kit Version 1.0.0
/ 	Copyright (c) 1982, 1990 by Mark Williams Company.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
////////
/
/ Array Technologies Inc - Graphics Solution - Device Driver
/
/	 Supports 40/80/132 column color text
/		     80/132 column monochrome text
/
/ State driven code
/
/	Input:	DS:SI - input string
/		ES:DI - current screen location
/		SS:BP - terminal information
/		CX    - input count
/		BP    - references terminal information
/		AH    - character attributes
/		AL    - character
/		BH    - (usually) kept zeroed for efficiency
/		DH    - current row
/		DL    - current column
/
/ 
////////

	NCB	= 2		/ number of horizontal bytes per char
	NCR	= 1		/ number of horizontal lines per char
	NHB	= 160		/ number of horizontal bytes per line
	NRB	= NCR*NHB	/ number of bytes per character row

	ATTR	= ah		/ attribute byte
	ZERO	= bh		/ (almost) always zero
	ROW	= dh		/ currently active vertical position
	COL	= dl		/ currently active horizontal position
	POS	= di		/ currently active display address

	INTENSE	= 0x08		/ high intensity attribute bit
	BLINK	= 0x80		/ blinking attribute bit
	REVERSE	= 0x70		/ reverse video

////////
/
/ Magic constants from <sys/io.h>
/
////////

	IO_SEG	= 0
	IO_IOC	= 2
	IO_BASE	= 8

	IOSYS	= 0
	IOUSR	= 1

////////
/
/ Data
/
////////

MM_FUNC		= 0		/ current state
MM_PORT		= 2		/ adapter base i/o port
MM_BASE		= 4		/ adapter base memory address
MM_ROW		= 6		/ screen row
MM_COL		= 7		/ screen column
MM_POS		= 8		/ screen position
MM_ATTR		= 10		/ attributes
MM_N1		= 11		/ numeric argument 1
MM_N2		= 12		/ numeric argument 2
MM_BROW		= 13		/ base row
MM_EROW		= 14		/ end row
MM_LROW		= 15		/ legal row limit
MM_SROW		= 16		/ saved cursor row
MM_SCOL		= 17		/ saved cursor column
MM_IBROW	= 18		/ initial base row
MM_IEROW	= 19		/ initial end row
MM_INVIS	= 20		/ cursor invisible mask
MM_NCOL		= 22		/ number of columns
MM_DATA		= 24		/ pointer to crt data
MM_MODE		= 26		/ mode register [0x21=col80/132,0x20=col40]

/ ASCII characters
AZERO		= 0x30
CLOWER		= 0x63
SEMIC		= 0x3B
SPACE		= 0x20

	.prvd
mmdata:	.word	mminit
	.word	0x03D4
	.word	0xB800
	.byte	0, 0
	.word	0
	.byte	0x7, 0, 0, 0, 23, 24, 0, 0, 0, 23
	.word	0
	.word	80
	.word	creg80
	.byte	0x21, 0x00
	.shri

////////
/
/ creg40, creg80, creg132 - crt register values for 40/80/132 column color
/         mreg80, mreg132 - crt register values for 80/132 column monochrome
/
////////

creg40:	.byte	0x38, 0x28, 0x2D, 0x0A, 0x1F, 0x06, 0x19, 0x1C
	.byte	0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00

creg80:	.byte	0x71, 0x50, 0x5A, 0x0A, 0x1F, 0x06, 0x19, 0x1C
	.byte	0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00

mreg80:	.byte	0x61, 0x50, 0x52, 0x0F, 0x19, 0x06, 0x19, 0x19
	.byte	0x02, 0x0D, 0x0B, 0x0C, 0x00, 0x00, 0x00, 0x00

#ifdef ATI_132
	.globl	creg132
creg132:.byte	0xB5, 0x84, 0x97, 0x0A, 0x1F, 0x06, 0x19, 0x1C
	.byte	0x02, 0x07, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00

	.globl	mreg132
mreg132:.byte	0x9F, 0x84, 0x89, 0x0F, 0x19, 0x06, 0x19, 0x19
	.byte	0x02, 0x0D, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00
#endif

////////
/
/ mmgo( iop )
/ IO *iop;
/
////////

	.globl	mmgo_

mmgo_:
	push	si
	push	di
	push	bp
	mov	bp, sp
	push	ds
	push	es
	cld
	mov	bx, 8(bp)		/ iop
	mov	si, IO_BASE(bx)		/ iop->io_base
	mov	cx, IO_IOC(bx)		/ iop->io_ioc
	cmp	IO_SEG(bx), $IOSYS
	je	0f
	mov	ds, uds_
0:	mov	bp, $mmdata

	mov	dx, MM_PORT(bp)		/ turn video off if color board
	cmp	dx, $0x3B4
	je	0f
	add	dx, $4
	movb	al, MM_MODE(bp)
	outb	dx, al
0:
	movb	ROW, MM_ROW(bp)
	movb	COL, MM_COL(bp)
	mov	es,  MM_BASE(bp)
	mov	POS, MM_POS(bp)
	sub	bx, bx
	movb	ATTR, MM_ATTR(bp)
	ijmp	MM_FUNC(bp)

exit:	pop	bx
	pop	es
	pop	ds
	movb	MM_ATTR(bp), ATTR
	mov	MM_FUNC(bp), bx
	movb	MM_ROW(bp), ROW		/ save row,column
	movb	MM_COL(bp), COL
	mov	MM_POS(bp), POS		/ save position

	mov	dx, MM_PORT(bp)		/ adjust cursor location
	mov	bx, POS
	or	bx, MM_INVIS(bp)
	shr	bx, $1

	movb	al, $14
	outb	dx, al
	inc	dx
	movb	al, bh
	outb	dx, al
	dec	dx
	movb	al, $15
	outb	dx, al
	inc	dx
	movb	al, bl
	outb	dx, al

	mov	dx, MM_PORT(bp)		/ turn video on
	add	dx, $4
	movb	al, MM_MODE(bp)
	orb	al, $0x08
	outb	dx, al
	mov	mmvcnt_, $300		/ 300 seconds before video disabled

	mov	bp, sp
	mov	bx, 8(bp)
	mov	ax, cx
	xchg	cx, IO_IOC(bx)
	sub	cx, IO_IOC(bx)
	add	IO_BASE(bx), cx
	pop	bp
	pop	di
	pop	si
	ret

////////
/
/ mminit - initialize screen
/
////////

mminit:	movb	ss:mmesc_, $CLOWER		/ schedule keyboard initialization
	mov	MM_NCOL(bp), $80	/ set 80 column mode
	movb	MM_MODE(bp), $0x21
	mov	MM_DATA(bp), $creg80

#ifdef	ATI_132
	mov	dx, $0x3DF		/ clear 132 column color
	movb	al, $0x00		/ in mode select register
	outb	dx, al

	mov	dx, $0x3BA		/ clear 132 column monochrome
	movb	al, $0x00		/ in mode select register
	outb	dx, al
#endif

	call	int11_			/ read equipment status
	andb	al, $0x30		/ isolate video bits
	cmpb	al, $0x30		/ if monochrome
	jne	0f
	mov	MM_DATA(bp), $mreg80	/	set monochrome register info
	mov	MM_PORT(bp), $0x3B4	/	set monochrome port
	mov	MM_BASE(bp), $0xB000	/	set monochrome base
	mov	es, MM_BASE(bp)		/		and extra segment.

0:	call	newcrt			/ reprogram crt registers

reinit:	sub	ax, ax			/ regenerate row table
	mov	bx, $rowtab
1:	mov	ss:(bx), ax
	add	ax, MM_NCOL(bp)
	add	ax, MM_NCOL(bp)
	add	bx, $2
	cmp	bx, $rowend
	jb	1b

	mov	dx, MM_PORT(bp)		/ zero display offset
	movb	al, $12
	outb	dx, al
	inc	dx
	subb	al, al
	outb	dx, al
	dec	dx
	movb	al, $13
	outb	dx, al
	inc	dx
	subb	al, al
	outb	dx, al

	mov	dx, MM_PORT(bp)		/ reset border to black
	add	dx, $5
	subb	al, al
	outb	dx, al

	mov	MM_INVIS(bp), $0
	movb	ATTR, $0x07
	movb	MM_ATTR(bp), ATTR
	movb	ROW, MM_IBROW(bp)
	movb	MM_BROW(bp), ROW
	movb	bl, MM_IEROW(bp)
	movb	MM_EROW(bp), bl
	sub	bx, bx
	movb	MM_N1(bp), $2
	jmp	mm_ed

////////
/
/ newcrt -- reload crt registers
/
/	Action:	Program crt registers with values defined in code space
/		at offset given by MM_DATA(bp).
/
/	Note:	AX, BX, DX, DI trashed on exit.
/
////////

newcrt:	mov	dx, MM_PORT(bp)		/ turn video off
	add	dx, $4
	movb	al, MM_MODE(bp)
	outb	dx, al

	mov	di, MM_DATA(bp)		/ program crt registers, last to first
	mov	bx, $15			/ [delay between i/o]
	mov	dx, MM_PORT(bp)		/ [NOTE:DI=obsolete screen offset]
0:	movb	al, bl
	outb	dx, al
	movb	al, cs:(bx,di)
	inc	dx
	outb	dx, al
	dec	dx
	dec	bx
	jge	0b
	ret

////////
/
/ mm_so - stand out - define 40 column attributes
/
////////

mm_so:
	cmp	MM_PORT(bp), $0x3D4	/ if color card
	jne	mm_si

	mov	MM_NCOL(bp), $40	/	setup for 40 column color
	movb	MM_MODE(bp), $0x20
	mov	MM_DATA(bp), $creg40

#ifdef	ATI_132
	mov	dx, $0x3DF		/	clear 132 column color
	movb	al, $0x00		/	in mode select register
	outb	dx, al			/	[delay between i/o]
#endif

	call	newcrt			/ program crt registers
	jmp	reinit

////////
/
/ mm_si - define 80 column attributes
/
////////

mm_si:
	cmp	MM_PORT(bp), $0x3D4	/ if color card
	jne	0f

	mov	MM_NCOL(bp), $80
	movb	MM_MODE(bp), $0x21
	mov	MM_DATA(bp), $creg80

#ifdef	ATI_132
	mov	dx, $0x3DF		/	clear 132 column color
	movb	al, $0x00		/	in mode select register
	outb	dx, al			/	[delay between i/o]
#endif

	call	newcrt			/	reprogram crt registers.
	jmp	reinit

0:	mov	MM_NCOL(bp), $80
	mov	MM_DATA(bp), $mreg80
	movb	MM_MODE(bp), $0x21

#ifdef	ATI_132
	mov	dx, $0x3BA		/	clear 132 column monochrome
	movb	al, $0x00		/	in mode select register
	outb	dx, al			/	[delay between i/o]
#endif

	call	newcrt			/	reprogram crt registers
	jmp	reinit

////////
/
/ mm_132 - define 132 column attributes
/
////////

mm_132:
	cmp	MM_PORT(bp), $0x3D4	/ if color card
	jne	0f

#ifdef	ATI_132
	mov	MM_DATA(bp), $creg132	/	set color crt values
	mov	MM_NCOL(bp), $132	/	set columns to 132
	movb	MM_MODE(bp), $0x21

	call	newcrt			/	set 132 column crt values
					/	BEFORE setting mode select reg

	mov	dx, $0x3DF		/	set 132 columns
	movb	al, $0x10		/		in mode select register
	outb	dx, al
#endif
	jmp	reinit

0:
#ifdef	ATI_132
	mov	MM_NCOL(bp), $132	/	set columns to 132
	movb	MM_MODE(bp), $0x21	/	set 80/132 column display mode
	mov	MM_DATA(bp), $mreg132	/	set monochrome crt values

	call	newcrt			/	set 132 column crt values
					/	BEFORE setting mode select reg

	mov	dx, $0x3BA		/	set 132 columns monochrome
	movb	al, $0x08		/		in mode select register
	outb	dx, al
#endif
	jmp	reinit

////////
/
/ mmspec - schedule special keyboard function
/
////////

mmspec:	movb	ss:mmesc_, al
	jmp	eval

////////
/
/ mmbell - schedule beep
/
////////

mmbell:	movb	ss:mmbeeps_, $-1
	jmp	eval

////////
/
/ mm_cnl - cursor next line
/
/	Moves the active position to the first column of the next display line.
/	Scrolls the active display if necessary.
/
////////

mm_cnl:	subb	COL, COL
	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jna	repos
	movb	ROW, MM_EROW(bp)
/	jmp	scrollup

////////
/
/ scrollup - scroll display upwards
/
////////

scrollup:
	push	ds
	push	si
	push	cx
	mov	ds, MM_BASE(bp)
	movb	bl, MM_BROW(bp)
	shlb	bl, $1
	mov	di, ss:rowtab(bx)
	mov	si, ss:rowtab+2(bx)
	movb	bl, ROW
	shlb	bl, $1
	mov	cx, ss:rowtab(bx)
	push	cx
	sub	cx, di
	shr	cx, $1
	cld
	rep
	movsw
	movb	al, $SPACE
	pop	di
	mov	cx, MM_NCOL(bp)
	rep
	stosw
	pop	cx
	pop	si
	pop	ds
	movb	bl, COL			/ reposition to ROW and COL
	shlb	bl, $1
	mov	POS, cs:coltab(bx)
	movb	bl, ROW
	shlb	bl, $1
	add	POS, ss:rowtab(bx)
	call	exit
	jmp	eval

////////
/
/ repos - reposition cursor
/
////////

repos:	movb	bl, COL			/ reposition to ROW and COL
	shl	bx, $1			/ [trash BH]
	mov	POS, cs:coltab(bx)
	subb	bh, bh			/ [clear BH]
	movb	bl, ROW
	shlb	bl, $1
	add	POS, ss:rowtab(bx)
/	jmp	eval

////////
/
/ eval - evaluate input character
/
////////

eval:	jcxz	ewait
	dec	cx				/ evaluate next char
	lodsb
	movb	bl, al
	shlb	bl, $1
	ijmp	cs:asctab(bx)

////////
/
/ mmputc - put character on screen
/
////////

mmputc:	stosw
	incb	COL
	cmpb	COL, MM_NCOL(bp)
	jnb	0f
	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	ijmp	cs:asctab(bx)

0:	subb	COL, COL
	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jg	0f
	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	ijmp	cs:asctab(bx)

0:	movb	ROW, MM_EROW(bp)
	jmp	scrollup

////////
/
/ Ewait - wait for next input char to evaluate
/
////////

ewait:	call	exit
	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	ijmp	cs:asctab(bx)

////////
/
/ mm_cr - carriage return
/
/	Moves the active position to first position of current display line.
/
////////

mm_cr:	subb	COL, COL
	movb	bl, ROW
	shlb	bl, $1
	mov	POS, ss:rowtab(bx)
	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	ijmp	cs:asctab(bx)

////////
/
/ mm_cub - cursor backwards
/
////////

mm_cub:	sub	POS, $2
	subb	COL, $1
	jnb	0f
	movb	COL, MM_NCOL(bp)
	decb	COL
	decb	ROW
	cmpb	ROW, MM_BROW(bp)
	jge	0f
	subb	COL, COL
	movb	ROW, MM_BROW(bp)
	movb	bl, ROW
	shlb	bl, $1
	mov	POS, ss:rowtab(bx)
0:	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	ijmp	cs:asctab(bx)

////////
/
/ Esc state - entered when last char was ESC - transient state.
/
////////

0:	call	exit
mm_esc:	jcxz	0b
	dec	cx
	lodsb
	movb	MM_N1(bp), ZERO
	movb	MM_N2(bp), ZERO
	movb	bl, al
	shlb	bl, $1
	jc	mmputc
	ijmp	cs:esctab(bx)

////////
/
/ Csi_n1 state - entered when last two chars were ESC [
/
/	Action:	Evaluates numeric chars as numeric parameter 1.
/
////////

0:	call	exit
csi_n1:	jcxz	0b
	dec	cx
	lodsb
	cmpb	al, $SEMIC
	je	csi_n2
	movb	bl, al
	subb	bl, $AZERO
	cmpb	bl, $9
	ja	csival
	shlb	MM_N1(bp), $1	/ n1 * 2
	movb	al, MM_N1(bp)	/ n1 * 2
	shlb	al, $1		/ n1 * 4
	shlb	al, $1		/ n1 * 8
	addb	al, MM_N1(bp)	/ n1 * 10
	addb	al, bl		/ n1 * 10 + digit
	movb	MM_N1(bp), al	/ n1 = (n1 * 10) + digit
	jmp	csi_n1

////////
/
/ Csi_n2 state - entered after input sequence ESC [ n ;
/
////////

0:	call	exit
csi_n2:	jcxz	0b
	dec	cx
	lodsb
	movb	bl, al
	subb	bl, $AZERO
	cmpb	bl, $9
	ja	csival
	shlb	MM_N2(bp), $1	/ n2 * 2
	movb	al, MM_N2(bp)	/ n2 * 2
	shlb	al, $1		/ n2 * 4
	shlb	al, $1		/ n2 * 8
	addb	al, MM_N2(bp)	/ n2 * 10
	addb	al, bl		/ n2 * 10 + digit
	movb	MM_N2(bp), al	/ n2 = (n2 * 10) + digit
	jmp	csi_n2

csival:	movb	bl, al
	shlb	bl, $1
	ijmp	cs:csitab(bx)

////////
/
/ Csi_gt state - entered after input sequence ESC [ >
/	
////////

0:	call	exit
csi_gt:	jcxz	0b
	dec	cx
	lodsb
	movb	bl, al
	subb	bl, $AZERO
	cmpb	bl, $9
	ja	1f
	shlb	MM_N1(bp), $1	/ n1 * 2
	movb	al, MM_N1(bp)	/ n1 * 2
	shlb	al, $1		/ n1 * 4
	shlb	al, $1		/ n1 * 8
	addb	al, MM_N1(bp)	/ n1 * 10
	addb	al, bl		/ n1 * 10 + digit
	movb	MM_N1(bp), al	/ n1 = (n1 * 10) + digit
	jmp	csi_gt

1:	movb	bl, al
	shlb	bl, $1
	ijmp	cs:csgtab(bx)

////////
/
/ mm_cbt - cursor backward tabulation
/
/	Moves the active position horizontally in the backward direction
/	to the preceding in a series of predetermined positions.
/
////////

mm_cbt:	orb	COL, $7			/ calculate next tab stop
	incb	COL
	subb	COL, $16		/ step back two tab positions
	jnb	0f
	subb	COL, COL		/ cannot step past column 0
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_cgh - process ESC [ > N1 h escape sequence
/
/	Recognized sequences:	ESC [ > 13 h	-- Set CRT saver enabled.
/
////////

mm_cgh:	cmpb	MM_N1(bp), $13
	jne	0f
	mov	ss:mmcrtsav_, $1
0:	jmp	eval

////////
/
/ mm_cgl - process ESC [ > N1 l escape sequence
/
/	Recognized sequences:	ESC [ > 13 l	-- Reset CRT saver.
/
////////

mm_cgl:	cmpb	MM_N1(bp), $13
	jne	0f
	mov	ss:mmcrtsav_, $0
0:	jmp	eval

////////
/
/ mm_cha - cursor horizontal absolute
/
/	Advances the active position forward or backward along the active line
/	to the character position specified by the parameter.
/	A parameter value of zero or one moves the active position to the
/	first character position of the active line.
/	A parameter value of N moves the active position to character position
/	N of the active line.
/
////////

mm_cha:	movb	COL, MM_N1(bp)
	orb	COL, COL
	je	0f
	decb	COL
0:	cmpb	COL, MM_NCOL(bp)
	jb	0f
	movb	COL, MM_NCOL(bp)
	decb	COL
0:	jmp	repos			/ reposition cursor


////////
/
/ mm_cht - cursor horizontal tabulation
/
/	Advances the active position horizontally to the next or following
/	in a series of predetermined positions.
/
////////

mm_cht:	push	cx
	sub	cx, cx
	movb	cl, COL
	orb	cl, $7
	incb	cl
	subb	cl, COL
	addb	COL, cl
	movb	al, $SPACE
	rep
	stosw
	pop	cx
	cmpb	COL, MM_NCOL(bp)
	jb	0f
	subb	COL, MM_NCOL(bp)
	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jna	0f
	movb	ROW, MM_EROW(bp)
	jmp	scrollup
0:	jmp	eval

////////
/
/ mm_cpl - cursor preceding line
/
/	Moves the active position to the first position of the preceding
/	display line.
/
////////

mm_cpl:	subb	COL, COL
	decb	ROW
	cmpb	ROW, MM_BROW(bp)
	jnb	0f
	movb	ROW, MM_BROW(bp)
	jmp	scrolldown
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_cud - cursor down
/
/	Moves the active position downward without altering the
/	horizontal position.
/
////////

mm_cud:	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jna	0f
	movb	ROW, MM_EROW(bp)
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_cuf - cursor forward
/
/	Moves the active position in the forward direction.
/
////////

mm_cuf:	incb	COL
	cmpb	COL, MM_NCOL(bp)
	jb	0f
	subb	COL, MM_NCOL(bp)
	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jna	0f
	movb	ROW, MM_EROW(bp)
	movb	COL, MM_NCOL(bp)
	decb	COL
0:	jmp	repos

////////
/
/ mm_cup - cursor position
/
/	Moves the active position to the position specified by two parameters.
/	The 1st parameter (mm_n1) specifies the vertical   position MM_ROW(bp).
/	The 2nd parameter (mm_n2) specifies the horizontal position MM_COL(bp).
/	A parameter value of 0 or 1 for the first or second parameter
/	moves the active position to the first line or column in the
/	display respectively.
/
////////

mm_cup:	movb	ROW, MM_N1(bp)
	orb	ROW, ROW
	je	0f
	decb	ROW
0:	addb	ROW, MM_BROW(bp)
	cmpb	ROW, MM_EROW(bp)
	jb	0f
	movb	ROW, MM_EROW(bp)
0:	movb	COL, MM_N2(bp)
	orb	COL, COL
	je	0f
	decb	COL
0:	cmpb	COL, MM_NCOL(bp)
	jb	0f
	movb	COL, MM_NCOL(bp)
	decb	COL
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_cuu - cursor up
/
/	Moves the active position upward without altering the horizontal
/	position.
/
////////

mm_cuu:	decb	ROW
	cmpb	ROW, MM_BROW(bp)
	jge	0f
	movb	ROW, MM_BROW(bp)
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_dl - delete line
/
/	Removes the contents of the active line.
/	The contents of all following lines are shifted in a block
/	toward the active line.
/
////////

mm_dl:	push	ds
	push	si
	push	cx
	mov	ds, MM_BASE(bp)
	movb	bl, ROW
	shlb	bl, $1
	mov	di, ss:rowtab(bx)
	mov	si, ss:rowtab+2(bx)
	movb	bl, MM_EROW(bp)
	shlb	bl, $1
	mov	cx, ss:rowtab(bx)
	sub	cx, di
	jle	0f
	shr	cx, $1
	rep
	movsw
	mov	di, ss:rowtab(bx)
	mov	cx, MM_NCOL(bp)
	movb	al, $SPACE
	rep
	stosw
	subb	COL, COL
	movb	bl, ROW
	shlb	bl, $1
	mov	di, ss:rowtab(bx)
0:	pop	cx
	pop	si
	pop	ds
	call	exit
	jmp	eval

////////
/
/ mm_dmi - disable manual input
/
/	Set flag preventing keyboard input, and causing cursor to vanish.
/
////////

mm_dmi:
	mov	ss:islock_, $1
	jmp	eval

////////
/
/ mm_ea - erase in area
/
/	Erase some or all of the characters in the currently active area
/	according to the parameter:
/		0 - erase from active position to end inclusive (default)
/		1 - erase from start to active position inclusive
/		2 - erase all of active area
/
////////

mm_ea:	movb	al, MM_N1(bp)
	cmpb	al, $0
	jne	0f
	movb	bl, MM_EROW(bp)
	jmp	mm_e0
0:	cmpb	al, $1
	jne	0f
	movb	bl, MM_BROW(bp)
	jmp	mm_e1
0:	subb	COL, COL
	movb	ROW, MM_BROW(bp)
	movb	bl, ROW
	shlb	bl, $1
	mov	POS, ss:rowtab(bx)
	movb	bl, MM_EROW(bp)
	subb	bl, ROW
	jmp	mm_e2


////////
/
/ mm_ed - erase in display
/
/	Erase some or all of the characters in the display according to the
/	parameter
/		0 - erase from active position to end inclusive (default)
/		1 - erase from start to active position inclusive
/		2 - erase all of display
/
////////

mm_ed:	movb	al, MM_N1(bp)
	cmpb	al, $0
	jne	0f
	movb	bl, MM_LROW(bp)
	jmp	mm_e0
0:	cmpb	al, $1
	jne	0f
	subb	bl, bl
	jmp	mm_e1
0:	subb	COL, COL
	movb	ROW, MM_BROW(bp)
	sub	POS, POS
	movb	bl, MM_LROW(bp)
	jmp	mm_e2

////////
/
/ mm_el - erase in line
/
/	Erase some or all of the characters in the line according to the
/	parameter:
/		0 - erase from active position to end inclusive (default)
/		1 - erase from start to active position inclusive
/		2 - erase entire line
/
////////

mm_el:	movb	al, MM_N1(bp)
	movb	bl, ROW
	cmpb	al, $0
	je	mm_e0
	cmpb	al, $1
	je	mm_e1
	shlb	bl, $1
	mov	POS, ss:rowtab(bx)
	subb	COL, COL
	subb	bl, bl
/	jmp	mm_e2

mm_e2:	push	cx
	movb	al, $SPACE
0:	mov	cx, MM_NCOL(bp)
	rep
	stosw
	decb	bl
	jge	0b
	pop	cx
	jmp	repos

mm_e1:	push	cx
	mov	cx, POS
	shlb	bl, $1
	mov	POS, ss:rowtab(bx)
	sub	cx, POS
	jl	0f
	movb	al, $SPACE
	shr	cx, $1
	rep
	stosw
0:	pop	cx
	jmp	repos

mm_e0:	push	cx
	shlb	bl, $1
	mov	cx, ss:rowtab+2(bx)
	sub	cx, POS
	jl	0f
	movb	al, $SPACE
	shr	cx, $1
	rep
	stosw
0:	pop	cx
	jmp	repos

////////
/
/ mm_emi - enable manual input
/
/	Clear flag preventing keyboard input.
/
////////

mm_emi:
	mov	ss:islock_, $0
	jmp	eval

////////
/
/ mm_il - insert line
/
/	Insert a erased line at the active line by shifting the contents
/	of the active line and all following lines away from the active line.
/	The contents of the last line in the scrolling region are removed.
/
////////

scrolldown:
mm_il:	push	ds
	push	si
	push	cx
	mov	ds, MM_BASE(bp)
	movb	bl, MM_EROW(bp)
	shlb	bl, $1
	mov	si, ss:rowtab(bx)
	mov	cx, si
	sub	si, $2
	mov	di, ss:rowtab+2(bx)
	sub	di, $2
	movb	bl, ROW
	shlb	bl, $1
	sub	cx, ss:rowtab(bx)
	jle	0f
	shr	cx, $1
	std
	rep
	movsw
	mov	di, ss:rowtab(bx)
	mov	cx, MM_NCOL(bp)
	movb	al, $SPACE
	cld
	rep
	stosw
	subb	COL, COL
	movb	bl, ROW
	shlb	bl, $1
	mov	di, ss:rowtab(bx)
0:	pop	cx
	pop	si
	pop	ds
	call	exit
	jmp	eval

////////
/
/ mm_hpa - horizontal position absolute
/
/	Moves the active position within the active line to the position
/	specified by the parameter.  A parameter value of zero or one
/	moves the active position to the first position of the active line.
/
////////

mm_hpa:	movb	COL, MM_N1(bp)
	orb	COL, COL
	je	0f
	decb	COL
0:	cmpb	COL, MM_NCOL(bp)
	jb	0f
	movb	COL, MM_NCOL(bp)
	decb	COL
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_hpr - horizontal position relative
/
/	Moves the active position forward the number of positions specified
/	by the parameter.  A parameter value of zero or one indicates a
/	single-position move.
/
////////

mm_hpr:	movb	al, MM_N1(bp)
	orb	al, al
	jne	0f
	incb	al
0:	addb	COL, al
	cmpb	COL, MM_NCOL(bp)
	jb	0f
	movb	COL, MM_NCOL(bp)
	decb	COL
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_hvp - horizontal and vertical position
/
/	Moves the active position to the position specified by two parameters.
/	The first parameter specifies the vertical position (MM_ROW(bp)).
/	The second parameter specifies the horizontal position (MM_COL(bp)).
/	A parameter value of zero or one moves the active position to the
/	first line or column in the display.
/
////////

mm_hvp:	movb	ROW, MM_N1(bp)
	orb	ROW, ROW
	je	0f
	decb	ROW
0:	cmpb	ROW, MM_LROW(bp)
	jna	0f
	movb	ROW, MM_LROW(bp)
0:	movb	COL, MM_N2(bp)
	orb	COL, COL
	je	0f
	decb	COL
0:	cmpb	COL, MM_NCOL(bp)
	jb	0f
	movb	COL, MM_NCOL(bp)
	decb	COL
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_ind - index
/
/	Causes the active position to move downward one line without changing
/	the horizontal position.  Scrolling occurs if below scrolling region.
/
////////

mm_ind:	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jg	0f
	jmp	repos
0:	movb	ROW, MM_EROW(bp)
	jmp	scrollup

////////
/
/ mm_new - save cursor position
/
////////

mm_new:	movb	MM_SCOL(bp), COL
	movb	MM_SROW(bp), ROW
	jmp	eval

////////
/
/ mm_old - restore old cursor position
/
////////

mm_old:	movb	COL, MM_SCOL(bp)
	movb	ROW, MM_SROW(bp)
	jmp	repos

////////
/
/ mm_ri - reverse index
/
/	Moves the active position to the same horizontal position on the
/	preceding line.  Scrolling occurs if above scrolling region.
/
////////

mm_ri:	decb	ROW
	cmpb	ROW, MM_BROW(bp)
	jge	0f
	movb	ROW, MM_BROW(bp)
	jmp	scrolldown
0:	jmp	repos

////////
/
/ mm_scr - select cursor rendition
/
/	Invokes the cursor rendition specified by the parameter.
/
/	Recognized renditions are:	0 - cursor visible
/					1 - cursor invisible
////////

mm_scr:	decb	 MM_N1(bp)
	je	0f
	jg	1f
	mov	MM_INVIS(bp), $0
	jmp	eval

0:	mov	MM_INVIS(bp), $-1
1:	jmp	eval

////////
/
/ mm_sgr - select graphic rendition
/
/	Invokes the graphic rendition specified by the parameter.
/	All following characters in the data stream are rendered
/	according to the parameters until the next occurrence of
/	SGR in the data stream.
/
/	Recognized renditions are:	1 - high intensity
/					4 - underline
/					5 - slow blink
/					7 - reverse video
/					30-37 - foreground color
/					40-47 - background color
/					50-57 - border color
/
////////

mm_sgr:	movb	al, MM_N1(bp)
	cmpb	al, $0
	jne	0f
	movb	ATTR, $0x07
1:	jmp	eval
0:	cmpb	al, $1		/ bold
	jne	0f
	orb	ATTR, $INTENSE
	jmp	1b
0:	cmpb	al, $4		/ underline
	jne	0f
	cmp	MM_PORT(bp), $0x03D4	/ color card?
	je	1b			/ yes, ignore underline
	andb	ATTR, $~0x77
	orb	ATTR, $0x01
	jmp	1b
0:	cmpb	al, $5		/ blinking
	jne	0f
	orb	ATTR, $BLINK
	jmp	1b
0:	cmpb	al, $7		/ reverse video
	jne	0f
	movb	al, $0x70
	cmp	MM_PORT(bp), $0x3D4	/ color card?
	jne	2f
	movb	al, ah			/ yes, exchange foreground/background
	andb	al, $0x77
	rolb	al, $1
	rolb	al, $1
	rolb	al, $1
	rolb	al, $1
2:	andb	ATTR, $~0x77
	orb	ATTR, al
	jmp	1b
0:	cmp	MM_PORT(bp), $0x03D4	/ color card?
	jne	1b			/ no, ignore remaining options
0:	subb	al, $30		/ foreground color
	jl	1f
	cmpb	al, $7
	jg	0f
	movb	bl, al
	andb	ATTR, $~0x07
	orb	ATTR, cs:fcolor(bx)
	jmp	1f
0:	subb	al, $10
	jl	1f
	cmpb	al, $7
	jg	0f
	movb	bl, al
	andb	ATTR, $~0x70
	orb	ATTR, cs:bcolor(bx)
	jmp	1f
0:	subb	al, $10
	jl	1f
	cmpb	al, $7
	jg	0f
	movb	bl, al
	movb	al, cs:fcolor(bx)
	push	dx
	mov	dx, MM_PORT(bp)
	add	dx, $5
	outb	dx, al
	pop	dx
/	jmp	1f
0:
1:	jmp	eval

////////
/
/ mm_ssr - set scrolling region
/
////////

mm_ssr:	movb	al, MM_N1(bp)
	decb	al
	jge	0f
	subb	al, al
0:	cmpb	al, MM_LROW(bp)
	ja	1f
	movb	bl, MM_N2(bp)
	decb	bl
	jge	0f
	subb	bl, bl
0:	cmpb	bl, MM_LROW(bp)
	ja	1f
	cmpb	al, bl
	ja	1f
	movb	MM_BROW(bp), al
	movb	MM_EROW(bp), bl
	movb	ROW, al
	subb	COL, COL
1:	jmp	repos

////////
/
/ mm_vpa - vertical position absolute
/
/	Moves the active position to the line specified by the parameter
/	without changing the horizontal position.
/	A parameter value of 0 or 1 moves the active position vertically
/	to the first line.
/
////////

mm_vpa:	movb	ROW, MM_N1(bp)
	decb	ROW
	jg	0f
	subb	ROW, ROW
0:	cmpb	ROW, MM_LROW(bp)
	jna	0f
	movb	ROW, MM_LROW(bp)
0:	jmp	repos			/ reposition cursor

////////
/
/ mm_vpr - vertical position relative
/
/	Moves the active position downward the number of lines specified
/	by the parameter without changing the horizontal position.
/	A parameter value of zero or one moves the active position
/	one line downward.
/
////////

mm_vpr:	movb	al, MM_N1(bp)
	orb	al, al
	jne	0f
	incb	al
0:	addb	ROW, al
	cmpb	ROW, MM_LROW(bp)
	jb	0f
	movb	ROW, MM_LROW(bp)
0:	jmp	repos			/ reposition cursor

////////
/
/ asctab - table of functions indexed by ascii characters
/
////////

asctab:	.word	eval,	eval,	eval,	eval	/	NUL  SOH  STX  ETX
	.word	eval,	eval,	eval,	mmbell	/	EOT  ENQ  ACK  BEL
	.word	mm_cub,	mm_cht,	mm_cnl,	mm_ind	/	BS   HT   LF   VT
	.word	eval,	mm_cr,	mm_so,	mm_si	/	FF   CR   SO   SI
	.word	eval,	eval,	eval,	eval	/	DLE  DC1  DC2  DC3
/ DEBUG: mm_132 is only inserted temporarily, for testing - 86/05/26
	.word	eval,	eval,	eval,	mm_132	/	DC4  NAK  SYN  ETB
	.word	eval,	eval,	eval,	mm_esc	/	CAN  EM   SUB  ESC
	.word	eval,	eval,	eval,	eval	/	FS   GS   RS   US
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	  ! dquote # \040 - \043
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	$ % & quote \044 - \047
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	( ) * + \050 - \053
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	, - . / \054 - \057
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	0 1 2 3 \060 - \063
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	4 5 6 7 \064 - \067
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	8 9 : ; \070 - \073
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	< = > ? \074 - \077
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	@ A B C \100 - \103
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	D E F G \104 - \107
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	H I J K \110 - \113
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	L M N O \114 - \117
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	P Q R S \120 - \123
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	T U V W \124 - \127
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	X Y Z [ \130 - \133
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	\ ] ^ _ \134 - \137
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	` a b c \140 - \143
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	d e f g \144 - \147
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	h i j k \150 - \153
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	l m n o \154 - \157
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	p q r s \160 - \163
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	t u v w \164 - \167
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	x y z { \170 - \173
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	| } ~ ? \174 - \177

////////
/
/ esctab - table of functions indexed by escape characters.
/
////////

esctab:	.word	mmputc,	mmputc,	mmputc,	mmputc	/	NUL  SOH  STX  ETX
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	EOT  ENQ  ACK  BEL
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	BS   HT   LF   VT
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	FF   CR   SO   SI
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	DLE  DC1  DC2  DC3
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	DC4  NAK  SYN  ETB
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	CAN  EM   SUB  ESC
	.word	mmputc,	mmputc,	mmputc,	mmputc	/	FS   GS   RS   US
	.word	eval,	eval,	eval,	eval	/	  ! dquote # \040 - \043
	.word	eval,	eval,	eval,	eval	/	$ % & quote \044 - \047
	.word	eval,	eval,	eval,	eval	/	( ) * + \050 - \053
	.word	eval,	eval,	eval,	eval	/	, - . / \054 - \057
	.word	eval,	eval,	eval,	eval	/	0 1 2 3 \060 - \063
	.word	eval,	eval,	eval,	mm_new	/	4 5 6 7 \064 - \067
	.word	mm_old,	eval,	eval,	eval	/	8 9 : ; \070 - \073
	.word	eval,	mmspec,	mmspec,	eval	/	< = > ? \074 - \077
	.word	eval,	eval,	eval,	eval	/	@ A B C \100 - \103
	.word	mm_ind,	mm_cnl,	eval,	eval	/	D E F G \104 - \107
	.word	eval,	eval,	eval,	eval	/	H I J K \110 - \113
	.word	eval,	mm_ri,	eval,	eval	/	L M N O \114 - \117
	.word	eval,	eval,	eval,	eval	/	P Q R S \120 - \123
	.word	eval,	eval,	eval,	eval	/	T U V W \124 - \127
	.word	eval,	eval,	eval,	csi_n1	/	X Y Z [ \130 - \133
	.word	eval,	eval,	eval,	eval	/	\ ] ^ _ \134 - \137
	.word	mm_dmi,	eval,	mm_emi,	mminit	/	` a b c \140 - \143
	.word	eval,	eval,	eval,	eval	/	d e f g \144 - \147
	.word	eval,	eval,	eval,	eval	/	h i j k \150 - \153
	.word	eval,	eval,	eval,	eval	/	l m n o \154 - \157
	.word	eval,	eval,	eval,	eval	/	p q r s \160 - \163
	.word	mmspec,	mmspec,	eval,	eval	/	t u v w \164 - \167
	.word	eval,	eval,	eval,	eval	/	x y z { \170 - \173
	.word	eval,	eval,	eval,	eval	/	| } ~ ? \174 - \177

////////
/
/ csitab - table of functions indexed by ESC [ characters.
/
////////

csitab:	.word	eval,	eval,	eval,	eval	/	NUL  SOH  STX  ETX
	.word	eval,	eval,	eval,	eval	/	EOT  ENQ  ACK  BEL
	.word	eval,	eval,	eval,	eval	/	BS   HT   LF   VT
	.word	eval,	eval,	eval,	eval	/	FF   CR   SO   SI
	.word	eval,	eval,	eval,	eval	/	DLE  DC1  DC2  DC3
	.word	eval,	eval,	eval,	eval	/	DC4  NAK  SYN  ETB
	.word	eval,	eval,	eval,	eval	/	CAN  EM   SUB  ESC
	.word	eval,	eval,	eval,	eval	/	FS   GS   RS   US
	.word	eval,	eval,	eval,	eval	/	  ! dquote # \040 - \043
	.word	eval,	eval,	eval,	eval	/	$ % & quote \044 - \047
	.word	eval,	eval,	eval,	eval	/	( ) * + \050 - \053
	.word	eval,	eval,	eval,	eval	/	, - . / \054 - \057
	.word	eval,	eval,	eval,	eval	/	0 1 2 3 \060 - \063
	.word	eval,	eval,	eval,	eval	/	4 5 6 7 \064 - \067
	.word	eval,	eval,	eval,	eval	/	8 9 : ; \070 - \073
	.word	eval,	eval,	csi_gt,	eval	/	< = > ? \074 - \077
	.word	eval,	mm_cuu,	mm_cud,	mm_cuf	/	@ A B C \100 - \103
	.word	mm_cub,	mm_cnl,	mm_cpl,	mm_cha	/	D E F G \104 - \107
	.word	mm_cup,	mm_cht,	mm_ed,	mm_el	/	H I J K \110 - \113
	.word	mm_il,	mm_dl,	eval,	mm_ea	/	L M N O \114 - \117
	.word	eval,	eval,	eval,	mm_ind	/	P Q R S \120 - \123
	.word	mm_ri,	eval,	eval,	eval	/	T U V W \124 - \127
	.word	eval,	eval,	mm_cbt,	eval	/	X Y Z [ \130 - \133
	.word	eval,	eval,	eval,	eval	/	\ ] ^ _ \134 - \137
	.word	mm_hpa,	mm_hpr,	eval,	eval	/	` a b c \140 - \143
	.word	mm_vpa,	mm_vpr,	mm_hvp,	mm_cup	/	d e f g \144 - \147
	.word	eval,	eval,	eval,	eval	/	h i j k \150 - \153
	.word	eval,	mm_sgr,	eval,	eval	/	l m n o \154 - \157
	.word	eval,	eval,	mm_ssr,	eval	/	p q r s \160 - \163
	.word	eval,	eval,	mm_scr,	eval	/	t u v w \164 - \167
	.word	eval,	eval,	eval,	eval	/	x y z { \170 - \173
	.word	eval,	eval,	eval,	eval	/	| } ~ ? \174 - \177

////////
/
/ csgtab - table of functions indexed by ESC [ > characters.
/
////////

csgtab:	.word	eval,	eval,	eval,	eval	/	NUL  SOH  STX  ETX
	.word	eval,	eval,	eval,	eval	/	EOT  ENQ  ACK  BEL
	.word	eval,	eval,	eval,	eval	/	BS   HT   LF   VT
	.word	eval,	eval,	eval,	eval	/	FF   CR   SO   SI
	.word	eval,	eval,	eval,	eval	/	DLE  DC1  DC2  DC3
	.word	eval,	eval,	eval,	eval	/	DC4  NAK  SYN  ETB
	.word	eval,	eval,	eval,	eval	/	CAN  EM   SUB  ESC
	.word	eval,	eval,	eval,	eval	/	FS   GS   RS   US
	.word	eval,	eval,	eval,	eval	/	  ! dquote # \040 - \043
	.word	eval,	eval,	eval,	eval	/	$ % & quote \044 - \047
	.word	eval,	eval,	eval,	eval	/	( ) * + \050 - \053
	.word	eval,	eval,	eval,	eval	/	, - . / \054 - \057
	.word	eval,	eval,	eval,	eval	/	0 1 2 3 \060 - \063
	.word	eval,	eval,	eval,	eval	/	4 5 6 7 \064 - \067
	.word	eval,	eval,	eval,	eval	/	8 9 : ; \070 - \073
	.word	eval,	eval,	eval,	eval	/	< = > ? \074 - \077
	.word	eval,	eval,	eval,	eval	/	@ A B C \100 - \103
	.word	eval,	eval,	eval,	eval	/	D E F G \104 - \107
	.word	eval,	eval,	eval,	eval	/	H I J K \110 - \113
	.word	eval,	eval,	eval,	eval	/	L M N O \114 - \117
	.word	eval,	eval,	eval,	eval	/	P Q R S \120 - \123
	.word	eval,	eval,	eval,	eval	/	T U V W \124 - \127
	.word	eval,	eval,	eval,	eval	/	X Y Z [ \130 - \133
	.word	eval,	eval,	eval,	eval	/	\ ] ^ _ \134 - \137
	.word	eval,	eval,	eval,	eval	/	` a b c \140 - \143
	.word	eval,	eval,	eval,	eval	/	d e f g \144 - \147
	.word	mm_cgh,	eval,	eval,	eval	/	h i j k \150 - \153
	.word	mm_cgl,	eval,	eval,	eval	/	l m n o \154 - \157
	.word	eval,	eval,	eval,	eval	/	p q r s \160 - \163
	.word	eval,	eval,	eval,	eval	/	t u v w \164 - \167
	.word	eval,	eval,	eval,	eval	/	x y z { \170 - \173
	.word	eval,	eval,	eval,	eval	/	| } ~ ? \174 - \177

////////
/
/ coltab - integer array of offsets to each column - up to 132 columns
/
////////

coltab:	.word	  0*NCB,   1*NCB,   2*NCB,   3*NCB
	.word	  4*NCB,   5*NCB,   6*NCB,   7*NCB
	.word	  8*NCB,   9*NCB,  10*NCB,  11*NCB
	.word	 12*NCB,  13*NCB,  14*NCB,  15*NCB
	.word	 16*NCB,  17*NCB,  18*NCB,  19*NCB
	.word	 20*NCB,  21*NCB,  22*NCB,  23*NCB
	.word	 24*NCB,  25*NCB,  26*NCB,  27*NCB
	.word	 28*NCB,  29*NCB,  30*NCB,  31*NCB
	.word	 32*NCB,  33*NCB,  34*NCB,  35*NCB
	.word	 36*NCB,  37*NCB,  38*NCB,  39*NCB
	.word	 40*NCB,  41*NCB,  42*NCB,  43*NCB
	.word	 44*NCB,  45*NCB,  46*NCB,  47*NCB
	.word	 48*NCB,  49*NCB,  50*NCB,  51*NCB
	.word	 52*NCB,  53*NCB,  54*NCB,  55*NCB
	.word	 56*NCB,  57*NCB,  58*NCB,  59*NCB
	.word	 60*NCB,  61*NCB,  62*NCB,  63*NCB
	.word	 64*NCB,  65*NCB,  66*NCB,  67*NCB
	.word	 68*NCB,  69*NCB,  70*NCB,  71*NCB
	.word	 72*NCB,  73*NCB,  74*NCB,  75*NCB
	.word	 76*NCB,  77*NCB,  78*NCB,  79*NCB
	.word	 80*NCB,  81*NCB,  82*NCB,  83*NCB
	.word	 84*NCB,  85*NCB,  86*NCB,  87*NCB
	.word	 88*NCB,  89*NCB,  90*NCB,  91*NCB
	.word	 92*NCB,  93*NCB,  94*NCB,  95*NCB
	.word	 96*NCB,  97*NCB,  98*NCB,  99*NCB
	.word	100*NCB, 101*NCB, 102*NCB, 103*NCB
	.word	104*NCB, 105*NCB, 106*NCB, 107*NCB
	.word	108*NCB, 109*NCB, 110*NCB, 111*NCB
	.word	112*NCB, 113*NCB, 114*NCB, 115*NCB
	.word	116*NCB, 117*NCB, 118*NCB, 119*NCB
	.word	120*NCB, 121*NCB, 122*NCB, 123*NCB
	.word	124*NCB, 125*NCB, 126*NCB, 127*NCB
	.word	128*NCB, 129*NCB, 130*NCB, 131*NCB

////////
/
/ rowtab - array of offsets to each row - up to 44 rows
/	   automatically regenerated by reinit().
/
/	NOTE: In kernel data space to allow modification in protected mode.
/
////////

	.shrd
rowtab:	.word	 0*NRB,	 1*NRB,	 2*NRB,	 3*NRB
	.word	 4*NRB,	 5*NRB,	 6*NRB,	 7*NRB
	.word	 8*NRB,	 9*NRB,	10*NRB,	11*NRB
	.word	12*NRB,	13*NRB,	14*NRB,	15*NRB
	.word	16*NRB,	17*NRB,	18*NRB,	19*NRB
	.word	20*NRB,	21*NRB,	22*NRB,	23*NRB
	.word	24*NRB,	25*NRB,	26*NRB,	27*NRB
	.word	28*NRB,	29*NRB,	30*NRB,	31*NRB
	.word	32*NRB,	33*NRB,	34*NRB,	35*NRB
	.word	36*NRB,	37*NRB,	38*NRB,	39*NRB
	.word	40*NRB,	41*NRB,	42*NRB,	43*NRB
rowend:
	.shri

////////
/
/ fcolor - foreground color
/ bcolor - background color
/
/	indexed by ansi color (black,red,green,brown,blue,magenta,cyan,white)
/	yields graphics color (black,blue,green,cyan,red,magenta,brown,white)
/		which is properly shifted for installation in attribute byte.
/
////////

fcolor:	.byte	0x00, 0x04, 0x02, 0x06, 0x01, 0x05, 0x03, 0x07
bcolor:	.byte	0x00, 0x40, 0x20, 0x60, 0x10, 0x50, 0x30, 0x70

////////
/
/ mm_voff()	-- Disable video display.
/
////////
	.globl	mm_voff_
mm_voff_:
	mov	dx, mmdata+MM_PORT
	add	dx, $4
	movb	al, mmdata+MM_MODE
	outb	dx, al
	ret

////////
/
/ mm_von()	-- Enable video display
/
////////
	.globl	mm_von_
mm_von_:
	mov	dx, mmdata+MM_PORT	/ enable video display
	add	dx, $4
	movb	al, mmdata+MM_MODE
	orb	al, $0x08
	outb	dx, al
	mov	mmvcnt_, $300		/ 300 seconds before video disabled
	ret
