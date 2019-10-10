/ (lgl-
/ 	COHERENT Driver Kit Version 1.1.0
/ 	Copyright (c) 1982, 1990 by Mark Williams Company.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
////////
/
/	Memory mapped video driver assembler assist.
/
////////
/
/	virtual terminal additions Copyright 1991, 1992 Todd Fleming
/	6/91, 1/92
/
/	Todd Fleming
/	1991 Mountainside Drive
/	Blacksburg, VA 24060
/	
////////
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
////////

	NCOL	= 80		/ number of columns
	NCB	= 2		/ number of horizontal bytes per char
	NCR	= 1		/ number of horizontal lines per char
	NHB	= 160		/ number of horizontal bytes per line
	NRB	= NCR*NHB	/ number of bytes per character row

	hBUFSIZE = NCOL*25	/ 1/2 buffer size for 25 row screen (TBF)

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
/ Characters
AZERO		= 0x30
CLOWER		= 0x63
HLOWER		= 0x68
LLOWER		= 0x6C
SEMIC		= 0x3B
SPACE		= 0x20

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
MM_SLOW		= 22		/ slow [no flicker] video update
MM_WRAP		= 23		/ wrap to start of next line
MM_CURTERM	= 24		/ flag:1=current virterm (TBF)
MM_UNUSED	= 25		/ not used, preseve even byte ordering (TBF)
MM_DATASIZE	= 26		/ size of data structure (TBF)

NVIRTERMS	= 4		/ # of virtual terminals (TBF)

	.globl	VIDSLOW_	/ Patchable kernel variable.
	.globl  mmspecspace_
	.prvd
mmspecspace_:
mmdata:	.word	mminit
	.word	0x03B4
	.word	0xB000
	.byte	0, 0
	.word	0
	.byte	0x7, 0, 0, 0, 23, 24, 0, 0, 0, 23
	.word	0
VIDSLOW_:.byte	0
	.byte	1
	.byte	1,0

.blkb	MM_DATASIZE*[NVIRTERMS-1]	/ allocate NVIRTERMS-1 more data spaces

MMDATAADDR:				/ address of structs (TBF)
mmdata2		=	mmdata  + MM_DATASIZE
mmdata3		=	mmdata2 + MM_DATASIZE
mmdata4		=	mmdata3	+ MM_DATASIZE
/ mmdata5		=	mmdata4	+ MM_DATASIZE
/ mmdata6		=	mmdata5	+ MM_DATASIZE
/ mmdata7		=	mmdata6 + MM_DATASIZE
/ mmdata8		=	mmdata7 + MM_DATASIZE
/ mmdata9		=	mmdata8 + MM_DATASIZE
/ mmdata10	=	mmdata9	+ MM_DATASIZE
 
	.word mmdata,  mmdata2, mmdata3, mmdata4
/	.word mmdata,  mmdata2, mmdata3, mmdata4, mmdata5
/	.word mmdata6, mmdata7, mmdata8, mmdata9, mmdata10

	.shri

//////////////
/
/ mmblankbuf(dest)   blank screen buffer (TBF)
/ unsigned dest;
/
/ dest is selector
/
//////////////

	.globl mmblankbuf_
mmblankbuf_:
	push si
	push di
	push es
	push bp
	mov  bp,sp

	mov ax, 10(bp)		/ es=dest
	mov es, ax

	mov di, $0			/ buffer start

	mov ax,	$0x0700		/ blank value
	mov	cx, $hBUFSIZE	/ size of buffer in words

	rep
	stosw				/ blank out buffer

	pop bp
	pop es
	pop	di
	pop si
	ret

//////////
/
/ mmbufcopy(dest,src) (TBF)
/ unsigned dest,src
/
/ copies screen buffer from src to dest. dest,src are selectors
/
//////////

	.globl mmbufcopy_
mmbufcopy_:
	push	si
	push	di
	push	ds
	push	es
	push	bp
	mov	bp, sp

	mov	ax, 12(bp)		/ es=dest
	mov es, ax		

	mov	ax, 14(bp)		/ ds=src
	mov ds, ax

	mov si,$0
	mov di,$0
	mov cx,$hBUFSIZE
	rep
	movsw				/ copy buffer
	
	pop bp
	pop es
	pop ds
	pop	di
	pop	si
	ret

////////
/
/ mmgo ( iop, dev )
/ IO *iop; dev_t dev
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
	cmp	bx, $0			/ skip if(iop==NULL)
	je	0f
	mov	si, IO_BASE(bx)		/ iop->io_base
	mov	cx, IO_IOC(bx)		/ iop->io_ioc

	cmp	IO_SEG(bx), $IOSYS	/ user address space
	je	0f
	mov	bx, uds_
	mov	ds, bx
0:
	mov	bx, 10(bp)		/ bx=dev (TBF)
	movb	bh, $0			/ bx=minor(dev) (TBF)
	sub	bx, $1			/ bx=minor(dev)-1 (TBF)
	shl	bx, $1 			/ bx=2*(minor(dev)-1) (TBF)
	mov	bp, ss:MMDATAADDR(bx)	/ bp=&mmspecspace[minor(dev)-1]; (TBF)
  
	mov	dx, MM_PORT(bp)		/ turn video off if color board
	cmp	dx, $0x3B4
	je	3f
	cmpb	MM_SLOW(bp), $0		/ check for slow [flicker-free]
	je	2f

	mov	dx, $0x3DA
1:	inb	al, dx			/ wait for vertical retrace
	testb	al, $8
	je	1b
2:
	mov	dx, $0x3D8		/ disable video
	movb	al, $0x25
	outb	dx, al
3:
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
	je	4f
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

4:
	mov	dx, MM_PORT(bp)		/ turn video on
	add	dx, $4
	movb	al, $0x29
	outb	dx, al
	mov	mmvcnt_, $600		/ 600 seconds before video disabled

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
/ mmcursor - cursor update entry point (TBF)
/
////////

	.globl	mmcursor_

mmcursor_:
	call exit						/ exit takes care of rest
	jmp eval

////////
/
/ mminit - initialize screen
/
////////

mminit:	movb	ss:mmesc_, $CLOWER	/ schedule keyboard initialization
	
	call	int11_			/ read equipment status
	and	ax, $0x30		/ isolate video bits
	cmp	ax, $0x30		/ if not monochrome
	je	0f
	mov	MM_PORT(bp), $0x3D4	/	set color port
	mov	MM_BASE(bp), $0xB800	/	set color base
	mov	es, MM_BASE(bp)		/
0:					/
	mov	dx, MM_PORT(bp)		/ turn video off
	add	dx, $4
	movb	al, $0x21
	outb	dx, al

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

	inc	dx			/ reset TECMAR XMSR register
	outb	dx, al

mminit2:movb	ss:mmesc_, $CLOWER
	mov	MM_INVIS(bp), $0
	movb	ATTR, $0x07
	movb	MM_ATTR(bp), ATTR
	movb	MM_WRAP(bp), $1
	movb	ROW, MM_IBROW(bp)
	movb	MM_BROW(bp), ROW
	movb	bl, MM_IEROW(bp)
	movb	MM_EROW(bp), bl
	sub	bx, bx
	movb	MM_N1(bp), $2
	jmp	mm_ed

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

mmbell:	
	cmp		MM_CURTERM(bp), $0	/ only if not current virterm (TBF)
	je		1f
	movb	ss:mmbeeps_, $-1
1:	jmp	eval

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
	mov	di, cs:rowtab(bx)
	mov	si, cs:rowtab+2(bx)
	movb	bl, ROW
	shlb	bl, $1
	mov	cx, cs:rowtab(bx)
	push	cx
	sub	cx, di
	shr	cx, $1
	cld
	rep
	movsw
	movb	al, $SPACE
	pop	di
	mov	cx, $NCOL
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
	add	POS, cs:rowtab(bx)
	call	exit
	jmp	eval

////////
/
/ repos - reposition cursor
/
////////

repos:	movb	bl, COL			/ reposition to ROW and COL
	shlb	bl, $1
	mov	POS, cs:coltab(bx)
	movb	bl, ROW
	shlb	bl, $1
	add	POS, cs:rowtab(bx)
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
	jc	mmputc
	ijmp	cs:asctab(bx)

////////
/
/ mmputc - put character on screen
/
////////

mmputc:	stosw				/ Update display memory.
	incb	COL
	cmpb	COL, $NCOL		/ Past end of line?
	jge	0f
	jcxz	ewait			/ Not past, evaluate next character.
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	jc	mmputc
	ijmp	cs:asctab(bx)

0:	cmpb	MM_WRAP(bp), $0		/ Yes past, Wrap around?
	jne	0f
	sub	di, $2			/ No wrap, adjust back to end of line.
	decb	COL
	jcxz	ewait			/ Not past, evaluate next character.
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	jc	mmputc
	ijmp	cs:asctab(bx)

0:	subb	COL, COL		/ Wrap to next line.
	incb	ROW
	cmpb	ROW, MM_EROW(bp)	/ Past scrolling region?
	jg	0f
	jcxz	ewait			/ Not past, evaluate next character.
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	jc	mmputc
	ijmp	cs:asctab(bx)

0:	movb	ROW, MM_EROW(bp)	/ Yes past, scroll up 1 line.
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
	jc	mmputc
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
	mov	POS, cs:rowtab(bx)
	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	jc	mmputc
	ijmp	cs:asctab(bx)

////////
/
/ mm_cub - cursor backwards
/
////////

mm_cub:	sub	POS, $2
	decb	COL
	jge	0f
	movb	COL, $NCOL-1
	decb	ROW
	cmpb	ROW, MM_BROW(bp)
	jge	0f
	subb	COL, COL
	movb	ROW, MM_BROW(bp)
	movb	bl, ROW
	shlb	bl, $1
	mov	POS, cs:rowtab(bx)
0:	jcxz	ewait
	dec	cx
	lodsb
	movb	bl, al
	shlb	bl, $1
	jc	mmputc
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
	jc	mmputc
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
	ja	0f
	shlb	MM_N1(bp), $1	/ n1 * 2
	movb	al, MM_N1(bp)	/ n1 * 2
	shlb	al, $1		/ n1 * 4
	shlb	al, $1		/ n1 * 8
	addb	al, MM_N1(bp)	/ n1 * 10
	addb	al, bl		/ n1 * 10 + digit
	movb	MM_N1(bp), al	/ n1 = (n1 * 10) + digit
	jmp	csi_gt

0:	cmpb	al, $HLOWER
	je	mm_cgh
	cmpb	al, $LLOWER
	je	mm_cgl
	jmp	eval

////////
/
/ Csi_q state - entered after input sequence ESC [ ?
/	
////////

0:	call	exit
csi_q:	jcxz	0b
	dec	cx
	lodsb
	movb	bl, al
	subb	bl, $AZERO
	cmpb	bl, $9
	ja	0f
	shlb	MM_N1(bp), $1	/ n1 * 2
	movb	al, MM_N1(bp)	/ n1 * 2
	shlb	al, $1		/ n1 * 4
	shlb	al, $1		/ n1 * 8
	addb	al, MM_N1(bp)	/ n1 * 10
	addb	al, bl		/ n1 * 10 + digit
	movb	MM_N1(bp), al	/ n1 = (n1 * 10) + digit
	jmp	csi_q

0:	cmpb	al, $HLOWER
	je	mm_cqh
	cmpb	al, $LLOWER
	je	mm_cql
	jmp	eval

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
	jg	0f
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
	decb	COL
	jge	0f
	subb	COL, COL
0:	cmpb	COL, $NCOL
	jb	0f
	movb	COL, $NCOL-1
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
	cmpb	COL, $NCOL
	jb	0f
	subb	COL, $NCOL
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
/ mm_cqh - process ESC [ ? N1 h escape sequence
/
/	Recognized sequences:	ESC [ ? 4 h	-- Set smooth scroll.
/				ESC [ ? 7 h	-- Set wraparound.
/
////////

mm_cqh:	cmpb	MM_N1(bp), $4		/ Smooth scroll.
	jne	0f
	movb	MM_SLOW(bp), $1
0:	cmpb	MM_N1(bp), $7		/ Wraparound.
	jne	0f
	movb	MM_WRAP(bp), $1
0:	jmp	eval

////////
/
/ mm_cql - process ESC [ ? N1 l escape sequence
/
/	Recognized sequences:	ESC [ ? 4 l	-- Set jump scroll.
/				ESC [ ? 7 l	-- Reset wraparound.
/
////////

mm_cql:	cmpb	MM_N1(bp), $4		/ Jump scroll.
	jne	0f
	movb	MM_SLOW(bp), $0
0:	cmpb	MM_N1(bp), $7		/ No wraparound.
	jne	0f
	movb	MM_WRAP(bp), $0
0:	jmp	eval

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
	cmpb	COL, $NCOL
	jb	0f
	subb	COL, $NCOL
	incb	ROW
	cmpb	ROW, MM_EROW(bp)
	jna	0f
	movb	ROW, MM_EROW(bp)
	movb	COL, $NCOL-1
0:	jmp	repos

////////
/
/ mm_cup - cursor position
/
/	Moves the active position to the position specified by two parameters.
/	The first parameter (mm_n1) specifies the vertical position (MM_ROW(bp)).
/	The second parameter (mm_n2) specifies the horizontal position (MM_COL(bp)).
/	A parameter value of 0 or 1 for the first or second parameter
/	moves the active position to the first line or column in the
/	display respectively.
/
////////

mm_cup:	movb	ROW, MM_N1(bp)
	decb	ROW
	jg	0f
	subb	ROW, ROW
0:	addb	ROW, MM_BROW(bp)
	cmpb	ROW, MM_EROW(bp)
	jb	0f
	movb	ROW, MM_EROW(bp)
0:	movb	COL, MM_N2(bp)
	decb	COL
	jg	0f
	subb	COL, COL
0:	cmpb	COL, $NCOL
	jb	0f
	movb	COL, $NCOL-1
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
	mov	di, cs:rowtab(bx)
	mov	si, cs:rowtab+2(bx)
	movb	bl, MM_EROW(bp)
	shlb	bl, $1
	mov	cx, cs:rowtab(bx)
	sub	cx, di
	jle	0f
	shr	cx, $1
	rep
	movsw
	mov	di, cs:rowtab(bx)
	mov	cx, $NCOL
	movb	al, $SPACE
	rep
	stosw
	subb	COL, COL
	movb	bl, ROW
	shlb	bl, $1
	mov	di, cs:rowtab(bx)
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
	mov	POS, cs:rowtab(bx)
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
	mov	POS, cs:rowtab(bx)
	subb	COL, COL
	subb	bl, bl
/	jmp	mm_e2

mm_e2:	push	cx
	movb	al, $SPACE
0:	mov	cx, $NCOL
	rep
	stosw
	decb	bl
	jge	0b
	pop	cx
	jmp	repos

mm_e1:	push	cx
	mov	cx, POS
	shlb	bl, $1
	mov	POS, cs:rowtab(bx)
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
	mov	cx, cs:rowtab+2(bx)
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
	mov	si, cs:rowtab(bx)
	mov	cx, si
	sub	si, $2
	mov	di, cs:rowtab+2(bx)
	sub	di, $2
	movb	bl, ROW
	shlb	bl, $1
	sub	cx, cs:rowtab(bx)
	jle	0f
	shr	cx, $1
	std
	rep
	movsw
	mov	di, cs:rowtab(bx)
	mov	cx, $NCOL
	movb	al, $SPACE
	cld
	rep
	stosw
	subb	COL, COL
	movb	bl, ROW
	shlb	bl, $1
	mov	di, cs:rowtab(bx)
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
	decb	COL
	jg	0f
	subb	COL, COL
0:	cmpb	COL, $NCOL
	jb	0f
	movb	COL, $NCOL-1
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
	cmpb	COL, $NCOL
	jb	0f
	movb	COL, $NCOL-1
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
	decb	ROW
	jg	0f
	subb	ROW, ROW
0:	cmpb	ROW, MM_LROW(bp)
	jna	0f
	movb	ROW, MM_LROW(bp)
0:	movb	COL, MM_N2(bp)
	decb	COL
	jg	0f
	subb	COL, COL
0:	cmpb	COL, $NCOL
	jb	0f
	movb	COL, $NCOL-1
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
/					8 - concealed on
/					30-37 - foreground color
/					40-47 - background color
/					50-57 - border color
/
////////

mm_sgr:	movb	al, MM_N1(bp)

	cmpb	al, $0			/ reset all = 0
	jne	0f
	movb	ATTR, $0x07
1:	jmp	eval

0:	cmpb	al, $1			/ bold = 1
	jne	0f
	orb	ATTR, $INTENSE
	jmp	1b

0:	cmpb	al, $4			/ underline = 4
	jne	0f
	cmp	MM_PORT(bp), $0x03D4	/ color card?
	je	1b			/ yes, ignore underline
	andb	ATTR, $~0x77
	orb	ATTR, $0x01
	jmp	1b

0:	cmpb	al, $5			/ blinking = 5
	jne	0f
	orb	ATTR, $BLINK
	jmp	1b

0:	cmpb	al, $7			/ reverse video = 7
	jne	0f
	movb	al, $0x70
	cmp	MM_PORT(bp), $0x3D4	/ color card?
	jne	2f
	movb	al, ATTR		/ yes, exchange foreground/background
	andb	al, $0x77
	rolb	al, $1
	rolb	al, $1
	rolb	al, $1
	rolb	al, $1
2:	andb	ATTR, $~0x77
	orb	ATTR, al
	jmp	1b

0:	cmpb	al, $8			/ concealed on = 8
	jne	0f
	cmp	MM_PORT(bp), $0x3D4	/ color card?
	jne	2f

	andb	ATTR, $0x70		/ Yes,	Set foreground color
	movb	al, ATTR		/	to background color.
	rorb	al, $1
	rorb	al, $1
	rorb	al, $1
	rorb	al, $1
	orb	ATTR, al
	jmp	1b

2:	andb	ATTR, $0x80		/ No, set attributes to non-display.
	jmp	1b			/	retain blink attribute.

0:	cmp	MM_PORT(bp), $0x03D4	/ color card?
	jne	1b			/ no, ignore remaining options
0:	subb	al, $30			/ foreground color
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

asctab:	.word	eval,	eval,	eval,	eval	/* NUL  SOH  STX  ETX  */
	.word	eval,	eval,	eval,	mmbell	/* EOT  ENQ  ACK  BEL  */
	.word	mm_cub,	mm_cht,	mm_cnl,	mm_ind	/* BS   HT   LF   VT   */
	.word	eval,	mm_cr,	eval,	eval	/* FF   CR   SO   SI   */
	.word	eval,	eval,	eval,	eval	/* DLE  DC1  DC2  DC3  */
	.word	eval,	eval,	eval,	eval	/* DC4  NAK  SYN  ETB  */
	.word	eval,	eval,	eval,	mm_esc	/* CAN  EM   SUB  ESC  */
	.word	eval,	eval,	eval,	eval	/* FS   GS   RS   US   */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/*   ! " # \040 - \043 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* $ % & quote \044 - \047 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* ( ) * + \050 - \053 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* , - . / \054 - \057 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* 0 1 2 3 \060 - \063 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* 4 5 6 7 \064 - \067 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* 8 9 : ; \070 - \073 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* < = > ? \074 - \077 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* @ A B C \100 - \103 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* D E F G \104 - \107 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* H I J K \110 - \113 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* L M N O \114 - \117 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* P Q R S \120 - \123 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* T U V W \124 - \127 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* X Y Z [ \130 - \133 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* \ ] ^ _ \134 - \137 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* ` a b c \140 - \143 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* d e f g \144 - \147 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* h i j k \150 - \153 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* l m n o \154 - \157 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* p q r s \160 - \163 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* t u v w \164 - \167 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* x y z { \170 - \173 */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* | } ~ ? \174 - \177 */

////////
/
/ esctab - table of functions indexed by ESC characters.
/
////////

esctab:	.word	mmputc,	mmputc,	mmputc,	mmputc	/* NUL  SOH  STX  ETX  */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* EOT  ENQ  ACK  BEL  */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* BS   HT   LF   VT   */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* FF   CR   SO   SI   */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* DLE  DC1  DC2  DC3  */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* DC4  NAK  SYN  ETB  */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* CAN  EM   SUB  ESC  */
	.word	mmputc,	mmputc,	mmputc,	mmputc	/* FS   GS   RS   US   */
	.word	eval,	eval,	eval,	eval	/*   ! " # \040 - \043 */
	.word	eval,	eval,	eval,	eval	/* $ % & quote \044 - \047 */
	.word	eval,	eval,	eval,	eval	/* ( ) * + \050 - \053 */
	.word	eval,	eval,	eval,	eval	/* , - . / \054 - \057 */
	.word	eval,	eval,	eval,	eval	/* 0 1 2 3 \060 - \063 */
	.word	eval,	eval,	eval,	mm_new	/* 4 5 6 7 \064 - \067 */
	.word	mm_old,	eval,	eval,	eval	/* 8 9 : ; \070 - \073 */
	.word	eval,	mmspec,	mmspec,	eval	/* < = > ? \074 - \077 */
	.word	eval,	eval,	eval,	eval	/* @ A B C \100 - \103 */
	.word	mm_ind,	mm_cnl,	eval,	eval	/* D E F G \104 - \107 */
	.word	eval,	eval,	eval,	eval	/* H I J K \110 - \113 */
	.word	eval,	mm_ri,	eval,	eval	/* L M N O \114 - \117 */
	.word	eval,	eval,	eval,	eval	/* P Q R S \120 - \123 */
	.word	eval,	eval,	eval,	eval	/* T U V W \124 - \127 */
	.word	eval,	eval,	eval,	csi_n1	/* X Y Z [ \130 - \133 */
	.word	eval,	eval,	eval,	eval	/* \ ] ^ _ \134 - \137 */
	.word	mm_dmi,	eval,	mm_emi,	mminit2	/* quote a b c \140 - \143 */
	.word	eval,	eval,	eval,	eval	/* d e f g \144 - \147 */
	.word	eval,	eval,	eval,	eval	/* h i j k \150 - \153 */
	.word	eval,	eval,	eval,	eval	/* l m n o \154 - \157 */
	.word	eval,	eval,	eval,	eval	/* p q r s \160 - \163 */
	.word	mmspec,	mmspec,	eval,	eval	/* t u v w \164 - \167 */
	.word	eval,	eval,	eval,	eval	/* x y z { \170 - \173 */
	.word	eval,	eval,	eval,	eval	/* | } ~ ? \174 - \177 */


////////
/
/ csitab - table of functions indexed by ESC [ characters.
/
////////

csitab:	.word	eval,	eval,	eval,	eval	/* NUL  SOH  STX  ETX  */
	.word	eval,	eval,	eval,	eval	/* EOT  ENQ  ACK  BEL  */
	.word	eval,	eval,	eval,	eval	/* BS   HT   LF   VT   */
	.word	eval,	eval,	eval,	eval	/* FF   CR   SO   SI   */
	.word	eval,	eval,	eval,	eval	/* DLE  DC1  DC2  DC3  */
	.word	eval,	eval,	eval,	eval	/* DC4  NAK  SYN  ETB  */
	.word	eval,	eval,	eval,	eval	/* CAN  EM   SUB  ESC  */
	.word	eval,	eval,	eval,	eval	/* FS   GS   RS   US   */
	.word	eval,	eval,	eval,	eval	/*   ! " # \040 - \043 */
	.word	eval,	eval,	eval,	eval	/* $ % & quote \044 - \047 */
	.word	eval,	eval,	eval,	eval	/* ( ) * + \050 - \053 */
	.word	eval,	eval,	eval,	eval	/* , - . / \054 - \057 */
	.word	eval,	eval,	eval,	eval	/* 0 1 2 3 \060 - \063 */
	.word	eval,	eval,	eval,	eval	/* 4 5 6 7 \064 - \067 */
	.word	eval,	eval,	eval,	eval	/* 8 9 : ; \070 - \073 */
	.word	eval,	eval,	csi_gt,	csi_q	/* < = > ? \074 - \077 */
	.word	eval,	mm_cuu,	mm_cud,	mm_cuf	/* @ A B C \100 - \103 */
	.word	mm_cub,	mm_cnl,	mm_cpl,	mm_cha	/* D E F G \104 - \107 */
	.word	mm_cup,	mm_cht,	mm_ed,	mm_el	/* H I J K \110 - \113 */
	.word	mm_il,	mm_dl,	eval,	mm_ea	/* L M N O \114 - \117 */
	.word	eval,	eval,	eval,	mm_ind	/* P Q R S \120 - \123 */
	.word	mm_ri,	eval,	eval,	eval	/* T U V W \124 - \127 */
	.word	eval,	eval,	mm_cbt,	eval	/* X Y Z [ \130 - \133 */
	.word	eval,	eval,	eval,	eval	/* \ ] ^ _ \134 - \137 */
	.word	mm_hpa,	mm_hpr,	eval,	eval	/* ` a b c \140 - \143 */
	.word	mm_vpa,	mm_vpr,	mm_hvp,	mm_cup	/* d e f g \144 - \147 */
	.word	eval,	eval,	eval,	eval	/* h i j k \150 - \153 */
	.word	eval,	mm_sgr,	eval,	eval	/* l m n o \154 - \157 */
	.word	eval,	eval,	mm_ssr,	eval	/* p q r s \160 - \163 */
	.word	eval,	eval,	mm_scr,	eval	/* t u v w \164 - \167 */
	.word	eval,	eval,	eval,	eval	/* x y z { \170 - \173 */
	.word	eval,	eval,	eval,	eval	/* | } ~ ? \174 - \177 */

////////
/
/ coltab - integer array of offsets to each column
/
////////

coltab:	.word	 0*NCB,	 1*NCB,	 2*NCB,	 3*NCB
	.word	 4*NCB,	 5*NCB,	 6*NCB,	 7*NCB
	.word	 8*NCB,	 9*NCB,	10*NCB,	11*NCB
	.word	12*NCB,	13*NCB,	14*NCB,	15*NCB
	.word	16*NCB,	17*NCB,	18*NCB,	19*NCB
	.word	20*NCB,	21*NCB,	22*NCB,	23*NCB
	.word	24*NCB,	25*NCB,	26*NCB,	27*NCB
	.word	28*NCB,	29*NCB,	30*NCB,	31*NCB
	.word	32*NCB,	33*NCB,	34*NCB,	35*NCB
	.word	36*NCB,	37*NCB,	38*NCB,	39*NCB
	.word	40*NCB,	41*NCB,	42*NCB,	43*NCB
	.word	44*NCB,	45*NCB,	46*NCB,	47*NCB
	.word	48*NCB,	49*NCB,	50*NCB,	51*NCB
	.word	52*NCB,	53*NCB,	54*NCB,	55*NCB
	.word	56*NCB,	57*NCB,	58*NCB,	59*NCB
	.word	60*NCB,	61*NCB,	62*NCB,	63*NCB
	.word	64*NCB,	65*NCB,	66*NCB,	67*NCB
	.word	68*NCB,	69*NCB,	70*NCB,	71*NCB
	.word	72*NCB,	73*NCB,	74*NCB,	75*NCB
	.word	76*NCB,	77*NCB,	78*NCB,	79*NCB

////////
/
/ rowtab - array of offsets to each row
/
////////

rowtab:	.word	 0*NRB,	 1*NRB,	 2*NRB,	 3*NRB
	.word	 4*NRB,	 5*NRB,	 6*NRB,	 7*NRB
	.word	 8*NRB,	 9*NRB,	10*NRB,	11*NRB
	.word	12*NRB,	13*NRB,	14*NRB,	15*NRB
	.word	16*NRB,	17*NRB,	18*NRB,	19*NRB
	.word	20*NRB,	21*NRB,	22*NRB,	23*NRB
	.word	24*NRB,	25*NRB,	26*NRB,	27*NRB
	.word	28*NRB,	29*NRB,	30*NRB,	31*NRB

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
/ mm_voff()	-- turn video display off
/
////////
	.globl	mm_voff_
mm_voff_:
	mov	dx, mmdata+MM_PORT
	add	dx, $4
	movb	al, $0x21
	outb	dx, al
	ret

////////
/
/ mm_von()	-- turn video display on
/
////////
	.globl	mm_von_
mm_von_:
	mov	dx, mmdata+MM_PORT	/ enable video display
	add	dx, $4
	movb	al, $0x29
	outb	dx, al
	mov	mmvcnt_, $900		/ 900 seconds before video disabled
	ret
