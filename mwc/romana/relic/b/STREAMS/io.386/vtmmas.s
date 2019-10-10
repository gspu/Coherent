	.unixorder
////////
/
/	Memory mapped video driver assembler assist.
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

	.set	NCOL, 80	/ number of columns
	.set	NCB, 2		/ number of horizontal bytes per char
	.set	SCB, 1		/ log2(NCB)
	.set	NCR, 1		/ number of horizontal lines per char
	.set	NHB, 160	/ number of horizontal bytes per line
	.set	NRB, 160	/ number of bytes per character row(NCR*NHB)

ATTR	.define	%ah	/* attribute byte */
ZERO	.define	%bh	/* (almost) always zero */
ROW	.define	%dh	/* currently active vertical position */
COL	.define	%dl	/* currently active horizontal position */
POS	.define	%edi	/* currently active display address */

	.set	INTENSE,	0x08	/ high intensity attribute bit
	.set	BLINK,		0x80	/ blinking attribute bit
	.set	REVERSE,	0x70	/ reverse video

	.set	SEG_386_UD,	0x10 	/ 32 bit user data segment descriptor
	.set	SEG_ROM,	0x40	/ ROM descriptor         @ F0000
	.set	SEG_VIDEOa,	0x48	/ 0x48: video descriptor @ B0000
	.set	SEG_VIDEOb,	0x50	/ 0x50: video descriptor @ B8000

////////
/
/ Magic constants from <sys/io.h>
/
////////

	.set	IO_SEG, 0
	.set	IO_IOC, 4
	.set	IO_BASE, 12

	.set	IOSYS, 0
	.set	IOUSR, 1

////////
/
/ Data
/
////////

	.set	MM_FUNC,	0	/ current state
	.set	MM_PORT,	4	/ adapter base i/o port
	.set	MM_BASE,	8	/ adapter base memory address
	.set	MM_OFFSET,	12	/ offset within segment
	.set	MM_ROW,		16	/ screen row
	.set	MM_COL,		20	/ screen column
	.set	MM_POS,		24	/ screen position
	.set	MM_ATTR,	28	/ attributes
	.set	MM_N1,		32	/ numeric argument 1
	.set	MM_N2,		36	/ numeric argument 2
	.set	MM_BROW,	40	/ base row
	.set	MM_EROW,	44	/ end row
	.set	MM_LROW,	48	/ legal row limit
	.set	MM_SROW,	52	/ saved cursor row
	.set	MM_SCOL,	56	/ saved cursor column
	.set	MM_IBROW,	60	/ initial base row
	.set	MM_IEROW,	64	/ initial end row
	.set	MM_INVIS,	68	/ cursor invisible mask
	.set	MM_SLOW,	72	/ slow [no flicker] video update
	.set	MM_WRAP,	76	/ wrap to start of next line

	.set	MM_VISIBLE,	80	// set if screen is being displayed
	.set	MM_ESC,		84	// escape char. state

	.set	MM_MSEG,	88	// heap space copy
	.set	MM_MOFF,	92	//
	.set	MM_VSEG,	96	// video memory
	.set	MM_VOFF,	100
	.set	MM_LOCKED,	104	// keyboard locked state
	.set	MM_SIZE,	108	//

	.globl	VIDSLOW		/ Patchable kernel variable.
	.globl	vtmminit

	.data
VIDSLOW:.long	0

LXXX:	.long	10		/ constant (imull)

	.text

////////
/
/ mmgo( iop, vp, index )
/ IO *iop;
/ VTDATA *vp;
/ int index;
/
////////

	.set	FRAME,32		/ ra, bx, si, di, ds, es, fs, bp

	.globl	vtmmgo

vtmmgo:
	push	%ebx
	push	%esi
	push	%edi
	push	%ds
	push	%es
	push	%fs
	push	%ebp
	movl	%esp,%ebp

	push	%ds	/ copy ds to fs
	pop	%fs

	cld
	mov	FRAME+0(%ebp),%ebx		/ iop
	mov	IO_BASE(%ebx),%esi		/ iop->io_base
	mov	IO_IOC(%ebx),%ecx		/ iop->io_ioc

	cmpl	$IOSYS,IO_SEG(%ebx)	/ user address space
	je	loc1
	mov	$SEG_386_UD,%eax
	movw	%ax,%ds
loc1:
	mov	FRAME+4(%ebp),%ebp		/ vtp
	mov	$0, %edx		/ assume not visible
	cmpb	$0, %fs:MM_VISIBLE(%ebp)	/ if this is the case, then
	je	loc2			/ don't mess with the video hw

	mov	%fs:MM_PORT(%ebp),%edx	/ turn video off if color board
	cmp	$0x3B4,%edx
	je	loc2
	cmpb	$0,%fs:MM_SLOW(%ebp)	/ check for slow [flicker-free]
	je	loc3

	mov	$0x3DA,%edx
loc4:	inb	(%dx)			/ wait for vertical retrace
	testb	$8,%al
	je	loc4
loc3:
	mov	$0x3D8,%edx		/ disable video
	movb	$0x25,%al
	outb	(%dx)
loc2:
	movb	%fs:MM_ROW(%ebp),ROW
	movb	%fs:MM_COL(%ebp),COL
	movw	%fs:MM_BASE(%ebp),%es
	mov	%fs:MM_POS(%ebp),POS
	sub	%ebx,%ebx
	movb	%fs:MM_ATTR(%ebp),ATTR

	ijmp	%fs:MM_FUNC(%ebp)

exit:	pop	%ebx
	mov	%ebx,%fs:MM_FUNC(%ebp)
	movb	ATTR,%fs:MM_ATTR(%ebp)
	movb	ROW,%fs:MM_ROW(%ebp)		/ save row,column
	movb	COL,%fs:MM_COL(%ebp)
	mov	POS,%fs:MM_POS(%ebp)		/ save position

/ ensure that only the screen associated with the keyboard
/ will actually get the cursor updated.
				/ physical index of screen to which the
	.globl	vtactive	/ keyboard is currently attached
	mov	%esp, %ebx
	mov	vtactive, %edx
	cmp	FRAME+8(%ebx), %edx	/ "index" - 3rd arg.
	jne	exit0

	mov	%fs:MM_PORT(%ebp),%edx	/ update cursor location
	mov	POS,%ebx
	add	%fs:MM_OFFSET(%ebp),%ebx	/ adjust to screen
	or	%fs:MM_INVIS(%ebp),%ebx
	shr	$1,%ebx

	movb	$14,%al
	outb	(%dx)
	inc	%edx
	movb	%bh,%al
	outb	(%dx)
	dec	%edx
	movb	$15,%al
	outb	(%dx)
	inc	%edx
	movb	%bl,%al
	outb	(%dx)
exit0:
	mov	%fs:MM_PORT(%ebp),%edx		/ turn video on
	cmp	$0, %fs:MM_VISIBLE(%ebp)		/ iff screen is visible
	je	exit1

	add	$4,%edx
	movb	$0x29,%al
	outb	(%dx)
	mov	$600,%fs:vtmmvcnt	/ 600 seconds before video disabled
exit1:
	mov	FRAME(%esp),%ebx	/ iop
	mov	%ecx,%eax
	xchg	%ecx, %fs:IO_IOC(%ebx)
	sub	%fs:IO_IOC(%ebx),%ecx
	add	%ecx,%fs:IO_BASE(%ebx)
	/ ra, bx, si, di, ds, es, fs, bp
	pop	%ebp
	pop	%fs
	pop	%es
	pop	%ds
	pop	%edi
	pop	%esi
	pop	%ebx
	ret


////////
/
/ mminit - initialize screen
/
////////
vtmminit:
	movw	%fs:MM_BASE(%ebp), %es
	movb	$0x63,%fs:MM_ESC(%ebp)		/ schedule keyboard initialization

	mov	%fs:MM_BASE(%ebp), %edx
	mov	%dx, %es
	movw	%fs:MM_BASE(%ebp), %dx

	cmp	$0, %fs:MM_VISIBLE(%ebp)
	jne	mminit0

/ DEBUG
/	mov	%fs:MM_PORT(%ebp),%edx		/ turn video off
/	add	$4,%edx
/	movb	$0x21,%al
/	outb	(%dx)
/ DEBUG

	mov	%fs:MM_PORT(%ebp),%edx		/ zero display offset
	movb	$12,%al
	outb	(%dx)
	inc	%edx
	subb	%al,%al
	outb	(%dx)
	dec	%edx
	movb	$13,%al
	outb	(%dx)
	inc	%edx
	subb	%al,%al
	outb	(%dx)

	mov	%fs:MM_PORT(%ebp),%edx	/ reset border to black
	add	$5,%edx
	subb	%al,%al
	outb	(%dx)

	inc	%edx			/ reset TECMAR XMSR register
	outb	(%dx)
mminit0:
	movl	$0,%fs:MM_INVIS(%ebp)
	movb	$0x07,ATTR
	movb	ATTR,%fs:MM_ATTR(%ebp)
	movb	$1,%fs:MM_WRAP(%ebp)
	movb	%fs:MM_IBROW(%ebp),ROW
	movb	ROW,%fs:MM_BROW(%ebp)
	movb	%fs:MM_IEROW(%ebp),%bl
	movb	%bl,%fs:MM_EROW(%ebp)
	sub	%ebx,%ebx
	movb	$2,%fs:MM_N1(%ebp)
	jmp	mm_ed

////////
/
/ mmspec - schedule special keyboard function
/
////////

mmspec:	movb	%al,%fs:MM_ESC(%ebp)
	jmp	eval

////////
/
/ mmbell - schedule beep
/
////////

mmbell:	movb	$-1,%fs:vtmmbeeps
	jmp	eval

////////
/
/ mm_cnl - cursor next line
/
/	Moves the active position to the next display line.
/	Scrolls the active display if necessary.
/
////////

mm_cnl:
	incb	ROW
	cmpb	%fs:MM_EROW(%ebp),ROW
	jna	repos
	movb	%fs:MM_EROW(%ebp),ROW

/	jmp	scrollup

////////
/
/ scrollup - scroll display upwards
/
////////

scrollup:
	push	%ds
	push	%esi
	push	%ecx

	movw	%fs:MM_BASE(%ebp),%ds
	movb	%fs:MM_BROW(%ebp),%bl
	mov	%cs:rowtab(,%ebx,4),%edi
	mov	%cs:rowtab+4(,%ebx,4),%esi

	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),%ecx

	push	%ecx
	sub	%edi,%ecx
	shr	$1,%ecx
	cld

	add	%fs:MM_OFFSET(%ebp), %esi	/ adjust for screen
	add	%fs:MM_OFFSET(%ebp), %edi

	rep
	movsw
	movb	$0x20,%al
	mov	$NCOL,%ecx

	pop	%edi
	add	%fs:MM_OFFSET(%ebp), %edi
	rep
	stosw

	sub	%fs:MM_OFFSET(%ebp), %edi
	pop	%ecx
	pop	%esi
	pop	%ds
	movb	COL,%bl			/ reposition to ROW and COL
	mov	%cs:coltab(,%ebx,4),POS
	movb	ROW,%bl
	add	%cs:rowtab(,%ebx,4),POS
	call	exit

	jmp	eval

////////
/
/ repos - reposition cursor
/
////////

repos:	movb	COL,%bl			/ reposition to ROW and COL
	mov	%cs:coltab(,%ebx,4),POS
	movb	ROW,%bl
	add	%cs:rowtab(,%ebx,4),POS
	jmp	eval

////////
/
/ Ewait - wait for next input char to evaluate
/
/ eval - evaluate input character
/
////////

ewait:
	call	exit
eval:
	jcxz	ewait

	dec	%ecx				/ evaluate next char
	lodsb
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:asctab(,%ebx,2)

////////
/
/ mmputc - put character in al on screen
/
////////

mmputc:
	add	%fs:MM_OFFSET(%ebp), POS	/ adjust for screen
	stosw				/ Update display memory.
	sub	%fs:MM_OFFSET(%ebp), POS	/ adjust for screen

	incb	COL
	cmpb	$NCOL,COL		/ Past end of line?
	jge	loc6
	jcxz	ewait			/ Not past, evaluate next character.
	dec	%ecx
	lodsb
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:asctab(,%ebx,2)

loc6:	cmpb	$0,%fs:MM_WRAP(%ebp)	/ Yes past, Wrap around?
	jne	loc7
	sub	$2,%edi			/ No wrap, adjust back to end of line.
	decb	COL
	jcxz	ewait			/ Not past, evaluate next character.
	dec	%ecx
	lodsb
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:asctab(,%ebx,2)

loc7:	subb	COL,COL		/ Wrap to next line.
	incb	ROW
	cmpb	%fs:MM_EROW(%ebp),ROW	/ Past scrolling region?
	jg	loc8
	jcxz	ewait			/ Not past, evaluate next character.
	dec	%ecx
	lodsb
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:asctab(,%ebx,2)

loc8:	movb	%fs:MM_EROW(%ebp),ROW	/ Yes past, scroll up 1 line.
	jmp	scrollup


////////
/
/ mm_cr - carriage return
/
/	Moves the active position to first position of current display line.
/
////////

mm_cr:	subb	COL,COL
	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),POS
	jcxz	ewait
	dec	%ecx
	lodsb
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:asctab(,%ebx,2)

////////
/
/ mm_cub - cursor backwards
/
////////

mm_cub:	sub	$2,POS
	decb	COL
	jge	loc9
	movb	$NCOL-1,COL
	decb	ROW
	cmpb	%fs:MM_BROW(%ebp),ROW
	jge	loc9
	subb	COL,COL
	movb	%fs:MM_BROW(%ebp),ROW
	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),POS
loc9:	jcxz	loc9a
	jmp	loc9b
loc9a:	jmp	ewait
loc9b:	dec	%ecx
	lodsb
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:asctab(,%ebx,2)

////////
/
/ Esc state - entered when last char was ESC - transient state.
/
////////

loc10:	call	exit
mm_esc:	jcxz	loc10
	dec	%ecx
	lodsb
/	movb	ZERO,%fs:MM_N1(%ebp)
/	movb	ZERO,%fs:MM_N2(%ebp)
	movb	$0, %fs:MM_N1(%ebp)
	movb	$0, %fs:MM_N2(%ebp)
	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:esctab(,%ebx,2)

////////
/
/ Csi_n1 state - entered when last two chars were ESC [
/
/	Action:	Evaluates numeric chars as numeric parameter 1.
/
////////

loc11:	call	exit
csi_n1:	jcxz	loc11
	dec	%ecx
	lodsb
	cmpb	$0x3b,%al
	je	csi_n2
	movb	%al,%bl
	subb	$0x30,%bl
	cmpb	$9,%bl
	ja	csival

	movb	%fs:MM_N1(%ebp),%al
	shlb	$2,%al
	addb	%fs:MM_N1(%ebp),%al
	shlb	$1,%al	
				/ n1 * 10

	addb	%bl,%al		/ n1 * 10 + digit
	movb	%al,%fs:MM_N1(%ebp)	/ n1 = (n1 * 10) + digit
	jmp	csi_n1

////////
/
/ Csi_n2 state - entered after input sequence ESC [ n ;
/
////////

loc12:	call	exit
csi_n2:	jcxz	loc12
	dec	%ecx
	lodsb
	movb	%al,%bl
	subb	$0x30,%bl
	cmpb	$9,%bl
	ja	csival

	movb	%fs:MM_N2(%ebp),%al
	shlb	$2,%al
	addb	%fs:MM_N2(%ebp),%al
	shlb	$1,%al	
				/ n1 * 10

	addb	%bl,%al		/ n2 * 10 + digit
	movb	%al,%fs:MM_N2(%ebp)	/ n2 = (n2 * 10) + digit
	jmp	csi_n2

csival:	movb	%al,%bl
	shlb	$1,%bl
	jc	mmputc
	ijmp	%cs:csitab(,%ebx,2)

////////
/
/ Csi_gt state - entered after input sequence ESC [ >
/	
////////

loc13:	call	exit
csi_gt:	jcxz	loc13
	dec	%ecx
	lodsb
	movb	%al,%bl
	subb	$0x30,%bl
	cmpb	$9,%bl
	ja	loc14
	mov	%fs:MM_N1(%ebp),%eax	/ n1 * 2
	imull	%cs:LXXX,%eax	/ n1 * 10
	add	%ebx,%eax	/ n1 * 10 + digit
	movb	%al,%fs:MM_N1(%ebp)	/ n1 = (n1 * 10) + digit
	jmp	csi_gt

loc14:	cmpb	$0x68,%al
	je	mm_cgh
	cmpb	$0x6c,%al
	je	mm_cgl
	jmp	eval

////////
/
/ Csi_q state - entered after input sequence ESC [ ?
/	
////////

loc15:	call	exit
csi_q:	jcxz	loc15
	dec	%ecx
	lodsb
	movb	%al,%bl
	subb	$0x30,%bl
	cmpb	$9,%bl
	ja	loc16
	mov	%fs:MM_N1(%ebp),%eax
	imull	%cs:LXXX,%eax
	add	%ebx,%eax	/ n1 * 10 + digit
	movb	%al,%fs:MM_N1(%ebp)	/ n1 = (n1 * 10) + digit
	jmp	csi_q

loc16:	cmpb	$0x68,%al
	je	mm_cqh
	cmpb	$0x6c,%al
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

mm_cbt:	orb	$7,COL			/ calculate next tab stop
	incb	COL
	subb	$16,COL		/ step back two tab positions
	jg	loc17
	subb	COL,COL		/ can't step past column 0
loc17:	jmp	repos			/ reposition cursor

////////
/
/ mm_cgh - process 'ESC [ > N1 h' escape sequence
/
/	Recognized sequences:	ESC [ > 13 h	-- Set CRT saver enabled.
/
////////

mm_cgh:	cmpb	$13,%fs:MM_N1(%ebp)
	jne	loc18
	movl	$1,%fs:vtmmcrtsav
loc18:	jmp	eval

////////
/
/ mm_cgl - process 'ESC [ > N1 l' escape sequence
/
/	Recognized sequences:	ESC [ > 13 l	-- Reset CRT saver.
/
////////

mm_cgl:	cmpb	$13,%fs:MM_N1(%ebp)
	jne	loc19
	movl	$0,%fs:vtmmcrtsav
loc19:	jmp	eval

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

mm_cha:	movb	%fs:MM_N1(%ebp),COL
	decb	COL
	jge	loc20
	subb	COL,COL
loc20:	cmpb	$NCOL,COL
	jb	loc21
	movb	$NCOL-1,COL
loc21:	jmp	repos			/ reposition cursor


////////
/
/ mm_cht - cursor horizontal tabulation
/
/	Advances the active position horizontally to the next or following
/	in a series of predetermined positions.
/
////////

mm_cht:	push	%ecx
	sub	%ecx,%ecx
	movb	COL,%cl
	orb	$7,%cl
	incb	%cl
	subb	COL,%cl
	addb	%cl,COL
	movb	$0x20,%al

	add	%fs:MM_OFFSET(%ebp), POS	/ adjust for screen
	rep
	stosw
	sub	%fs:MM_OFFSET(%ebp), POS		/ adjust for screen

	pop	%ecx
	cmpb	$NCOL,COL
	jb	loc22
	subb	$NCOL,COL
	incb	ROW
	cmpb	%fs:MM_EROW(%ebp),ROW
	jna	loc22
	movb	%fs:MM_EROW(%ebp),ROW
	jmp	scrollup
loc22:	jmp	eval

////////
/
/ mm_cpl - cursor preceding line
/
/	Moves the active position to the first position of the preceding
/	display line.
/
////////

mm_cpl:	subb	COL,COL
	decb	ROW
	cmpb	%fs:MM_BROW(%ebp),ROW
	jnb	loc23
	movb	%fs:MM_BROW(%ebp),ROW
	jmp	scrolldown
loc23:	jmp	repos			/ reposition cursor

////////
/
/ mm_cqh - process 'ESC [ ? N1 h' escape sequence
/
/	Recognized sequences:	ESC [ ? 4 h	-- Set smooth scroll.
/				ESC [ ? 7 h	-- Set wraparound.
/
////////

mm_cqh:	cmpb	$4,%fs:MM_N1(%ebp)		/ Smooth scroll.
	jne	loc24
	movb	$1,%fs:MM_SLOW(%ebp)
loc24:	cmpb	$7,%fs:MM_N1(%ebp)		/ Wraparound.
	jne	loc25
	movb	$1,%fs:MM_WRAP(%ebp)
loc25:	jmp	eval

////////
/
/ mm_cql - process 'ESC [ ? N1 l' escape sequence
/
/	Recognized sequences:	ESC [ ? 4 l	-- Set jump scroll.
/				ESC [ ? 7 l	-- Reset wraparound.
/
////////

mm_cql:	cmpb	$4,%fs:MM_N1(%ebp)		/ Jump scroll.
	jne	loc26
	movb	$0,%fs:MM_SLOW(%ebp)
loc26:	cmpb	$7,%fs:MM_N1(%ebp)		/ No wraparound.
	jne	loc27
	movb	$0,%fs:MM_WRAP(%ebp)
loc27:	jmp	eval

////////
/
/ mm_cud - cursor down
/
/	Moves the active position downward without altering the
/	horizontal position.
/
////////

mm_cud:	incb	ROW
	cmpb	%fs:MM_EROW(%ebp),ROW
	jna	loc28
	movb	%fs:MM_EROW(%ebp),ROW
loc28:	jmp	repos			/ reposition cursor

////////
/
/ mm_cuf - cursor forward
/
/	Moves the active position in the forward direction.
/
////////

mm_cuf:	incb	COL
	cmpb	$NCOL,COL
	jb	loc29
	subb	$NCOL,COL
	incb	ROW
	cmpb	%fs:MM_EROW(%ebp),ROW
	jna	loc29
	movb	%fs:MM_EROW(%ebp),ROW
	movb	$NCOL-1,COL
loc29:	jmp	repos

////////
/
/ mm_cup - cursor position
/
/	Moves the active position to the position specified by two parameters.
/	The first parameter (mm_n1) specifies the vertical position (MM_ROW(%ebp)).
/	The second parameter (mm_n2) specifies the horizontal position (MM_COL(%ebp)).
/	A parameter value of 0 or 1 for the first or second parameter
/	moves the active position to the first line or column in the
/	display respectively.
/
////////

mm_cup:	movb	%fs:MM_N1(%ebp),ROW
	decb	ROW
	jg	loc30
	subb	ROW,ROW
loc30:	addb	%fs:MM_BROW(%ebp),ROW
	cmpb	%fs:MM_EROW(%ebp),ROW
	jb	loc31
	movb	%fs:MM_EROW(%ebp),ROW
loc31:	movb	%fs:MM_N2(%ebp),COL
	decb	COL
	jg	loc32
	subb	COL,COL
loc32:	cmpb	$NCOL,COL
	jb	loc33
	movb	$NCOL-1,COL
loc33:	jmp	repos			/ reposition cursor

////////
/
/ mm_cuu - cursor up
/
/	Moves the active position upward without altering the horizontal
/	position.
/
////////

mm_cuu:	decb	ROW
	cmpb	%fs:MM_BROW(%ebp),ROW
	jge	loc34
	movb	%fs:MM_BROW(%ebp),ROW
loc34:	jmp	repos			/ reposition cursor

////////
/
/ mm_dl - delete line
/
/	Removes the contents of the active line.
/	The contents of all following lines are shifted in a block
/	toward the active line.
/
////////

mm_dl:	push	%ds
	push	%esi
	push	%ecx
	movw	%fs:MM_BASE(%ebp),%ds
	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),%edi
	mov	%cs:rowtab+4(,%ebx,4),%esi
	movb	%fs:MM_EROW(%ebp),%bl
	mov	%cs:rowtab(,%ebx,4),%ecx
	sub	%edi,%ecx
	jle	loc35
	shr	$1,%ecx

	add	%fs:MM_OFFSET(%ebp), %esi	/ adjust for screen
	add	%fs:MM_OFFSET(%ebp), %edi
	rep
	movsw
	sub	%fs:MM_OFFSET(%ebp), %esi

	mov	%cs:rowtab(,%ebx,4),%edi
	mov	$NCOL,%ecx
	movb	$0x20,%al

	add	%fs:MM_OFFSET(%ebp), %edi
	rep
	stosw
	sub	%fs:MM_OFFSET(%ebp), %edi

	subb	COL,COL
	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),%edi
loc35:	pop	%ecx
	pop	%esi
	pop	%ds
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
	movl	$1,%fs:islock
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

mm_ea:	movb	%fs:MM_N1(%ebp),%al
	cmpb	$0,%al
	jne	loc36
	movb	%fs:MM_EROW(%ebp),%bl
	jmp	mm_e0
loc36:	cmpb	$1,%al
	jne	loc37
	movb	%fs:MM_BROW(%ebp),%bl
	jmp	mm_e1
loc37:	subb	COL,COL
	movb	%fs:MM_BROW(%ebp),ROW
	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),POS
	movb	%fs:MM_EROW(%ebp),%bl
	subb	ROW,%bl
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

mm_ed:	movb	%fs:MM_N1(%ebp),%al
	cmpb	$0,%al
	jne	loc38
	movb	%fs:MM_LROW(%ebp),%bl
	jmp	mm_e0
loc38:	cmpb	$1,%al
	jne	loc39
	subb	%bl,%bl
	jmp	mm_e1
loc39:	subb	COL,COL
	movb	%fs:MM_BROW(%ebp),ROW
	sub	POS,POS
	movb	%fs:MM_LROW(%ebp),%bl
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

mm_el:	movb	%fs:MM_N1(%ebp),%al
	movb	ROW,%bl
	cmpb	$0,%al
	je	mm_e0
	cmpb	$1,%al
	je	mm_e1
	mov	%cs:rowtab(,%ebx,4),POS
	subb	COL,COL
	subb	%bl,%bl
/	jmp	mm_e2

mm_e2:	push	%ecx
	movb	$0x20,%al

loc40:	mov	$NCOL,%ecx
	add	%fs:MM_OFFSET(%ebp), POS
	rep
	stosw
	sub	%fs:MM_OFFSET(%ebp), POS

	decb	%bl
	jge	loc40
	pop	%ecx
	jmp	repos

mm_e1:	push	%ecx
	mov	POS,%ecx
	mov	%cs:rowtab(,%ebx,4),POS
	sub	POS,%ecx
	jl	loc41
	movb	$0x20,%al
	shr	$1,%ecx

	add	%fs:MM_OFFSET(%ebp), POS
	rep
	stosw
	sub	%fs:MM_OFFSET(%ebp), POS

loc41:	pop	%ecx
	jmp	repos

mm_e0:	push	%ecx
	mov	%cs:rowtab+4(,%ebx,4),%ecx
	sub	POS,%ecx
	jl	loc42
	movb	$0x20,%al
	shr	$1,%ecx

	add	%fs:MM_OFFSET(%ebp), POS
	rep
	stosw
	sub	%fs:MM_OFFSET(%ebp), POS

loc42:	pop	%ecx
	jmp	repos

////////
/
/ mm_emi - enable manual input
/
/	Clear flag preventing keyboard input.
/
////////

mm_emi:
	movl	$0,%fs:islock
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
mm_il:	push	%ds
	push	%esi
	push	%ecx
	movw	%fs:MM_BASE(%ebp),%ds
	movb	%fs:MM_EROW(%ebp),%bl
	mov	%cs:rowtab(,%ebx,4),%esi
	mov	%esi,%ecx
	sub	$2,%esi
	mov	%cs:rowtab+4(,%ebx,4),%edi
	sub	$2,%edi
	movb	ROW,%bl
	sub	%cs:rowtab(,%ebx,4),%ecx
	jle	loc43
	shr	$1,%ecx
	std

	add	%fs:MM_OFFSET(%ebp), %esi
	add	%fs:MM_OFFSET(%ebp), %edi
	rep
	movsw
	sub	%fs:MM_OFFSET(%ebp), %esi

	mov	%cs:rowtab(,%ebx,4),%edi
	mov	$NCOL,%ecx
	movb	$0x20,%al
	cld

	add	%fs:MM_OFFSET(%ebp), %edi
	rep
	stosw
	sub	%fs:MM_OFFSET(%ebp), %edi

	subb	COL,COL
	movb	ROW,%bl
	mov	%cs:rowtab(,%ebx,4),%edi
loc43:	pop	%ecx
	pop	%esi
	pop	%ds
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

mm_hpa:	movb	%fs:MM_N1(%ebp),COL
	decb	COL
	jg	loc44
	subb	COL,COL
loc44:	cmpb	$NCOL,COL
	jb	loc45
	movb	$NCOL-1,COL
loc45:	jmp	repos			/ reposition cursor

////////
/
/ mm_hpr - horizontal position relative
/
/	Moves the active position forward the number of positions specified
/	by the parameter.  A parameter value of zero or one indicates a
/	single-position move.
/
////////

mm_hpr:	movb	%fs:MM_N1(%ebp),%al
	orb	%al,%al
	jne	loc46
	incb	%al
loc46:	addb	%al,COL
	cmpb	$NCOL,COL
	jb	loc47
	movb	$NCOL-1,COL
loc47:	jmp	repos			/ reposition cursor

////////
/
/ mm_hvp - horizontal and vertical position
/
/	Moves the active position to the position specified by two parameters.
/	The first parameter specifies the vertical position (MM_ROW(%ebp)).
/	The second parameter specifies the horizontal position (MM_COL(%ebp)).
/	A parameter value of zero or one moves the active position to the
/	first line or column in the display.
/
////////

mm_hvp:	movb	%fs:MM_N1(%ebp),ROW
	decb	ROW
	jg	loc48
	subb	ROW,ROW
loc48:	cmpb	%fs:MM_LROW(%ebp),ROW
	jna	loc49
	movb	%fs:MM_LROW(%ebp),ROW
loc49:	movb	%fs:MM_N2(%ebp),COL
	decb	COL
	jg	loc50
	subb	COL,COL
loc50:	cmpb	$NCOL,COL
	jb	loc51
	movb	$NCOL-1,COL
loc51:	jmp	repos			/ reposition cursor

////////
/
/ mm_ind - index
/
/	Causes the active position to move downward one line without changing
/	the horizontal position.  Scrolling occurs if below scrolling region.
/
////////

mm_ind:	incb	ROW
	cmpb	%fs:MM_EROW(%ebp),ROW
	jg	loc52
	jmp	repos
loc52:	movb	%fs:MM_EROW(%ebp),ROW
	jmp	scrollup

////////
/
/ mm_new - save cursor position
/
////////

mm_new:	movb	COL,%fs:MM_SCOL(%ebp)
	movb	ROW,%fs:MM_SROW(%ebp)
	jmp	eval

////////
/
/ mm_old - restore old cursor position
/
////////

mm_old:	movb	%fs:MM_SCOL(%ebp),COL
	movb	%fs:MM_SROW(%ebp),ROW
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
	cmpb	%fs:MM_BROW(%ebp),ROW
	jge	loc53
	movb	%fs:MM_BROW(%ebp),ROW
	jmp	scrolldown
loc53:	jmp	repos

////////
/
/ mm_scr - select cursor rendition
/
/	Invokes the cursor rendition specified by the parameter.
/
/	Recognized renditions are:	0 - cursor visible
/					1 - cursor invisible
////////

mm_scr:	decb	 %fs:MM_N1(%ebp)
	je	loc54
	jg	loc55
	movl	$0,%fs:MM_INVIS(%ebp)
	jmp	eval

loc54:	movl	$-1,%fs:MM_INVIS(%ebp)
loc55:	jmp	eval

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

mm_sgr:	movb	%fs:MM_N1(%ebp),%al

	cmpb	$0,%al			/ reset all = 0
	jne	loc56
	movb	$0x07,ATTR
loc57:	jmp	eval

loc56:	cmpb	$1,%al			/ bold =  1
	jne	loc58
	orb	$INTENSE,ATTR
	jmp	loc57

loc58:	cmpb	$4,%al			/ underline = 4
	jne	loc59
	cmp	$0x03D4,%fs:MM_PORT(%ebp)	/ color card?
	je	loc57			/ yes, ignore underline
	andb	$0x88,ATTR
	orb	$0x01,ATTR
	jmp	loc57

loc59:	cmpb	$5,%al			/ blinking = 5
	jne	loc60
	orb	$BLINK,ATTR
	jmp	loc57

loc60:	cmpb	$7,%al			/ reverse video = 7
	jne	loc61
	movb	$0x70,%al
	cmp	$0x3D4,%fs:MM_PORT(%ebp)	/ color card?
	jne	loc62
	movb	ATTR,%al		/ yes, exchange foreground/background
	andb	$0x77,%al
	rolb	$1,%al
	rolb	$1,%al
	rolb	$1,%al
	rolb	$1,%al
loc62:	andb	$0x88,ATTR
	orb	%al,ATTR
	jmp	loc57

loc61:	cmpb	$8,%al			/ concealed on = 8
	jne	loc63
	cmp	$0x3D4,%fs:MM_PORT(%ebp)	/ color card?
	jne	loc64

	andb	$0x70,ATTR		/ Yes,	Set foreground color
	movb	ATTR,%al		/	to background color.
	rorb	$1,%al
	rorb	$1,%al
	rorb	$1,%al
	rorb	$1,%al
	orb	%al,ATTR
	jmp	loc57

loc64:	andb	$0x80,ATTR		/ No, set attributes to non-display.
	jmp	loc57			/	retain blink attribute.

loc63:	cmp	$0x03D4,%fs:MM_PORT(%ebp)	/ color card?
	jne	loc57			/ no, ignore remaining options
loc65:	subb	$30,%al			/ foreground color
	jl	loc66
	cmpb	$7,%al
	jg	loc67
	movb	%al,%bl
	andb	$0xf8,ATTR
	orb	%cs:fcolor(%ebx),ATTR
	jmp	loc66
loc67:	subb	$10,%al
	jl	loc66
	cmpb	$7,%al
	jg	loc68
	movb	%al,%bl
	andb	$0x8f,ATTR
	orb	%cs:bcolor(%ebx),ATTR
	jmp	loc66
loc68:	subb	$10,%al
	jl	loc66
	cmpb	$7,%al
	jg	loc69
	movb	%al,%bl
	movb	%cs:fcolor(%ebx),%al
	push	%edx
	mov	%fs:MM_PORT(%ebp),%edx
	add	$5,%edx
	outb	(%dx)
	pop	%edx
/	jmp	loc66
loc69:
loc66:	jmp	eval

////////
/
/ mm_ssr - set scrolling region
/
////////

mm_ssr:	movb	%fs:MM_N1(%ebp),%al
	decb	%al
	jge	loc70
	subb	%al,%al
loc70:	cmpb	%fs:MM_LROW(%ebp),%al
	ja	loc71
	movb	%fs:MM_N2(%ebp),%bl
	decb	%bl
	jge	loc72
	subb	%bl,%bl
loc72:	cmpb	%fs:MM_LROW(%ebp),%bl
	ja	loc71
	cmpb	%bl,%al
	ja	loc71
	movb	%al,%fs:MM_BROW(%ebp)
	movb	%bl,%fs:MM_EROW(%ebp)
	movb	%al,ROW
	subb	COL,COL
loc71:	jmp	repos

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

mm_vpa:	movb	%fs:MM_N1(%ebp),ROW
	decb	ROW
	jg	loc73
	subb	ROW,ROW
loc73:	cmpb	%fs:MM_LROW(%ebp),ROW
	jna	loc74
	movb	%fs:MM_LROW(%ebp),ROW
loc74:	jmp	repos			/ reposition cursor

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

mm_vpr:	movb	%fs:MM_N1(%ebp),%al
	orb	%al,%al
	jne	loc75
	incb	%al
loc75:	addb	%al,ROW
	cmpb	%fs:MM_LROW(%ebp),ROW
	jb	loc76
	movb	%fs:MM_LROW(%ebp),ROW
loc76:	jmp	repos			/ reposition cursor

////////
/
/ asctab - table of functions indexed by ascii characters
/
////////

	.align	4
asctab:	.long	eval,	eval,	eval,	eval	/* NUL  SOH  STX  ETX  */
	.long	eval,	eval,	eval,	mmbell	/* EOT  ENQ  ACK  BEL  */
	.long	mm_cub,	mm_cht,	mm_cnl,	mm_ind	/* BS   HT   LF   VT   */
	.long	eval,	mm_cr,	eval,	eval	/* FF   CR   SO   SI   */
	.long	eval,	eval,	eval,	eval	/* DLE  DC1  DC2  DC3  */
	.long	eval,	eval,	eval,	eval	/* DC4  NAK  SYN  ETB  */
	.long	eval,	eval,	eval,	mm_esc	/* CAN  EM   SUB  ESC  */
	.long	eval,	eval,	eval,	eval	/* FS   GS   RS   US   */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/*   ! " # \040 - \043 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* $ % & ' \044 - \047 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* ( ) * + \050 - \053 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* , - . / \054 - \057 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* 0 1 2 3 \060 - \063 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* 4 5 6 7 \064 - \067 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* 8 9 : ; \070 - \073 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* < = > ? \074 - \077 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* @ A B C \100 - \103 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* D E F G \104 - \107 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* H I J K \110 - \113 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* L M N O \114 - \117 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* P Q R S \120 - \123 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* T U V W \124 - \127 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* X Y Z [ \130 - \133 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* \ ] ^ _ \134 - \137 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* ` a b c \140 - \143 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* d e f g \144 - \147 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* h i j k \150 - \153 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* l m n o \154 - \157 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* p q r s \160 - \163 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* t u v w \164 - \167 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* x y z { \170 - \173 */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* | } ~ ? \174 - \177 */

////////
/
/ esctab - table of functions indexed by ESC characters.
/
////////

esctab:	.long	mmputc,	mmputc,	mmputc,	mmputc	/* NUL  SOH  STX  ETX  */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* EOT  ENQ  ACK  BEL  */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* BS   HT   LF   VT   */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* FF   CR   SO   SI   */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* DLE  DC1  DC2  DC3  */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* DC4  NAK  SYN  ETB  */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* CAN  EM   SUB  ESC  */
	.long	mmputc,	mmputc,	mmputc,	mmputc	/* FS   GS   RS   US   */
	.long	eval,	eval,	eval,	eval	/*   ! " # \040 - \043 */
	.long	eval,	eval,	eval,	eval	/* $ % & ' \044 - \047 */
	.long	eval,	eval,	eval,	eval	/* ( ) * + \050 - \053 */
	.long	eval,	eval,	eval,	eval	/* , - . / \054 - \057 */
	.long	eval,	eval,	eval,	eval	/* 0 1 2 3 \060 - \063 */
	.long	eval,	eval,	eval,	mm_new	/* 4 5 6 7 \064 - \067 */
	.long	mm_old,	eval,	eval,	eval	/* 8 9 : ; \070 - \073 */
	.long	eval,	mmspec,	mmspec,	eval	/* < = > ? \074 - \077 */
	.long	eval,	eval,	eval,	eval	/* @ A B C \100 - \103 */
	.long	mm_ind,	mm_cnl,	eval,	eval	/* D E F G \104 - \107 */
	.long	eval,	eval,	eval,	eval	/* H I J K \110 - \113 */
	.long	eval,	mm_ri,	eval,	eval	/* L M N O \114 - \117 */
	.long	eval,	eval,	eval,	eval	/* P Q R S \120 - \123 */
	.long	eval,	eval,	eval,	eval	/* T U V W \124 - \127 */
	.long	eval,	eval,	eval,	csi_n1	/* X Y Z [ \130 - \133 */
	.long	eval,	eval,	eval,	eval	/* \ ] ^ _ \134 - \137 */
	.long	mm_dmi,	eval,	mm_emi,	vtmminit /* ` a b c \140 - \143 */
	.long	eval,	eval,	eval,	eval	/* d e f g \144 - \147 */
	.long	eval,	eval,	eval,	eval	/* h i j k \150 - \153 */
	.long	eval,	eval,	eval,	eval	/* l m n o \154 - \157 */
	.long	eval,	eval,	eval,	eval	/* p q r s \160 - \163 */
	.long	mmspec,	mmspec,	eval,	eval	/* t u v w \164 - \167 */
	.long	eval,	eval,	eval,	eval	/* x y z { \170 - \173 */
	.long	eval,	eval,	eval,	eval	/* | } ~ ? \174 - \177 */


////////
/
/ csitab - table of functions indexed by ESC [ characters.
/
////////

csitab:	.long	eval,	eval,	eval,	eval	/* NUL  SOH  STX  ETX  */
	.long	eval,	eval,	eval,	eval	/* EOT  ENQ  ACK  BEL  */
	.long	eval,	eval,	eval,	eval	/* BS   HT   LF   VT   */
	.long	eval,	eval,	eval,	eval	/* FF   CR   SO   SI   */
	.long	eval,	eval,	eval,	eval	/* DLE  DC1  DC2  DC3  */
	.long	eval,	eval,	eval,	eval	/* DC4  NAK  SYN  ETB  */
	.long	eval,	eval,	eval,	eval	/* CAN  EM   SUB  ESC  */
	.long	eval,	eval,	eval,	eval	/* FS   GS   RS   US   */
	.long	eval,	eval,	eval,	eval	/*   ! " # \040 - \043 */
	.long	eval,	eval,	eval,	eval	/* $ % & ' \044 - \047 */
	.long	eval,	eval,	eval,	eval	/* ( ) * + \050 - \053 */
	.long	eval,	eval,	eval,	eval	/* , - . / \054 - \057 */
	.long	eval,	eval,	eval,	eval	/* 0 1 2 3 \060 - \063 */
	.long	eval,	eval,	eval,	eval	/* 4 5 6 7 \064 - \067 */
	.long	eval,	eval,	eval,	eval	/* 8 9 : ; \070 - \073 */
	.long	eval,	eval,	csi_gt,	csi_q	/* < = > ? \074 - \077 */
	.long	eval,	mm_cuu,	mm_cud,	mm_cuf	/* @ A B C \100 - \103 */
	.long	mm_cub,	mm_cnl,	mm_cpl,	mm_cha	/* D E F G \104 - \107 */
	.long	mm_cup,	mm_cht,	mm_ed,	mm_el	/* H I J K \110 - \113 */
	.long	mm_il,	mm_dl,	eval,	mm_ea	/* L M N O \114 - \117 */
	.long	eval,	eval,	eval,	mm_ind	/* P Q R S \120 - \123 */
	.long	mm_ri,	eval,	eval,	eval	/* T U V W \124 - \127 */
	.long	eval,	eval,	mm_cbt,	eval	/* X Y Z [ \130 - \133 */
	.long	eval,	eval,	eval,	eval	/* \ ] ^ _ \134 - \137 */
	.long	mm_hpa,	mm_hpr,	eval,	eval	/* ` a b c \140 - \143 */
	.long	mm_vpa,	mm_vpr,	mm_hvp,	mm_cup	/* d e f g \144 - \147 */
	.long	eval,	eval,	eval,	eval	/* h i j k \150 - \153 */
	.long	eval,	mm_sgr,	eval,	eval	/* l m n o \154 - \157 */
	.long	eval,	eval,	mm_ssr,	eval	/* p q r s \160 - \163 */
	.long	eval,	eval,	mm_scr,	eval	/* t u v w \164 - \167 */
	.long	eval,	eval,	eval,	eval	/* x y z { \170 - \173 */
	.long	eval,	eval,	eval,	eval	/* | } ~ ? \174 - \177 */

////////
/
/ coltab - integer array of offsets to each column
/
////////

coltab:	.long	 0<<SCB,	 1<<SCB,	 2<<SCB,	 3<<SCB
	.long	 4<<SCB,	 5<<SCB,	 6<<SCB,	 7<<SCB
	.long	 8<<SCB,	 9<<SCB,	10<<SCB,	11<<SCB
	.long	12<<SCB,	13<<SCB,	14<<SCB,	15<<SCB
	.long	16<<SCB,	17<<SCB,	18<<SCB,	19<<SCB
	.long	20<<SCB,	21<<SCB,	22<<SCB,	23<<SCB
	.long	24<<SCB,	25<<SCB,	26<<SCB,	27<<SCB
	.long	28<<SCB,	29<<SCB,	30<<SCB,	31<<SCB
	.long	32<<SCB,	33<<SCB,	34<<SCB,	35<<SCB
	.long	36<<SCB,	37<<SCB,	38<<SCB,	39<<SCB
	.long	40<<SCB,	41<<SCB,	42<<SCB,	43<<SCB
	.long	44<<SCB,	45<<SCB,	46<<SCB,	47<<SCB
	.long	48<<SCB,	49<<SCB,	50<<SCB,	51<<SCB
	.long	52<<SCB,	53<<SCB,	54<<SCB,	55<<SCB
	.long	56<<SCB,	57<<SCB,	58<<SCB,	59<<SCB
	.long	60<<SCB,	61<<SCB,	62<<SCB,	63<<SCB
	.long	64<<SCB,	65<<SCB,	66<<SCB,	67<<SCB
	.long	68<<SCB,	69<<SCB,	70<<SCB,	71<<SCB
	.long	72<<SCB,	73<<SCB,	74<<SCB,	75<<SCB
	.long	76<<SCB,	77<<SCB,	78<<SCB,	79<<SCB

////////
/
/ rowtab - array of offsets to each row
/
////////

rowtab:	.long	0, 160, 320, 480 
	.long	640, 800, 960, 1120
	.long	1280, 1440, 1600, 1760
	.long	1920, 2080, 2240, 2400
	.long	2560, 2720, 2880, 3040
	.long	3200, 3360, 3520, 3680
	.long	3840, 4000, 4160, 4320
	.long	4480, 4640, 4800, 4960


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
	.globl	vtmm_voff
vtmm_voff:
	push	%ebp
	mov	%esp, %ebp
	mov	8(%ebp), %ebp		/ VTERM *
	mov	%ds:MM_PORT(%ebp), %edx

	add	$4,%edx
	movb	$0x21,%al
	outb	(%dx)
	pop	%ebp
	ret

////////
/
/ mm_von( vp )	-- turn video display on
/ VTDATA *vp;
////////
	.globl	vtmm_von
vtmm_von:
	push	%ebp
	mov	%esp, %ebp
	mov	8(%ebp), %ebp		/ VTERM *
	mov	%ds:MM_PORT(%ebp), %edx

	add	$4,%edx
	movb	$0x29,%al
	outb	(%dx)
	mov	$900,vtmmvcnt		/ 900 seconds before video disabled
	pop	%ebp
	ret

	.globl	vtds_sel
vtds_sel:
	mov	$0, %eax
	mov	%ds, %ax
	ret
