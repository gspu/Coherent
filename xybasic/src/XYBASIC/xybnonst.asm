	title	'XYBASIC	REV 2.14.00	05/27/81'

;VERSION 05/18/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;version EQUates

true	equ	-1
false	equ	0
	page	0		;lines per page on PRN listing

;version selection switches
versn	equ	21400		;current version number
cpm	equ	false		;iff CP/M version
nonst	equ	true 		;iff nonstandard (Custom I/O) version
isis2	equ	false		;iff ISIS-II version

;conditional assembly switches
strng	equ	true		;iff string version
float	equ	true		;iff floating point version
rom	equ	nonst		;iff ROM version (NONST only)
romsq	equ	false		;iff ROMSQ version
compl	equ	false		;iff COMPILER version (NONST ROM only)
rtpak	equ	false		;iff RUNTIME version (ROMSQ COMPL only)
editc	equ	true		;iff editing commands AUTO, DELETE, EDIT, RENUM
sdisk	equ	false		;iff sequential disk version (CP/M STRNG only)
f9511	equ	false 		;iff 9511 floating point chip version
fpbcd	equ	false		;iff BCD floating point, 10 digit mantissa
realt	equ	false		;iff SBC 80/20 real time clock
for0	equ	true		;iff e.g. FOR I = 1 TO 0 never entered
asm80	equ	false		;iff assembling under ASM80
key80	equ	true 		;iff more than 80H keywords
debug	equ	false		;iff debugging
				;CPM and DEBUG: ^B gets SID, ^X echoes to disk
				;ISIS2 and DEBUG: ^B gets monitor
				;NONST:  DEBUG has no effect
				;PACKI and DEBUG: 8251 initialization skipped

;customized versions
amd	equ	false		;iff AMD
bendx	equ	false		;iff Bendix (STRNG NONST only)
camac	equ	false 		;iff KineticSystems (STRNG, NONST or CP/M only)
c3908	equ	false 		;iff CAMAC version for 3908, false iff 3909
c3885	equ	false 		;iff CP/M SDISK CAMAC version for 3885
epstn	equ	false		;iff Epstein (NONST only)
genmc	equ	false		;iff General Microwave (NONST only)
packi	equ	false		;iff Packard Instruments
wild	equ	false		;iff Wild Heerbrugg

;location
	if	asm80
	name	xybasic		;ISIS-II module name for ASM80
	cseg			;start of relocatable code segment
	else
romorg	equ	00100H		;first ROM location
	if	rom
ramorg	equ	04000H		;first RAM location
	endif
	endif
	org	romorg

;global EQUates
maxnl	equ	8		;max var name length(>=7 for string conversion)
inttn	equ	8		;max # of interrupt table entries (< 32)
inttl	equ	inttn * 8 + 1	;int table length = n 8-byte entries + eof
randa	equ	64B5H		;pseudorandom number generator multiplier
randc	equ	3A73H		;pseudorandom number generator constant
stakl	equ	80H		;stack length
stakm	equ	25		;max stack use without recursion by GTEXP
vbyts	set	3		;bytes per ESTACK value entry if integer only
dbyts	set	10		;bytes needed for string space and file areas
nlmax	equ	80		;input buffer length
	if	strng
stmax	equ	4		;number of string temporaries
vbyts	set	4		;bytes per value entry if string
dbyts	set	dbyts+100H	;100H bytes for string space
	endif
	if	float
	if	fpbcd		;BCD floating point
vbyts	set	7		;bytes per value entry
fbyts	equ	6		;bytes per FOR-entry value
	else			;binary floating point
vbyts	set	5		;bytes per value entry if floating
fbyts	equ	4		;bytes per FOR-entry value
sindx	equ	19		;# significant binary places
	endif
	else			;not floating point version
fbyts	equ	2		;bytes per FOR-entry value if integer
	endif

;ASCII character equivalents
cntlb	equ	02H
cntld	equ	04H
cntle	equ	05H
cntlf	equ	06H
cntlg	equ	07H
cntlh	equ	08H
lf	equ	0AH
cntlk	equ	0BH
cntll	equ	0CH
cr	equ	0DH
cntln	equ	0EH
cntlo	equ	0FH
cntlq	equ	11H
cntlr	equ	12H
cntls	equ	13H
cntlt	equ	14H
cntlx	equ	18H
cntly	equ	19H
cntlz	equ	1AH
semic	equ	3BH
	if	not epstn
cntlc	equ	03H
cntlp	equ	10H
cntlu	equ	15H
rbout	equ	7FH
	else			;Epstein version control chars
cntlc	equ	0AH
cntlp	equ	04H
cntlu	equ	0BH
rbout	equ	1AH
escap	equ	1BH
	endif

;information passed to compiler
	if	compl and not rtpak
	db	remt		;REM token value
	dw	romend+1-romorg	;ROM use
	dw	ramend+1-ramorg	;RAM use
	endif

start:	jmp	init		;start initialization dialog
	jmp	gtpar		;to get parameters for CALL

;nonstandard system EQUates
	if	nonst and not camac
timem	equ	8C13H		;time counts
timen	equ	0FE12H
timeh	equ	73H
	if	not wild
	if	genmc
headl	equ	17		;header length
	else
headl	equ	13
	endif
stbyt	equ	3AH		;start byte
tybyt	equ	0		;type byte -- for future use
;jump vector for i/o routines
	jmp	conin
	jmp	cnout
	jmp	rdrin
	jmp	pout
	jmp	lout
	jmp	cstat
;jmp table for user-defined device drivers
jmpta:	rept	24
	jmp	0
	endm
	if	compl
boot:	jmp	0		;absolute address, else compilation relocates
	else
boot:	jmp	dmodc		;^B same as ^C in nonstandard versions
	endif
	endif			;end of NOT WILD conditional
	endif			;end of NONST conditional

;CP/M EQUates
	if	cpm
cpmba	equ	0H		;CP/M base address
iobyt	equ	cpmba+3H	;io byte address
bdos	equ	cpmba+5H	;DOS entry point
fcbad	equ	cpmba+5CH	;default file control block address
fcbfn	equ	fcbad + 1	;file name address
fcbft	equ	fcbad + 9	;file type address
fcbcr	equ	fcbad + 32	;current record number
dmaad	equ	cpmba+80H	;default DMA address
dkopf	equ	15		;disk open function
dkclf	equ	16		;close
dkdlf	equ	19		;delete
dkrdf	equ	20		;read
dkwtf	equ	21		;write
dkmkf	equ	22		;make
dkdmf	equ	26		;set DMA address function
timem	equ	8C13H		;TIME counts
timen	equ	0FE12H
timeh	equ	73H
	if	amd		;serial SAVE/LOAD for AMD version
headl	equ	13
stbyt	equ	3AH		;start byte
tybyt	equ	0		;type byte -- for future use
	endif
	if	sdisk
filen	equ	166		;number of bytes per sequential access file
dbyts	set	dbyts+2*filen	;number of bytes needed
	endif
	if	debug
boot	equ	cpmba+38H	;^B gets SID instead of CP/M
	else
boot	equ	cpmba		;^B gets CP/M if not debugging
	endif
	endif			;end of CPM conditional

;ISIS-II EQUates
	if	isis2
isis	equ	40H		;ISIS-II entry point
dkopf	equ	0		;disk open function
dkclf	equ	1		;close
dkrdf	equ	3		;read
dkwrf	equ	4		;write
exitf	equ	9		;exit to ISIS-II
erri2	equ	12		;error
timem	equ	947BH		;time counts
timen	equ	0FE36H
timeh	equ	6BH
;MDS monitor EQUates
conin	equ	0F803H		;console in
cnout	equ	0F809H		;console out
lout	equ	0F80FH		;list out
cstat	equ	0F812H		;console status
mechk	equ	0F81BH		;top of free RAM function
iobyt	equ	3		;iobyte location
	endif			;end of ISIS2 conditional

;CAMAC RTPAK i/o JMP vector
	if	camac and nonst and true 
conin:	jmp	0
cnout:	jmp	0
rdrin:	jmp	0
pout:	jmp	0
lout:	jmp	0
cstat:	jmp	0
boot:	jmp	0
	endif

;Wild version EQUates
	if	wild
	if	rtpak
	jmp	wtoke		;tokenization module entry point
weom:	dw	6000H		;default end of RAM
boot	equ	0H		;boot after program completion in rtpak
	endif
wmon	equ	38H		;Wild monitor entry point
wdbl	equ	80H		;length of central data buffer -- 254 max
wdbuf	equ	80H		;base address of central data buffer
wexec	equ	40H		;base address of 18-byte EXEC buffer
	endif

;ROMSQ version default value bytes
	if	romsq and not wild
dfltw:	db	0		;default width
dfltm:	dw	1		;default end of memory
dflta:	dw	0		;default program address
	endif

;CAMAC version boot and conditional interrupt enable on reentry
	if	camac and nonst and (not true )
boot:	jmp	nsddi		;DI and return to DOS
	if	not rtpak
	call	cndei		;conditional EI, then DMODE
	endif
	endif

;Packard Instruments version XYBASIC reentry and interrupt entry point
	if	packi
	jmp	dmod2		;to DMODE
	jmp	paisr		;interrupt service routine entry point
	endif

;real-time clock version clock tick routine entry point
	if	realt
	jmp	dmod2		;to DMODE
	jmp	clock		;clock routine entry
	endif

;IFTYP is a macro to simplify coding of conditional type branches.
iftyp	macro	faddr,saddr
	if	float
	jnz	faddr		;;branch to floating routine
	endif
	if	strng
	jc	saddr		;;branch to string routine
	endif			;;else fall through to integer routine
	endm

;PUSH3 (and PUSH4) is a macro to save BC, DE, HL (and PSW) on routine entry.
;The corresponding exit is JMP POP3 (POP4), defined in INOUT.
push3	macro
	push	b
	push	d
	push	h
	endm
push4	macro
	push3
	push	psw
	endm

;DZ is a macro to fill blocks of RAM with 0 (used like DS).
dz	macro	n
	rept	n
	db	0
	endm
	endm

;RIM and SIM are 8085 opcodes to read and set interrupt mask.
rim	macro
	db	20H
	endm
sim	macro
	db	30H
	endm

;ERROR is a macro to define error calls.
;TYP may be F, N or C for fatal, nonfatal, or continue scanning.
;M1 and M2 are the error message, M2 being the last byte.
error	macro	typ, m1, m2
	call	erro&TYP		;;call ERROF, ERRON or ERROC
	db	'&M1', '&M2' or 80H	;;error message
	endm


;end of VERSION
	page
;AUX 11/19/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;statement auxilliary routines

;general purpose routines used in statement execution

;LINBC gets current line # in BC.
;Call:	(LNNUM)	current line # address, 0 if direct
;Retn:	A	clobbered
;	BC	if (LNNUM) = 0 then 0 else ((LNNUM))
;	DE	preserved
;	HL	if (LNNUM) = 0 then 0 else (LNNUM) + 1
;	Zero	set iff (LNNUM) = 0
linbc:	lhld	lnnum
linb1:	mov	a,h
	ora	l
	jz	linb2		;line # is 0, i.e. direct
	mov	c,m
	inx	h
	mov	b,m		;line # to BC
	ret
linb2:	mov	b,a
	mov	c,a		;0 to BC
	ret

;IDTST issues a fatal ID error if current statement is direct, i.e. (LNNUM)=0.
;IITST performs EOS test, then issues fatal II error if current is indirect.
	if	compl
idtst	equ	linbc		;just return current line in BC
	else
idtst:	call	linbc		;zero set iff direct
	rnz
iderr:	error	f, I, D		;fatal ID error
iitst:	call	eos
	call	linbc		;zero set iff direct
	rz			;ok if direct
	error	f, I, I		;issue fatal II error
	endif			;end of NOT COMPL conditional

;RTEST returns Zero set iff location HL is RAM.
rtest:	mov	a,m		;fetch byte
	cma
	mov	m,a		;store complement
	cmp	m		;compare to stored value
	cma
	mov	m,a		;restore value
	ret

	if	romsq and not compl
;ISSRC returns if SOURC address working space, issues fatal RO error otherwise.
issrc:	lxi	d,srcad
	lhld	sourc
	call	cmdhu		;compare SOURC to SRCAD
	rz			;addressing working space, OK
roerr:	error	f, R, O		;fatal RO error

;ISROM checks if current program is running in ROM, issues nonfatal RO error
;and scans to next command if so.
isrom:	lhld	sourc
	call	rtest		;test if ROM
	rz			;RAM, OK
	error	c, R, O		;nonfatal RO error and scan on

	endif			;end of ROMSQ AND NOT COMPL conditional

;EOS checks for garbage on end of statement, and is called by routines which
;alter normal control flow (GOTO, GOSUB, RETURN, etc.).
;Falls through to syntax error if next nonspace char is not :, ' or <cr>.
;Retn:	A	next nonspace char
;	BC,DE	preserved
;	HL	address of next nonspace char (i.e. of :, ' or <cr>)
eos:	call	dtst0
	rnc			;ok if next is delimiter
snerr:	error	c, S, N		;issue SN error and scan to next
exerr:	error	f, E, X		;fatal EX error

	if	compl
uferr:	error	c, U, F		;issue UF error and scan to next
	endif

;DTEST tests whether A contains a delimiter.
;Retn:	Carry	reset iff (A) = :, ' or <cr>
;	Registers	preserved
dtst0:	call	gtcho
dtest:	cpi	':'
	rz
dtst1:	cpi	cr
	rz
	if	rtpak or not compl	;comments already purged if compiled
	cpi	''''
	rz
	endif
	stc
	ret

;GTLHS gets a destination variable reference.  It calls GTVAR with
;INLHS true (to indicate tracing may be desired) and stores the
;destination returned in LHSAD for ASSGN to perform assignment.
gtlhs:	if	not compl
	mvi	a,255
	sta	inlhs		;set INLHS to true
	endif
	call	gtvar		;perform variable reference
	jc	snerr		;no variable name
	sta	lhsty		;save type
	shld	lhsad		;save destination
	if	not compl
	mov	b,a		;save type
	xra	a
	sta	inlhs		;reset INLHS to false
	mov	a,b		;restore type
	endif
	ret

;ASIGN is CALLed by FOR, NEXT, LET, READ, INPUT to change a variable's value.
;The value addressed by HL is assigned to the destination LHSAD.
;Trace information is printed if (VTRAC) is negative.
asigv:	call	esval		;value to ESTACK
	lhld	estkp
	inx	h		;point to value
asign:	lda	lhsty		;desired type to A
	call	cnvrt		;convert value to desired type
	if	strng
	cpi	strst
	cz	scopv		;copy to string space if type string
	endif
	call	bytsd		;# bytes in value to DE
	inx	h		;point to value
	mov	b,h
	mov	c,l		;value source to BC
	lhld	lhsad		;destination to HL
	if	compl
	jmp	moved		;value to destination and return if COMPL
	else
	push	psw		;save type
	push	b		;and save value location
	call	moved		;value to destination
	pop	h		;value location to HL
	pop	b		;value type to B
				;and fall through to TRVAL
;TRVAL prints variable value if tracing is desired.
;Call:	VTRAC	bit 7 set iff tracing desired
;	B	value type
;	HL	value location
trval:	lda	vtrac
	ora	a
	rp			;done if not tracing
trva1:	mvi	a,'='		;BTEST entry point
	call	writc		;write the =
	dcx	h		;point to type
	mov	a,b		;fetch type
	if	strng
	cpi	strst
	jnz	prval		;print nonstring value
	xchg
	lxi	h,stemp
	inr	m		;increment STEMP count before fetching string
	xchg
	endif
	jmp	prval		;print the value and return
	endif			;end of NOT COMPL conditional

;ASIGI performs traced integer assignment for NEXT and SCALL value return.
asigi:	mov	m,b
	dcx	h
	mov	m,c		;value to destination
	if	not compl
	push	b
	mvi	b,intst
	call	trval		;print trace info if desired
	pop	b
	endif
	ret

;BYTSD returns with the number of bytes in an value in DE.
bytsd:	lxi	d,vbyts-1
	if	float
	cpi	sngst
	rz			;return 4 if floating
	if	fpbcd
	mvi	e,3
	else
	dcx	d
	endif
	endif
	if	strng
	cpi	strst
	rz			;return 3 if string
	endif
	if	float or strng
	dcx	d
	endif
	ret			;return 2 if integer

;FETCH fetches a value addressed by HL and returns its type in the status bits.
;Call:	HL	pointer to type
;Retn:	A	type token
;	BC	value if integer, Carry reset and Zero set
;	C,DE	length, location if string, Carry set and Zero set
;	FACC	value if floating, Carry reset and Zero reset
;FETBC does a FETCH of arg in BC.
fetbc:	mov	h,b
	mov	l,c
fetch:	mov	a,m		;fetch the type token
fetc1:	if	float
	cpi	sngst
	jz	fetcf		;fetch floating
	endif
	if	strng
	cpi	strst
	jz	fetcs		;fetch string
	endif
fetci:	cpi	intst
	jnz	exerr
	if	float
	shld	argad		;save location for retry of ambiguous op
	endif
mobcm:	inx	h		;Zero is set, Carry reset
	mov	c,m
	inx	h
	mov	b,m		;fetch integer to BC
	ret
	if	strng
fetcs:	inx	h
	mov	c,m		;length to C
	call	modem		;location to DE
	lxi	h,stemp
	dcr	m		;decrement # string temps in use
	jm	exerr		;EX error if negative
	xra	a		;Zero is set
	stc			;and Carry set also
	ret
	endif
	if	float
	if	f9511		;9511 version fetches to 9511 stack
fetcf:	push	h
	push	b
	inx	h		;point to first value byte
	call	lod95		;load value to 9511 stack
	pop	b		;restore BC
	pop	h		;restore HL
	mvi	a,sngst		;floating point token to A
	ora	a		;clear Carry and Zero
	ret			;and return
	else			;NOT F9511
fetcf:	push3			;save registers
	inx	h		;point to value
	call	fload		;load value to FACC
	mvi	a,sngst		;return type in A
	ora	a		;Carry reset, Zero reset
	jmp	pop3		;restore registers and return
	endif
	endif

;MVDEM returns (M):(M-1) in DE, (HL)-2 in HL
;BC, PSW preserved
mvdem:	mov	d,m
	dcx	h
	mov	e,m
	dcx	h
	ret

;MVMDE moves (DE) to (M-1):(M-2), returns (HL)-2 in HL
mvmde:	dcx	h
	mov	m,d
	dcx	h
	mov	m,e
	ret

;MODEM moveds (M+2):(M+1) to DE, returns (HL)+2 in HL.
modem:	inx	h
	mov	e,m
	inx	h
	mov	d,m
	ret

;ISBYT checks whether (B) = 0.  If so, it returns.
;If not, a nonfatal BY error is issued and B is set to 0.
;A clobbered, B forced to 0, other registers preserved.
isbyt:	mov	a,b
	ora	a		;clear carry, set zero iff (B) = 0
	rz
	mvi	b,0		;force (B) to 0
	error	n, B, Y		;nonfatal BYte error
	ret			;and return

;CPLDE replaces (DE) with its two's complement.
;CPLD1 replaces (DE) with its one's complement.
;Retn:	A	clobbered
;	BC,HL	preserved
;	DE	two's complemented
;	Carry	set iff (DE) = 8000H, i.e. overflow
cplde:	dcx	d
cpld1:	mov	a,e		;entry point to one's complement DE --
	cma			;   NB carry set if called with 7FFFH
	mov	e,a
	mov	a,d
	cma
	mov	d,a
	xri	80H
	ora	e		;zero set iff (DE) = 8000H
	rnz
	cmc			;set carry if overflow
	ret

;ADAHL adds (A) + (HL), leaves result in HL and sets carry on overflow.
adahl:	add	l
	mov	l,a
	rnc			;done if no carry
	inr	h		;else inc high order
	rz			;return with carry set iff overflow
	cmc
	ret

;SBAHL subtracts (HL) - (A), leaves result in HL and sets carry on underflow.
sbahl:	sub	l		;(A) - (L) to A, carry set iff L > A
	cma			;L - A - 1
	mov	l,a
	inx	h		;L - A
	rc
	dcr	h
	ret

;CMBDU compares (BC) to (DE) as 16 bit unsigned integers.
;Retn:	A	clobbered
;	BC,DE,HL	unchanged
;	Zero	set iff (BC) = (DE)
;	Carry	set iff (BC) < (DE)
cmbdu:	mov	a,b
	cmp	d		;carry set iff (B) < (D)
	rnz			;finished unless (B) = (D)
	mov	a,c		;(B) = (D), so compare (C) to (E)
	cmp	e
	ret

;CMDHU same as CMBDU except DE::HL
cmdhu:	mov	a,d
	cmp	h
	rnz
	mov	a,e
	cmp	l
	ret

;CMBDS compares (BC) to (DE) as 16 bit signed (two's complement) integers.
;Retn:	same as CMBDU above.
cmbds:	mov	a,b
	xra	d		;sign set iff signs agree
	jp	cmbdu		;unsigned compare works when signs agree
	mov	a,b		;signs disagree, sign of B gives result
	ral			;NB zero is reset from xra above
	ret

;FLIP is called by READ and INPUT to exchange text pointers in TEXTP and TXTP2.
;Clobbers DE, preserves PSW, leaves TEXTP in HL.
flip:	lhld	txtp2
	xchg			;TXTP2 to DE
	lhld	textp
	shld	txtp2		;TEXTP to TXTP2
	xchg
	shld	textp		;TXTP2 to TEXTP
	ret

;DMODX is a common exit for various versions of LOAD and EXEC.
;First the control and expr stacks are reset.  Then execution continues
;if the command was direct, and XYBASIC returns to DMODE if indirect
;(since the source program has been changed by the EXEC or LOAD).
;Call:	HL	eof address of new program
	if	not compl
dmodx:	call	new1		;reset stacks
	call	linbc		;Zero set iff direct
	rz			;continue normally if direct
	jmp	dmod2		;otherwise to DMODE
	endif

;BDTST is called from FOR and NEXT to test FOR-loop termination.
;Call:	BC or FACC	new FOR-variable value (integer or floating)
;	DE	increment pointer
;	HL	bound pointer
;	LHSTY	FOR-variable type
;Retn:	Carry	Set iff exit condition satisfied, i.e. value>bound and inr>=0
;			or value<bound and inr<0
bdtst:	inx	d		;point to second incr byte
	if	float
	lda	lhsty
	cpi	sngst
	jnz	bdts1		;integer type
	push	d
	if	f9511
	call	lod95		;load bound to 9511 stack
	call	cmpf0		;perform floating compare, no arg switch
	else
	call	cmpfl		;do floating compare
	endif
	pop	d
	rz			;value=bound, return Carry reset
	if	f9511 or fpbcd
	dcx	d		;first byte has sign of 9511, BCD
	endif
	ldax	d
	rlc			;incr sign to Carry
	rm			;value<bound, return incr sign as result
	cmc			;value>bound, return complemented incr sign
	ret
	endif
bdts1:	ldax	d
	mov	e,m
	inx	h
	mov	d,m		;integer bound to DE
	rlc
	cnc	bcde		;exchange value and bound if incr>=0
	jmp	cmbds		;and CMBDS returns desired Carry status

;FNDST is called from FOR and READ to scan through a program for
;the matching NEXT or next DATA statement.
;Call:	(textp)	text address at which scanning is to begin
;	B	token to be matched (NEXT or DATA)
;		(1) Found				(2) Notfound
;Retn:	A	token					0
;	B	preserved				preserved
;	C,DE	clobbered				clobbered
;	HL	address of next unparsed text char	eof address
;	Carry	reset					set
;	(textp)	ditto					eof address - 1
fnds0:	call	dtst0		;check if at delimiter
	cmc
	rnc			;return carry reset if not
				;else empty DATA, fall through to retry
fndst:	mvi	c,1		;initialize FOR count
	lxi	d,4		;to skip bytes after <cr>
fnds1:	call	gtcha		;get next char
	cmp	b
	jz	fnds3		;found one
	cpi	':'
	jz	fnds1		;multiple statements -- look at next
	if	rtpak or not compl	;comments purged if compiled
	cpi	''''
	cz	rem		;on-line comment -- scan to <cr>
	endif
	cpi	cr
	jz	fnds2		;cr
	cpi	fort
	cz	inrc		;increment FOR-count if FOR
	call	gtdel		;scan to delimiter
	jmp	fnds1		;keep trying
inrc:	inr	c
	ret
fnds2:	mov	a,m		;fetch next line length byte
	ora	a		;check for end of file
	jz	bkupc		;return carry set if failed
	dad	d		;point to next text byte
	shld	textp		;store new pointer
	jmp	fnds1		;and keep looking
fnds3:	cpi	datat
	jz	fnds0		;done if DATA
fnds4:	call	gtnam		;look for var name after NEXT
	dcr	c		;decrement count
	jnc	fnds5
	cmc			;clear carry in case found
	rz			;done if NEXT and FOR count is 0
	jmp	fnds1		;and keep looking
fnds5:	rz			;return if counted to 0
	call	gtcom		;look for comma after <var name>
	jc	fnds1		;none
	jmp	fnds4		;else look for more

;FNDLN finds the line # address of the line which DE points into.
;Used by READ for DATA syntax errors.
;Call:	DE	pointer into text
;Retn:	BC,DE	preserved
;	HL	line # address of desired text line
fndln:	if	not romsq
	lxi	h,srcad
	else
	lhld	sourc
	endif
fnln1:	shld	temp		;save length byte address
	mov	a,m		;fetch length
	call	adahl		;address next line
	call	cmdhu		;compare to desired pointer
	jnc	fnln1		;keep looking
	lhld	temp		;restore length byte addr
	inx	h		;point to line #
	ret

	if	realt
;CLOCK is the interrupt service routine to tick the real-time clock.
;The interrupt branches to 10H, POKEd during initialization to come here.
;The four bytes at TIMEX contain 20ths-20, seconds-60, minutes-60 and hours-24.
clock:	push	psw
	push	h
	lxi	h,timex		;address 20ths counter
	inr	m		;tick it
	jnz	clocx		;done
	mvi	m,255 and -20	;reset to -20
	inx	h		;address seconds counter
	inr	m		;tick it
	jnz	clocx		;done
	mvi	m,255 and -60	;reset to -60
	inx	h		;address minutes counter
	inr	m		;tick...
	jnz	clocx		;done
	mvi	m,255 and -60	;reset
	inx	h		;address hours counter
	inr	m		;tick...
	jnz	clocx		;done
	mvi	m,255 and -24	;welcome to tomorrow
clocx:	pop	h
	mvi	a,20H
	out	0D8H		;reinitialize the clock
	pop	psw
	ei			;reenable interrupts
	ret
	endif

	if	editc and (not compl)
;LNREF changes all occurrences of <line #>s in source text for RENUM.
lnref:	lxi	h,srcad		;begin at the beginning
;process next line of source text
lnre1:	mov	a,m		;fetch length byte
	ora	a
	rz			;eof, done
	push	h
	inx	h
	inx	h
	inx	h
	inx	h		;address first text byte
;process next byte of source text
lnre2:	mov	a,m		;fetch a text byte
	inx	h
	cpi	cr		;check if end of line
	jz	lnre5		;yes
	cpi	''''
	jz	lnre4		;on-line comment, scan to <cr>
	cpi	remt
	jz	lnre4		;REM, scan to <cr>
	if	key80
	cpi	20H
	jc	lnre3		;reserved word token, check it
	endif
	cpi	80H
	jc	lnre2		;not a token, try next
;found a <token>, check if <line #> can and does follow
lnre3:	call	kltst		;test if token can have <line #> following
	jc	lnre2		;no, try next
	sta	txtp2		;save token in TXTP2 in case ON list or LIST
	shld	textp		;set TEXTP to scan possible <line #>
lnr3j:	call	gtcho		;skip spaces, if any
	push	h		;save HL pointing to first nonspace
	call	gtlno		;look for <line #>
	xthl			;restore HL
	pop	b		;first nondigit location to BC
	jc	lnre2		;not a <line #>, try next byte
;found a <line #>
	push	h		;save first for insertion of new line #
	xchg
	call	cplde		;- first
	xchg
	dad	b		;last + 1 - first = <line #> length to HL
	push	h		;save length
	call	findl		;look for <line #>
	jc	lnr3c		;not found, flag the line
	push	h		;save location
	lhld	rnold
	xchg
	call	findl		;find location of first renumbered line
	pop	d		;location of desired line to DE
	call	cmdhu
	jc	lnr3d		;before renumbered lines, unchanged
	push	h		;save first loc
	lhld	rninc
	mov	b,h
	mov	c,l		;increment to BC
	lhld	rnnew		;first destination line # to HL
	jz	lnr3b		;matched, take first line #
;compute new <line #> corresponding to old <line #>
lnr3a:	xthl			;first line loc to HL
	mov	a,m
	call	adahl		;address next line
	call	cmdhu		;compare to desired line #
	xthl
	dad	b		;compute new line #
	jnz	lnr3a		;no match, try next
;convert new <line #> to string and compare to length of old
lnr3b:	pop	d		;discard saved location
	mov	b,h
	mov	c,l		;new line # to BC
	xra	a
	call	cvtis		;and converted to string, no leading char
	call	bcde		;location to BC, length to E
	pop	h		;length of old line # to L
	sub	l		;new length - old length
	pop	h		;old line # loc to HL
	jm	lnr3e		;old line # longer
	jnz	lnr3f		;old line # shorter
	call	movd0		;move new line # to replace old
	jmp	lnr3h		;and keep scanning
;old <line #> not found, flag bit 7 of line break byte
lnr3c:	pop	h		;discard saved length
	pop	h		;and discard saved first loc
	pop	h		;length byte addr to HL
	push	h		;and resaved
	inx	h
	inx	h
	inx	h		;address break byte
	mvi	m,80H		;set bit 7 to indicate line # not found
	jmp	lnr3h		;and keep scanning
;old <line #> precedes renumbered lines, leave unchanged
lnr3d:	pop	h		;discard saved length
	pop	h		;and discard saved first loc
	jmp	lnr3h		;and keep scanning
;old <line #> longer than new <line #>
lnr3e:	push	psw		;save offset
	call	movd0		;copy new line # to old place
	shld	textp		;and set new TEXTP
	xchg			;new destination to DE
	pop	psw		;restore offset
	pop	h		;line length pointer to HL
	push	psw
	add	m		;add offset
	mov	m,a		;and store new line length
	pop	psw
	push	h		;resave line length pointer
	cma
	inr	a		;complement offset
	xchg			;restore destination to HL
	push	h		;save destination
	call	adahl		;+offset = source
	push	h
	xchg
	call	cplde		;-source
	lhld	eofad
	inx	h
	dad	d		;count bytes to move
	xchg			;to DE
	pop	b
	pop	h
	call	moved		;block move the remaining text
	dcx	h
	shld	eofad		;store new eof
	jmp	lnr3h		;and keep scanning
;old <line #> shorter than new
lnr3f:	push	psw
	push	b
	push	d
	push	h		;save all
	xchg			;first old line # byte addr to DE
	lhld	eofad
	mov	b,h
	mov	c,l		;end of file to BC
	call	adahl		;offset + eof = new eof
	shld	eofad		;store new eof
lnr3g:	ldax	b		;fetch a text byte
	mov	m,a		;and store in new location
	dcx	b
	dcx	h
	mov	a,e
	cmp	c
	jnz	lnr3g
	mov	a,d
	cmp	b
	jnz	lnr3g		;copy more text bytes
	pop	h
	pop	d
	pop	b		;restore new line # info
	call	movd0		;and copy line # into text
	shld	textp
	pop	psw		;offset
	pop	h		;line length addr
	add	m		;old length + offset
	mov	m,a		;gives new length
	push	h
lnr3h:	lda	txtp2		;recover token preceding <line #>
	lhld	textp
	cpi	gotot
	jz	lnr3i		;GOTO
	cpi	gsubt
	jz	lnr3i		;GOSUB
	cpi	listt
	jnz	lnre2		;not GOTO, GOSUB nor LIST, keep scanning
lnr3i:	call	gtcom		;look for comma
	jc	lnre2		;none, keep scanning
	jmp	lnr3j		;look for next element in <line #> list
;scan to next <cr>
lnre4:	mov	a,m
	inx	h
	cpi	cr
	jnz	lnre4
;end of source text line, try the next
lnre5:	pop	h		;length byte addr to HL
	mov	a,m
	call	adahl		;address next line
	jmp	lnre1		;and try next line

;KLTST tests if token in A may have <line #> following.
;Call:	A	Token
;Retn:	C	Clobbered
;	A,B,DE,HL	Preserved
;	Carry	Set iff not found
kltst:	push	h
	mvi	c,klnct		;table count to C
	lxi	h,klnta		;table addr to HL
klts1:	cmp	m		;compare token to table entry
	jz	klts2		;matched
	inx	h
	dcr	c
	jnz	klts1		;try next
	stc			;not found
klts2:	pop	h		;restore HL
	ret			;and return
	endif			;end of EDITC conditional

	if	not compl
;BKNAM constructs a 3-byte symbol table 'name' at BUFAD for a line break.
;Byte 1 is H6-H0, byte 2 is L6-L0, both with bit 7 reset.
;Byte 3 has 1,H7,L7 in bits 7-5 and bits 4-0 reset.
;Call:	HL	desired break entry 'name' (i.e. line # addr)
bknam:	xchg			;desired line # addr to DE
	lxi	h,bufad
	mov	a,d
	ani	7FH
	mov	m,a		;byte 1 = D6-D0 to BUFAD
	inx	h
	mov	a,e
	ani	7FH
	mov	m,a		;byte 2 = E6-E0 to BUFAD+1
	inx	h
	mov	a,d
	ral			;D7 to Carry
	mov	a,e
	rar			;D7, E7 to A7, A6
	stc
	rar			;1, D7, E7 to A7-5
	ani	0E0H		;mask off A4-0
	mov	m,a		;byte 3 to BUFAD+2
	ret
	endif			;end of NOT COMPL conditional

;GTPAR gets a parameter from CALL command line.
;Parameters must be <var ref> or *<array var name>.
;Retn:	A	0 if no more params, 1 if integer, 2 if string, 3 if floating
;	B	bytes per entry
;	C	# dims
;	DE	address of first dimension
;	HL	address of first data item
gtpar:	call	gtcnd		;look for comma not followed by delimiter
	mvi	a,0
	rc			;no more parameters, return 0
	mvi	d,multt
	call	gtd		;look for *
	jnc	gtpa1		;array passed
	call	gtvar		;else var ref
	lxi	b,0		;# dims = 0 to C
	push	b		;push 0 for DE
	jnc	gtpa2		;and continue below
mcerr:	error	f, M, C		;fatal MC error
gtpa1:	call	fdvar		;look for var name
	jc	mcerr
	ldax	d		;fetch type
	mov	c,m		;# dims to C
	mvi	b,0		;to allow dad
	inx	h		;point to first dim byte
	push	h		;and save
	dad	b
	dad	b		;point to first data byte
gtpa2:	call	bytsd		;bytes per entry to DE
	mov	b,e		;and then to B
	pop	d		;dim addr to DE
	mov	a,b
	dcr	a		;A gets 1 for int, 2 string, 3 floating
	ret

;DISAB disables all interrupts
	if	not wild
disab:	xra	a
	sta	inttc		;reset interrupt table count
	sta	intad		;clear interrupt table
	ret

;<byte expr> , <byte expr> [, <byte expr>] [,$]
;IINFO gets interrupt information for ENABLE and WAIT.
;Four bytes corresponding to the first four bytes of an interrupt table
;entry are returned in BCDE (type, port, mask, value).
iinfo:	call	gtbex
	mov	b,c		;port # to  B
	mvi	c,0C0H		;type to C
	push	b		;and saved
	call	gtcbe
	mov	d,c		;value to D
	mvi	e,0		;mask 0 for now
	call	gtcom
	jc	iinf3		;default mask 0, null $
	call	gtcho
	cpi	'$'
	jz	iinf1		;default mask 0, $
	call	gtbex
	mov	e,c		;mask to E
	call	gtcom
	jc	iinf2		;null $
iinf1:	call	gtcha
	cpi	'$'
	jnz	snerr
	mov	a,d
	cma
	ora	e
	mov	d,a		;value = NOT value OR mask (if $)
	pop	b
	mvi	c,0E0H		;set type $ bit
	ret
iinf2:	mov	a,d
	ora	e
	mov	d,a		;value = value OR mask (if null $)
iinf3:	pop	b
	ret
	endif			;end of NOT WILD conditional


;end of AUX
	page
;CPM 5/21/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;CP/M version SAVE and LOAD, including GTFIL

	if	cpm		;CP/M versions

	if	rtpak		;UF error in Runtime Module version
save	equ	uferr
load	equ	uferr
	else

;SAVE <filename> [,A]
save:	call	prntm		;print SAVING message
	db	'SAVING', ' ' or 80H
	call	gtfil		;read filename, initialize file control block
	mov	a,b
	ora	a
	jnz	snerr		;SN error if ,R specified
	mov	a,c
	if	amd
	cpi	2
	jz	saves		;save via PUN device in AMD version
	endif
	ora	a
	push	psw		;save ,A status
	call	cdkmk		;delete old one, make new one
	pop	psw		;restore ,A info
	jz	savea		;ASCII save
	if	amd
	jm	savea		;HEX save in AMD version
	endif
	call	rtdma		;reset DMA address just in case
	if	romsq
	call	last		;last to BC
	push	b		;save last
	lhld	sourc		;first source address to HL
	if	amd
	dcx	h
	endif
	else
	lhld	eofad		;eof address to HL
	push	h		;save last
	lxi	h,srcad		;next source address to HL
	endif
;EOF address is stacked, next address to save is in HL
save1:	mov	b,h
	mov	c,l		;next address to BC
	lxi	d,80H
	dad	d		;current + 80H = next to HL
	push	h		;save next
	lxi	h,dmaad		;destination = default DMA address
	call	moved		;move current block to default DMA address
	call	cdkwt		;write the block
	pop	h		;next to HL
	pop	d		;EOF address to DE
	push	d		;and saved again
	call	cmdhu		;compare
	jnc	save1		;eof >= current -- keep going
	pop	d		;unstack saved EOF address
cdkcl:	mvi	c,dkclf
cdkc1:	call	bdosf		;close it
	cpi	255
	rnz			;done if closed successfully
dkerr:	call	rtdma		;reset DMA address after errors
	error	f, D, K		;fatal DK error
rtdma:	lxi	d,dmaad		;default DMA address to DE
stdma:	mvi	c,dkdmf		;set DMA address to (DE)
	if	c3885 and not debug
	jmp	bdos1		;disable interrupts if 3885
	else
	jmp	bdos
	endif
bdosf:	lxi	d,fcbad		;fcb address to DE
	if	c3885 and not debug
bdos1:	lda	imode		;fetch current interrupt mode status
	ora	a
	jnz	bdos		;disabled, just do the BDOS call
	di			;enabled, disable around BDOS call
	call	bdos
	ei
	ret
	else
	jmp	bdos		;just do it
	endif
cdkmk:	mvi	c,dkdlf
	call	bdosf		;delete old one
	mvi	c,dkmkf
	jmp	cdkc1		;make new one
cdkwt:	mvi	c,dkwtf
	call	bdosf
	ora	a
	rz
	jmp	dkerr
savea:	if	amd
	mov	b,a		;save HEX or ASCII status
	endif
	call	sprst		;reset disk buffer pointer
	lda	omode
	push	psw		;save output mode
	mvi	a,80H
	sta	omode		;80H to OMODE
	if	amd
	mov	a,b
	ora	a
	jm	saveh		;HEX save
	endif
	if	romsq
	lhld	sourc
	else
	lxi	h,srcad
	endif
	lxi	b,-1
	call	list1		;list the program to disk
sava1:	mvi	c,cntlz
	call	dkout		;end of file
	call	cdkwt		;write the last record
	pop	psw
	sta	omode		;restore omode
	jmp	cdkcl		;close file and return

	if	amd		;HEX or serial SAVE in AMD version

saveh:	lhld	sourc
	dcx	h		;first - 1 to HL
	push	h		;and saved
	xchg
	call	cplde		;complemented
	xchg
	shld	temp		;-first+1 saved in TEMP
	call	last		;last to HL
	xthl			;first -1 to HL
	pop	d		;last to DE
;convert the file to hex and write it
savh1:	push	h		;save current
	lxi	b,10H		;max record length to BC
	dad	b		;current + max to HL
	call	cmdhu		;compare eof to current + max
	pop	h		;restore current
	mov	a,c		;max to A
	jnc	savh2		;eof >= current + max, write max
	mov	a,e
	sub	l
	inr	a		;eof+1 - current = remaining to be written
	jz	savh4		;current = eof+1, just write eof record
savh2:	push	d		;save eof address
	mov	e,a		;length to  E
	mvi	d,0		;checksum to D
	mvi	c,':'
	call	dwrit		;write record mark
	mov	a,e
	call	wbyte		;write length
	call	waddr		;write address and record type
savh3:	mov	a,m		;fetch data byte
	inx	h
	call	wbyte		;write data
	dcr	e
	jnz	savh3		;write more data
	call	wcsum		;write the checksum
	pop	d		;restore eof address
	jmp	savh1		;and do more
;write eof record
savh4:	mvi	c,':'
	call	dwrit		;write :
	xra	a
	mov	d,a		;checksum to D
	call	wbyte		;write record length = 0
	lhld	sourc
	dcx	h		;first - 1 to HL
	call	waddr		;write starting address 0 and record type
	call	wcsum		;write the checksum
	jmp	sava1		;write eof, close and return

;save to serial device (PUNch)
saves:	lxi	b,fcbad+1	;filename source
	lxi	h,headr+2	;filename destination
	mvi	e,8		;filename length
	call	movd0		;copy filename to headr
	lhld	sourc
	dcx	h		;first to HL
	push	h
	push	h		;and saved
	call	last		;last to HL
	pop	d		;first to DE
	call	cplde		;-first to DE
	inx	d		;-first + 1
	dad	d		;last - first + 1 = length
	push	h		;and saved
	lxi	h,headr
	mvi	e,headl		;header length to E
savs1:	mov	c,m		;header char to C
	call	pout		;and out to punch device
	inx	h
	dcr	e
	jnz	savs1		;keep sending header chars
	pop	d		;file length to DE
	pop	h
savs2:	inx	d
	push	d		;save length+1
	mov	a,d
	ora	a		;zero set iff length < 255
	jz	savs3
	mvi	e,0
savs3:	dcr	e		;length of block to E
	mvi	c,stbyt
	call	pout		;send start  byte
	mvi	c,tybyt
	call	pout		;send type byte
	mov	c,e
	call	pout		;send length byte
	mov	a,e
	ora	a		;check if length = 0
	jz	savs5		;yes, done
	call	ctest		;check for console break char
	mvi	d,0		;checksum in D
savs4:	mov	c,m
	call	pout		;send source char
	mov	a,m
	add	d
	mov	d,a		;update checksum
	inx	h
	dcr	e
	jnz	savs4		;send more source chars
	mov	c,d
	call	pout		;send checksum
savs5:	pop	d		;recover length + 1  to DE
	mov	a,d
	ora	a
	rz			;done if length < 255
	dcr	d		;else new length = length+1-256 = length-255
	jmp	savs2		;and save more blocks

	endif			;end of AMD conditional

;LOAD <filename> [,A] [,R]
load:	if	romsq
	call	issrc		;must be addressing working space
	endif
	call	prntm		;print LOADING message
	db	'LOADING', ' ' or 80H
	if	amd
	call	gtcom		;look for comma
	jnc	lods0		;LOAD without filename in AMD version
	endif
	call	gtfil		;read file name, initialize FCB
	if	amd
	mov	a,c
	cpi	2
	jz	loads		;load from RDR device in AMD version
	endif
	push	b		;save ,A and ,R info
	mvi	c,dkopf
	call	bdosf		;try to open it
	cpi	255
	jnz	load0		;successful open
	pop	b
	mvi	c,0		;reset C to indicate ,A
	push	b		;and save
	call	gtfl4		;reset file type to .BAS
	call	cdkop		;and try to open .BAS file
load0:	call	new		;clobber old program
	pop	b
	mov	a,c
	ora	a
	jz	loada		;ASCII load
	if	amd
	jm	loadh		;HEX load
	endif
lod0b:	push	b		;save ,R status -- initialization entry point
	lhld	symta
	lxi	d,-7FH
	dad	d		;first bad dma address to HL
	if	amd
	lxi	d,srcad-1
	else
	lxi	d,srcad
	endif
load1:	call	cmdhu		;compare to see how much space still available
	jnc	loadx		;program too large -- OM error
	push	h		;save bad address
	push	d		;save destination
	call	stdma		;set DMA address
	call	cdkrd		;read a record
	pop	h
	pop	d		;restore destination to HL, bad addr to DE
	jz	load2		;eof
	lxi	b,80H
	dad	b		;find next destination
	xchg			;destination to DE, bad addr to HL
	jmp	load1
loada:	lda	omode
	mov	c,a
	push	b		;save OMODE and ,R status
	call	dloa1		;read a record, set buffer pointer
	mvi	a,7FH
	sta	omode		;set OMODE for ASCII load
loda1:	call	gtlin		;get a line
	call	tkize		;tokenize it
	jc	loda1		;ignore if no line #
	cnz	addln		;add to source
	jmp	loda1		;and keep loading

	if	amd		;HEX or serial LOAD in AMD version

loadh:	lda	omode
	mov	c,a
	push	b
	call	dloa1
	mvi	a,7FH
	sta	omode
lodh1:	call	readc		;read a char from hex file
	sui	':'
	jnz	lodh1		;not record mark, retry
	mov	d,a		;0 for checksum to D
	call	rbyte		;read a file byte
	jz	lodh3		;eof record
	mov	e,a		;else record length to E
	call	rbyte		;read destination msb
	push	psw		;and save
	call	rbyte		;read destination lsb
	pop	h		;destination msb to H
	mov	l,a		;destination now in HL
	lxi	b,srcad-1	;base address to BC
	dad	b		;add to base address for actual destination
	call	rbyte		;skip type byte
lodh2:	call	rbyte		;read a data byte
	mov	m,a		;and store
	inx	h		;address next
	dcr	e
	jnz	lodh2		;load another data byte
	call	rbyte		;read checksum
	jnz	loadx		;checksum error
	dcx	h		;point to last loaded byte
	call	new1		;and reset EOFAD in case end of program
	jmp	lodh1		;load next record
lodh3:	call	readc		;keep reading chars
	jmp	lodh3		;until EOF in readc exits from LOAD

;serial LOAD via RDR device
loads:	mov	a,b
	push	psw		;save ,R status
	lxi	b,fcbad+1
	lxi	h,headr+2
	mvi	e,8
	call	movd0		;move filename to header block
	jmp	lods1		;and continue as below
lods0:	mvi	d,'S'
	call	gtdsn		;skip S after comma
	call	gtcom		;look for comma before ,R
	mvi	a,0
	jc	lodsa		;not ,R
	mvi	d,'R'
	call	gtdsn		;skip R after comma
	mvi	a,1
lodsa:	push	psw		;save ,R status	
	lxi	h,headr+2	;first filename char address to HL
	lxi	b,8		;0 to B, # filename chars to C
	call	fillm		;fill filename with 0s
lods1:	lxi	h,headr
	mvi	c,headl
	call	ctest		;check for console break char
lods2:	call	rdrin		;read a char
	cmp	m		;compare to header char
	jz	lods3		;matched, try next
	mov	a,m		;else fetch header char
	ora	a		;check if null, i.e. LOAD ,S typed
	jnz	lods1		;not null, try again from the top
lods3:	inx	h
	dcr	c
	jnz	lods2		;see if next matches too
	call	new		;got the file header, prepare to load
	lxi	h,srcad-1	;load address to HL
lods4:	call	rdrin		;read start byte
	cpi	stbyt		;check if start byte
	jnz	cserr		;issue CS error if not
	call	ctest		;check for console break char
	call	rdrin		;read type byte
	cpi	tybyt		;check if type byte
	jnz	cserr		;issue CS error if not
	call	rdrin		;read length byte
	ora	a
	jz	lods6		;block length 0, done
	mov	e,a		;block length to E
	inr	a
	push	psw		;save length+1
	mvi	d,0		;checksum to D
lods5:	call	rdrin		;read a char
	mov	m,a		;store it
	inx	h
	add	d
	mov	d,a		;update checksum
	dcr	e
	jnz	lods5		;more chars in block
	call	rdrin		;read the checksum
	cmp	d
	jnz	cserr		;checksum error
	pop	psw		;recover block length+1
	jz	lods4		;length was 255, so load more blocks
	dcx	h		;point to new eof adress
lods6:	pop	psw		;recover ,R status
	ora	a
	jz	dmodx		;reset stacks and continue iff direct and no ,R
	call	new1		;else reset stacks
	jmp	loadr		;and run the program
cserr:	call	new		;erase the garbage
	error	f, C, S		;fatal CS error

	endif			;end of AMD conditional

cdkop:	mvi	c,dkopf
	if	sdisk
	call	bdosf
	cpi	255
	rnz			;successful open
	error	f, F, N		;fatal FN error
	else
	jmp	cdkc1		;open file
	endif
cdkrd:	mvi	c,dkrdf
	call	bdosf		;read a record
	cpi	2
	jz	dkerr		;read error
	cpi	1
	ret			;Zero set iff eof
loadx:	call	new		;clobber bad fragment
	jmp	omerr		;and issue fatal OM error
load2:	call	rtdma		;reset DMA address to default
	lxi	d,-1
	call	findl		;find bottom of source text
	mov	a,m
	call	adahl
	pop	psw		;recover ,R status
	ora	a
	jz	dmodx		;no ,R specified, so reset stacks as usual
	call	new1		;set new eof, cstack, estack
				;and fall through to LOADR to run
loadr:	call	loadz		;reset LNNUM and TEXTP
	jmp	run		;and RUN the program
loadz:	call	lnnu0		;reset LNNUM to 0
	lxi	h,nlnad
	shld	textp		;reset TEXTP to input buffer
	mvi	m,cr		;and <cr> to input buffer
	ret

	if	debug
dsav0:	lda	amode
	cma
	sta	amode		;toggle saving mode if ^D
	ora	a
	jz	dsav3
	mvi	c,7
	lxi	d,libad
	lxi	h,fcbad+9
dsav1:	ldax	d
	mov	m,a		;set filetyp to LIB
	inx	d
	inx	h
	dcr	c
	jnz	dsav1		;store another char
	sta	fcbcr		;set current record to 0
	call	cdkmk		;delete old one, make new one
	call	sprst		;reset buffer pointer
dsav2:	mvi	a,cntlx
	jmp	pop3		;and return
dsav3:	mvi	c,cntlz
	call	dkout		;write a control-z as eof
	call	cdkwt		;write the last record
	call	cdkcl		;and close the file
	jmp	dsav2
libad	db	'LIB', 0, 0, 0, 0
	endif			;end of ^d conditional

;write char to disk file for ASCII SAVE and PRINT @foo
dkout:	lhld	sptr		;write one char to buffer
	mov	m,c
	inx	h
	shld	sptr		;update pointer
	mov	a,h
	ora	a
	rz			;buffer not full -- done
	call	cdkwt		;write a record
sprst:	lxi	h,80H
	shld	sptr		;reset SPTR to base of buffer
	ret

;read char from disk for ASCII LOAD and [L]INPUT @foo
dload:	lhld	sptr		;buffer pointer to HL
	mov	a,h
	ora	a
	cnz	dloa1		;read another record
	mov	a,m		;fetch next char
	ani	7FH		;remove parity bit
	inx	h
	shld	sptr		;move up pointer
	cpi	cntlz
	jnz	pop3		;return unless eof
	lxi	sp,stack-4	;OMODE and NEXTS return pushed
	pop	b		;recover ,R status and OMODE
	mov	a,c
	sta	omode		;recover OMODE
	mov	a,b
	ora	a
	jz	dmod2		;return to direct mode if no ,R
	jmp	loadr		;else RUN the program
dloa1:	call	cdkrd		;read a record
	call	sprst		;reset pointer
	rnz			;return unless eof
	mvi	m,cntlz
	ret

;GTFIL gets <filename> [,A] [,R] and initializes a file control block to
;<filename>.XYB or <filename>.BAS.  The name may consist of a letter followed
;by adjacent printable chars, and is padded by spaces to 8 chars.
;The message addressed by HL is printed (SAVING or LOADING), and
;the filename is echoed.
;Retn:	B	0 if no ,R specified, 1 if ,R
;	C	0 if .BAS (,A), 1 if .XYB (no ,A)
;In AMD version, filetype is .HEX and C is 0FFH if ,H specified.
;In AMD version, C is 2 if ,S specified.
blkfn:	db	0, '           ', 0, 0, 0, 0, 80H	;blank filename
ambfn:	db	'???????????', 0, 0, 0, 0, 80H		;ambiguous filename
xybft:	db	'XYB', 80H
basft:	db	'BAS', 80H
	if	amd
hexft:	db	'HEX', 80H
	endif
gtfil:	call	gtfnm		;get unambiguous filename
	lxi	d,fcbad+1
	mvi	c,8
	call	prstr		;echo it
	call	wcrlf		;and write crlf
	call	gtcom		;look for comma
	lxi	b,1		;0 to B, 1 to C
	jc	gtfl2		;no comma, filetype is .XYB
	mvi	d,'A'
	call	gtd		;look for A
	if	amd
	jnc	gtfl0		;ASCII specified
	mvi	d,'H'
	call	gtd		;look for H
	mvi	c,0FFH		;0FFH to C for HEX
	jnc	gtf0a		;HEX specified
	mvi	d,'S'
	call	gtd		;look for S
	mvi	c,2		;2 to C for serial
	jnc	gtf0a		;serial specified
	mvi	c,1		;1 to C for XYB
	jmp	gtfl1		;not A or H, must be R
	else			;not AMD
	jc	gtfl1		;no A, must be R after comma
	endif
gtfl0:	mov	c,b		;0 to C to indicate .BAS filetype
gtf0a:	call	gtcom		;look for ,R
	jc	gtfl2		;none
gtfl1:	mvi	d,'R'
	call	gtdsn		;skip ,R
	mvi	b,1		;1 to B to indicate ,R
gtfl2:	lxi	d,xybft
	mov	a,c
	ora	a
	if	amd
	jz	gtfl4
	jp	gtfl5
	lxi	d,hexft
	jmp	gtfl5
	else
	jnz	gtfl5
	endif
gtfl4:	lxi	d,basft		;filetype .BAS -- LOAD entry point
gtfl5:	push	b		;save status
	lxi	h,fcbft
	call	cpyst		;copy filetype to FCB
	dcx	h
	mvi	m,0		;reset last char
	pop	b		;restore status
	ret			;and return

;GTFNM gets an unambiguous filename.
gtfnm:	call	gtafn		;get ambiguous filename
	lxi	h,fcbad+1	;first char location to HL
	mvi	c,11		;char count to C
	mvi	a,'?'		;? to A
gtfn0:	cmp	m		;check if filename char is ?
	jz	snerr		;yes, SN error
	inx	h
	dcr	c
	jnz	gtfn0		;try next char
	ret

;GTAFN gets an ambiguous filename and initializes the default FCB.
;Leading spaces are removed and lower case is converted to UPPER.
gtafn:	lxi	d,blkfn
	lxi	h,fcbad
	push	h
	call	cpyst		;copy blank filename to fcb
	xra	a
	sta	fcbcr		;reset current record field to 0
	if	strng
	call	evals		;look for filename
	call	fetch
	jnc	snerr		;nonstring filename
	else			;parse string ad hoc if nonstring version
	mvi	d,'"'
	call	gtdsn
	mov	d,h
	mov	e,l		;first char address to DE
	mvi	c,0		;char count to C
gta0a:	mov	a,m		;fetch possible filename char
	inx	h		;point to next
	cpi	'"'
	jz	gta0b		;done if at close quote
	cpi	cr
	jz	snerr		;SN error if cr before close quote
	inr	c		;bump count
	jmp	gta0a		;and try next
gta0b:	shld	textp		;update TEXTP
	endif
	pop	h		;restore fcb address to HL
	mov	a,c
	ora	a
	jz	gtaf5		;null file name, take @:*.*
gtaf0:	ldax	d
	cpi	' '
	jnz	gtaf1		;no more leading spaces
	inx	d
	dcr	c		;remove a leading space
	jz	gtaf5		;no chars after spaces, take @:*.*
	jmp	gtaf0		;and try next
gtaf1:	dcr	c
	jz	gtaf2		;one-char filename, skip : check
	inx	d
	ldax	d		;fetch second character
	dcx	d
	cpi	':'
	jnz	gtaf2		;no disk specified
	ldax	d		;fetch disk name
	sui	'@'		;subtract ASCII bias
	jc	snerr
	ani	1FH		;convert lower to upper
	cpi	5
	jnc	snerr
	mov	m,a		;store disk number in fcb
	inx	d
	inx	d		;point to char after :
	dcr	c
	dcr	c		;and update char count
gtaf2:	inr	c		;correct char count in C
	jz	gtaf5		;no more chars, take *.*
	inx	h		;point to first filename char destination
	mvi	b,8		;max # filename chars to B
gtaf3:	call	fchar		;process filename char
	dcr	b
	jm	gtaf4		;skip it
	mov	m,a
	inx	h		;store it
gtaf4:	inx	d
	dcr	c
	rz			;done
	jmp	gtaf3		;else process next
gtaf5:	lxi	d,ambfn
	lxi	h,fcbad+1
	jmp	cpyst		;copy *.* to fcb

fchar:	ldax	d		;fetch the char
	ani	7FH		;remove parity
	cpi	'"'+1
	jc	snerr		;space, quote, exclam, controls not allowed
	cpi	'*'
	jz	fcha1		;expand * to ???...
	cpi	'.'
	jz	fcha2		;filetype follows
	cpi	60H
	rc			;printable char
	sui	20H		;convert lower to upper
	ret
fcha1:	dcr	b
	jm	fcha3
	mvi	m,'?'		;store a ?
	inx	h
	jmp	fcha1
fcha2:	mvi	b,3		;filetype char count
fcha3:	pop	h		;pop FCHAR return
	lxi	h,fcbft		;filetype address to HL
	jmp	gtaf4

	if	amd		;routines for HEX LOADing and SAVEing

;INTEL HEX format is a series of records, with all info in ASCII:
;frame 0	record mark ':' [3AH]
;frames 1-2	record length n, hex number 0-FFH [0 for eof;  here max=10H]
;frames 3-6	load address
;frames 7-8	record type [here 0]
;frames 9 - 8+2*n	data
;frames 9+2*n - 10+2*n	checksum  [negated sum mod 256 of preceding items]

;WASCI converts A3-A0 to ASCII and falls through to DWRIT to write
wasci:	ani	0FH		;00H, ..., 09H, 0AH, ..., 0FH
	adi	90H		;90H, ..., 99H, 9AH, ..., 9FH
	daa			;90H, ..., 99H, 00H+C,...,05H+C
	aci	40H		;D0H, ..., D9H, 41H, ..., 46H
	daa			;30H, ..., 39H, 41H, ..., 46H
	mov	c,a		;pass value to write through C
				;and fall through to DWRIT
dwrit:	push4
	call	dkout		;write the char
	jmp	pop4

;WBYTE writes byte from A as two ASCII bytes, updating checksum in D
wbyte:	push	psw
	rrc
	rrc
	rrc
	rrc
	call	wasci		;convert left nibble to ascii and write
	pop	psw
	push	psw
	call	wasci		;convert right nibble to ascii and write
	pop	psw
	add	d
	mov	d,a		;update checksum
	ret

;WADDR writes address from HL (subtracting loading bias), and record type.
waddr:	push	h
	push	d
	xchg			;address to DE
	lhld	temp		;-first to HL
	dad	d		;load address to HL
	pop	d
	mov	a,h
	call	wbyte		;write high byte
	mov	a,l
	call	wbyte		;write low byte
	xra	a
	call	wbyte		;write record type = 0
	pop	h
	ret			;and return

;WCSUM writes the checksum from D, followed by CR and LF.
wcsum:	xra	a
	sub	d
	call	wbyte		;write checksum
	mvi	c,cr
	call	dwrit
	mvi	c,lf
	jmp	dwrit		;write cr and lf and return

;RBYTE reads two ASCII bytes and builds binary char, updating checksum in D.
;Retn:	A	char read
;	C	clobbered
;	D	updated checksum
;	BEHL	preserved
;	Zero	set iff new checksum = 0
rbyte:	call	readc		;read a byte
	call	ishex		;convert ASCII to binary
	jc	loadx		;not an ASCII hex digit, abort
	rlc
	rlc
	rlc
	rlc
	mov	c,a		;high nibble to C
	call	readc		;read another
	call	ishex
	jc	loadx		;not ASCII hex digit
	ora	c		;form complete byte from nibbles
	mov	c,a		;and save
	add	d		;update checksum
	mov	d,a		;and checksum to D
	mov	a,c		;restore result to A
	ret
	endif			;end of AMD conditional
	endif			;end of NOT RTPAK conditional
	endif			;end of CPM conditional


;end of CPM
	page
;DRIVER 10/22/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;interpreter driver


;The interpreter driver has several entry points.
;DMOD2, after errors, resets SP and falls through to...
;DMODE, after execution, prints OK prompt, then...
;DMOD3 is the driver itself.  Gets a line of user text, tokenizes it and
;either executes it (no line #) or adds it to source text, then gets another.

	if	compl		;return to DMODE boots in COMPL version
dmod2	equ	boot
dmodc	equ	boot
dmode	equ	boot
	else
dmod2:	lxi	sp,stack	;reset SP
dmode:	call	prtm0		;print OK message
	if	epstn
	db	cr, lf, 13H, 'OK', 12H, cr, lf or 80H
	else
	db	cr, lf, 'OK', cr, lf or 80H
	endif
	xra	a
	sta	gchar		;clear GET character
	sta	tracl		;clear tracing status
	if	strng
	sta	stemp		;clear # string temps in use
	endif
dmod3:	call	lnnu0		;reset LNNUM to 0 in case ^C typed
	call	gtlin		;get a line from user
	call	tkize		;tokenize it
	jc	xsta1		;execute it if no line #
	cnz	addln		;else add to source text unless <cr>
	jmp	dmod3		;and get another line
	endif			;end of COMPL conditional

;NEXTC scans to next command, resets SP and falls through to NEXTS.
nextc:	lhld	cstkp
	shld	estkp		;reset ESTACK in case error within expr
	call	gtdel		;scan to delimiter
	lxi	sp,stack	;reset SP and fall through to NEXTS

;NEXTS is branched to with TEXTP pointing to a delimiter (: ' <cr>), else it
;issues a SN error.  Performs break char and interrupt tests.
;Returns to DMOD1 if at eof.  Moves TEXTP to first char
;of next statement, updates LNNUM and performs break test.  Then falls through
;to XSTAT for statement execution.
nexts:	if	camac and nonst and (not rtpak)
	in	1
	ani	2
	cnz	ctst0		;read char if present
	else
	call	cstat		;test console for break char
	rrc
	cc	ctst0		;read char if present
	endif
	if	not wild
	lda	intad
	rlc			;test whether interrupt table empty
	cc	itest		;no, check if interrupt occurs
	endif
	lhld	textp
	shld	savtp		;save textp
	mov	a,m		;inline CALL GTCH1 to get next char
	inx	h
	cpi	' '
	jz	$-4
	shld	textp
	cpi	':'
	jz	xstat		;another statement on same line
	if	rtpak or not compl	;comments are removed if compiled
	cpi	''''
	cz	rem		;on-line comment
	endif
	cpi	cr
	jnz	snerr		;syntax error -- garbage after statement
	mov	a,m		;fetch length byte of next line
	ora	a
	jz	dmode		;end of source text
	inx	h
	shld	lnnum		;save line number address
	inx	h
	inx	h
	mov	a,m		;fetch break byte
	inx	h		;point to first text byte
	shld	textp		;set text pointer
	if	not compl
	rar			;NB carry was reset above!
	cc	btest		;test for break if bit 0 was set

;XSTAT is branched to with TEXTP pointing to first char of a statement.
;Performs trace mode test, then falls through to XSTA1 for
;actual statement execution.
;XSTA1 is entry point from direct mode, to avoid trace checks.
;XSTA2 is entry point for THENpart of an IF statement.
xstat:	lda	trace
	sta	tracl		;set tracing status of current statement
	ora	a
	cnz	tprnt		;print trace line # if traceon
	endif			;skip trace tests in COMPL version
xsta1:	lxi	b,nexts
	if	compl
xstat	equ	xsta1
	endif
	push	b		;stack normal return address to allow RET
xsta2:	lhld	textp		;inline CALL GTCHA follows to get next token
	mov	a,m
	inx	h
	cpi	' '
	jz	$-4
	shld	textp
	ora	a
	jp	let0		;not token, must be LET or null statement
	if	wild
	cpi	wbuft
	jz	wletb		;BUFFER legal lhs in WILD version
	endif
	sui	cmdtk		;subtract min token value
	jc	snerr		;too small, not a command token
	cpi	ncmds		;compare to number of commands
	jnc	snerr		;too big
	lxi	h,cmdta		;command branch table address to HL
	add	a		; * 2 bytes per entry
	mov	c,a
	mvi	b,0
	dad	b		;add offset to base address
	mov	a,m		;low order address to A, temporarily
	inx	h
	mov	h,m		;high order addr to H
	mov	l,a		;low order addr to L
	pchl			;branch to address

;BTEST checks for line breakpoints.
;Called before XSTAT, so TRACL is not yet reinitialized.
;Call:	A	line break byte, RARed once.
	if	not compl
btest:	ora	a		;test further if more bits set
	lxi	h,xsta1		;replace XSTAT return addr with XSTA1 to
	xthl			;	avoid resetting TRACL on return
	jz	tprnt		;print break line and return if only bit 0 set
	rrc			;bit 1 of break byte to carry
	jnc	btst2		;no count or variables, check for DMODE break
	push	psw		;save break byte, rotated twice
;Now the symbol table entry containing line break info must be found.
	lhld	lnnum
	call	bknam		;break entry 'name' to BUFAD
	mvi	a,brkst
	call	stlk0		;look up break entry
	jc	exerr		;not found -- EX error
;Next the count (# times before next break) is fetched.
	mov	e,m
	inx	h
	mov	d,m		;count to DE
	dcx	d		;decrement count
	mov	a,d
	ora	e
	jnz	btst3		;nonzero -- return
;Counted to zero, so reset count to original value and print break info.
	mov	b,h
	mov	c,l
	dcx	b		;BC points to count
	inx	h		;HL points to reset
	mov	a,m
	stax	b
	inx	b
	inx	h
	mov	a,m
	stax	b		;copy reset to count
	call	tprnt		;print break [<line #>]
	pop	psw		;restore break byte
	rrc			;bit 2 of break byte to Carry
	jnc	btst1		;skip var printing
;The break specified a <var list>, so variable values must be printed.
	push	psw		;save break byte, RRCed 3 times
	inx	h		;point to var list addr
	mov	a,m
	inx	h
	mov	h,m
	mov	l,a		;varlist address to HL
	shld	txtp2
	call	flip		;scan var list
	lxi	h,trace
	mov	a,m
	push	psw		;save TRACE
	mvi	m,255		;and set TRACE to true
btst0:	call	gtlhs		;get var ref
	mov	b,a		;save type
	call	trva1		;print = value
	call	gtcom		;look for another
	jnc	btst0		;print more vars
	pop	psw
	sta	trace		;restore trace
	call	flip		;restore textp
	pop	psw
;BTEST returns or breaks to DMODE, depending on whether BREAK specified $.
btst1:	rlc
btst2:	ani	2		;test bit 3 (after two rotates)
	rz			;no $ -- continue
	mvi	a,cslbk		;line break entry token to A
	jmp	stop1		;$
btst3:	mov	m,d
	dcx	h
	mov	m,e		;new value to count
	pop	psw		;restore break byte
	pop	h		;POP the RETurn to XSTA1
	jmp	xstat		;and return to XSTAT for TRACL initialization

;BPRNT prints <tab>[<line #>] if not TRACL, and is called by NEXT and TRSET.
;TPRNT prints <tab>[<line #>] and sets TRACL to TRUE.
;Both preserve BC,DE,HL and clobber A.
bprnt:	lda	tracl
	ora	a
	rnz			;suppress break printout if traced
tprnt:	push3
	call	wcrlf		;write crlf
	mvi	a,'['
	call	writc		;write [
	lhld	lnnum		;line number address to HL
	call	prntl		;print the line
	mvi	a,']'		;write ]
	call	writc
	if	wild
	call	wrtsp		;write a space (no tab routine)
	else
	call	prco1		;tab to next tab stop
	endif
	mvi	a,255
	sta	tracl		;set TRACL to indicate line traced already
	jmp	pop3

	endif			;end of NOT COMPL conditional

;ITEST determines whether interrupt occurs and then returns or interrupts.
;If no interrupt:
;Retn:	A,BC,DE	clobbered
;	HL	address of interrupt table eof
;If interrupt occurs, ITEST's return address is POPed, a normal statement
;return address is PUSHed, and a GOSUB to the appropriate line # is executed.
	if	not wild
itest:	lxi	h,intad		;first byte address to HL
itst1:	rlc			;test enable/suspend bit
	jnc	itst3		;suspended -- look for more
	mov	c,a		;save $ bit in sign bit of C
	mov	d,h
	mov	e,l		;save first byte address in DE
	inx	h		;point to port # byte
	mov	a,m
	call	rdp1		;read the port
	inx	h		;point to mask byte
	ora	m
	inx	h		;point to value byte
	xra	m		;compare & clear carry
	jz	itst2
	stc			;carry set iff compare not zero
itst2:	rar			;carry to sign bit of A
	xra	c		;sign bit set iff no interrupt
	jp	itst4		;interrupt occurs
	xchg			;restore first byte address to HL
itst3:	lxi	d,8
	dad	d		;address next entry
	mov	a,m		;first byte of table entry to A
	rlc			;test on/off bit
	rnc			;no more table entries -- return
	jmp	itst1		;and check it
itst4:	call	linbc		;zero set iff direct mode
	rz			;no interrupts from direct mode
	xchg			;restore first byte address to HL
	xthl			;pop ITEST return, push first byte address
	mvi	a,csint
	call	cpush		;token, textp, lnnum to control stack
	pop	d
	call	mvmde
	xchg			;  and to HL
	mov	a,m		;fetch first byte
	ani	0BFH		;suspend interrupt entry
	mov	m,a
	lxi	d,7
	dad	d		;address byte 8 of entry
	call	mvdem
	push	h		;save entry ptr
	call	findl		;look for ENABLE line # address
	jc	enerr		;none, EN error
	inx	h		;point to line # byte
	xthl			;push ENABLE line # address, recover ptr
	call	mvdem		;desired interrupt routine line # to DE
	lxi	h,nexts
	xthl			;pop ENABLE line # address, push return addr
	shld	lnnum		;set lnnum in case line not found
	jmp	goto2		;find line #, continue from there
	endif			;end of NOT WILD conditional


;end of DRIVER
	page
;EVAL 06/10/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;expression evaluator

;EVAL evaluates an expression.
;Space above the control stack is used as a working stack, called the
;E(xpression)STACK.  Each expression component already scanned is stored on
;the ESTACK as a byte identifier specifying its type (e.g. delimiter, paren,
;unary op) and precedence.  Fns and ops have an addtional byte containing
;the fn id.  Values are stored as a type byte followed by value bytes.  String
;values saved in the ESTACK are addressed by string temporaries, for access
;during garbage collection.
;Many types of errors may occur during EVAL.
;SN error occurs if an expression is illformed.
;OM error occurs if ESTACK overflows the available space.
;TM error occurs if arguments are of wrong type.
;FC, OV, BY and similar errors can occur during fn/op execution.
;EX errors should never occur, and indicate something is wrong.
;Retn:	Carry	Set iff no <expr> found, i.e. first char parsed is bad
;	A	type token of result, preserved if Carry
;	BC, DE	preserved
;	HL	pointer to type (followed by value bytes), preserved if Carry

eval:	push4			;save all in case failure
	mvi	c,isdel+1	;delimiter type/prec to C

;EVAL0 saves the token in C on the ESTACK.  If it is a fn or op,
;the id in B is also ESTACKed.
eval0:	mov	a,c		;fetch type/prec
	cpi	isop1
	jc	eva0a		;not a fn/op, just ESTACK type/prec
	mov	c,b		;else id to C
	mov	b,a		;save type/prec in B
	call	espsh		;ESTACK the id
	mov	c,b		;restore type/prec to C
eva0a:	call	espsh		;ESTACK the type/prec

;EVAL1 is the 'expecting value' state
;The next item parsed must be a literal, variable reference or 0-ary fn
;(evaluated and ESTACKed, then expecting op), or else a fn name, ( or
;unary op (ESTACKed, then expecting value).  Any other acts as delimiter.
eval1:	lhld	textp
eva1a:	mov	a,m		;fetch next text char
	inx	h
	cpi	' '
	jz	eva1a		;try next if space
	cpi	40H
	jc	evlip		;0-3FH, must be literal or (
	ora	a
	jp	evvar		;40-7FH, must be var ref
	call	gttyp		;type to A, id info to BC
	jc	evalx		;not a fn or op
	shld	textp		;read the fn or op
	cpi	isop1
	jc	evfn0		; 0-ary or user-defined function
	jz	eval0		;ESTACK unary op
	cpi	isop2
	jz	evopu		;binary op must be unary + or -
eva1b:	mvi	d,'('		;otherwise must be a function id
	call	gtdsn		;so skip ( after fn
	jmp	eval0		;ESTACK type/prec and id and expect value

;EVAL2 pushes a value (type token and value bytes) to ESTACK.
eval2:	call	esval		;value to ESTACK

;EVAL3 is the 'expecting op' state.
;The next item parsed should be a comma [fn(expr,expr)] or
;binary op [expr op expr] or ) [fn(expr) or fn(expr,expr) or (expr)].
;Any other is treated as a delimiter.
eval3:	lhld	estkp
	mov	a,m
	ani	tmask
	mov	d,a		;previous type to D
	mov	a,m
	ani	pmask
	inr	a
	mov	e,a		;previous precedence + 1 to E
	lxi	b,vbyts
	dad	b		;address new ESTACK top
	shld	estkp		;and update the pointer
	call	gtcho		;look at next item
	call	gttyp		;find its type
	cc	evarp		;not an op, must be ) or comma or delimiter
	cpi	isop2		;check if binary op
	jnz	eval4		;no, must be delimiter -- reduce
;Compare precedences of current and previous op and reduce when appropriate.
	mov	a,c		;fetch current type/prec
	ani	pmask
	cmp	e		;compare current to previous+1
	jc	eval4		;reduce
	call	gtcha		;else read current
	jmp	eval0		;and stack the info bytes

;At EVAL4 previous .>=. current precedence, so the stacktop is reduced
eval4:	lhld	estkp
	lxi	b,-vbyts+1
	dad	b		;address value on ESTACK
	mov	a,d		;fetch previous type
	cpi	isdel
	jz	eval5		;delimiter, done
	cpi	isop1
	jz	evop1		;unary op
	cpi	isop2
	jz	evop2		;binary op
	push	h		;otherwise current char must be )
	call	gtcha		;so read it
	cpi	')'
	jnz	snerr
	pop	h		;and restore value pointer
	mov	a,d		;restore previous type
	cpi	iscom
	jz	evfn2		;comma -- do a binary function
	cpi	islp
	jz	evpar		;( <expr> ) becomes <expr>
	cpi	isfn1
	jz	evfn1		;unary function
	cpi	isufn
	jz	evuf1		;unary user-defined function
	cpi	isfn2
	jz	snerr		;binary fn with one arg
	jmp	exerr		;else fatal EX error -- something wrong

;EVAL5 is the exit from a successful EVAL.
eval5:	pop	d		;discard saved PSW value
	pop	d		;and also saved HL value
	push	h		;save return value for HL
	mov	a,m		;fetch result type
	dcx	h		;NB carry is reset from CPI ISDEL
	dcx	h		;HL points to location before delimiter
eval6:	shld	estkp		;reset ESTACK past delimiter
pop3:	pop	h		;value to HL
	pop	d
pop1:	pop	b		;restore registers
	ret			;and done

;EVALX is the abnormal exit from EVAL.  If ESTACK contains only the
;initial delimiter, returns with Carry.  Otherwise issues SN error.
evalx:	lhld	estkp
	mov	a,m		;fetch top ESTACK item
	cpi	isdel+1		;compare to delimiter type/prec
	jnz	snerr		;ESTACK nonempty, SN error
	dcx	h		;else point past delimiter
	pop	psw		;restore A
	stc			;set Carry
	jmp	eval6		;restore and return


;ESVAL pushes a value to the ESTACK.
;Entry:	A	type token
;	BC	value if integer
;	C,DE	value if string
;	(TEMP),BCD	value if floating
esval:	lhld	estkp
	inx	h
	mov	m,a		;store type token
	inx	h		;point to first value byte
	if	float
	cpi	sngst
	jnz	esvl1		;not floating
	lda	temp		;recover value for A
	if	f9511
	jmp	sto95		;store floating value on ESTACK and return
	else
	jmp	fstor		;store floating value on ESTACK and return
	endif
	endif
esvl1:	if	strng
	cpi	strst
	jnz	esvl2		;not string
	call	atemp		;assign ESTACK location to string temp
	mov	m,c		;length to ESTACK
	inx	h
	mov	m,e
	inx	h
	mov	m,d		;location to ESTACK
	ret
	endif
esvl2:	cpi	intst
	jnz	exerr		;not integer
	mov	m,c
	inx	h
	mov	m,b		;integer value to ESTACK
	ret

;EVLIP looks for ( and stacks if present, else falls through to...
;EVLIT evaluates a literal, jumps to EVALX if none.
evlip:	cpi	'('
	jnz	evlit		;not (, must be literal
	shld	textp		;read the (
	mvi	c,islp+2	;type/prec to C
	jmp	eval0		;( to ESTACK and expect value next
evlit:	call	gtlt1		;get literal
	jc	evalx		;exit if not found
	jmp	eval2		;and ESTACK it

;EVVAR evaluates a variable reference.
evvar:	call	gtvar		;perform variable reference
	jc	evalx		;no var
	if	strng
	cpi	strst		;check if type string
	jnz	evva0		;nonstring
	xchg
	lhld	estkp
	inx	h
	inx	h		;point to value destination
	call	atemp		;assign string temporary
	xchg			;restore value location
	mvi	a,strst		;and restore type token
	endif
evva0:	mov	b,h		;EVFN0 entry point
	mov	c,l		;source to BC
evva1:	lhld	estkp		;EVPAR entry point
	inx	h
	mov	m,a		;ESTACK the type
	inx	h		;value destination to HL
	mvi	e,vbyts-1	;byte count to E
	call	movd0		;move value to ESTACK
	jmp	eval3		;and expect op

;EVOPU deals with unary + and -.
evopu:	mov	a,b		;refetch id
	cpi	plust-udfnt
	jz	eval1		;ignore unary plus
	lxi	b,isop1+12	;id=0 to B, type/prec to C for unary minus
	cpi	mint-udfnt
	jz	eval0		;stack unary minus
	call	bakup		;else unread the binary op
	jmp	evalx		;and exit

;EVPAR reduces (<expr>) to <expr>.
evpar:	mov	a,m		;type to A
	push	h
	dcx	h		;address (
	dcx	h		;address previous
	shld	estkp		;reset ESTACK
	pop	b
	inx	b		;value location to BC
	jmp	evva1		;and move down the value

;EVARP deals with ) or comma in expecting op state, loading C with type/prec.
;The previous item must be checked because of cases like DIM A (2,3).
evarp:	mov	a,d		;fetch previous type
	cpi	isdel
	jz	evar1		;previous was delim, treat current as delim
	mov	a,b		;refetch current
	mvi	c,isrp+2	;right paren type/prec to C
	cpi	')'
	rz
	mvi	c,iscom+3	;comma type/prec to C
	cpi	','
	jnz	evar1		;treat others as delimiters
	mvi	a,iscom		;comma type to A
	cmp	d		;compare to previous type
	mvi	a,isop2		;for compare on return
	rnz			;return unless currant and previous both comma
	dcr	e		;to allow successive commas for ternary fns
	ret
evar1:	mvi	c,isdel		;treat any other as delimiter
	ret

;EVFN0 evaluates a 0-ary function and evaluates (if 0-ary)
;or ESTACKs (if N-ary) a user-defined function.
evfn0:	cpi	isfn0
	mov	a,b		;fn id to A
	jz	excut		;execute 0-ary function
;The current item is a user-defined function call.
	call	gtnam		;get function name
	dcx	h
	push	h		;save TEXTP
	mvi	a,ufnst
	call	stlk0		;look up the fn name
	jc	snerr		;not found
	lda	varty
	mov	c,a		;save desired result type in C
	ldax	d		;fetch type
	ani	0E0H		;mask off type bytes
	jz	exuf0		;execute 0-ary user-defined function
	cpi	20H
	jnz	fcerf		;not 0-ary or N-ary, must be active already
;N-ary user-definable function information is ESTACKed.
	push	b		;save desired result type
	push	h
	mov	b,d
	mov	c,e		;type byte addr to BC
	call	epsh2		;and ESTACKed
	pop	h
	mov	c,m
	inx	h
	mov	b,m		;bound var addr to BC
	call	epsh2		;and ESTACKed
	pop	b		;recover desired result type
	call	espsh		;and ESTACK it
	pop	h		;pop TEXTP
	mvi	c,isufn+2	;type token to C
	jmp	eva1b		;skip ( and ESTACK type
exuf0:	mov	a,m
	inx	h
	mov	h,m
	mov	l,a		;function body address to HL
	shld	textp		;reset TEXTP to scan fn body
	lhld	symta
	push	h		;save SYMTA
	mov	a,c		;desired result type to A
	jmp	exufn		;and execute the fn

;EVUFN evaluates a user-defined function.
evuf1:	mvi	b,1		;number of args is in B
evufn:	xchg			;bound var val addr to DE
	lhld	textp
	push	h		;save TEXTP
	lhld	symta
	push	h		;and SYMTA
	lxi	h,-7
	dad	d
	shld	estkp		;reset ESTACK
	call	modem		;fetch type byte addr
	push	d		;and save it
	call	modem		;bound var addr to DE
	xchg
	shld	textp		;bound var addr to TEXTP
	xchg
	inx	h
	mov	a,m
	push	psw		;save desired result type
	inx	h
	inx	h		;point to bound var value
evuf2:	push	h		;save value location
	push	b		;save # args left
	push	h		;and save value location
	call	gtnam		;scan the bound var
	jc	exerr
	mov	c,a		;name length to C
	call	fdva1		;and build a new symbol table entry for it
	inx	h		;point to value destination
	xthl			;save destination, bound var value loc to HL
	lda	varty		;bound var type
	call	cnvrt		;convert to desired type
	if	strng
	cpi	strst
	cz	scopv		;free temp and copy to string space if string
	endif
	inx	h		;point to value
	xthl			;destination to HL
	pop	b		;value loc to BC
	call	bytsd		;byte count to DE
	call	moved		;copy value to destination
	pop	b		;arg count to B
	dcr	b		;and decremented
	jz	evuf3		;no more args
	mvi	d,','
	call	gtdsn		;skip comma
	pop	h		;value location to HL
	lxi	d,vbyts+1
	dad	d		;address next value
	jmp	evuf2		;and repeat for next bound var
evuf3:	pop	h		;pop saved value location
	call	gtreq		;skip ) = after last bound var
	pop	psw		;restore desired result type
	pop	d		;type byte location to DE
				;and execute the fn
;EXUFN executes a user-defined function.
;Entry:	A	desired result type
;	DE	symbol table FN entry type byte addr
;	Stacked	SYMTA and TEXTP values
exufn:	push	psw
	ldax	d		;refetch type byte
	ori	80H		;turn on active bit
	stax	d		;and store
	xchg
	lxi	d,stack+stakm-stakl
	call	cplde
	xchg
	dad	sp		;check for stack overflow
	jnc	fcerf		;fatal FC error, recursion too deep
	call	eval		;evaluate the fn body
	push	psw		;save Carry status
	ldax	d
	ani	7FH
	stax	d		;reset active bit
	pop	psw
	xchg			;save result location in DE
	pop	h
	mov	a,h		;desired result type to A
	pop	h
	shld	symta		;reset SYMTA
	pop	h
	shld	textp		;and reset TEXTP
	jc	fcerf		;unsuccessful eval
	xchg			;recover result location
	call	cnvrt		;convert result to desired type
	inx	h		;point to value
	jmp	evva0		;and ESTACK the result

;EVCOM deals with ternary MID$ and INSTR and N-ary user-defined fns.
evcom:	pop	b		;pop the saved arg address
	mvi	b,2		;# args seen thus far to B
evco1:	inx	h		;point to arg
	cpi	isufn+2
	jz	evufn		;N-ary user-defined fn
	if	strng
	cpi	isfn2+2
	jz	evfn3		;binary fn, must be ternary MID$ or INSTR
	endif
	cpi	iscom+3		;look for another comma
	jnz	snerr		;not found, something is wrong
	inr	b		;bump arg count
	dad	d		;address next token of ESTACK
	mov	a,m		;and fetch it
	jmp	evco1		;and look for more

;EVFN3 deals with the special cases ternary MID$ and ternary INSTR.
;Branches to routine IINST3 or MID3 with arg pointers to args 1,2,3
;in BC,DE,HL and return address to EVAL2 stacked.
	if	strng
evfn3:	mov	a,b
	sui	3
	jnz	snerr		;number of args must be 3
	push	h
	dcx	h
	dcx	h
	mov	a,m		;fetch fn id
	dcx	h
	shld	estkp		;reset ESTACK
	lxi	d,vbyts+1
	lxi	h,eval2
	xthl			;push return address, arg1 addr to HL
	push	h
	dad	d		;address arg2
	push	h
	dad	d		;address arg3
	pop	d
	pop	b
	cpi	instt-udfnt
	jz	inst3		;ternary INSTR
	cpi	midst-udfnt
	jz	mid3		;ternary MID$
	jmp	snerr		;else SN error
	endif

;EVOP1 evaluates a unary op.
;EVFN1 evaluates a unary function.
;EVFN2 evaluates a binary function.
evfn2:	push	h		;save arg2 address
	lxi	d,-vbyts-2
	dad	d
	mov	a,m		;fetch type/prec byte
	cpi	isfn2+2
	jnz	evcom		;must be ternary fn or N-ary user-defined fn
	inx	h		;point to arg1
	pop	d		;arg2 address to DE
evop1:
evfn1:	mov	b,h
	mov	c,l		;arg1 address to BC
	dcx	h		;to type
	dcx	h		;to id
	mov	a,m		;fn id to A
	jmp	evopa		;and execute it

;EVOP2 evaluates a binary operator.
evop2:	push	h		;save arg2 address
	dcx	h
	dcx	h
	mov	a,m		;op id to A
	lxi	d,-vbyts
	dad	d
	pop	d		;arg2 addr to DE
	mov	b,h
	mov	c,l		;arg1 addr to BC
evopa:	dcx	h
	shld	estkp		;reset ESTACK
				;and fall through to EXCUT to execute

;EXCUT executes a function or operator.
;Entry:	A	fn/op identifier
;	BC	pointer to arg1 (if any) in expr stack
;	DE	pointer to arg2 (if any) in expr stack
;Exit:	A	result token
;	BC	result if integer
;	C,DE	result if string
;	(FACC)	result if floating
;EXCUT fetches the desired arg types, fn/op address and result type from
;the fn/op type information table.  If the desired arg type is nonzero
;(zero means no arg or arg of ambiguous type), the desired and actual
;arg types are compared and conversion to the desired type performed if
;necessary.  If the desired arg type is integer, the
;pointer in BC or DE is replaced by the actual value of the arg.  If
;the desired arg1 type is floating, the value is loaded to FACC.  Otherwise
;EXCUT branches to the fn/op routine with the pointers in BC and DE intact.
;The result type is returned from the fn/op routine if type AMBST, otherwise
;the specified result type is taken as the actual result type.
excut:	mvi	h,0
	mov	l,a		;id * 1
	dad	h		;id * 2
	dad	h		;id * 4
	call	adahl		;id * 5
	push	d
	lxi	d,funta
	dad	d		;+ base address = first info addr
	pop	d
	mov	a,m		;fetch arg2 desired type
	ora	a
	jz	excu1		;skip conversion
	push	b		;save arg1 address
	mov	b,d
	mov	c,e		;arg2 addr to BC
	call	cnvtb		;convert arg2 to desire type
	mov	d,b
	mov	e,c		;and return to DE
	pop	b		;restore arg1
excu1:	inx	h
	mov	a,m		;fetch arg1 desired type
	ora	a
	cnz	cnvtb		;convert arg1
	inx	h
	mov	a,m
	inx	h
	push	h		;save info pointer
	mov	h,m
	mov	l,a		;fn/op address to HL
	push	h
	lxi	h,exret
	xthl			;return addr to stack, fn/op addr to HL
	pchl			;execute it
exret:	pop	h		;restore info pointer
	inx	h
	sta	temp		;save returned result type info
	mov	a,m
	if	f9511
	cpi	sngst		;check if 9511 floating point op
	jz	exre1		;yes
	endif
	ora	a
	jnz	eval2		;return specified result type
	lxi	h,temp
	mov	a,m		;else pass returned result type
	mov	m,e		;and save E in TEMP in case floating
	jmp	eval2		;and ESTACK the result
	if	f9511
exre1:	call	fet95		;fetch result from 9511 stack
	sta	temp		;save A value
	mov	a,m		;restore floating result type
	jmp	eval2		;and ESTACK the result
	endif

;CNVRT converts between differing variable types.
;Call:	A	desired type token
;	HL	pointer to value to convert
;Retn:	A,BC,DE,HL	preserved, HL now pointing to converted value.
;A nonfatal TM error occurs if conversion is impossible.
;An EX error should never occur.
;EVALT does an EVALS, then converts result to type LHSTY.
evalt:	call	evals
	lda	lhsty		;fetch desired type, fall through to...
cnvrt:	cmp	m		;compare actual to desired type
	rz			;actual = desired, done
	push	b
	push	d
	push	h		;save registers
	mov	b,a		;save desired type in B
	if	strng
	cpi	strst
	jz	tmerr		;desired=string, actual=numeric
	mov	a,m		;fetch actual
	cpi	strst
	jz	tmerr		;desired=numeric, actual=string
	mov	a,b		;restore desired
	endif
	if	float
	cpi	intst
	jz	cnvfi		;convert floating to integer
	cpi	sngst
	jnz	exerr
;Float an integer value
	mov	a,m		;fetch actual
	cpi	intst
	jnz	exerr		;must be integer
	mov	m,b		;store new type = floating
	inx	h
	if	f9511		;load integer value to 9511 stack
	push	h		;save location
	mov	a,m		;fetch low order byte
	out	d9511		;and stack
	inx	h
	mov	a,m		;fetch high order byte
	out	d9511		;and stack
	mvi	a,flt95
	call	o9511		;float the integer value
	call	fet95		;fetch value from 9511 stack
	pop	h		;restore value destination
	call	sto95		;and store on ESTACK
	jmp	cnvrx		;restore registers and return as below
	else
	push	h
	mov	c,m		;lsbyte of value to C
	inx	h
	mov	b,m		;msbyte to B
	if	fpbcd
	call	fflot		;float the value
	else
	lxi	d,0		;0 to DE
	mvi	a,16		;scale factor to A
	call	flot1		;float the value
	endif
	pop	h
	call	fstor		;and store the floated value
	jmp	cnvrx		;restore registers and return as below
	endif
;CNVFI fixes a floating value.
cnvfi:	mov	a,m		;fetch actual
	cpi	sngst
	jnz	exerr		;actual must be floating
	mov	m,b		;store new actual type = integer
	inx	h
	push	h		;save location
	if	f9511
	call	lod95		;value to 9511 stack
	call	int95		;fix floating value
	jnc	cnvf1		;successful conversion
	error	n, o, v		;else nonfatal OV error
	else
	call	fload		;load value to FACC
	call	iint		;convert to integer
	cc	iover		;cannot fix, nonfatal OV error
	endif
cnvf1:	pop	h		;restore value loc
	mov	m,c		;store lsbyte
	inx	h
	mov	m,b		;store msbyte
cnvrx:	pop	h		;restore location
	mov	a,m		;type to A
	pop	d		;restore DE
	pop	b		;and BC
	ret
	endif
tmerr:	error	c, T, M		;issue TM error and scan on

;CNVRB is called from EXCUT to perform type conversion on arguments.
;Call:	A	type token
;	BC	arg pointer
;Retn:	A,DE,HL	preserved
;	BC	arg pointer to converted type, or arg value if integer
;	FACC	arg value if floating
cnvbi:	mvi	a,intst		;convert to integer
cnvtb:	push	b
	xthl			;HL saved, pointer to HL
	call	cnvrt		;do the conversion
	cpi	intst
	jz	cnvb1		;integer, fetch it
	if	float
	cpi	sngst
	cz	fetcf		;floating, load the FACC
	endif
	xthl			;pointer saved, HL restored
	pop	b		;pointer to BC
	ret
cnvb1:	call	fetci		;fetch integer value
	pop	h		;restore HL
	ret

;GTTYP gets type and precedence info for a fn/op.
;Call:	A	token
;Retn:	A	token type (precedence masked off)
;	B	fn id (offset into function table), ISDEL if Carry
;	C	type/precedence byte
;	DE, HL	preserved
;	Carry	Set iff (A) not fn/op token
gttyp:	mov	b,a		;char to B
	sui	udfnt		;subtract first fn/op token
	rc
	cpi	nfuns		;compare to # of fns
	cmc
	rc
	mov	b,a		;id to B
	push	h
	lxi	h,typta
	call	adahl		;addr type/prec byte
	mov	c,m		;type/prec byte to C
	pop	h
	mov	a,c
	ani	tmask		;type to A
	ret

;ESPSH pushes C to ESTACK and checks for OM error.
;Since a value will be ESTACKed next, ESPSH assures that
;at least VBYTS of space remain above the pushed token.
epsh2:	call	espsh		;push one byte
	mov	c,b		;and fall through to push another
espsh:	lhld	symta
	lxi	d,-vbyts-1
	dad	d
	xchg			;SYMTA - VBYTS - 1 to DE
	lhld	estkp
	inx	h		;ESTKP + 1 to HL
	call	cmdhu
	jc	eserr		;SYMTA <= ESTKP + VBYTS + 1
	mov	m,c		;store value
	shld	estkp		;and update pointer
	ret
eserr:	lhld	eofad
	call	cspst		;reset control stack
	error	c, O, M		;issue OM error and scan to next

;EVALS does an EVAL, issues SN error if no <expr> found.
evals:	call	eval
	rnc
	jmp	snerr

;IEVAL does an EVAL, converts result to integer and returns it in BC.
ieval:	push	h
	call	eval
	jc	ieva1		;no expr
	mvi	a,intst
	call	cnvrt		;convert to integer
	call	fetci		;result integer to BC
ieva1:	pop	h		;restore HL
	ret

;GTEXP does an IEVAL, issues SN error if no <expr> found.
gtexp:	call	ieval
	rnc
	jmp	snerr

;GTBEX gets a byte-valued expression.
;GTCBE gets a comma followed by a byte-valued expression, branches to SN error
;if comma is not present.
gtcbe:	call	gtcom
	jc	snerr
gtbex:	call	gtexp
	jmp	isbyt


;end of EVAL
	page
;FINOUT 10/10/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;floating point input/output routines

	if	float and (not f9511) and (not fpbcd)

;CONSTANTS
FPONE:	DW   81H, 0		;FLOATING POINT 1.
FPTEN:	DB   84H, 20H, 0, 0	;FLOATING POINT 10.
FPRND:	DB   6CH, 6H, 37H, 0BCH	;FLOATING POINT ROUNDER .000 000 5


;FLOATING POINT INPUT
FINP:	MOV  E,M	;1ST CHARACTER OF STRING
	CALL SVAD	;SET CHARACTER ADDR,PNT FLG,EXP
	INX  H		;TO ADDR THE VALUE SIGN
	MVI  M,80H	;SET VALUE SIGN POSITIVE
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  M,D	;SET ACC TO ZERO
	LDA  TEMP	;FETCH DESIRED VALUE SIGN
	ORA  A
	MOV  A,E	;1ST CHARACTER
	JZ  INP2	;IF POSITIVE DESIRED
	LXI  H,TMP3	;TO ADDR THE VALUE SIGN
	MOV  M,D	;SET VALUE SIGN NEGATIVE
;ANALYZE THE NEXT CHARACTER IN THE STRING
INP2:	MVI  B,0	;DIGIT 2ND WORD OR DECIMAL EXPONENT
	CPI  '.'	;COMPARE TO DECIMAL POINT
	JZ   INP3	;IF DECIMAL POINT
	CPI  'E'	;COMPARE TO EXPONENTIATION SIGN
	JZ   INP4	;IF EXPONENTIATION
	cpi  'E'+20H
	jz   inp4	;allow lower case e in case untokenized
	SUI  '0'	;SUBTRACT ASCII BIAS
	CPI  0AH	;SET CARRY IF CHARACTER IS VALID DIGIT
	JNC  INP8	;IF CHARACTER IS NOT A VALID DIGIT
	STA  TMP4	;SAVE THE CURRENT DIGIT
	LXI  H,FPTEN	;TO ADDR FLOATING POINT 10
	CALL FMUL	;MULTIPLY BY TEN
	LXI  H,VALE	;TO ADDR THE VALUE
	CALL FSTOR	;STORE OLD VALUE TIMES 10
	INX  H		;TO ADDR THE CURRENT DIGIT
	MOV  A,M	;CURRENT DIGIT
	MVI  B,0	;CLEAR 2ND WORD OF DIGIT
	MOV  C,B	;CLEAR 3RD WORD OF DIGIT
	MOV  D,B	;CLEAR 4TH WORD OF DIGIT
	MVI  E,08	;INDICATE DIGIT IS IN REGISTER A
	CALL FFLOT	;CONVERT DIGIT TO FLOATING POINT
	LXI  H,VALE	;TO ADDR THE VALUE
	CALL FADD	;ADD OLD VALUE TIMES 10
	LXI  H,TMP2	;TO ADDR THE DECIMAL POINT FLAG
	MOV  A,M	;DECIMAL POINT FLAG
	ANA  A		;SET CONTROL BITS
	JZ   INP1	;IF NO DECIMAL POINT ENCOUNTERED
	DCX  H		;TO ADDR THE INPUT EXPONENT
	DCR  M		;DECREMENT THE INPUT EXPONENT
INP1:	CALL CHAD	;GET ADDR OF NEXT CHARACTER
	MOV  A,M	;NEXT CHAR
	JMP  INP2	;TO PROCESS NEXT CHARACTER
INP3:	LXI  H,TMP2	;TO ADDR THE DECIMAL POINT FLAG
	XRA  M		;ZERO IF FLAG SET
	MOV  M,A	;SET DECIMAL POINT FLAG
	JNZ  INP1	;IF FLAG NOT ALREADY SET
	LHLD ADRL	;READDRESS THE INPUT STRING
	JMP  INP8	;IF 2ND DECIMAL POINT

;PROCESS DECIMAL EXPONENT
INP4:	CALL CHAD	;GET ADDR OF NEXT CHARACTER
	MOV  A,M	;NEXT CHARACTER OF STRING
	mvi  e,0	;possible exponent sign (-) to E
	CPI  MINT	;COMPARE TO MINUS CHARACTER
	JZ   INP5	;IF MINUS SIGN
	cpi  '-'
	jz   inp5	;allow untokenized minus
	mov  e,a	;else sign positive (nonzero) to E
	CPI  PLUST	;COMPARE TO PLUS CHARACTER
	jz   inp5	;plus
	cpi  '+'	;check for untoenized plus
	JNZ  INP6	;IF NOT PLUS SIGN
INP5:	INX  H		;TO ADDR THE NEXT CHARACTER
	MOV  A,M	;NEXT CHARACTER OF STRING
INP6:	SUI  '0'	;SUBTRACT ASCII BIAS
	JZ   INP5	;IGNORE LEADING ZERO ON EXPONENT
	MVI  B,0	;POSSIBLE DECIMAL EXPONENT
	CPI  0AH	;SET CARRY IF A DECIMAL DIGIT
	JNC  INP8	;IF NOT A DECIMAL DIGIT
	MOV  B,A	;DECIMAL EXPONENT EQUAL DIGIT
	INX  H		;TO ADDR THE NEXT CHARACTER
	MOV  A,M	;NEXT CHARACTER OF STRING
	SUI  '0'	;SUBTRACT ASCII BIAS
	CPI  0AH	;SET CARRY IF A DECIMAL DIGIT
	JNC  INP7	;IF NOT A DECIMAL DIGIT
	INX  H		;ADDRESS NEXT CHAR
	MOV  C,A	;LSD OF DECIMAL EXPONENT
	MOV  A,M	;FETCH NEXT
	SUI  '0'	;SUBTRACT ASCII BIAS
	CPI  0AH
	JC   INPOV	;THREE DIGITS, SCAN REMAINING AND RETURN 0 OR OV
	MOV  A,B	;MSD OF DECIMAL EXPONENT
;FORM COMPLETE DECIMAL EXPONENT
	ADD  A		;2*MSD
	ADD  A		;4*MSD
	ADD  B		;5*MSD
	ADD  A		;10*MSD
	ADD  C		;10*MSD + LSD
	MOV  B,A	;DECIMAL EXPONENT
INP7:	MOV  A,E	;SIGN OF DECIMAL EXPONENT
	ANA  A		;SET CONTROL BITS
	JNZ  INP8	;IF SIGN POSITIVE
	SUB  B		;COMPLEMENT DECIMAL EXPONENT
	MOV  B,A	;DECIMAL EXPONENT
INP8:	SHLD TEXTP
	LXI  H,TMP3	;TO ADDR THE INPUT SIGN
	MOV  C,M	;INPUT SIGN
	LXI  H,ACCS	;TO ADDR THE ACC SIGN
	MOV  M,C	;ACC SIGN
	MOV  A,B	;DECIMAL EXPONENT

;CONVERT DECIMAL EXPONENT TO BINARY
INP9:	LXI  H,TMP1	;TO ADDR THE DECIMAL EXPONENT
	ADD  M		;ADJUST DECIMAL EXPONENT
	JZ   FTEST	;IF DECIMAL EXPONENT IS ZERO
	MOV  M,A	;CURRENT DECIMAL EXPONENT
	LXI  H,FPTEN	;TO ADDR FLOATING POINT 10
	JP   INP10	;IF MULTIPLY REQUIRED
	CALL FDIV	;DIVIDE BY 10
	MVI  A,1	;TO INCREMENT THE DECIMAL EXPONENT
	JMP  INP9	;TO TEST FOR COMPLETION
INP10:	CALL FMUL	;MULTIPLY BY 10
	MVI  A,-1	;TO DECREMENT THE DECIMAL EXPONENT
	JNC  INP9	;TO TEST FOR COMPLETION
	JMP  FOVER	;OV ERROR ON OVERFLOW AND RETURN FPMAX
INPOV:	INX  H
	MOV  A,M
	SUI  '0'
	CPI  0AH
	JC   INPOV	;SCAN REMAINING EXPONENT DIGITS
	SHLD TEXTP	;RESET TEXTP TO FIRST NONDIGIT
	MOV  A,E	;SIGN OF DECIMAL EXPONENT
	ORA  A
	JZ  FZRO	;LARGE NEGATIVE EXPONENT, RETURN 0.
	JMP  FOVER	;POSITIVE, ISSUE FLOATING OVERFLOW ERROR


;FLOATING POINT OUTPUT
;The contents of the FACC is not preserved
FOUT:	LXI  H,BUFAD-1	;USE BUFAD TO STORE CONVERTED VALUE
	CALL SVAD	;SET CHARACTER ADDRESS, DIGIT COUNT, DECIMAL EXPONENT
	CALL FTEST	;LOAD ACC TO REGISTERS
	LXI  H,VALE	;TO ADDR THE ACC SAVE AREA
	CALL FSTOR	;REGISTER STORE

;OUTPUT SIGN CHARACTER
	CALL CHAD	;GET ADDR OF CHARACTER
	MVI  M,' '	;STORE SPACE CHARACTER
	ANA  A		;SET CONTROL BITS
	JZ   OUT3	;IF ACC IS ZERO
	MOV  E,A	;ACC EXPONENT
	MOV  A,B	;ACC SIGN AND 1ST FRACTION
	ANA  A		;SET CONTROL BITS
	MOV  A,E	;ACC EXPONENT
	JP   OUT0	;IF ACC IS POSITIVE
	MVI  M,'-'	;CHANGE SIGN TO MINUS
;CHECK IF ACCUMULATOR IN RANGE .01 TO 1 TO ALLOW .01
OUT0:	CPI  7AH	;COMPARE TO .01 EXPONENT
	JC   OUT1	;TOO SMALL, SCALE NORMALLY
	CPI  81H
	JC   OUT5	;SKIP SCALING IF BETWEEN .01 AND 1
;SCALE ACCUMULATOR TO 0.1 TO 1.0 RANGE
OUT1:	CPI  7EH	;COMPARE TO SMALL EXPONENT
OUT2:	LXI  H,FPTEN	;TO ADDR FLOATING POINT 10
	JC   OUT4	;IF EXPONENT TOO SMALL
	CPI  81H	;COMPARE TO LARGE EXPONENT
	JC   OUT5	;IF EXPONENT NOT TOO LARGE
	CALL FDIV	;DIVIDE BY 10
OUT3:	LXI  H,TMP2	;TO ADDR THE DECIMAL EXPONENT
	INR  M		;INCREMENT DECIMAL EXPONENT
	JMP  OUT2	;TO TEST FOR SCALING COMPLETE
OUT4:	CALL FMUL	;MULTIPLY BY 10
	LXI  H,TMP2	;TO ADDR THE DECIMAL EXPONENT
	DCR  M		;DECREMENT DECIMAL EXPONENT
	JMP  OUT1	;TO TEST FOR SCALING COMPLETE

;ROUND THE VALUE BY ADDING 0.000 000 5
OUT5:	CALL FABS	;MAKE ACC POSITIVE
	LXI  H,FPRND	;TO ADDR THE ROUNDER
	CALL FADD	;ADD THE ROUNDER
	CPI  81H	;CHECK FOR OVERFLOW
	JNC  OUT2	;IF EXPONENT TOO LARGE

;SET DIGIT COUNTS
	LXI  H,TMP2	;TO ADDR THE DECIMAL EXPONENT
	MOV  A,M	;DECIMAL EXPONENT
	MOV  E,A	;DIGITS BEFORE DECIMAL EXPONENT
	CPI  7		;COMPARE TO LARGE EXPONENT
	JC   OUT6	;IF EXPONENT IN RANGE
	MVI  E,1	;DIGITS BEFORE DECIMAL EXPONENT
OUT6:	SUB  E		;ADJUST DECIMAL EXPONENT
	MOV  M,A	;DECIMAL EXPONENT
	MVI  A,6	;TOTAL NUMBER OF DIGITS
	SUB  E		;DIGITS AFTER DECIMAL POINT
	INX  H		;TO ADDR THE 2ND DIGIT COUNT
	MOV  M,A	;DIGITS AFTER DECIMAL POINT
	DCR  E		;DECREMENT THE DIGIT COUNT
	MOV  A,E	;DIGITS BEFORE THE DECIMAL POINT

;OUTPUT SIGNIFICANT DIGITS
OUT7:	LXI  H,TMP1	;TO ADDR THE DIGIT COUNT
	ADD  M		;ADJUST THE DIGIT COUNT
	MOV  M,A	;NEW DIGIT COUNT
	JM   OUT8	;IF COUNT RUN OUT
	LXI  H,FPTEN	;TO ADDR FLOATING POINT 10
	CALL FMUL	;MULTIPLY BY 10
	MVI  E,8	;TO PLACE DIGIT IN REGISTER 'A'
	CALL FFIX	;CONVERT TO FIXED FORMAT
	CALL CHAD	;GET CHARACTER ADDRESS
	ADI  '0'	;ADD ASCII BIAS
	MOV  M,A	;OUTPUT DECIMAL DIGIT
	XRA  A		;CLEAR CURRENT DIGIT
	MVI  E,8	;BINARY SCALING FACTOR
	CALL FFLOT	;RESTORE VALUE MINUS DIGIT
	MVI  A,-1	;TO ADJUST THE DIGIT COUNT
	JMP  OUT7	;LOOP FOR NEXT DIGIT
OUT8:	LXI  H,TMP3	;TO ADDR 2ND DIGIT COUNT
	MOV  A,M	;DIGITS AFTER DECIMAL POINT
	MVI  M,-1	;SET 2ND COUNT NEGATIVE
	ANA  A		;SET CONTROL BITS
	JM   OUT9	;IF 2ND COUNT RAN OUT
	CALL CHAD	;CHARACTER ADDRESS ROUTINE
	MVI  M,'.'	;STORE DECIMAL POINT
	JMP  OUT7	;LOOP FOR NEXT DIGIT
OUT9:	LHLD ADRL	;ADDR THE LAST DIGIT STORED
OUT9A:	MOV  A,M	;FETCH IT
	DCX  H
	CPI  '0'
	JZ   OUT9A	;SUPPRESS TRAILING ZEROS
	CPI  '.'
	JZ  OUT9B	;SUPPRESS TRAILING DECIMAL POINT
	INX  H		;READDRESS LAST DIGIT
OUT9B:	SHLD ADRL	;STORE FUDGED POINTER
	LXI  H,TMP2	;ADDR THE DECIMAL EXPONENT
	MOV  A,M
	ORA  A
	JZ   OUT13	;IF DECIMAL EXPONENT IS ZERO

;OUTPUT DECIMAL EXPONENT
	MVI  B,'+'	;PLUS SIGN
	JP   OUT10	;IF EXPONENT IS POSITIVE
	MVI  B,'-'	;MINUS SIGN
	MOV  C,A	;NEGATIVE EXPONENT
	XRA  A		;ZERO
	SUB  C		;COMPLEMENT THE EXPONENT
OUT10:	MVI  C,'0'-1	;EMBRYO ASCII TENS DIGIT
OUT11:	INR  C		;INCREMENT TENS DIGIT
	SUI  0AH	;REDUCE REMAINDER
	JNC  OUT11	;IF MORE TENS
	ADI  3AH	;RESTORE AND ADD ASCII BIAS
	MOV  D,A	;UNITS DIGIT
	MVI  A,'E'	;EXPONENT SIGN
	CALL CHAD	;GET CHARACTER ADDRESS
	CALL FSTOR	;STORE LAST 4 CHARACTERS
OUT12:	INX  H		;LAST ADDR + 1
	MOV  A,L	;LAST + 1 TO A
	PUSH PSW	;SAVE LAST+1
	LXI  H,VALE	;TO ADDR THE ACC SAVE AREA
	CALL FLOAD	;RESTORE ACC
	POP  PSW	;RESTORE LAST+1
	LXI  D,BUFAD	;FIRST TO DE
	SUB  E		;LAST+1 - FIRST = LENGTH TO A
	MOV  C,A	;AND TO C
	RET

;EXPONENT IS ZERO, DONE
OUT13:	LHLD ADRL	;ADDRESS OF LAST CHAR TO HL
	JMP  OUT12	;AND CONTINUE AS ABOVE

;SAVE THE CHARACTER STRING ADDRESS
SVAD:	MOV  A,L	;CHARACTER STRING WORD
	MOV  B,H	;CHARACTER STRING BANK
	MVI  C,0	;INPUT EXPONENT OR DIGIT COUNT
	MOV  D,C	;DECIMAL POINT FLAG OR DECIMAL EXPONENT
	LXI  H,ADRL	;TO ADDR THE CHARACTER STRING ADDRESS
	JMP  FSTOR	;STORE A, B, C, AND D AND RETURN THRU STR

;OBTAIN THE ADDRESS OF THE NEXT CHARACTER
CHAD:	LHLD ADRL	;TO ADDR THE CHARACTER STRING ADDRESS
	INX  H		;BUMP IT
	SHLD ADRL	;RESTORE IT
	RET		;RETURN TO CALLER

	endif		;end of FLOAT conditional

;end of FINOUT
	PAGE
;FMATH 06/09/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;floating point math package


	if	float and (not f9511) and (not fpbcd)

;The floating point (FP) math package operates on normalized floating
;binary point numbers stored in four consecutive bytes of memory.
;Byte 1			Binary exponent E
;Byte 2 bit 7		Sign S
;Byte 2 bits 6-0	Mantissa msb M1
;Byte 3			Mantissa 2sb M2
;Byte 4			Mantissa lsb M3
;If E is 0, the value is 0 and bytes 2-4 are meaningless;  otherwise
;E represents the binary exponent with a bias of 80H.
;The sign bit S is 0 for positive, 1 for negative.
;The complete mantissa is M1-M3 with a hidden 1 replacing S in M1 bit 7
;and the binary point located left of M1.  Thus a nonzero value is
; S * [2^(E-80H)] * [(80H+M1) * (2^-8)] * [M2 * (2^-16)] * [M3 * (2^-24)].

;The floating point accumulator (FACC) contains a floating point value
;unpacked into five consecutive bytes of RAM starting at ACCE.
;Byte 1 (ACCE)		Exponent E, biased as above
;Byte 2 (ACCS)		Sign, 0 if negative and 80H if positive
;Bytes 3-5 (ACC1-ACC3)	Mantissa with hidden 1 restored in ACC1

;FLOATING POINT STORE
FSTR0:	MOV  M,E	;STORE ZEROETH WORD
	INX  H		;TO ADDR THE 1ST WORD
FSTOR:	MOV  M,A	;STORE 1ST WORD
FSTR1:	INX  H		;TO ADDR THE 2ND WORD
	MOV  M,B	;STORE 2ND WORD
	INX  H		;TO ADDR THE 3RD WORD
	MOV  M,C	;STORE 3RD WORD
	INX  H		;TO ADDR THE 4TH WORD
	MOV  M,D	;STORE 4TH WORD
	RET		;RETURN TO CALLER

;FLOATING POINT ZERO
FZRO:	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	XRA  A		;ZERO
	MOV  M,A	;CLEAR ACC EXPONENT
	RET		;RETURN TO CALLER

;FLOATING POINT CHANGE SIGN
FCHS:	MVI  A,80H	;MASK TO CHANGE ACC SIGN BIT
	DB   6		;THIS MVI A, SAVES A JUMP
			;AND CONTINUE AS IN FABS

;FLOATING POINT ABSOLUTE VALUE
FABS:	XRA  A		;NB MUST BE 1-BYTE INSTRUCTION
	LXI  H,ACCS	;TO ADDR THE ACC SIGN
	ANA  M		;COMPLIMENT OF SIGN
	XRI  80H	;COMPLIMENT THE SIGN BIT
	MOV  M,A	;ACC SIGN

;FLOATING POINT TEST
FTEST:	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	ANA  A		;SET CONTROL BITS
	JZ   FZRO	;IF ACC IS ZERO
	MOV  E,A	;ACC EXPONENT
	INX  H		;TO ADDR THE ACC SIGN
	MOV  A,M	;ACC SIGN
	INX  H		;TO ADDR THE ACC 1ST FRACTION
	XRA  M		;ACC SIGN AND 1ST FRACTION
	INX  H		;TO ADDR THE ACC 2ND FRACTION
	MOV  C,M	;ACC 2ND FRACTION
	INX  H		;TO ADDR THE ACC 3RD FRACTION
	MOV  D,M	;ACC 3RD FRACTION
	JMP  FADD8	;TO SET EXIT CONDITIONS

;FLOATING POINT LOAD
FLOAD:	MOV  A,M	;OPERAND EXPONENT
	ANA  A		;SET CONTROL BITS
	JZ   FZRO	;IF OPERAND IS ZERO
	MOV  E,A	;OPERAND EXPONENT
	INX  H		;TO ADDR THE OPERAND SIGN AND 1ST FRACTION
	MOV  A,M	;OPERAND SIGN AND 1ST FRACTION
	INX  H		;TO ADDR THE OPERAND 2ND FRACTION
	MOV  C,M	;OPERAND 2ND FRACTION
	INX  H		;TO ADDR THE OPERAND 3RD FRACTION
	MOV  D,M	;OPERAND 3RD FRACTION

;STORE THE OPERAND IN THE ACCUMULATOR
	MOV  L,A	;OPERAND SIGN AND 1ST FRACTION
	ORI  80H	;ACC 1ST FRACTION
	MOV  B,A	;ACC 1ST FRACTION
	XRA  L		;ACC SIGN
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	CALL FSTR0	;SET THE ACC
	XRA  B		;ACC SIGN AND 1ST FRACTION

; SET CONTROL BITS AND EXIT

	MOV  B,A	;ACC SIGN AND 1ST FRACTION
	ORI  01		;SET SIGN BIT FOR EXIT
	MOV  A,E	;ACC EXPONENT
	RET		;RETURN TO CALLER

;FLOATING POINT MULTIPLY
FMUL:	MOV  A,M	;OPERAND EXPONENT
	ANA  A		;SET CONTROL BITS
	CNZ  MDEX	;READ OPERAND IF NOT ZERO
	JZ   FZRO	;IF ZERO OR UNDERFLOW
	JC   FOVER	;IF OVERFLOW
	CALL MULX	;FIXED POINT MULTIPLY 

;NORMALIZE IF NECESSARY
	MOV  A,B	;1ST PRODUCT
	ANA  A		;SET CONTROL BITS
	JM   RNDA	;IF NO NORMALIZATION REQUIRED
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	SBI  01		;DECREMENT ACC EXPONENT
	MOV  M,A	;ACC EXPONENT
	RZ		;RETURN TO CALLER IF UNDERFLOW
	CALL LSH	;LEFT SHIFT SUBROUTINE

;ROUND IF NECESSARY
RNDA:	CALL ROND	;ROUNDING SUBROUTINE
	JC   FOVER	;IF OVERFLOW
	MOV  B,A	;ACC SIGN AND 1ST FRACTION
	ORI  01		;SET SIGN BIT
	MOV  A,E	;ACC EXPONENT
	RET		;RETURN TO CALLER

;FLOATING POINT DIVIDE
FDIVD:	XCHG		;XYBASIC ENTRY POINT
	INX	H
FDIV:	XRA  A		;ZERO
	SUB  M		;COMPLEMENT OF DIVISOR EXPONENT
	CPI  01		;SET CARRY IF DIVISION BY ZERO
	CNC  MDEX	;READ OPERAND IF NOT ZERO
	JC   FOVER	;IF OVERFLOW OR DIVISION BY ZERO
	JZ   FZRO	;IF UNDERFLOW OR ZERO
	MOV  C,A	;DIVISOR 1ST FRACTION
	CALL DIVX	;FIXED POINT DIVIDE
	JC   RNDA	;IF NO OVERFLOW
;FLOATING OVERFLOW -- ISSUE NONFATAL OV ERROR AND RETURN MAX VALUE
FOVER:	ERROR N, O, V	;ISSUE NONFATAL OV ERROR
	LXI  H,FPMAX	;MAX POSITIVE FP VALUE
	JMP  FLOAD	;TO ACC AND RETURN

;FLOATING POINT SUBTRACT
FSUB:	MVI  A,80H	;MASK TO CHANGE OPERAND SIGN
	DB   6		;THIS MVI B, SAVES A JUMP
			;CONTINUE AS IN FADD

;FLOATING POINT ADD
FADD:	XRA  A		;NB MUST BE 1-BYTE INSTRUCTION
;LOAD THE OPERAND
	MOV  E,M	;OPERAND EXPONENT
	INX  H		;TO ADDR THE OPERAND SIGN, 1ST FRACTION
	XRA  M		;OPERAND SIGN AND 1ST FRACTION
	MOV  B,A	;OPERAND SIGN AND 1ST FRACTION
	INX  H		;TO ADDR THE OPERAND 2ND FRACTION
	MOV  C,M	;OPERAND 2ND FRACTION
	INX  H		;TO ADDR THE OPERAND 3RD FRACTION
	MOV  D,M	;OPERAND 3RD FRACTION

;SAVE INITIAL EXPONENT
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	DCX  H		;TO ADDR THE INITIAL EXPONENT
	MOV  M,A	;INITIAL EXPONENT

;CHECK FOR ZERO OPERAND
	MOV  A,E	;OPERAND EXPONENT
	ANA  A		;SET CONTROL BITS
	JZ   FTEST	;IF OPERAND IS ZERO

;GENERATE SUBTRACT FLAG AND RESTORE SUPPRESSED FRACTION BIT
	MOV  L,B	;OPERAND SIGN AND 1ST FRACTION
	MOV  A,B	;OPERAND SIGN AND 1ST FRACTION
	ORI  80H	;OPERAND 1ST FRACTION
	MOV  B,A	;OPERAND 1ST FRACTION
	XRA  L		;OPERAND SIGN
	LXI  H,ACCS	;TO ADDR THE ACC SIGN
	XRA  M		;SUBTRACTION FLAG
	STA  SFLAG	;SUBTRACTION FLAG

;DETERMINE RELATIVE MAGNITUDES OF OPERAND AND ACCUMULATOR
	DCX  H		;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	ANA  A		;SET CONTROL BITS
	JZ   FADD9	;IF ACC IS ZERO
	SUB  E		;DIFFERENCE OF EXPONENTS
	JC   FADD2	;IF ACC SMALLER THAN OPERAND

;CHECK FOR INSIGNIFICANT OPERAND
	JM   FTEST	;IF OPERAND IS INSIGNIFICANT
	CPI  19H	;COMPARE SHIFT COUNT TO 25
	JC   FADD3	;POSITION THE OPERAND
	JMP  FTEST	;OPERAND IS INSIGNIFICANT

;CHECK FOR INSIGNIFICANT ACCUMULATOR
FADD2:	JP   FADD9	;IF ACC IS INSIGNIFICANT
	CPI  0E7H	;COMPARE SHIFT COUNT TO -25
	JC   FADD9	;IF ACC IS INSIGNIFICANT
	MOV  M,E	;OPERAND EXPONENT
	MOV  E,A	;SHIFT COUNT
	LDA  SFLAG	;SUBTRACT FLAG
	LXI  H,ACCS	;TO ADDR THE ACC SIGN
	XRA  M		;OPERAND SIGN
	MOV  M,A	;ACC SIGN
	XRA  A		;ZERO
	SUB  E		;COMPLIMENT SHIFT COUNT

;EXCHANGE ACCUMULATOR AND OPERAND
	INX  H		;TO ADDR THE ACC 1ST FRACTION
	MOV  E,M	;ACC 1ST FRACTION
	MOV  M,B	;OPERAND 1ST FRACTION
	MOV  B,E	;ACC 1ST FRACTION
	INX  H		;TO ADDR THE ACC 2ND FRACTION
	MOV  E,M	;ACC 2ND FRACTION
	MOV  M,C	;OPERAND 2ND FRACTION
	MOV  C,E	;ACC 2ND FRACTION
	INX  H		;TO ADDR THE ACC 3RD FRACTION
	MOV  E,M	;ACC 3RD FRACTION
	MOV  M,D	;OPERAND 3RD FRACTION
	MOV  D,E	;ACC 3RD FRACTION

;POSITION THE OPERAND
FADD3:	CALL RSH	;POSITION THE OPERAND
	LDA  SFLAG	;SUBTRACT FLAG
	ANA  A		;SET CONTROL BITS
	LXI  H,ACC3	;TO ADDR THE ACC 3RD FRACTION
	JM   FADD5	;IF SUBTRACT REQUIRED

;ADD ADDEND TO AUGEND
FADD4:	MOV  A,M	;AUGEND 3RD FRACTION
	ADD  D		;ADDEND 3RD FRACTION
	MOV  D,A	;SUM 3RD FRACTION
	DCX  H		;TO ADDR THE AUGEND 2ND FRACTION
	MOV  A,M	;AUGEND 2ND FRACTION
	ADC  C		;ADDEND 2ND FRACTION
	MOV  C,A	;SUM 2ND FRACTION
	DCX  H		;TO ADDR THE AUGEND 1ST FRACTION
	MOV  A,M	;AUGEND 1ST FRACTION
	ADC  B		;ADDEND 1ST FRACTION
	MOV  B,A	;SUM 1ST FRACTION
	JNC  FADD7	;IF NO CARRY FROM 1ST FRACTION

;RIGHT SHIFT SUM TO NORMALIZED POSITION
	RAR		;RIGHT SHIFT SUM 1ST FRACTION
	MOV  B,A	;SUM 1ST FRACTION
	MOV  A,C	;SUM 2ND FRACTION
	RAR		;RIGHT SHIFT SUM 2ND FRACTION
	MOV  C,A	;SUM 2ND FRACTION
	MOV  A,D	;SUM 3RD FRACTION
	RAR		;RIGHT SHIFT SUM 3RD FRACTION
	MOV  D,A	;SUM 3RD FRACTION
	RAR		;4TH FRACTION = LOW BIT OF 3RD
	MOV  E,A	;SUM 4TH FRACTION
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	ADI  1		;INCREMENT THE ACC EXPONENT
	JC   FOVER	;IF OVERFLOW
	MOV  M,A	;ACC EXPONENT
	JMP  FADD7	;TO ROUND FRACTION

;SUBTRACT SUBTRAHEND FROM MINUEND
FADD5:	XRA  A		;MINUEND 4TH FRACTION IS ZERO
	SUB  E		;SUBTRAHEND 4TH FRACTION
	MOV  E,A	;DIFFERENCE 4TH FRACTION
	MOV  A,M	;MINUEND 3RD FRACTION
	SBB  D		;SUBTRAHEND 3RD FRACTION
	MOV  D,A	;DIFFERENCE 3RD FRACTION
	DCX  H		;TO ADDR THE MINUEND 2ND FRACTION
	MOV  A,M	;MINUEND 2ND FRACTION
	SBB  C		;SUBTRAHEND 2ND FRACTION
	MOV  C,A	;DIFFERENCE 2ND FRACTION
	DCX  H		;TO ADDR THE MINUEND 1ST FRACTION
	MOV  A,M	;MINUEND 1ST FRACTION
	SBB  B		;SUBTRAHEND 1ST FRACTION
	MOV  B,A	;DIFFERENCE 1ST FRACTION
FADD6:	CC   COMP	;COMPLIMENT IF NEGATIVE
	CP   NORM	;NORMALIZE IF NECESSARY
	JP   FZRO	;IF UNDERFLOW OR ZERO
FADD7:	CALL ROND	;ROUNDING
	JC   FOVER	;IF OVERFLOW
FADD8:	MOV  B,A	;ACC SIGN AND 1ST FRACTION
	LXI  H,PREX	;TO ADDR THE PREVIOUS EXPONENT
	MOV  A,E	;ACC EXPONENT
	SUB  M		;DIFFERENCE THE EXPONENTS
	MOV  L,A	;DIFFERENCE IN EXPONENTS
	MOV  A,B	;ACC SIGN AND 1ST FRACTION
	ORI  1		;SET SIGN BIT FOR EXIT
	MOV  A,E	;ACC EXPONENT
	MOV  E,L	;SIGNIFICANCE INDEX
	RET		;RETURN TO CALLER

;LOAD THE ACCUMULATOR WITH THE OPERAND
FADD9:	LDA  SFLAG	;SUBTRACTION FLAG
	LXI  H,ACCS	;TO ADDR THE ACC SIGN
	XRA  M		;OPERAND SIGN
	DCX  H		;TO ADDR THE ACC EXPONENT
	CALL FSTR0	;SET THE ACC
	XRA  B		;ACC SIGN AND 1ST FRACTION
	JMP  FADD8	;JOIN EXIT CODE

;READ THE OPERAND AND CHECK THE ACCUMULATOR EXPONENT
MDEX:	MOV  B,A	;EXPONENT MODIFIER
	INX  H		;TO ADDR THE OPERAND SIGN AND 1ST FRACTION
	MOV  C,M	;OPERAND SIGN AND 1ST FRACTION
	INX  H		;TO ADDR OPERAND 2ND FRACTION
	MOV  D,M	;OPERAND 2ND FRACTION
	INX  H		;TO ADDR OPERAND 3RD FRACTION
	MOV  E,M	;OPERAND 3RD FRACTION
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	ANA  A		;SET CONTROL BITS
	RZ		;RETURN IF ACC IS ZERO
	ADD  B		;RESULT EXPONENT PLUS BIAS
	MOV  B,A	;RESULT EXPONENT PLUS BIAS
	RAR		;CARRY TO SIGN BIT
	XRA  B		;CARRY AND SIGN MUST DIFFER
	MOV  A,B	;RESULT EXPONENT PLUS BIAS
	MVI  B,80H	;EXPONENT BIAS, SIGN MASK, MSB
	JP   OVUN	;IF OVERFLOW OR UNDERFLOW
	SUB  B		;REMOVE EXCESS EXPONENT BIAS
	RZ		;RETURN IF UNDERFLOW
	MOV  M,A	;RESULT EXPONENT
	INX  H		;TO ADDR THE ACC SIGN
	MOV  A,M	;ACC SIGN
	XRA  C		;RESULT SIGN IN SIGN BIT
	ANA  B		;RESULT SIGN
	MOV  M,A	;RESULT SIGN
	MOV  A,C	;OPERAND SIGN AND 1ST FRACTION
	ORA  B		;OPERAND 1ST FRACTION
	RET		;RETURN TO CALLER
OVUN:	RLC		;SET CARRY BIT IF OVERFLOW
	RC		;RETURN IF OVERFLOW
	XRA  A		;ZERO
	RET		;RETURN IF UNDERFLOW

;LEFT SHIFT THE B, C, D, AND E REGISTERS ONE BIT
LSH:	MOV  A,E	;ORIGINAL CONTENTS OF E
	RAL		;LEFT SHIFT E
	MOV  E,A	;RESTORE TO E
	MOV  A,D	;ORIGINAL CONTENTS OF D
	RAL		;LEFT SHIFT D
	MOV  D,A	;RESTORE TO D
	MOV  A,C	;ORIGINAL CONTENTS OF C
	RAL		;LEFT SHIFT C
	MOV  C,A	;RESTORE TO C
	MOV  A,B	;ORIGINAL CONTENTS OF B
	ADC  A		;LEFT SHIFT B
	MOV  B,A	;RESTORE TO B
	RET		;RETURN TO CALLER

;RIGHT SHIFT THE B, C, D, AND E REGISTERS
;BY THE SHIFT COUNT IN THE A REGISTER.
;              - OR -
;SHIFT OPERAND TO REGISTER INDICATED
;BY THE SHIFT COUNT(MODULO 8).
RSH:	MVI  E,0	;OPERAND 4TH FRACTION IS ZERO
	MVI  L,08	;EACH REG IS 8 BITS OF SHIFT
RSH1:	CMP  L		;COMPARE SHIFT COUNT TO 8
	JM   RSH2	;IF REQUIRED SHIFT IS < 8
	MOV  E,D	;OPERAND 4TH FRACTION
	MOV  D,C	;OPERAND 3RD FRACTION
	MOV  C,B	;OPERAND 2ND FRACTION
	MVI  B,0	;OPERAND 1ST FRACTION IS ZERO
	SUB  L		;REDUCE SHIFT COUNT BY ONE REGISTER
	JNZ  RSH1	;IF MORE SHIFTS REQUIRED

;SHIFT OPERAND RIGHT BY 'SHIFT COUNT' BITS
RSH2:	ANA  A		;SET CONTROL BITS
	RZ		;RETURN IF SHIFT COMPLETE
	MOV  L,A	;SHIFT COUNT
RSH3:	ANA  A		;CLEAR CARRY BIT
	MOV  A,B	;OPERAND 1ST FRACTION
	RAR		;RIGHT SHIFT 1ST FRACTION
	MOV  B,A	;OPERAND 1ST FRACTION
	MOV  A,C	;OPERAND 2ND FRACTION
	RAR		;RIGHT SHIFT 2ND FRACTION
	MOV  C,A	;OPERAND 2ND FRACTION
	MOV  A,D	;OPERAND 3RD FRACTION
	RAR		;RIGHT SHIFT 3RD FRACTION
	MOV  D,A	;OPERAND 3RD FRACTION
	MOV  A,E	;OPERAND 4TH FRACTION
	RAR		;RIGHT SHIFT 4TH FRACTION
	MOV  E,A	;OPERAND 4TH FRACTION
	DCR  L		;DECREMENT SHIFT COUNT
	JNZ  RSH3	;IF MORE SHIFTS REQUIRED
	RET		;RETURN TO CALLER

;COMPLIMENT THE B, C, D, AND E REGISTERS
COMP:	DCX  H		;TO ADDR THE ACC SIGN
	MOV  A,M	;ACC SIGN
	XRI  80H	;CHANGE SIGN
	MOV  M,A	;ACC SIGN
COMP1:	XRA  A		;ZERO -- FFIX ENTRY POINT
	MOV  L,A	;ZERO
	SUB  E		;COMPLIMENT 4TH FRACTION
	MOV  E,A	;RESTORE TO E
	MOV  A,L	;ZERO
	SBB  D		;COMPLIMENT 3RD FRACTION
	MOV  D,A	;RESTORE TO D
	MOV  A,L	;ZERO
	SBB  C		;COMPLIMENT 2ND FRACTION
	MOV  C,A	;RESTORE TO C
	MOV  A,L	;ZERO
	SBB  B		;COMPLIMENT 1ST FRACTION
	MOV  B,A	;RESTORE TO B
	RET		;RETURN TO CALLER

;NORMALIZE THE REGISTERS
NORM:	MVI  L,20H	;MAXIMUM NORMALIZING SHIFT
NORM1:	MOV  A,B	;1ST FRACTION
	ANA  A		;SET CONTROL BITS
	JNZ  NORM3	;IF 1ST FRACTION IS NON ZERO
	MOV  B,C	;1ST FRACTION
	MOV  C,D	;2ND FRACTION
	MOV  D,E	;3RD FRACTION
	MOV  E,A	;ZERO 4TH FRACTION
	MOV  A,L	;NORMALIZING SHIFT COUNT
	SUI  08		;REDUCE SHIFT COUNT
	MOV  L,A	;NORMALIZING SHIFT COUNT
	JNZ  NORM1	;IF FRACTION NON ZERO
	RET		;IF FRACTION IS ZERO
NORM2:	DCR  L		;DECREMENT SHIFT COUNT
	MOV  A,E	;ORIGINAL CONTENTS OF E
	RAL		;LEFT SHIFT E
	MOV  E,A	;RESTORE TO E
	MOV  A,D	;ORIGINAL CONTENTS OF D
	RAL		;LEFT SHIFT D
	MOV  D,A	;RESTORE TO D
	MOV  A,C	;ORIGINAL CONTENTS OF C
	RAL		;LEFT SHIFT C
	MOV  C,A	;RESTORE TO C
	MOV  A,B	;ORIGINAL CONTENTS OF B
	ADC  A		;LEFT SHIFT B
	MOV  B,A	;RESTORE TO B
NORM3:	JP   NORM2	;IF NOT YET NORMALIZED
	MOV  A,L	;NORMALIZING SHIFT COUNT
	SUI  20H	;REMOVE BIAS
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	ADD  M		;ADJUST ACC EXPONENT
	MOV  M,A	;NEW ACC EXPONENT
	RZ		;RETURN IF ZERO EXPONENT
	RAR		;BORROW BIT TO SIGN
	ANA  A		;SET SIGN TO INDICATE UNDERFLOW
	RET		;RETURN TO CALLER

;ROUND THE B, C, AND D REGISTERS
ROND:	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,E	;4TH FRACTION
	ANA  A		;SET CONTROL BITS
	MOV  E,M	;ACC EXPONENT
	CM   RNDR	;CALL 2ND LEVEL ROUNDER
	RC		;RETURN IF OVERFLOW
	MOV  A,B	;1ST FRACTION
	INX  H		;TO ADDR THE ACC SIGN
	XRA  M		;ACC SIGN AND 1ST FRACTION
	JMP  FSTR1	;RETURN THRU 'STORE' SUBROUTINE

;SECOND LEVEL ROUNDER
RNDR:	INR  D		;ROUND 3RD FRACTION
	RNZ		;RETURN IF NO CARRY
	INR  C		;CARRY TO 2ND FRACTION
	RNZ		;RETURN IF NO CARRY
	INR  B		;CARRY TO 1ST FRACTION
	RNZ		;RETURN IF NO CARRY
	MOV  A,E	;ACC EXPONENT
	ADI  1		;INCREMENT ACC EXPONENT
	MOV  E,A	;NEW ACC EXPONENT
	MVI  B,80H	;NEW 1ST FRACTION
	MOV  M,A	;NEW ACC EXPONENT
	RET		;RETURN TO 1ST LEVEL ROUNDER

;FIXED POINT MULTIPLY
MULX:	LXI  H,MULX4+9	;TO ADDR THE 1ST MULTIPLICAND
	MOV  M,A	;1ST MULTIPLICAND
	LXI  H,MULX4+5	;TO ADDR THE 2ND MULTIPLICAND
	MOV  M,D	;2ND MULTIPLICAND
	LXI  H,MULX4+1	;TO ADDR THE 3RD MULTIPLICAND
	MOV  M,E	;3RD MULTIPLICAND
	XRA  A		;CLEAR 6TH PRODUCT
	MOV  E,A	;CLEAR 5TH PRODUCT
	MOV  D,A	;CLEAR 4TH PRODUCT

;MULTIPLY BY EACH ACCUMULATOR FRACTION IN TURN
	LXI  H,ACC3	;TO ADDR THE 3RD FRACTION
	CALL MULX2	;MULTIPLY BY ACC 3RD FRACTION
	LXI  H,ACC2	;TO ADDR THE 2ND FRACTION
	CALL MULX1	;MULTIPLY BY ACC 2ND FRACTION
	LXI  H,ACC1	;TO ADDR THE 1ST FRACTION

;MULTIPLY BY ONE ACCUMULATOR WORD
MULX1:	MOV  A,D	;5TH PARTIAL PRODUCT
	MOV  E,C	;4TH PARTIAL PRODUCT
	MOV  D,B	;3RD PARTIAL PRODUCT
MULX2:	MOV  B,M	;MULTIPLIER
	MOV  L,A	;5TH PARTIAL PRODUCT
	XRA  A		;ZERO
	MOV  C,A	;TO 2ND PARTIAL PRODUCT
	SUB  B		;SET CARRY BIT FOR EXIT FLAG
	JNC  FFIX0	;MULTIPLY BY 0

;LOOP FOR EACH BIT OF MULTIPLIER WORD
MULX3:	MOV  A,L	;5TH PARTIAL PRODUCT, EXIT FLAG
	ADC  A		;SHIFT EXIT FLAG OUT IF DONE
	RZ		;EXIT IF MULTIPLICATION DONE
	MOV  L,A	;5TH PARTIAL PRODUCT, EXIT FLAG
	MOV  A,E	;4TH PARTIAL PRODUCT
	RAL		;SHIFT 4TH PARTIAL PRODUCT
	MOV  E,A	;4TH PARTIAL PRODUCT
	MOV  A,D	;3RD PARTIAL PRODUCT
	RAL		;SHIFT 3RD PARTIAL PRODUCT
	MOV  D,A	;3RD PARTIAL PRODUCT
	MOV  A,C	;2ND PARTIAL PRODUCT
	RAL		;SHIFT 2ND PARTIAL PRODUCT
	MOV  C,A	;2ND PARTIAL PRODUCT
	MOV  A,B	;1ST PARTIAL PRODUCT AND MULTIPLIER
	RAL		;SHIFT THEM
	MOV  B,A	;1ST PARTIAL PRODUCT AND MULTIPLIER
	JNC  MULX3	;IF NO ADDITION REQUIRED

;ADD THE MULTIPLICAND TO THE PRODUCT IF THE MULTIPLIER BIT IS A ONE
	MOV  A,E	;4TH PARTIAL PRODUCT
	CALL MULX4	;TO RAM CODE

;COMPLETE ADDITION OF MULTIPLICAND
	MOV  C,A	;2ND PARTIAL PRODUCT
	JNC  MULX3	;IF NO CARRY TO 1ST PRODUCT
	INR  B		;ADD CARRY TO 1ST PRODUCT
	ANA  A		;CLEAR CARRY BIT
	JMP  MULX3

;FIXED POINT DIVIDE
;
;SUBTRACT THE DIVISOR FROM THE ACCUMULATOR TO OBTAIN THE FIRST REMAINDER
DIVX:	LXI  H,ACC3	;TO ADDR THE ACC 3RD FRACTION
	MOV  A,M	;ACC 3RD FRACTION
	SUB  E		;DIVISOR 3RD FRACTION
	MOV  M,A	;REMAINDER 3RD FRACTION
	DCX  H		;TO ADDR THE ACC 2ND FRACTION
	MOV  A,M	;ACC 2ND FRACTION
	SBB  D		;DIVISOR 2ND FRACTION
	MOV  M,A	;REMAINDER 2ND FRACTION
	DCX  H		;TO ADDR THE ACC 1ST FRACTION
	MOV  A,M	;ACC 1ST FRACTION
	SBB  C		;DIVISOR 1ST FRACTION
	MOV  M,A	;REMAINDER 1ST FRACTION

;HALVE THE DIVISOR AND STORE FOR ADDITION OR SUBTRACTION
	MOV  A,C	;DIVISOR 1ST FRACTION
	RAL		;SET CARRY BIT
	MOV  A,C	;DIVISOR 1ST FRACTION
	RAR		;HALF OF DIVISOR 1ST FRACTION
	STA  DIVX5+12	;1ST SUBTRACT DIVISOR
	STA  DIVX6+9	;1ST ADD DIVISOR
	MOV  A,D	;DIVISOR 2ND FRACTION
	RAR		;HALF OF DIVISOR 2ND FRACTION
	STA  DIVX5+8	;2ND SUBTRACT DIVISOR
	STA  DIVX6+5	;2ND ADD DIVISOR
	MOV  A,E	;DIVISOR 3RD FRACTION
	RAR		;HALF OF DIVISOR 3RD FRACTION
	STA  DIVX5+4	;3RD SUBTRACT DIVISOR
	STA  DIVX6+1	;3RD ADD DIVISOR
	MVI  B,0	;INIT QUOTIENT 1ST FRACTION
	MOV  A,B	;DIVISOR 4TH QUOTIENT IS ZERO
	RAR		;LOW BIT OF DIVISOR 3RD FRACTION
	STA  DIVX5+1	;4TH SUBTRACT DIVISOR
	STA  DIVX5+15	;4TH ADD DIVISOR
	STA  DIVX6+12	;4TH ADD DIVISOR

;LOAD FIRST REMAINDER AND CHECK SIGN
	LXI  H,ACC1	;TO ADDR THE REMAINDER 1ST FRACTION
	MOV  A,M	;REMAINDER 1ST FRACTION
	INX  H		;TO ADDR THE REMAINDER 2ND FRACTION
	MOV  D,M	;REMAINDER 2ND FRACTION
	INX  H		;TO ADDR THE REMAINDER 3RD FRACTION
	MOV  E,M	;REMAINDER 3RD FRACTION
	ANA  A		;SET CONTROL BITS
	JM   DIVX4	;IF REMAINDER IS NEGATIVE

;ADJUST THE EXPONENT, POSITION THE REMAINDER, AND INITIALIZE THE QUOTIENT
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  C,M	;QUOTIENT EXPONENT
	INR  C		;INCREMENT THE QUOTIENT EXPONENT
	RZ		;RETURN IF OVERFLOW
	MOV  M,C	;QUOTIENT EXPONENT
	MOV  L,E	;REMAINDER 3RD FRACTION
	MOV  H,D	;REMAINDER 2ND FRACTION
	MOV  E,A	;REMAINDER 1ST FRACTION
	MVI  D,1	;INITIALIZE QUOTIENT 3RD FRACTION
	MOV  C,B	;INITIALIZE QUOTIENT 2ND FRACTION

;SUBTRACT THE DIVISOR FROM THE REMAINDER IF IT IS POSITIVE
DIVX1:	XRA  A		;REMAINDER 4TH FRACTION IS ZERO
	CALL DIVX5	;CALL RAM CODE
DIVX2:	RLC		;SHIFT REMAINDER 4TH FRACTION TO CARRY

;SHIFT THE REMAINDER LEFT ONE BIT
	MOV  A,B	;QUOTIENT 1ST FRACTION
	RAL		;MSB OF QUOTIENT TO CARRY
	RC		;IF DIVISION COMLPETE
	RAR		;REMAINDER 4TH FRACTION TO CARRY
	MOV  A,L	;REMAINDER 3RD FRACTION
	RAL		;LEFT SHIFT REMAINDER 3RD FRACTION
	MOV  L,A	;REMAINDER 3RD FRACTION
	MOV  A,H	;REMAINDER 2ND FRACTION
	RAL		;LEFT SHIFT REMAINDER 2ND FRACTION
	MOV  H,A	;REMAINDER 2ND FRACTION
	CALL LSH	;LEFT SHIFT IT

;BRANCH IF SUBTRACTION IS REQUIRED
	MOV  A,D	;QUOTIENT 3RD FRACTION
	RRC		;REMAINDER SIGN TO CARRY BIT
	JC   DIVX1	;SUBTRACT DIVISOR IF REMAINDER IS POSITIVE

;ADD THE DIVISOR IF THE REMAINDER IS NEGATIVE
DIVX3:	MOV  A,L	;REMAINDER 3RD FRACTION
	CALL DIVX6	;TO RAM CODE
	JMP  DIVX2

;POSITION THE REMAINDER AND INITIALIZE THE QUOTIENT
DIVX4:	MOV  L,E	;REMAINDER 3RD FRACTION
	MOV  H,D	;REMAINDER 2ND FRACTION
	MOV  E,A	;REMAINDER 1ST FRACTION
	MOV  D,B	;INITIALIZE QUOTIENT 3RD FRACTION
	MOV  C,B	;INITIALIZE QUOTIENT 2ND FRACTION
	JMP  DIVX3	;ADD DIVISOR IF REMAINDER IS NEGATIVE

;FIXED POINT TO FLOATING POINT CONVERSION
FFLOT:	MOV  L,E	;INPUT EXPONENT
	MOV  E,D	;4TH INPUT FRACTION
	MOV  D,C	;3RD INPUT FRACTION
	MOV  C,B	;2ND INPUT FRACTION
	MOV  B,A	;1ST INPUT FRACTION
	MOV  A,L	;INPUT EXPONENT
FLOT1:	XRI  80H	;APPLY EXPONENT BIAS
	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  M,A	;ACC EXPONENT
	INX  H		;TO ADDR THE ACC SIGN
	MVI  M,80H	;SET ACC SIGN POSITIVE
	INX  H		;TO ADDR THE ACC 1ST FRACTION
	MOV  A,B	;1ST INPUT FRACTION
	ANA  A		;SET SIGN BIT
	RAL		;INPUT SIGN TO CARRY
	JMP  FADD6	;COMPLETE THE CONVERSION

;FLOATING POINT TO FIXED POINT CONVERSION
FFIX:	LXI  H,ACCE	;TO ADDR THE ACC EXPONENT
	MOV  A,M	;ACC EXPONENT
	ANA  A		;SET CONTROL BITS
	JZ   FFIX1	;IF ACC IS ZERO
	MOV  A,E	;INPUT EXPONENT
	ADI  7FH	;APPLY BIAS-1
	SUB  M		;SHIFT COUNT - 1
	jc   ffix2	;check for -32768 if too large
	CPI  1FH	;COMPARE TO LARGE SHIFT
	JNC  FFIX1	;IF ACC TOO SMALL
	ADI  1		;SHIFT COUNT
	LXI  H,ACC1	;TO ADDR THE ACC 1ST FRACTION
	MOV  B,M	;ACC 1ST FRACTION
	INX  H		;TO ADDR THE ACC 2ND FRACTION
	MOV  C,M	;ACC 2ND FRACTION
	INX  H		;TO ADDR THE ACC 3RD FRACTION
	MOV  D,M	;ACC 3RD FRACTION
	CALL RSH	;POSITION THE FRACTION
	LXI  H,ACCS	;TO ADDR THE ACC SIGN
	MOV  A,M	;ACC SIGN
	ANA  A		;SET CONTROL BITS
	CP   COMP1	;COMPLIMENT FRACTION IF NEGATIVE
	MVI  A,1	;NON-ZERO
	ORA  B		;SET CONTROL BITS FOR EXIT
	MOV  A,B	;1ST RESULT
	MOV  B,C	;2ND RESULT
FFIX0:	MOV  C,D	;3RD RESULT  -- MULX2 ENTRY POINT
	MOV  D,E	;4TH RESULT
	RET		;RETURN TO CALLER
FFIX1:	XRA  A		;ZERO
	MOV  B,A	;ZERO
	MOV  C,A	;ZERO
	MOV  D,A	;ZERO
	RET		;RETURN TO CALLER
;FIX2 checks for -32768 (90H, 0, 80H, 0, 0) in FACC, otherwise returns Carry.
ffix2:	mov  a,m
	sui  90H
	inx  h
	ora  m
	inx  h
	mov  c,m	;ACC1 to C
	inx  h
	ora  m
	inx  h
	ora  m
	stc
	rnz		;ACCE <> 90H or ACCS, ACC2, ACC3 <> 0
	mov  b,a	;0 to B in case success
	mov  a,c
	cpi  80H
	rz		;-32768, return 80H in A and 0 in B
	stc
	ret		;ACC1 <> 80H

;floating point increment routine for fast NEXT code
fincr:	mov	a,m	;get argument exponent
	cpi	81h	;does it have the exponent of a fp one?
	jnz	fadd	;nope - have to plow thru a fp add
	push	h	;yup - test remainder of bytes
	inx	h
	mov	a,m
	inx	h
	ora	m
	inx	h
	ora	m
	pop	h
	jnz	fadd	;not a fp one, have to use add
	push	h
	lxi	h,acce	;see if acc is easy to increment
	mov	a,m
	ora	a	;is it zero?
	jz	finc1	;yes - hard to increment
	dcr	a	;generate a carry if FACC is >=1 and
	inx	h
	add	m	;positive
finc1:	pop	h
	jnc	fadd	;not easy to increment
	lda	acce	;get current exponent
	mvi	e,0	;to defeat etest
	cpi	25+81h	;will exponents differ by more than 23?
	rnc		;yes - no point in adding
	sui	81h	;difference of exponents now in reg a
	lxi	b,8000h	;put mantissa of fp one into regs BCD
	mov	d,c
	jz	finc4
	mov	e,a	;save difference
	ani	7	;take difference mod 8
	mov	l,a	;bits to rotate the signifficant bit
	mov	a,b	;fetch sig bit to rotate
	jz	finc3	;bit in right pos, now pick right byte
finc2:	rrc		;keep rotating until in right place
	dcr	l
	jnz	finc2
finc3:	mov	b,a	;assume bit belongs in 1st fraction
	mov	a,e	;test number of bits to see if it is so
	cpi	8	;set carry if it belongs here
	jc	finc4	;in which case, all is shifted
	mov	c,b	;assume bit belongs in 2nd fraction
	mov	b,l	;zero 1st fraction
	cpi	16	;set carry if it belongs in 2nd fraction
	jc	finc4
	mov	d,c	;well, it must belong in 3rd fraction
	mov	c,l	;zero second fraction
finc4:	lxi	h,acc3	;needed by add4
	jmp	fadd4


	endif		;end of FLOAT conditional

;end of FMATH
	PAGE
;FNSOPS 11/05/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;arithmetic and control function and op routines

;auxilliary functions called by fn / op routines

;READP reads the port # given in C.
	if	not wild
readp:	call	isbyt
	mov	a,c
rdp1:	sta	rport+1		;set port #
	jmp	rport		;read the port and return
	endif

;STBIT sets bit # [(E) and (D)] of DE.
;Retn:	A,HL	clobbered
;	BC	preserved
;	DE	bit # [(E) and (D)] set, others reset
	if	not wild
stbt0:	mvi	d,0FH
stbit:	mov	a,e
	ana	d		;(E) masked by (D) to A, carry reset
	lxi	d,1		;set bit 0 of DE
stbi1:	rz			;done if (A) = 0
	xchg
	dad	h		;move bit left in HL
	xchg
	dcr	a
	jmp	stbi1
	endif

;SNFIX replaces (BC) and (DE) by their abs values, sets sign bit
;of TEMP iff exactly one of them was < 0.
;SNFX0 does SNFIX, returns Carry if (DE) = 0.
snfx0:	mov	a,d
	ora	e
	stc
	rz			;return Carry if DE=0, else do SNFIX
snfix:	mov	a,b
	xra	d		;desired sign to A7
	sta	temp		;and saved
	call	iabs		;replace BC by abs(BC)
	rc
	mov	a,d
	ora	a
	cm	cplde		;replace DE by -(DE) if negative
	ret

;MULTY multiplies nonnegative integers in A and DE, leaves HL + product in HL.
;NB MULTY is used by RND to produce (HL) + (A) * (DE) mod 2 ^ 16 exactly, even
;on overflow, and is therefore more complicated than it would otherwise be.
;Call:	A	nonnegative multiplier
;	DE	nonnegative multiplicand
;Retn:	A	zero unless overflow
;	BC	preserved
;	DE	clobbered
;	HL	(HL) + (A) * (DE), mod 2 ^ 16 if overflow
;	Carry	set iff overflow
;MULBD multiplies unsigned values in BC and DE, returns result in HL.
mulbd:	call	cmbdu
	cnc	bcde		;force BC <= DE
	mov	a,b
	ora	a
	stc
	rnz			;overflow if both args >= 2 ^ 8
	mov	a,c		;multiplier to A and fall though to MULT0
mult0:	lxi	h,0
multy:	push	b
	mvi	b,0		;keep track of overflow in B
mult1:	ora	a		;clear carry
	rar			;get next bit of multiplier in carry
	jnc	mult2
	dad	d		;add multiplicand to partial product
	cc	ovset		;overflow
mult2:	ora	a
	jz	mult3		;done when no more bits on
	xchg
	dad	h		;shift multiplicand left one
	xchg
	cc	ovset		;overflow
	jmp	mult1
mult3:	mov	a,b
	pop	b
	ral			;carry set iff overflow
	ret

;DIVD0 does the work of division.
;Call:	BC	nonnegative dividend
;	DE	nonnegative nonzero divisor
;Retn:	DE	remainder
;	HL	quotient
;The dividend starts in HL and is shifted bit by bit into DE as quotient bits
;are shifted into HL.  At DIVD1, we have [0 bits : current dividend] in DE,
;[dividend bits : quotient bits] in HL.
;A tells us how many bits are left of : in the above description.
divd0:	mov	h,b
	mov	l,c		;dividend to HL
	call	umind		;- divisor to BC
	lxi	d,0		;current dividend 0
	mvi	a,16		;count to A
	ora	a		;first quotient bit 0
divd1:	push	psw		;save quotient bit
	dad	h		;shift HL left
	push	psw		;save high bit of dividend
	xchg
	dad	h		;shift DE left
	pop	psw
	cc	inxh		;add in dividend bit
	pop	psw
	jnc	divd3
	inx	d		;add in quotient bit
divd3:	push	h		;save current dividend
	dad	b		;try subtracting divisor
	jnc	divd4		;too big, retain old one
	xthl			;ok, keep new one
divd4:	pop	h		;carry set iff subtraction worked
	xchg
	dcr	a
	jnz	divd1		;keep going
	aci	0		;zero set iff carry reset
	dad	h		;shift quotient one more time
	rz
inxh:	inx	h		;add in last quotient bit
	ret

;AMBOP is called during execution of an ambiguous binary op
;(namely + - * / and relations).  The args are forced to
;match and are fetched, and the type is returned in the status bits.
;In addition, the arg1 pointer is saved in TEMP2 for RETRY if integer.
;Call:	BC,DE	arg1, arg2 pointers
;Retn:	BC,DE	arg1, arg2 values if integer
;	CDE,BHL	arg1, arg2 values if string
;	FACC,HL	arg1 value, arg2 pointer if floating
;	Carry	set iff string
;	Zero	set iff integer
ambop:	xchg			;arg2 addr to HL
	ldax	b		;fetch arg1 type
ambo1:	cmp	m		;RETRY entry point
	cnz	ambty		;force types to agree
	push	b		;save arg1 pointer
	call	fetch		;arg2 pointer to HL
	iftyp	ambof, ambos	;branch if noninteger
	pop	h		;arg1 pointer to HL
	push	b		;save arg2 value
	call	fetch		;fetch arg1 value to BC
	pop	d		;arg2 value to DE
	ret
	if	strng
ambos:	pop	h		;arg1 pointer to HL
	mov	b,c		;arg2 length to B
	push	d		;save arg2 loc
	call	fetch		;arg1 to CDE
	pop	h		;arg2 loc to HL
	ret
	endif
	if	float
ambof:	xthl			;save arg2 pointer, arg1 pointer to HL
	call	fetch		;fetch arg1
	pop	h
	inx	h		;point to arg2 value with HL
	ret
	endif
;AMBTY forces both args to floating.
ambty:	if	float
	call	cnvtf		;float arg2
	push	b
	xthl
	call	cnvrt		;float arg1
	xthl
	pop	b
	ret
	else
	jmp	tmerr		;fatal TM error if nonfloating version
	endif

;CMPAR does the work of comparing two args for relation routines.
;Call:	BC, DE	arg pointers
;Retn:	BC	0
;	Carry, Zero	set as for CMBDS
cmpar:	call	ambop		;fetch args, set status bits
	iftyp	cmpaf, cmpas	;branch if noninteger
	call	cmbds		;compare integer arg1 to arg2, signed
	lxi	b,0
	ret
	if	strng
cmpas:	call	cmstr		;compare string arg1 to arg2
	lxi	b,0
	ret
	endif
	if	float
cmpaf:	call	cmpfl		;perform floating compare
	lxi	b,0
	rp			;arg1 > arg2, return Carry reset
	rz			;arg1 = arg2, return Zero set
	stc			;arg1 < arg2, return Carry set
	ret

;ETEST tests the significance value in E.  If the result is
;insignificant it is replaced by a floating zero.
	if	(not f9511) and (not fpbcd)
etest:	mov	h,a		;save A
	mov	a,e
	adi	80H
	cpi	(80H-sindx) and 255	;compare to desired significance index
	mov	a,h		;restore A
	rnc			;result significant
	cpi	83H-sindx	;compare exponent to see if small
	rnc			;not small, take computed value
	jmp	fzro		;result insignificant, take 0. instead
	endif

;CMPFL compares two FP numbers, returns Sign and Zero set as for FTEST.
	if	f9511
cmpfl:	mvi	a,xch95
	call	o9511		;exchange order of args
cmpf0:	call	fsub0		;subtract args -- BDTST entry point
	in	c9511		;read result status
	ani	zer95		;check Zero bit
	jnz	cmpf1		;result is 0
	in	c9511
	ral			;sign status to bit A7
	ani	80H		;mask to sign bit
	ori	1		;reset Zero, set Sign appropriately
	ret
cmpf1:	xra	a		;set Zero, reset Sign
	ret
	else			;NOT F9511
	if	fpbcd
cmpfl:	call	fsub		;subtract
ftest:	lda	facc		;fetch result sign/exponent byte
	ora	a
	ret			;Zero set iff equal, else sign set
	else
cmpfl:	xchg			;arg2 exp address to DE
	lxi	h,acce		;arg1 (in FACC) exponent address to HL
	mov	a,m
	ora	a		;Zero set iff exp(arg1) = 0
	ldax	d		;fetch exp(arg2)
	jz	cmpf5
	ora	a		;Zero set iff exp(arg2) = 0
	inx	h
	jz	cmpf7
	inx	d
	ldax	d		;fetch sign(arg2)
	xra	m		;compare to sign(arg1)
	jp	cmpf6		;signs not equal
	dcx	h
	dcx	d
	ldax	d		;fetch exp(arg2) again
	sub	m		;compare to exp(arg1)
	jnz	cmpf2		;exponents not equal
	inx	h
	inx	h
	inx	d
	ldax	d		;fetch mmsb(arg2)
	ori	80H		;restore hidden bit
	cmp	m		;compare to msb(arg1)
	jnz	cmpf4		;msb of fraction not equal
	inx	h
	inx	d
	ldax	d		;fetch byte2(arg2)
	cmp	m		;compare to byte2(arg1)
	jnz	cmpf4		;byte2 of fraction not equal
	dcx	d
	dcx	d		;readdress exp(arg2)
cmpf1:	xchg			;arg2 pointer to HL
	call	fsub		;arg1-arg2
	call	etest		;check if almost 0
	jmp	ftest		;set status bits and return
cmpf2:	dcr	a
	jz	cmpf3		;exponents differ by 1
	inr	a
	inr	a
	jnz	cmpf4		;exponents differ by more than 1
cmpf3:	inx	h
	inx	h
	inx	d
	ldax	d		;fetch msb(arg2)
	xra	m		;msb(arg2) xor msb(arg1)
	dcx	d
	inr	a		;Zero set if near exponent boundary
	jz	cmpf1		;in which case do the subtraction
	dcx	h
	dcx	h
	ldax	d		;fetch exp(arg2)
	cmp	m		;Carry set if exp(arg2) < exp(arg1)
cmpf4:	rar			;Carry to A7
	lxi	h,accs		;address arg1 sign
	xra	m		;complement unless negative
	ori	1		;assure Zero reset
	ret
cmpf5:	ora	a
	rz			;both args 0, hence equal
	inx	d
cmpf6:	xchg			;sign(arg2) will give result
cmpf7:	mov	a,m
	xri	80H		;complemented sign(arg1) gives result
	ori	1		;assure Zero reset
	ret
	endif			;end of NOT FPBCD conditional
	endif			;end of NOT F9511 conditional

retry:	lhld	argad		;arg1 pointer to HL
	mov	b,h
	mov	c,l		;and then to BC
	lxi	h,vbyts+2
	dad	b		;arg2 pointer to HL
	xra	a		;to assure nonzero CMP at AMBO1
	jmp	ambo1		;convert to floating and refetch
rtry1:	lhld	argad		;recover arg pointer
cnvtf:	mvi	a,sngst
	jmp	cnvrt		;float it and return
	endif

fcern:	error	n, F, C		;issue nonfatal FC error
	ret			;and return
fcerf:	error	f, F, C		;fatal FC error


;routines for fn and op execution

;ambiguous arithmetic ops
;AOP is a macro for executing ambiguous ops, namely + - * ABS and unary -.
;IOP, FOP and SOP are addresses of integer, floating and string routines.
;UNARY is a flag, TRUE if unary and FALSE if binary.
;SGNIF is a flag for significance checking, TRUE for + and -.
;FCERF is fatal FC error for string arg to arithmetic routine
aop	macro	iop,fop,sop,unary,sngif
	local	exfop
	if	unary		;;first fetch args and set status bits
	call	fetbc		;;fetch one arg from BC if unary
	else
	call	ambop		;;fetch two agreeing args if binary
	endif
	iftyp	exfop, sop	;;branch if noninteger
	call	iop		;;execute integer op
	mvi	a,intst		;;return type integer
	rnc			;;done unless overflow
	if	float
	if	unary
	call	rtry1		;;recover arg and float it
	else
	call	retry		;;recover args and float them
	endif
exfop:	call	fop		;;execute floating op
	if	f9511
	call	fet95		;;fetch result if 9511 op
	else
	if	sngif and not fpbcd
	call	etest		;;test value in E for significance if SGNIF
	endif
	endif
	mov	e,a		;;save A
	mvi	a,sngst		;;return type floating
	ret
	else
	jmp	iover		;;integer OV error if not floating version
	endif
	endm

;Addition(+): {<integer>|<floating>|<string>} x {<integer>|<floating>|<string>}
;	--> {<integer>|<floating>|<string>}
aadd:	aop	iadd,fadd,sadd,false,true

;Subtraction (-): {<integer>|<floating>} x {<integer>|<floating>}
;	--> {<integer>|<floating>}
asub:	aop	isub,fsub,fcerf,false,true

;Multiplication (*): {<integer>|<floating>} x {<integer>|<floating>}
;	--> {<integer>|<floating>}
amul:	aop	imul,fmul,fcerf,false,false

;Unary minus (-): {<integer> | <floating>} --> {<integer> | <floating>}
aumin:	aop	iumin,fchs,fcerf,true,false

;ABS: {<integer> | <floating>} --> {<integer> | <floating>}
aabs:	aop	iabs,fabs,fcerf,true,false

;Integer ABS, unary minus: <integer> --> <integer>
;IABS and IUMIN preserve DE and HL.
iabs:	mov	a,b
iabs1:	ora	a		;sign set iff (BC) <0
	rp			;return if > 0, otherwise perform unary minus
iumin:	call	bcde		;arg to DE, DE to BC
umind:	call	cplde		;complement it
;BCDE exchanges BC and DE.
bcde:	push	b
	mov	b,d
	mov	c,e
	pop	d
	ret

;Integer addition and subtraction: <integer> x <integer> --> <integer>
isub:	call	cplde		;complement and add
	rc
iadd:	mov	a,d
	xra	b		;sign (BC) xor sign (DE) to sign
	mov	a,b		;sign of BC to A7 in case agree
	xchg			;arg2 to HL
	dad	b
	mov	b,h
	mov	c,l		;(BC) + (DE) to BC
	jm	retnc		;result ok if signs differ, return Carry reset
	xra	b		;compare actual & desired result signs
	rp			;return if no overflow
stcr:	stc
	ret

;Integer multiplication: <integer> x <integer> --> <integer>
imul:	call	snfix		;force BC, DE >= 0 and save desired result sign
	rc
	call	mulbd		;BC * DE to HL
	rc
imul1:	mov	b,h		;divid entry point
	mov	c,l		;result to BC
	dad	h
	rc			;overflow if >= 2 ^ 15
	lda	temp
	jmp	iabs1		;return according to desired sign

;Integer division (\): <integer> x <integer> --> <integer>
idivd:	call	snfx0		;force BC >= 0, DE > 0
	jc	iover		;integer overflow if DE=0
	call	divd0		;quotient to HL, remainder to DE
	jmp	imul1		;fix sign of result and return

;MOD: <integer> x <integer> --> <integer>
;X mod Y = sign(X) * [abs(X) mod abs(Y)], so X = [(X/Y)*Y] + [X mod Y].
opmod:	mov	h,b		;save desired result sign in H
	call	snfx0		;force BC >= 0, DE > 0
	rc
	mov	a,h
	sta	temp		;desired result sign to TEMP
	call	divd0		;remainder to DE
	xchg			;result to HL
	jmp	imul1		;and return according to desired sign

;Relations: {<integer>|<floating>|<string>} x {<integer>|<floating>|<string>}
;	--> <integer>
;Relations call CMPAR to set status bits and return 0 in BC.  The status
;bits are used to return if false (0), or decrement BC if true (-1).
equal:	call	cmpar		;compare and zero BC
	rnz			;false iff zero reset
	dcx	b		;true
	ret
neq:	call	cmpar
	rz			;false iff zero set
	dcx	b
	ret
lthan:	call	cmpar
	rnc			;false iff carry reset
	dcx	b
	ret
geq:	call	cmpar
	rc			;false iff carry set
	dcx	b
	ret
leq:	call	cmpar
	jc	leq1
	rnz			;false iff carry reset and zero reset
leq1:	dcx	b
	ret
gthan:	call	cmpar
	rz			;false if zero set
	rc			;false if carry set
	dcx	b
	ret

;LOGOP is a macro to perform logical ops on B and D, and on C and E.
logop	macro	linst
	mov	a,b
	linst	d		;;apply logical instruction to B and D
	mov	b,a		;;and result to B
	mov	a,c
	linst	e		;;apply logical instruction to C and E
	mov	c,a		;;and result to C
	ret
	endm

;NOT, XOR: <integer> --> <integer>
opnot:	lxi	d,-1		;not is xor with -1
opxor:	logop	xra

;AND, RESET: <integer> x <integer> --> <integer>
	if	not wild
reset:	call	stbt0
	call	cpld1		;reset bit #(E) mod 16 of DE, then AND
	endif
opand:	logop	ana

;OR, SET: <integer> x <integer> --> <integer>
	if	not wild
setfn:	call	stbt0		;set bit #(E) mod 16 of DE, then OR
	endif
opor:	logop	ora

;JOIN: <integer> x <integer> --> <integer>
	if	not wild
join:	call	isbyt		;check (B) = 0
	mov	b,d
	call	isbyt		;check (D) = 0
	mov	b,c
	mov	c,e
	ret
	endif

	if	float and not f9511
;INT: <floating> --> {<integer> | <floating>}
int:	if	fpbcd
	lxi	h,ftemp
	call	fstor		;save arg in FTEMP in case IINT fails
	endif
	call	iint		;try to convert to integer
	mvi	a,intst		;result type = integer
	rnc			;return if successful conversion to integer
	if	fpbcd
	lxi	h,ftemp
	call	fload		;restore arg to FACC
	call	fint		;truncate arg
	dcr	e
	jnz	int1		;no digits truncated, done
	lda	facc
	ora	a
	jp	int1		;truncated and positive, done
	lxi	h,fpone
	call	fsub		;truncated and negative, subract one for result
	else			;NOT FPBCD
	call	flint		;get integer part of floating value
	mov	e,a
	endif			;end of NOT FPBCD conditional
int1:	mvi	a,sngst		;result type = floating
	ret

;IINT fixes a floating value to a two-byte integer value.
	if	not fpbcd	;IINT is same as FFIX in FPBCD version
iint:	mvi	e,16
	call	ffix		;fix floating point value
	rc			;cannot fix to integer, return Carry
	mov	c,b
	mov	b,a		;result to BC
	ret
	endif			;end of NOT FPBCD conditional

;UNS: <integer> --> <floating>
	if	fpbcd
	if	not wild
unsfn:	mov	a,b
	ora	a
	push	psw		;save arg sign
	call	fflot		;float the arg
	pop	psw
	rp			;arg was positive, done
	lxi	h,unsmax
	jmp	fadd		;else result is 65536 + arg
unsmax:	db	45H, 65H, 53H, 60H, 0, 0
	endif
	else			;NOT FPBCD
unsfn:	xra	a		;0 to A
	lxi	d,24		;0 to D, 24 to E
	jmp	fflot		;float ABCD and return

flint:	mvi	e,32
	call	ffix		;try fixing to 3-byte integer
	mvi	e,32		;scale factor to E
	jnc	fflot		;float the fixed value
	jmp	ftest		;fetch the floating value
	endif			;end of NOT FPBCD conditional
	endif

;FIRST, LAST: --> <integer>
	if	romsq and (not wild)
first:	lhld	sourc
	dcx	h		;HL points to cr preceding source
	mov	b,h
	mov	c,l		;to BC
	ret
	else
first	equ	exerr		;EX error in non-ROMSQ versions
	endif
	if	romsq		;LAST must be defined in WILD versions
last:	lxi	d,-1
	call	findl		;find line 65535
	mov	e,m
	mvi	d,0		;length to DE
	jmp	fre2		;last addr to BC
	else
last	equ	exerr
	endif

;MSBYTE, LSBYTE: <integer> --> <integer>
	if	not wild
msbyt:	mov	c,b		;move ms to ls byte
lsbyt:	mvi	b,0		;zero ms byte
	ret
	endif

;BCD: <integer> --> <integer>
	if	not wild
bcd:	xra	a
	call	cvtis		;convert integer to string, no leading char
	cpi	5
	jnc	fcer0		;length >= 5, nonfatal FC error and return 0
	lxi	h,0		;embryo value to HL
bcd1:	ldax	d		;fetch next char of string
	inx	d		;and point to next
	sui	'0'		;subtract ASCII bias
	dad	h
	dad	h
	dad	h
	dad	h		;result left one nibble
	ora	l
	mov	l,a		;and new nibble ORed in
	dcr	c		;count down length
	jnz	bcd1		;more chars in string
	mov	b,h
	mov	c,l		;result to BC
	ret
	endif			;end of NOT WILD conditional
fcer0:	call	fcern		;issue nonfatal FC error
	lxi	b,0
	mvi	a,intst		;and return integer 0 as result
	ret

;BIN: <integer> --> <integer>
	if	not wild
bin:	lxi	h,0		;embryo value to HL
	call	bin1		;sum high order byte
	mov	b,c
	call	bin1		;sum low order byte
	mov	b,h
	mov	c,l		;result to BC
	ret
bin1:	mov	a,b
	rar
	rar
	rar
	rar			;ms four bits to A3-A0
	call	bin2		;add to sum
	mov	a,b		;ls four bits to A3-A0
bin2:	ani	0FH		;mask off other bits
	cpi	0AH
	jnc	fcerf		;not a bcd digit, FC error
	mov	d,h
	mov	e,l		;copy value to DE
	endif			;end of NOT WILD conditional
;HL10A replaces (HL) with 10*(HL)+(A), and is called by GTDEC.
;Call:	DE,HL	value to multiply by 10
;	A	value to add to product
;Retn:	BC	preserved
;	DE	clobbered
;	HL	result
hl10a:	dad	h		; * 2
	dad	h		; * 4
	dad	d		; * 5
	dad	h		; * 10
	rc			;for GTDEC, never happens from BIN
	mov	e,a
	mvi	d,0		;next value to DE
	dad	d		;and added also
	ret

;GET, GET$: --> {<integer> | <string>}
get:	lda	gchar		;value for GET is in GCHAR
	mov	c,a
	xra	a
	mov	b,a		;value to BC
	sta	gchar		;clear GCHAR
	if	strng
	mvi	d,'$'
	call	gtd		;look for $
	mvi	a,intst		;return type integer
	rc			;integer GET
	mov	a,c		;else string GET$
	mvi	c,0
	ora	a		;Zero set iff no char in GET
	cnz	chrs1		;copy char to string space
	mvi	a,strst		;and return type string
	ret
	else			;NOT STRNG
	mvi	a,intst		;return type integer
	ret
	endif

;IOBYTE: --> <integer>
	if	(not camac) and (not wild)
iobyf:	lda	iobyt
	jmp	peek1		;return IOBYTE value in BC
	endif

;PEEK: <integer> --> <integer>
	if	not wild
peek:	ldax	b		;fetch the byte
	endif			;peek1 is needed for POS
peek1:	mvi	b,0		;zero the MSbyte
	mov	c,a		;return value in BC
	ret

;POS: --> <integer>
	if	not camac
pos:	lda	colum
	jmp	peek1		;return current column in BC
	endif

;TEST: <integer> x <integer> --> <integer>
	if	not wild
test:	call	stbt0		;set bit # (E) mod 16 of DE
	call	opand		;AND with (BC) -- returns (C) in A
	ora	b		;zero set iff (BC) = 0
test1:	lxi	b,0		;return 0 if zero set
	rz
	inx	b		;return 1 if zero reset
	ret
	endif

;SGN: {<integer> | <floating>} --> <integer>
sgn:	call	fetbc		;fetch arg
	iftyp	fsgn,fcerf	;branch if noninteger
	mov	a,b
	ora	c		;Zero set iff (BC)=0
	rz			;and result is 0
	mov	a,b
	ori	1		;Zero reset, minus set iff (BC) < 0
sgn1:	lxi	b,-1
	rm			;< 0, return -1
	inx	b
	rz			;= 0, return 0
	inx	b
	ret			;else > 0, return 1
	if	float and not f9511
fsgn:	call	ftest		;set status bits
	jmp	sgn1		;and return as above
	endif

;IN: <integer> --> <integer>
	if	not wild
inp:	call	readp		;read the port
	mov	c,a		;and return value read in BC
	ret
	endif

;SENSE: <integer> x <integer> --> <integer>
	if	not wild
sense:	mvi	d,7
	call	stbit		;set bit # (E) mod 8 of E
	call	readp		;read port
	ana	e
	jmp	test1		;return zero iff bit reset
	endif

;FRE, FRE$: --> <integer>
frefn:	if	strng
	mvi	d,'$'
	call	gtd		;look for $
	jc	fre		;not FRE$, just return FRE
	call	garbg		;garbage collect to compact string space
	jmp	fre1		;and continue as below
	endif
fre:	lhld	cstkp
	xchg			;address of control stack top to DE
	inx	d		;CSTKP + 1
	lhld	symta		;symbol table address to HL
fre1:	call	cplde		;- CSTKP - 1
fre2:	dad	d		;top - [bottom + 1] = available
	mov	b,h		;NB 'subtraction', so carry reset iff borrow
	mov	c,l		;result to BC
	ret

;LSHIFT: <integer> x <integer> --> <integer>
	if	not wild
lshft:	mov	a,e		;shift (BC) left by (E) mod 16 places
	ani	0FH
	mov	d,b
	mov	e,c		;BC to DE for STBI1
	call	stbi1		;shift (DE) left (A) bits
	mov	b,d
	mov	c,e
	ret
	endif

;RSHIFT, ROTATE: <integer> x <integer> --> <integer>
	if	not wild
rshft:	mvi	d,0		;shift (BC) right by (E) mod 16 places
	db	21H		;lxi h, to skip ensuing two bytes
rotat:	mvi	d,80H		;rotate / shift (BC) right by (E) mod 16 places
	mov	a,e		;entry point for rshft with (D) = 0
	ani	0FH		;mask E and clear carry
	rz			;done if zero
	mov	e,a		;(E) mod 16 to E
rota1:	mov	a,b
	jnc	rota2
	ora	d		;turn on high bit if rotating and carry
rota2:	rar
	mov	b,a		;carry now has high bit for C
	mov	a,c
	rar			;carry now has high bit for B if rotating
	mov	c,a
	dcr	e
	jnz	rota1		;keep rotating
	rnc			;done unless carry
	mov	a,b
	ora	d		;turn on high bit if rotating
	mov	b,a
	ret
	endif			;end of NOT WILD conditional

	if	realt
;TIME:  <integer> --> <integer>
;TIME (0) returns current real time clock ticks in 20ths,
;TIME (1) returns current seconds,
;TIME (2) returns current minutes, and
;TIME (3) returns current hours.
;TIME (n) for any other n gives nonfatal FC error and returns 0.
timec:	db	20, 60, 60, 24	;TIMEX offsets
time:	lxi	d,4
	call	cmbdu
	jnc	fcer0		;nonfatal FC error if arg not 0 - 3
	lxi	h,timex
	dad	b		;address desired TIMEX component
	mov	a,m		;and fetch current value
	lxi	h,timec
	dad	b		;address offset
	add	m		;current value + offset = correct current count
	mov	c,a		;result to BC
	ret
	endif

;RND: --> <integer> [if nonFLOATing version]
;RND: <floating> --> <floating> [if FLOATing version]
;RND generates the next pseudorandom 16-bit integer
;	RANDX = (RANDA * RANDX + RANDC) mod 2 ^ 16, and returns the 15-bit
;	pseudorandom integer RANDX / 2.  Cf. Knuth Ch. 3, esp. pp. 155-6.
	if	not wild
rnd:	if	float
	inx	b
	push	b		;save pointer to arg
	if	f9511
	ldax	b		;fetch first byte of arg
	ora	a		;Zero set iff arg is 0
	else
	call	ftest
	endif
	jnz	rnd1		;nonzero arg, take it for multiplier
	lxi	h,fpone		;zero arg, take 1 instead
	xthl
	endif
rnd1:	lhld	randx
	push	h
	xchg			;(RANDX) to DE
	mvi	a,randa shr 8	;ms bits of RANDA to A
	call	mult0		;ms(RANDA) * (RANDX) to HL
	mov	h,l
	mvi	l,0		;and then * 2 ^ 8
	lxi	d,randc
	dad	d		; + RANDC
	pop	d		;(RANDX) to DE again
	mvi	a,randa and 255	;ls bits of RANDA to A
	call	multy		;(RANDA) * (RANDX) + RANDC to HL
	shld	randx		;and to RANDX
	if	float
	if	f9511
	mov	a,l
	out	d9511		;low order to 9511 stack
	mov	a,h
	out	d9511		;high order to stack
	xra	a
	out	d9511
	out	d9511		;two high order 0s to stack
	mvi	a,fld95
	call	o9511		;float 32 bit integer
	in	d9511		;read sign/exponent byte
	ora	a		;check if 0
	jz	rnd2
	sui	16		;fudge exponent
	ani	7FH
rnd2:	out	d9511		;replace fudged sign and exponent
	pop	h
	call	lod95		;load the multiplier
	jmp	fmul		;multiply and return
	else			;NOT F9511
	mov	b,h
	mov	c,l		;value to BC
	if	fpbcd
	call	fflot		;float to value -32768 to 32767
	lxi	h,intmn
	call	fsub		;subtract -32768, giving 0 to 65535
	lxi	h,unsmax
	call	fdiv		;divide by 65536, giving range [0, 1)
	else			;NOT FPBCD
	lxi	d,8		;0 to D, scale factor = 8 to E
	mov	a,d		;0 to A
	call	fflot		;float to random value in range [0,1)
	endif			;end of NOT FPBCD conditional
	pop	h		;arg to HL
	jmp	fmul		;result = arg * rnd and return
	endif			;end of F9511 conditional
	else			;NOT FLOAT
	ora	a
	mov	a,h
	rar
	mov	b,a
	mov	a,l
	rar
	mov	c,a		;return RANDX / 2
	ret
	endif
	endif			;end of NOT WILD conditional


;end of FNSOPS
	page
;FPBCD 11/19/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1980 by Mark Williams Company, Chicago
;BCD floating point package


	if	float and fpbcd
;
;   FORMAT:  6 BYTES
;      BYTE 0
;           BITS 0-6:  EXCESS-64 EXPONENT
;           BIT  7:  SIGN
;      BYTES 1-5:  10-DIGIT BCD FRACTION
;   N = (-1)^S * 10^(E - 64) * 0.DDDDDDDDDD
;
BIAS	EQU	64		;EXPONENT BIAS

;
;  CONSTANT DATA (ROMABLE)
;
EMAX 	DB 	043H,014H,073H,0,0,0 ;MAX. EXP. ARGUMENT
FPMAX 	DB 	07FH,099H,099H,099H,099H,099H
INTMX 	DB 	045H,032H,076H,070H,0,0
INTMN 	DB 	0C5H,032H,076H,080H,0,0
;
; CONSTANTS FOR USE IN FUNCTIONS
ATAN1	DB 	040H		;ARCTAN(1)
	DB	078H,053H,098H,016H,033H
D12PI	DB	040H		;1/(2PI)
	DB 	015H,091H,054H,094H,031H
D1LE4	DB	040H		;1/(LOG E 4)
	DB	057H,056H,046H,027H,031H
D1L10	DB	040H		;1/LN(10)
	DB	043H,042H,094H,048H,019H
D4PI	DB	041H		;4/PI
	DB	012H,073H,023H,095H,045H
DPI2	DB	041H		;PI/2
	DB	015H,070H,079H,063H,027H
DPI4	DB	040H		;PI/4
	DB	078H,053H,098H,016H,034H
DSR22	DB	040H		;SQR(2)/2
	DB	070H,071H,6,078H,010H
HALF	DB	040H,050H,0,0,0,0
LN10	DB	041H		;LN(10)
	DB	023H,2,058H,050H,093H
LN2	DB	040H		;LN(2)
	DB	069H,031H,047H,018H,6 
PI  	DB	041H,031H,041H,059H,026H,054H
SR2M1	DB	040H		;SQR(2) - 1
	DB	041H,042H,013H,056H,020H
SR2P1  DB	041H		;SQR(2) + 1
	DB	024H,014H,021H,035H,062H
SQR10	DB	041H		;SQR(10)
	DB	031H,062H,027H,076H,060H
THIRD	DB	040H,033H,033H	;1/3
	DB	033H,033H,033H
TWO	DB	041H,020H,0
	DB	0,0,0
TWOPI	DB	041H,062H,083H	;2 PI
	DB	018H,053H,7
;
;  COEFFICIENTS FOR FUNCTION EXPANSIONS
;
;  TANGENT
TB3	DB	0C2H,015H,078H
	DB	030H,032H,084H
	DB	0C4H,014H,9
	DB	063H,024H,018H
	DB	0C2H,040H,098H
	DB	017H,8,075H
	DB	0C3H,015H,069H
	DB	020H,4,022H
	DB	0C2H,055H,020H
	DB	040H,041H,071H
	DB	0C1H,094H,038H
	DB	016H,055H,098H
TC0	DB	03FH,035H,091H
	DB	010H,014H,097H
;  COSINE
FPONE	DB	041H,010H,0
	DB	0,0,0
	DB	0C0H,030H,084H
	DB	025H,013H,075H
	DB	03FH,015H,085H
	DB	043H,044H,020H
	DB	0BDH,032H,059H
	DB	091H,089H,0
	DB	03BH,035H,090H
	DB	086H,0,0
CP05	DB	0B9H,024H,061H
	DB	0,0,0
	DB	040H,078H,053H
	DB	098H,016H,034H
	DB	0BFH,080H,074H
	DB	055H,012H,019H
	DB	03EH,024H,090H
  	DB	039H,045H,070H
	DB	0BCH,036H,057H
	DB	062H,0,0
CP14	DB	03AH,031H,033H
	DB	0,0,0
;  ARCTAN
	DB	043H,021H,060H
	DB	062H,030H,079H
	DB	043H,032H,026H
	DB      062H,7,0
	DB	043H,013H,027H
	DB	2,039H,082H
FATP3	DB	042H,012H,088H
	DB	083H,083H,3
	DB	043H,021H,060H
	DB	062H,030H,079H
	DB	043H,039H,046H
	DB	082H,083H,093H
	DB	043H,022H,010H
	DB	050H,088H,030H
FATQ3	DB	042H,038H,050H
	DB	014H,086H,051H
;  EXP
EB2	DB	042H,090H,0      
	DB 	0,0,0
	DB	042H,028H,0
	DB	0,0,0
	DB	043H,033H,0
	DB	0,0,0
	DB	0C5H,014H,058H
	DB	0,0,0
;  NATURAL LOG
LB3	DB	0C1H,013H,012H
	DB	082H,059H,017H
	DB	0C1H,033H,050H
  	DB	025H,024H,081H
	DB	0C1H,025H,084H
	DB	017H,087H,055H
	DB	0C0H,012H,087H
	DB	020H,099H,053H
	DB	0C1H,051H,002H
	DB	099H,053H,028H
	DB	0C1H,059H,041H
	DB	022H,044H,090H
LC0	DB	040H,041H,079H
	DB	059H,018H,037H
;  SQUARE ROOT
	DB	040H,014H,053H
	DB	043H,0,051H
	DB	041H,018H,046H
	DB	072H,073H,095H
	DB	0C1H,018H,078H
	DB	050H,038H,016H
SR3	DB	040H,090H,010H
	DB	015H,056H,4
;  SQUARE ROOT (X >= .25)
	DB	040H,025H,092H
	DB	080H,058H,067H
	DB	041H,010H,052H
	DB	3,026H,060H
SR6	DB	0C0H,031H,063H
	DB	024H,089H,045H
;
;  BINARY-DECIMAL CONVERSION TABLE
BDTAB	DB	0,0,1
	DB	0,0,2
	DB	0,0,4
	DB	0,0,8
	DB	0,0,016H
	DB	0,0,032H
	DB	0,0,064H
	DB	0,1,028H
	DB	0,2,056H
	DB	0,5,012H
	DB	0,010H,024H
	DB	0,020H,048H
	DB	0,040H,096H
	DB	0,081H,092H
	DB	1,063H,084H
	DB	3,027H,068H

;
;  ADDITION  (FACC) <== (FACC) + (H,L)
FADT2	LXI	H,FTMP2
FADD	MOV 	A,M
	ORA	A
	RZ	         	;OP-2 = 0
	LXI  	D,FACC
	LDAX  	D
   	ORA	A
	JZ  	FLOAD		;OP-1 = 0
FADD0	XRA	M
	PUSH   	PSW		;(SF) ==> SUBTRACT
	LDAX 	D
	ANI	080H
	STA	SIGN		;STORE SIGN OF OP-1
	MVI	A,6
	STA   	FLEN		;DEFAULT LENGTH = 6
	MOV  	A,M
	ANI	07FH		;FORM EXP-2
	STA	EXP2
	CALL	UPAC2
	LXI  	H,EXP1
   	MOV  	M,A
	INX	H
	SUB 	M		;A <== EXP1 - EXP2
	MOV 	C,A
	LXI	H,FACC+5
	SHLD	RSLT		;SUM LOCATION
	CPI	-10
	JNC	FAD1		;-10 <= DIFF < 0
	CPI	11
	JC	FAD1		;0 <= DIFF <= 10
	ORA	A
	JP	FAD9		;DONE IF DIFF > 10
;    DIFF < -10
	CALL	FZACC	  	;FACC <== 0
	LDA 	EXP2
	STA	EXP1		;EXP1 <== EXP2
FAD1	MVI     B,0
	PUSH  	B
	XCHG			;(H,L) <== OP-2
	INX  	H		;POINT TO FRACTION
	MVI	C,5
	LXI	D,ACALT+6
	CALL  	FMOVE		;ACALT+6 <== OP-2 FRACTION
	MOV 	H,D		;(H,L) = (D,E) = OP-2
	MOV	L,E
	POP	B
	MOV	A,C
	ORA	A
	JZ 	FAD8		;DIFF = 0
	JM  	FAD4		;DIFF < 0
;     DIFF > 0
	ANI	1
	JZ 	FAD2		;EVEN
;        DIFF > 0 AND ODD
	INX	D
	PUSH	B
	LXI	B,6
	CALL	FD10		;DIV. OP-2 BY 10
	LHLD	RSLT		;DESTINATION
	INX	H		;INCR. IT
	POP	B
	JMP	FAD3
FAD2	LHLD	RSLT
FAD3	MOV	A,C
	RAR
	MOV	C,A		;HALF OF DIFF
	DAD	B		;ADD BYTE SHIFT TO DEST.
        SHLD    RSLT
	LXI	H,FLEN
	ADD	M
	MOV	M,A		;TOTAL FIELD LENGTH
	JMP	FAD8
;     DIFF < 0
FAD4	LXI	H,FACC+1
	ANI	1
	JZ	FAD5		;EVEN
;     ODD AND < 0
	PUSH	B
	LXI 	B,6
	CALL	FD10		;FACC <==  FACC/10
	POP	B
FAD5	XRA	A
	MOV	B,A		;B=0
  	SUB	C		;A = -DIFF
	RAR			;A = BYTE SHIFT
	ANI	07FH
	JZ	FAD7		;NO BYTE SHIFT
	MOV	C,A		;(B,C) <== -DIFF/2
	DAD	B
	DCX	H
	PUSH	D
	CALL	FSTOR		;SHIFT FACC RIGHT
	POP	D
FAD6	DCX	H
	MOV	M,B		;CLEAR START OF FIELD
	DCR	C
	JNZ	FAD6
FAD7	LDA	EXP2
	STA	EXP1		;EXP1 <== LARGER EXP
FAD8	XCHG			;(H,L) <== OP-2
	LXI	B,4
	DAD	B
	XCHG			;(D,E) <== LOW END OP-2
	LHLD	RSLT
	INR	C		;LOOP COUNTER = 6
	INR	C
	POP	PSW		;XOR OF SIGNS
	JM	FSUB1		;SUBTRACT
	CALL	FADDL		;ADD LOOP
	CALL 	FRNRM
	JMP	FRND		;ROUND
FAD9	POP	PSW
	JMP	FRND0
;  ADD LOOP
FADDL	XRA	A		;CLEAR CARRY
FADL1	LDAX	D
	ADC	M
	DAA
	MOV	M,A
	DCX	D
	DCX	H
	DCR	C
	JNZ	FADL1
	RNC			;DONE IF NO CARRY
FADL0   MOV	A,M		;CARRY 1
	ADI	1
	DAA
	MOV	M,A
	DCX	H
	JC	FADL0
	RET
;
;  ABSOLUTE VALUE
FABS	LXI	H,FACC
	MOV	A,M
	ANI	07FH
	MOV	M,A
	RET
;
;  ARCTAN
;  IF ABS(X) > 10 E 10 THEN FATAN(X) = SGN(X) * PI/2
;  IF ABS(X) <= 5 E -4 THEN FATAN(X) = X - X^3/3
;  ELSE  ABS(X) <= SQR(2) -1 :   X0=0, Y=X
;        ABS(X) > SQR(2) + 1 :   X0=INFINITY, Y = -1/X
;        SQR(2) - 1 < ABS(X) < SQR(2)+1:  X0=1, Y=(X-1)/(X+1)
;  FATAN(X) = FATAN(X0) + FATAN(Y) WHERE
;     FATAN(Y) = Y * P(Y^2)/Q(Y^2)
FATAN	LXI	H,FACC
	MOV	A,M
	ANI 	080H
	STA	SIGN1		;SAVE SIGN 
	MOV	A,M
	ANI	07FH
	MOV	M,A		;X <== ABS(X)
 	CPI	BIAS+11
	JNC	FAT97		;EXP > 10
	CPI	BIAS-4
	JC	FAT98		;EXP < -4
	JNZ     FAT0		;EXP > -4
	INX	H		;EXP = -4
	MOV	A,M
 	DCX	H
	CPI	6
	JC	FAT98		;Z <= 5 E (-4)
;     5 E (-4) < X < E 10
FAT0    LXI	H,SR2M1	;SQR(2) - 1
	CALL	FCMP
	JNC	FAT1		;X > SQR(2) -1
;     X <= SQR(2) -1
	LXI 	H,FTMP2
	MVI	C,6
	CALL 	FZERO		;X0 = 0 = ARCTAN(0)
	JMP	FAT4
FAT1    LXI     H,SR2P1
	CALL	FCMP
	JC	FAT2
;     X > SQR(2) + 1
        LXI	H,DPI2
        LXI	D,FTMP2
	CALL	FLOD1		;X0 = PI/2 = ARCTAN(INFINITY)
	CALL	FLINV		;FACC <== 1/X
	CALL	FCHS		;FACC <== -1/X
	JMP	FAT4
;     SQR(2) - 1 < X < SQR(2) + 1
FAT2	LXI	H,ATAN1
	LXI	D,FTMP2
	CALL	FLOD1		;X0 = ARCTAN(1)
	LXI	H,FPONE
	CALL	FADD		;FACC <== X+1
	CALL	FSTT1		;SAVE IN FTMP1
	LXI	H,TWO
	CALL	FSUB		;FACC <== X-1
	LXI	H,FTMP1
	CALL	FDIV		;FACC <== (X-1)/(X+1)
FAT4	CALL	FSTT3		;FTMP3 <== Y
	CALL	FCO35		;FTMP1 <== Y^2
;     RATIONAL FUNCTION
;        DENOMINATOR
	LXI     H,FATQ3 
	MVI 	C,3  
	CALL	FPLY0
	CALL	FSTT4		;STORE DENOM. IN FTMP4
;        NUMERATOR
	LXI	H,FTMP1
	CALL	FLOAD
	LXI	H,FATP3
	MVI	C,3
	CALL	FPOLY
	LXI	H,FTMP3
	CALL	FMUL		; *Y
	LXI	H,FTMP4
	CALL	FDIV		;P/Q
	CALL	FADT2		;+ARCTAN(X0)
FAT6	LXI	H,FACC
	LDA	SIGN1
	ORA	M		;APPEND SIGN
	MOV	M,A
	RET
;     IF X > 1 E 10 THEN FATAN(X) = SGN(X) * PI/2
FAT97	LXI	H,DPI2
	CALL	FLOAD
	JMP	FAT6
;     IF X < 5 E (-4) THEN FATAN(X) = X - X^3/3
FAT98	CALL	FSTT1		;STORE IN FTMP1
	CALL	FMUL		;SQUARE X
        LXI     H,FTMP1
	CALL	FMUL		;CUBE X
	LXI	H,THIRD
	CALL	FMUL
	LXI	H,FTMP1
	CALL	FSUB
	JMP	FAT6
;
;  FLOATING POINT A TO X POWER
FATOX	XCHG
	INX	H
	LDA	FACC
	ORA	A
	JZ	FATX1		;0 TO POWER
	JM	FATX2		;NEG TO POWER
	PUSH	H
FATX0	CALL	FLN
	POP	H
	CALL	FMUL
	JMP	FEXP		;A^X = EXP (X * LN (A))
;     0 TO POWER
FATX1	ORA	M
	JM	FOVER		;0 TO NEGATIVE
	RET
;     NEGATIVE TO POWER
FATX2	PUSH	H
	CALL	FSTT1		;SAVE A IN FTMP1
	POP	H
	PUSH	H
	CALL	FLOAD		;LOAD POWER
	CALL	FSTT2		;AND SAVE IN FTMP2
	CALL	FFIX		;GET INTEGER PART OF POWER
	POP	H
	PUSH	B		;AND SAVE INTEGER PART
	PUSH	H
	CALL	FCMP
	POP	H
	JNZ	FATX4		;NEGATIVE A TO NONINTEGER X
	CALL	FLONE		;LOAD 1 = A^0
	POP	D		;INTEGER POWER TO DE
	LDA	FTMP2
	ORA	A
	RZ			;DONE IF POWER IS 0
	CM	CPLDE		;COMPLEMENT POWER IF NEGATIVE
	MOV	A,D
	ORA	A
	JNZ	FOVER		;POWER TOO HIGH
FATX3	LXI	H,FTMP1
	PUSH 	D
	CALL	FMUL
	POP	D
	DCR	E
   	JNZ	FATX3
	LDA	FTMP2
	ORA	A
	RP			;POSITIVE POWER, DONE
;FACC <== 1/FACC
FLINV	CALL	FSTT1		;STORE FACC IN FTMP1
	CALL	FLONE		;LOAD 1
	LXI	H,FTMP1
	JMP	FDIV		;INVERT
FATX4	XTHL			;POP SAVED POWER, PUSH POWER ADDRESS
	CALL	FCERN		;NONFATAL FC ERROR
	LXI	H,FTMP1
	CALL	FLOAD		;RELOAD A
	CALL	FCHS		;AND FORCE POSITIVE
	JMP	FATX0		;RETURN -A ^ X AS RESULT
;
;  CHANGE SIGN
FCHS	LXI	H,FACC
	MOV	A,M
	XRI	080H
	MOV	M,A
	RET
;
;  COMPARE FACC WITH (H,L)
;     EXIT:  (CF) ==> FACC <= (H,L); (ZF) ==> EQUAL; ELSE >
FCMP	LXI	D,FACC
	LDAX	D
	ORA	A
	JP	FCM0		;FACC >= 0
	XRA	M
	STC
	RM			;SIGNS DIFFER
	XCHG			;BOTH NEGATIVE
	JMP	FCM1
FCM0	ORA	M
	RM			;(H,L) <0
FCM1	MVI	C,6
FCM2	LDAX	D
	SUB	M
	RC
	RNZ
	INX	H
	INX	D
	DCR	C
	JNZ	FCM2
	STC
	RET
;
;  COSINE
;  X <== ABS (X) SINCE COS(-X) = COS(X)
;  X > 10 E 10 NOT PERMITTED
;  IF X > 2 * PI THEN X <== X MOD 2*PI
;  IF X > PI THEN X <== 2*PI - X
;  IF X > PI/2 THEN X <== PI -X AND SIGN FLAG IS SET
;       (I.E., COS(X) = - COS (PI - X) )
;  IF X <= PI/4 THEN Y = 4X/PI, COS(X) = POLYNOMIAL(Y^2)
;  IF PI/4 < X <= PI/2 THEN X1 = PI/2 - X, Y = 4*X1/PI,
;     COS(X) = SIN(X1) = Y * POLYNOMIAL(Y^2)
FCOS	XRA	A
	STA	SIGN1		;SET POSITIVE
	CALL	FABS		;COS(X) = COS(-X)
	CPI	BIAS+11
	JNC	FCERN		;EXP >= 10 NOT VALID
 	LXI	H,TWOPI
	CALL	FCMP
	JC	FCO1		;X <= 2 PI
;     IF X > 2 PI  THEN X <== X (MOD 2 PI)
	CALL	FSTT1		;STORE IN FTMP1
	LXI	H,D12PI
	CALL	FMUL		;DIV BY 2 PI
	CALL	FINT		;INTEGER PART
	LXI	H,TWOPI
	CALL	FMUL
	LXI	H,FTMP1
	CALL	FSUB		;MINUS X
	CALL	FCHS
FCO1	LXI	H,PI
	CALL	FCMP
	JC 	FCO2
;     IF X > PI THEN X <== 2 PI - X
	CALL	FCHS
	LXI 	H,TWOPI
	CALL	FADD
FCO2	LXI	H,DPI2
	CALL	FCMP
	JC	FCO3
;     IF X > PI/2 THEN X <== PI - X AND SIGN = NEGATIVE
	CALL 	FCHS
	LXI	H,PI
	CALL	FADD
	LXI	H,SIGN1
	MOV	A,M
	XRI	080H
	MOV	M,A
FCO3	LXI	H,DPI4
	CALL	FCMP
	JC	FCO5		;X <= PI/4
;   PI/4 < X < PI/2
;     X0 = PI/2 - X; Y = (X0 * 4/PI)^2
	CALL	FCHS
	LXI	H,DPI2
	CALL	FADD		;PI/2 -X
	CALL	FCO34
	CALL	FCO35
;    EVAL. POLYNOMIAL
   	LXI	H,CP14
	MVI	C,4
	CALL	FPOLY
	LXI	H,FTMP2		;Y
	CALL	FMUL
FCO4	LXI	H,FACC
	LDA	SIGN1
	XRA	M		;SET SIGN
	MOV	M,A
	RET
;     X <= PI/4
;       Y = (4X/PI)^2
FCO5	LXI	H,D4PI
	CALL	FMUL
	CALL	FCO35
;     EVAL POLYNOMIAL
	LXI	H,CP05
	MVI	C,5
	CALL	FPOLY
	JMP	FCO4
;     DIVIDE VARIABLE BY PI/4
FCO34	LXI	H,D4PI
FC34A	CALL	FMUL
	JMP	FSTT2		;FTMP2 = 4X/PI
;     SQUARE FACC AND STORE AT FTMP1
FCO35	CALL	FSTT1
	CALL	FMUL
	JMP	FSTT1
;
;  FLOATING POINT DIVISION
;     FACC <== FACC / (H,L)
FDIVD	XCHG
	INX	H
FDIV	MOV	A,M
	ORA	A
	JZ      FOVER		;DIV BY 0
	CALL	UPAC1
	ADI	BIAS+1
	SUB	B		;GET RESULT EXPONENT
	JC	FZACC		;UNDERFLOW
	JZ	FZACC
	JM	FOVER		;OVERFLOW
	STA	EXP1
	LXI	H,FTEMP
	LXI	B,5
	MOV	M,B		;FTEMP 1ST BYTE = 0
	INX 	H
	INX	D
	XCHG
	CALL	FMOVE		;FTEMP <== DIVISOR FRACTION
	LXI	H,FACC+1
	LXI	D,ACALT+2
	MVI	C,5
FD0	MOV	A,M		;ACALT+2 <== DIVIDEND FRACTION
	MOV	M,B		;CLEAR FACC TO 0
	STAX	D
	INX	H
	INX	D
	DCR	C
	JNZ     FD0
	PUSH	H		;SAVE ACALT LOCATION
	MVI	B,11		;NO. OF QUOTIENT DIGITS
;     DIVIDE LOOP
FD1	LXI	H,FTEMP
	LXI	D,ACALT+1	;QUOTIENT LOCATION
	MVI	C,6
FD2	LDAX	D
	CMP	M
	JC	FD5		;DIVIDEND < DIVISOR
	JNZ 	FD3		;DIVISOR < DIVIDEND
	INX	D
	INX	H
	DCR	C
	JNZ 	FD2
FD3	MVI	C,6		;DIVIDEND >= DIVISOR
	LXI	H,FTEMP+5
	LXI	D,ACALT+6
	STC
;        SUBTRACT LOOP
FD4	MVI	A,099H
	ACI	0
	SUB	M
	XCHG
	ADD	M
	DAA
	MOV	M,A
	XCHG
	DCX	H
	DCX	D
	DCR	C
	JNZ     FD4
	POP	H
	PUSH	H
	INR	M		;QUOTIENT DIGIT
	JMP	FD1
FD5	LXI	H,FACC+1
	DCR	B		                        
    	JNZ     FD6             ; NEXT DIGIT
	MOV	A,M		;END OF DIVISION LOOP
	ORA	A
	JNZ     FD7		;DONE
	MVI	B,1		;LEADING 0 - DO ONE MORE DIGIT
	LXI	H,EXP1
	DCR	M		;ADJUST EXP
	LXI	H,FACC+1
FD6	MVI	C,12
	CALL	FM10		;SHIFT RESULT INTO FACC
	JMP	FD1
FD7	MVI	C,6
	CALL	FM10		;SHIFT RESULT INTO PLACE
	POP	H		;CLEAR STACK
	JMP	FRND
;  DIVIDE FLOATING POINT FIELD AT (H,L) BY 10
;     ENTRY: (B,C) = NO OF BYTES
;     EXIT;  (H,L) = START OF FIELD
FD10	DAD	B	;(H,L) = LOW ORDER END
FD11	DCX	H
	MOV	A,M
	RRC
	RRC
	RRC
	RRC
	ANI	0FH
	MOV	B,A		;STORE ONE DIGIT
	DCX	H
	MOV	A,M
	RLC	
	RLC
	RLC
	RLC
	ANI	0F0H
	ORA	B		;APPEND DIGIT
	INX	H
	MOV	M,A
	DCR	C
	JNZ	FD11
	MOV	M,B		;MAKE FIRST DIGIT 0
	RET
;
;  EXP FUNCTION
;  IF X = 0 THEN EXP(X) = 1
;  IF X < 0 THEN EXP(X) = 1 / EXP(-X)
;  ELSE   Z = INT (.5 + X/ LN(10))
;         Y = X / LN(10) - Z
;         B = Y / (4 LOG10 E)     I.E., 0 <= B < .3
;         A = 2 + B^2 * (P1/Q1)
;         EXP(B) = (A + B) / (A - B)
;         EXP(X) = 10^Z * (EXP(B))^4
FEXP	LXI	H,EMAX
	CALL	FCMP
	JNC	FEX31		;> MAX
	XRA	A
	STA	EXP4		;CLEAR INVERT FLAG AND SIGN
	STA	SIGN1
	LXI	H,FACC
	ORA	M
	JZ	FLONE		;EXP(0) = 1
	JP	FEXP1
	STA	EXP4		;FLAG NEGATIVE ARGUMENT
	ANI 	07FH
	MOV	M,A		;MAKE ABS VALUE
FEXP1	LXI	H,D1L10
	CALL	FMUL		;T = X / LN(10)
	CALL	FSTT2		;FTMP2 <== T
	LXI	H,HALF
	CALL	FADD
	CALL	FFIX		;A = INT(.5 + T)
	PUSH	B		;SAVE INT PART
	CALL	FCHS
	CALL	FADT2		;FRAC PART
	LXI	H,D1LE4		;DIVIDE BY 4 LOG E
	CALL	FC34A		;FTMP2 <== Y
	CALL	FCO35		;FTMP1 = T = Y^2
;     EVALUATE CONTINUED FRACTION
   	LXI	H,EB2
	CALL	FTAN6
	LXI	H,FTMP4
	CALL	FLOAD		;LOAD P1
	LXI	H,FTMP3
	CALL	FDIV		;P1/Q1
	LXI	H,FTMP1
	CALL	FMUL		; *T
	LXI	H,TWO
	CALL	FADD		; +2
	CALL	FSTT3		;FTMP3 = A = 2 + T(P1/Q1)
 	LXI	H,FTMP2		;Y
	CALL	FSUB
	CALL	FSTT4		;FTMP4 = A - Y
	LXI	H,FTMP3
	CALL	FLOAD		;Y
	LXI	H,FTMP2
	CALL	FADD		;FACC = A + Y
	LXI	H,FTMP4
	CALL	FDIV 		; (A+Y) / (A - Y)
	CALL  	FCO35		;SQUARE
	CALL	FCO35		;4TH POWER
	CALL	FLONE		;LOAD 1
	POP	B		;GET POWER OF 10
	INR	C
	MOV	A,C
	ADI	BIAS
	STA	FACC		;FACC = 10 TO POWER
	LXI	H,FTMP1
	CALL	FMUL		;MULT BY E TO POWER
	JMP	FTAN5		;SEE WHETHER TO INVERT
FEX31	CALL	FCERN		; > MAX
	LXI	H,FPMAX
	JMP	FLOAD
;  CONVERT FACC TO 16-BIT BINARY INTEGER IN (B,C)
FFIX	LXI	H,INTMX
IINT	EQU	FFIX		;IINT IS SAME AS FFIX IF FPBCD
	CALL	FCMP
	JNC	FFIX1		;OVERFLOW
	LXI	H,INTMN
	CALL	FCMP
	JZ	FF0		;-32768
	JC	FFIX2		;UNDERFLOW
;     RANGE OK
FF0	LXI	H,FACC
	MOV	A,M
	ORA	A
	JNZ	FF1
	LXI	B,0		;ZERO VALUE
	RET
FF1	PUSH	PSW		;SAVE SIGN
	CALL	FINT
	MOV	A,C
	ORA	A
	JNZ	FF3		;NONZERO INTEGER PART
;     INTEGER PART 0
	POP	PSW
	RP			;RETURN IF POSITIVE
	DCX	B		;NEGATIVE FRACTION
	JMP	FF6
FF3	POP	PSW		;REMOVE SAVED SIGN
	MOV	D,C		;D = NO. OF INTEGER DIGITS
	LXI	H,0		;(H,L) = BINARY ACCUMULATOR
	LXI	B,FACC+1	;(B,C) = LOC OF FRACTION
FF4	LDAX	B		;CONVERSION LOOP
	RAR
	RAR
	RAR
	RAR
	CALL	FF7		;LEFT DIGIT
	JZ 	FF5		;END OF INTEGER DIGITS
	LDAX	B
	CALL	FF7		;RIGHT DIGIT
	INX	B		;NEXT BYTE
	JNZ	FF4		;LOOP
FF5	MOV	B,H
	MOV	C,L		;(B,C) <== VALUE
	LDA	FACC
	ORA	A
	RP			;NOT NEGATIVE
	DAD	D		;IF NEGATIVE ROUND DOWN
	MOV	B,H
	MOV	C,L
	XRA	A		;2'S COMPLEMENT
	SUB	C
	MOV	C,A
	MVI	A,0
	SBB	B
	MOV	B,A
FF6	PUSH	B
	CALL	FFLOT		;FACC <== INTEGER PART
	POP	B		;VALUE
	XRA	A		;CLEAR CARRY
	RET
FF7	PUSH	D		;ADD CURRENT DIGIT TO BINARY TOTAL
	DAD	H		;(H,L) <== (H,L) * 10
	MOV	E,L
	MOV	D,H
	DAD	H
	DAD	H
	DAD	D
	ANI	0FH		;GET DIGIT
	MOV	E,A
	MVI	D,0
	DAD	D		;ADD NEW DIGIT
	POP	D
	DCR	D
	RET
FFIX1	LXI	B,32767		;OVERFLOW
	JMP	FF8
FFIX2	LXI	B,-32768	;UNDERFLOW
FF8	CALL	FF6 
	STC
	RET
;
;  FLOAT BINARY VALUE IN (B,C)
;     RESULT IN FACC
FFLOT	PUSH	B
	MVI	C,11
	LXI	H,FACC
	CALL	FZERO
	POP	B
	STA	SIGN		;POSITIVE
	LXI	D,BDTAB-3	;CONVERSION TABLE
	ORA	B
	JM	FFL0  		;VALUE <0
	JNZ	FFL1		;>0
	CMP	C
	RZ
	JMP	FFL1
FFL0	ANI	080H
	STA	SIGN		;NEGATIVE
	XRA	A		;NEGATE (B,C)
	SUB	C
	MOV	C,A
	MVI	A,0
	SBB	B
 	MOV	B,A
;     SHIFT (B,C) RIGHT
FFL1	MOV	A,B
	ORA	A		;TEST FOR 0 AND CLEAR CARRY
	JNZ     FFL2		;B IS NOT 0
        MOV	A,C             ;B=0
	ORA	A
	JNZ	FFL3
;     DONE WHEN (B,C) = 0
	LXI	H,EXP1
	MVI	M,BIAS+6
	JMP	FNORM		;NORMALIZE
FFL2	RAR			;B <== B/10
	MOV	B,A
	MOV	A,C
FFL3	RAR			;C <== C/10
	MOV	C,A 
	INX	D		;NEXT PLACE IN TABLE
	INX	D
	INX	D
	JNC	FFL1		;0-BIT
;     ON 1-BIT ADD VALUE FROM TABLE
	PUSH	D
	PUSH	B
	INX	D
	INX	D
	LXI	H,FACC+3
	MVI	C,3
	CALL	FADDL		;ADD DECIMAL VALUE
	POP	B
	POP	D
	JMP	FFL1
;
;  INPUT FLOATING POINT VALUE FROM STRING
;     RESULT IN FACC
FINP	SHLD	TEXTP		;SAVE STRING POINTER
	CALL	FINP0
	LHLD	TEXTP
	JMP	BAKUP		;BACK UP THE TEXT POINTER
FINP0	LXI	H,FACC
	MVI	C,12
	CALL	FZERO
	MOV	B,A		;B = 0 = EXPONENT
	MOV	E,A		;E = 0 = SIG DIGIT FLAG
	STA	SIGN
	INR	A
	MOV	D,A		;D = DEC. POINT FLAG = 1
FIN00	CALL	FINC		;GET CHARACTER
	JC	FIN1B		;NON NUMERIC
FIN0	CPI 	'0' 
	JNZ     FIN2		;NONZERO DIGIT
	JMP	FIN00		;SKIP A LEADING 0
FIN1	CALL    FINC
FIN1A	JNC	FIN2		;DIGIT
FIN1B	CPI	'9'+1
	JNC	FIN3		;>9
	CPI	'.'
	JNZ	FIN7		; END OF STRING
	XRA	A
	CMP	D
	JZ	FIN7		; 2 DEC. POINTS
	MOV	D,A		;FLAG FOR DEC. POINT
	JMP	FIN1
;     PROCESS DIGIT
FIN2	SUI	'0'		;MAKE NUMERIC
	MOV	C,A
	LDA	FACC+2
 	ANI	0F0H
	JNZ     FIN22		;10 DIGITS ALREADY IN
	PUSH	B
	LXI	H,FACC+1
	MVI	C,6
	CALL	FM10		;FACC <== FACC * 10
	POP	B
	MOV	A,C		;NEW DIGIT
	ADD	M		;ADD IN NEW DIGIT
	MOV	M,A
	MOV	A,C		;RECOVER NEW DIGIT
	ORA	E
	MOV	E,A		;SIG FLAG NONZERO IF SIG DIGIT SEEN
	MOV	A,D		;FETCH DEC PT FLAG
	ORA	A
	JZ	FIN21		;DEC PT SEEN
	INR	B		;DEC PT NOT SEEN YET, BUMP EXPONENT
FIN21	MOV	A,E		;FETCH SIG DIGIT FLAG
	ORA	A
	JNZ	FIN1		;TRY NEXT DIGIT
	DCR	B		;PAST DEC PT AND NO SIG DIGIT, DCR EXPONENT
	JMP	FIN1
;MORE THAN 10 SIGNIFICANT DIGITS
FIN22	MOV	A,D		;FETCH DECIMAL POINT FLAG
	DCR	A
	JNZ	FIN23		;PAST DECIMAL POINT, LEAVE EXPONENT UNCHANGED
	INR	B		;BUMP EXPONENT FOR DIGIT
FIN23	CALL	FINC		;FETCH ANOTHER CHAR
	JC	FIN1B		;NONDIGIT
	JMP	FIN22		;ANOTHER DIGIT
FIN3	CPI	'E'
	JZ	FIN4
	CPI	'E'+20H		;LOWER CASE E
        JNZ     FIN7		;NOT VALID CHARACTER
;     PROCESS EXPONENT
FIN4	MVI	C,0		;EXP. DIGIT COUNT
	MOV	E,C		;BUILD EXPONENT IN E
	CALL	FINC		;GET CHAR
	JNC	FIN4C		;DIGIT
	CPI	MINT
	JZ	FIN4A		;TOKENIZED MINUS SIGN
	CPI	PLUST
	JZ	FIN4B		;TOKENIZED PLUS SIGN
	CPI	'+'
	JZ	FIN4B		;PLUS SIGN
	CPI	'-'
	JNZ     SNERR		;NOT DIGIT, PLUS, MINUS -- SYNTAX ERROR
FIN4A	MVI	A,080H		;MINUS
	STA	SIGN
FIN4B	CALL	FINC
	JC	SNERR		;SYNTAX ERROR IF NONDIGIT FOLLOWS SIGN
FIN4C	CPI	'0'
	JNZ	FIN5
	CALL	FINC		;GET CHAR AFTER LEADING EXPONENT 0
	JC	FIN6		;EXPONENT IS 0
	JMP	FIN4C		;CHECK FOR ANOTHER LEADING 0
FIN5	INR	C
	SUI	'0'
	ADD	E		;ADD TO EXPONENT
	MOV 	E,A
	CALL	FINC
	JC      FIN6		;END
	PUSH	PSW		;ANOTHER EXP. DIGIT
	MOV	A,C
	CPI	3
	JZ	FIN5A		;TOO MANY DIGITS
	MOV	A,E		;E <== E * 10
	ADD	A
	MOV	E,A
	ADD	A
	ADD     A
	ADD	E
	MOV	E,A
	POP	PSW
	JMP	FIN5
FIN5A	POP	PSW
FIN6	LDA	SIGN
	ORA	A
	JZ	FIN7A		;EXP >= 0
	MOV	A,B		;NEGATE EXPONENT
	SUB	E
	ADI	BIAS
	JM	FZACC		;UNDERFLOW
	JZ 	FZACC
	JMP	FIN8
FIN7	MVI	E,0
FIN7A	MOV	A,B		;FORM TOTAL EXPONENT
	ADD	E
	ADI	BIAS
	JM	FOVER		;OVERFLOW
FIN8	STA	EXP1
	LXI	H,FACC
	MVI	C,6
FIN9	INX	H		;FIND NONZERO BYTE
	MOV	A,M
	ORA	A
	JNZ	FIN10
	DCR	C
	RZ			;ZERO RESULT
	JMP	FIN9
FIN10	LXI	D,FACC+1	;NORMALIZE
	CALL	FLOD1		;MOVE OVER TO START OF FACC
	LDAX	D
	CPI	010H
	JNC	FIN11
	LXI	H,FACC+1
	MVI	C,6
	CALL    FM10            ;HALF BYTE SHIFT
FIN11	LXI	H,EXP1
	LDA	TEMP		;SIGN
	ANI	080H
	ORA	M		;APPEND EXP.
	STA	FACC
	XRA	A
	RET
;     GET CHARACTER FROM STRING
;     CF = 1 ==> NOT NUMERIC
FINC	LHLD	TEXTP
	MOV	A,M
	INX	H
	SHLD	TEXTP		;NEW TEXT POINTER
	CPI	'0'
	RC			;<0
	CPI	'9'+1
	CMC
	RET
;
;  FACC <== INTEGER PART (TRUNCATED)
;  EXIT:  (D,E) = 1 IFF NONZERO DIGITS WERE TRUNCATED
;         (B,C) = NO. OF INTEGER DIGITS
FINT	XRA	A
	MOV	E,A
	MOV	D,A
	MOV	B,A
	LXI	H,FACC
	ORA	M
	RZ			;ZERO
	MVI	E,1		;FRACTION FLAG
	ANI	07FH		;GET EXPONENT
	SUI	BIAS
	JZ	FZACC		;PURE FRACTION
	JM	FZACC
	MVI	E,0
	CPI	10
	RNC			;ALREADY INTEGER
	PUSH	PSW		;SAVE NO. OF DIGITS
	ORA	A
	RAR
	PUSH    PSW		;(CF) = 1 IF ODD
	MOV	C,A
	DAD	B
	INX	H		;(H,L)= LOW END OF INTEGER PART
	MVI	A,5
	SUB	C
	MOV	C,A		;C = NO OF BYTES TO CLEAR
	POP	PSW
	JNC	FINT2		;EVEN
	MOV	A,M		;CLEAR RIGHT DIGIT OF BYTE
	ANI  	0FH
	JZ	FINT3		;NO FRACTION DIGIT
	MVI	E,1
	MOV	A,M
	ANI	0F0H
	MOV	M,A
	JMP     FINT3
FINT2	MOV	A,M
	ORA	A
	JZ	FINT3		;ALREADY 0
	MVI	E,1		;FLAG NONZERO FRACTION
	MVI	M,0		;CLEAR BYTE
FINT3	INX	H
	DCR	C
	JNZ	FINT2
	POP	PSW		;NO. OF DIGITS
	MOV	C,A
	RET
;
;  NATURAL LOG
; X = 10^A * Y WHERE .1 <= Y < 1
; Y = 2^M * Z WHERE 1/2 <= Z < 1, M=0,-1,-2,-3
; V = T * T
; LN (V) = LN (1+T)/(1-T) = T * P2(V) / Q2(V)
; LN (X) = A * LN (10) + (M - 1/2) * LN (2) + LN (1+T)/(1-T)
FLN	LXI	H,FPONE
	CALL	FCMP
	JZ	FZACC		;LN(1) = 0
	LXI	H,FACC
	MOV	A,M
	ORA	A
	JM	FL94		;LOG OF NEGATIVE
	JZ	FCERN		;LOG OF 0
	MVI	M,BIAS		;MAKE PURE FRACTION
	SUI	BIAS
	STA     EXP3		;STORE UNBIASED EXP
	MVI	C,0
FL0	INX	H
	MOV	A,M
	CPI     050H
	JNC	FL1		;FRAC >= .5
	PUSH	B
	DCX	H
	PUSH	H
	LXI	H,FTEMP
	CALL	FSTOR
	CALL	FADD		;X <== 2*X
	POP	H
	POP	B
	INR	C		;COUNT
	JMP	FL0
FL1	LXI	H,EXP4
	MOV	M,C		;SAVE SCALE FACTOR
	CALL	FSTT1	 	;FTMP1 = SCALED FRACTION
	LXI	H,DSR22	
	CALL	FADD		;ADD SQR(2)/2
	CALL	FSTT2
	LXI	H,FTMP1
	CALL	FLOAD
	LXI	H,DSR22
	CALL	FSUB
	LXI	H,FTMP2
	CALL	FDIV		;T = (Y - SQR(2)/2) /(Y + ...)
	CALL	FSTT2		;FTMP2 = T
	CALL	FCO35		;FTMP1 = T^2
;     LOG ((1+T)/(1-T)) = CONTINUED FRACTION
	LXI	H,LB3
	CALL	FTAN7
	LXI	H,LC0
	CALL	FADD
	LXI	H,FTMP2
	CALL	FMUL		;T * P/Q
	CALL	FSTT1
	LDA	EXP3
	MOV	C,A
	MVI	B,0		;(B,C) = EXP
	ORA	A
	JP	FL2
	DCR	B		;SIGN EXTEND
FL2	CALL	FFLOT
	LXI	H,LN10
	CALL	FMUL		;FACC <== EXP * LN(10)
	CALL	FSTT2
	LDA	EXP4		;M
	MOV	C,A
	MVI	B,0
	CALL	FFLOT
	LXI	H,HALF
	CALL	FADD
	LXI	H,LN2
	CALL	FMUL		;FACC <== -(M - 1/2) * LN(2)
	CALL	FCHS
	CALL	FADT2
	LXI	H,FTMP1
	JMP 	FADD
FL94	ANI	07FH		;LOG OF NEGATIVE
	MOV	M,A		;MAKE POSTIVE
	CALL    FCERN
	JMP   	FLN
;
;  LOAD FACC FROM (H,L)
;     EXIT:  (D,E) = FACC
FLONE	LXI	H,FPONE		;LOAD 1
FLOAD	LXI	D,FACC
FLOD1	MVI	C,6
;     MOVE FIELD FROM (H,L) TO (D,E)
FMOVE	PUSH 	D
FMOV1 	MOV	A,M
	STAX	D
	INX	H
	INX	D
	DCR	C
	JNZ	FMOV1
	POP	D
	RET
;
;  F.P . MULTIPLY   FACC <== FACC * (H,L)
FMUL	MOV 	A,M
	ORA	A
	JZ      FZACC     	;OP-2 = 0
	CALL	UPAC1
	ADD	B	 	;GET RESULT EXP
	SUI 	BIAS-1
	JZ	FZACC		;UNDERFLOW
	STA     EXP1		;STORE EXP
	XCHG
	LXI	D,FTEMP
        MVI     C,6
	CALL	FMOVE		;MOVE OP-2 TO FTEMP
	XCHG
	MVI	M,0
	LXI	B,5
	DAD	B		;(H,L) = LOW END OF OP-2
	PUSH	H
	LXI	H,HOLD4
	CALL	FSTOR		;HOLD4 <== OP-1
	DCX	H		;(H,L) = END OF HOLD3
	MVI	B,3
	XCHG
FM2	LXI	H,6
	DAD	D		;(H,L) <== END OF HOLD(I+1)
	MVI	C,6
	ORA	A		;CLEAR CARRY
FM3	MOV	A,M		;HOLD(I) <== 2 * HOLD(I+1)
	ADC	A		;DOUBLE IT
	DAA
	STAX	D
	DCX	D
	DCX	H
	DCR	C
	JNZ	FM3
	DCR	B
	JNZ	FM2
	CALL	FZACC		;CLEAR FACC
	POP	H		;(H,L) <== OP-2
	PUSH	H
	CALL	FMLOP	 	;MULT LOOP FOR RIGHT DIGITS
	LXI	H,FACC+1
	MVI	C,10
	CALL	FD10		;FACC <== FACC/10
	LXI	H,FTEMP+1
	MVI	C,5
	CALL    FD10
	POP	H
	CALL	FMLOP		;MULT LOOP FOR LEFT DIGITS
	JMP	FNORM		;NORMALIZE
;   MULTIPLY LOOP
;     FACC <== HOLD1 * RIGHT DIGITS OF FTEMP
FMLOP	MVI	C,5		;OUTER LOOP COUNTER
	LXI	D,FACC+11	;PRODUCT LOCATION
	PUSH	D
	PUSH	H		;END OF MULTIPLIER
FML1	POP	D
	LDAX	D		;GET MULTIPLIER DIGIT
	DCX	D		;NEXT DIGIT
	POP	H		;SUM LOCATION
	DCX	H
	PUSH	H		;NEXT SUM LOCATION
	PUSH	D		;NEXT MPR. DIGIT
	LXI	D,HOLD1+5
	ANI	0FH		;DIGIT
	RLC			;PUT IN LEFT NIBBLE
	RLC
	RLC
	RLC
FML2	ORA	A
	JZ	FML4		;SKIP LOOP ON ZERO DIGIT
	PUSH	H		;ACCUM LOCATION
	ADD	A		;DOUBLE DIGIT
	MOV	B,A
	JNC	FML3		;NO ADD ON 0 BIT
	PUSH	B
	PUSH	D
	MVI	C,6
	CALL	FADDL		;ACCUMULATE PRODUCT
	POP	D
	POP	B
FML3	LXI	H,6
	DAD	D		;NEXT HOLD BUFFER
	XCHG
	POP	H		;ACCUM. LOCATION 
	MOV	A,B		;GET DIGIT
	JMP	FML2		;INNER LOOP
FML4	DCR	C
	JNZ	FML1		;OUTER LOOP (NEXT DIGIT)
	POP	H
	POP	H		;CLEAR STACK
	RET
;  MULTIPLY FLOATING POINT VALUE AT (H,L) BY 10
;   ENTRY:  C = NO OF BYTES IN FIELD
FM10	MOV	A,M
	ANI	0FH
	RAL	
	RAL
	RAL
	RAL
	MOV	M,A
	INX	H
	MOV	A,M
	RAR
	RAR
	RAR
	RAR
	ANI	0FH
	DCX	H
	ORA	M
	MOV	M,A		;STORE DIGIT PAIR
	INX	H
	DCR	C
	JNZ	FM10
	DCX	H
	ANI	0F0H		;LAST DIGIT = 0
	MOV	M,A
	RET
;
;  NORMALIZE
FNORM	LXI	H,FACC+1
	MVI	C,0
FN0	MOV	A,M		;FIND HIGHEST NON-0 DIGIT
	ORA	A
	JNZ	FN1
	INR	C		;COUNT ZEROS
	INR	C
	INX	H
	JMP	FN0
FN1	XRA	A
	CMP	C
	JZ	FN2		;FIRST BYTE NON-0
	DCX	H
	PUSH	B
	CALL    FLOAD           ;MOVE FRACTION LEFT
	POP	B
	LXI	H,FACC+1
FN2	MOV	A,M
	ANI	0F0H
	JNZ	FN3	        ;DONE
	INR	C
	PUSH	B
	LXI	B,6
	CALL	FM10		;SHIFT LEFT 1/2 BYTE
	POP	B
FN3	LXI	H,EXP1
	MOV	A,M
	SUB	C		;ADJUST EXP.
	JZ	FZACC		;UNDERFLOW TO 0
	JC	FZACC
	JM	FOVER		;OVERFLOW
	MOV	M,A
	JMP	FRND
;
;  OUTPUT FROM FLOATING POINT TO STRING
FOUT	LXI	H,BUFAD		;RESULT ADDRESS
	PUSH	H		;SAVE ORIG. BUFAD
	PUSH	H		;CURRENT CHAR. POINTER
	LXI	H,FACC
	MOV	A,M
	ORA     A
	JNZ	FOU1
	POP	H		;ZERO VALUE
	MVI	M,' ' 		;OUTPUT ' 0'
	INX	H
	MVI	M,'0'
	MVI	C,2
	POP	D
	RET
FOU1	ANI     080H		;SIGN
	STA	SIGN
	MOV	A,M
	ANI	07FH
	STA	EXP1		;STORE EXP.
	MVI	M,0		;MAKE FACC PURE FRACTION
	LXI	H,FACC+5
	CALL    FRND1		;ROUND TO 8 PLACES
	POP	H
	MVI	B,' '		;OUTPUT LEADING ' ' OR '-'
	LDA	SIGN
	ORA	A
	JP	FOU2
	MVI	B,'-'
FOU2	MOV	M,B
	MVI	C,1		;CHAR. COUNT
	INX	H
	LDA	EXP1
	SUI	BIAS		;UNBIASED EXPONENT
	MOV	E,A		;E = EXP
	MOV	B,A		;B = INTEGER DIGIT COUNTER
	JM	FOU3		;EXP < 0
	JZ      FOU3		;EXP = 0
	CPI 	9
	JC	FOU4		;EXP <= 8
FOU3	MVI	M,'.'		;LEADING POINT
	INR	C
	INX	H
	CPI	-7
	JC	FOU4		;LARGE NEG EXPONENT, USE E NOTATION
FOU3A	MVI	M,'0'		;ELSE STORE 0 AFTER .
	INR	C
	INX	H
	INR	A
	JNZ	FOU3A
	MVI	E,0		;RESET EXPONENT TO 0
FOU4	PUSH	H		;SAVE OUTPUT POINTER
	LXI	H,FACC
	MVI	D,4		;BYTE COUNTER
;      OUTPUT DIGITS
FOU5	INX	H		;(H,L) = POINTER TO FACC
	MOV 	A,M
	RAR			;LEFT DIGIT
	RAR
	RAR
	RAR
	ANI	0FH
	XTHL			;(H,L) <== OUTPUT POINTER
	CALL	FOUT1		;OUTPUT DIGIT
	XTHL
	MOV 	A,M
	XTHL
	ANI	0FH		;RIGHT DIGIT
	CALL	FOUT1
	XTHL
	DCR	D
	JNZ	FOU5
	POP	H		;CHAR POINTER
FOU6	DCX	H		;BACK UP TO LAST CHAR
	DCR	C
	MOV	A,M
	CPI	'.'
	JZ      FOU7		;DELETE TRAILING POINT
	CPI	'0'
	JZ  	FOU6		;BACK UP TO NONZERO
	INX	H
	INR	C
;     SEE IF 'E' NOTATION NEEDED
FOU7	MOV	A,E
	ORA 	A
	JM	FOU8
	CPI	9
	JC	FOU12		;0 <= EXP <= 8
;     OUTPUT 'E' AND EXPONENT
FOU8	MVI	M,'E'
	INX	H
	INR	C
	ORA	A
	JP	FOU9
	MVI	M,'-'		;NEGATIVE EXP
	INX	H
	INR	C
	XRA	A
	SUB	E		;MAKE EXP POSITIVE
FOU9	CPI	10
	JC	FOU11		;ONE-DIGIT EXP
;  GET FIRST DIGIT OF EXP
   	MVI	D,0		;D <== EXP/10
FOU10 	SUI	10
	INR	D
	CPI	10
	JNC	FOU10
	MOV	E,A		;E <== REMAINDER
	MOV	A,D
	ADI	'0'
	MOV	M,A		;OUTPUT FIRST DIGIT
	INX	H
	INR	C
	MOV	A,E
FOU11	ADI	'0'
	MOV	M,A
	INR	C
FOU12	POP	D		;(D,E) = BUFAD
	RET
;     OUTPUT DIGIT
FOUT1	ADI	'0'  		;MAKE CHARACTER
	MOV	M,A
	INR	C		;CHAR COUNT
	INX	H
	DCR	B		;INTEGER COUNT
	RNZ
	MVI	M,'.'		;DECIMAL POINT
	INR	C
	INX	H
	RET
;  OVERFLOW
FOVER	ERROR	N,O,V
	LXI	H,FPMAX
	CALL	FLOAD		;LOAD MAX VALUE
	LXI	H,FACC
	LDA	SIGN
	ORA	M		;APPEND SIGN
	MOV	M,A
	RET
;
;  EVALUATE POLYNOMIAL
;    ENTRY:  FTMP1 = VARIABLE,  (C) = DEGREE, (H,L) = HIGHEST ORDER COEFFICIENT
; P(Y) = (...(Y*CN+ C(N-1)) * Y + ...) * Y + C0
FPOLY	PUSH  	B
	PUSH	H
	CALL	FMUL		;MULT BY HIGHEST COEFFICIENT
	JMP	FPLY1
FPLY0	PUSH	B
	PUSH	H
	CALL	FADD
	LXI	H,FTMP1
	CALL	FMUL		;MULT BY VARIABLE
FPLY1	POP	H
	POP	B
	LXI	D,-6
	DAD	D		;NEXT COEFFICIENT
	DCR	C		;DEGREE COUNT
	JNZ	FPLY0
	JMP	FADD		;ADD LAST COEFFICIENT
;  RENORMALIZE
FRNRM	LXI	H,FACC
	MOV	A,M
	ORA	A
	RZ			;NO RENORM NEEDED
	LXI	B,7
	CALL	FD10		;SHIFT RIGHT 1/2 BYTE
	LXI	H,EXP1		;INCR. EXP
	INR	M
	JM	FOVER		;OVERFLOW
	RET
;
;  ROUND FLOATING POINT VALUE
FRND	LXI	H,ACALT
	CALL	FRND1 
FRND0	LXI	H,SIGN
	LDA	EXP1
	ORA	M		;REPACK RESULT
	STA	FACC
	RET
FRND1	MOV	A,M
	ADI	050H
	DAA
	RNC			;DONE IF NO CARRY
	DCX	H
	CALL	FADL0		;CARRY ONE
	JMP	FRNRM		;RENORMALIZE
;
;  SINE FUNCTION
; IF ABS(X) < 4 E (-7) RETURN X
; ELSE SIN (X) = COS (X - PI/2)
FSIN	LXI	H,FACC
	MOV	A,M
	ANI	07FH		;GET EXP
	CPI	039H
;     IF ABS(X) < 4 E (-7) THEN RETURN X
	RC			;EXP < -7
	JNZ	FS0
	INX	H
	MOV	A,M
	CPI	040H
	RC
;     ELSE SIN(X) = COS (X - PI/2)
FS0	LXI	H,DPI2
	CALL	FSUB
	JMP	FCOS
;
;  SQUARE ROOT
; X = Y * 10^A WHERE .1 <= Y < 1
; B <== INT (A/2)
; C <== (A - 2*B) * SQR(10)
; Z0 <== POLYNOMIAL(Y)
; Z1 <== 1/2 (Z0 + Y/Z0), ETC.
; SQR(X) = Z * 10^B * SQR(10) IF C=1
;        = Z * 10^B IF C = 0
FSQR	LXI	H,FACC
	MOV	A,M
	ORA	A
	CM	FCERN		;SQR OF NEGATIVE
	ANI	7FH
	MOV	M,A		;FORCE POSITIVE
	SUI	BIAS		;UNBIAS EXP
	RAR			;A <== EXP/2
	PUSH	PSW		;SAVE EVEN/ODD FLAG
	STA	SIGN1		;STORE EXP/2
	MVI	M,BIAS		;SET EXP TO 0
	CALL	FSTT1		;SAVE X
	INX	H
	MOV	A,M
	CPI	025H
	JNC	FSQ4		;X > .25
;     X <= .25
	LXI	H,SR3
	MVI	C,3
FSQ0	CALL	FPOLY		;POLYNOMIAL
;     2 NEWTON-RAPHSON ITERATIONS
	CALL	FSQ2
	CALL	FSQ2
	POP	PSW		;EVEN/ODD FLAG
	JNC	FSQ1		;EVEN
	LXI	H,SQR10
	CALL	FMUL		;IF ODD MULT BY SQR(10)
FSQ1	LXI	H,FACC
	LDA	SIGN1		;GET EXP/2
	ADD	M
	MOV	M,A		; SET RESULT EXP
	RET
;     NEWTON-RAPHSON ITERATION
;       Z1 =   (X/Z0 + Z0) * .5
FSQ2	CALL	FSTT2		;STORE Z
	LXI	H,FTMP1
	CALL	FLOAD
	LXI 	H,FTMP2
	CALL  	FDIV		;X/Z0
	LXI	H,FTMP2
	CALL	FADD
	LXI	H,HALF
	JMP	FMUL
;     X > .25
FSQ4	LXI	H,SR6
	MVI	C,2
	JMP	FSQ0
;
;  STORE FACC AT (H,L)
;     EXIT:  (H,L) = DESTINATION FIELD
FSTT4	LXI	H,FTMP4
	JMP	FSTOR
FSTT3	LXI	H,FTMP3
	JMP	FSTOR
FSTT2	LXI	H,FTMP2
	JMP	FSTOR
FSTT1	LXI	H,FTMP1
FSTOR	PUSH	B
	LXI	B,5
	DAD	B		;(H,L) = LOW END OF FIELD
	INX	B
	LXI	D,FACC+5
FST0	LDAX	D
	MOV	M,A
	DCX	D
	DCX	H
	DCR	C
	JNZ	FST0
	INX	H
	POP	B
	RET
;
;  FLOATING POINT SUBTRACTION		FACC <== FACC - (H,L)
FSUB 	MOV	A,M
	ORA	A
	RZ			;OP - 2 = 0
	LXI	D,FACC
	LDAX	D
	ORA	A
	JZ	FSUB0		;SUBTRACT FROM 0
   	XRI	080H		;REVERSE SIGN
	JMP	FADD0
FSUB0	CALL	FLOAD		;LOAD OP-2
	JMP	FCHS
;  SUBTRACT LOOP
FSUB1	LDA	FLEN
	MOV	C,A		;LENGTH OF LOOP
 	STC
FSU2 	XCHG
	MVI	A,099H
	ACI	0
	SUB	M
	XCHG
	ADD	M
	DAA
	MOV	M,A
	DCX	H
	DCX	D
	DCR	C
	JNZ	FSU2
	JC	FSU4		;FORM CORRECT
	LHLD	RSLT
	LDA	FLEN
	MOV	C,A
	STC
FSU3	MVI	A,099H		;RECOMPLEMENT
	ACI	0
	SUB	M
	ORA	A		;CLEAR ACY  
	DAA
	MOV	M,A
	DCX	H
	DCR	C
	JNZ 	FSU3
	LXI	H,SIGN
	MOV	A,M
	XRI	080H		;REVERSE SIGN ON RECOMPLEMENT
	MOV	M,A
FSU4	LXI	H,FACC+1
	MVI	C,7
FSU5	MOV	A,M		;CHECK FOR 0 RESULT
	ORA	A
	JNZ	FNORM		;NORMALIZE NON-0
	INX	H
 	DCR	C
	JNZ	FSU5	
	RET			;RETURN 0
;
;  TANGENT
; STORE SIGN AND MAKE ABS VALUE
; Y <== 4X/PI
; R <== FRAC(Y)
; A <== INT(Y) (MOD 4)
; IF A > 2 THEN REVERSE SIGN
; IF A IS ODD, R <== 1-R
; IF A = 1 OR 2 (MOD 4) SET COTAN FLAG
; Y <== R * PI/4
; T <== Y * Y
; TAN (X) = X * (C0 + P2(T)/Q2(T))
; IF FLAG IS SET, TAKE COTAN I.E. 1/TAN
; APPEND SIGN
FTAN	XRA	A
 	STA	EXP4		;CLEAR COTAN FLAG
	LXI	H,FACC
	MOV	A,M
	ANI	080H		;GET SIGN
	STA	SIGN1
	MOV	A,M
	ANI	07FH		;GET EXP
	MOV	M,A		;MAKE ABS. VALUE
	CALL	FCO34		;FTMP2 <== 4X/PI
	LXI	H,INTMX
	CALL	FCMP
	JNC	FCERN		;VALUE TOO LARGE
	CALL	FFIX		;GET INTEGER PART
	PUSH	B		;SAVE INT PART
	CALL	FCHS		;GET FRAC PART
	CALL	FADT2		;R = FRAC PART
	POP	B
	MOV	A,C
	ANI	3		;A <== INT PART (MOD 4)
	CPI	2
	JC	FTAN2
	LXI	H,SIGN1		;IF > 2 (MOD 4) REVERSE SIGN
	PUSH	PSW
	MOV	A,M
	XRI	080H
	MOV	M,A
	POP	PSW
FTAN2	PUSH	PSW
	RAR
	JNC	FTAN3		;EVEN
	CALL	FCHS		;IF ODD, R = 1 - R
	LXI	H,FPONE
	CALL	FADD
FTAN3	POP	PSW
	INR	A
	ANI	2
	JZ	FTAN4		;A WAS 0 OR 3 (MOD 4)
	STA	EXP4		;IF 1 OR 2 (MOD 4) SET COTAN FLAG
FTAN4	LXI	H,DPI4
	CALL	FC34A		;FTMP2 = Y = R * PI/4
	CALL	FCO35	 	;FTMP1 = Y^2
	LXI	H,TB3
	CALL	FTAN7		;EVAL. CONTINUED FRAC.
	LXI	H,TC0
	CALL	FADD
	LXI	H,FTMP2
	CALL	FMUL		;*Y
FTAN5	LDA	EXP4		;GET INVERT FLAG
	ORA	A
	CNZ	FLINV		;INVERT IF DESIRED
	JMP	FAT6		;APPEND SIGN
;     EVALUATE CONTINUED FRACTION
; P1 <== C2 (X + B3)
; Q1 <== (X + B2) (X + B3) + C3
; P2 <== C1 * Q1
; Q2 <== (X + B1) * Q1 + P1
FTAN6	PUSH	H
	CALL	FADD		;X + B3
	CALL	FSTT3		;FTMP3 = X + B3
	POP	H
	LXI	D,6
	DAD	D		;NEXT COEFFICIENT
	PUSH	H
	CALL	FMUL		;* C3
	CALL	FSTT4		;FTMP4 = P1
	LXI	H,FTMP1
	CALL	FLOAD		;X
	POP	H
	LXI	D,6
	DAD	D
 	PUSH	H
	CALL	FADD		;+ B2
	LXI	H,FTMP3
	CALL	FMUL		; * (X + B3)
	POP	H
	LXI	D,6
	DAD	D
	PUSH	H
	CALL	FADD		; + C3
	CALL	FSTT3		;FTMP3 = Q1
	POP	H
	RET
FTAN7	CALL	FTAN6
	PUSH	H
	LXI	H,FTMP1
	CALL	FLOAD		;X
	POP	H
	LXI	D,6
	DAD	D
	PUSH	H
	CALL	FADD		; + B1
	LXI	H,FTMP3
	CALL	FMUL		; * Q1
	LXI	H,FTMP4
	CALL	FADD		; + P1
	CALL	FSTT4		; FTMP4 = Q2
	LXI	H,FTMP3
	CALL	FLOAD		; Q1
	POP	H
	LXI	D,6
	DAD	D
	CALL	FMUL		; * C1 = P2
	LXI	H,FTMP4        ; P2/Q2
	JMP	FDIV
;
;  ZERO OUT FACC
FZACC	MVI	C,6
	LXI	H,FACC
;  ZERO OUT FLOATING POINT FIELD AT (H,L)
FZERO	XRA	A
FZER1	MOV	M,A
	INX	H
	DCR	C
	JNZ	FZER1
	RET
;
;  UNPACK FLOATING POINT OPERANDS FOR DIV. AND MULT.
UPAC1	ANI	07FH		;GET EXP-2 
	MOV	B,A		;SAVE EXP-2 IN B
	LXI	D,FACC
	LDAX	D
	ORA	A
	JNZ	UNP1
;     OP-1 = 0
	INX	SP		;EXIT FROM CALL
	INX	SP
	RET			;RETURN 0
UNP1	XRA	M		;GET RESULT SIGN
	ANI	080H
	STA	SIGN
UPAC2	XCHG			;(D,E) = OP-2
	MOV	A,M
	PUSH	PSW		;SAVE EXP-1
	MVI 	M,0		;FACC <== PURE FRACTION
	LXI	H,ACALT
	MVI	C,7
	CALL	FZERO		;CLEAR ACALT
	POP	PSW
	ANI	07FH		;EXP-1
	RET

	endif

;end of module FPBCD
;INOUT 12/05/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;input / output & error routines

;WRITC writes a character from A.
;Call:	A	char to be written
;Retn:	A,BC,DE,HL	preserved
;The character is sent to the CON device if (OMODE) = 0.
;The char is also sent to the LST device if (LMODE) <> 0.
;The char is sent to the disk (in CP/M nonCOMPL version) if (AMODE) <> 0.
;COLUM gives the print column of the last char written (line of width WIDTH).
;A crlf is written if the current char causes COLUM > WIDTH.
;A linefeed is preceeded by (NULLS) nulls.
	if	not wild
;WRTS0 is called by print routines to print trailing space after numbers.
wrts0:	lhld	colum		;column to L, width to H
	mov	a,l
	cmp	h
	rz			;suppress trailing space if in last column
	endif
wrtsp:	mvi	a,' '
writc:	push4			;save registers
	mov	c,a		;char passed in C
	lhld	colum		;width to H, column to L
	cpi	cr
	jnz	wrtc1
	mvi	l,0		;cr resets column count
wrtc1:	if	not wild
	cpi	lf
	cz	wnuls		;write nulls if linefeed
	endif
	if	not epstn
	cpi	cntlh
	cz	decrc		;decrement column if <control-h>
	endif
	cpi	20H
	cnc	bumpc		;bump column if printable
	shld	colum
	if	cpm and sdisk and (not rtpak)
	lhld	filep
	mov	a,h
	ora	l
	jnz	dwrit		;write char to disk file
	endif
	if	bendx and sdisk
	lda	bfilp
	ora	a
	jnz	dwrit		;write char to disk file
	endif
	lda	omode
	if	(cpm or isis2 or genmc or (bendx and sdisk)) and not compl
	cpi	80H		;check if doing ASCII save
	jz	dsave
	endif
	if	wild
	cpi	6
	jnc	wrtc2		;>= 6, not PRINT @
	ora	a
	jnz	wwrit		;1 <= OMODE <= 5, PRINT @ to monitor
	endif
wrtc2:	ora	a		;check if output to be suppressed
	if	(not wild) or (not rtpak)	;no CNOUT in WILD RTPAK
	push	b
	cz	cnout
	pop	b
	endif
	if	(cpm or isis2 or genmc) and not compl
	cpi	7FH		;check if doing ASCII LOAD
	jz	pop4		;ignore remaining checks if so
	endif
	if	cpm and debug and not compl
	lda	amode
	ora	a
	push	b
	cnz	dkout		;write to disk if saving
	pop	b
	endif
	if	not wild
	lda	lmode
	ora	a		;check LST write mode
	cnz	lout
	endif
pop4:	pop	psw		;common restore and returns
	jmp	pop3
decrc:	dcr	l		;decrement column count for backspace
	rp			;done unless was column 0
	inr	l		;undecrement
	jmp	wlf		;write linefeed and return
bumpc:	mov	a,l
	inr	l		;increment column
	cmp	h
	rc			;column < width -- ok
	mov	a,h		;fetch width
	ora	a
	rz			;suppress automatic crlfs if width is 0
	mvi	l,1		;column >= width -- write crlf and return
	if	epstn
	mov	a,h
	cpi	80
	rz			;Epstein hardware does crlf at column 80
	endif
wcrlf:	mvi	a,cr
	call	writc
wlf:	mvi	a,lf
	jmp	writc
	if	not wild
wnuls:	lda	nulls		;fetch null count
	mov	b,a		;save in B
	inr	b
	xra	a		;null char to A
wnul1:	dcr	b
	rz			;done
	call	writc		;write a null
	jmp	wnul1		;and test for more
	endif

;READC reads one char from the console.
;The parity bit (i.e. bit 7) is reset.
;The user is returned to direct mode if the char is <cntl-c>.
;The system is booted if the char is <cntl-b>.
;The LST mode is toggled if  the char is <cntl-p>.
;Retn:	A	char read, masked by 7FH
;	BC,DE,HL	preserved
readc:	push3			;save registers
	if	(cpm or isis2 or genmc or (bendx and sdisk)) and not compl
	lda	omode
	cpi	7FH		;check if ASCII load
	jz	dload		;yes
	endif
	call	conin		;read the console
	ani	7FH		;mask off parity
	if	(not wild) or (not rtpak)	;no ^B, ^C in Wild RTPAK
	cpi	cntlb
	cz	echoc
	if	debug
	cz	boot		;call monitor if debug and control-B
	else
	if	camac and cpm
	cz	cdsab		;disable interrupt before booting if CP/M CAMAC
	endif
	jz	boot		;jmp monitor if not debug and control-B
	endif
	cpi	cntlc
	cz	echoc
	jz	dmodc		;break to direct mode if control-c
	endif
	if	cpm and debug and not compl
	cpi	cntlx
	jz	dsav0
	endif
	if	not wild 	;no LST device
	cpi	cntlp
	jnz	pop3
	call	echoc
	lda	lmode		;toggle LST mode if control-p
	cma
	sta	lmode
	mvi	a,cntlp		;restore char to A
	endif
	jmp	pop3

	if	(cpm or isis2 or genmc) and not compl
dsave:	call	dkout		;write char to disk file
	jmp	pop4		;restore and return
	endif

;REDYC determines whether char is ready at console.
;Retn:	Carry	set iff char ready
redyc:	push3			;save registers
	call	cstat
	rrc
	jmp	pop3		;restore and return

;CTEST looks for character at console, and READCs it if present.
;All chars are ignored except the following:
;	<cntl-b>	reboots (from READC)
;	<cntl-c>	breaks (branch to dmode, with message)
;	<cntl-o>	toggles CONsole output mode
;	<cntl-p>	toggles LST mode (from READC)
;	<cntl-s>	suspends interpreter execution until either
;				<cntl-x> (reboot) or <cntl-q> (resume)
;Retn:	A	clobbered
;	BC,DE,HL	preserved
ctest:	call	redyc		;check if char ready
	rnc			;no char ready
ctst0:	call	readc		;ready, so read the char -- driver entry point
	if	(not wild) or (not rtpak)
	cpi	cntlo
	cz	echoc
	jz	ctst1		;toggle output mode if cntl-o and return
	endif
	if	not wild
	cpi	cntls
	jz	ctst2		;wait for cntl-q if cntl-s
	endif
	sta	gchar		;else save it for GET
	ret
	if	not wild
ctst2:	call	readc
	cpi	cntlq		;check if cntl-q
	rz			;return if so
	cpi	cntls
	rz			;return if cntl-s toggled also
	jmp	ctst2		;else wait
	endif
	if	(not wild) or (not rtpak)
ctst1:	lda	omode
	cma			;toggle omode
	sta	omode
	jmp	wcrlf		;echo crlf and return
	endif

;WRTBS prints (BC) as a signed decimal number, with leading '-' if negative.
;WRTBU prints (BC) as an unsigned decimal number.
;The current column & width are checked to avoid breaking the number with crlf.
;WRTB1 is an entry point from PRINT, to print nonnegatives with leading space.
;Call:	BC	integer
;Retn:	A	clobbered
;	BC,DE,HL	preserved
wrtbs:	mov	a,b
	ora	a
	jp	wrtbu		;write unsigned if positive
	push	b		;else save BC
	call	iumin		;negate BC (NB -32768 is OK)
	mvi	a,'-'		;leading minus sign
	jmp	wrtb2		;and write
wrtbu:	xra	a		;no leading char
wrtb1:	push	b		;save BC
wrtb2:	push	d
	push	h		;and DE, HL
	call	cvtis		;convert integer to string
	call	prstl		;print string on one line
	jmp	pop3

;PRSTR prints the string addressed by C,DE.
;PRSTL tries to print the string addressed by C,DE without crlf.
prstl:	lhld	colum		;width to H, column to L
	mov	a,h		;fetch width
	ora	a
	jz	prstr		;just print the string if width is 0
	mov	a,c		;length to A
	add	l		;column + length = last column needed
	cmc			;carry reset iff > 255
	jnc	prsl1		;much too long, write crlf first
	cmp	h		;carry reset iff last needed >= width
	jz	prstr		;just fits
prsl1:	cnc	wcrlf		;write crlf if too long for current printline
prstr:	inr	c		;length+1
prst1:	dcr	c		;decrement length
	rz			;done
	ldax	d		;fetch next string char
	call	writc		;print it
	inx	d
	jmp	prst1		;and write more

;CVTIS converts the integer in BC to a string of ASCII decimal digits.
;The result is loaded into the buffer at BUFAD.
;Call:	A	leading char (ASCII space or minus sign, 0 for none)
;	BC	unsigned integer value
;Retn:	A,C	length of string result
;	DE	location of string result, i.e. BUFAD
;	B,HL	clobbered
cvtis:	mov	h,b
	mov	l,c		;value to HL
	lxi	b,bufad		;destination to BC
	push	b		;and saved
	ora	a
	jz	cvis1		;no leading char desired
	stax	b		;store leading char
	inx	b		;and point to next available location
	xra	a		;clear A for mod10
cvis1:	lxi	d,-10000
	call	mod10
	lxi	d,-1000
	call	mod10
	lxi	d,-100
	call	mod10
	lxi	d,-10
	call	mod10
	mov	a,l		;units digit to A
	call	mod1b		;and to string
	pop	d		;location to DE
	mov	a,c		;last+1 to A
	sub	e		;last+1 - first = length
	mov	c,a		;and length to C
	ret

;MOD10 adds the decimal digit n = (HL) div -(DE) to the string at (BC)
;and leaves HL with (HL) + n * (DE).  Zeros suppressed if (A) = 0 when called.
mod10:	push	b
	mvi	b,255
mod1a:	shld	temp
	inr	b
	dad	d
	jc	mod1a
	lhld	temp		;remainder to HL
	mov	d,b		;result to D, temporarily
	pop	b		;restore BC
	ora	d		;zero set iff (A) > 0 or (D) > 0
	rz
	mov	a,d		;result to A
mod1b:	adi	'0'		;entry point for final digit
	stax	b		;add ASCII digit to string
	inx	b
	ret

	if	not compl
;PRNTL prints line # (as unsigned decimal, suppressed if zero) and space
;and a line of user source text, expanding tokens as necessary.
;Call:	HL	address of line #
;Retn:	A,DE	clobbered
;	BC	preserved
;	HL	address following last text byte, i.e. after <cr>
;PRTL0 prints line with <linefeed> if (DE) matches location.
;PRTLC prints line with <linefeed> at TEXTP.
prtlc:	lhld	textp
	xchg			;current TEXTP to DE
	lhld	lnnum		;current LNNUM to HL
	call	prtl0		;print the line
	jmp	wcrlf		;and crlf
prntl:	lxi	d,0		;print without linefeed
prtl0:	push	b
	call	linb1		;line # to BC
	jz	prtl3		;zero, i.e. direct mode
	call	wrtbu		;write the line #
	inx	h		;past break byte
	inx	h		;to first text byte
	mvi	a,' '
	cmp	m		;check if first char is space
	cnz	wrtsp		;write a space if not
prtl1:	pop	b		;restore BC
prtl2:	call	cmdhu		;compare to text pointer
	cz	wlf		;write linefeed if equal
	mov	a,m		;fetch next byte from text
	inx	h
	cpi	cr
	rz			;cr, done
	push	h
	call	prtch		;print char or token
	pop	h
	jmp	prtl2
prtl3:	lxi	h,tlnad
	jmp	prtl1

;PRTCH prints a char or token.
;Call:	A	char or token
prtch:	call	fndtk		;look up the token
	jnc	writc		;char, just print it
	endif			;else fall through to PRTST to print token

;PRTST prints a string with successive calls to WRITC.
;Call:	HL	address of string's first text char
;Retn:	A	clobbered
;	BC,DE	preserved
;	HL	address following string's last text char
prtst:	mov	a,m		;fetch a byte to print
	ani	7FH		;mask off end of entry bit
	call	writc		;print it
	mov	a,m		;check end of entry byte again
	ora	a
	inx	h		;move up pointer
	jp	prtst		;keep printing
	ret			;end of entry, done

;PRNTM prints the message which directly follows the CALL PRNTM.
;PRTM0 turns on OMODE and then prints message with PRNTM.
prtm0:	xra	a
	sta	omode		;turn on output mode
	if	cpm and sdisk	;reset file pointer
	lxi	h,0
	shld	filep
	endif
	if	bendx and sdisk
	sta	bfilp
	endif
prntm:	pop	h		;address following CALL to HL
	call	prtst		;print the message
	pchl			;and return to the next address

	if	wild		;error handling
errof:
erron:
erroc:	xra	a
	sta	omode		;reset OMODE in case within PRINT @
	pop	h		;recover address of caller
	mov	d,m		;first message char to D
	inx	h
	mov	a,m
	ani	7FH
	mov	e,a		;second message char to E
	call	linbc		;line number to BC
	mov	h,b
	mov	l,c		;then to HL
	mvi	a,9		;error code = 9 to A
	jmp	wmon		;and pass to monitor
	else			;NOT WILD
;ERROF, ERRON and ERROC are error routine entry points for Fatal,
;Nonfatal and Continue errors.  In each case the
;error routine is invoked with the ERROR macro, e.g. with
;	ERROR	F, X, Y
;issuing a fatal XY error with the code
;	call	errof
;	db	'X', 'Y' or 80H
;The error message following the call is printed, followed by the
;user source line # (if any) and user source line.
;If TRAP, any error terminates execution and returns the user to DMODE, with
;a CONTinue entry on the CSTACK to allow continuation at the next command.
;If UNTRAP:
;Fatal errors return to DMODE, as if TRAP.
;Nonfatal errors continue execution with all registers preserved.
;Continue errors continue execution with the next source text command.
errof:	xthl			;message address to HL, HL to stack
	push	psw		;PSW to stack
	stc			;Carry set for fatal
	jmp	erro0
erroc:	xthl
	push	psw
	xra	a		;Carry reset, Zero set for Continue
	jmp	erro0
erron:	xthl
	push	psw
	ori	1		;Carry reset, Zero reset for nonfatal
erro0:	push	d		;save DE
	xchg			;message address to DE
	if	cpm and sdisk and (not rtpak)
	lhld	filep
	endif
	if	bendx and sdisk
	lxi	h,bfilp
	mov	h,m
	endif
	push	h		;save current BFILP
	push	psw		;save error type
	call	prtm0		;turn on OMODE, print CRLF
	db	cr, lf or 80H
	xchg			;message address to HL
	call	prtst		;print message
	pop	psw
	push	h		;save return address in case nonfatal
	push	psw		;and save error type again
	call	prntm		;print ERROR message
	if	compl
	db	' ERROR IN LINE', ' ' or 80H
	push	b
	call	linbc		;line # to BC
	call	wrtbu		;write it
	call	wcrlf		;and crlf
	pop	b
	else
	db	' ERROR:', ' ' or 80H
	call	prtlc		;print the line
	if	editc
	push	b
	call	linbc		;line number to BC
	jz	erro1		;skip resetting ERRLN if 0
	mov	h,b
	mov	l,c		;then to HL
	shld	errln		;and save for EDIT
erro1:	pop	b		;restore BC
	endif
	lda	trap		;0 trap, 255 untrap
	ora	a
	jz	erro2		;TRAP, so scan to next and go to DMODE
	endif
	pop	psw		;recover error type
	jc	erro2		;fatal, to DMODE as if TRAP
	jz	nextc		;continue with next command
	pop	d		;return address to DE
	if	cpm and sdisk and (not rtpak)
	pop	h
	shld	filep		;restore FILEP
	endif
	if	bendx and sdisk
	pop	psw
	sta	bfilp
	endif
	call	ctest		;test for console break char
	xchg			;return address to HL
	pop	d		;restore DE
	pop	psw		;restore psw
	xthl			;restore HL, return address to stack
	ret
	if	camac
erro2	equ	dmod2		;no continuation after errors in CAMAC versions
	else
;Fatal errors:  build control stack entry, scan to delimiter, and goto DMODE.
erro2:	call	linbc		;LNNUM to HL, Zero set iff direct
	jz	dmod2		;error from DMODE, skip entry building
	dcx	h		;address length byte of current line
	push	h		;and save
	mov	e,m
	mvi	d,0		;length of current line to DE
	dad	d		;address of following line
	xchg			;to DE
	lhld	textp
	call	cmdhu		;check if within current text line
	jc	dmod2		;after current line, skip entry building
	pop	d
	call	cmdhu
	jnc	dmod2		;before current line, skip entry building
	call	gtdel		;else scan to delimiter
	jmp	endc1		;build break entry and go to DMODE
	endif			;end of NOT CAMAC conditional
	endif			;end of NOT WILD condtional


;end of INOUT
	page
;ISIS2 05/21/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;statement routines for ISIS-II version SAVE and LOAD, plus GTFIL

	if	isis2		;ISIS-II versions

	if	rtpak		;UF error in Runtime Module version
save	equ	uferr
load	equ	uferr
	else

;^B exit to ISIS-II
	if	debug
boot	equ	0		;^B gets MDS monitor rather than ISIS-II
	else
boot:	mvi	c,exitf
	lxi	d,exblk
	call	isis		;return to ISIS
	endif

;SAVE <filename>
save:	call	prntm		;print SAVING message
	db	'SAVING', ' ' or 80H
	mvi	a,2		;set A for writing
	call	gtfil		;get file name and open it
	shld	wrblk		;set aft of write block
	jnc	savea		;ASCII save
	ora	a
	jnz	saveh		;HEX save
	if	romsq
	lhld	sourc
	else
	lxi	h,srcad
	endif
	shld	wrblk+2		;set starting address = srcad
	if	romsq
	push	d
	call	last
	pop	d
	else
	lhld	eofad		;eof addr to HL
	endif
	dad	d		;length of file to HL
	shld	wrblk+4		;set count for write
	call	isisw		;write the file
isisc:	mvi	c,dkclf		;close the file and return
	lxi	d,clblk
isise:	call	isis
	lda	estat
	ora	a		;check error status
	rz			;no error
	mvi	c,erri2
	lxi	d,estat
	call	isis		;issue ISIS error, fall through to DKERR
dkerr:	error	f, D, K		;fatal DK error
;ASCII SAVE
savea:	lda	omode
	push	psw		;save output mode
	mvi	a,80H
	sta	omode		;80H to OMODE to indicate ASCII save
	call	save0		;initialize file block
	lxi	b,-1
	call	list1		;list the program to disk file
	mvi	c,cntlz
	call	dwrit		;followed by eof
	pop	psw
	sta	omode		;restore omode value
	jmp	isisc		;close the file and return
;SAVE0 initializes WRBLK to write 1 char from TEMP2,
;returns first program loc in HL.
save0:	lxi	h,temp2
	shld	wrblk+2		;file chars passed in temp2
	lxi	h,1
	shld	wrblk+4		;set count = 1 char per call
	if	romsq
	lhld	sourc
	else
	lxi	h,srcad
	endif
	ret
;HEX SAVE
saveh:	xchg			;-first to HL
	shld	temp		;and saved
	call	save0		;initialize WRBLK
	push	h		;save first
	if	romsq		;last address to HL
	call	last
	else
	lhld	eofad
	endif
	xchg			;last to DE
	pop	h		;first to HL
	dcx	h		;first - 1
;convert the file to hex and write it
savh1:	push	h		;save current
	lxi	b,10H		;max record length to BC
	dad	b		;current + max to HL
	call	cmdhu		;compare eof to current + max
	pop	h		;restore current
	mov	a,c		;max to A
	jnc	savh2		;eof >= current + max, write max
	mov	a,e
	sub	l
	inr	a		;eof+1 - current = remaining to be written
	jz	savh4		;current = eof+1, just write eof record
savh2:	push	d		;save eof address
	mov	e,a		;length to  E
	mvi	d,0		;checksum to D
	mvi	c,':'
	call	dwrit		;write record mark
	mov	a,e
	call	wbyte		;write length
	call	waddr		;write address and record type
savh3:	mov	a,m		;fetch data byte
	inx	h
	call	wbyte		;write data
	dcr	e
	jnz	savh3		;write more data
	call	wcsum		;write the checksum
	pop	d		;restore eof address
	jmp	savh1		;and do more
;write eof record
savh4:	mvi	c,':'
	call	dwrit		;write :
	xra	a
	mov	d,a		;checksum to D
	call	wbyte		;write record length = 0
	if	romsq
	lhld	sourc
	dcx	h
	else
	lxi	h,srcad-1
	endif
	call	waddr		;write starting address 0 and record type
	call	wcsum		;write the checksum
	mvi	c,cntlz
	call	dwrit		;write eof
	jmp	isisc		;close file and return


;LOAD <filename>
load:	if	romsq
	call	issrc		;must be addressing working space
	endif
	call	prntm		;print LOADING message
	db	'LOADING', ' ' or 80H
	mvi	a,1		;set A for reading
	call	gtfil		;get file name, open it
	shld	rdblk		;set aft of read param block
	jnc	loada		;ASCII load
	ora	a
	jnz	loadh		;HEX load
	lxi	h,srcad
	shld	rdblk+2		;set starting address = srcad
	lhld	memt
	dad	d		;max possible program length to HL
	shld	rdblk+4		;and then to read block
	call	new		;clobber old program
	call	isisr		;read the file
	call	isisc		;close the file
	lxi	d,srcad-1	;first addr - 1 to DE
	lhld	temp		;actual count to HL
	dad	d		;compute eof address
load1:	mov	a,m
	ora	a
	jz	dmodx		;all is well
load2:	call	new		;else erase the bad prog
	jmp	dkerr		;and issue DK error
isisr:	mvi	c,dkrdf
	lxi	d,rdblk
	jmp	isise
;ASCII load
loada:	call	load0		;set RDBLK and clobber old program
	lda	omode
	push	psw		;save OMODE
	mvi	a,7FH
	sta	omode		;set OMODE for ASCII load
loda1:	call	gtlin		;get a line
	call	tkize		;tokenize it
	jc	loda1		;ignore if no line #
	cnz	addln		;add to source
	jmp	loda1		;and keep loading
;LOAD0 initializes RDBLK to read 1 char to TEMP2.
load0:	lxi	h,temp2
	shld	rdblk+2
	lxi	h,1
	shld	rdblk+4
	jmp	new
;HEX load
loadh:	call	load0		;initialize RDBLK and clobber old program
lodh1:	call	dread		;read a char from hex file
	sui	':'
	jnz	lodh1		;not record mark, retry
	mov	d,a		;0 for checksum to D
	call	rbyte		;read a file byte
	jz	lodh3		;eof record
	mov	e,a		;else record length to E
	call	rbyte		;read destination msb
	push	psw		;and save
	call	rbyte		;read destination lsb
	pop	h		;destination msb to H
	mov	l,a		;destination now in HL
	lxi	b,srcad-1	;base address to BC
	dad	b		;add to base address for actual destination
	call	rbyte		;skip type byte
lodh2:	call	rbyte		;read a data byte
	mov	m,a		;and store
	inx	h		;address next
	dcr	e
	jnz	lodh2		;load another data byte
	call	rbyte		;read checksum
	jnz	loadx		;checksum error
	dcx	h		;point to last loaded byte
	shld	eofad		;and reset EOFAD in case end of program
	jmp	lodh1		;load next record
lodh3:	call	isisc		;close the file
	lhld	eofad		;point to end of file
	jmp	load1
loadx:	call	isisc		;close the file after error
	jmp	load2		;erase bad program, issue DK error


;The ISIS-II version of GTFIL gets a file name and opens the file.
;The filename is [:device:]"<filename>" [, {A | H} ] .
;Call:	A	1 to read, 2 to write
;	HL	address of message to print (SAVING or LOADING)
;Retn:	Carry	Reset iff ASCII specified (with ,A)
;	A	0 for .XYB, 1 for .HEX
;	DE	- first address of source + 1
;	HL	aft of opened file
devf0:	db	':F0:', 80H	;default device name to copy
basft:	db	'.BAS ', 80H	;file type to copy
xybft:	db	'.XYB ', 80H	;file type to copy
hexft:	db	'.HEX ', 80H	;file type to copy
gtfil:	sta	opblk+4		;set access mode of open parameter block
	mvi	d,':'
	call	gtd		;look for device name
	lxi	d,fname		;file name addr to DE
	jc	gtfl0		;no device name, take default
	call	gtchd		;store :, read 1st char
	call	gtchd		;store 1st, read 2nd char
	call	gtchd		;store 2nd, read :
	cpi	':'
	jnz	snerr		;no :
	call	gtchd		;store :, read "
gtfl1:	cpi	'"'
	jnz	snerr		;no "
	call	gtild		;get first filename char
	jc	snerr		;not letter or digit, SN error
	mvi	b,5		;max # chars in filename is 6
gtfl2:	stax	d		;store filename char
	inx	d
	call	writc		;echo the char
gtfl3:	call	gtild		;get next filename char
	jc	gtfl4		;no more filename chars
	dcr	b
	jp	gtfl2		;store and get next
	jmp	gtfl3		;too long, skip storing
gtfl4:	push	d		;save next fname addr
	call	wcrlf		;write cr and lf
	mvi	d,'"'
	call	gtdsn		;skip close quote
	call	gtcom		;look for comma
	lxi	d,xybft		;XYB filetype addr to DE
	mvi	a,0		;0 to A in case .XYB
	cnc	gtfl5		;use other filetype instead if comma
	pop	h		;next fname addr to HL
	push	psw		;save carry status
	call	cpyst		;copy file type to FNAME
	mvi	c,dkopf
	lxi	d,opblk
	call	isise		;open the file
	if	romsq
	lhld	sourc
	dcx	h		;address preceding source to HL
	xchg			;and then to DE
	else
	lxi	d,srcad-1	;address preceding source to DE
	endif
	call	cplde		;- first address + 1
	lhld	aft		;aft to HL
	pop	psw		;restore carry status
	ret
gtfl5:	mvi	d,'A'
	call	gtd		;look for A
	lxi	d,basft
	rnc			;gotcha, return Carry reset
	mvi	d,'H'
	call	gtdsn		;SN error if neither H nor A
	lxi	d,hexft
	mvi	a,1
	stc			;return Carry, A = 1 if HEX
	ret
gtfl0:	xchg			;FNAME addr to HL
	lxi	d,devf0		;default device name addr to DE
	call	cpyst		;copy :F0: to FNAME
	dcx	h		;HL points to next FNAME loc available
	xchg			;DE points to next
	call	gtcha		;get next char
	jmp	gtfl1		;and continue as above
gtchd:	stax	d
	inx	d
	jmp	gtcha

;routines for disk i/o

;DKOUT writes a character from C to the open disk file
dkout:	lxi	h,temp2
	mov	m,c		;character to temp2 for writing
isisw:	mvi	c,dkwrf
	lxi	d,wrblk
	jmp	isise		;write the char and return

;DLOAD reads char from disk for ASCII LOAD
dload:	call	drea1		;read char
	jnz	pop3		;return unless eof
	call	isisc		;close the file
	lxi	sp,stack-4	;omode and nexts return pushed
	pop	psw
	sta	omode		;restore omode
	jmp	dmod2		;and return to direct mode

;DREAD reads char for HEX LOAD
dread:	push3
	call	drea1
	jnz	pop3
	jmp	loadx		;eof read

;DREA1 is called from DLOAD and DREAD to read a disk char.
;Retn:	A	char read
;	BCDEHL	clobbered
;	Zero	set iff eof
drea1:	call	isisr		;read char to TEMP2
	lda	temp		;fetch actual count
	ora	a
	rz			;actual = 0, i.e. eof
	lda	temp2		;else fetch char read
	ani	7FH		;remove parity bit
	cpi	cntlz		;Zero set iff eof
	ret

;routines for HEX LOADing and SAVEing
;INTEL HEX format is a series of records, with all info in ASCII:
;frame 0	record mark ':' [3AH]
;frames 1-2	record length n, hex number 0-FFH [0 for eof;  here max=10H]
;frames 3-6	load address
;frames 7-8	record type [here 0]
;frames 9 - 8+2*n	data
;frames 9+2*n - 10+2*n	checksum  [negated sum mod 256 of preceding items]

;WASCI converts A3-A0 to ASCII and falls through to DWRIT to write
wasci:	ani	0FH		;00H, ..., 09H, 0AH, ..., 0FH
	adi	90H		;90H, ..., 99H, 9AH, ..., 9FH
	daa			;90H, ..., 99H, 00H+C,...,05H+C
	aci	40H		;D0H, ..., D9H, 41H, ..., 46H
	daa			;30H, ..., 39H, 41H, ..., 46H
	mov	c,a		;pass value to write through C
				;and fall through to DWRIT
dwrit:	push4
	call	dkout		;write the char
	jmp	pop4

;WBYTE writes byte from A as two ASCII bytes, updating checksum in D
wbyte:	push	psw
	rrc
	rrc
	rrc
	rrc
	call	wasci		;convert left nibble to ascii and write
	pop	psw
	push	psw
	call	wasci		;convert right nibble to ascii and write
	pop	psw
	add	d
	mov	d,a		;update checksum
	ret

;WADDR writes address from HL (subtracting loading bias), and record type.
waddr:	push	h
	push	d
	xchg			;address to DE
	lhld	temp		;-first to HL
	dad	d		;load address to HL
	pop	d
	mov	a,h
	call	wbyte		;write high byte
	mov	a,l
	call	wbyte		;write low byte
	xra	a
	call	wbyte		;write record type = 0
	pop	h
	ret			;and return

;WCSUM writes the checksum from D, followed by CR and LF.
wcsum:	xra	a
	sub	d
	call	wbyte		;write checksum
	mvi	c,cr
	call	dwrit
	mvi	c,lf
	jmp	dwrit		;write cr and lf and return

;RBYTE reads two ASCII bytes and builds binary char, updating checksum in D.
;Retn:	A	char read
;	C	clobbered
;	D	updated checksum
;	BEHL	preserved
;	Zero	set iff new checksum = 0
rbyte:	call	dread		;read a byte
	call	ishex		;convert ASCII to binary
	jc	loadx		;not an ASCII hex digit, abort
	rlc
	rlc
	rlc
	rlc
	mov	c,a		;high nibble to C
	call	dread		;read another
	call	ishex
	jc	loadx		;not ASCII hex digit
	ora	c		;form complete byte from nibbles
	mov	c,a		;and save
	add	d		;update checksum
	mov	d,a		;and checksum to D
	mov	a,c		;restore result to A
	ret

	endif			;end of NOT RTPAK conditional
	endif			;end of ISIS2 conditional


;end of ISIS2
	page
;NONST 08/06/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;routines for nonstandard version SAVE and LOAD, plus GTFIL
;includes custom EPSTN and GENMC versions


	if	nonst		;NONST versions

	if	(not camac) or (not rtpak)	;not for CAMAC RTPAK version
;DOIO is common routine to perform i/o operations in nonstandard version.
;Call:	A	offset of desired JMP from beginning of jump vector
;	E	shift count for finding desired iobyte field
conin:	xra	a
coni1:	push	d		;save DE
	mvi	e,1
doio:	push	h		;save HL
	lxi	h,jmpta		;jmp vector base address to HL
	call	adahl		;+offset = jmp address for device #0
	lda	iobyt		;i/o byte to A
doio1:	dcr	e
	jz	doio2		;shift no more
	rar
	jmp	doio1
doio2:	ani	3		;mask to desired field only
	mov	e,a		;save in E
	ral			;desired device # * 2
	add	e		;desired device # * 3
	call	adahl		; + base = jmp address for selected device
	pop	d		;saved HL to DE
	xthl			;saved DE to HL, device jmp address to stack
	xchg			;restore DE and HL
	ret			;branch to desired device driver
cnout:	mvi	a,12
	jmp	coni1
rdrin:	mvi	a,24
	push	d
	mvi	e,3
	jmp	doio
pout:	mvi	a,36
	push	d
	mvi	e,5
	jmp	doio
lout:	mvi	a,48
	push	d
	mvi	e,7
	jmp	doio
	endif			;end of NOT CAMAC or NOT RTPAK conditional

	if	compl
save	equ	uferr		;UF error in COMPL version
load	equ	uferr
	else

	if	(not epstn) and (not genmc) and (not bendx)
;normal NONST SAVE and LOAD
;SAVE <filename>
save:	call	prntm		;print SAVING message
	db	'SAVING', ' ' or 80H
	call	gtfil		;get file name
	if	packi
	mvi	c,cntlr
	call	pout		;TAPEON
	call	save7		;punch leading nulls
	endif
	if	not romsq
	lxi	d,srcad-2
	call	cplde
	lhld	eofad
	else
	lhld	sourc
	dcx	h		;first to HL
	push	h
	push	h		;and saved
	call	last		;last to HL
	pop	d		;first to DE
	call	cplde		;-first to DE
	inx	d		;-first + 1
	endif
	dad	d		;last - first + 1 = length
	push	h		;and saved
	lxi	h,headr
	mvi	e,headl		;header length to E
save1:	mov	c,m		;header char to C
	call	pout		;and out to punch device
	inx	h
	dcr	e
	jnz	save1		;keep sending header chars
	pop	d		;file length to DE
	if	romsq
	pop	h
	else
	lxi	h,srcad-1	;save pointer to HL
	endif
save2:	inx	d
	push	d		;save length+1
	mov	a,d
	ora	a		;zero set iff length < 255
	jz	save3
	mvi	e,0
save3:	dcr	e		;length of block to E
	mvi	c,stbyt
	call	pout		;send start  byte
	mvi	c,tybyt
	call	pout		;send type byte
	mov	c,e
	call	pout		;send length byte
	mov	a,e
	ora	a		;check if length = 0
	jz	save5		;yes, done
	call	ctest		;check for console break char
	mvi	d,0		;checksum in D
save4:	mov	c,m
	call	pout		;send source char
	mov	a,m
	add	d
	mov	d,a		;update checksum
	inx	h
	dcr	e
	jnz	save4		;send more source chars
	mov	c,d
	call	pout		;send checksum
save5:	pop	d		;recover length + 1  to DE
	mov	a,d
	ora	a
	if	packi
	jz	save6
	else
	rz			;done if length < 255
	endif
	dcr	d		;else new length = length+1-256 = length-255
	jmp	save2		;and save more blocks
	if	packi
save6:	call	save7		;punch trailing nulls
	mvi	a,cntlt
	jmp	writc		;TAPEOFF and return
save7:	lxi	b,(64 shl 8)	;64 to B, 0 to C
save8:	call	pout		;punch a null
	dcr	b
	jnz	save8		;punch more nulls
	ret
	endif


;LOAD <filename>
load:	if	romsq
	call	issrc		;must be addressing working space
	endif
	lxi	h,headr+2	;first filename char address to HL
	lxi	b,8		;0 to B, # filename chars to C
	call	fillm		;fill filename with 0s
	call	prntm		;print LOADING message
	db	'LOADING', ' ' or 80H
	call	gtcho		;look at next char
	call	dtest		;check if delimiter
	cc	gtfil		;get file name and set up header if not
	if	packi
	mvi	c,cntlq
	call	pout		;XON to turn on RDR
	endif
load1:	lxi	h,headr
	mvi	c,headl
	call	ctest		;check for console break char
;NB Tarbell needs different startup
;lxi h,headr+2 and mvi c,headl-2 above
;mvi a,10H and out 6EH here
load2:	call	rdrin		;read a char
	cmp	m		;compare to header char
	jz	load3		;matched, try next
	mov	a,m		;else fetch header char
	ora	a		;check if null, i.e. LOAD <cr> typed
	jnz	load1		;not null, try again from the top
load3:	inx	h
	dcr	c
	jnz	load2		;see if next matches too
	call	new		;got the file header, prepare to load
	lxi	h,srcad-1	;load address to HL
load4:	call	rdrin		;read start byte
	cpi	stbyt		;check if start byte
	jnz	cserr		;issue CS error if not
	call	ctest		;check for console break char
	call	rdrin		;read type byte
	cpi	tybyt		;check if type byte
	jnz	cserr		;issue CS error if not
	call	rdrin		;read length byte
	ora	a
	if	packi
	jz	load6		;XOFF before exiting in Packard version
	else
	jz	dmodx		;block length 0, done
	endif
	mov	e,a		;block length to E
	inr	a
	push	psw		;save length+1
	mvi	d,0		;checksum to D
load5:	call	rdrin		;read a char
	mov	m,a		;store it
	inx	h
	add	d
	mov	d,a		;update checksum
	dcr	e
	jnz	load5		;more chars in block
	call	rdrin		;read the checksum
	cmp	d
	jnz	cserr		;checksum error
	pop	psw		;recover block length+1
	jz	load4		;length was 255, so load more blocks
	dcx	h		;point to new eof adress
	if	packi
load6:	mvi	c,cntls
	call	pout		;XOFF to turn off RDR
	endif
	jmp	dmodx		;reset stacks and continue iff direct
cserr:	call	new		;erase the garbage
	if	packi
	mvi	c,cntls
	call	pout		;XOFF to turn off RDR
	endif
	error	f, C, S		;fatal CS error
	endif			;end normal NONST conditional

	if	(not epstn) and (not bendx)	;NONST GTFIL, including GENMC
;the nonstandard version of GTFIL gets a file name and initializes header block
gtfil:	call	gtfl4		;skip the open quote
	if	not genmc
	lxi	b,headr+2	;point to filename location with BC
	else
	lxi	b,headr+3
	endif
	call	gtalp		;get first char
	jc	snerr
	mvi	d,8		;max char count to D
gtfl1:	stax	b		;store a filename char in header
	inx	b
	if	not genmc
	call	writc		;and echo to console
	endif
	call	gtild
	jc	gtfl5		;no more chars, pad with spaces
gtfl2:	dcr	d
	jnz	gtfl1		;store another
gtfl3:	call	gtild
	jnc	gtfl3		;scan remaining chars, if any
gtfl4:	mvi	d,'"'
	jmp	gtdsn		;get quote mark and return
gtfl5:	mvi	a,' '
gtfl6:	dcr	d
	jz	gtfl4
	stax	b
	inx	b
	jmp	gtfl6
	endif


	if	epstn		;EPSTN versions

;SAVE <filename>
save:	lxi	h,savma		;SAVING message address to HL
	call	gtfil		;get file name
	if	not romsq
	lxi	b,srcad-1
	else
	call	first
	endif			;first addr to BC
	call	wrtbh		;write the start address in hex
	mvi	a,'-'
	call	writc		;write a -
	if	romsq
	call	last		;last addr to BC
	else
	lhld	eofad
	mov	b,h
	mov	c,l
	endif
	call	wrtbh		;write BC as hex number
save1:	call	prntm		;print escape sequence
	db	cr, escap, escap or 80H
	ret			;and return
savma:	db	cr, lf, escap, 'DSAV', ' ' or 80H

;LOAD <filename>
load:	if	romsq
	call	issrc		;must be addressing working space
	endif
	lxi	h,lodma
	call	gtfil		;get file name and set up header
	call	new		;got the file, prepare to load
	call	save1		;write escape sequence
	if	romsq
	call	last
	else
	lxi	d,-1
	call	findl
	mov	a,m
	call	adahl
	endif			;last addr to HL
	jmp	dmodx		;reset stacks and continue iff direct
lodma:	db	cr, lf, escap, 'DLOD', ' ' or 80H

gtfil:	push	h		;save message address
	mvi	d,'"'
	call	gtdsn		;skip open quote
	lxi	d,bufad-1	;destination-1 to DE
	call	gtalp		;look for alpha first char
	jc	snerr		;SN error if none
	mvi	c,5		;max char count to C
gtfl1:	inx	d
	stax	d		;store a filename char
gtfl2:	call	gtild		;look for following letter or digit
	jc	gtfl3		;done if none
	dcr	c		;else decrement count
	jp	gtfl1		;and store the char
	jmp	gtfl2		;or scan and ignore remaining chars
gtfl3:	ldax	d		;recover last filename char
	ori	80H		;turn on high bit
	stax	d		;and replace
	mvi	d,'"'
	call	gtdsn		;skip close quote
	pop	h		;restore message addr
	call	prtst		;print it
	lxi	h,bufad
	call	prtst		;print the filename
	call	prntm		;print filetype XYB
	db	'.XYB', ' ' or 80H
	ret			;and return

;WRITH writes (A3-A0) as a hex digit, masking off A7-A4.
;WRTBH writes (BC) as hex word, currently as #dddd without suppressing zeroes.
writh:	ani	0FH		;00H, ... , 09H, 0AH, ... , 0FH
	adi	90H		;90H, ... , 99H, 9AH, ... , 9FH
	daa			;90H, ... , 99H, 0H+C,... , 5H+C
	aci	40H		;D0H, ... , D9H, 41H, ... , 46H
	daa			;30H, ... , 39H, 41H, ... , 46H
	jmp	writc		;write and return

wrtbh:	push	d
	mvi	d,255
wrtw1:	mov	a,b
	rrc
	rrc
	rrc
	rrc			;rotate right four places
	call	writh		;write MS four bits
	mov	a,b
	call	writh		;write LS four bits
	inr	d
	mov	b,c
	jz	wrtw1
	pop	d
	ret

	endif			;end of EPSTN conditional

	if	genmc		;GENMC versions SAVE and LOAD
;SAVE <filename>
save:	call	prntm		;print SAVING message
	db	'SAVING', ' ' or 80H
	call	gtfil		;get file name
	lxi	h,headr+3
	call	prtst		;echo the <filename>
	lda	omode
	push	psw		;save current OMODE
	mvi	a,80H
	sta	omode		;reset OMODE for ASCII saving
	lxi	h,headr
	call	prtst		;save header
	if	romsq
	lhld	sourc
	else
	lxi	h,srcad		;first prog addr to HL
	endif
	lxi	b,-1
	call	list1		;do the ASCII save
	mvi	c,cntlz
	call	pout		;and write an eof
	pop	psw
	sta	omode		;restore OMODE
	ret

;LOAD [<filename>]
load:	if	romsq
	call	issrc		;illegal if not addressing working space
	endif
	call	prntm		;print LOADING message
	db	'LOADING', ' ' or 80H
	call	gtcho		;look at next char
	call	dtest		;test if delimiter
	jnc	load5		;no <filename>, just load next
	call	gtfil		;get <filename> to header
;LOAD1 looks for <filename> matching header
load1:	lxi	h,headr
	mvi	c,headl-1
	call	loadh		;look for matching filename from RDR
	jnz	load1		;does not match <filename>
	call	rdrch		;read last, should be <lf>
	cpi	lf
	jnz	load1		;no match
;at LOAD2 the approriate header has been found, so file is loaded
load2:	lxi	h,headr+3
	call	prtst		;echo the <filename>
	call	new		;erase old program
	mvi	a,7FH
	sta	omode		;reset OMODE for ASCII loading
load3:	call	gtlin
	call	tkize
	jc	load3
	cnz	addln
	jmp	load3
;LOAD5 gets the next file, regardless of filename
load5:	lxi	h,headr
	mvi	c,3
	call	loadh		;look for <cr> <lf> ' from RDR
	jnz	load5		;not found, try again
	mvi	c,8		;now scan eight <filename> chars
load6:	call	rdrch
	mov	m,a		;save the char
	inx	h
	dcr	c
	jnz	load6
	mvi	c,5
	call	loadh		;look for .BAS <cr>
	jnz	load5		;no match
	call	rdrch		;read <lf>
	cpi	lf
	jnz	load5
	jmp	load2		;found good header, load it
;LOADH looks for (C) chars from RDR
;Retn:	Zero	Set iff (C) chars match string addressed by HL
loadh:	call	rdrch		;read a char
	cmp	m
	rnz			;no match, return Zero reset
	inx	h
	dcr	c
	jnz	loadh		;look at next
	ret			;match, return Zero set

dkout	equ	pout		;write to PUN device

rdrch:	call	ctest		;check for console break char
	call	rdrin		;read from reader
	ani	7FH		;mask off parity
	jz	rdrch		;ignore nulls (ASCII 0s)
	cpi	cntly		;check if <control-Y>
	rnz
	error	f, E, F		;fatal EF error
dload:	call	rdrch
	cpi	cntlz		;look for eof
	jnz	pop3
	jmp	dmod2		;return to DMODE if eof

	endif			;end of GENMC conditional

	endif			;end of NOT COMPL conditional
	endif			;end of NONST conditional


;end of NONST
	page
;PARSING 8/26/79
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979 by Mark Williams Company, Chicago
;parsing routines

;Register use for all parsing routines, except as noted.
;Call:	(textp)	address of next text char
;Retn:	A	clobbered
;	BC,DE	preserved
;	HL	address of next unparsed char
;	(textp)	ditto
;	Carry	set iff failure

;GTCHA fetches the next nonspace character.
gtcha:	lhld	textp
gtch1:	mov	a,m		;fetch character
	inx	h		;point to next
	cpi	' '
	jz	gtch1		;try again if space
	shld	textp		;store new text pointer
	ret

;GTCHO looks ahead to the next nonspace char.
gtcho:	call	gtcha
bakup:	dcx	h		;back up textp
	shld	textp
	ret

;GTALP fetches the next char if alphabetic, returns Carry if not.
gtalp:	call	gtcho
	call	isalp
	rc			;not a letter
read1:	inx	h
	shld	textp		;else read it
	ret

;GTCOM gets a comma.
gtcom:	call	gtcha
	cpi	','
	rz
	jmp	bkupc		;back up textp and return Carry

;GTCND gets a comma not followed by a delimiter.
gtcnd:	call	gtcom
	rc			;no comma
	call	dtst0		;look for delimiter
	cmc			;carry set iff delimiter follows comma
	ret

;GTD returns carry reset and moves up textp if next char matches (D), else
;returns carry set and leaves (textp) unchanged.
;GTDSN gets a char which must match (D), else SN error is issued.
;GTDTR does a GTDSN and echoes the char if trace print is desired.
gtd:	call	gtcha
	cmp	d
	rz
bkupc:	stc			;return carry set if no match
	jmp	bakup		;back up textp and return
	if	not compl
gtdtr:	lda	inlhs
	ora	a
	jz	gtdsn		;do not echo unless INLHS
	lda	vtrac
	ral			;carry set iff trace print desired
	mov	a,d
	cc	writc		;write it if so
	endif			;and fall through to GTDSN
gtdsn:	call	gtd
	rnc
	jmp	snerr
	if	compl
gtdtr	equ	gtdsn
	endif
gtreq:	mvi	d,')'
	call	gtdsn		;skip ) and fall through to skip =
gtequ:	mvi	d,eqult
	jmp	gtdsn		;skip = token

;GTDEL scans text until delimiter is found.
gtdel:	call	dtst0		;test if current is delimiter
	rnc			;delimiter, done
	call	read1		;no, read the current
	cpi	'"'
	cz	gtclq		;watch for quoted strings
	jmp	gtdel		;and try the next

;GTCLQ scans to close quote or <cr>.
;Call:	HL	next char addr
;Retn:	HL	addr of " or cr
;	textp	reset
gtclq:	mov	a,m		;fetch next
	cpi	cr
	rz			;done if cr
	call	read1		;else read it
	cpi	'"'
	jnz	gtclq		;and keep reading if not close "
	dcx	h		;point to "
	ret

;ISDIG presrves A and returns Carry set iff (A) is not ASCII digit.
isdig:	cpi	'0'
	rc			;too small
	cpi	'9'+1
	cmc
	ret

;ISHEX converts an ASCII hex digit to binary.
;Both upper and lower case alphabetic characters are allowed.
;Call:	A	character
;Retn:	Carry	set iff not a hex digit
;	A	binary value if Carry reset, clobbered if Carry
;	BCDEHL	preseved
ishex:	sui	'0'		;subtract bias
	cpi	'G'+20H-'0'
	cmc
	rc			;return Carry set if < '0' or > 'f'
	cpi	10
	cmc			;Carry reset for '0' to '9'
	rnc			;decimal digit
	ani	1FH		;mask to convert lower case to upper
	sui	'A'-'0'		;'A' becomes 0, 'B' becomes 1...
	cpi	6
	cmc
	rc			;return Carry if not legal hex digit
	adi	10		;'A' becomes 10, 'B' becomes 11...
	ret

;GTILD gets the next letter or digit without skipping spaces, and is
;called by GTNAM and GTFIL to build a variable or file name.
;Call:	HL	address of next char to parse
;Retn:	A	next char
;	BC,DE	preserved
;	HL	incremented
;	(textp)	value of HL when called
;	Carry	set iff next not letter or digit
gtild:	mov	a,m
	shld	textp		;leave textp set for failure
	inx	h
ldtst:	call	isdig
	rnc			;digit
isalp:	cpi	'A'
	rc			;neither
	cpi	'Z'+1
	cmc			;carry iff not letter
	ret

;GTDEC scans an unsigned decimal integer and returns its value.
;An unsigned decimal integer is a string of decimal digits.
;The digit count (not including leading 0s) is used for overflow detection.
;1-4 digits indicates no overflow.
;6+ digits indicates overflow.
;5 digits indicates overflow iff (value of 4 digits)*10 > 65535 or
;	(value of 4 digits)*10 + (value of digit 5) > 65535.
;	Note that 9999*5 is always < 65535.
;Retn:	Carry	set iff no decimmal digit
;	Zero	set if overflow, i.e. integer > 65535
;	A	clobbered
;	BC	preserved
;	DE	integer value 0 <= n <= 65535 if Carry and Zero reset
;	DE	0 if Carry or Zero
;	HL	address of next nondigit
;GTDE0 is called from GTDEC to scan through leading 0s.
gtde0:	inx	h
	shld	textp		;read leading 0
	mov	a,m		;fetch next char
	sui	'0'
	jz	gtde0		;another leading 0
	cpi	10		;carry set iff decimal digit
	jc	gtde1		;digit follows leading 0s
retnc:	ora	a		;nondigit, return C,Z reset and result 0
	ret
gtdec:	lxi	d,0		;default value to DE
	call	gtcho		;next char to A, textp to HL
	sui	'0'		;subtract ASCII bias
	jz	gtde0		;strip off leading 0s
	cpi	10
	jc	gtde1		;digit
	mov	a,m		;refetch next
	ora	a		;clear Zero
	stc			;and set Carry
	ret
gtde1:	mov	e,a		;first digit value to DE
	push	b		;save BC
	mvi	b,-6 and 255	;-(max # digits + 1) to B
gtde2:	inr	b		;# digits read - max # - 1
	inx	h
	mov	a,m		;fetch next char
	sui	'0'		;subtract ASCII bias
	cpi	10
	jnc	gtde3		;nondigit, done
	push	h		;save textp
	mov	h,d
	mov	l,e		;copy value to HL
	call	hl10a		;10 * old value + current digit = new value
	xchg			;to DE
	pop	h		;restore textp
	jnc	gtde2		;get next if no overflow
	inr	b		;bump digit count to assure > 5 on overflow
	jmp	gtde2		;and get next char
gtde3:	shld	textp		;update textp
	mov	a,b		;fetch digit count - max - 1
	pop	b		;restore BC
	ora	a
	rm			;count <= max #, return both C and Z reset
	xra	a		;overflow, return C reset and Z set
	mov	d,a
	mov	e,a		;and 0 in DE
	ret

;GTLNO returns a legal line # in DE, using GTDEC.
;Brances to fatal US error if GTDEC returns overflow or 0.
;Otherwise 	(1) nonexistent or 	(2) 0 < n < 2 ^ 16.
;Retn:	A	next non-space char	clobbered
;	BC	preserved		preserved
;	DE	0			n
;	HL	addr of next nondigit	address of next nondigit
;	Carry	set			reset
gtlno:	call	gtdec		;look for decimal integer
	rc			;none, return Carry
	mov	a,d
	ora	e
	rnz			;legal line #, return Carry reset
userr:	error	f, U, S		;fatal US error -- illegal line #

;GTLIT gets a literal, and is called by EVAL, READ, INPUT and VAL.
;The literal may be preceeded by optional spaces and untokenized + and - signs.
;Call:	HL	(textp)
;Retn:	Carry	set iff no literal found
;	A	type token
;	BC	value if integer
;	C,DE	value if string
;	(TEMP)BCD	value if floating
gtlit:	mov	a,m		;fetch next
	inx	h		;and point to following
gtlt1:	push	psw
	xra	a
	sta	temp		;sign = positive
	pop	psw
gtlt2:	call	isdig		;check if char is digit
	jnc	gtlid
	if	float
	cpi	'.'
	jz	gtlif		;get floating literal
	endif
	shld	textp		;else read the char
	if	strng
	cpi	'"'
	jz	gtlis		;get string literal
	endif
	lxi	b,intst shl 8	;overflow flag to B, 0 to C
	mov	d,c
	mov	e,c		;embryo value 0 to DE
	cpi	'#'
	jz	gtlih		;get hex literal
	cpi	'&'
	jz	gtlib		;get binary literal
	cpi	' '
	jz	gtlt3		;leading space -- ignore
	cpi	'+'
	jz	gtlt3		;unary +
	cpi	'-'		;check for unary -
	jnz	bkupc		;else back up TEXTP and return Carry
	lda	temp		;unary minus -- complement sign
	cma
	sta	temp
gtlt3:	mov	a,m
	inx	h		;point to next
	jmp	gtlt2		;and keep looking
;get a string literal, i.e. quoted string
	if	strng
gtlis:	push	h		;save string location
	call	gtclq		;scan to close " or cr
	pop	d		;location to DE
gtls1:	mov	a,l		;last+1 -- EVUNQ entry point
	sub	e		;last+1-first = length
	mov	c,a		;to C
	mvi	a,strst		;type = string
	ora	a		;return Carry reset
	ret
	endif
;get binary integer literal
gtlib:	xchg			;textp to DE, value to HL
gtlb1:	ldax	d		;fetch char
	sui	'0'		;subtract ASCII bias
	cpi	2
	jnc	ovtst		;not a binary digit, done
	inx	d
	inr	c		;bump count
	dad	h		;shift value left one bit
	cc	ovset		;set overflow flag
	rar			;current digit to Carry
	jnc	gtlb1		;try next
	inx	h		;add current digit to value
	jmp	gtlb1		;and try next
;get hex integer literal
gtlih:	xchg			;textp to DE, value to HL
gtlh1:	ldax	d		;fetch next
	call	ishex
	jc	ovtst		;not a hex digit, done
	inx	d
	inr	c		;bump count
	push	psw
	mov	a,h
	ani	0F0H		;Zero reset if overflow will occur
	cnz	ovset		;set overflow flag
	pop	psw		;restore current nibble
	dad	h
	dad	h
	dad	h
	dad	h		;value left four bits
	ora	l
	mov	l,a		;add in current nibble
	jmp	gtlh1
;common exit for binary and hex integer literals
ovtst:	xchg			;textp to HL
	shld	textp		;and reset
	mov	a,c		;digit count to A
	ora	a
	jz	bkupc		;no digits after # or &, return Carry
	mov	a,b		;fetch result type
	mov	b,d
	mov	c,e		;result to DE
	cpi	intst
	rz			;return type integer if no overflow
;issue nonfatal OV error and return max integer value in BC
iover:	error	n, O, V		;else issue nonfatal OV error
	lxi	b,7FFFH		;max positive value to BC
	mvi	a,intst		;result is integer
	ora	a		;carry reset
	ret
ovset:	dcr	b
	ret
;get numeric literal with first char digit
gtlid:	push	h		;save textp
	call	gtdec		;get decimal number
	mov	a,m		;fetch next char
	pop	h		;old textp to HL
	if	float
	jz	gtlif		;get floating literal if too big
	cpi	'.'
	jz	gtlif		;or if next is .
	cpi	'E'
	jz	gtlif		;or if next is E
	cpi	'E'+20H
	jz	gtlif		;also allow lower case e in case untokenized
	mov	a,d
	ora	a
	jm	gtlif		;or if value is > 32767 but < 65536
	else
	jz	iover		;OV error if too big in nonfloating version
	endif
	lda	temp		;fetch sign
	ora	a
	cnz	cplde		;complement value if negative desired
	mov	b,d
	mov	c,e		;value to BC
	mvi	a,intst
	rnc			;return unless cannot complement
	if	float
gtlif:	dcx	h
	call	finp		;get floating literal
	sta	temp		;save A
	mvi	a,sngst
	ora	a		;return Carry reset
	ret
	else
	jmp	iover
	endif

;GTNAM gets a variable name into buffer at BUFAD, its type into VARTY,
;	and its length into A.
;A name is <letter> [<letter> | <digit>]* [$ | <exclam> | %] without spaces.
;After MAXNL characters, additional chars are scanned but ignored.
;The first letter determines the variable type according to TYBUF,
;	unless the trailing character $ | <exclam> | % is specified.
;Call:	(textp)	current text pointer
;Retn:	A	length of variable name
;	HL	address of first char after name
;	(bufad)	symbol name string
;	(varty)	variable type
;	Carry	set iff no name, i.e. first char not letter
gtnam:	call	gtalp		;get letter
	rc
	push	b
	push	d
	mov	b,a		;save first char
	if	strng or float	;must find var type if noninteger version
	xchg			;save HL in DE
	lxi	h,tybuf-'A'
	call	adahl		;address default type byte
	mov	a,m		;fetch it
	xchg			;restore HL
	else
	mvi	a,intst		;else type is integer
	endif
	sta	varty		;store type in VARTY
	mov	a,b		;restore char
	lxi	b,bufad
	mvi	d,1		;char count to D
gtnm1:	stax	b		;store a char
gtnm2:	call	gtild		;get next char
	jc	gtnm3		;no more
	mov	e,a		;save new char
	mov	a,d
	cpi	maxnl		;compare count to max name length
	jnc	gtnm2		;count >= length, so don't insert
	mov	a,e		;restore char
	inx	b
	inr	d		;bump count and pointer
	jmp	gtnm1		;and insert
gtnm3:	cpi	'%'
	cz	gtnm4		;type is integer
	if	float
	cpi	'!'
	cz	gtnm6		;type is floating
	endif
	if	strng
	cpi	'$'
	cz	gtnm7		;type is string
	endif
	if	camac
	cpi	'#'
	cz	gtnm8
	endif
	ldax	b
	ori	80H
	stax	b		;end of string bit on
	mov	a,d
	pop	d
	pop	b
	ret
gtnm4:	mvi	a,intst		;integer token to A
gtnm5:	shld	textp		;move textp past trailing char
	inx	h
	sta	varty		;reset VARTY to specified type
	xra	a		;clear A for following compares
	ret
	if	float
gtnm6:	mvi	a,sngst		;single precision token to A
	jmp	gtnm5
	endif
	if	strng
gtnm7:	mvi	a,strst		;string token to A
	jmp	gtnm5
	endif
	if	camac
gtnm8:	mvi	a,camst
	jmp	gtnm5
	endif

;FDVAR looks for variable name, returns carry set if none.
;If var not found in symbol table, assumes its type to be simple and
;builds a new symbol table entry for it.
;Retn:	DE	entry type address
;	HL	address following name in entry
fdvar:	call	gtnam
	rc
	mov	c,a		;save length
	call	stlku		;look up symbol in symbol table
	rnc			;found it
fdva1:	lda	varty		;fetch var type -- unary user fn entry point
	if	camac
	cpi	camst		;check if camvar
	jz	snerr		;SN error if undefined camvar
	endif
	call	bytsd		;# bytes per entry to DE
	xchg			;and to HL
	mvi	b,0		;length to BC
	inx	h		;+length
	inx	h		;+type
	inx	h		;+dims
	dad	b		;entry length to HL
	call	stpsh		;build the entry
	dcx	d		;DE points to type byte
	mvi	m,0		;zero the dim byte
	ret

;GTVAR performs a variable reference.
;Gets a variable name from text, returns with carry set if none.
;If variable name not found in symbol table, assumes its type to be simple
;and builds a new symbol table entry for it.
;Prints trace info if INLHS and either TRACE or variable break bit on,
;and sets VTRAC accordingly.
;Issues BS error if subscript out of bounds.
;Retn:	A	type token of referenced variable
;	DE	type byte address of referenced var
;	HL	address of referenced variable
;	Carry	set iff no name found
;	(vtrac)	bit 7 set iff trace desired
gtvar:	call	fdvar
	rc			;no name found
	if	not compl
	lda	inlhs
	ora	a
	cnz	trset		;set VTRAC if in lhs
	endif
	mov	a,m		;fetch # dims
	mov	c,a		;and save in C
	inx	h
	ora	a		;zero set iff simple var
	lda	varty		;value to return to A
	rz			;simple var
	if	camac
	cpi	camst		;check if camvar reference
	jnz	gtva0		;no
	mvi	a,intst		;yes, value of camvar ref is integer
gtva0:	ora	a		;reset Carry for return
	endif
	push	psw		;save value to return
	push	d
	call	bytsd		;find bytes per entry
	mov	b,e		;and save in B
	push	b		;save bytes per entry and # dims
	lxi	b,1		;product to BC
	lxi	d,0		;sum to DE
	push	d		;and saved
	mvi	d,'('
;GTVA1 is executed for each subscript of the variable.  The stack contains
;the result type, type byte address, bytes per entry/# dimensions, and sum
;of subscripts thus far.  BC contains the product of bounds thus far, and
;HL points to the next bound.
gtva1:	push	h		;save pointer
	call	gtdtr		;skip (
	pop	h		;restore pointer
	mov	e,m
	inx	h
	mov	d,m		;next bound to DE
	inx	h		;point to next
	xthl			;save pointer, sum to HL
	push	b		;save product
	push	d		;save dim
	push	h		;save sum
	push	b		;save product
	call	gtsub		;get subscript to BC
	pop	d		;product to DE
	call	mulbd		;subscript * product to HL
	pop	d		;sum to DE
	dad	d		;new sum = sum + subscript * product to HL
	pop	d		;bound to DE
	pop	b		;product to BC
	inx	d		;bound+1
	push	h		;save sum
	call	mulbd		;new product = product * (bound+1) to HL
	pop	d		;sum to DE
	xthl			;product to stack, pointer to HL
	pop	b		;product to BC
	xthl			;pointer to stack, counts to HL
	dcr	l		;decrement # dims count
	jz	gtva2		;done
	xthl			;save counts, restore pointer
	push	d		;save sum
	mvi	d,','
	jmp	gtva1		;and do some more dimensions
gtva2:	mov	a,h		;bytes per entry to A
	pop	h		;pointer to HL
gtva3:	dad	d		;add sum once for each value byte
	dcr	a
	jnz	gtva3		;pointer * sum = location
	push	h
	mvi	d,')'
	call	gtdtr		;skip )
	pop	h
	pop	d		;restore type byte address
	pop	psw		;and value to return
	ret

;GTSUB gets a subscript expression, issuing nonfatal BS error if out of bounds.
;The subscript value is printed if VTRAC is negative.
;Call:	DE	bound
;Retn:	BC	subscript, 0 <= (BC) <= (DE)
gtsub:	if	not compl
	lda	inlhs
	ora	a		;check if INLHS
	jm	gtsu2		;yes -- fix so no extraneous tracing
	endif
gtsu0:	push	d		;save bound
	lxi	d,stack+stakm+10-stakl
	call	cplde
	xchg
	dad	sp		;number of bytes left to HL
	jnc	bserr		;fatal BS error if too little room left
	call	gtexp		;expr to BC
	lxi	d,0
	mov	a,b
	ora	a		;check sign of subscript
	cm	gtsu1		;negative
	pop	d		;restore bound
	call	cmbds
	rc			;< bound
	rz			;= bound
gtsu1:	mov	b,d
	mov	c,e		;replace expr with bound
	error	n, B, S		;nonfatal BS error and return
	if	not compl
gtsu2:	xra	a
	sta	inlhs		;reset INLHS
	call	gtsu0		;get the subscript
	mvi	a,255
	sta	inlhs		;turn INLHS back on
	lda	vtrac
	ora	a
	rp
	jmp	wrtbu		;print subscript val if tracing
	endif
bserr:	error	f, B, S		;fatal BS error

;GTIVA is called from SCALL to perform an integer variable reference.
;Retn:	Carry	Set if not integer var ref
;	BC	value of integer var
;	DE	preserved
gtiva:	push	d
	call	gtvar		;look for var ref
	pop	d
	rc			;not found
	mov	c,m
	inx	h
	mov	b,m		;value to BC
	if	strng or float
	cpi	intst
	rz			;return Carry reset if integer
	stc			;and Carry set if not
	endif
	ret

	if	not compl
;TRSET determines if trace printing is desired, setting VTRAC accordingly
;and echoing the variable name if so.
;A trailing type char is printed if the type is not the default type.
;Call:	DE	addr of var type byte
;Retn:	A	clobbered
;	BC,DE,HL	preserved
;	Carry	reset
;	(vtrac)	minus iff trace printing desired
trset:	push	h
	lhld	trace		;INLHS to H, TRACE to L
	ldax	d		;type byte to A -- A7 set iff var break
	ora	l		;minus iff var break or TRACE on
	ana	h		;minus iff tracing desired
	sta	vtrac
	jp	trst1		;done if not tracing
	call	bprnt		;echo line number if not done already
	mov	h,d
	mov	l,e
	inx	h		;name addr to HL
	if	strng or float
	push	h		;and saved
	call	prtst		;print it
	pop	h		;restore addr
	mov	a,m		;refetch first char
	ani	7FH		;mask off possible high bit
	lxi	h,tybuf-'A'
	call	adahl		;address default type buffer location
	ldax	d		;fetch type byte
	ani	1FH		;mask to type
	cmp	m		;compare to default
	jz	trst1		;same as default, no trailing char
	call	tycha		;type char to H
	mov	a,h
	call	writc		;and printed
	else			;NOT STRNG and NOT FLOAT
	call	prtst		;just print the name
	endif
trst1:	pop	h
	ret

;TYCHA returns type char in H for type in A.
	if	strng or float
tycha:	if	strng
	mvi	h,'$'
	cpi	strst
	rz			;string
	endif
	if	float
	mvi	h,'!'
	cpi	sngst
	rz			;floating
	endif
	if	camac
	mvi	h,'#'
	cpi	camst
	rz			;camvar
	endif
	mvi	h,'%'
	ret			;integer
	endif			;end of STRNG or FLOAT conditional
	endif			;end of NOT COMPL conditional

;CMPST compares the strings at (DE) and M.  Zero set iff match.
cmpst:	ldax	d
	cmp	m
	inx	d
	inx	h
	rnz			;return zero reset if no match
	ora	a
	jp	cmpst		;keep comparing
	xra	a		;match -- set zero and return
	ret

;CPYST copies a string from (DE) to M.
cpys0:	lxi	d,bufad
cpyst:	ldax	d
	mov	m,a
	inx	d
	inx	h
	ora	a
	jp	cpyst
	ret


;end of PARSING
	page
;SDISK 1/12/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;statement routines for CP/M SDISK version


	if	cpm and sdisk

;The sequential disk file block working space is located at the
;top of RAM, between (STRT) and (FILET).  Location FILES contains the
;number of file blocks currently allocated.

;Each potential sequential disk file has a 166 byte file block, containing:
;1	File #, 0 if unused and nonzero if OPEN
;2	Mode, 0 for Output and nonzero for Input
;3	Count, 0-7FH indicating last position written/read in current buffer
;4	Column, initially 0, ignored for Input
;5	Width, initially 72, ignored for Input
;6-38	File control block (33 bytes)
;39-166	Buffer (128 bytes)


	if	rtpak		;EQUates for C3885 RTPAK version
dclr0	equ	uferr
close	equ	uferr
dinp0	equ	uferr
open	equ	uferr
dprin	equ	uferr
eoffn	equ	uferr
	else			;NOT RTPAK

;CLEAR @<expr>
dclr0:	inr	a		;clear @0 means files=1, etc.
	jz	bferr		;CLEAR @ 255 not allowed
	mov	c,a		;desired # files to C
	call	closn		;close any open files
	lda	files		;current #
	sub	c
	jp	dclr2		;need fewer, so skip memory check
	lxi	d,-filen
	lhld	symta
dclr1:	dad	d
	inr	a
	jnz	dclr1		;leave room for additional file
	xchg			;new symta to DE
	lhld	eofad
	call	cmdhu
	jc	omerr		;not enough space, fatal OM error
dclr2:	lhld	strt
	xchg
	call	cplde		;- top of string space to DE
	lhld	memt
	dad	d		;-current amount of string space to HL
	push	h		;and saved
	mov	a,c
	sta	files		;save new file max
	lhld	filet		;top of file space to HL
	lxi	d,-filen	;space per file to DE
dclr3:	dcr	a
	jz	dclr4
	mvi	m,0		;initialize empty file space
	dad	d
	jmp	dclr3
dclr4:	shld	strt		;store new top of string space
	pop	d
	dad	d		;subtract string space needed
	jmp	cle0a		;and initialize symbol table, etc.

;CLOSE [@<expr>, ...]
close:	call	gtsfn		;look for file #
	jc	closn		;none, close all
	jz	bferr		;DK error if @0 closed
	mov	h,b
	mov	l,c
clos1:	ora	a
	jnz	closi		;close file open for input
	mov	a,m		;fetch count
	sui	80H
	mov	b,a		;eof count to B
	mvi	c,cntlz
clos2:	push	b
	push	h
	call	dwrtc		;write an eof
	pop	h
	pop	b
	inr	b
	jnz	clos2		;write more eofs
closi:	inx	h
	inx	h		;point to file number
	mvi	m,0		;reset to 0
	lxi	d,-37
	dad	d
	xchg			;fcb address to DE
	mvi	c,dkclf
	if	c3885 and not debug
	call	bdos1		;BDOS call with interrupts disabled
	else
	call	bdos
	endif
	cpi	255
	rnz
	jmp	dkerr		;close error
closn:	call	sfils
clon1:	dcr	b
	rz			;all closed
	push	b
	push	d
	push	h		;save all
	mov	a,m		;fetch file number
	ora	a
	dcx	h
	mov	a,m		;fetch type
	dcx	h		;point to count
	cnz	clos1		;close a file
	pop	h
	pop	d
	pop	b		;restore
	dad	d		;point to next
	jmp	clon1		;and see if more to close

;DIR [<ambiguous filename>]
	if	not c3885
dircm:	call	gtaf5		;set filename to *.*
	xra	a
	sta	fcbad		;set disk to @
	sta	fcbcr
	call	dtst0		;look for delimiter
	cc	gtafn		;get desired ambiguous filename if present
	mvi	c,17
dirc1:	call	ctest		;look for console break char
	call	wcrlf		;write crlf
	call	bdosf		;search for next occurrence of filename
	cpi	255
	rz			;no more
	ani	3
	rrc
	rrc
	rrc
	adi	81H
	mov	e,a
	mvi	d,0		;address of filename to DE
	mvi	c,8
	call	prstr		;print the filename
	call	wrtsp		;and print a space
	mvi	c,3
	call	prstr		;and print filetype
	mvi	c,18		;look for next
	jmp	dirc1
	endif

;INPUT @<expr>, <var list>
dinp0:	push	psw
	call	gtcom		;skip trailing comma
	pop	psw
	jz	inpu1		;INPUT @0, normal INPUT with no quoted string
	ora	a
	jz	fmerr		;FM error if file open for Output
	push	b		;save file count pointer
dinp1:	call	gtlhs		;get destination
	pop	h		;file count pointer to HL
	cpi	strst
	push	psw		;save type (Zero set iff string)
	lxi	d,nlnad		;input buffer address to DE
	mvi	c,0		;char count to C
dinp2:	call	dread		;read a disk file char
	cpi	' '+1
	jc	dinp2		;ignore leading spaces or control chars
	cpi	','
	jz	dinp2		;and ignore leading commas
dinp3:	cpi	' '
	jnz	dinp4		;next not space
	pop	psw
	push	psw		;Zero set iff string
	mvi	a,' '
	jnz	dinp6		;done if space and numeric
dinp4:	cpi	','
	jz	dinp6		;done if comma
	cpi	cr
	jz	dinp5		;done if <cr>
	stax	d		;else store the char
	inx	d
	inr	c		;bump the count
	jz	dinp8		;256 chars
	call	drea0		;else look at next char
	jz	dinp6		;done if eof
	inr	m		;else read it
	jmp	dinp3		;and repeat
dinp5:	call	dinlf		;ignore trailing <linefeed> if present
dinp6:	pop	psw
	push	h
	lxi	h,nlnad
	jnz	dinp9		;numeric value
	xchg			;location to DE
	mvi	a,strst
dinp7:	call	asigv		;assign value to destination
	call	gtcnd		;look for comma and more vars
	jnc	dinp1
	pop	h		;pop saved file count pointer
din10:	call	drea0		;look at first unread char
	rz			;eof, done
	cpi	' '		
	jz	din11		;ignore trailing spaces
	cpi	cr	
	rnz			;done if next char not space or <cr>
	inr	m		;ignore the trailing <cr>
;DINLF ignores next character iff <linefeed>
dinlf:	call	drea0		;look at next char
	rz			;eof
	cpi	lf
	rnz			;not <lf>, return
	inr	m		;read the <lf>
	ret			;and return
din11:	inr	m		;read the trailing space
	jmp	din10		;and look for more trailing spaces or <cr>
dinp8:	dcr	m		;unread the 256th char
	dcr	c		;correct the count
	jmp	dinp6		;and assign it
dinp9:	mvi	a,cr
	stax	d		;put <cr> after value
	shld	txtp2
	call	flip		;to read the input buffer
	call	gtlit		;evaluate the numeric value
	jc	fierr		;bad value
	cpi	strst
	jz	fierr		;string value is also bad
	push	d
	push	psw
	call	flip		;to read as before
	ldax	d		;fetch next char from input buffer
	cpi	cr
	jnz	fierr		;FI error if not <cr>, value is bad
	pop	psw
	pop	d
	jmp	dinp7		;else assign value to destination as above
fierr:	error	f, F, I		;fatal FI error
fmerr:	error	f, F, M		;fatal FM error

;LINPUT [@<expr>] <string var>
	if	not c3885
linpt:	call	idtst		;LINPUT is illegal in direct mode
	call	gtsfn		;look for file #
	push	psw
	push	b
	call	gtcom		;skip comma, if any
	call	gtlhs		;get destination
	cpi	strst
	jnz	snerr		;must be string
	pop	h		;count pointer to HL
	pop	psw
	jc	linp0
	jz	linp0		;normal linput
	ora	a
	jz	fmerr		;FM error if open for output
	lxi	d,nlnad
	push	d		;save input buffer address
	mvi	c,0
	jmp	linf1
linf0:	inr	c		;char count to C
	jz	linf2		;256 chars read
linf1:	call	dread		;read a disk char
	stax	d		;store it
	inx	d
	cpi	cr
	jnz	linf0		;read another unless at cr
	pop	d		;location = input buffer to DE
	call	dinlf		;skip trailing <lf> if present
	jmp	linp2		;assign value to destination
linf2:	dcr	m		;unread the 256th char
	dcr	c		;correct the count
	pop	d		;restore location
	jmp	linp2		;assign value to desination
;LINPUT from the console
linp0:	lhld	textp
	push	h		;save textp
	call	gtlin		;get input line
	xthl			;recover old textp
	shld	textp
	pop	h
	mov	d,h
	mov	e,l		;copy first char address to DE
	mvi	a,cr
	mvi	c,0		;char count to C
linp1:	cmp	m
	jz	linp2		;done if char is cr
	inr	c
	inx	h
	jmp	linp1
linp2:	mvi	a,strst
	jmp	asigv		;assign string value to destination
	endif

;MARGIN [@<expr>,] <expr>
	if	not c3885
margn:	call	gtsfn		;find file
	jc	marg0		;none, change WIDTH
	push	psw		;save Zero status
	push	b		;and pointer
	mvi	d,','
	call	gtdsn		;skip ,
	call	gtbex		;get byte value
	pop	h		;pointer to HL
	pop	psw		;restore Zero
	jz	marg1		;@0, change WIDTH
	dcx	h
	dcx	h		;point to file width field
	mov	m,c		;and change it
	ret
marg0:	call	gtbex		;get byte value
marg1:	mov	a,c
	sta	width		;value to WIDTH
	ret
	endif

;OPEN {I | O | U} @<expr>, <filename>
open:	call	gtcha		;get desired mode
	push	psw		;and save
	call	gtcom		;allow comma after mode
	call	gtatn		;get desired file number
	jc	bferr
	jz	bferr		;0 not allowed
	call	sfils		;set up B, DE, HL for search
	mov	c,b		;max # files to C also
	push	d
	push	h		;and save
;first check if number already associated with open file
open1:	dcr	c
	jz	open2
	cmp	m
	jz	foerr		;desired number already open, FO error
	dad	d
	jmp	open1		;else try next
open2:	mov	c,a		;desired number to C
	push	b
	mvi	d,','
	call	gtdsn		;skip ,
	call	gtfnm		;get desired filename
;check if file of same name is already open
	pop	b
	pop	h
	push	h
	push	b
opn2a:	dcr	b
	jz	opn2c		;checked all files, ok
	mov	a,m		;fetch file #
	lxi	d,-37
	dad	d		;address the file fcb
	ora	a
	jz	opn2b		;no file, skip the test
	push	b
	push	h
	lxi	d,fcbad
	lxi	b,(12 shl 8) or 12	;count = 12 chars to B and C
	call	cmstr		;compare file fcb with new fcb
	jz	foerr		;already open, FO error
	pop	h
	pop	b
opn2b:	lxi	d,-129
	dad	d		;address the next file #
	jmp	opn2a		;and test it
opn2c:	pop	b
	pop	h
	pop	d
;then check if space available for file
open3:	dcr	b
	jz	operr		;all available file areas full
	mov	a,m
	ora	a
	jz	open4		;available
	dad	d
	jmp	open3		;else try next
open4:	mov	m,c
	dcx	h
	pop	psw		;recover desired mode
	cpi	'O'
	jz	openo
	cpi	'U'
	jz	openu
;else open for input
	cpi	'I'
	jnz	snerr		;SN error if not I
	inx	h
	mvi	m,0		;reset file # to 0 in case not found
	push	b
	push	h
	call	cdkop		;open the file
	pop	h
	pop	b
	mov	m,c		;set the file #
	dcx	h
	mvi	m,255		;mode = input
	dcx	h
	mvi	m,7FH		;count = 7FH so next read gets new buffer
	lxi	d,-35
	dad	d		;HL addresses file fcb
	jmp	open6
;open for output
openo:	xra	a
	mov	m,a		;mode = output
	dcx	h
	mov	m,a		;buffer address = 0
	dcx	h
	mov	m,a		;column = 0
	dcx	h
	mvi	m,72		;default width = 72
	lxi	d,-33
	dad	d		;HL addresses file fcb
	push	h
	call	cdkmk		;delete old, make new
	pop	h
open6:	lxi	b,fcbad
	lxi	d,33
	jmp	moved		;copy default fcb to fcb and return
;open for update
openu:	mvi	m,0		;mode = output
	dcx	h
	push	h		;save count address
	dcx	h
	mvi	m,72		;column = 72
	dcx	h
	mvi	m,72		;default width = 72
	lxi	d,-161
	dad	d		;address buffer base
	xthl			;save
	push	h		;and save count address
	call	cdkop		;open the file
opnu1:	call	cdkrd		;read a record
	mvi	a,0		;count zero in case eof
	jz	opnu4		;eof
	lxi	h,dmaad		;default dma address to HL
	mvi	a,cntlz		;eof to A
opnu2:	cmp	m		;check if next char is eof
	jz	opnu3		;yup
	inr	l		;else try next
	jnz	opnu2
	jmp	opnu1		;reading another buffer if necessary
opnu3:	mov	a,l
	sui	80H		;compute the new count
opnu4:	pop	h
	mov	m,a		;store the new count
	pop	h
	lxi	b,dmaad
	lxi	d,80H
	call	moved		;copy the buffer to file entry buffer
	lda	fcbcr
	dcr	a
	sta	fcbcr		;update the cr field to rewrite the same record
	jmp	open6		;and copy the fcb to the file entry
foerr:	error	f, F, O		;fatal FO error
operr:	error	f, O, P		;fatal OP error

;PRINT @<expr>, <print list>
dprin:	push	psw
	mvi	d,','
	call	gtd		;skip comma if present
	pop	psw
	jz	prin0		;normal PRINT if @0
	ora	a
	jnz	fmerr		;FM error if file is OPEN for input
	lxi	h,nulls
	mov	a,m		;fetch current NULLS value
	push	psw		;and save
	mvi	m,0		;and reset to 0 for disk PRINT
	lhld	colum
	push	h		;save current column and width
	mov	h,b
	mov	l,c
	shld	filep		;set file pointer so PRINT goes to file
	dcx	h
	mov	e,m		;column to E
	push	h
	dcx	h
	mov	d,m		;width to D
	xchg
	shld	colum		;set file column and width
	call	prin0		;print the line
	lda	colum
	pop	h
	mov	m,a		;reset file column
	pop	h
	shld	colum		;restore column and width
	pop	psw		;recover original NULLS value
	sta	nulls		;and restore it
	lxi	h,0
	shld	filep		;reset file pointer
	ret

;SCRATCH [<ambiguous filename>]
	if	not c3885
scrat:	call	gtafn		;get ambiguous filename
	mvi	c,dkdlf
	jmp	bdosf		;delete it
	endif


;functions

;EOF: <integer> --> <integer>
eoffn:	mov	a,b
	ora	a
	jnz	bferr		;arg too large, cannot be file #
	mov	a,c
	call	gtsf0		;find arg value
	ora	a
	jz	fmerr		;FM error if open for output
	mov	h,b
	mov	l,c
eoff0:	call	drea0		;look at next char
	lxi	b,-1
	rz			;eof, return -1
	inx	b
	ret			;no eof, return 0


;routines

;DREAD reads a char from a disk file, issues DK error if char is EOF.
dread:	call	drea0		;read a char
	jz	eferr		;EF error if read through eof
	inr	m		;else read the char
	ret
eferr:	error	f, E, F		;fatal EF error

;DREA0 looks at the next character from a disk file.
;Call:	HL	pointer to count of file entry
;Retn:	A	next character
;	BC,DE,HL	preserved
;	Zero	Set iff eof
drea0:	push	h
	push	d
	mov	a,m		;fetch count
	inr	a		;and bump it
	jp	drea1		;ok unless 80H
;must read a new buffer of characters and reset count
	mvi	m,255		;reset count of last char read
	push	h
	push	b
	lxi	d,-35
	dad	d
	push	h		;save fcb address
	lxi	d,-128
	dad	d		;address buffer
	xchg
	call	stdma		;set dma address to buffer
	pop	d
	mvi	c,dkrdf
	if	c3885 and not debug
	call	bdos1		;BDOS call with interrupts disabled
	else
	call	bdos
	endif
	push	psw		;save status
	call	rtdma		;and reset the dma address to 80H
	pop	psw		;restore status
	pop	b
	pop	h
	cpi	2
	jz	dkerr		;read error
	cpi	1
	mvi	a,cntlz
	jz	drea2		;return if eof
	xra	a		;reset count
drea1:	lxi	d,-163
	dad	d
	call	adahl		;address desired char
	mov	a,m		;fetch it
drea2:	cpi	cntlz		;Zero set iff eof
	pop	d
	pop	h
	ret

dwrit:	call	dwrtc		;write char to disk file
	jmp	pop4		;restore and return
dwrtc:	mov	a,m		;fetch count
	inr	m		;update count
	push	psw
	xchg			;count pointer to DE
	lxi	h,-163
	dad	d		;point to buffer base
	call	adahl		;add count+base = destination
	mov	m,c		;char to destination
	pop	psw
	rp			;return unless count is 80H
	xra	a
	stax	d		;store new count = 0
	inx	h		;point to fcb
	push	h		;save fcb address
	lxi	d,-128
	dad	d
	xchg
	call	stdma		;set dma address to buffer
	pop	d		;recover fcb address
	push	d		;and save again
	mvi	c,dkwtf
	if	c3885 and not debug
	call	bdos1		;BDOS call with interrupts disabled
	else
	call	bdos
	endif
	push	psw
	call	rtdma		;reset dma address
	pop	psw
	pop	d		;fcb address to DE
	ora	a
	rz			;successful write, return
	lxi	h,37		;otherwise disk is full
	dad	d
	mvi	m,0		;reset the file # to 0, now inactive
	mvi	c,dkdlf
	if	c3885 and not debug
	call	bdos1		;BDOS call with interrupts disabled
	else
	call	bdos
	endif
	error	f, D, F		;fatal DF error

	endif			;end of NOT RTPAK conditional

;@ <expr>
;GTATN gets @ followed by a number for sequential disk routines.
;A fatal DK error occurs if the <expr> is not in the range 0 to 255.
;Retn:	Carry	Set iff no @ found
;	Zero	Set iff <expr> is 0
;	A	Value of <expr>
gtatn:	mvi	d,'@'
	call	gtd		;look for @
	rc			;not found
	call	gtexp		;evaluate the <expr>
	mov	a,b
	ora	a
	jnz	bferr		;fatal BF error if not between 0 and 255
	ora	c		;value to A
	ret
bferr:	error	f, B, F		;fatal BF error

;GTSFN finds an OPEN sequential disk file.  A DK error occurs
;if a nonzero file # is given but no corresponding OPEN file exists.
;Retn:	Carry	Set iff no @ found
;	Zero	Set iff file @0 is specified
;	A	Mode of file (0 output, nonzero input)
;	BC	Pointer to count field of file entry
;	HL	Pointer to byte preceding file buffer
gtsfn:	call	gtatn		;look for @<expr>
	rc			;no @
	rz			;@ 0
	if	rtpak
	jmp	uferr		;issue UF error if nonzero file # in RTPAK
	else			;NOT RTPAK
gtsf0:	call	sfils		;set up registers for search -- EOF entry point
gtsf1:	dcr	b
	jz	bferr		;not found, BF error
	cmp	m		;compare desired to actual
	jz	gtsf2		;gotcha
	dad	d		;else try next
	jmp	gtsf1
gtsf2:	mov	b,h
	mov	c,l		;file pointer to BC
	dad	d		;HL points below buffer
	ori	1		;Carry and Zero reset
	dcx	b
	ldax	b		;fetch mode
	dcx	b		;point to count with BC
	ret

sfils:	lxi	h,files
	mov	b,m		;max # files to B
	lhld	filet
	lxi	d,-filen
	ret

	endif			;end of NOT RTPAK conditional
	endif			;end of SDISK conditional

;end of SDISK
	page
;STACKS 5/23/79
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979 by Mark Williams Company, Chicago
;symbol table and control stack manipulation routines


;FNDTK finds the location of a token in the keyword table.
;Call:	A	character or token
;Retn:	Carry	Reset if character, set if token
;	A	Preserved if character, 0 if token
;	BC,DE	Preserved
;	HL	Preserved if character, address of table entry if token
	if	not compl
fndtk:	ora	a
	if	key80
	jm	fndt1		;command or function token
	cpi	20H
	rnc			;printable char, return Carry reset
	cpi	rwdtk
	cmc
	rnc			;<cr> or <bell>, return Carry reset
	lxi	h,rwdta		;elese reserved word token
	sui	(rwdtk-1) and 0FFH	;token bias
	jmp	fndt2
	else			;not KEY80
	rp			;return Carry reset if positive
	endif
fndt1:	lxi	h,keyta
	adi	nkeys+1		;token bias
fndt2:	dcr	a		;decement token count
	jnz	fndt3		;not there yet
	stc			;else set Carry
	ret			;and return
fndt3:	push	psw		;save token count
fndt4:	mov	a,m		;fetch char
	inx	h
	ora	a
	jp	fndt4		;skip more chars in same keyword
	pop	psw		;restore token count
	jmp	fndt2		;and keep looking
	endif

;STPSH builds a new symbol table entry.
;Call:	A	token
;	HL	entry length
;	(bufad)	symbol name
;Retn:	(symta)	(symta) - length
;	((symta))	length
;	((symta)+1)	token
;	((symta)+2...)	name
;Other bytes in the entry are zeroed
;	A	token
;	BC	preserved
;	DE	address of name
;	HL	address following name
stpsh:	push	b
	push	psw		;save token
	xchg			;length to DE
	mvi	a,254
	cmp	e		;Carry set iff (E) is 255
	sbb	a		;A is 255 if (E) is 255, 0 otherwise
	ora	d		;Zero set iff (DE) <= 254
	jz	stps1
	inx	d
	inx	d
stps1:	push	d		;save length
	call	cplde		;- length to DE
	lhld	symta
	dad	d		;old - length = new symbol table address
	jnc	omerr		;much too big, OM error
	xchg
	lhld	cstkp
	xchg			;CSTKP to DE
	call	cmdhu		;compare to new SYMTA
	jnc	omerr		;CSTKP >= new SYMTA, OM error
	shld	symta		;store new SYMTA
	pop	d		;restore length
	push	d		;and save
	push	h		;and save SYMTA
stps2:	mvi	m,0		;zero a byte
	inx	h
	dcx	d
	mov	a,d
	ora	e
	jnz	stps2		;zero more bytes
	pop	h		;restore pointer
	pop	d		;and length
	mov	a,d
	ora	a
	jz	stps3
	mvi	m,255		;long entry
	inx	h
	mov	m,d
	inx	h
stps3:	mov	m,e		;store length
	inx	h
	pop	psw		;recover token
	push	psw
	mov	m,a		;store token
	inx	h
	push	h		;save name pointer
	call	cpys0		;copy name from bufad
	pop	d		;return name pointer in DE
	pop	psw		;restore token to A
	pop	b
	ret
omerr:	error	f, O, M		;fatal OM error

;STNXT gets the address of the next symbol table entry.
;Call:	HL	address of current symbol table entry length byte
;Retn:	Carry	set iff no more entries
;	BC	preserved
;	DE	current entry type byte address
;	HL	next entry address
stnxt:	mov	a,m		;fetch length byte
	ora	a
	stc
	rz			;return carry set if at end of table
	mov	d,h
	mov	e,l
	inx	d		;point to type byte
	cpi	255		;check if long entry
	jnz	adahl		;no, so length + current = next and return
	xchg			;current to DE, length byte addr to HL
	mov	a,m
	inx	h
	push	h		;save type byte addr - 1
	mov	l,m
	mov	h,a		;length to HL
	dad	d		;current + length = next
	pop	d		;type byte addr to DE
	inx	d
	ret

;STLKU looks up a symbol in the symbol table.
;Call:	(bufad)	symbol name string
;	(varty)	symbol type token
;Retn:	Carry	set iff not found, otherwise:
;	DE	entry type address
;	HL	address following name in entry
stlku:	lda	varty
stlk0:	lhld	symta
	mov	b,a		;desired type to B
stlk1:	call	stnxt		;address next entry
	rc			;return carry set iff not found
	ldax	d		;fetch entry type
	ani	1FH		;mask to type
	cmp	b
	jnz	stlk1		;not the right type, try next
	push	h		;save next
	push	d		;save type byte addr
	xchg
	inx	h		;point to name
	lxi	d,bufad
	call	cmpst		;compare to desired name
	pop	d		;restore type addr to DE
	jz	pop1		;matched, pop next to BC and retn Carry reset
	pop	h		;else next to HL
	jmp	stlk1		;and try next

;STZAP resets BREAK and FN entries in the symbol table.
;Retn:	BC	preserved
;	A,DE,HL	clobbered
stzap:	lhld	symta
stza1:	call	stnxt
	if	compl
	rc			;end of table, done if no line breaks to zap
	else
	jc	unbkl		;unbreak lines if not compl
	endif
	ldax	d
	ani	1FH		;mask to type
	if	not compl
	cpi	brkst
	jz	stza2		;break
	endif
	cpi	ufnst		;check if user-defined fn
	jnz	stza1		;neither FN nor break, try next
stza2:	xra	a
	stax	d		;store 0 type byte
	jmp	stza1

	if	not compl
;UNBKL removes all line breakpoints.
unbkl:	if	romsq		;first loc to test to HL
	lhld	sourc
	call	rtest
	rnz			;done if ROM
	else
	lxi	h,srcad
	endif
unbl1:	mov	a,m
	ora	a
	rz			;done
	push	h
	call	unbr1		;unbreak a line
	pop	h
	call	adahl		;point to next line
	jmp	unbl1		;and keep going
	endif

;CPUSH checks that sufficient free memory remains to build a control stack
;entry and if not issues an OM error.  Otherwise it pushes a token, (TEXTP) and
;(LNNUM) to the control stack, and updates the stack pointer.
;Call:	A	token (l.s. 5 bits give entry length)
;Retn:	A,BC	clobbered
;	DE	return text pointer
;	HL	(cstkp) - 4
;	(cstkp)	(cstkp) + (A4-A0)
;	((cstkp))	token
;	((cstkp)-1:(cstkp)-2)	line # address
;	((cstkp)-3:(cstkp)-4)	return text pointer
cpush:	lhld	textp
	push	h
cpsh1:	mov	b,a		;save token
	lhld	cstkp
cpsh2:	ani	1FH		;mask to length
	call	adahl
	jc	cpsh3		;new stacktop > 64K, OM error
	call	cspst		;store new control stack pointer
	xchg			;cstack pointer to DE
	lhld	symta		;symbol table pointer to HL
	call	cmdhu		;compare
	jnc	cpsh3		;OM -- flush and continue
	lhld	lnnum
	if	not compl
	mov	a,h
	ora	l
	cz	icstd		;increment CSTACK direct count if direct
	endif
	xchg			;current line # address to DE
	mov	m,b		;token to control stack
	call	mvmde		;return line # address to control stack
	pop	d
	jmp	mvmde		;return text pointer to control stack & return
cpsh3:	call	clea2		;reset CSTACK pointer
	error	n, O, M		;nonfatal OM error
	mov	a,b		;restore token
	jmp	cpsh2		;try again

;CSPOP is used to pop information from the control stack for RETURNs from
;GOSUBs and interrupts, for CONTinues, and for NEXTs.
;If the entry was made from direct mode, the dmode count CSTKD is
;decremented and an ID error issued if < 0.
;Call:	HL	address of control stack entry type/length byte
;Retn:	(lnnum)	control stack entry line # bytes
;	(textp)	control stack entry return address
;	PSW,BC	preserved
;	DE	new textp
;	HL	(HL) - 5
cspop:	if	compl
	dcx	h
	call	mvdem		;line # to DE
	else			;non COMPL version must update CSTKD
	push	psw
	push	h		;save type byte addr
	dcx	h
	call	mvdem		;line # of entry to DE
	xthl			;save entry pointer, type byte addr to HL
	if	not compl
	mov	a,d
	ora	e		;test if direct mode entry
	jnz	cspo1		;no -- restore and return
	call	dcstd		;yes -- decrement CSTACK direct count
	mov	a,m		;fetch type byte
	cpi	csfor		;check if doing a NEXT
	cz	icstd		;undecrement count if so
	endif
cspo1:	pop	h		;restore entry ptr
	pop	psw
	endif
	xchg
	shld	lnnum		;store new lnnum
	xchg
	call	mvdem
	xchg
	shld	textp		;store new textp
	xchg
	ret			;and continue from there

;CSDIG digs in the control stack for an entry with type matching B or C.
;Call:	B,C	desired type/length bytes
;Retn:	A	type/length byte found, 0 if none
;	BC,DE	preserved
;	HL	addr of type/length byte, eofad if not found
;	Carry	set iff not found
csdig:	lhld	cstkp
csdi1:	mov	a,m		;fetch type/length byte
	cmp	b
	rz			;found
	cmp	c
	rz			;found
	ani	1FH		;mask to length
	stc
	rz			;not found
	call	sbahl		;address next
	jmp	csdi1		;and try it

;CSRST resets the control stack after purging an entry.
;Call:	A	type/length byte of entry to be purged
;	HL	address of next entry in stack
;Retn:	BC	preserved
;	HL	new cstkp
csrst:	push	b
	inx	h		;point to destination
	push	h		;and save it
	ani	1FH		;mask A to length
	call	adahl		;destination + offset = source
	push	h		;saved
	xchg
	call	cplde
	lhld	cstkp
	dad	d		;# of bytes to move -1 to HL
	inx	h
	xchg			;and then to DE
	pop	b		;source to BC
	pop	h		;destination to HL
	cc	moved		;move rest  of stack
	pop	b		;restore BC
	dcx	h		;readdress cstkp
	jmp	cspst		;and reset it

;ICSTD increments the CSTACK direct count CSTKD.  DCSTD decrements CSTKD and
;issues an ID error if negative.  CSTKD counts how many CSTACK entries
;have been built from direct mode, and catches errors such as attempting to
;RETURN or NEXT when the GOSUB or FOR context has been lost.
;Status bits affected, all registers preserved.
	if	not compl
icstd:	push	h
	lxi	h,cstkd
	inr	m		;increment cstack direct count
	pop	h
	ret
dcstd:	push	h
	lxi	h,cstkd
	dcr	m		;decrement cstack direct count
	pop	h
	rp			;ok if nonnegative
	jmp	iderr		;fatal ID error if negative
	endif


;end of STACKS
	page
;STATES1 12/02/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;statement routines for statements ASSIGN through EXEC


;ASSIGN {LST# | PUN# | RDR# | CON#} <expr>
	if	(not camac) and (not wild)
;ASSIGN changes the value of the specified field of IOBYTE.
assig:	call	gtcha		;fetch token
	sui	contk		;subtract CON# token bias
	jc	snerr		;too small
	cpi	4
	jnc	snerr		;too big
	rlc			;*2 bits  = shift count (0, 2, 4, 6)
	mov	e,a		;shift count to E
	call	gtexp		;arg to BC
	mov	a,c
	ani	0FCH
	ora	b		;check if arg > 3
	cnz	fcern		;yes, nonfatal FC error
	mvi	a,3
	mov	b,a		;mask to B
	ana	c
	mov	c,a		;masked new bits to C1-0
	if	nonst
	mov	a,e
	ora	a		;check if CON#
	jnz	assi1		;no, just update IOBYTE
	mov	b,a		;0 to B
	lxi	h,jmpta+60	;base of console status jump vector to HL
	dad	b
	dad	b
	dad	b		;+3 bytes * desired CON#
	shld	cstat+1		;becomes new console status jump
	mvi	b,3		;restore mask to B
	endif
assi1:	call	lshft		;shift mask and bits to desired position
	mov	a,b
	cma
	lxi	h,iobyt
	ana	m		;mask out old field
	ora	c		;OR in the new field
	mov	m,a		;store the new iobyte
	ret
	endif

;AUTO [<line #> [, <line #>]]
	if	editc
	if	compl
auto	equ	uferr
	else
auto:	if	romsq
	call	issrc		;must be addressing working space
	endif
	lxi	h,10
	push	h		;default increment = 10
	push	h		;default first line # = 10
	call	gtlno		;look for <line #>
	jc	auto1		;no args, take defaults
	xchg			;<line #> to HL
	xthl			;and replaces default <line #>
	call	gtcnd		;look for comma
	jc	auto1		;no second arg
	call	gtlno		;look for increment arg
	jc	snerr		;not found
	xchg			;increment to HL
	pop	d
	xthl			;and replaces default incrment
	push	d
auto1:	call	iitst		;AUTO illegal indirect
;At AUTO2 the next desired line # and the increment are on the stack.
auto2:	pop	b		;next line # to BC
	push	b
	call	wrtbu		;write the line #
	mov	d,b
	mov	e,c		;line # to DE
	call	findl		;look for it in current source text
	mvi	a,' '
	jc	auto3		;write a space if no such line exists
	mvi	a,'*'		;else write a *
auto3:	call	writc
	call	lnnu0		;reset LNNUM to 0 in case ^C typed
	call	gtlin		;get line from user
	call	tkize		;tokenize it
	jz	dmod2		;return to DMODE if user types <cr>
	cnc	lnnu0
	jnc	snerr		;SN error if <line #> typed
	pop	h
	push	h
	shld	lnnum		;set LNNUM to desired line
	call	addln		;add new line to source text
	pop	h
	pop	d
	dad	d		;new line # is line # + increment
	jc	auto4		;OV error if > 65535
	push	d
	push	h
	jmp	auto2		;else get next line
auto4:	call	lnnu0
	mvi	a,cr
	sta	nlnad		;reset input buffer for error message
	call	iover		;issue OV error
	jmp	dmod2		;and return to direct mode
	endif
	else
auto	equ	exerr		;EX error in non-EDITC versions
	endif

;BREAK {<var list> | <line #> [,<expr>] [;<var list>] [;$]}
;BREAK <var list> creates variable breaks by setting symbol table entry bits.
;BREAK <line #>... creates line breakpoints by setting the line break byte,
;and also building a symbol table line break entry if necessary.
	if	compl
break	equ	uferr		;UF error in COMPL version
	else
break:	call	gtlno		;look for line #
	jc	bkvar		;no line #, must be var break
	if	romsq
	call	isrom		;line breaks only if running in RAM
	endif
	call	fdlno		;find the line
	inx	h
	inx	h
	inx	h		;address break byte
	push	h		;save break byte addr
	call	gtcom		;look for comma
	lxi	b,1		;default count = 1 to BC
	cnc	gtexp		;get count if comma
	push	b		;save count
	dcx	b
	mov	a,b
	ora	c		;Zero set iff count = 1
	mvi	a,1		;break byte value to A
	jz	brkl1
	ori	2		;bit 1 on iff count <> 1
;At BRKL1 the break byte addr and count are PUSHed, the break byte is in A.
brkl1:	mov	e,a		;save the break byte
	mvi	d,semic
	call	gtd		;look for ;
	shld	txtp2		;save varlist address
	jnz	brkl4		;no ; present
brkl2:	push	d
	call	gtvar		;look for var ref
	pop	d
	jc	brkl3		;none, must be $
	call	idtst		;BREAK illegal direct if <var list> present
	mov	a,e
	ori	6		;set varlist bits in break byte
	mov	e,a
	call	gtcom		;look for comma
	jnc	brkl2		;more vars
	call	gtd		;look for ;
	jnz	brkl4
brkl3:	mvi	d,'$'
	call	gtdsn		;skip $
	mov	a,e
	ori	8		;set bit 3 to indicate break to DMODE
	mov	e,a
brkl4:	mov	a,e		;break byte value to A
	pop	b		;count to BC
	pop	h		;restore break byte addr
	mov	m,a		;store break byte
	ani	6		;check if must build symbol table entry
	rz			;no, done
;Now a symbol table line break entry is built.  The entry contains
;a length byte, type byte, three 'name' bytes with the encoded line #,
;two count bytes (how many passes until next break), two reset bytes (the
;value for resetting count when it reaches zero), and two varlist addr bytes.
	push	b		;save count
	dcx	h
	dcx	h		;HL contains line # addr
	call	bknam		;form break entry name in BUFAD
	lxi	h,11		;entry length = 11
	mvi	a,brkst		;token to A
	call	stpsh		;build the entry
	pop	d		;count to DE
	mov	m,e
	inx	h
	mov	m,d		;count to entry
	call	momde		;reset to entry
	xchg
	lhld	txtp2
	xchg			;varlist address to DE
	endif
momde:	inx	h
	mov	m,e
	inx	h
	mov	m,d		;varlist address to entry
	ret			;and return
	if	not compl
;BKVAR sets type byte bits in symbol table entries to indicate variable breaks.
bkvar:	call	fdvar		;find variable name
	jc	snerr		;no var
	ldax	d
	ori	80H		;set break bit in type byte
	stax	d
	call	gtcnd		;look for comma
	jnc	bkvar		;break another var
	ret
	endif

;CALL <expr> [, {<var ref> | * <var name>}]*
calcm:	lxi	h,nextc
	push	h		;push return address to scan to next statement
	call	gtexp		;get location
	push	b
	ret			;branch to user routine

;CAMAC commands are in module CAMAC.

;CLEAR
;CLEAR <expr>		[in STRNG versions]
;CLEAR @ <expr>		[in CPM SDISK versions]
;DCLR0 is in module SDISK.
clear:	if	strng
	call	dtst0		;check if delimiter follows
	jnc	clea1		;yes, keep same string space
	if	cpm and sdisk
	call	gtatn		;look for @ <expr>
	jnc	dclr0		;change max number of disk files
	endif
	call	gtexp		;otherwise get argument
	mov	a,b
	ora	a
	jm	fcerf		;fatal FC error if negative string space given
	mov	d,b
	mov	e,c		;to DE
	call	cplde		;complement desired amount of string space
	lhld	eofad
	lxi	b,9
	dad	b		;leave enough room to compute trivial exprs
	push	h		;save eof top pointer
	lhld	strt		;top of string space to HL
	dad	d		;new MEMT value to HL
	pop	d		;eof pointer to DE
	call	cmdhu		;make sure sufficient space available
	jnc	omerr		;no -- fatal OM error
cle0a:	shld	memt		;yes -- store new MEMT value
	mvi	m,0		;and initialize symbol table
	endif
clea1:	if	not compl
	call	unbkl		;unbreak line breaks
	endif
	lhld	memt
	if	strng
	shld	strpt		;initialize string pointer
	shld	strp2		;and string temp pointer
	endif
	shld	symta		;and symbol table
	if	strng or float	;reset default type buffer entries
	if	float		;default token to B, buffer length 26 to C
	lxi	b,(sngst shl 8) or 26
	else
	lxi	b,(intst shl 8) or 26
	endif
	lxi	h,tybuf		;default type buffer address to HL
	call	fillm		;reset default type buffer
	endif			;end of STRNG or FLOAT conditional
	if	camac and c3908	;reset BKSET parameters if CAMAC on 3908
	lxi	b,7		;0 to B, 7 to C
	lxi	h,cmblk
	call	fillm		;reset BKSET parameters to 0
	endif
	if	packi
	call	pinit		;reset Packard FIELD and data buffer
	sta	paonl		;and reset mode to OFFLINE
	endif
clea2:	lhld	eofad
	jmp	cspst		;reset the control stack

;CONT
	if	compl
cont	equ	uferr		;UF error in COMPL version
	else
cont:	call	iitst		;CONT illegal indirect
	lhld	cstkp
	mov	a,m
	cpi	csbrk		;see if BREAK entry atop control stack
	jz	cont1		;yes -- pop it
	cpi	cslbk		;see if line break entry
	jnz	cnerr		;no -- CN error
	call	cont1		;pop entry
	pop	b		;pop CONT return address
	jmp	xstat		;continue at XSTAT, skipping break test
cont1:	call	cspop		;yes -- pop it
	jmp	cspst		;reset control stack pointer and return
cnerr:	error	f, C, N		;fatal CN error
	endif

;DATA [<expr>] [,<expr>]*
;DATA is ignored when encountered, so the command dispatch table branches
;to GTDEL to scan to next command for DATA.

;DEF FN <var name> [(<var name> [, <var name>]* )] = <expr>
;DEF {INT | STR | SNG | CAMVAR} <letter> [- <letter>]
def:	call	gtcha		;get following token
	cpi	udfnt
	jnz	defvt		;not a user-def FN, must be var declaration
;DEF FN builds a symbol table entry for a user-defined function.  The entry
;contains a length byte, type token, name bytes, and two address bytes.
;The addr bytes contain the fn body addr if 0-ary, or bound var addr otherwise.
;The type token is UFNST if 0-ary and (UFNST or 20H) otherwise.
	call	idtst		;DEF FN illegal direct
	call	gtnam		;get fn name
	lxi	h,4
	call	adahl		;name length + overhead = entry length to HL
	push	h		;and saved
	mvi	a,ufnst
	call	stlk0		;look up the fn name 
	jnc	dderr		;already defined -- fatal DD error
	mvi	d,'('
	call	gtd		;look for (
	jc	def1		;0-ary fn
	xthl			;save bound var addr, entry length to HL
	mvi	a,ufnst
	ori	20H		;set unary bit
	call	stpsh		;build symbol table entry
	push	h
def0:	call	fdvar		;look for nonsubscripted variable
	jc	snerr
	call	gtcom		;look for comma
	jnc	def0		;comma must be followed by another bound var
	call	gtreq		;skip ) =
	pop	h
	jmp	def2
def1:	call	gtequ		;skip = token
	mvi	a,ufnst		;token to A
	xthl			;save addr, get entry length
	call	stpsh		;build symbol table entry
def2:	pop	d		;body address to DE
	mov	m,e
	inx	h
	mov	m,d		;body address to entry
	jmp	gtdel		;scan function body and return

	if	strng or float
;DEFVT sets the default type for variables with given initial letter(s).
;The 26 byte buffer TYBUF contains the default variable types.
;DEFTY returns the type token in B corresponding to keyword token in A.
defty:	mvi	b,intst
	cpi	intt
	rz			;integer type
	if	float
	mvi	b,sngst
	cpi	sngt
	rz			;floating type
	endif
	if	strng
	mvi	b,strst
	cpi	strgt
	rz			;string type
	endif
	if	camac
	mvi	b,camst
	cpi	camt
	rz
	endif
	call	bakup		;else back up TEXTP
	jmp	snerr		;and issue SN error
defvt:	call	defty		;desired type to B
	call	gtalp		;first letter to A
	jc	snerr		;SN error if none
	mov	e,a		;and first saved in E
	mvi	d,mint
	call	gtd		;look for -
	mov	a,e		;default last = same as first
	jc	defv1		;no second letter specified
	call	gtalp		;else get second
	jc	snerr
defv1:	sub	e		;second - first
	jm	snerr		;second precedes first
	inr	a		;# of entries to change
	mov	c,a		;count to C
	mvi	d,0		;DE now has first letter in ASCII
	lxi	h,tybuf-'A'
	dad	d		;address first entry to change
	else
defvt	equ	snerr		;issue SN error if integer version
	endif
	if	strng or float or rom
;FILLM fills (C) bytes of memory starting at (HL) with (B).
fillm:	mov	m,b		;change to desired value
	inx	h
	dcr	c
	jnz	fillm		;and fill more
	ret
	endif

;DELAY <expr> [ , <expr> [ , <expr> ]]
	if	not camac	;CAMAC DELAY is in module CAMAC
	if	realt		;hardware real time clock delay
;DELAY waits for the real time clock to tick the specified number of times.
;The arguments are assumed to be minutes, seconds and tenths of seconds.
;Typing any character aborts the DELAY.
;The implementation counts clock ticks rather than adding the arg to
;the current time and waiting until the resulting time.  The
;latter blows up if a user interrupt service routine renables
;(so the clock ticks) but lasts until after the specified time.
delay:	ora	a		;reset Carry for GTEXP
	call	dela5		;minutes to BC
	push	b		;and saved
	call	dela4		;seconds to BC
	push	b		;and saved
	call	dela4		;tenths of seconds to BC
	mov	h,b
	mov	l,c		;tenths to HL
	dad	h		;* 2 = 20ths to HL
	pop	d		;seconds to DE
;at DELA1 minutes count is PUSHed, seconds count in DE, 20ths count in HL.
dela1:	lda	timex		;fetch low order clock count
	mov	b,a		;low order clock to B
dela2:	mov	a,h
	ora	l
	jnz	dela3		;20ths count is nonzero, enter delay loop
	lxi	h,20		;reset 20ths count
	mov	a,d
	ora	e
	dcx	d		;decrement seconds count
	jnz	dela1		;seconds count was nonzero, enter loop
	pop	b		;minutes count to BC
	mov	a,b
	ora	c
	rz			;minutes also zero, finished
	lxi	d,59		;reset seconds count
	dcx	b
	push	b		;save updated minutes count
	jmp	dela1		;and reenter loop
dela3:	call	redyc		;look for console char
	jc	pop1		;char typed, pop minutes and abort delay
	lda	timex		;fetch current low order clock
	cmp	b		;check if clock has ticked
	jz	dela3		;no, keep waiting
	dcx	h		;yes, decrement tick count
	jmp	dela1		;and check again
;DELA4 gets [, <expr>] argument to BC, 0 if omitted, OR error if negative.
dela4:	lxi	b,0
	call	gtcnd		;look for comma
dela5:	cnc	gtexp		;get arg
	mov	a,b
	ora	a
	rp			;ok
	jmp	fcer0		;negative arg, nonfatal FC error and return 0
	else			;software real time clock delay
delay:	lhld	timex
	call	dela1		;delay minutes
	lhld	timex+2
	call	dela1		;delay seconds
	call	gtexp		;get hundredths count
	mov	a,b
	ora	c
	rz			;done if hundredths count is zero
	mov	d,b
	mov	e,c		;to DE
delh1:	lxi	h,-100
	dad	d		;carry reset iff hundredths count < 100
	jnc	delh2		;delay hundredths
	xchg			;hundredths=hundredths-100 to DE
	lhld	timex+2		;seconds count to HL
	call	timer		;delay one second
	jmp	delh1		;and try again
delh2:	lda	timex+4		;minutes / 256 to A
	call	mult0		;* hundredths count
	mov	b,h
	mov	c,l
	lxi	d,24
	call	divd0		;divide by 24 to put hundredth count in HL
	xchg
	call	cpld1
	xchg			;complement it and fall through to TIMER
;TIMER is the basic timing loop shared by TIME and DELAY.  After a delay of
;15*256 cycles it increments the count in HL, looks for a console char, and
;continues.  Returns Carry set if char typed, reset if HL counts to 0.
timer:	call	redyc		;look for char
	rc			;return carry set if char typed
	xra	a
time1:	dcr	a
	jnz	time1		;wait a while
	inx	h		;increment the count
	mov	a,h
	ora	l
	jnz	timer		;keep counting until count hits zero
	ret			;return carry reset if counted out
;DELA1 is called from DELAY to delay for <expr> minutes or seconds
;by executing TIMER (BC)*(HL) times.
dela1:	call	gtexp		;get the argument
dela2:	mov	a,b
	ora	c
	jz	dela3		;count is zero -- done
	push	h
	call	timer
	pop	h
	dcx	b		;decrement the count
	jmp	dela2		;and keep waiting
dela3:	mvi	d,','
	call	gtd		;look for comma
	rnc			;comma -- continue with DELAY
	pop	h		;else pop the DELA1 return
	ret			;and return from DELAY
	endif			;end of NOT REALT conditional
	endif			;end of NOT CAMAC conditional

;DELETE <line #> [, <line #>]
	if	editc
	if	compl
delet	equ	uferr
	else
delet:	if	romsq
	call	issrc		;must be addressing working space
	endif
	call	gtlno		;look for <line #>
	jc	snerr		;SN error if none
	call	findl		;find the line
	push	h
	push	psw
	call	gtcnd		;look for comma
	jnc	dele1		;found comma
	pop	psw
	jc	userr		;US error if not found
	pop	h		;location to HL
	push	h
	jmp	dele2
dele1:	pop	psw
	call	gtlno		;look for second <line #>
	jc	snerr		;SN error if none
	call	findl		;find it
	jc	dele3		;not found, HL points to next
dele2:	mov	e,m
	mvi	d,0		;length to DE
	dad	d		;HL points to next
dele3:	pop	d		;first line location to DE
	call	cmdhu		;compare
	rnc			;first loc >= second loc, ignore
	push	h		;save second loc
	push	d		;save first loc
	xchg			;second loc to DE
	call	cplde		;complemented
	call	iitst		;DELETE illegal indirect
	lhld	eofad		;eof address to HL
	dad	d
	inx	h		;eof - second + 1 = byte count to HL
	xthl			;destination = first loc to HL
	pop	d		;count to DE
	pop	b		;source = second loc to BC
	call	moved		;block move remainder of program
	dcx	h		;point to new last byte
	jmp	new1		;reset stacks
	endif
	else
delet	equ	exerr		;EX error in non-EDITC versions
	endif

;DIM <var name> (<expr>[,<expr>]*) [,<var name> (<expr> [,<expr>]*)]*
dim:	call	gtnam		;look for name
dim0:	jc	snerr		;SN error if none
	push	psw		;save symbol length
	lxi	h,bufad+maxnl	;saving address to HL
	call	cpys0		;copy var name to save it
	call	stlku		;look it up
	jc	dims0
dderr:	error	f, D, D		;fatal DD error if defined already
dims0:	mvi	d,'('
	call	gtdsn		;skip (
	pop	psw
	mov	l,a		;name length to L
	lda	varty
	mov	h,a		;type token to H
	shld	savtp		;and HL saved
	call	bytsd		;bytes per entry to E, # dims so far to D
	push	d		;and saved
	lxi	d,1		;find # elements in DE
inone	equ	$-2		;address of integer 1 for FOR step default
	push	d
;DIMS1 is repeated for each dimension of an array.  Each dimension bound
;is PUSHed, as well as bytes per entry/#dims and #elements thus far.
dims1:	pop	d		;# elements to DE
	pop	h		;#dims to H, bytes per entry to L
	inr	h		;increment # dims count
	call	gtexp		;get dim
	mov	a,b
	ora	a
	jm	fcerf		;fatal FC error if negative
	push	b		;and save it
	push	h		;save count
	inx	b		;dim + 1 (to allow subscript 0)
	call	mulbd		;new # elements = BC * DE to HL
	jc	omerr		;too many
	push	h		;save # elements
	mvi	d,','
	call	gtd		;look for ,
	jnc	dims1		;get more dimensions
	mvi	d,')'
	call	gtdsn		;skip )
	lxi	d,bufad+maxnl
	lxi	h,bufad
	call	cpyst		;restore symbol name to BUFAD
;Now the new symbol table entry for the array is built, containing length
;byte, type byte, name, #dims, bounds (2 bytes * #dims) and elements.
	pop	d		;restore # elements to DE
	pop	b		;bytes per entry to C, # dims to B
	if	camac
	lhld	savtp
	mov	a,h		;fetch type token in case camvar
	cpi	camst
	jnz	dim1a		;not a camvar
	mov	a,b
	cpi	1		;make sure camvar is 1-d
	jnz	snerr		;SN error if not
	endif
dim1a:	mov	l,b
	mvi	h,0		;now find entry length to build entry
	dad	h		;# dims * 2 bytes per dim
	inx	h		;+ length byte
	inx	h		;+ type byte
	inx	h		;+ #dims byte
	mov	a,c		;bytes per entry to A
dims2:	dad	d
	jc	omerr		;too big
	dcr	a
	jnz	dims2		;# elements * bytes per entry
	xchg			;length so far to DE
	lhld	savtp	
	mov	a,h		;symbol type token to A
	mvi	h,0		;HL now has name length
	dad	d		;entry length in HL
	jc	omerr
	call	stpsh		;build table entry
	mov	m,b		;# dims to table
	mov	c,b
	mvi	b,0		;BC now has # dims
	dad	b
	dad	b
	inx	h		;HL points past last dim
	if	camac
	cpi	camst		;check if camvar
	jnz	dims3		;no
	xthl			;component count to HL
	inx	h		;actual count = dimension + 1
	shld	cvcnt		;and saved, in case DECLARE with values
	dcx	h
	xthl			;restore count, recover location
	shld	cvloc		;and save location
	endif
dims3:	pop	d		;dim to DE
	call	mvmde		;and then to table
	dcr	c		;count down dims
	jnz	dims3
	call	gtcnd		;look for comma
	jnc	dim		;dimension another
	ret

;DISABLE [<line #>]
	if	not wild
dsabl:	call	gtlno		;line # to DE, carry set if none
	lxi	h,intad
	lda	inttc
	jnc	dsab2		;line # present
	sta	temp		;save old # entries
	xra	a
	sta	inttc		;set entry count to 0
	mov	m,a		;store table eof
dsab1:	lda	temp
	ora	a		;check if any disabled
	rnz
enerr:	error	f, E, N		;fatal EN error
dsab2:	xra	a
	sta	temp		;set flag to tell if any disabled
dsab3:	mov	a,m
	ora	a		;check current entry type byte
	jz	dsab1		;eof
	push	h		;save current addr
	lxi	b,8
	dad	b
	push	h		;save next addr
	dcx	h
	mov	b,m
	dcx	h
	mov	c,m		;ENABLE line # to BC
	call	cmbdu		;compare to desire line #
	jz	dsab4		;match -- purge it
	pop	h
	pop	b
	jmp	dsab3		;no match -- try next
dsab4:	pop	b		;next to BC -- source
	pop	h		;current to HL -- destination
	push	b		;save next
	push	d		;save line #
	mov	d,b
	mov	e,c		;current to DE
	push	h		;save destination
	lxi	h,intad+inttl	;last location + 1 to HL
	call	cplde		;- current to DE
	dad	d		;count to HL
	xchg			;then to DE
	pop	h		;restore destination
	call	moved		;move remainder of table
	lxi	h,inttc		;address table count
	mov	a,m		;fetch count
	sta	temp		;set flag to true (nonzero)
	dcr	m		;update count
	pop	d		;restore line #
	pop	h		;next to HL
	jmp	dsab3		;keep trying
	endif			;end of NOT WILD conditional

;EDIT [<line #>]
	if	editc
	if	compl
edit	equ	uferr
	else
edit:	if	romsq
	call	issrc		;must be addressing working space
	endif
	call	gtlno		;look for <line #>
	jnc	edit0
	lhld	errln		;edit most recent error line if none
	xchg			;to DE
edit0:	call	iitst		;EDIT illegal indirect
	call	fdlno		;find the line
	inx	h
	mov	c,m
	inx	h
	mov	b,m		;line number to BC
	inx	h
	inx	h
	push	h		;save pointer to first byte of line
	xra	a
	call	cvtis		;convert line number to string
	push	psw		;save length
	call	bcde		;string loc to BC, length to E
	lxi	h,nlnad		;destination to HL
	call	movd0		;copy line number string to input buffer
	xchg			;next input buffer location to DE
	pop	psw
	cma			;-length-1
	adi	nlmax+4		;max # chars + 3 - length
	mov	c,a		;to C to avoid line overflow
	mvi	a,' '
	pop	h		;first char address to HL
	push	h
	cmp	m		;check if first char of line is <space>
	cnz	edit4		;add a space if not
edit2:	pop	h		;line pointer to HL
	mov	a,m		;fetch next char or token in line
	inx	h
	push	h		;and save pointer to next
	call	fndtk		;check if char or token
	jc	edit3		;token
	call	edit4		;store the char
	cpi	cr		;check if cr
	jnz	edit2		;no, continue
	lxi	sp,stack	;reset SP in case direct line results
	call	ledit		;edit the line
	call	tkize		;tokenize the line
	jc	xsta1		;execute if no line number
	cnz	addln		;add line to current source
	jmp	dmod2		;and return to OK prompt
edit3:	mov	a,m		;fetch char of token
	ani	7FH		;mask off possible high bit
	call	edit4		;store the char
	mov	a,m		;refetch
	inx	h
	ora	a
	jp	edit3		;not end of token, get next char
	jmp	edit2		;end of token, get next char of line
edit4:	stax	d		;store the char
	inx	d		;point to next location
	dcr	c		;decrement remaining char count
	rnz			;return if enough room
	mvi	a,cr		;too little room in buffer, EX error
	sta	nlnad		;cr to input buffer
	jmp	exerr		;and issue EX error
	endif
	else
edit	equ	exerr		;EX error in non-EDITC versions
	endif

;ENABLE <line #> , <byte expr> , <byte expr> [, <byte expr>] [,$]
	if	not wild
enabl:	call	idtst		;ENABLE illegal direct
	push	b		;save current line #
	call	gtlno
	push	d		;save subroutine line #
	mvi	d,','
	call	gtdsn		;skip comma
	call	iinfo		;get interrupt info
	push	d
	push	b		;and save it
	lxi	h,inttc
	mov	a,m		;# entries to A
	cpi	inttn		;compared to max
	jnc	enerr		;too many -- EN error
	inr	m		;store new # entries
	ora	a
	ral
	ral
	ral			;*8 bytes per entry
	lxi	h,intad-1
	call	adahl		;+ address - 1 = address of new entry - 1
	mvi	a,4
enab1:	pop	d
	call	momde		;two bytes to entry
	dcr	a		;four times
	jnz	enab1
	inx	h
	mvi	m,0		;and new eof
	ret
	endif			;end of NOT WILD conditional

;END
endcm:	if	sdisk and (not rtpak)
	call	closn		;close any OPEN files
	endif
	if	camac and false	;Northstar version
	call	close		;close any OPEN files
	endif
endc1:	if	compl		;ERROR entry point
	jmp	boot		;boot in COMPL version
	else
	mvi	a,csbrk		;break token to A
	jmp	stop2		;save break info and return to dmode
	endif

;EXEC [<expr> [,G] ]
	if	romsq
	if	compl
exec	equ	uferr
	else
exec:	call	ieval		;look for location
	lxi	h,srcad
	jc	exec1		;no loc, restore SOURC to SRCAD
	endif			;end of NOT COMPL conditional
	if	wild
execw:	dcx	b		;arg-1 should be 0-8 -- initial entry point
	lxi	d,9
	call	cmbdu
	jnc	roerr		;RO error if arg was not 1-9
	lxi	h,wexec		;address base of EXEC buffer
	dad	b
	dad	b		;address selected EXEC buffer pointer
	mov	a,m		;fetch low order address
	inx	h
	mov	h,m		;high order address to H
	mov	l,a		;and low order to L
	ora	h
	jz	roerr		;RO error if buffer entry is 0
	endif			;end of WILD conditional
	if	wild or not compl
	if	not wild
	mov	h,b
	mov	l,c		;location to HL
	endif			;end of NOT WILD conditional
exec0:	mov	a,m		;fetch first char of prog
	cpi	cr
	jnz	roerr		;not a program, fatal RO error
	inx	h		;else point to first length byte
exec1:	shld	sourc		;reset SOURC
	if	not compl
	call	gtcom		;look for comma
	lhld	eofad		;EOF address to HL
	jc	dmodx		;no comma, reset stacks and return to DMODE
	call	new1		;reset control stack
	mvi	d,'G'
	call	gtdsn		;skip G after comma
	lhld	sourc		;new program address to HL
	endif
	jmp	bakup		;reset TEXTP and continue execution at new prog
	endif			;end of NOT COMPL conditional
	else
exec	equ	exerr		;EX error in non-ROMSQ versions
	endif			;end of ROMSQ conditional


;end of STATES1
	page
;STATES2 05/21/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;statement routines for statements FOR through NULL


;FOR <var name> = <expr> TO <expr> [STEP <expr>]
for:	call	gtlhs		;get destination for ASIGN
	if	strng
	cpi	strst
	jz	snerr		;SN error if FOR <string var>
	endif
	lxi	h,inone		;integer 1 addr to HL
	if	float
	cpi	sngst
	jnz	for1		;integer FOR
	lxi	h,fpone		;floating 1. addr to HL
	endif
;At FOR1 the FOR var addr is in LHSAD, the default incr pointer in HL.
;The CSTACK is examined for a previous FOR entry with the same variable,
;and the entry is flushed if found.
for1:	push	h		;save default incr pointer
	lxi	b,(csfor shl 8) or csfor
	lhld	cstkp		;set BC, HL for CSDIG
for1a:	call	csdi1		;look for old FOR entry
	jc	for2		;not found
	push	h		;save current entry addr
	mov	a,m
	ani	1FH
	call	sbahl		;address next CSTACK entry
	xthl			;save next addr, current addr to HL
	lxi	d,-5
	dad	d		;address var addr bytes
	call	mvdem		;fetch FOR entry var addr to DE
	lhld	lhsad		;FOR var addr to HL
	call	cmdhu		;check if same variable
	pop	h		;next entry addr to HL
	jnz	for1a		;no match, try next
	mov	a,b		;match, entry type to A
	call	csrst		;and purge the entry
;FOR2 builds a new CSTACK FOR entry.  The entry consists of a type/length byte
;(1 byte), line # address, return textp and variable address (2 bytes each),
;and bound and increment values (2 or 4 bytes each).  After CPUSH allocates
;space for the entry, the CSTACK pointer is reset to ignore the protoentry
;in case FOR aborts, e.g. with a SN error.  Another CPUSH completes the entry.
for2:	mov	a,b
	call	cpush		;allocate new CSTACK FOR entry
	xchg
	if	not compl
	call	linbc
	cz	dcstd		;unincrement CSTACK direct count if direct
	endif
	lhld	lhsad
	xchg
	call	mvmde		;variable addr to entry
	mvi	a,2*fbyts+1	;skip two values
	call	sbahl		;address next CSTACK entry
	shld	cstkp		;reset CSTACK in case SN error in FOR
	inx	h		;address bottom of new FOR entry
	push	h		;and save pointer for bound/incr insertion
	call	let1		;get initial value and assign to var
	mvi	d,tot
	call	gtdsn		;skip TO token
	call	evalt		;get bound of desired type
	inx	h		;point to bound value
	xthl			;FOR entry pointer to HL
	pop	b		;bound pointer to BC
	call	moveb		;copy bound to entry
	xthl			;save entry addr for increment
	push	h		;and default incr
	mvi	d,stept
	call	gtd		;look for STEP
	pop	h		;default increment pointer to HL
	jc	for2a		;no STEP, take default value
	call	evalt		;get increment value
	inx	h		;and point to value
for2a:	xthl			;entry pointer to HL
	pop	b		;incr pointer to BC
	if	for0
	push	h		;save pointer to incr for FOR0 test
	endif
	call	moveb		;copy increment to entry and return
	mvi	a,csfor
	if	not for0
	jmp	cpush		;build the new CSTACK entry
	else
	call	cpush
;must now check for vacuous condition (e.g. FOR I=1 TO 0) in FOR0 version
	lhld	lhsad
	if	float
	lda	lhsty
	cpi	sngst
	if	f9511
	cz	lod95		;load floating value to 9511 stack
	else
	cz	fload		;load floating value to FACC
	endif
	endif
	mov	c,m
	inx	h
	mov	b,m		;fetch integer value to BC
	pop	d		;incr pointer to DE
	lxi	h,-fbyts
	dad	d		;bound pointer to HL
	call	bdtst		;test FOR condition
	rnc			;ok, just return
	lhld	cstkp
	call	next6		;purge the CSTACK entry just built
	call	eos
	mvi	b,nextt
	call	fndst		;find matching NEXT
	jnc	next7		;look for following comma, return or do another
	error	f, F, R		;not found, fatal FR error
	endif

;GOSUB <line #>
gosub:	call	goto		;find new textp and lnnum
gosu1:	mvi	a,csgos		;ON entry point
	push	b		;save return text pointer
	jmp	cpsh1		;build control stack entry and return

;GOTO <line #>
;GOTO is called by GOSUB to read a line #, look for it and
;check for end of statement garbage.  Branches to US error if illegal
;or no such line #, otherwise:
;Retn:	A	clobbered
;	BC	return text pointer, i.e. delimiter address
;	DE	line #
;	HL	address of <cr> preceding desired line in text
;	(textp)	same as HL
goto:	call	gtlno		;line # to DE
	jc	snerr		;no line #
goto1:	call	eos		;end of statement test -- IF entry point
goto2:	call	fdlno		;find line # -- interrupt, ON entry point
	jmp	bakup		;back up textp and return

;IF <expr> THEN {<line #> | <statement>}
ifcom:	call	gtexp		;evaluate the IF expression
	mvi	d,thent
	call	gtdsn		;skip THEN token
	mov	a,b
	ora	c
	jz	rem		;false -- scan to cr and return
	call	gtlno		;look for line #
	jc	xsta2		;none -- execute <statement> THENpart
	jmp	goto1		;GOTO line # and return

;INPUT [<quoted string>] [;] <var list>
;INPUT @<expr>, <var list>	[in SDISK versions]
;INPUT @ <var list>		[in PACKI versions]
;INPUT must scan two lines, the command line INPUT <varlist> and the line
;of data typed by the user.  The two text pointers are kept in TEXTP
;and TXTP2, and exchanged by FLIP.  The original <varlist> TEXTP
;is also PUSHed, to be available when REDOing.  If the data is of
;incorrect type or there are fewer data than variables, REDO.  If there are
;fewer variables than data, EXCESS IGNORED.
;DINP0 is in module SDISK.
;PAINP is in module PACKARD.
input:	call	idtst		;INPUT illegal direct mode
	if	sdisk
	call	gtsfn		;look for disk file specification
	jnc	dinp0		;disk INPUT
	endif			;end of SDISK conditional
	if	packi
	mvi	d,'@'
	call	gtd		;look for @
	jnc	painp		;found @, must be Packard INPUT @
	endif
	xra	a
	sta	omode		;turn on output mode
	mvi	d,'"'
	call	gtd		;look for quoted string
	cnc	prquo		;print it if present
	mvi	d,semic
	call	gtd		;look for ; and ignore if present
;INPU1 sets up pointers, and is entry point for retries after REDO message.
inpu1:	push	h		;save textp to redo
	shld	txtp2		;save textp to scan varlist
	mvi	a,'?'
	call	writc		;write a ?
	call	wrtsp		;and a space
	lhld	savtp
	shld	textp		;reset text pointer in case ^C typed
	call	gtlin		;get input line
	call	flip		;to scan varlist
;INPU2 gets the next variable and checks its type (string or nonstring).
inpu2:	call	gtlhs		;get destination
	call	flip		;to scan data
	if	strng
	cpi	strst
	jnz	inpu3		;nonstring variable
	call	evunq		;get string value, perhaps unquoted
	jc	inpu7		;no string, REDO
	jmp	inpu4		;assign string value to destination
	endif
;INPU3 gets a nonstring value.
inpu3:	call	gtlit		;get value
	jc	inpu7		;REDO if none
	if	strng
	cpi	strst		;check if string value
	jz	inpu7		;REDO if so
	endif
;INPU4 assigns a value to the destination variable.
inpu4:	call	asigv		;assign value to destination
	call	gtcnd		;look for comma not followed by delimiter
	jc	inpu6		;no more data or bad item
	call	flip		;to scan varlist
	call	gtcom		;look for comma
	jnc	inpu2		;continue with next var in varlist
	call	prntm		;else print EXCESS IGNORED
	db	'EXCESS IGNORED', cr, lf or 80H
;INPU5 is the exit from a successful INPUT.
inpu5:	pop	h		;pop REDO pointer
	ret			;and done
inpu6:	call	dtst0		;check if delimiter
	jc	inpu7		;no, REDO
	call	flip		;else scan varlist
	call	gtcom		;see if more vars
	jc	inpu5		;no, done
;INPU7 prints REDO message and retries after bad data.
inpu7:	call	prntm		;more vars than data, print REDO message
	db	'RED', 'O' or 80H
	pop	h
	jmp	inpu1		;reset varlist pointer and try again

;[LET] <var ref> = <expr>
let0:	call	bakup		;back up textp to get current char again
	call	dtest		;test if delimiter
	rnc			;null statement
let:	if	camac
	call	gtcam		;look for camac variable
	jnc	letcv		;perform camac LET
	endif
	if	wild
	mvi	d,wbuft
	call	gtd
	jnc	wletb		;BUFFER legal lhs in WILD version
	endif
	call	gtlhs		;get destination for ASSGN
let1:	call	gtequ		;skip = token -- FOR entry point
	call	evals		;evaluate the rhs
	jmp	asign		;and assign rhs to lhs
;letcv is in module CAMAC

;LIST [<line #>] [, <line #>]
	if	compl
list	equ	uferr		;UF error in COMPL version
	else
list:	call	gtlno		;min line # to DE, 0 if none
	call	findl		;look for it
	push	h		;save min line length byte address
	call	gtcom		;skip the comma, if any
	call	gtlno		;max line # to DE, 0 if none
	cc	cpld1		;complement to give default max = 0FFFFH
	call	eos		;check for garbage
	mov	b,d
	mov	c,e		;max to BC
	pop	h		;min length byte address to HL
list1:	mov	a,m		;fetch length byte
	ora	a
	rz			;eof -- done
	inx	h
	mov	e,m
	inx	h
	mov	d,m		;line # of current line to DE
	call	cmbdu		;compare unsigned to max
	rc			;max < (DE) -- return
	dcx	h		;point to line #
	call	ctest		;look for break char
	call	prntl		;print the line
	call	wcrlf		;and crlf
	jmp	list1		;and try next
	endif			;end of NOT COMPL conditional

;LOAD is in section STATES4.

;MOVE {TO | FROM} <expr>
;MOVE performs a block move of a ROMSQ version user program TO or FROM RAM.
;The specified location must not overlap either BASIC or user RAM.
	if	romsq and (not wild)
	if	compl
move	equ	uferr
	else
move:	call	gtcha
	push	psw		;save TO | FROM token
	call	gtexp		;get location
	if	camac		;RAM test only in segmented CAMAC versions
	lxi	d,ramorg
	else
	lxi	d,romorg	;base of BASIC to DE
	if	rom		;check if within BASIC in ROM versions
	lxi	h,eoram+1	;top of BASIC + 1 to HL
	call	mvtst		;check if loc within BASIC
	lxi	d,ramorg	;base of user RAM to DE
	endif			;end of ROM conditional
	endif			;end of NOT CAMAC conditional
	if	cpm and sdisk
	lhld	filet		;top of RAM is (FILET) in CP/M SDISK versions
	else
	if	strng
	lhld	strt		;top of RAM is (STRT) in STRNG versions
	else
	lhld	memt		;top of RAM is (MEMT) otherwise
	endif
	endif
	inx	h		;top of user RAM + 1 to HL
	mov	a,h
	ora	l
	cnz	mvtst		;check for loc within user RAM
	pop	psw		;restore token
	cpi	tot
	jnz	movef		;must be FROM
	push	b		;save destination
	lxi	d,srcad-2
	call	cplde		;2-SRCAD to DE
	lhld	eofad
	dad	d		;last - first + 1 = length
	xchg			;to DE
	pop	h		;destination to HL
	push	h
	push	d		;save dest, length
move1:	call	rtest		;check if RAM
	jnz	roerr		;ROM, RO error
	inx	h
	dcx	d
	mov	a,d
	ora	e
	jnz	move1		;check next
	lxi	b,srcad-1	;source to BC
	pop	d		;length to DE
	pop	h		;destination to HL
	jmp	moved		;block move and return
movef:	cpi	fromt
	jnz	snerr		;neither TO nor FROM, SN error
	ldax	b		;fetch first prog char
	cpi	cr
	jnz	roerr		;not a prog, RO error
	push	b
	push	b
	push	b
	lhld	sourc
	xthl			;save SOURC value
	inx	h		;point to first length byte
	shld	sourc
	call	last		;last loc to BC
	pop	h
	shld	sourc		;restore SOURC
	pop	d
	call	cplde		;-first to DE
	lhld	symta
	xchg			;-first to HL, SYMTA to DE
	dad	b		;last-first=length-1 to HL
	push	h		;save length for block move
	lxi	b,srcad
	push	b		;save destination for block move
	dad	b		;last needed + 1 to HL
	call	cmdhu
	jc	omerr		;too big, OM error
	pop	h		;destination
	pop	d		;length
	pop	b
	inx	b		;source
	call	moved		;block move into working space
	dcx	h		;point to new eof
	jmp	new1		;reset stacks and EOFAD
;MVTST is called from MOVE to assure location specified does not overlap
;with XYBASIC or with user RAM.  Issues fatal RO error if min <= loc <= max.
;Call:	BC	specified location
;	DE	min
;	HL	max+1
mvtst:	call	cmbdu
	rc			;loc < min
	xchg			;max+1 to DE
	call	cmbdu
	rnc			;loc >= max+1
	jmp	roerr		;else fatal RO error

	endif			;end of NOT COMPL conditional
	else
move	equ	exerr		;EX error in non-ROMSQ versions
	endif			;end of ROMSQ conditional

;NEW
	if	compl
newcm	equ	uferr		;UF error in COMPL version
	else
newcm:	if	romsq
	call	issrc		;check if addressing working space
	endif
	call	iitst		;NEW is illegal indirect
	if	sdisk
	call	closn		;close any OPEN files
	endif
new:	call	clea1		;clear symbol table -- LOAD, init entry point
	xra	a
	sta	trace		;traceoff
	sta	trap		;trapon
	lxi	h,srcad
	mov	m,a		;source text empty
new1:	push	h		;ADDLN, DMODX, MOVE FROM entry point
	call	stzap		;reset BREAK and FN symbol table entries
	call	rstor		;restore the READ pointer
	if	not wild
	call	disab		;disable interrupts
	endif
	pop	h
	shld	eofad		;set eof address -- LOAD entry point
	endif			;end of NOT COMPL conditional
cspst:	shld	cstkp		;clear control stack -- call to reset it
	shld	estkp		;reset expr stack pointer
	ret

;NEXT [<var name> [, <var name>]* ]
;First the most recent CSTACK FOR entry is found.
next:	lxi	b,(csfor shl 8) or csfor
	call	csdig		;look for CSTACK FOR entry
	jc	nferr		;not found -- fatal NF error
	push	h		;save FOR entry address
	lxi	d,-5
	dad	d		;point to var address bytes
	mov	b,m
	dcx	h
	mov	c,m		;fetch var addr to BC
	push	h		;and save var addr pointer
	lhld	textp
	shld	savtp		;save TEXTP in case retry necessary
	push	b
	call	fdvar		;look for var name after NEXT
	inx	h
	pop	b
	jc	next1		;no name
;NEXT <var name> specified, so the FOR <var name> is compared to it.
	xchg			;NEXT var addr DE, type addr HL
	call	cmbdu		;compare FOR and NEXT <var name>s
	jz	next3		;FOR and NEXT <var name>s match
;<var name>s do not match, so CSTACK FOR entry is poped.
	lhld	savtp
	shld	textp		;restore textp to get var name again
	pop	h
	pop	h		;FOR entry address to HL
	mov	a,m		;type/length byte to  A
	mov	b,a		;saved in B
	ani	1FH		;mask to length
	call	sbahl		;address next entry
	mov	a,b		;type/length restored
	call	csrst		;purge the FOR entry from CSTACK
	jmp	next		;and try again
;When no <var name> is specified after NEXT, the last CSTACK FOR entry
;<var name> is assumed.  The type byte must be found for correct tracing.
next1:	lhld	symta
next2:	call	stnxt		;address next symbol table entry
	xchg
	call	cmbdu		;compare to desired value addr
	xchg
	jnc	next2		;var addr >= next entry, try another
	xchg			;type byte addr to HL
;At NEXT3 the desired <var name> is known.  The stack contains the FOR entry
;addr and the FOR entry var addr, BC contains the var addr, and HL
;contains the type byte pointer for the specified <var name>.
;The trace info [<line #>]<var name> is printed if desired.
next3:	if	float
	mov	a,m
	ani	1FH		;mask to type
	sta	lhsty		;save destination type
	endif
	if	not compl
	lda	trace
	ora	m		;sign set iff tracing
	sta	vtrac		;set VTRAC for ASSGN
	inx	h		;point to name
	jp	next4		;no tracing desired
	call	bprnt		;print trace line #
	call	prtst		;print var name
	endif
;At NEXT4 the bound & incr pointers are found in preparation for incrementing,
;and the old value is replace by value+increment.
next4:	pop	h		;FOR entry var addr pointer to HL
	lxi	d,-fbyts
	dad	d
	push	h		;save incr pointer
	push	h
	dad	d
	xthl			;save bound pointer, incr pointer to HL
	push	b		;and save var addr
	if	float
	lda	lhsty
	cpi	intst
	jz	nxt4i		;integer FOR
;Now floating value is replaced by old value plus increment.
	push	b
	if	f9511
	call	lod95		;incr to 9511 stack
	pop	h
	call	lod95		;var value to 9511 stack
	call	fadd		;add increment to var value
	mvi	a,pto95
	call	o9511		;copy stacktop
	call	fet95		;fetch result from 9511 stack
	else
	xthl			;save incr address, var address to HL
	call	fload
	pop	h		;incr address to HL
	if	fpbcd
	call	fadd		;add var value to increment
	else
	call	fincr		;instead of CALL FADD
	call	etest		;perform significance test
	endif
	endif
	pop	h		;restore var addr
	if	compl
	if	f9511
	call	sto95		;result to var addr
	else
	call	fstor		;result to var addr
	endif
	else			;NOT COMPL
	push	h
	if	f9511
	call	sto95		;result to var addr
	else
	call	fstor		;result to var addr
	endif
	pop	h		;restore var addr for tracing
	mvi	b,sngst		;type to B
	call	trval		;print trace info if desired
	endif
	jmp	next5
	endif
;Integer value is replaced by old value plus increment.
nxt4i:	mov	c,m
	inx	h
	mov	b,m		;incr to BC
	pop	h
	mov	e,m
	inx	h
	mov	d,m		;value to DE
	push	h
	call	iadd		;integer add incr+value
	cc	iover		;overflow, nonfatal OV error
	pop	h
	call	asigi		;assign and print trace info if desired
;At NEXT5 the bound is compared to the value to test for loop termination.
next5:	pop	h		;bound pointer to HL
	pop	d		;incr pointer to DE
	call	bdtst		;test for completion condition
	pop	h		;FOR entry addr to HL
	jnc	cspop		;don't exit loop -- get new text addr & return
next6:	if	compl
	lxi	d,-(2*fbyts+7)	;- entry length to DE
	else
;Before purging the FOR-entry the CSTACK direct count must be updated.
	dcx	h
	mov	a,m
	dcx	h
	ora	m		;check line # of FOR
	cz	dcstd		;decrement CSTKD if direct
	lxi	d,-(2*fbyts+5)
	endif
	dad	d		;point to next entry
	mvi	a,csfor
	call	csrst		;reset stack pointer
next7:	call	gtcnd		;look for comma
	jnc	next		;do another NEXT if comma
	ret
nferr:	error	f, N, F		;fatal NF error

;NULL <expr>
	if	not wild
null:	call	gtbex		;get byte-value expr
	mov	a,c
	sta	nulls		;value to NULLS
	ret
	endif			;end of NOT WILD conditional

;end of STATES2
	page
;STATES3 12/05/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;statement routines for statements ON through WAIT


;ON <expr> {GOTO | GOSUB} <line #> [, <line #>]*
on:	call	gtexp		;value to BC
	call	gtcha
	cpi	gotot
	jz	ongot
	cpi	gsubt
	jnz	snerr		;SN error if not GOTO or GOSUB
	stc
ongot:	push	psw		;carry set for GOSUB, reset for GOTO
	dcx	b		;value - 1 to BC
	mov	a,b
	ora	a
	inx	b
	cnz	on3		;count negative or big
on1:	call	gtlno		;line # to DE
	dcr	c		;decrement count
	jz	on2		;do current line #
	call	gtcom		;look for comma
	jnc	on1		;keep going
	call	onerr		;nonfatal ON error if line # list exhausted
on2:	call	gtdel		;scan to end of statement
	call	goto2		;set TEXTP and LNNUM
	pop	psw
	rnc			;done if GOTO
	jmp	gosu1		;else build GOSUB control stack entry
on3:	mvi	c,1
	jm	onerr		;replace negative values by 1
	mvi	c,255		;and positive by 255
onerr:	error	n, O, N		;nonfatal ON error
	ret			;and return

;OUT <expr>, <expr>
	if	not wild
put:	call	gtbex		;get byte expression
	mov	a,c
	sta	wport + 1	;set port #
	call	gtcbe		;get comma, byte expr
	mov	a,c
	jmp	wport		;OUT it and return
	endif			;end of NOT WILD conditional

	if	epstn
;PLOT <expr> [, <expr>]*
plot:	lda	colum		;find current column
	push	psw		;and save
plot1:	xra	a
	sta	colum		;clear column count
	call	gtbex		;get byte valued expr
	mov	a,c
	call	writc		;write the char
	call	gtcom		;look for comma
	jnc	plot1		;more
	pop	psw
	sta	colum		;restore column count
	ret
	endif

;POKE <expr>, <expr>
	if	not wild
poke:	call	gtexp
	push	b		;save location
	call	gtcbe		;get comma, byte expr
	mov	a,c
	pop	b
	stax	b		;store data in location
	ret
	endif			;end of NOT WILD conditional

;{PRINT | ?} [@<expr>] [[<print item>] {, | ;}]* [<print item>]
;<print item> ::= [UNS | TAB | SPC] <expr> | <string> | <camvar name>
;PRINT first PUSHes a return to itself, so the print routines may be CALLed
;and simply RETurn.  The routines for comma and semicolon check for following
;delimiter, so a delimiter found after any other item produces a crlf.
;DPRIN is in module SDISK.
;WILD PRINT is in module WILD, and calls PRIN0.
	if	not wild
print:	if	sdisk
	call	gtsfn		;look for file number
	jnc	dprin		;PRINT to file
	endif
	endif
prin0:	lxi	b,prin0
	push	b		;push common return to print more
	call	gtcho		;look at next char
	cpi	'"'		;look for quoted string first to avoid eval
	jz	prqu0		;quoted string
	if	camac
	cpi	drivt
	jz	prdri		;print DRIVER
	call	gtcam		;look for camvar name
	jnc	prcam		;print the camvar value
	endif
	call	eval		;look for expression
	jnc	prval		;expression found, print its value
	call	dtest
	jz	prdel		;delimiter, print crlf and exit
	if	not wild	;only one print item allowed in WILD versions
	call	read1		;read the char
	cpi	','
	jz	prcom		;comma
	cpi	semic
	jz	prsem		;semicolon
	cpi	tabt
	jz	prtab		;TAB
	if	not camac
	cpi	spct
	jz	prspc		;SPC
	endif
	if	not float	;UNS allowed as print fn in nonfloat versions
	cpi	unst
	jz	pruns
	endif
	if	not strng	;CHR$ allowed as print fn in nonstring versions
	cpi	chrst
	jz	prchr
	endif
	endif			;end of NOT WILD conditional
	jmp	snerr		;else SN error
;print delimiter
prdel:	pop	b		;pop the PRINT return
	jmp	wcrlf		;write crlf and exit
;print the value of an EVALuated expression
prval:	call	fetc1		;fetch the value
	iftyp	prsng,prstr	;branch if noninteger
prinv:	mov	a,b
	ora	a		;check sign of integer value
	jp	priv1		;print unsigned if positive
	if	wild
	jmp	wrtbs
	else
	call	wrtbs		;otherwise write signed
	jmp	wrts0		;write trailing space and return
	endif
priv1:	mvi	a,' '		;leading space to A
	if	wild
	jmp	wrtb1
	else
	call	wrtb1		;write unsigned with leading space
	jmp	wrts0		;write trailing space and return
	endif
	if	not wild
;comma, space to the next tab stop
prcom:	call	prco1		;do the comma
				;and fall through to PRSEM for delimiter test
;semicolon
prsem:	call	dtst0		;check if next char is delimiter
	rc			;nondelimiter, continue printing
	pop	b		;else pop the PRINT return
	ret			;and exit from PRINT
prco1:	lhld	colum
	mov	a,l		;fetch current column
	ora	a
	rz
	if	float		;comma column width is 14 if floating
	lxi	b,14		;comma column width to BC
	mov	l,b		;0 to L
	jp	prco2		;column is < 128 currently
	mvi	l,126		;else make L 14*9
	sub	l		;and reduce column accordingly
prco2:	dad	b		;add comma width
	sub	c		;subtract comma width from column
	jp	prco2		;repeat until negative
	else			;comma column width is 8 if nonfloating
	ori	7
	inr	a		;compute desired column
	mov	l,a		;save column in L
	endif
	mov	a,h
	ora	a
	mov	a,l		;fetch desired column
	jz	prco3		;width 0, suppress cr check
	cmp	h		;compare to width
	jnc	wcrlf		;write crlf if >= width
prco3:	mov	c,a
	jmp	prta1		;else tab to column desired
;UNS
	if	not float
pruns:	call	gtexp		;get argument
	jmp	priv1
	endif
;TAB
prtab:	call	prta0		;get arg mod width
prta1:	lda	colum
	cmp	c
	rz			;found desired column
	call	prta2		;else write spaces until there
	jmp	prta1
prta2:	jc	wrtsp		;write a space if left of desired column
	jmp	wcrlf		;else write a crlf
;PRTA0 evaluates <expr> mod width for TAB and SPC
prta0:	call	gtexp		;get argument
	lda	width
	ora	a		;Zero set iff width 0
	mov	e,a
	mvi	d,0
	push	d
	cnz	opmod		;evaluate arg mod width unless width 0
	pop	d		;restore width to DE
	mov	a,b
	ora	a		;check if arg was negative
	cm	iadd		;yes, add width to result to make positive
	ret
;SPC
	if	not camac
prspc:	call	prta0		;get arg mod width
	mov	a,c
	ora	a
prsp1:	rz			;done
	call	wrtsp		;else write a space
	dcr	c		;decrement space count
	jmp	prsp1		;and repeat
	endif
	endif			;end of NOT WILD condtional
;quoted string
prqu0:	call	read1		;read the "
prquo:	mov	d,h
	mov	e,l		;address of first char to DE
	call	gtclq		;scan to close quote or cr
	mov	a,l		;fetch last+1 address
	sub	e		;last+1 - first = # chars
prst0:	mov	c,a		;to C
	jmp	prstr		;and print the string
	if	not strng	;allow CHR$ as print fn
prchr:	call	gtbex		;get byte-value expr
	mov	a,c
	jmp	writc		;write value and return
	endif
;floating point value
	if	float
prsng:	call	fout		;convert FACC to string
	if	wild
	jmp	prstl
	else
	call	prstl		;write the string
	jmp	wrts0		;write trailing space and return
	endif
	endif
;PRCAM is in module CAMAC.

;RANDOMIZE <expr>
	if	not wild
rndiz:	call	gtexp
	mov	h,b
	mov	l,c		;value to HL
	shld	randx		;and it becomes new seed
	ret
	endif			;end of NOT WILD conditional

;READ <var list>
;READ must scan the command line READ <var list> and lines of DATA [<expr>,]*.
;The text pointers are kept in TEXTP and TXTP2 and exchanged by FLIP.
read:	lhld	rdptr
	shld	txtp2		;READ pointer to TXTP2
reada:	call	gtlhs		;get destination for ASSGN
	if	strng
	push	psw		;save var type in string version
	endif
	call	flip		;to scan data
	call	gtcnd		;see if comma (in DATA <lit>,<lit>...) next
	cc	reade		;no, find next DATA statement
	if	strng
	pop	psw		;restore var type
	cpi	strst
	jnz	readb		;not a string var
	call	evunq		;string var, get value (possibly unquoted)
	jmp	readd
	endif
readb:	call	gtlit		;look for value
	jc	readf		;no value found
readd:	call	asigv		;assign value to variable
	call	flip		;to scan varlist
	call	gtcom		;look for comma
	jnc	reada		;read another
	lhld	txtp2		;else recover new RDPTR
	shld	rdptr		;and store it
	ret			;and done
;READE finds the next DATA statement.
;Issues nonfatal OD error and retries if no more DATA.
;Issues fatal SN error if SN error in DATA.
reade:	call	dtst0		;check if at delimiter
	jc	readf		;no -- SN error in DATA
	mvi	b,datat
	call	fndst		;find next DATA
	rnc			;OK if found
	error	n, O, D		;nonfatal OD error
	call	rstor		;do a RESTORE
	shld	textp
	jmp	reade		;and try again
;READF issues fatal SN error for bad DATA items.
readf:	xchg			;DATA textp to DE
	call	fndln		;find its line number
	shld	lnnum		;reset line # for error message
	error	f, S, N		;fatal SN error

;REM <unquoted string>
rem:	call	gtcha
	cpi	cr
	jnz	rem		;scan to <cr>
	call	bakup		;back up text pointer
	inx	h		;let HL point to next byte in case on-line
	ret

;RENUM [<line #> [, <line #> [, <line #>]]]
	if	editc
	if	compl
renum	equ	uferr
	else
;RENU0 gets an optional comma followed by <line #>, default value from DE.
renu0:	call	gtcnd		;look for comma
	cnc	gtlno		;if comma look for <line #>
	xchg			;to HL
	ret			;and return
;First RENUM must get arguments.
;RNOLD gets the old line # of first renumbered line,
;RNNEW gets the new line # of first renumbered line, and
;RNINC gets the renumbering increment.
renum:	if	romsq
	call	issrc		;must be addressing working space
	endif
	call	gtlno		;look for <line #>
	jnc	renu1		;<line #> given
	call	findl		;none given -- look for line 0
	stc			;to skip following FDLNO
renu1:	cnc	fdlno		;find the specified line #
	push	h		;save first renumbered line location
	call	modem		;line # of first renumbered line to DE
	push	d		;and saved
	xchg
	shld	rnold		;and saved in RNOLD
	lxi	d,10		;default arg2 is 10
	call	renu0		;get increment
	mov	a,h
	ora	l
	jz	userr		;fatal US error if 0 (e.g., RENUM 10,,)
	shld	rninc		;increment saved in RNINC
	xthl			;save increment, first line line # to HL
	xchg			;and then to DE as default arg3
	call	renu0		;get destination <line #>
	shld	rnnew		;and saved in RNNEW
	xchg			;destination to DE
	call	iitst		;RENUM illegal indirect
;Now test if the specified args give too large a max <line #>.
	pop	b		;increment to BC
	pop	h		;location to HL
	push	b
	push	d
	push	h		;save args
	mov	a,m		;fetch length
	call	adahl		;address second line
renu2:	mov	a,m		;fetch length
	ora	a
	jz	renu3		;eof
	call	adahl		;address next line
	xchg
	dad	b		;compute its eventual line #
	xchg
	jnc	renu2		;continue if no overflow
	jmp	userr		;too large, US error
;Now test if last line # before first is < new first.
renu3:	lxi	b,0		;last line before first initially 0
	pop	d		;first line # length byte address to DE
	lxi	h,srcad		;start at beginning of source text
renu4:	call	cmdhu		;compare first to current
	jz	renu5		;matched, BC now has last
	inx	h
	mov	c,m
	inx	h
	mov	b,m		;line number to BC
	dcx	h
	dcx	h
	mov	a,m		;fetch length byte
	call	adahl		;address next
	jmp	renu4		;and try it
renu5:	pop	d		;new first line # to DE
	push	d		;and resave
	call	cmbdu		;compare to line # before first
	jnc	userr		;US error if previous is larger
	lhld	textp
	push	h		;save TEXTP
	call	lnref		;change <line #> references in source text
	pop	h
	shld	textp
;Now change the actual <line #>s.
	lhld	rnold
	xchg			;RNOLD to DE
	call	findl		;find (possibly new) location of first changed
	pop	d		;RNNEW to DE
	pop	b		;RNINC to BC
renu6:	mov	a,m		;fetch length of next line
	ora	a
	jz	renu7		;all line numbers changed
	push	h		;save current length byte address
	call	adahl		;address next line
	xthl			;save next, recover current
	inx	h
	mov	m,e
	inx	h
	mov	m,d		;change the line number
	pop	h		;next line length byte address to HL
	xchg
	dad	b
	xchg			;update line number
	jmp	renu6		;and renumber more
;Check break bytes for flag indicating illegal <line #> in line.
renu7:	mvi	b,0		;clear bad line flag
	lxi	h,srcad		;begin at the beginning
renu8:	mov	a,m
	ora	a
	jz	renux		;eof, done
	push	h		;save length byte loc
	inx	h
	inx	h		;skip line # bytes
	inx	h		;and address break byte
	mov	a,m		;fetch it
	ora	a		;check bit 7
	jp	renu9		;ok if not set
	dcx	h
	dcx	h		;address line # byte
	call	prntl		;print the bad line
	call	wcrlf		;and crlf
	mvi	b,255		;and set bad line flag
renu9:	pop	h
	mov	a,m		;refetch length
	call	adahl		;address next line
	jmp	renu8		;and try next line
renux:	push	b		;save bad line flag
	call	addl3		;text may have moved, reset stacks
	pop	psw		;recover bad line flag
	ora	a
	rz			;successful RENUM
	jmp	userr		;else US error message
	endif
	else
renum	equ	exerr		;EX error in non-EDITC versions
	endif

;RESTORE [<line #>]
restr:	call	gtlno		;look for line #
	jc	rstor		;none, take least
	call	fdlno		;find the line
	jmp	rsto1		;and reset the read pointer
rstor:	if	romsq
	lhld	sourc
	else
	lxi	h,srcad
	endif
rsto1:	dcx	h		;point to <cr> preceding line
	shld	rdptr		;let read pointer address <cr>
	ret

;RETURN
retrn:	call	eos		;end of statement test
	lxi	b,(csgos shl 8) or csint
	call	csdig		;look for gosub or interrupt cstack entry
	jc	rgerr		;not found -- RG error
	call	cspop		;pop cstack information
	cmp	b		;zero set if gosub, reset interrupt
	jz	csrst		;GOSUB entry -- reset stack & return
	push	psw
	call	mvdem		;interrupt table byte addr to DE
	ldax	d
	ori	40H		;resume interrupt
	stax	d
	pop	psw
	jmp	csrst		;reset control stack and return
rgerr:	error	f, R, G		;fatal RG error

;RUN [<line #>]
	if	compl
run	equ	uferr		;UF error if COMPL version
	else
run:	if	sdisk
	call	closn		;close any OPEN files
	endif
	if	camac and false	;Northstar version
	call	close		;close any OPEN files
	endif
	call	gtlno		;look for line #
	push	psw
	push	d
	call	iitst		;RUN illegal indirect
	call	clea1		;clear CSTACK, symbol table, string space
	if	not wild
	call	disab		;disable interrupts
	endif
	call	rstor		;restore READ pointer
	pop	d
	pop	psw
	jnc	goto2		;line # specified, execute from there
	shld	textp		;else point to <cr> before source text
	ret			;and execute from there
	endif

;SAVE is in section STATES4.

;SCALL <expr> [, <integer var ref>]*
;SCALL loads BC, DE and HL with the values of the <integer var ref>s, if any,
;and branches to the <expr> address.  When the user routine RETurns,
;the values in BC, DE and HL are assigned to the <integer var ref>s.
scall:	call	gtexp
	lhld	textp
	shld	savtp		;save textp for rescan on return
	lxi	h,sretn
	push	h		;return address to stack
	push	b		;branch address to stack
	lxi	d,3		;maximum parameter count to DE
scal1:	call	gtcnd		;look for comma not followed by delimiter
	jc	scal2		;none, set up registers and branch to user
	call	gtiva		;perform integer var ref
	jc	mcerr		;not a var ref
	push	b		;save parameter value
	dcr	e		;decrement parameter count
	jnz	scal1		;get more parameters if not three already
	call	gtcnd		;check if more parameters
	jc	scal2		;no more
	error	n, M, C		;nonfatal MC error if too many
;At SCAL2, DE contains 3 - # parameters specified, and the parameter values
;are PUSHed.  By adding DE to the address POP3, the branch address
;is computed to POP 3, 2, 1 or 0 values before branching to the user routine.
scal2:	lxi	h,pop3
	dad	d		;compute pop address
	pchl			;branch to it
;SRETN is the entry point from the user routine's RETurn.
sretn:	push	h
	push	d
	push	b		;save registers
	lhld	savtp
	shld	textp		;restore TEXTP
	lxi	d,0		;# of POPed registers to DE
sret1:	call	gtcnd		;look for comma
	jc	scal2		;done, POP extras and return
	inr	e		;increment POPed count
	mvi	a,3
	cmp	e
	jc	nextc		;too many, scan to end and return
	push	d
	call	gtlhs		;get destination
	inx	h		;point to high order destination
	pop	d
	pop	b		;value to BC
	call	asigi		;assign integer value, traced
	jmp	sret1		;and  repeat

;SETTIME <expr> [, <expr>] [, <expr>]
	if	realt and not camac	;SETTIME in REALT versions only
sttim:	lxi	h,timex+3	;HL addresses hour count
	lxi	d,24		;max # hours
	ora	a		;reset Carry
	call	stti2		;get hours count and set clock
	lxi	d,60		;max # minutes or seconds
	call	stti1		;get minutes count and set clock
	call	stti1		;get seconds count and set clock
	dcx	h
	mvi	m,255 and -20	;reset 20ths count
	ret
stti1:	dcx	h		;point to next time component
	push	h
	call	gtcnd		;look for comma
	pop	h
	lxi	b,0		;default to 0 if unspecified
stti2:	cnc	gtexp		;get desired count
	call	cmbdu		;compare to max
	cnc	fcer0		;too big, nonfatal FC error and return 0
	mov	a,c		;fetch count
	sub	e		;subtract bias
	mov	m,a		;and store to set clock
	ret
	endif

;STOP
	if	compl		;STOP boots in COMPL version
	if	wild
stop	equ	uferr		;UF error in WILD RTPAK
	else
stop	equ	boot
	endif
	else
stop:	mvi	a,csbrk		;break token to A
stop1:	push	psw		;line break entry point
	call	linbc		;line # to BC
	jz	dmod2		;direct mode -- do not save info
	call	prtm0		;turn on OMODE, print BREAK message
	db	cr, lf, 'BREAK AT LINE', ' ' or 80H
	call	wrtbu		;print line # unsigned
	pop	psw
stop2:	call	cpush		;break info to control stack -- END entry point
	jmp	dmod2		;and continue in DMODE
	endif			;end of COMPL conditional

;TIME
	if	(not camac) and (not realt)
time:	mvi	a,cntlg
	call	writc		;ring a bell
	lxi	h,0
	call	readc		;get and ignore first char
	call	timer
	jnc	exerr		;counted to 0 -- EX error
	call	ctest		;read the char TIMER saw
	push	h		;save minute count
	mov	a,h
	sta	timex+4		;minute count / 256 to TIMEX+4
	mov	b,h
	mov	c,l
	lxi	d,60
	call	divd0		;min count / 60 = sec count to HL
	pop	d
	call	cplde		;complement minute count
	xchg
	shld	timex		;complmented minute count to TIMEX
	call	cplde		;complement second count
	xchg
	shld	timex+2		;complemented second count to TIMEX+2
	ret
	endif

;TRACE | UNTRACE
;TRAP | UNTRAP
	if	compl
tcon	equ	uferr		;UF error if COMPL version
tcoff	equ	uferr
tpon	equ	uferr
tpoff	equ	uferr
	else
tcon:	stc
tcoff:	sbb	a		;A gets 0 if no carry, 255 if carry
	sta	trace
	ret
tpoff:	stc
tpon:	sbb	a
	sta	trap
	ret
	endif			;end of NOT COMPL conditional

;UNBREAK [<line #> | <var list>]
;UNBREAK removes all breakpoints.
;UNBREAK <line #> removes the breakpoint (if any) on the specified line.
;UNBREAK <var list> removes breakpoints on the specified variables.
	if	compl
unbrk	equ	uferr		;UF error if COMPL version
	else
unbrk:	call	gtlno		;see if line # present
	jc	unbr2		;no
;Remove breakpoint from line # in DE.
	if	romsq
	call	isrom		;must be running in RAM
	endif
	call	fdlno		;find the line
;UNBR1 removes the breakpoint on line addressed by HL.
unbr1:	inx	h		;move pointer from length byte
	inx	h		;   past line #
	inx	h		;   to break byte
	mvi	m,0		;and zero it
	ret
unbr2:	call	dtest		;see if var list present
	jc	ubvar		;yes
	lhld	symta		;no, unbreak all lines and variables
;UNBR3 removes all variable breakpoints.
unbr3:	call	stnxt		;get next symbol table entry
	jc	unbkl		;end of table -- unbreak lines and return
	ldax	d
	ani	7FH
	stax	d		;unbreak one entry
	jmp	unbr3
;UBVAR removes breakpoints on the specified <var list> variables.
ubvar:	call	fdvar		;find var ref
	jc	snerr
	ldax	d
	ani	7FH
	stax	d		;unbreak it
	call	gtcnd		;look for comma
	jnc	ubvar		;unbreak another var
	ret
	endif			;end of NOT COMPL conditional

;UNTRACE and UNTRAP are under TRACE and TRAP above.

;WAIT <byte expr> , <byte expr> [, <byte expr>] [,$]
	if	not wild
wait:	call	iinfo		;get interrupt info
	mov	a,c
	ral
	ral
	mov	c,a		;$ bit to C7
	mov	a,b		;port # to A
	call	rdp1		;read
	ora	e		;mask
	xra	d		;compare
	jz	wait1
	stc			;carry set iff compare nonzero
wait1:	rar			;A7 set iff compare nonzero
	xra	c		;sign set iff keep waiting
	rp			;condition fulfilled -- return
	lhld	savtp
	shld	textp		;reset text pointer to parse WAIT again
	ret
	endif			;end of NOT WILD conditional


;end of STATES3
	page
;STRINGS 11/1/79
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979 by Mark Williams Company, Chicago
;string routines and functions

	if	strng

;String space consists of RAM from (MEMT)+1 to (STRT).  String variables are
;stored from (MEMT)+1 to (STRPT), and (STRPT)+1 to (STRP2) is used for
;temporary string storage.  The string temporaries at STEMP contain the
;addresses of ESTACK string entries, as the strings may move during garbage
;collection.

;ATEMP assigns the ESTACK location in HL to a string temporary.
;The temporary is freed when the string value is FETCHed.
;Retn:	A	clobbered
;	BCDEHL	preserved
atemp:	push	d
	xchg			;location to DE
	lxi	h,stemp		;temp base addr to HL
	mov	a,m		;fetch # temps in use
	inr	a		;and bump
	cpi	stmax		;compare to # available
	jnc	sterr		;too many, fatal ST error
	mov	m,a		;store new # in use
	add	a		;# * 2 bytes per temp
	call	adahl		;+ base = bottom of new temp
	mov	m,d
	dcx	h
	mov	m,e		;location to temp
	xchg			;restore loc to HL
	pop	d		;and restore DE
	ret
sterr:	error	f, S, T		;fatal ST error

;SCOPY copies a new string to string temp space for GET$, CHR$, and STR$.
;Call:	C	string length
;	DE	string location
;Retn:	B	preserved
;	C	string length
;	DE	string location (in string space)
;	HL	top of string temp space used
;SCOP0 is called from concatenation (SADD) and assignment (SCOPV) to
;fetch the string value addressed by HL and then copy to string space.
scop0:	call	fetcs		;fetch string arg
scopy:	mov	a,c
	call	stfre		;assure sufficient free space
	mov	a,c
	ora	a
	rz			;null string, just return
	push	b		;save length
	call	bcde		;length to E, location to BC
	inx	h		;next available location to HL
	push	h		;save location
	call	movd0		;copy string to string space
	dcx	h
	shld	strp2		;new top of string temp space used
	pop	d		;restore location
	pop	b		;and length
	ret

;SCOPV copies the value addressed by HL to string space for string assignment.
scopv:	push	h
	inx	h
	mov	a,m		;fetch length
	lhld	strpt
	shld	strp2		;clear string temp spacce
	call	stfre		;assure sufficient free space
	pop	h		;restore value pointer
	push	h
	call	scop0		;fetch the value and copy to string space
	shld	strpt		;and reset string space var pointer
	pop	h		;restore the value pointer
	push	h
	inx	h
	inx	h
	mov	m,e
	inx	h
	mov	m,d		;new location to value entry
	pop	h		;restore HL
	mov	a,m		;and restore type
	ret

;STFRE assures that (A) bytes of string space is availfable.
;Garbage collection is performed if required, and an OS error if insufficient.
;Call:	A	amount of string space needed
;Retn:	BC,DE	preserved
;	HL	top of used string space (STRP2)
stfre:	push	b
	push	d
	mov	c,a
	mvi	b,0		;desired space to BC
	lhld	strt
	xchg			;top of string space to DE
	lhld	strp2		;current used string space top to HL
	push	h		;and saved
	dad	b		;current top + desired amount to HL
	cnc	cmdhu		;compare top to needed
	jnc	pop3		;top >= needed, restore and return
	pop	h
	push	b		;save desired
	call	garbg		;garbage collect
	xchg
	pop	b
	push	h		;save used top
	dad	b
	cnc	cmdhu		;compare top to needed
	jnc	pop3		;ok after garbage collection
	error	f, O, S		;fatal OS error

;GARBG is the string space garbage collector.  All strings referenced by
;string variables or temporaries are compacted to the base of string space.
;Retn:	DE	top of used string space (STRPT)
;	HL	top of string space (STRT)
garbg:	lhld	memt
	inx	h		;point to first byte of string space
	shld	gcmin		;min value to accept
;First the string with the minimum location >= GCMIN is found.
;GCLST contains the least so far, and GCLOC the location of the least.
garb1:	lxi	h,-1
	shld	gclst		;string space loc of least string found
	lhld	symta
	push	h
garb2:	pop	d		;set initial values for NXTST
	call	nxtst		;find next symbol table string var
	jc	garb4		;no more string entries
	push	d
	push	h
	mov	d,b
	mov	e,c		;next string location to DE
	call	gccmp		;compare to min and least
	pop	h		;restore next entry pointer to HL
	jc	garb2		;< min or >= least, try next
	xchg
	shld	gclst		;else current becomes new least
	xchg
	shld	gcloc		;and gcloc stores its address
	jmp	garb2		;and try next
;All strings checked, must check if least was found.
garb4:	lhld	gclst
	mov	a,h
	ana	l
	inr	a
	jz	garb5		;no least found, string var compacting done
;Move the least string to the top of available string space.
	lhld	gcloc
	dcx	h
	dcx	h
	dcx	h		;point to length byte
	call	tpmov		;move string temps pointing within string
	call	stmov		;move the string
	jmp	garb1		;and continue compacting
;String vars compacted, now compact the string temps.
garb5:	lhld	gcmin
	dcx	h
	shld	strpt		;store new string var pointer
	lhld	strt
	shld	gclst		;set GCLST for GCCMP
	lxi	h,stemp
	mov	a,m		;fetch # temps in use
garb6:	dcr	a
	jm	garb7		;all temps moved, garbage collection done
	push	psw		;save # temps still to move
	call	modem		;fetch temp loc to DE
	push	h
	push	d		;save temp loc
	xchg
	call	modem		;fetch loc of temporary to DE
	call	gccmp		;compare to min and least
	pop	h		;temp loc to HL
	cnc	stmov		;move temp to base of avail space
	pop	h
	pop	psw
	jmp	garb6		;and look for more temps
garb7:	lhld	gcmin
	dcx	h
	shld	strp2		;store new top of string temp space
	xchg
	lhld	strt		;and return string space top in HL
	ret
gccmp:	lhld	gcmin
	call	cmdhu
	rc			;location < min, try next
	lhld	gclst
	call	cmdhu
	cmc			;location >= least, try next
	ret

;STMOV moves a string during garbage collection.
;Call:	HL	pointer to string length byte
;	(GCMIN)	destination
;Retn:	(GCMIN)	next unused location in string space
stmov:	mov	e,m		;length to E
	inx	h
	push	h		;save location address
	mov	c,m
	inx	h
	mov	b,m		;location to BC
	lhld	gcmin		;destination to HL
	push	h		;and saved
	call	movd0		;move string in string space
	shld	gcmin		;and store new min
	pop	d
	pop	h
	mov	m,e
	inx	h
	mov	m,d		;and copy new loc to value
	ret

;TPMOV moves temporaries during garbage collection.
tpmov:	lxi	d,stemp
	ldax	d		;fetch # temps in use
tpmo1:	dcr	a
	rm			;no more temps to check
	push	h		;save string value location
	push	h
	xchg
	call	modem		;fetch ESTACK location of temp to DE
	xthl			;save STEMP pointer, string value ptr to HL
	push	psw		;save # temps still to check
	push	h		;and string value ptr
	xchg			;ESTACK temp loc to HL
	call	modem		;fetch temp loc to DE
	xthl			;save string temp pointer, value ptr to HL
	push	d		;and save temp loc
	mov	c,m		;length to C
	call	modem		;location to DE
	mvi	b,0
	call	cplde		;- loc to DE
	pop	h		;temp loc to HL
	dad	d		;temp loc - source loc
	jnc	tpmo2		;temp loc < source
	xchg			;offset to DE
	dcx	b		;length - 1 to BC
	call	cmbdu
	jc	tpmo2		;length <= offset
	lhld	gcmin
	dad	d		;destination + offset = new loc
	xchg			;to DE
	pop	h
	mov	m,d
	dcx	h
	mov	m,e		;and to temp
	push	h
tpmo2:	pop	h
	pop	psw		;# temps still unchecked
	pop	d		;temp pointer
	pop	h		;value pointer
	jmp	tpmo1		;and check the next temp

;NXTST is used during garbage collection to find locations of nonnull strings.
;Call:	DE	next symbol table entry addr (initially SYMTA)
;	HL	next length byte in current entry (initially SYMTA)
;Retn:	Carry	Set iff no more nonnull strings
;	A	length of next nonnull string
;	BC	location of string
;	DE	next entry addr
;	HL	next length byte
nxtst:	call	cmdhu
	jnz	nxts3		;more in current entry
nxts1:	call	stnxt		;address next symbol table entry
	rc			;no more
	ldax	d		;fetch type byte
	ani	1FH		;mask to type
	cpi	strst
	jnz	nxts1		;not a string
	xchg			;next entry addr to DE
nxts2:	inx	h		;point to next name byte
	ora	m
	jp	nxts2		;scan past name
	inx	h		;point to # dims
	mov	c,m		;# dims to C
	mvi	b,0
	inx	h
	dad	b
	dad	b		;point to first length byte
nxts3:	mov	a,m		;fetch length
	call	mobcm		;fetch location to BC
	inx	h		;point to next
	ora	a
	rnz			;return unless null
	jmp	nxtst		;else try next

;EVUNQ gets a string value for READ or INPUT.  If the next item scanned
;EVALuates to a string value, its value is passed.  Otherwise the item is
;considered to be an unquoted string starting at the first nonblank and
;delimited by the next comma or cr, and a pointer to it is returned.
;Retn:	Carry	Set iff next nonblank char is delimiter
;	C, DE	length, location of string
evunq:	call	gtcha		;fetch first nonblank
	cpi	'"'
	jz	gtlis		;quoted string literal
	call	bakup		;let HL and TEXTP point to first
	call	dtst1
	cmc
	rc			;first nonblank is delimiter, return Carry
	mov	d,h
	mov	e,l		;first nonblank loc to DE
evun1:	cpi	','
	jz	gtls1		;done if next is comma
	call	dtst1
	jnc	gtls1		;or if next is cr or '
	call	read1		;else read the current
	mov	a,m		;and fetch next
	jmp	evun1		;and continue scanning

;CMSTR compares two strings.
;Call:	B	length of string 2
;	C	length of string 1
;	DE	location of string 1
;	HL	location of string 2
;Retn:	Carry	Set iff string 1 < string 2
;	Zero	Set iff string 1 = string 2
cmstr:	mov	a,c
	ora	a
	jz	cmst1		;end of string 1
	mov	a,b
	ora	a
	jz	cmst2		;end of string 2
	ldax	d		;else fetch string 1 char
	cmp	m		;compare to string 2
	rnz			;unequal
	dcr	b
	dcr	c		;decrement lengths
	inx	d
	inx	h		;increment pointers
	jmp	cmstr		;and continue checking
cmst1:	cmp	b
	ret
cmst2:	ori	1		;clear Carry and Zero
	ret

;SIARG is called by LEFT$, RIGHT$ and MID$ to manipulate arguments.
;Call:	BC,DE	arg1 (string), arg2 (integer) pointers
;Retn:	A	arg2, 0 if < 0 and 255 if > 255 (with nonfatal FC error)
;	C, DE	string length and location
;SIAR0 is called by INST3 and MID3 to fetch integer arg 0 <= arg <= 255.
siar0:	call	cnvbi		;force to integer and fetch
	mov	h,b
	mov	l,c
	jmp	siar1		;force 0 <= arg <= 255 to A
siarg:	push	d		;save integer arg2
	call	fetbc		;fetch string arg1 to C, DE
	jnc	tmerr		;fatal TM error if nonstring
	pop	h		;arg2 to HL
siar1:	mov	a,h		;MID3 entry point
	ora	a
	mov	a,l		;lsbyte of arg2 to A
	rz			;done if 0 <= arg2 <= 255
	call	fcern		;else issue nonfatal FC error
	mov	a,h
	ora	a
	mvi	a,0
	rm			;and return 0 if < 0
	dcr	a		;else return 255
	ret


;string functions follow

;Concatenation (+): <string> x <string> --> <string>
sadd:	mov	a,b
	ora	a
	jz	sadd3		;string 2 null, return s1
	mov	a,c
	ora	a
	jz	sadd2		;string 1 null, return s2
	add	b		;else find length of concatenation
	jnc	sadd1		;not too long
	error	n, L, S		;issue nonfatal LS error
	mov	a,c
	cma
	mov	b,a		;useable length of s2 = 255 - length of s1
	add	c		;length of concatenation to A
sadd1:	push	psw		;save length
	lxi	h,stemp
	inr	m
	inr	m		;reassign string temporaries
	call	stfre		;assure sufficient space available
	lhld	estkp
	inx	h		;address s1 ESTACK entry
	push	h
	call	scop0		;fetch and copy to string space
	pop	h
	push	d		;save location of copied s1
	lxi	d,vbyts+2
	dad	d		;address s2 ESTACK entry
	call	scop0		;fetch and copy to string space
	pop	d		;result location to DE
	pop	psw
	mov	c,a		;result length to C
	mvi	a,strst		;result type to A
	ret
sadd2:	mov	c,b		;length of s2 to C
	xchg			;and loc to DE
sadd3:	mvi	a,strst
	ret

;INSTR: [<integer> x] <string> x <string> --> <integer>
instr:	lxi	h,1		;default arg0 value to HL
inst0:	push	h		;save first to test -- INST3 entry point
	dcx	h
	push	h		;save first-1
	call	ambop		;fetch matching args
	jnc	tmerr		;nonstrings
	xthl			;save s1 location, first-1 to HL
	xchg
	dad	d		;s2 loc + first - 1 = s2 remaining loc to HL
	mov	a,c
	mov	c,b
	sub	e		;s2 length + first - 1 = s2 remaining length
	pop	d
	jc	inst2		;s2 too short, return 0
	jz	inst2		;s2 rem null, return 0
	mov	b,a		;s2 rem len to B
;At INST1 B=s2 rem len, C=s1 len, DE=s1 loc, HL=s2 rem loc, stack=result
inst1:	mov	a,b
	cmp	c
	jc	inst2		;remaining part of s2 too short, return 0
	push	b
	push	d
	push	h
	mov	b,c		;set lengths equal for string compare
	call	cmstr		;compare s1 to LEFT$(rem s2,LEN(s1))
	pop	h
	pop	d
	pop	b
	jz	pop1		;matched, pop result to BC and return
	dcr	b		;else decr s2 rem len
	inx	h		;and incr s2 rem loc
	xthl
	inx	h		;and incr proto result
	xthl
	jmp	inst1		;and try again
inst2:	pop	b
	lxi	b,0
	ret
;INST3 executes ternary INSTR.
inst3:	push	d		;save arg2
	push	h		;and arg3
	call	siar0		;force 0 <= arg3 <= 255 to A
	mov	l,a
	mvi	h,0		;arg3 to HL
	ora	a
	cz	inxh		;fudge value 0 to 1
	pop	d
	pop	b		;restore args
	call	inst0		;perform INSTR
	mvi	a,intst		;and return type integer
	ret

;BIN$:	<integer> --> <string>
binfn:	lxi	h,(16 shl 8) or 1	;16 digits, 1 bit each
	jmp	hexf0

;HEX$:	<integer> --> <string>
hexfn:	lxi	h,(4 shl 8) or 4	;4 digits, 4 bits each
;HEXF0 is called by BIN$ and OCT$ to convert to string.
;	BC	integer value to convert
;	H	max digit count
;	L	nummber of bits per digit
hexf0:	lxi	d,bufad+1	;destination to DE
	push	b		;save arg
	mov	b,h		;remaining digit count to B
	mvi	h,0		;to suppress leading 0s
hexf1:	mov	c,l		;shift count to C -- OCT$ entry point
	xthl			;save status & count, get value
	xra	a		;build result digit in A
hexf2:	dad	h		;shift arg left
	ral			;Carry to A0
	dcr	c
	jnz	hexf2		;shift more bits
	adi	90H		;90H, ..., 99H, 9AH, ..., 9FH
	daa			;90H, ..., 99H, 00H+C,...,05H+C
	aci	40H		;D0H, ..., D9H, 41H, ..., 46H
	daa			;30H, ..., 39H, 41H, ..., 46H
	stax	d		;store ASCII digit
	inx	d
	dcr	b
	jz	hexf4		;done
	xthl			;save value, get status
	sui	'0'		;Zero set iff digit is 0
	ora	h		;Zero set iff leading 0
	mov	h,a		;save leading 0 status
	jnz	hexf1		;not a leading 0, do not suppress
	dcx	d		;suppress it
	jmp	hexf1
hexf4:	pop	b		;discard saved status
	lxi	h,bufad
	mvi	m,' '		;store first char = <space>
	xchg			;first loc to DE, last+1 to HL
	mov	a,l
	sub	e		;compute length
	mov	c,a		;length to C
	jmp	scopy		;copy to string space and return

;OCT$:	<integer> --> <string>
octfn:	mov	h,b
	mov	l,c		;arg to HL
	dad	h		;shift left one bit, Carry iff leading 1
	mov	b,h
	mov	c,l		;shifted arg to BC
	lxi	h,(5 shl 8) or 3	;5 digits, 3 bits each
	jnc	hexf0		;high bit 0, so just do as in HEX$
	lxi	d,bufad+1
	mvi	a,'1'
	stax	d		;store leading ASCII 1
	inx	d
	push	b		;save arg
	mov	b,h		;repeat count to B
	jmp	hexf1		;leave H nonzero to retain 0s

;STR$: {<integer> | <floating>} --> <string>
strs:	if	float		;<floating> --> <string>
	call	fout		;convert floating value to string
	else			;<integer> --> <string>
	mov	a,b
	ora	a
	mvi	a,0
	jp	strs1		;convert to string with no leading char if +
	call	iumin		;else negate the arg
	mvi	a,'-'
strs1:	call	cvtis		;convert integer to string
	endif
	jmp	scopy		;copy to string space and return

;VAL: <string> --> {<integer> | <floating>}
val:	call	fetbc		;fetch the arg
	inr	c		;bump length
	jnz	val1
	dcr	c		;length was 255, unincrement
val1:	call	scopy		;make new copy with extra char
	mvi	m,cr		;last char is cr
	xchg			;location to HL
	shld	txtp2		;and to TXTP2 for FLIP
	call	flip		;let TEXTP scan the string
	call	gtlit		;look for a literal
	push	psw
	push	b
	push	d		;and save result of GTLIT
	call	gtcha		;get first char after literal
	call	flip		;restore TEXTP
	mov	h,a		;first char after lit to H
	pop	d
	pop	b
	pop	psw		;recover result of GTLIT
	mov	l,a
	mov	a,h
	cpi	cr		;check if next after lit was cr
	mov	a,l
	jnz	fcer0		;FC error if not
	cpi	intst		;check if type integer
	rz			;yes, return
	if	float
	cpi	sngst
	jnz	fcer0		;not floating nor integer, FC error
	lxi	h,temp
	mov	e,m		;restore saved value for A to E if floating
	ret
	endif

;ASC: <string> --> <integer>
asc:	call	len		;fetch string arg, 0 to B
	mov	a,c		;length to A
	ora	a		;check length
	jz	fcer0		;null, nonfatal FC error and return 0
	ldax	d		;else fetch character
	mov	c,a
	ret

;LEN: <string> --> <integer>
len:	call	fetbc		;fetch string arg
	mvi	b,0		;return length in BC
	ret

;CHR$: <integer> --> <string>
chrs:	call	isbyt		;arg must be byte expr
	mov	a,c		;value to A
chrs1:	lxi	d,temp
	stax	d		;save in TEMP
	mvi	c,1		;length is 1
	jmp	scopy		;and copy to string space

;LEFT$: <string> x <integer> --> <string>
lefts:	call	siarg		;args to A, CDE
left1:	cmp	c
	rnc			;arg >= length, return unchanged
	mov	c,a		;else arg becomes new length
	ret

;RIGHT$: <string> x <integer> --> <string>
right:	call	siarg
righ1:	cmp	c
	rnc			;arg >= length, return unchanged
	dcr	c
	inx	d		;else chop off head char
	jmp	righ1		;and try again

;MID$: <string> x <integer> [x <integer>] --> <string>
mids:	mvi	a,255		;default arg3 value to A
mid0:	push	psw		;MID3 entry point
	call	siarg
	pop	h		;arg3 to H
	ora	a
	jz	mid2		;arg2 is 0, just do LEFT$
	inr	c
	dcx	d		;add bogus head char
mid1:	inx	d
	dcr	c		;lop off head
	rz			;return if null
	dcr	a		;and decrease arg2
	jnz	mid1
mid2:	mov	a,h
	jmp	left1		;and do a LEFT$ with arg3
mid3:	push	b		;save arg1
	mov	b,d
	mov	c,e		;arg2 to BC
	call	cnvbi		;convert to integer and fetch
	push	b		;and save arg2
	mov	b,h
	mov	c,l
	call	siar0		;force 0 <= arg3 <= 255 to A
	pop	d		;arg2 value to DE
	pop	b		;and arg1 addr to BC
	call	mid0		;do the MID$
	mvi	a,strst		;and return type string
	ret

	if	realt
;TIME$: --> <string>
;TIME$ returns the current time as string "hh:mm:ss".
timed:	lxi	d,timex+3	;DE addresses hours count
	lxi	h,bufad		;HL addresses string being built
	push	h		;save for result
	di			;disable so clock does not tick during fetch
	ldax	d		;fetch hours-24
	adi	24		;hours
	call	time1		;convert hours, fetch minutes
	call	time1		;convert minutes, fetch seconds
	ei			;all fetched, renable
	call	time1		;convert seconds
	pop	d		;recover string address
time0:	mvi	c,8		;length = 8 for hh:mm:ss
	jmp	scopy		;copy to string space and return
;TIME1 adds two ASCII decimal digits and a colon to result string.
;Call:	A	desired value (hours, minutes or seconds)
;	DE	count location
;	HL	string destination
;Retn:	A	next count (from (DE)-1)
;	DE	decremented
;	HL	next location = HL + 3
time1:	mvi	m,'0'		;store tens digit
time2:	cpi	10
	jc	time3		;tens digit is now correct
	inr	m		;else bump tens digit
	sui	10		;and subtract from count
	jmp	time2		;and try again
time3:	adi	'0'		;make remainder ASCII
	inx	h
	mov	m,a		;and add to string
	inx	h
	mvi	m,':'		;and add colon
	inx	h		;point to next available
	dcx	d
	ldax	d		;fetch next count
	adi	60		;add negative bias
	ret
	endif			;end of REALT conditional


	endif			;end of STRNG conditional


;end of STRINGS
	page
;TABLES 01/12/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;tables


;symbol table type token EQUates
intst	equ	1		;integer variable
sngst	equ	2		;single precision floating point variable
strst	equ	3		;string variable
ufnst	equ	4		;user-defined function
brkst	equ	5		;break entry
camst	equ	6		;camac variable

;control stack token EQUates
csgos	equ	25H		;gosub
csfor	equ	47H+2*fbyts	;for
csbrk	equ	65H		;break (and stop)
csint	equ	87H		;interrupt
cslbk	equ	0A5H		;line break

;expr stack type byte EQUates
tmask	equ	0F0H		;type mask
pmask	equ	00FH		;precedence mask
;first characters
isdel	equ	10H		;delimiter
islp	equ	20H		;left parenthesis
isrp	equ	30H		;right parenthesis
iscom	equ	40H		;comma
;then 0-ary and user-defined functions
isfn0	equ	50H		;0-ary function
isufn	equ	60H		;user-defined function
;then ops and fns, op1 first
isop1	equ	70H		;unary op
isop2	equ	80H		;binary op
isfn1	equ	90H		;unary function
isfn2	equ	0A0H		;binary function

;branch addresses for command execution, in order of table position
cmdta:	dw	let
	dw	ifcom
	dw	for
	dw	next
	dw	goto
	dw	gosub
	dw	print
	dw	input
	dw	read
	dw	retrn
	dw	def
	dw	dim
	dw	gtdel
	dw	restr
	dw	stop
	dw	endcm
	if	not wild
	dw	null
	endif
	dw	calcm
	dw	scall
	if	not wild
	dw	put
	dw	poke
	dw	wait
	endif
	dw	tcoff
	dw	tcon
	dw	tpoff
	dw	tpon
	dw	break
	dw	unbrk
	if	not wild
	dw	enabl
	dw	dsabl
	dw	rndiz
	endif
	dw	rem
	dw	delay
	dw	save
	dw	load
	dw	list
	dw	clear
	dw	newcm
	dw	run
	dw	cont
	if	not camac
	dw	auto
	dw	delet
	dw	edit
	dw	renum
	endif
	if	not wild
	dw	move
	endif
	dw	exec
	if	epstn
	dw	plot
	endif
	if	sdisk and not c3885
	dw	open
	dw	close
	dw	linpt
	dw	margn
	if	bendx
	dw	bmsos
	dw	bname
	dw	bzap
	else
	dw	dircm
	dw	scrat
	endif
	endif
	if	bendx
	dw	bfils
	endif
	if	camac
	dw	camcd
	dw	declr
	dw	drivr
	dw	xenab
	dw	rstat
	dw	camcl
	dw	cinit
	dw	cinhi
	dw	loopc
	dw	bypas
	dw	onlin
	dw	pulse
	dw	open
	dw	close
	if	not c3885
	dw	creat
	endif
	dw	lampa
	dw	bkset
	dw	block
	dw	cenab
	dw	cdsab
	dw	sttim
	else
	if	wild
	dw	wgbuf
	else
	dw	assig
	endif
	if	realt
	dw	sttim
	else
	dw	time
	endif
	endif
	if	packi
	dw	onlin
	dw	oflin
	dw	field
	endif
	dw	on			;must follow ONLINE

;type / precedence bytes for functions and ops
;ms four bits type, ls four bits precedence
typta:	db	isufn					;fn
	db	isop1 + 6				;not
	db	isop2 + 8, isop2 + 8			;+, -
	db	isop2 + 9, isop2 + 9			;*, /
	if	float
	db	isop2 + 9, isop2 + 10			;\, ^
	endif
	db	isop2 + 7, isop2 + 7, isop2 + 7, isop2 + 7, isop2 + 7
	db	isop2 + 7, isop2 + 7, isop2 + 7		;relations
	db	isop2 + 5, isop2 + 4, isop2 + 4		;and, xor, or
	db	isop2 + 9 				;mod
	if	not wild
	db	isop2 + 11				;join
	endif
	db	isfn0, isfn0				;get, fre
	if	not wild
	if	float
	db	isfn1+2					;rnd
	else
	db	isfn0
	endif
	endif
	if	wild
	db	isfn0					;buffer
	else
	if	not camac
	db	isfn0					;pos
	endif
	db	isfn0, isfn0				;first, last
	endif
	if	packi
	db	isfn0					;flag
	endif
	if	sdisk and not c3885
	db	isfn1+2					;eof
	endif
	if	float
	if	not wild
	db	isfn1 + 2				;UNS
	endif
	db	isfn1 + 2, isfn1 + 2, isfn1 + 2, isfn1 + 2
	db	isfn1 + 2, isfn1 + 2, isfn1 + 2		;unary functions
	if	not camac
	db	isfn1 + 2				;ATN
	endif
	endif
	if	strng
	db	isfn1 + 2, isfn1 + 2, isfn1 + 2, isfn1 + 2
	db	isfn1 + 2, isfn1 + 2, isfn1 + 2, isfn1 + 2
	db	isfn2 + 2, isfn2 + 2, isfn2 + 2, isfn2 + 2
	endif
	if	camac
	db	isfn0, isfn0
	if	c3885
	db	isfn1 + 2
	else
	db	isfn0
	endif
	else
	if	not wild
	db	isfn0					;iobyte
	endif
	if	realt
	db	isfn0					;time$
	db	isfn1 + 2				;time
	endif
	endif
	db	isfn1 + 2, isfn1 + 2
	if	not wild
	db	isfn1 + 2, isfn1 + 2
	db	isfn1 + 2, isfn1 + 2, isfn1 + 2		;unary functions
	db	isfn2 + 2, isfn2 + 2, isfn2 + 2, isfn2 + 2, isfn2 + 2
	db	isfn2 + 2, isfn2 + 2			;binary functions
	db	isfn1 + 2				;in
	endif

;argument and result type information table for fns and ops
;FN is a macro building a five-byte table entry for a function.
;The first byte gives the type of arg2.
;The second byte gives the type of arg1.
;The third and fourth bytes give the function address.
;The fifth byte gives the type of the result.
;Unused arguments are indicated by 0, ambiguous entries by AMBST.
;E.g.:	fn	sgn, ambst, 0, intst
;indicates that the routine to compute SGN is at location SGN, the first arg
;ambiguous (integer or float), no second arg, and the result integer.
ambst	equ	0
fn	macro	addr, a1typ, a2typ, rtype
	db	a2typ, a1typ
	dw	addr
	db	rtype
	endm
;NB unary minus first, in place of fn
funta:	fn	aumin, ambst, 0, ambst
	fn	opnot, intst, 0, intst
	fn	aadd, ambst, ambst, ambst
	fn	asub, ambst, ambst, ambst
	fn	amul, ambst, ambst, ambst
	if	float
	fn	fdivd, sngst, sngst, sngst	;floating /
	fn	idivd, intst, intst, intst	;integer \
	fn	fatox, sngst, sngst, sngst
	else
	fn	idivd, intst, intst, intst	;/ is integer divide if nonfloating
	endif
	fn	leq, ambst, ambst, intst
	fn	leq, ambst, ambst, intst
	fn	geq, ambst, ambst, intst
	fn	geq, ambst, ambst, intst
	fn	neq, ambst, ambst, intst
	fn	equal, ambst, ambst, intst
	fn	lthan, ambst, ambst, intst
	fn	gthan, ambst, ambst, intst
	fn	opand, intst, intst, intst
	fn	opxor, intst, intst, intst
	fn	opor, intst, intst, intst
	fn	opmod, intst, intst, intst
	if	not wild
	fn	join, intst, intst, intst
	endif
	fn	get, 0, 0, ambst
	fn	frefn, 0, 0, intst
	if	not wild
	if	float
	fn	rnd, sngst, 0, sngst
	else
	fn	rnd, 0, 0, intst
	endif
	endif
	if	wild
	fn	wbuff, 0, 0, strst
	else
	if	not camac
	fn	pos, 0, 0, intst
	endif
	fn	first, 0, 0, intst
	fn	last, 0, 0, intst
	endif
	if	packi
	fn	flag, 0, 0, intst
	endif
	if	sdisk and not c3885
	fn	eoffn, intst, 0, intst
	endif
	if	float
	if	not wild
	fn	unsfn, intst, 0, sngst
	endif
	if	f9511
	fn	int, sngst, 0, sngst
	else
	fn	int, sngst, 0, ambst
	endif
	fn	fsqr, sngst, 0, sngst
	fn	fexp, sngst, 0, sngst
	fn	fln, sngst, 0, sngst
	fn	fsin, sngst, 0, sngst
	fn	fcos, sngst, 0, sngst
	fn	ftan, sngst, 0, sngst
	if	not camac
	fn	fatan, sngst, 0, sngst
	endif
	endif
	if	strng
	fn	binfn, intst, 0, strst
	fn	hexfn, intst, 0, strst
	fn	octfn, intst, 0, strst
	fn	chrs, intst, 0, strst
	if	float
	fn	strs, sngst, 0, strst
	else
	fn	strs, intst, 0, strst
	endif
	fn	asc, strst, 0, intst
	fn	len, strst, 0, intst
	fn	val, strst, 0, ambst
	fn	lefts, strst, intst, strst
	fn	right, strst, intst, strst
	fn	mids, strst, intst, strst
	fn	instr, ambst, strst, intst
	endif
	if	camac
	fn	camqf, 0, 0, intst
	fn	timed, 0, 0, strst
	if	c3885
	fn	eoffn, intst, 0, intst
	else
	fn	typfn, 0, 0, intst
	endif
	else
	if	not wild
	fn	iobyf, 0, 0, intst
	endif
	if	realt
	fn	timed, 0, 0, strst			;TIME$
	fn	time, intst, 0, intst			;TIME
	endif
	endif
	fn	sgn, ambst, 0, intst
	fn	aabs, ambst, 0, ambst
	if	not wild
	fn	msbyt, intst, 0, intst
	fn	lsbyt, intst, 0, intst
	fn	bcd, intst, 0, intst
	fn	bin, intst, 0, intst
	fn	peek, intst, 0, intst
	fn	rotat, intst, intst, intst
	fn	test, intst, intst, intst
	fn	sense, intst, intst, intst
	fn	rshft, intst, intst, intst
	fn	lshft, intst, intst, intst
	fn	reset, intst, intst, intst
	fn	setfn, intst, intst, intst
	fn	inp, intst, 0, intst
	endif

;keyword table EQUates
ncmds	equ	(typta-cmdta)/2		;number of commands
nfuns	equ	funta-typta		;number of functions
nrwds	set	3			;number of reserved words
	if	not wild
nrwds	set	nrwds+1
	if	not camac
nrwds	set	nrwds+5
	endif
	endif
	if	float
nrwds	set	nrwds+1
	else
nrwds	set	nrwds+1
	if	strng
nrwds	set	nrwds+1
	endif
	endif
	if	strng
nrwds	set	nrwds+1
	else
nrwds	set	nrwds+1
	endif
	if	not wild
nrwds	set	nrwds+1
	endif
	if	key80
nkeys	equ	ncmds+nfuns		;number of keywords
	else
nkeys	equ	ncmds+nfuns+nrwds
	endif

;keyword table
;Adding or deleting a keyword always requires changing the KEYword table.
;In addition, adding a command requires changing the branch table at CMDTA.
;Adding a fn/op requires changing the type/prec table at TYPTA and the arg/result
;table at FUNTA.  Adding a reserved word requires changing the NRWDS EQUates.

;KEY is a macro to define keyword table entries and corresponding token values.
key	macro	f, l, tval
	if	not nul tval
tval	equ	token			;;equate token value if desired
	endif
token	set	token+1			;;increment token value
	if	wild or not compl
	db	'&F', '&L' or 80H	;;keyword
	endif
	endm
;NOKEY is a macro to leave holes in table for commands not included in
;a non-ROMSQ or non-EDITC version, for token compatibility.
nokey	macro	n
	if	wild or not compl
	rept	n
	db	0FFH		;;token is <rubout> or 80H
	endm
	endif
token	set	token+n
	endm
token	set	-nkeys and 0FFH	;first token value

;commands
keyta:	key	LE, T, cmdtk
	key	I, F
	key	FO, R, fort
	key	NEX, T, nextt
	key	GOT, O, gotot
	key	GOSU, B, gsubt
	key	PRIN, T, prntt
	key	INPU, T
	key	REA, D
	key	RETUR, N
	key	DE, F
	key	DI, M
	key	DAT, A, datat
	key	RESTOR, E, rstrt
	key	STO, P
	key	EN, D
	if	not wild
	key	NUL, L
	endif
	key	CAL, L
	key	SCAL, L
	if	not wild
	key	OU, T
	key	POK, E
	key	WAI, T
	endif
	key	UNTRAC, E
	key	TRAC, E
	key	UNTRA, P
	key	TRA, P
	key	BREA, K, brkt
	key	UNBREA, K, unbkt
	if	not wild
	key	ENABL, E, enabt
	key	DISABL, E, dsabt
	key	RANDOMIZ, E
	endif
	key	RE, M, remt
	key	DELA, Y
	key	SAV, E
	key	LOA, D
	key	LIS, T, listt
	key	CLEA, R
	key	NE, W
	key	RU, N, runt
	key	CON, T
	if	not camac
	if	editc
	key	AUT, O
	key	DELET, E
	key	EDI, T
	key	RENU, M
	else
	nokey	4
	endif
	endif
	if	romsq
	if	not wild
	key	MOV, E
	endif
	key	EXE, C
	else
	nokey	2
	endif
	if	epstn
	key	PLO, T
	endif
	if	sdisk and not c3885
	key	OPE, N
	key	CLOS, E
	key	LINPU, T
	key	MARGI, N
	if	bendx
	key	MSO, S
	key	NAM, E
	key	ZA, P
	else
	key	DI, R
	key	SCRATC, H
	endif
	endif
	if	bendx
	key	FILE, S
	endif
	if	camac
	key	CAMA, C
	key	DECLAR, E
	key	DRIVE, R, drivt
	key	XENABL, E
	key	RSTATU, S
	key	CAMCL, R
	key	INI, T
	key	INHIBI, T
	key	LOOPCO, L
	key	BYPAS, S
	key	ONLIN, E
	key	PULS, E
	key	OPE, N
	key	CLOS, E
	if	not c3885
	key	CREAT, E
	endif
	key	LAMPA, T
	key	BKSE, T
	key	BLOC, K
	key	ENA, B
	key	DISA, B
	key	SETTIM, E
	else
	if	wild
	key	GETBU, F
	else
	key	ASSIG, N
	endif
	if	realt
	key	SETTIM, E
	else
	key	TIM, E
	endif
	endif
	if	packi
	key	ONLIN, E
	key	OFFLIN, E
	key	FIEL, D
	endif
	key	O, N

;functions and ops
	key	F, N, udfnt
	key	NO, T
	key	, +, plust
	key	, -, mint
	key	, *, multt
	key	, /
	if	float
	key	, \
	key	, ^^
	endif
	key	^<, =
	key	=, ^<
	key	=, ^>
	key	^>, =
	key	^<, ^>
	key	, =, eqult
	key	, ^<
	key	, ^>
	key	AN, D
	key	XO, R
	key	O, R
	key	MO, D
	if	not wild
	key	JOI, N
	endif
	key	GE, T
	key	FR, E
	if	not wild
	key	RN, D
	endif
	if	wild
	key	BUFFE, R, wbuft
	else
	if	not camac
	key	PO, S
	endif
	if	romsq
	key	FIRS, T
	key	LAS, T
	else
	nokey	2
	endif
	endif
	if	packi
	key	FLA, G
	endif
	if	sdisk and not c3885
	key	EO, F
	endif
	if	float
	if	not wild
	key	UN, S
	endif
	key	IN, T, intt
	key	SQ, R
	key	EX, P
	key	LO, G
	key	SI, N
	key	CO, S
	key	TA, N
	if	not camac
	key	AT, N
	endif
	endif
	if	strng
	key	BIN, $
	key	HEX, $
	key	OCT, $
	key	CHR, $
	key	STR, $
	key	AS, C
	key	LE, N
	key	VA, L
	key	LEFT, $
	key	RIGHT, $
	key	MID, $, midst
	key	INST, R, instt
	endif
	if	camac
	key	CA, M, camt
	key	TIME, $
	if	c3885
	key	EO, F
	else
	key	TY, P
	endif
	else
	if	not wild
	key	IOBYT, E
	endif
	if	realt
	key	TIME, $
	key	TIM, E
	endif
	endif
	key	SG, N
	key	AB, S
	if	not wild
	key	MSBYT, E
	key	LSBYT, E
	key	BC, D
	key	BI, N
	key	PEE, K
	key	ROTAT, E
	key	TES, T
	key	SENS, E
	key	RSHIF, T
	key	LSHIF, T
	key	RESE, T
	key	SE, T
	key	I, N
	endif

;reserved words and PRINT functions
	if	key80
rwdtk	equ	0EH		;first reserved word token
token	set	rwdtk
	endif
rwdta:	key	THE, N, thent
	key	T, O, tot
	key	STE, P, stept
	if	not wild
	key	TA, B, tabt
	if	not camac		;no devices in CAMAC version
	key	SP, C, spct
	key	CON, #, contk
	key	RDR, #
	key	PUN, #
	key	LST, #
	endif
	endif
	if	float
	key	SN, G, sngt		;for DEF SNG
	else
	key	UN, S, unst		;UNS is print fn if nonfloating
	if	strng
	key	IN, T, intt		;for DEF INT
	endif
	endif
	if	strng
	key	ST, R, strgt		;for DEF STR
	else
	key	CHR, $, chrst		;CHR$ is print fn if nonstring
	endif
	if	not wild
	if	romsq
	key	FRO, M, fromt		;for MOVE FROM
	else
	nokey	1
	endif
	endif

;end of keyword table

;table of keywords after which <line #>s may occur, for RENUM
	if	editc and not compl
klnta:	db	gotot, gsubt, thent, runt, rstrt
	db	listt, brkt, unbkt
	if	not wild
	db	enabt, dsabt
	endif
klnct	equ	$-klnta
	endif

;copyright message
;not in module INITIAL to prevent overlaying
mwcma:	db	'COPYRIGHT 1978, 1979, 1980, 1981'
	if	epstn
	db	13H
	endif
	db	' BY MARK WILLIAMS COMPANY, CHICAGO'
	if	epstn
	db	12H, cr, lf, lf, 'PREPARED FOR ', 16H
	db	'EPSTEIN & BERGHORN', 12H, ', NORTHBROOK IL', cr, lf
	endif
	db	cr, lf or 80H

;end of TABLES
	page
;TOKENIZE 10/22/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;tokenization and line editing

	if	(not wild) or (not rtpak)	;WILD RTPAK uses GTLIN in WILD
;GTLIN gets a line of source text from the user and sets TEXTP.
;Uses:	B	temp storage
;	C	unused
;	D	LS byte of first text address
;	E	LS byte of max text address
;	HL	text address
;Chars are read with READC and inserted starting at NLNAD, except:
;	<rubout>	deletes last character
;	<control-R>	retypes current line
;	<control-U>	starts over
;	<control-H>	deletes last char, echoes <cntl-h> to backspace cursor
;	<cr>		echoes <lf> and returns
;	<control-G>	accepted
;	other <control>s	ignored (ASCII 0-1FH)
;Chars typed when buffer is full echo <control-G> and are ignored.
	if	not wild
gtli0:	if	epstn
	call	writc		;echo the ^K
	else
	call	echoc		;echo ^U
	call	wcrlf		;write crlf after ^U
	endif
	if	editc
	mvi	a,cr
	sta	nlnad		;clobber old buffer contents in case ^E
	endif
	endif			;end of NOT WILD conditional
gtlin:	lxi	d,nlnad+nlmax	;lsbyte of max address to E
	lxi	h,nlnad		;first text byte address to HL
	mov	d,l		;lsbyte of text address to D
	if	editc
	call	readc		;check first char in case ^E
	cpi	cntle
	if	wild
	jnz	gtli2
	else
	jnz	gtl1d		;first char not ^E
	endif
	jmp	ledit		;enter line editor
	endif
gtli1:	call	readc		;get a character
	if	not wild
gtl1d:	cpi	rbout
	jnz	gtli2
	if	epstn
	call	writc		;echo the <rub> in Epstein version
	endif
	mov	a,l		;rubout
	cmp	d
	jz	gtli1		;start over if line is empty
	if	epstn
	dcx	h
	jmp	gtli1		;back up pointer and get next
	else
	mvi	a,'/'
	call	writc		;echo slash first
gtl1a:	dcx	h
	mov	a,m		;get previous char
	call	writc		;and echo it
gtl1b:	call	readc		;read another from console
	cpi	rbout		;see if still rubbing out
	jnz	gtl1c		;no, echo end slash and continue
	mov	a,l
	cmp	d
	jz	gtl1b		;at start of line
	jmp	gtl1a
gtl1c:	mov	b,a		;save new char
	mvi	a,'\'
	call	writc		;write end slash
	mov	a,b		;restore new char
	endif			;end of NOT EPSTN conditional
	endif			;end of NOT WILD conditional
gtli2:	mov	m,a		;insert the char
	cpi	cr
	jz	gtli5		;echo lf and return
	if	not wild
	cpi	cntlu
	jz	gtli0		;crlf and start over if ^U
	endif
	if	not epstn
	if	(not wild) or (not rtpak)
	cpi	cntlh
	jz	gtli7		;backspace if control-h
	endif
	if	editc
	cpi	cntle
	jz	gtli8		;edit already typed line if ^E
	endif
	if	not wild
	cpi	cntlr
	cz	echoc		;echo ^R
	jz	gtli6		;retype line if ^R
	endif
	cpi	cntlg
	cz	echoc		;echo ^G
	jz	gtli3		;accept bell
	cpi	20H
	jc	gtli1		;ignore 0H - 1FH (controls)
	endif
gtli3:	mov	a,l
	cmp	e		;check for line overflow
	mov	a,m		;restore char to A
	cz	gtli4		;replace with bell if line too long
	if	epstn
	cpi	escap
	cz	gtli6		;echo escape as $ in Epstein version
	endif
	call	writc		;echo char
	inx	h		;bump insertion pointer
	jmp	gtli1		;and get next
gtli4:	mvi	a,cntlg		;replace char with bell
	dcx	h		;leave pointer unchanged
	ret
gtli5:	if	not compl
	xra	a
	sta	cstkd		;clear control stack direct count
	endif
	lxi	h,nlnad
	shld	textp		;reset text pointer
	jmp	wcrlf		;echo crlf and return
	if	epstn
gtli6:	mvi	a,'$'
	ret
	else
	if	not wild
gtli6:	call	wcrlf
	push	d		;save DE
	lxi	d,nlnad		;first addr to DE
	mov	a,l
	sub	e		;last+1 - first = # chars to A
	call	prst0		;and print the line
	pop	d		;restore DE
	jmp	gtli1		;and wait for next char
	endif
	if	(not wild) or (not rtpak)
gtli7:	mov	a,l
	cmp	d
	jz	gtli1		;ignore if at start of line
	mvi	a,cntlh		;restore the ^H
	call	writc		;and echo the ^H to backspace
	dcx	h		;decrement position
	jmp	gtli1
	endif
	if	editc
gtli8:	call	wcrlf		;write crlf to get to new line
	mvi	m,cr		;store cr
	jmp	ledit		;line edit the line
	endif
	endif			;end of NOT EPSTN conditional

;ECHOC echoes meaningful control chars as ^char.
;Call:	A	control char value in ASCII
;Retn:	PSW,A,BC,DE,HL	preserved
echoc:	push	psw
	mvi	a,'^'
	call	writc		;write ^
	pop	psw		;restore control char value
	push	psw		;and resave
	adi	40H		;add ASCII bias
	call	writc		;write the char
	pop	psw		;restore status bits and A
	ret
	endif			;end of NOT WILD and NOT RTPAK conditional

	if	editc
;LEDIT does the work of line editing.
;Uses:	B	Chars left of cursor, initially 0
;	C	Max # of chars to add without overflowing buffer
;	HL	Cursor position, initially NLNAD
;	NLNAD	Line currently being edited
;	TLNAD	Original contents of line (in case ^U typed)
;Retn:	NLNAD	contains edited line
;	TEXTP	reset to NLNAD
;	CSTKD	reset to 0
;	GCHAR	reset to 0
;	TLNAD	clobbered
;	Registers	clobbered
ledit:	lxi	h,nlnad		;input buffer address to HL
	shld	textp		;reset TEXTP
	lxi	d,tlnad		;tokenization buffer address to DE
ledt0:	lxi	b,nlmax+3	;0 to B, NLMAX+3 to C
ledt1:	mov	a,m		;fetch char from input buffer
	stax	d		;and save in tokenization buffer
	dcr	c		;decrement remaining char count
	inx	d
	inx	h
	sui	cr		;check if at cr
	jnz	ledt1		;no, keep copying
	sta	cstkd		;clear CSTACK direct count
	sta	gchar		;clear GCHAR
	lxi	h,nlnad		;input buffer address to HL
	call	ledi6		;type line
ledt2:	call	readc		;get command character
ledt3:	cpi	cr
	jz	led11		;done if <cr>
	cpi	cntlu
	jz	led10		;^U
	push	h
	lxi	h,ledt2
	xthl			;push LEDT2 to allow RETurn from routines
	cpi	rbout
	jz	ledi1		;<rubout>
	cpi	20H
	jnc	ledi0		;printable char
	cpi	cntld
	jz	ledi2		;^D
	cpi	cntlf
	jz	ledi3		;^F
	cpi	cntlg
	jz	ledi0		;^G, treat as printable
	cpi	cntlh
	jz	ledi4		;^H
	cpi	cntlk
	jz	ledi5		;^K
	cpi	cntll
	jz	ledi6		;^L
	cpi	cntln
	jz	ledi7		;^N
	cpi	cntlr
	jz	ledi8		;^R
	cpi	cntlt
	jz	ledi9		;^T
	ret			;ignore any other chars

;printable char or ^G is inserted.
ledi0:	dcr	c		;decrement char count
	jz	led0b		;too many chars
	mov	d,m		;char right of cursor to D
	mov	m,a		;insert new char
	call	led9a		;incr chars left of cursor and echo char
	push	h		;and save
led0a:	mov	a,d		;fetch saved char
	mov	d,m		;save next char
	mov	m,a		;store current char
	inx	h
	cpi	cr
	jnz	led0a
	pop	h		;restore cursor loc
	ret
led0b:	inr	c		;undecrement count
led0c:	mvi	a,cntlg
	jmp	writc		;and write a bell

;<rubout> erases the char left of cursor, echoing it within slashes.
ledi1:	mov	a,b
	ora	a
	rz			;ignore if already at left margin
	mvi	a,'/'
	call	writc		;write initial /
led1a:	dcx	h
	mov	a,m
	call	led4a		;write deleted char and move remainder of line
	call	readc		;read next char
	cpi	rbout
	jnz	led1b		;no additional <rubout>s
	mov	a,b
	ora	a
	jnz	led1a		;<rubout> another
led1b:	mov	d,a		;save next char
	mvi	a,'\'
	call	writc		;write the end \
	mov	a,d		;restore next char
	pop	d		;pop the return to LEDIT
	jmp	ledt3		;and return to LEDT1 instead

;^D deletes the char right of cursor.
ledi2:	mov	a,m		;fetch char right of cursor
	cpi	cr
	rz			;leave unchanged if at end of line
	inr	c		;one more char is now available
	push	h		;save cursor location
	mov	d,h
	mov	e,l
led2a:	inx	h
	mov	a,m		;fetch next char
	stax	d		;and store
	inx	d
	cpi	cr
	jnz	led2a		;continue until cr
	pop	h		;restore cursor
	ret

;^F <char> searches for next occurence of <char>.
ledi3:	call	readc		;get search character
	sta	gchar		;and save
led3a:	mov	d,a		;search char to D
	push	h		;save cursor position
led3b:	mov	a,m		;fetch next char
	inx	h
	cpi	cr		;check if at cr
	jz	led3d		;not found
	cmp	d		;check for match
	jnz	led3b
	xchg			;match location to DE
	pop	h		;restore current loc
led3c:	call	cmdhu		;compare current to desired
	rz
	mov	a,m
	call	led9a		;move right and print
	jmp	led3c
led3d:	pop	h		;recover cursor location
	jmp	led0c		;beep and continue

;^H deletes char left of cursor and echoes ^H.
ledi4:	mov	a,b
	ora	a
	rz			;no chars left of cursor
	dcx	h
	mvi	a,cntlh
led4a:	call	writc		;echo the ^H
	dcr	b		;decrement chars left of cursor
	jmp	ledi2		;move remainder of chars

;^K kills the chars right of the cursor.
ledi5:	mov	a,m		;fetch next
	cpi	cr
	rz			;done if at cr
	call	ledi2		;else delete a char
	jmp	ledi5		;and repeat

;^L prints remainder of line and moves cursor to left.
ledi6:	mov	a,m
	inx	h
	call	writc		;write next char
	cpi	cr
	jnz	ledi6		;not at <cr> yet
	lxi	h,nlnad		;cursor at left of line
	mvi	b,0		;0 chars left of cursor
	jmp	wlf		;write <lf> after <cr>

;^N gets next occurence of ^F <char>.
ledi7:	lda	gchar		;get previous ^F character
	jmp	led3a		;and continue as for ^F

;^R retypes the line, leaving cursor unchanged.
ledi8:	mov	d,h
	mov	e,l		;cursor position to DE
	call	ledi6		;type remainder of line
	jmp	led3c

;^T moves the cursor one char right.
ledi9:	mov	a,m		;fetch next
	cpi	cr
	rz			;done if no chars right of cursor
led9a:	inx	h
	inr	b		;increment chars to left count
	jmp	writc		;echo char and return

;^U restores the original contents of the buffer and retries.
led10:	call	wcrlf		;write crlf
	lxi	h,tlnad		;tokenization buffer address to HL
	lxi	d,nlnad		;input buffer address to DE
	jmp	ledt0		;and continue as above

;<cr> resets GCHAR, prints the edited line and exits from line editor.
led11:	xra	a
	sta	gchar		;reset GET character
	jmp	ledi6		;print line and return

	endif			;end of EDITC conditional

	if	wild or not compl
;TKIZE tokenizes a line of text and computes its length.
;Call:	(newln)	address of first text byte
;Uses:	BC	address of next tokenized text line byte
;	HL	address of next untokenized text line byte
;Retn:	A,BC,DE	clobbered
;	HL	address of first tokenized text line byte (i.e. (newln))
;	(textp)	ditto
;	(lnnum)	line #, 0 if none
;	(lnlen)	length of tokenized line + overhead, 0 to delete
;			(i.e. if line consists of [<line #>] <cr>)
;	Carry	set iff no line # and line not <cr>
;	Zero	set if <cr>
;The tokenized line is identical to the original line, EXCEPT:
;(1)  The line # (if any) and spaces preceding it are removed, and
;(2)  Instances of keywords not inside " " or following # are replaced
;	by the correponding tokens.
;0 is stored in the byte after <cr> as a pseudo-eof for direct mode execution.
tkize:	if	not (wild and rtpak)
	call	lnnu0
	endif
	lxi	h,nlnad		;store text address in HL
	lxi	b,tlnad		;tokenized line address to BC
	push	b		;save for exit
	call	gtdec		;look for decimal line #
	jc	tkiz0		;none
	mov	a,d
	ora	e
	jz	snerr		;SN error if zero or too big
tkiz0:	push	psw		;carry set iff no line #
	xchg
	shld	lnnum		;line # to lnnum
	xchg
	mov	a,m		;next text char to A
	sui	cr		;compare with sui (to use zero)
	jnz	tkiz1
	inx	h		;delete line by letting length = 0
	mov	m,a		;store pseudo-eof
	sta	lnlen
	pop	psw
	mov	a,d
	ora	e		;carry reset, zero set iff <cr>
	jmp	sttp1		;set textp and return
tkiz1:	call	tkizb		;tokenize body of input line
	xra	a
	stax	b		;pseudo-eof for direct mode exec
	lxi	h,tlnad
	mov	a,l		;lsbyte of first address to A
	cma			;- first text address - 1
	add	c		;+ last address + 1 = actual length - 1
	adi	5		;+ line overhead + 1 = length
	sta	lnlen		;store tokenized line length
	pop	psw		;carry set iff no line #
	jnc	sttp1
	dcx	h
	shld	savtp		;initialize savtp for direct mode commands
sttp1:	pop	h		;return first text byte addr in HL
	shld	textp		;initialize text pointer
	ret

copyp:	mvi	a,prntt		;substitute PRINT token for ?
tkiza:	stax	b
	inx	b
	inx	h
	mvi	d,':'
	cpi	datat
	jz	tkza2		;do not tokenize DATA
	cpi	remt
	if	wild and rtpak	;suppress comments in WILD RTPAK tokenization
	jz	wrem
	cpi	''''
	jnz	tkizb
wrem:	mvi	a,cr
	stax	b
	inx	b
	ret
	else
	jz	tkza1		;or REMarks (for ?)
	cpi	''''
	jnz	tkizb		;or on-line comments
	endif			;end of NOT (WILD AND RTPAK) conditional
tkza1:	mvi	d,cr
tkza2:	call	cpyd1		;copy without tokenizing
tkizb:	call	copys		;copy spaces
	cpi	'"'
	cz	copyq		;copy to close quote
	cpi	'#'
	jz	copyh		;watch for keywords after #
	cpi	cr
	jz	copya		;finished if cr -- copy and return
	cpi	'?'
	jz	copyp		;? abbreviation for PRINT
	push	b		;save tokenized text pointer
	mvi	c,-nkeys and 0FFH	;table length to C
	lxi	d,keyta		;table address to DE
	call	tlkup		;perform table lookup
	if	key80
	jnc	tkizc		;found it
	mvi	c,-nrwds and 0FFH
	lxi	d,rwdta
	call	tlkup		;check for reserved word
	jc	tkizc		;not a keyword nor a reserved word
	adi	rwdtk+nrwds	;use tokens starting at RWDTK
	endif
tkizc:	pop	b		;restore text pointer
	jmp	tkiza		;copy A and keep tokenizing


;Tokenizer routines.
;COPYA copies one char from A through BC.
;COPYS copies successive spaces (if any) from M through BC.
;COPYQ copies " from A through BC, then copies from M through BC until
;	" (inclusive, incl. trailing spaces) or until <cr> (exclusive).
;COPYH copies hex digits, so e.g. #DEF and #BIN tokenize correctly.
;Call:	BC	address of next available destination
;	HL	address of next available char (i.e. source)
;Retn:	A	char copied for COPY1,COPYA
;		first char not copied for COPYS, COPYQ
;	BC	next available destination address
;	DE	preserved
;	HL	next available source address

copya:	stax	b		;store through BC
	inx	b
	inx	h
	ret

copyq:	mov	d,a
copyd:	stax	b
	inx	b
	inx	h		;copy current character
cpyd1:	mov	a,m
	cpi	cr
	rz			;return without copying if cr
	cmp	d
	jnz	copyd		;keep copying if neither (D) nor cr
cops0:	stax	b
	inx	b
cops1:	inx	h
copys:	mov	a,m
	cpi	' '
	rnz			;return at first non-space
	if	wild and rtpak
	jmp	cops1		;suppress spaces in WILD RTPAK tokenization
	else
	jmp	cops0
	endif

copyh:	stax	b
	inx	b
	inx	h		;copy char and spaces
	mov	a,m
	call	ldtst		;test if letter or digit
	jnc	cpyh1		;yes
	sui	20H		;convert from possible lower case
	call	ldtst
	jc	tkizb		;not a letter or digit
cpyh1:	cpi	'F'+1
	jnc	tkizb		;letter but not legit hex digit
	jmp	copyh		;ok, copy it

;TLKUP performs table lookup for the tokenizer.
;Call:	C	- number of table entries
;	DE	address of first table byte
;	HL	address of first text byte
;Retn:	Carry	set if not found
;	A	token (position of matched word from table bottom) if found,
;			first text byte if not
;	B	preserved
;	C	token if found, zero if not
;	DE	address of first text byte if found,
;			of first byte following table if not
;	HL	address of last matched text byte if found,
;			of first text byte (i.e. unchanged) if not
tlkup:	push	h		;save text pointer
tlku0:	mov	a,m		;fetch text char
	sui	20H		;convert lower case to upper
	cpi	'A'
	jc	tlku1		;char  < 'a'
	cpi	'Z'+1
	jnc	tlku1		;char > 'z'
	mov	m,a		;store converted char
tlku1:	ldax	d		;table byte to A
	cmp	m		;compare to text byte
	jnz	tlku2
	inx	d		;matched -- try next byte
	inx	h
	jmp	tlku0
tlku2:	ani	7FH		;reset sign bit of table byte
	cmp	m		;compare again
	jnz	tlku3		;failed -- no match
	mov	a,c		;match -- return token in A, Carry reset
	pop	d		;unstack the saved pointer
	ret
tlku3:	ldax	d		;failed
	inx	d
	ani	80H		;look at sign bit
	jz	tlku3		;keep looking for end of entry
	pop	h		;restore text pointer
	inr	c		;increment count
	jnz	tlkup		;try next table entry
	mov	a,m		;return text char if no match
	stc			;return Carry on failure
	ret
	endif			;end of WILD OR NOT COMPL conditional

	if	not compl
;ADDL adds a tokenized line to source text.
;Call:	(lnnum)	line #
;	(lnlen)	line length, 0 to delete
;	(newln)	address of first byte of new line
;	(eofad)	end of source file address
;If line # is already in text, the old line is replaced with the new.
;If not, the new line is simply inserted.
;Branches out if memory full, i.e. if source top overlaps symbol table bottom.
;Clobbers all registers.
addln:	if	romsq
	lxi	d,srcad
	lhld	sourc
	call	cmdhu		;Zero set iff addressing working space
	cnz	lnnu0		;not in working space, reset LNNUM to 0
	jnz	roerr		;and issue fatal RO error
	endif
	lhld	lnnum
	xchg			;line # to DE
	call	findl		;look for it
	push	h		;save pointer for line insertion later
	lxi	b,0		;length of old line to BC
	jc	addl0		;0 if no such line,
	mov	c,m		;  else length from pointer
addl0:	dad	b		;address of next  line = old + length to HL
	shld	temp		;save it for old > new case below
	xchg
	call	cplde		;- next line address to DE
	lhld	eofad		;end of file address to HL
	inx	h
	dad	d		;eof - next line + 1 = count to HL
	xchg			;count to DE for block move
	lda	lnlen		;new line length to A, 0 to delete
	sub	c		;new - old = offset
	mov	c,a		;offset to c
	jc	addl4		;old > new
	jz	addl2		;old = new, so just insert it
;block move text below longer new line
	push	d		;old < new
	lhld	symta
	lxi	d,-9
	dad	d		;leave enough room to compute trivial exprs
	xchg			;symbol table address to DE
	lhld	eofad
	push	h
	dad	b		;HL gets eofad + offset = destination
	mov	b,h
	mov	c,l
	call	cmbdu		;compare destination to symbol table addr
	cnc	lnnu0		;out of memory -- reset LNNUM to 0
	jnc	omerr		;and issue fatal OM error
	shld	eofad		;reset EOF address
	pop	b		;BC gets old eof address = source
	pop	d		;restore count
addl1:	ldax	b		;fetch byte from BC
	mov	m,a		;store through HL
	dcx	b
	dcx	h
	dcx	d
	mov	a,d
	ora	e
	jnz	addl1		;move more bytes
;insert current line into source
addl2:	pop	d		;recover insertion address
	lda	lnlen
	ora	a
	jz	addl3		;done if length = 0
	stax	d		;else store length
	sui	4		;length - overhead = length to move
	lxi	b,tlnad		;source address to BC
	lhld	lnnum
	if	editc
	shld	errln		;set ERRLN
	endif
	xchg			;insertion addr to HL, lnnum to DE
	call	momde		;store line #
	inx	h
	mvi	m,0		;store zero break byte
	inx	h		;HL now has destination
	mov	e,a		;actual length to E for insertion
	call	movd0		;copy new line into text
;reset stacks and return
addl3:	lhld	eofad		;eof address to HL
	jmp	new1		;reset stacks and return
;block move text below shorter new line
addl4:	lhld	temp		;recover next text line address
	push	h
	dcr	b		;B becomes 255, since offset in BC is < 0
	dad	b		;HL gets next + offset = destination
	pop	b		;BC gets next = source
	call	moved		;move text down in memory
	dcx	h
	shld	eofad		;reset EOF address
	jmp	addl2		;and insert current line

;LNNU0 resets LNNUM to 0.
lnnu0:	lxi	h,0
	shld	lnnum
	ret

	endif			;end of NOT COMPL conditional

;MOVED performs block move of memory Down.
;Call:	BC	source address
;	DE	count (# of bytes to move)
;	HL	destination address
;Retn:	A	clobbered
;	BC	address of last source byte + 1
;	DE	zero
;	HL	address of last destination byte + 1
;MOVEB moves 2 or 4 bytes with MOVED.
moveb:	if	float		;byte count to DE
	mvi	e,vbyts-1
	else
	mvi	e,2
	endif			;and fall through to MOVD0
movd0:	mvi	d,0
moved:	ldax	b		;BC contains source
	mov	m,a		;HL contains destination
	inx	b
	inx	h
	dcx	d		;DE contains count
	mov	a,d
	ora	e		;test if (DE) = 0
	jnz	moved
	ret

;FINDL finds line with given line # in user source text.
;Call:	DE	desired line #
;Retn:	A	clobbered
;	BC	value of HL when called
;	DE	preserved, i.e. desired line #
;	HL	address of a user source line length byte, namely:
;			Success, length byte of desired line # in source
;			Failure, length byte of first greater line #, or EOF
;	Carry	set iff not found
;	Zero	set if found or if at end of table
findl:	push	h
	if	romsq
	lhld	sourc
	else
	lxi	h,srcad		;search from start
	endif
fndl1:	mov	a,m		;length byte to A
	ora	a
	jz	fndl2		;not found if at end of table
	inx	h
	mov	c,m
	inx	h
	mov	b,m		;source line # to BC
	dcx	h
	dcx	h		;point to length byte again
	call	cmbdu		;compare to desired line #
	jz	fndl3		;equal -- success
	jnc	fndl2		;greater -- failure
	mov	c,m		;less, keep trying -- length to BC
	mvi	b,0
	dad	b		;let HL point to address of next entry
	jmp	fndl1
fndl2:	stc			;failure -- return with carry set
fndl3:	pop	b		;return old HL in BC
	ret

;FDLNO does a FINDL, issues fatal US error if not found.
fdlno:	call	findl
	rnc
	jmp	userr

;end of TOKENIZE
	page
;XMATH 06/09/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;floating point extended function package


	if	float and (not f9511) and (not fpbcd)


;8080 FLOATING POINT POLYNOMIAL EXPANDER - FPOLY
;  THIS ROUTINE EVALUATES A POLYNOMIAL OF THE FORM
;F(X) = A(0) + A(1)*X^(1*(K+J)) + A(2)*X^(2*(K+J))
;		+ A(2)*X^(2*(K+J)) + ...
; WHERE
;	A(0) IS AN INITIAL SUM VALUE IN THE
;	     THE FLOATING POINT ACCUMULATOR
;	A(N) IS A TERM CONSTANT FROM A LIST, THE
;	     ADDRESS OF WHICH IS SUPPLIED IN HL
; AND K AND J ARE INTEGER VALUES SIMULATED BY
; A SUPPLIED VALUE FOR THE INITIAL POWER TERM, AND
; A SUPPLIED VALUE FOR THE POWER TERM TO BE
; MULTIPLIED BY AFTER EACH TERM CALCULATION.
;    THE EVALUATION IS TERMINATED BY ONE OF TWO EVENTS.
;	1. IF THE SUPPLIED SIGNIFICANCE STOP VALUE
;	   IS EVER LESS THAN THE SIGNIFICANCE INDEX
;	   RETURNED BY THE FLOATING POINT ADD ROUTINE
;	   (INDICATING THE TERM WOULD NOT AFFECT THE RESULT)
;OR
;	2. A TERM CONSTANT WITH AN EXPONENT OF ZERO
;	   IS ENCOUNTERED.
;
;DETAILED CALLING SEQUENCE ...
;
;	LXI	H,INPWR		;ADDRESS OF POWER LIST
;	PUSH	H		;ON STACK
;	LXI	H,KLIST		;ADDRESS OF CONSTANT LIST
;	CALL	FPOLY
	;	...		;RETURN HERE WITH F(X) INFACC
;
;INPWR:	DS	4		;INITIAL POWER OF X
;	DS	4		;TERM POWER MULTIPLIER
;
;	...
;
;KLIST:	DB	-1		;SIGNIFICANCE STOP VALUE
;	DW	K,K		;TERM CONSTANT A(1)
;	DW	K,K		;TERM CONSTANT A(2)
;	...
;	DW	K,K		;TERM CONSTANT A(N)
;	DB	0		;TERMINATOR
;
;   A SIGNIFICANCE STOP VALUE OF -1 WILL CAUSE THE
;ROUTINE TO UNCONDITIONALLY CALCULATE ALL TERMS IN
;THE LIST.  A VALUE OF 16 IS USED IN THE SIN/COS
;ROUTINE WITH NO EFFECT ON THE ACCURACY.
;  SEE THE DESCRIPTION OF THE INDEX IN THE MATH PACKAGE WRITEUP.

FPOLY:	LXI	D,SIG		;MOVE ARGS OVER
	MOV	A,M		;GET SIG
	STAX	D		;AND SAVE
	INX	H
	SHLD	VECT		;SAVE TERM TABLE ADDRESS
	POP	H		;POP OFF RETURN ADDRESS
	XTHL			;AND EXCHANGE WITH ARGUMENT
	MVI	C,8
FPOL0:	MOV	A,M
	INX	H
	INX	D
	STAX	D
	DCR	C
	JNZ	FPOL0
	CALL	FTEST		;RESTORE
	LXI	H,FPSUM		;AND SAVE
	PUSH	H
	CALL	FSTOR		; SUM
	LXI	H,FPTXN
	CALL	FLOAD		;GET CURRENT POWER
FPOL1:	LHLD	VECT		;AND CURRENT TERM
	CALL	FMUL		;A(N)*X^(N+K)
	POP	H
	PUSH	H
	CALL	FADD		;ADD IT IN
	PUSH	PSW
	LDA	SIG
	CMP	E		;CHECK FOR NO SIGNIFICANCE
	JC	FPOL2
	POP	PSW
	POP	H
	PUSH	H
	CALL	FSTOR
	LHLD	VECT 
	LXI	D,4
	DAD	D
	SHLD	VECT 
	MOV	A,M
	ANA	A
	POP	H
	JZ	FLOAD
	PUSH	H
	LXI	H,FPTXN
	PUSH	H
	CALL	FLOAD
	LXI	H,FPTXN+4
	CALL	FMUL
	POP	H
	CALL	FSTOR
	JMP	FPOL1

FPOL2:	POP	PSW
	POP	H
	RET


;8080 INTEGER/FRACTIONAL PART - FINT
;	CALL FINT	;ARGUMENT IN FAC
;	...
;RETURNS WITH SIGNED INTEGER PART IN HL
;	   AND SIGNED FRACIONAL PART IN FAC
;
;OVERFLOW CONDITIONS SET IF
;  -32767 > VALUE > 32767

FINT:	CALL	FTEST		;SET UP ABCD TO FAC
	MOV	E,A		;SAVE EXP
	MOV	A,B		;GET M0
	ANI	80H		;AND ISOLATE SIGN BIT
	STA	FINSN		;SAVE SIGN
	MOV	A,E		;GET EXPONENT
	CPI	129		;AND CHECK RANGE
	JNC	FINT0		;NOT TOO SMALL
	LXI	H,0		;VALUE .LT. 1.0
	RET
FINT0:	CPI	144		;SEE IF TOO LARGE
	JC	FINT2		;VALUE IS IN RANGE
	LDA	FINSN		;OVERFLOW
	RLC			;SET HL
	LXI	H,7FFFH		;TO +/-
	JNC	$+4		; FULL SCALE
	INX	H
	PUSH	H
	CALL	FTEST		;RESTORE ABCD
	POP	H		;AND
	STC			;SET OFLOW AND
	RET			;EXIT

;SET UP MANTISSA FOR SHIFTING AND CLEAR INTEGER
FINT2:	XCHG			;D->H
	PUSH	B		;MOV BC
	POP	D		;TO DE
	SUI	80H		;MAKE SHIFT COUNT
	MOV	C,A		;INTO C
	MOV	B,H		;MOVE M2
	MOV	A,D		;SET
	ORI	80H		;MANTISSA
	MOV	D,A		; B23
	LXI	H,0		;CLEAR INTEGER
;MANTISSA (M0,M1,M2) IN DEB, INTEGER (I0,I1) IN HL
;SHIFT  H  L  D  E  B   LEFT 'C' BITS
;      I0,I1,M0,M1,M2
FINT3:	DAD	H		;I0,I1 LEFT
	XCHG
	DAD	H		;M0,M1 LEFT INTO CY
	JNC	$+4
	INX	D		;CY PROPAGATE INTO I0,I1
	XCHG
	XRA	A		;CLEAR A & CY
	ADD	B		;GET M2
	RAL			;LEFT B7->CY, 0->B0
	MOV	B,A		; AND RESTORE
	JNC	$+4
	INX	D		;CY PROPAGATE INTO M0,M1
	DCR	C		;DECREMENT EXP
	JNZ	FINT3		;AND CONTINUE
	XCHG			;I0,I1 <-> M0',M1'
;CHECK NEW MANTISSA (M0',M1',M2') FOR ZERO
	ORA	H		;CHECK
	ORA	L		;MN'
	JZ	FINT6		;MN' IS ZERO
;NORMALISE NEW MANTISSA IN HLB
; DECREMENTING EXPONENT IN C
FINT4:	MOV	A,H		;GET M0'
	RLC			;CHECK HIGH ORDER BIT B23
	JC	FINT5		;NORMALIZATION COMPLETE
	DAD	H		;SHIFT M0',M1' LEFT
	MOV	A,B		;GET M2'
	RAL			;M2' LEFT B7->CY, 0->B0
	MOV	B,A		;RESTORE
	JNC	$+4		;NO DATA
	INX	H		;PROPAGATE BIT
	DCR	C		;DECREMENT EXPONENT
	JMP	FINT4		;AND CONTINUE
;PREPARE FRACTIONAL PART FOR STORAGE
FINT5:	MOV	A,H		;GET M0'
	ANI	127		;KILL B23
	MOV	H,A		;(SIGN)
	LDA	FINSN		;GET SIGN OF INPUT
	ORA	H		;AND MOVE TO B23
	MOV	H,L		;SWAP FOR
	MOV	L,A		;SHLD LATER
	MOV	A,C		;GET EXPONENT
	ADI	80H		;AND BIAS UP
;STORE FRACIONAL PART FOR RE-LOAD
FINT6:	STA	FINFP		;STORE EXP
	SHLD	FINFP+1		;STORE M0',M1'
	MOV	A,B		;GET M2'
	STA	FINFP+3		;AND STORE
;TRANSFER SIGN TO INTEGER
	LDA	FINSN		;GET SIGN
	RLC
	JNC	FINT7		;POSITIVE
	MOV	A,D		;NEGATE
	CMA			;INTEGER
	MOV	D,A		; BY
	MOV	A,E		;  COMPLEMENTING
	CMA			;   AND
	MOV	E,A		;    ADDING
	INX	D		;     ONE
FINT7:	PUSH	D		;SAVE INTEGER
	LXI	H,FINFP		;RE-LOAD
	CALL	FLOAD		;FRACTION TO FAC
	POP	H		;RESTORE INTEGER
	RET			;AND EXIT


;FSQR - 8080 FLOATING POINT SQUARE ROOT
;METHOD:	APPROXIMATION FOLLOWED BY THREE
;		NEWTON ITERATIONS
;
; SQUARE ROOT (X)
;
;LET X=2^(2B)*F WHEN .25<=F<1
;THEN SQR(X)=2^B*SQR(F)
;WHERE SQR(F)=P(I)   I=NUMBER OF ITERATION
;
; P(1)=A*F+B   AS A FIRST APPROXIMATION
;   WHERE A=.875, B=.27863 WHEN .25<=F<.5
;    OR
;	   A=.578125, B=.421875 WHEN .5<=F<1
;AND THEN
;	P(I+1)=(P(I)+F/P(I))/2
;  AND P(4) IS FINAL RESULT FOR SQR(F)
;
;CALLING SEQUENCE WITH 8008/8080 MATH PACKAGE
;	CALL	FSQR		;SQR(FPAC) -> FPAC
;
;NEGATIVE INPUTS WILL BE TREATED AS POSITIVE AFTER CALLING FCERN
FSQR:	CALL	FTEST		;RESTORE
	RZ			;ZERO
	CM	FCERN		;NEGATIVE ERROR
	CM	FABS		;SET FLAG AND USE ABS VALUE
	LXI	H,FPTXX		;AND SAVE
	CALL	FSTOR		;X
	MVI	E,80H		;CALCULATE
	RRC			;EXPONENT
	JNC	$+4		; FOR
	DCR	E		;  F
	MOV	A,E		;AND
	LXI	H,FPTF		;STORE
	CALL	FSTOR		; F
	RRC			;DECIDE
	LXI	H,FPTA1
	JNC	$+6		;WHICH
	LXI	H,FPTA2		; A AND B
	PUSH	H		;    TO USE
	CALL	FLOAD		; AND FETCH A
	LXI	H,FPTF		;CALCULATE
	CALL	FMUL		;AF
	POP	H		; AND
	INX	H		;  THEN
	INX	H		;   GET
	INX	H		;    B
	INX	H
	CALL	FADD		;AF+B
	LXI	H,FPTP		;AND
	PUSH	H
	CALL	FSTOR		; SAVE P1
	CALL	NEWTN		;THREE
	CALL	NEWTN		;NEWTON
	CALL	NEWTN		; ITERATIONS
	MOV	H,A		;SAVE RESULT EXPONENT
	LDA	FPTXX		;CALCULATE
	SUI	127		;EXPONENT
	RAR			; FOR
	ADD	H		;  RESULT
	POP	H
	MOV	M,A		;AND SET IT
	JMP	FLOAD		;LOAD RESULT

NEWTN:	LXI	H,FPTF		;GET
	CALL	FLOAD		;F
	LXI	H,FPTP
	PUSH	H
	CALL	FDIV		;F/P
	POP	H
	PUSH	H
	CALL	FADD		;F/P+P
	DCR	A		;(F/P+P)/2
	POP	H		;AND
	JMP	FSTOR		; SAVE P(I+1)


FPTA1:	DB	80H,60H,0,0		;.875
	DB	7FH,0EH,0A8H,98H	;.27863

FPTA2:	DB	80H,14H,0,0		;.578125
	DB	7FH,58H,0,0		;.421875


;********* END - FSQR *********


;8080 FLOATING POINT EXPONENTIAL - FEXP
;	CALL	FEXP		;E^(FAC) -> FAC
;
; **** INTERNAL OVER/UNDERFLOW WILL OCCUR FOR INPUT
;	VALUES GREATER THAN LN(2^127)/(LOGBASE2(E)).
;	OR APPROXIMATELY
;	      -61 < VALUE < 61
;
; IF OVERFLOW OCCURS THEN 'ERROR' WILL BE CALLED AND
; FAC WILL BE SET TO FULL SCALE VALUE.
;
;METHOD:
; POLYNOMIAL APPROXIMATION
;TO FIND E^X THE FOLLOWING IDENTITY IS USED.
;TO REDUCE THE RANGE, WE LET
;	X LOG2 E = N + D + Z
;WHERE ...
;	N IS THE INTEGRAL PORTION OF THE REAL NUMBER,
;	D IS A DISCRETE FRACTION (1/8, 3/8, 5/8, OR
;	  7/8) OF THE REAL NUMBER (FRAC PART), AND
;	Z IS THE REMAINDER WHICH IS IN THE RANGE 
;	  -1/8 <= Z <= 1/8
; THUS,
;		E^X = 2^N * 2^D * 2^Z
;AND IT IS NECCESSARY TO ONLY APPROXIMATE 2^Z FOR
;-1/8 <= Z <= 1/8 BY USING THE POLYNOMIAL F(Z).
;
; F(Z) = A0 + A1*Z + A2*Z^2 + A3*Z^3 + A4*Z^4 +
;				A5*Z^5
;WHERE ...
;	A0 = 1.0
;	A1 = .69314718057
;	A2 = .24022648580
;	A3 = .055504105406
;	A4 = .0096217398747
;	A5 = .0013337729375
;
FEXP:	CALL	FTEST		;SET ABCD TO FAC
	MOV	A,B		;GET SIGN
	STA	FPSGN		;AND SAVE
	CALL	FABS		;TAKE ABS VALUE
	LXI	H,LOG2E		;X=X*LOG2(E)
	CALL	FMUL
	CALL	FINT		;INTEGER TO HL, FRAC IN FAC
	PUSH	H		;SAVE INTEGER
	LXI	H,FPTX		;AND
	CALL	FSTOR		; FRACTION
	POP	H		;RESTORE INTEGER
	MOV	A,H		;TO CHECK RANGE
	ANA	A
	JZ	FEXP1		;CAN'T BE > 256

FEXP0:	LDA	FPSGN		;NUMBER TOO LARGE
	RLC			;SO RETURN
	JC	FZRO		; 0 OR FULL SCALE
	JMP	FOVER		;NONFATAL OV ERROR, TAKE MAX AND RETURN
FEXP1:	MOV	A,L		;CHECK N
	CPI	127		;LARGEST THAT WILL FIT
	JNC	FEXP0		;E^88 = 1.7 E+38
	STA	FPTN		;SAVE INT PART N
;SELECT VALUE FOR 'D'
	LDA	FPTX		;GET EXPONENT OF FRAC
	LXI	D,4
	LXI	H,FPTDF
	CPI	127
	JC	FEXP5		;FRAC < .25
	DAD	D		;FPTDF+4 TO HL
	JZ	FEXP5		;.25 <= FRAC < .5
	DAD	D		;FPTDF+8 TO HL
	LDA	FPTX+1		;GET M0
	RLC			;SHIFT B6
	RLC			;TO CARRY
	JNC	FEXP5		;.5 <= FRAC < .75
	DAD	D		;.75 <= FRAC < 1, FPTDF+12 TO HL
FEXP5:	PUSH	H		;HL POINTS TO SELECTED D
	LXI	H,FPTX		;X TO FAC
	CALL	FLOAD
	POP	H		;ADDRESS OF D
	PUSH	H		;SAVED AGAIN
	CALL	FSUB		;CALCULATE Z
	MOV	E,A		;AND
	LDA	FPSGN		; TRANSFER
	RLC			;  SIGN
	MOV	A,E		;   TO Z
	CC	FCHS
	LXI	H,FPTZ0		;SAVE Z
	PUSH	H		;PUSH FOR FPOLY
	CALL	FSTOR		; IN
	LXI	H,FPTZ1		;  POLY
	CALL	FSTOR		;   TABLE
	LXI	H,FPONE		;GET TERM
	CALL	FLOAD		;A0 = 1.0
	LXI	H,EXPLY		; AND DO THE
	CALL	FPOLY		;  POLYNOMIAL F(Z)
	POP	H		;SELECT
	LXI	D,16		;SIGNED
	XCHG			; VALUE
	LDA	FPSGN		;  FOR
	RLC			;   2^D
	JNC	$+4
	DAD	H		;2^-D TABLE
	DAD	D		;HL HAS ADDRESS
	PUSH	H		;OF 2^D
	CALL	FTEST		;RESTORE
	POP	H		;AND CALCULATE
	CALL	FMUL		;2^D * F(Z)
;NOW MULTIPLY BY 2^N
	MOV	E,A		;SAVE EXP
	LDA	FPSGN		;AND
	RLC			;TRANSFER
	LDA	FPTN		;SIGN
	JNC	$+5		; TO N
	CMA
	INR	A		;NEGATE N
	ADD	E		;RESULT = 2^N * 2^D * F(Z)
	LXI	H,FPTZ1		;STORE TEMP
	PUSH	H
	CALL	FSTOR		;FOR RELOAD
	POP	H		; TO
	JMP	FLOAD		;  EXIT WITH FAC = E^X


;D TEST VALUES AND 2^D TABLES
FPTDF:	DW	7EH,0		;.125
	DW	407FH,0		;.375
	DW	2080H,0		;.625
	DW	6080H,0		;.875

;2^D FOR PLUS D VALUES
	DW	0B81H,0C295H	;2^(1/8)
	DW	2581H,0D7FEH	;2^(3/8)
	DW	4581H,2A67H	;2^(5/8)
	DW	6A81H,0C7C0H	;2^(7/8)

;2^D FOR MINUS D VALUES
	DW	6A80H,0C7C0H	;2^(-1/8)
	DW	4580H,2A67H	;2^(-3/8)
	DW	2580H,0D7FEH	;2^(-5/8)
	DW	0B80H,0C295H	;2^(-7/8)

;MISC. CONSTANTS
LOG2E:	DW	3881H,3BAAH	;LOG2(E) 1.442695041
FPMAX:	DW	7FFFH,-1	;POS FULL SCALE


EXPLY:	DB	-1		;POLY TABLE

	DW	3180H,1872H	;A1 .69314718057
	DW	757EH,0EFFDH	;A2 .24022648580
	DW	637CH,4658H	;A3 .055504105406
	DW	1D7AH,81A4H	;A4 .0096217398747
	DW	2E77H,0FED1H	;A5 .0013337729375

	DB	0		;TABLE TERMINATOR

;****** END - FEXP ******


;8080 FLOATING POINT NATURAL LOGARITHM - FLN
;  THIS ROUTINE CALCULATES THE NATURAL LOGARITHM
;OF THE NUMBER IN THE FLOATING POINT ACCUMULATOR.
;
;	LN(FAC) -> FAC
;
;METHOD - POLYNOMIAL APPROXIMATION
;
; GIVEN A NORMALISED REAL NUMBER
;	X = 2^K * F
;WHERE THE RANGE OF F IS .5 <= F < 1, AND J AND G
;ARE FOUND SUCH THAT X = 2^J * G WHERE
;(SQR(2)/2) <= G < SQR(2).  THIS IS DONE BY SETTING
;J=K-1, G=2*F IF F < SQR(2)/2 AND J=K, G=F OTHER-
;WISE.
;
; THUS:
;	LN(X) = J * LN(2) + LN(G)
;
;   THE APPROXIMATION FOR LN(G), WHERE
;SQR(2)/2 <= G < SQR(2), IS BASED ON THE SERIES
; LN((V+X)/(V-X) = 2((X/V) + (X^3/(3*V^3)) +
;			(X^5/(5*V^5)) + ... )
;WHICH CONVERGES FOR (-V<X<V).
;
;   WITH THE TRANSFORMATION
;     X = V((G-1)/(G+1)), V = (SQR(2)+1)^2
;SO THAT -1 <= X < 1 FOR SQR(2)/2  <= G < SQR(2).
;
; SUBSTITUTING
;	LN(G) = 2(Z + Z^3/3 + Z^5/5 + ... )
; WHERE  Z = X/V = (G-1)/G+1).
;
;  THE APPROXIMATION USED IS G(Z) FOR LN(G)/Z IN
;THE RANGE SQR(2)/2 <= G < SQR(2).
; THUS THE REQUIRED CALCULATION IS
;	LN(X) = J * LN(2) + Z * G(Z)
;  G(Z) = B0 + B2*Z^2 + B4*Z^4 + B6*Z^6 + B8*Z^8
;WHERE
;	B0 = 2.0
;	B2 = .666666564181
;	B4 = .400018840613
;	B6 = .28453572660
;	B8 = .125

FLN:	CALL	FTEST		;RESTORE
	CM	FCERN
	JZ	FCERN		;FC ERROR AND RETURN IF ZERO
	CM	FABS		;MAKE INPUT POSITIVE AFTER ERROR
	STA	FLNK		;SAVE EXPONENT
	MVI	A,80H		;SAVE
	LXI	H,FLNF		;FRACTIONAL
	PUSH	H
	CALL	FSTOR		; PART
	POP	H		;RELOAD
	PUSH	H		; AND RESAVE
	CALL	FLOAD		;  TO FACC
	LXI	H,FLNR2		;CHECK RANGE
	CALL	FSUB		;NEG IF F < SQR(2)/2
	POP	H		;POINT TO F
	JP	FLNA		;JMP IF F >= SQR(2)/2
	DCX	H		;POINT TO K
	DCR	M		;J = K-1
	INX	H		;BACK TO F
	INR	M		;G = 2 * F
FLNA:	CALL	FLOAD		;GET G
	LXI	H,FPONE		;GET 1.0
	CALL	FADD		;G+1
	LXI	H,FLNZI		;AND
	PUSH	H
	CALL	FSTOR		; SAVE
	LXI	H,FLNM2		;GET -2.0
	CALL	FADD		;G-1
	POP	H
	PUSH	H
	CALL	FDIV		;(G-1)/(G+1)
	LXI	H,FLNF		;SAVE
	PUSH	H
	CALL	FSTOR		;Z
	POP	H
	CALL	FMUL		;Z^2
	POP	H
	PUSH	H		;SAVE FOR FPOLY
	CALL	FSTOR		;SAVE
	LXI	H,FLNZ2		;SAVE
	CALL	FSTOR		;TERM POWER
	CALL	FZRO		;CLEAR SUM
	LXI	H,FLNPL		;AND
	CALL	FPOLY		; CALC POLYNOMIAL
	LXI	H,FLNM2		;ADD
	CALL	FSUB		;2.0
	LXI	H,FLNF		;AND
	PUSH	H
	CALL	FMUL		;Z*G(Z)
	POP	H
	PUSH	H
	CALL	FSTOR		;Z*G(Z)
;FLOAT J, CALCULATE J * LN(2)
	LDA	FLNK		;GET J
	SUI	80H		;UN-BIAS EXP
	MVI	E,32		;SET INTEGER SCALING
	MOV	D,A		;INTEGER TO I3
	LXI	B,-1		;INTEGER
	MVI	A,-1		;NEGATIVE
	JM	$+5		;GO FLOAT
	INX	B		;INTEGER
	INR	A		;NEGATIVE
	CALL	FFLOT		;FLOAT J
	LXI	H,FLNL2
	CALL	FMUL		;J*LN(2)
	POP	H
	JMP	FADD		;Z*G(Z)


FLNR2:	DW	3580H,0F304H	;SQR(2)/2
FLNL2:	DW	3180H,1872H	;LN(2)
FLNM2:	DW	8082H,0		;-2.0

FLNPL:	DB	16		;SIG INDEX STOPPER
	DW	2A80H,0A9AAH 	;B2 .666666564181
	DW	4C7FH,45CFH	;B4 .400018840613
	DW	117FH,0ABAEH 	;B6 .28453572660
	DW	7EH,0		;B8 .125

	DB	0		;TERMINATOR

;******* END - FLN ******


;8080 FLOATING POINT SIN/COS ROUTINE - FSIN, FCOS
;  THIS ROUTINE WILL CALCULATE THE SINE OR COSINE
;OF THE ANGLE IN THE FLOATING POINT ACCUMULATOR.
;
;  THE ANGLE MUST BE IN RADIANS AND MAY HAVE ANY
;MAGNITUDE.
;
;	SIN(FAC) -> FAC
;	COS(FAC) -> FAC
;
;METHOD:
;	POLYNOMIAL APPROXIMATION
;
;   GIVEN A REAL NUMBER, X, N, AND Y ARE DEFINED
;SUCH THAT
;	X/(2*PI) = N + Y
;WHERE N IS AN INTEGER AND 0 <= Y < 1.
;THUS, X = 2*PI*N + 2*PI*Y, AND THE IDENTITIES ARE
;  SIN(X) = SIN(2*PI*Y) AND COS(X) = COS(2*PI*Y)
;
;THE POLYNOMIAL APPROXIMATION, F(Z), FOR THE
;FUNCTION SIN(2*PI*Z)/Z IS USED WHERE
;-.25 <= Z < .25.
;
;   THE PROPERTIES OF SINES AND COSINES ARE USED TO
;COMPUTE THESE FUNCTIONS AS FOLLOWS:
;
;	COS(2*PI*Y) = F(Z)
;WHERE
;	Z = .25-Y IN THE RANGE 0 <= Y < .5
;	Z = Y-.75 IN THE RANGE .5 <= Y < 1
;
;	SIN(2*PI*Y) = F(Z)
;WHERE
;	Z = Y IN THE RANGE 0 <=Y < .25
;	Z = .5-Y IN THE RANGE .25 <= Y < .75
;	Z = Y-1 IN THE RANGE .75 <= Y < 1
;
; F(Z) = A1*Z + A2*Z^3 + A3*Z^5 + A4*Z^7 + A5*Z^9
;WHERE
;	A1 = 6.2831853
;	A2 = -41.341681
;	A3 = 81.602481
;	A4 = -76.581285
;	A5 = 39.760722

FSIN:	DB	3EH		;(MVI A,0AFH) FN CODE FOR SIN
FCOS:	XRA	A		;0 FN CODE FOR COS
	STA	FSCFX		;SAVE FUNCTION CODE
	CALL	FTEST		;RESTORE
	MOV	A,B		;GET M0
	ANI	80H		;STRIP OFF
	STA	FSCSG		; AND SAVE
	CALL	FABS		;GET ABS(FAC)
	LXI	H,F2PI
	CALL	FDIV		;X/(2*PI)
	CPI	129
	CNC	FINT		;Y >= 1, GET FRACTIONAL PART
	MOV	E,A		;SAVE EXPONENT
	LDA	FSCFX		;CHECK
	RLC			;FUNCTION CODE
	MOV	A,E		;RESTORE EXPONENT
	JC	FSC3		;FUNCTION IS SIN
	CPI	80H
	JC	FSC2		;Y < .5
	LXI	H,FSC75		;Y - .75
	JMP	FSC6
FSC2:	CALL	FCHS		;-Y
	LXI	H,FSC25		;.25-Y
	JMP	FSC4A
FSC3:	CPI	127		;Y < .25 ?
	JC	FSC7		;YES, GO DO POLY
	CPI	80H
	JC	FSC4		;Y < .5
	MOV	E,A		;SAVE EXP
	MOV	A,B		;CHECK
	RLC			;B22
	RLC			; FOR
	MOV	A,E		;  .75, RESTORE EXPONENT
	JC	FSC5		;Y >= .75
FSC4:	CALL	FCHS		;Y < .75 SO -Y
	LXI	H,FSC50
FSC4A:	CALL	FADD		;.5-Y
	JMP	FSC7
FSC5:	LXI	H,FPONE
FSC6:	CALL	FSUB		;Y-1
FSC7:	LXI	H,FSCZ		;SAVE
	PUSH	H
	CALL	FSTOR		;Z
	POP	H
	PUSH	H		;SAVE FOR FPOLY
	CALL	FMUL		;Z^2
	LXI	H,FSCZ2
	CALL	FSTOR		;SAVE Z^2
	CALL	FZRO		;ZERO SUM
	LXI	H,FSCPL		;AND
	CALL	FPOLY		; DO POLY
	MOV	E,A		;SAVE EXP
	LDA	FSCFX		;AND CHECK
	RLC			; FUNCTION
	MOV	A,E		;RESTORE EXPONENT
	RNC			;AND EXIT IF COS FUNCTION
	LDA	FSCSG		;GET INPUT SIGN
FSC8:	RLC			;TO CY -- FATN ENTRY POINT
	MOV	A,E		;RESTORE EXPONENT
	RNC			;RETURN IF INPUT +
	JMP	FCHS		;ELSE CHANGE RESULT SIGN


FPI:	DW	4982H,0DB0FH	;PI
FHPI:	DW	4981H,0DB0FH	;PI/2
FSC75:	DW	4080H,0		;.75
FSC25:	DW	7FH,0		;.25
FSC50:	DW	80H,0		;.5

FSCPL:	DB	16		;SIN/COS POLY SIG STOPPER
F2PI:	DW	4983H,0DB0FH	;A1 PI*2
	DW	0A586H,0E25DH	;A2 -41.341681
	DW	2387H,7834H	;A3 81.602481
	DW	9987H,9E29H	;A4 -76.581285
	DW	1F86H,0FB0AH	;A5 39.760722
	DB	0		;TERMINATOR


;****** END - FSIN/FCOS ******


;8080 FLOATING POINT REAL BASE TO REAL EXPONENT
;		FAX
;  THIS ROUTINE RAISES A REAL NUMBER TO A REAL POWER. 
;	***** NOTE *****
;  BECAUSE OF OVERFLOW CONDITIONS IN THE ROUTINE
;FLN, THE FOLLOWING LIMITS ARE PLACED ON THE
;INPUTS TO FAX:
;
;	A > 0
;AND
;	ABS(X*LN(A)) < 50.0
;
;CALLING SEQUENCE ...
;	LXI H,X		;ADDRESS OF X
;	CALL FAX	;FAC ^ X  ->  FAC
;
;METHOD
;If X is a positive integer < 256, iterated multiplication is used.
;Otherwise the identity  A^X = E^(X*LN(A)) is used.

FATOX:	XCHG
	INX	H		;ARG2 POINTER TO HL
FAX:	PUSH	H
	call	ftest		;load registers
	lxi	h,ftmpa
	call	fstor		;save A in FTMPA
	pop	h
	push	h
	call	fload		;get X to FACC
	call	fint		;check if X is integer
	push	psw
	push	h		;save integer part
	lxi	h,ftmpa
	call	fload		;restore A to FACC
	pop	d		;integer part to DE
	pop	psw
	pop	h		;X pointer to HL
	ora	d		;Zero set iff A positive integer < 256
	jnz	fax2		;raise to real power
	inr	e
	push	d		;save the integer power+1
	lxi	h,fpone		;initialize result to 1.0
	call	fload
fax1:	pop	d
	dcr	e		;decrement multiply count
	jz	ftest		;done, set registers and return
	push	d
	lxi	h,ftmpa
	call	fmul		;multiply result by A
	jmp	fax1		;and continue multiplying
fax2:	push	h		;raising to real, large or negative power
	CALL	FLN		;LN(A)
	POP	H
	CALL	FMUL		;X*LN(A)
	JMP	FEXP		;E^(X*LN(A))


	if	false
;8080 FLOATING POINT COMMON LOGARITHM - FLOG
;  THIS ROUTINE CALCULATES THE COMMON OR BASE 10
;LOGARITHM OF THE NUMBER IN THE FLOATING POINT
;ACCUMULATOR.
;
;	LOG(FAC) -> FAC
;
;METHOD
;	 THE FOLLOWING IDENTITY IS USED.
;
;	LOG(X) = LN(X)/LN(10)

FLOG:	CALL FLN	;LN(ABS(X))
	LXI H,FLGTI	;1/LN(10)
	JMP FMUL	;LN(X)/LN(10)


;8080 FLOATING POINT ANTILOG - FALOG
;
; THIS ROUTINE CALCULATES THE BASE 10 ANTILOG OF
;THE NUMBER IN THE FLOATING POINT ACCUMULATOR.
;
;	ALOG(FAC) -> FAC
;
;METHOD
;	 THE FOLLOWING IDENTITY IS USED
;	10^X = E^(X*LN(10))

FALOG:	CALL FTEST	;RESTORE
	LXI H,FLOGT	;LN(10)
	CALL FMUL	;X*LN(10)
	JMP FEXP	;E^(X*LN(10))


;8080 DEGREE>RADIAN AND RADIAN>DEGREE - FCDR, FCRD
; THESE ROUTINES CONVERT THE NUMBER IN THE FLOATING
;POINT ACCUMULATOR FROM DEGREES TO RADIANS (FCDR)
;OR FROM RADIANS TO DEGREES (FCRD).
;
;	FAC;DEGREES -> FAC;RADIANS
;	FAC;RADIANS -> FAC;DEGREES
;
;METHOD
;	 THE FOLLOWING IDENTITIES ARE USED.
;
;	X;RADIANS = X;DEGREES * PI/180
;	X;DEGREES = X;RADIANS * 180/PI

FCDR:	CALL FTEST	;RESTORE
	LXI H,FXDR
	JMP FMUL	;X*PI/180

FCRD:	CALL FTEST	;RESTORE
	LXI H,FXRD
	JMP FMUL	;X*180/PI

	endif

;8080 FLOATING POINT TANGENT - FTAN
;   THIS ROUTINE CALCULATES THE TANGENT OF THE ANGLE
;IN THE FLOATING POINT ACCUMULATOR.
;   THE ANGLE MUST BE EXPRESSED IN RADIANS.
;
;	TAN(FAC) -> FAC
;
;METHOD
;	 THE FOLLOWING IDENTITY IS USED.
;
;	TAN(X) = SIN(X)/COS(X)
;
;	***** NOTE *****
; FOR VALUES OF X CLOSE TO 0 OR CLOSE TO A MULTIPLE
;OF 2*PI, EITHER THE SIN OR COS OF X WILL BE CLOSE
;TO UNITY.  THIS CONDITION WILL AFFECT THE ACCURACY
;USING THE IDENTITY ABOVE AND IT IS SUGGESTED
;THAT OTHER METHODS BE USED TO COMPUTE THE FUNCTION
;TAN(X) IF THIS INACCURACY CAN NOT BE TOLERATED.

FTAN:	CALL	FTEST		;RESTORE
	RZ
	LXI	H,FTMPA
	PUSH	H
	CALL	FSTOR		;SAVE X
	CALL	FCOS		;COS(X)
	LXI	H,FTMPB
	CALL	FSTOR		;SAVE COS(X)
	POP	H
	CALL	FLOAD		;GET X
	CALL	FSIN		;SIN(X)
	LXI	H,FTMPB
	JMP	FDIV		;SIN(X)/COS(X)


	if	false
FLGTI:	DW 5E7FH,0D95BH	;1/LN(10)
FLOGT:	DW 1382H,8E5DH	;LN(10)

FXDR:	DW 0E7BH,35FAH	;PI/180
FXRD:	DW 6586H,0E12EH	;180/PI
	endif


;****** END -FAX, FLOG, FALOG, FCDR, FCRD, FTAN *****


	if	not camac
;8080 FLOATING POINT ARCTANGENT - FATAN
;   THIS ROUTINE CALCULATES THE ARCTANGENT OF THE
;NUMBER IN THE FLOATING POINT ACCUMULATOR.
;
;	ATAN(FAC) -> FAC
;
;METHOD
;	 POLYNOMIAL APPROXIMATION
;
;   THIS ROUTINE IS BUILT AROUND A POLYNOMIAL, F(X),
;THAT APPROXIMATES ATAN(Z) IN THE RANGE
;-.25 <= Z <= .25.  THE ATAN(Z) FOR VALUES OF Z
;OUTSIDE THIS RANGE IS FOUND BY USING THE FOLLOWING
;IDENTITIES:
;	      ATAN(-Z) = -ATAN(Z)
;AND
;	ATAN(Z) = A(K) + ATAN((Z-B(K))/(Z*B(K)+1))
;WHERE
;	A(K) = K*PI/7, B(K) = TAN(A(K))
;AND K IS DETERMINED SO THAT
;TAN((2*K-1)*PI/14) <= ABS(Z) <= TAN((2*K+1)*PI/14)
;WITH K = 1, 2, OR 3.
;
;  HAVING DETERMINED THE VALUE OF K APPROPRIATE
;TO Z, THE TRANSFORMATION
;	X = (Z-B(K))/(Z*B(K)+1)
;PUTS X IN THE RANGE -TAN(PI/14) < X < TAN(PI/14).
;THE POLYNOMIAL WORKS OVER A SLIGHTLY LARGER RANGE
;TO ALLOW DETERMINATION OF K USING THE VALUES
;	K=0  IF ABS(Z) < .25
;	K=1  IF .25 < ABS(Z) < .75
;	K=2  IF .75 < ABS(Z) < 2
;	K=3  IF ABS(Z) > 2
;THEN
;	ATAN(Z) = A(K) + F(X)   IF Z >= 0
;	ATAN(Z) = -A(K) - F(X)  IF Z < 0
;F(X) = X*(1 - Q1*X^2 + Q2*X^4 - Q3*X^6)
;WHERE
;	Q1 = .333329573
;	Q2 = .199641035
;	Q3 = .131779888

FATAN:	CALL	FTEST		;RESTORE
	RZ
	MOV	E,A		;SAVE EXPONENT
	MOV	A,B		;GET M0
	ANI	80H		;ISOLATE SIGN, B23
	STA	FATSG		;AND SAVE
	CALL	FABS
	MOV	E,A		;SAVE EXP
	LXI	H,FATK
	MVI	M,0		;K=0
	CPI	127
	JZ	FAT4		;.25 <= Z < .5, SO K=1
	JC	FAT7		;Z < .25, SO K=0 AND DO F(X), X=Z
	MVI	M,2
	CPI	129
	JZ	FAT6		;.75 <= Z < 1, K=2
	JNC	FAT4		;Z > 2, K=3
	MOV	A,B
	RLC
	RLC			;CHECK 1/4 BIT
	DCR	M
	JNC	FAT6		;.5 <= Z <.75, K=1
FAT4:	INR	M
FAT6:	MOV	A,M		;FETCH K
	PUSH	PSW		;SAVE K
	MOV	A,E		;GET EXPONENT
	LXI	H,FATZX		;SAVE
	CALL	FSTOR		;Z
	POP	PSW		;GET K
	DCR	A		;K-1
	ADD	A		;(K-1)*2
	ADD	A		;(K-1)*4
	ADD	A		;(K-1)*8
	LXI	H,FATC1		;START OF TABLE
	MOV	E,A		;CALCULATE
	MVI	D,0		;ADDRESS
	DAD	D		; OF A(K)
	PUSH	H		;AND SAVE IT
	LXI	D,4
	DAD	D		;ADDRESS OF B(K)
	PUSH	H		;SAVED ALSO
	CALL	FLOAD		;GET B(K)
	LXI	H,FATZX
	PUSH	H
	CALL	FMUL		;Z*B(K)
	LXI	H,FPONE
	CALL	FADD		;Z*B(K)+1
	LXI	H,FATXM
	CALL	FSTOR		;SAVE Z*B(K)+1
	POP	H
	CALL	FLOAD		;GET Z
	POP	H		;ADDRESS OF B(K)
	CALL	FSUB		;Z-B(K)
	LXI	H,FATXM
	CALL	FDIV		;(Z-B(K))/(Z*B(K)+1)
FAT7:	LXI	H,FATZX
	PUSH	H
	CALL	FSTOR		;SAVE X
	POP	H
	CALL	FMUL		;X^2
	LXI	H,FATXM
	PUSH	H		;SAVE FOR FPOLY
	CALL	FSTOR		;SAVE INITIAL POWER
	LXI	H,FATX2		;SAVE TERM POWER MULTIPLIER
	CALL	FSTOR
	LXI	H,FPONE
	CALL	FLOAD		;INITIALISE SUM
	LXI	H,FATPL
	CALL	FPOLY		;F(X)
	LXI	H,FATZX
	CALL	FMUL		;X*F(X)
	MOV	E,A		;SAVE EXPONENT
	LDA	FATK		;GET K
	ANA	A		;SEE IF 0
	MOV	A,E		;RESTORE EXPONENT
	JZ	FAT8		;K=0, NO A(K)
	POP	H		;ADDRESS OF A(K)
	CALL	FADD		;A(K)+F(X)
FAT8:	MOV	E,A		;SAVE EXP
	LDA	FATSG		;GET INPUT SIGN
	JMP	FSC8		;EXIT AS FOR SIN AND COS

FATC1:	DW	657FH,0FAC8H	;A(1) PI/7 .4487989506
	DW	767FH,0F390H	;B(1) TAN(A(1)) .4815746188
	DW	6580H,0FAC8H	;A(2) 2*PI/7 .8975979011
	DW	2081H,0C681H	;B(2) TAN(A(2)) 1.253960337
	DW	2C81H,0BB56H	;A(3) 3*PI/7 1.346396852
	DW	0C83H,7F33H	;B(3) TAN(A(3)) 4.381286272

FATPL:	DB	-1		;ATAN POLY TABLE
	DW	0AA7FH,2DAAH	;Q1  -.333329573
	DW	4C7EH,0B36EH	;Q2  .199641035
	DW	867EH,4FF1H	;Q3  -.131779888
	DB	0		;TERMINATOR


;****** END - FATAN ******
	endif			;end of NOT CAMAC contditional


	if	false
;8080 FLOATING POINT INVERSE SIN/COS - FASIN, FACOS
;   THESE ROUTINES CALCULATE THE INVERSE
;SINE AND COSINE OF A NUMBER IN THE FLOATING
;POINT ACCUMULATOR.
;   THE RESULTANT ANGLE IS IN RADIANS.
;
;	ASIN(FAC) -> FAC
;	ACOS(FAC) -> FAC
;
;METHOD
;	  THE FOLLOWING IDENTITIES ARE USED
;
;	ASIN(X) = ATAN(X/SQR(1-X^2))
;	ACOS(X) = ATAN(SQR(1-X^2)/X)
;AND
;	ASIN(-X) = -ASIN(X)
;	ACOS(-X) = ACOS(X) + PI

FASIN:	CALL FTEST	;RESTORE
	CPI 129
	JC FAS4		;ABS(X) < 1
	JZ FAS1		;ABS(X) MAY = 1

FAS0:	CALL FTEST
	JMP FCERN
FAS1:	MOV E,A		;SAVE EXP
	MOV A,B		;CHECK
	ANI 127		;FOR
	JZ FAS3		; UNITY
FAS2:	MOV A,E		;RESTORE EXP
	JMP FAS0	;ERROR ABS(X) > 1
FAS3:	ADD C
	JNZ FAS2	;> 1
	ADD D		;CHECK M3
	JNZ FAS2	;> 1
	MOV A,B		;X = +- 1
FSETP:	PUSH PSW	;SAVE SIGN
	LXI H,FHPI
	CALL FLOAD	;GET PI/2
	MOV E,A		;SAVE EXP
	POP PSW		;GET SIGN
	JMP FSC8	;EXIT AS FOR SIN AND COS

FAS4:	CALL FR1X2	;SQR(1-X^2)
	LXI H,FASCT
	CALL FSTOR	;SAVE
	LXI H,FASCX
	CALL FLOAD	;GET X
	LXI H,FASCT
	CALL FDIV	;X/SQR(1-X^2)
	JMP FATAN	;ATAN(X/SQR(1-X^2))


FACOS:	CALL FTEST	;RESTORE
	ANA A		;SET FLAGS
	JZ FSETP	;X=0, SET PI/2
	CPI 129
	JC FAC1		;ABS(X) < 1 CONTINUE
	MOV E,A		;SAVE EXP
FAC0:	MOV A,E		;RESTORE EXPONENT
	STC		;SET ERROR FLAG
	JNZ FCERN	;AND RET IF ABS(X) > 1
	MOV A,B		;CHECK
	ANI 127		;FOR
	JNZ FAC0	; X
	ADD C		;  =
	JNZ FAC0	;   1
	ADD D
	JNZ FAC0	;> 1
	JMP FZRO	;X=1 SO RETURN 0
FAC1:	CALL FR1X2	;SQR(1-X^2)
	LXI H,FASCX
	CALL FDIV	;SQR(1-X^2)/X
	CALL FATAN	;ATAN(SQR(-1X^2))
	RP		;LEAVE IF +
	LXI H,FPI
	JMP FADD	;ADD PI

;ROUTINE TO CALCULATE SQR(1-X^2)

FR1X2:	LXI H,FASCX
	CALL FSTOR	;SAVE X
	LXI H,FASCX
	CALL FMUL	;X^2
	CALL FCHS	;-X^2
	LXI H,FPONE
	CALL FADD	;1-X^2
	JMP FSQR	;SQR(1-X^2)


;****** END - FASIN, FACOS ******
	endif


	endif		;end of FLOAT conditional

;****** END - XMATH PACKAGE ******

;end of XMATH
	PAGE
;RAM 05/20/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;RAM storage for non-ROM versions

	if	not rom
$-macro				;suppress DZ macro expansion on printouts

patch:	dz	30		;patch area

	if	asm80
	dseg			;relocatable data segment
	endif

ramorg:				;first RAM storage location

;first the version-dependent RAM locations

	if	nonst and not camac
iobyt:	dz	1		;I/O byte in first RAM location
cstat:	jmp	jmpta+60	;console status impure code
	if	not compl
headr:	db	stbyt,tybyt, '         XYB'
	endif
	endif

	if	cpm		;impure code section for console & LST i/o
cstat:	jmp	0		;console status
conin:	jmp	0		;console in
cnout:	jmp	0		;console out
lout:	jmp	0		;list out
	if	amd
pout:	jmp	0		;punch out
rdrin:	jmp	0		;reader in
headr:	db	stbyt, tybyt, '        XYB'	;serial SAVE/LOAD header
	endif
sptr:	dz	2		;dump buffer pointer
	if	sdisk
	if	bendx
bfilp:	dz	1
bfblk:	dz	9		;Bendix SDISK file block
	else			;CP/M SDISK
files:	db	3
filep:	dz	2
filet:	dz	2
	endif
	endif
	if	debug
amode:	dz	1		;1 if dumping to disk, 0 otherwise
	endif
	endif			;end of CPM conditional

	if	isis2
fname:	dz	16		;file name

opblk:	dw	aft		;open parameter block
	dw	fname		;file name address
	dz	2		;access mode
	dz	2		;no line editing
	dw	estat		;error status

wrblk:	dz	2		;write parameter block aft
	dz	2		;address of buffer
	dz	2		;count
	dw	estat		;error status

rdblk:	dz	2		;read parameter block aft
	dz	2		;address
	dz	2		;count
	dw	temp		;actual
	dw	estat		;error status

clblk:				;close parameter block
aft:	dz	2		;aft
exblk:	dw	estat		;exit parameter block

estat:	dz	2		;error status
	dw	estat		;error status addr for error param block
temp2:	dz	1		;one-byte buffer for ASCII save and load chars
	endif			;end of ISIS2 conditional

;RAM locations which must have nonzero values in ROM version
	if	not compl
dmodc:	jmp	init		;^C branch point, changed to JMP DMOD0
	endif
rport:	in	0		;impure code for port reading
	ret
wport:	out	0		;impure code for port writing
	ret
	if	romsq
sourc:	dw	srcad		;source text starting address in ROMSQ version
	endif
	if	float and (not f9511) and (not fpbcd)
;IMPURE MULTIPLY SUBROUTINE
MULX4:	ADI	0		;ADD OPERAND 3RD FRACTION
	MOV	E,A		;4TH PARTIAL PRODUCT
	MOV	A,D		;3RD PARTIAL PRODUCT
	ACI	0		;ADD OPERAND 2ND FRACTION
	MOV	D,A		;3RD PARTIAL PRODUCT
	MOV	A,C		;2ND PARTIAL PRODUCT
	ACI	0		;ADD OPERAND 1ST FRACTION
	RET			;TO ROM CODE
;IMPURE DIVIDE SUBROUTINE
DIVX5:	SUI	0		;SUBTRACT DIVISOR 4TH FRACTION
	MOV	A,L		;REMAINDER 3RD FRACTION
	SBI	0		;SUBTRACT DIVISOR 3RD FRACTION
	MOV	L,A		;REMAINDER 3RD FRACTION
	MOV	A,H		;REMAINDER 2ND FRACTION
	SBI	0		;SUBTRACT DIVISOR 2ND FRACTION
	MOV	H,A		;REMAINDER 2ND FRACTION
	MOV	A,E		;REMAINDER 1ST FRACTION
	SBI	0		;SUBTRACT DIVISOR 1ST FRACTION
	MOV	E,A		;REMAINDER 1ST FRACTION
	MVI	A,0		;REMAINDER 4TH FRACTION
	RET 			;TO ROM CODE
DIVX6:	ADI	0		;ADD DIVISOR 3RD FRACTION
	MOV	L,A		;REMAINDER 3RD FRACTION
	MOV	A,H		;REMAINDER 2ND FRACTION
	ACI	0		;ADD DIVISOR 2ND FRACTION
	MOV	H,A		;REMAINDER 2ND FRACTION
	MOV	A,E		;REMAINDER 1ST FRACTION
	ACI	0		;ADD DIVISOR 1ST FRACTION
	MOV	E,A		;REMAINDER 1ST FRACTION
	MVI	A,0		;REMAINDER 4TH FRACTION
	RET			;TO ROM CODE
	endif

	if	camac		;impure routines for CAMAC command execution
;CSETC changes the IN or OUT ports according to the desired crate
camwr:	mov	a,m		;write data
	out	0
	inx	h
	mov	a,m
	out	1
	inx	h
	mov	a,m
	out	2
	ret
camfn:	mov	a,m		;perform function
	out	3
	inx	h
	mov	a,m
	out	4
	inx	h
	mov	a,m
	out	5
	ret
camrd:	in	6		;read data
	mov	m,a
	inx	h
	in	7
	mov	m,a
	inx	h
	in	8
	mov	m,a
	ret
timex:	db	255 and -120, 255 and -60, 255 and -24
	else			;NOT CAMAC
	if	realt		;SBC 80/20 version uses real time clock
timex:	db	255 and -20, 255 and -60, 255 and -60, 255 and -24
	else			;valiues for software real time clock
timex:	dw	timem		;TIME count for minutes, complemented
	dw	timen		;TIME count for seconds, complemented
	db	timeh		;TIME count for hundredths
	endif
	endif

colum:	dz	1		;current column position on printline
width:	db	255		;terminal width -- NB must follow COLUM

;locations which must be zeroed in ROM version
lnnum:	dz	2		;line number (address) of current line
	if	not compl
	if	editc
errln:	dz	2		;line number (address) of most recent error
	endif
trap:	dz	1		;trap mode -- 0 trapon, m1 trapoff
trace:	dz	1		;trace mode -- 0 traceoff, m1 traceon
inlhs:	dz	1		;m1 if getting LHS -- NB must follow TRACE
vtrac:	dz	1		;minus iff tracing -- NB must follow INLHS
	endif
	if	camac
cmdat:	dz	3		;data D1, D2, D3 for read or write
cmcra:	dz	1		;crate C -- must follow D3
cmslo:	dz	1		;slot N -- must follow C
	dz	1		;subaddress A -- must follow N
cmfnc:	dz	1		;fn code F -- must follow A
scrte:	dz	1		;serial driver crate -- must follow F
sdrvr:	dz	1		;serial driver slot -- must follow serial crate
camqr:	dz	1		;Q response
camxr:	dz	3		;X, O, I responses -- must follow CAMQR
	if	c3908
cmblk:	dz	7		;BKSET data
	endif
cvcnt:	dz	2		;camvar component count
cvloc:	dz	2		;camvar value location
xnabl:	dz	1		;X response check enable
imode:	dz	1		;interrupt mode, 0 if EI, 255 if DI
fmode:	dz	1
	endif
	if	not wild
lmode:	dz	1		;LST mode
	endif
	if	packi
paonl:	dz	1		;ONLINE
	endif
omode:	dz	1		;output suppress mode
	if	not wild
inttc:	dz	1		;number of entries currently in interrupt table
intad:	dz	inttl		;interrupt table
randx:	dz	2		;pseudorandom number generator seed
nulls:	dz	1		;number of nulls printed after crlf
	endif			;end of NOT WILD conditional
gchar:	dz	1		;last console char read for GET
	if	strng		;string ram
stemp:	dz	1		;number of string temporaries in use

;locations which may come up randomly in ROM version
	dz	2*stmax		;string temporaries, 2 bytes each
strt:	dz	2		;top of string space address
strpt:	dz	2
strp2:	dz	2		;string space pointers
gcmin:	dz	2
gclst:	dz	2
gcloc:	dz	2		;garbage collection temp storage
	endif			;end of string ram
	if	editc and (not compl)
	if	strng
rnnew	equ	gcmin
rninc	equ	gclst
rnold	equ	gcloc		;RENUM and garbage collection share temps
	else
rnnew:	dz	2
rninc:	dz	2
rnold:	dz	2
	endif
	endif
	if	camac and nonst and (not rtpak)	;Northstar DOS RAM
dunit:	dz	1		;disk unit
fname:	dz	9		;file name
drbuf:	dz	256		;buffer
nxtad:	dz	2
nxtbl:	dz	2
lstbl:	dz	2
lstfl:	dz	2
	endif
	if	packi
paign:	dz	1		;IGNORE byte
pacnt:	dz	1		;count of chars in current line
pamod:	dz	1		;MODE byte -- 0 waiting, 1 storing, 2 insignif
pafno:	dz	1		;current/next field #
pafbp:	dz	2		;field buffer pointer
pafld:	dz	pafct		;FIELD data buffer
padat:	dz	padct		;data buffer
padp0:	dz	2		;FLAG of next record pointer
padp1:	dz	2		;first data pointer
padp2:	dz	2		;last+1 data pointer
padp3:	dz	2		;FLAG of most recent record pointer
	endif
	if	float
argad:	dz	2		;address of arg for amb op retry
	if	f9511
fptmp:	dz	3		;temporaries for 9511 FIN and FOUT
	else			;NOT F9511
	if	fpbcd		;BCD floating point RAM
FACC 	DZ 	6  		;FLOATING POINT ACCUMULATOR
ACALT 	DZ 	6  		;ALTERNATE ACCUMULATOR
HOLD1 	DZ 	6  		;FLOATING POINT MULTIPLY BUFFERS
HOLD2 	DZ 	6
HOLD3 	DZ 	6
HOLD4 	DZ 	6
FTEMP	DZ 	6  		;FLOATING POINT WORK AREAS
FTMP1 	DZ 	6
FTMP2 	DZ 	6
FTMP3 	DZ 	6
FTMP4 	DZ 	6
EXP1 	DZ 	1 		;EXPONENT WORK AREAS
EXP2 	DZ 	1  
EXP3 	DZ 	1
EXP4 	DZ 	1
SIGN 	DZ 	1 		;SIGN WORK AREAS
SIGN1 	DZ 	1
FLEN 	DZ 	1 		;LENGTH OF SUBTRACT FIELD
RSLT 	DZ 	2 		;ADDRESS STORAGE
	else
;RAM LOCATIONS USED BY THE BINARY FLOATING POINT SYSTEM
PREX:	dz	1		;PREVIOUS EXPONENT
ACCE:	dz	1		;ACC EXPONENT
ACCS:	dz	1		;ACC SIGN
ACC1:	dz	1		;ACC 1ST FRACTION
ACC2:	dz	1		;ACC 2ND FRACTION
ACC3:	dz	1		;ACC 3RD FRACTION
SFLAG:	dz	1		;SUBTRACTION FLAG
ADRL:	dz	2		;ADDR OF CHARACTER STRING WORD ADDRESS
TMP1:	dz	1		;TEMPORARY STORAGE
TMP2:	dz	1		;    "        "
TMP3:	dz	1		;    "        "
VALE:	dz	4		;VALUE EXPONENT
TMP4:	dz	1		;TEMPORARY STORAGE
;and then the XMATH RAM
SIG:	dz	1		;SIG INDEX FROM CALLER
FPTXN:	dz	8		;RUNNING POWER AND POWER MULTIPLIER
FPSUM:	dz	4		;RUNNING TERM SUM
VECT:	dz	2		;ADDRESS OF CURRENT TERM
FINSN:	dz	1		;B7 = INPUT SIGN B6-B0 = 0
FINFP:	dz	4		;FRACTIONAL PART
FPTXX:	dz	4
FPTF:	dz	4
FPTP:	dz	4
FPTX:	dz	4		;TEMP STORAGE
FPTN:	dz	1		;N
FPSGN:	dz	1		;INPUT SIGN
FPTZ0:	dz	4		;INITIAL POWER OF Z
FPTZ1:	dz	4		;TERM POWER MULTIPLIER (Z1=Z)
FLNZI:	dz	4		;INITIAL POWER
FLNZ2:	dz	4		;POWER TERM MULTIPLIER
FLNK:	dz	1		;K,J
FLNF:	dz	4		;F,G,X,G(Z) -- NB must follow FLNK
FSCFX:	dz	1		;0AFH FOR SIN, 0 FOR COS
FSCSG:	dz	1		;0 IF INPUT +, 80H IF -
FSCZ:	dz	4		;Z
FSCZ2:	dz	4		;Z^2
FTMPA:	dz	4		;TEMP STORAGE
FTMPB:	dz	4		; ----"-----
	if	not camac	;no ATN in CAMAC versions
FATZX:	dz	4		;Z,X
FATXM:	dz	4		;INITIAL POWER OF X
FATX2:	dz	4		;TERM POWER MULIPLIER
FATSG:	dz	1		;INPUT SIGN
FATK:	dz	1		;K
	endif			;end of NOT CAMAC conditional
	endif			;end of NOT FPBCD conditional
	endif			;end of NOT F9511 conditional
	endif			;end of FLOAT conditional

;stack space
	if	asm80
	stkln	stakl
	else
	dz	stakl
stack:	ds	0
	endif

varty:	dz	1		;type of last var name scanned
	if	strng or float	;default variable type buffer
tybuf:	dz	26
	endif
temp:	dz	2		;all-purpose temp storage
txtp2:	dz	2		;secondary textp for READ, INPUT
savtp:	dz	2		;textp save
lhsty:	dz	1		;lhs variable type
lhsad:	dz	2		;lhs variable address
cstkp:	dz	2		;control stack pointer
estkp:	dz	2		;expression stack pointer
symta:	dz	2		;starting address of symbol table
bufad:	dz	maxnl+maxnl	;buffer for number-string conversion, var names
	db	':'		;colon precedes tokenized input buffer
tlnad:	if	editc		;tokenization buffer seperate in EDITC version
	db	cr
	dz	nlmax+2		;NLMAX chars + space + cr  + eof
	endif
nlnad:	db	cr		;first byte of input buffer
	if	(sdisk or (camac and nonst)) and (not rtpak)
	dz	255		;input buffer is used by LINPUT too
	else
	dz	nlmax+2		;NLMAX chars + space + cr + eof
	endif
	if	not compl
lnlen:	dz	1		;length of current input line
cstkd:	dz	1		;control stack direct count
tracl:	dz	1		;line traced -- 0 no, m1 yes
	endif
textp:	dz	2		;current text line pointer
rdptr:	dz	2		;READ text pointer
eofad:	dz	2		;ending address of user source text
memt:	dz	2		;top of memory address
	db	cr		;cr precedes source text
srcad:	dz	1		;source text

$*macro				;resume macro expansion hex printing
	endif			;end of NOT ROM conditional

;end of RAM
	page
;ROM 12/12/80
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980 by Mark Williams Company, Chicago
;RAM EQUates for ROM version

	if	rom

	if	compl and not rtpak
patch:	dz	30		;compiler patch area
	endif

;values  for block move into nonzero RAM locations
nzram:	if	camac		;impure routines for CAMAC command execution
	mov	a,m		;CAMWR, write data
	out	0
	inx	h
	mov	a,m
	out	1
	inx	h
	mov	a,m
	out	2
	ret
	mov	a,m		;CAMFN, perform function
	out	3
	inx	h
	mov	a,m
	out	4
	inx	h
	mov	a,m
	out	5
	ret
	in	6		;CAMRD, read data
	mov	m,a
	inx	h
	in	7
	mov	m,a
	inx	h
	in	8
	mov	m,a
	ret
	db	255 and -120, 255 and -60, 255 and -24	;TIMEX
	else
	if	not wild
	db	0		;IOBYT in first RAM location
	jmp	jmpta+60	;CSTAT, console status
	endif
	if	realt		;TIMEX, time counts
	db	255 and -20, 255 and -60, 255 and -60, 255 and -24
	else
	dw	timem
	dw	timen
	db	timeh
	endif
	endif
	in	0		;RPORT
	ret
	out	0		;WPORT
	ret
	if	bendx and sdisk
	jmp	0		;BRCHA
	endif
	if	romsq
	dw	srcad		;SOURC
	endif
	if	float and (not f9511) and (not fpbcd)
;impure code for FMULT and FDIV routines
;MULX4, RAM MULTIPLY SUBROUTINE
	ADI	0		;ADD OPERAND 3RD FRACTION
	MOV	E,A		;4TH PARTIAL PRODUCT
	MOV	A,D		;3RD PARTIAL PRODUCT
	ACI	0		;ADD OPERAND 2ND FRACTION
	MOV	D,A		;3RD PARTIAL PRODUCT
	MOV	A,C		;2ND PARTIAL PRODUCT
	ACI	0		;ADD OPERAND 1ST FRACTION
	RET			;TO ROM CODE
;DIVX5, RAM DIVIDE SUBROUTINE
	SUI	0		;SUBTRACT DIVISOR 4TH FRACTION
	MOV	A,L		;REMAINDER 3RD FRACTION
	SBI	0		;SUBTRACT DIVISOR 3RD FRACTION
	MOV	L,A		;REMAINDER 3RD FRACTION
	MOV	A,H		;REMAINDER 2ND FRACTION
	SBI	0		;SUBTRACT DIVISOR 2ND FRACTION
	MOV	H,A		;REMAINDER 2ND FRACTION
	MOV	A,E		;REMAINDER 1ST FRACTION
	SBI	0		;SUBTRACT DIVISOR 1ST FRACTION
	MOV	E,A		;REMAINDER 1ST FRACTION
	MVI	A,0		;REMAINDER 4TH FRACTION
	RET 			;TO ROM CODE
;DIVX6, RAM DIVIDE SUBROUTINE
	ADI	0		;ADD DIVISOR 3RD FRACTION
	MOV	L,A		;REMAINDER 3RD FRACTION
	MOV	A,H		;REMAINDER 2ND FRACTION
	ACI	0		;ADD DIVISOR 2ND FRACTION
	MOV	H,A		;REMAINDER 2ND FRACTION
	MOV	A,E		;REMAINDER 1ST FRACTION
	ACI	0		;ADD DIVISOR 1ST FRACTION
	MOV	E,A		;REMAINDER 1ST FRACTION
	MVI	A,0		;REMAINDER 4TH FRACTION
	RET			;TO ROM CODE
	endif
	if	compl and not rtpak	;compiler values
	dw	romend		;TEXTP
	dw	romend		;RDPTR
	dw	ramend+1	;EOFAD
	db	0		;COLUM
	db	0		;WIDTH
	dw	0		;MEMT
romend:	db	cr		;cr precedes source text
srcad	equ	romend+1	;source text location
	else			;RTPAK OR NOT COMPL
	if	not rtpak
	jmp	init		;DMODC
	if	(not epstn) and (not camac) and (not bendx)	;HEADR
	if	not genmc
	db	stbyt,tybyt,'        XYB'
	else
	db	cr, lf, '''', '        .BAS', cr, lf or 80H
	endif
	endif
	endif			;end of NOT RTPAK conditional
	db	0		;COLUM
	db	255		;WIDTH
	endif
nzlen	equ	$-nzram		;number of nonzero bytes to block move


;ram location EQUates
	if	asm80
	dseg			;start of relocatable data segment
	else
savpc	equ	$		;save ROM PC location
	org	ramorg		;and reset PC to start of RAM
	endif

;first locations which must get nonzero values by block move
	if	camac
camwr	ds	12
camfn	ds	12
camrd	ds	12
timex	ds	3
	else			;not CAMAC
	if	not wild
iobyt	ds	1
cstat	ds	3
	endif
	if	realt
timex	ds	4
	else
timex	ds	5
	endif
	endif			;end of NOT CAMAC conditional
rport	ds	3
wport	ds	3
	if	bendx and sdisk
brcha	ds	3
	endif
	if	romsq
sourc	ds	2
	endif
	if	float and (not f9511) and (not fpbcd)
mulx4	ds	11
divx5	ds	17
divx6	ds	14
	endif
	if	compl and not rtpak
textp	ds	2
rdptr	ds	2
eofad	ds	2
colum	ds	1
width	ds	1
memt	ds	2
	else
	if	not rtpak
dmodc	ds	3
	if	not (epstn or camac or bendx)
headr	ds	headl
	endif
	endif
colum	ds	1
width	ds	1
	endif

;next the locations which must be zeroed
lnnum	ds	2
	if	not compl
	if	editc
errln	ds	2
	endif
trap	ds	1
trace	ds	1
inlhs	ds	1
vtrac	ds	1
	endif
	if	camac
cmdat	ds	3
cmcra	ds	1
cmslo	ds	2
cmfnc	ds	1
scrte	ds	1
sdrvr	ds	1
camqr	ds	1
camxr	ds	3
	if	c3908
cmblk	ds	7
	endif
cvcnt	ds	2
cvloc	ds	2
xnabl	ds	1
imode	ds	1
fmode	ds	1
	endif
	if	bendx and sdisk
bwtct	ds	3
breof	ds	2
bfilp	ds	1
bfblk	ds	9
	endif
	if	not wild
lmode	ds	1
	endif
	if	packi
paonl	ds	1
	endif
omode	ds	1
	if	not wild
inttc	ds	1
intad	ds	inttl
randx	ds	2
nulls	ds	1
	endif
gchar	ds	1
	if	strng
stemp	ds	1
	endif
zlen	equ	$-lnnum		;number of bytes to zero

;followed by locations which may come up randomly
	if	strng
	ds	stmax+stmax
strt	ds	2
strpt	ds	2
strp2	ds	2
gcmin	ds	2
gclst	ds	2
gcloc	ds	2
	endif
	if	editc and (not compl)
	if	strng
rnnew	equ	gcmin
rninc	equ	gclst
rnold	equ	gcloc
	else
rnnew	ds	2
rninc	ds	2
rnold	ds	2
	endif
	endif
	if	bendx
	if	sdisk
bfnam	ds	15
bfna2	ds	15
	else
bfnam	ds	11
	endif
bmeps	ds	3
	endif
	if	wild and rtpak	;WTOKE temp storage
wmemt	ds	2
weof	ds	2
wlast	ds	2
wnext	ds	2
	endif
	if	camac and nonst and (not rtpak)	;Northstar DOS
dunit	ds	1
fname	ds	9
drbuf	ds	256
nxtad	ds	2
nxtbl	ds	2
lstbl	ds	2
lstfl	ds	1
	endif
	if	packi
paign	ds	1
pacnt	ds	1
pamod	ds	1
pafno	ds	1
pafbp	ds	2
pafld	ds	pafct
padat	ds	padct
padp0	ds	2
padp1	ds	2
padp2	ds	2
padp3	ds	2
	endif
	if	float		;floating point system RAM
ARGAD	ds	2
	if	f9511
fptmp	ds	3
	else
	if	fpbcd
FACC 	DS 	6  		;FLOATING POINT ACCUMULATOR
ACALT 	DS 	6  		;ALTERNATE ACCUMULATOR
HOLD1 	DS 	6  		;FLOATING POINT MULTIPLY BUFFERS
HOLD2 	DS 	6
HOLD3 	DS 	6
HOLD4 	DS 	6
FTEMP	DS 	6  		;FLOATING POINT WORK AREAS
FTMP1 	DS 	6
FTMP2 	DS 	6
FTMP3 	DS 	6
FTMP4 	DS 	6
EXP1 	DS 	1 		;EXPONENT WORK AREAS
EXP2 	DS 	1  
EXP3 	DS 	1
EXP4 	DS 	1
SIGN 	DS 	1 		;SIGN WORK AREAS
SIGN1 	DS 	1
FLEN 	DS 	1 		;LENGTH OF SUBTRACT FIELD
RSLT 	DS 	2 		;ADDRESS STORAGE
	else
;first the FMATH RAM
PREX	ds	1
ACCE	ds	1
ACCS	ds	1
ACC1	ds	1
ACC2	ds	1
ACC3	ds	1
SFLAG	ds	1
ADRL	ds	2
TMP1	ds	1
TMP2	ds	1
TMP3	ds	1
VALE	ds	4
TMP4	ds	1
;and then the XMATH RAM
SIG	ds	1
FPTXN	ds	8
FPSUM	ds	4
VECT	ds	2
FINSN	ds	1
FINFP	ds	4
FPTXX	ds	4
FPTF	ds	4
FPTP	ds	4
FPTX	ds	4
FPTN	ds	1
FPSGN	ds	1
FPTZ0	ds	4
FPTZ1	ds	4
FLNZI	ds	4
FLNZ2	ds	4
FLNK	ds	1
FLNF	ds	4
FSCFX	ds	1
FSCSG	ds	1
FSCZ	ds	4
FSCZ2	ds	4
FTMPA	ds	4
FTMPB	ds	4
	if	not camac
FATZX	ds	4
FATXM	ds	4
FATX2	ds	4
FATSG	ds	1
FATK	ds	1
	endif
	endif
	endif
	endif

	if	asm80
	stkln	stakl
	else
	ds	stakl
stack	ds	0
	endif

varty	ds	1
	if	strng or float
tybuf	ds	26
	endif
temp	ds	2
txtp2	ds	2
savtp	ds	2
lhsty	ds	1
lhsad	ds	2
cstkp	ds	2
estkp	ds	2
symta	ds	2
bufad	ds	maxnl+maxnl
	ds	1
	if	editc
tlnad	ds	nlmax+3
	else
tlnad	ds	0
	endif
	if	(sdisk or (camac and nonst)) and (not rtpak)
nlnad	ds	256
	else
nlnad	ds	nlmax+3
	endif
	if	compl and not rtpak
ramend	equ	$
	else
	if	wild or not rtpak
lnlen	ds	1
	endif
	if	not rtpak
cstkd	ds	1
tracl	ds	1
	endif
textp	ds	2
rdptr	ds	2
eofad	ds	2
memt	ds	2
	ds	1
srcad	equ	$
	endif

	if	not asm80
	org	savpc		;restore previous ROM PC
	endif

	endif			;end of ROM conditional

;end of ROM
	page
;INITIAL 05/20/81
;XYBASIC Interpreter Source Module
;Copyright (C) 1978, 1979, 1980, 1981 by Mark Williams Company, Chicago
;initialization dialog


	if	asm80
	cseg			;relocatable code segment
	endif

;XYBASIC performs a JMP INIT from ROMORG on initial entry.
;The user program source and working space overlay the code
;in this module after initialization in RAM versions.

init:	lxi	sp,stack	;initialize SP

;save A on entry in Wild version
	if	wild
	push	psw
	endif

;set up RAM locations if ROM version
	if	rom
	lxi	b,nzram		;source address to BC
	lxi	d,nzlen		;byte count to DE
	lxi	h,ramorg	;destination address to HL
	call	moved		;block move nonzero bytes to RAM
	lxi	b,zlen		;0 to B, bytes to zero to C
	call	fillm		;fill RAM locations with 0
	mvi	a,':'
	lxi	h,tlnad-1
	mov	m,a		;colon precedes tokenized input buffer
	inx	h
	mvi	a,cr
	mov	m,a		;cr to tokenized input buffer
	if	editc
	sta	nlnad		;and cr to input buffer in EDITC version
	endif
	sta	srcad-1		;cr precedes source text
	endif			;end of ROM conditional

;initialize runtime package pointers
	if	rtpak		;initialize souce text pointers
	lxi	h,srcad
	mvi	m,0		;source text empty
	shld	eofad		;initialize EOFAD
	call	cspst		;reset CSTACK and ESTACK
	endif			;end of RTPAK conditional

;set up i/o jmp vector if CP/M version
	if	cpm
	lhld	cpmba+1		;3E03H + base
	lxi	d,3
	dad	d		;3E06H + base
	shld	cstat+1		;store to allow console status not via BDOS
	dad	d		;3E09H + base
	shld	conin+1		;store to allow console reading without echo
	dad	d		;3E0CH + base
	shld	cnout+1		;store to allow console writing
	dad	d		;3E0FH + base
	shld	lout+1		;store to allow list device writing
	if	amd		;also define PUN and RDR in AMD version
	dad	d
	shld	pout+1		;3E12H + base is PUN out
	dad	d
	shld	rdrin+1		;3E15H + base is RDR in
	endif			;end of AMD conditional
	endif			;end of CPM conditional

;initialize the real time clock for REALT version
	if	realt
	mvi	a,0C3H
	sta	10H		;JMP to RST 2
	lxi	h,clock
	shld	11H		;so RST 2 does a JMP CLOCK
	ei			;enable interrupts so clock runs
	endif

;initialize the real time clock for CAMAC version
	if	camac
	lxi	h,101H
	shld	scrte		;set serial crate and driver to 1
	if	c3885 and not debug	;clock initialization for 3885
;initialize RST 7.5 at 3CH to JMP CLOCK
	mvi	a,0C3H
	sta	3CH		;JMP to 3CH
	lxi	h,clock
	shld	3DH		;CLOCK to 3DH
;initialize counter mode and set interrupt mask
	mvi	a,74H
	out	0BH		;set counter 1 to mode 2
	lxi	b,800
	mov	a,c
	out	9
	mov	a,b
	out	9		;initialize counter
	mvi	a,0BH
	sim			;set interrupt mask
	ei
	endif
;clock initialization in old NONST version
	if	nonst and (not rtpak) and false
	mvi	a,0C3H
	sta	1008H		;JMP to 1008H
	lxi	h,clock
	shld	1009H		;CLOCK to 1009H so interrupts will JMP CLOCK
	mvi	a,255
	out	4
	mvi	a,0A6H
	out	11
	out	19
	mvi	a,18H
	out	2
	ei
	endif
	endif

;Packard Instruments version initialization
	if	packi and not debug
	mvi	a,5
	sta	nulls		;initialize NULLS for TI Silent 700
	rim			;read interrupt mask
	ani	4
	ori	8		;set
	sim			;reset interrupt mask
	ei			;enable interrupts
	endif

;print initial message
	if	not (wild and rtpak)
	call	prntm
	if	epstn
	db	0FH, 13H
	endif
	db	cr, lf
	if	compl
	if	rtpak
	db	'RUNTIME '
	else
	db	'COMPILED '
	endif
	endif
	if	(not strng) and (not float)
	db	'INTEGER '
	endif
	if	amd
	db	'AMD '
	endif
	if	bendx
	db	'BENDIX '
	endif
	if	packi
	db	'PACKARD INSTRUMENTS '
	endif
	if	wild
	db	'WILD '
	endif
	db	'XYBASIC '
	if	epstn
	db	12H
	endif
	if	cpm and (not camac)
	db	'CP/M '
	endif
	if	isis2
	db	'ISIS-II '
	endif
	if	sdisk and (not camac)
	db	'DISK '
	endif
	if	editc
	db	'EDIT '
	endif
	db	'REV ', versn/10000+'0', '.'
	if	((versn/1000) mod 10) ne 0
	db	(versn/1000) mod 10 + '0'
	endif
	db	(versn/100) mod 10 +'0'
	if	(versn mod 100) ne 0
	db	'.'
	if	((versn/10) mod 10) ne 0
	db	(versn/10) mod 10 + '0'
	endif
	db	versn mod 10+'0'
	endif
	if	debug
	db	'D'
	endif
	db	cr, lf or 80H

;print copyright message unless CAMAC version
	if	not camac
	lxi	h,mwcma
	call	prtst		;print copyright message
	endif
	endif			;end of NOT (WILD AND RTPAK) conditional

;if compiler, find top of memory and execute program directly
	if	compl and not rtpak
	lhld	memt		;passed memt value to HL
	mov	a,h
	ora	l		;Zero set iff default to found value
	cz	eomem		;take default value
	if	strng
	shld	strt		;store top of string space
	dcr	h		;leave 100H bytes of string space
	endif
	shld	memt
	mvi	m,0		;initialize symbol table
	call	clea1		;initialize
	mvi	m,0		;initialize cstack
	jmp	nexts		;and execute the program
	else			;(not COMPL) or RTPAK

;check default width if ROMSQ or CP/M version
	if	romsq and not wild
	lda	dfltw		;default width to A
	ora	a
	jnz	init2		;take specified width
	endif
	if	cpm and not compl
	lda	fcbad+1
	cpi	' '		;check if filename specified on command line
	mvi	a,72
	jnz	init2		;yes, default width to 72
	endif

;prompt for width
	if	wild and rtpak
	mvi	a,255
	sta	width
	else
init1:	lxi	d,1		;terminal width min  = 1
	call	initp		;print message, get width
	db	'WIDTH?', ' ' or 80H
	jc	init1		;illegal value
	mvi	a,nlmax		;default width to A
	jz	init2		;take default value
	mov	a,h
	ora	a
	jnz	init1		;too big -- try again
	mov	a,l
init2:	sta	width
	endif			;end of NOT (WILD AND RTPAK) conditional

init3:	if	rom		;min memtop value to DE
	lxi	d,srcad+dbyts
	else
	lxi	d,eoram+dbyts
	endif

;check default memtop value if ROMSQ or CP/M version
	if	romsq and not wild
	lhld	dfltm		;default end of memory to HL
	call	cmdhu
	jc	init4		;check specified value
	mov	a,h
	ora	l		;Zero set iff default to max
	jz	init5		;take max value
	endif
	if	cpm and not compl
	lda	fcbad+1
	cpi	' '
	jnz	init5		;take max
	endif

;prompt for end of memory
	if	wild and rtpak
	lhld	weom
	call	cmdhu		;compare to least
	jc	init6		;take given value
	else
	call	initp		;print message, get mem top
	db	'END OF MEMORY?', ' ' or 80H
	jc	init3		;bad value, retry
	jz	init5		;take default value
init4:	call	rtest		;test if specified location is RAM
	jnz	init3		;not RAM, retry
	jmp	init6		;else take specified location
	endif			;end of NOT (WILD AND RTPAK) conditional
init5:	call	eomem		;find default end of memory

;initialize RAM pointers
init6:	xra	a
	if	sdisk and cpm and (not rtpak)
	shld	filet		;store end of file space
	mov	m,a		;initialize first default file area
	lxi	d,-filen
	dad	d
	mov	m,a		;initialize second default file area
	dad	d
	endif
	if	strng
	shld	strt		;store top of string space
	dcr	h		;leave 100H bytes of string space
	endif
	mov	m,a		;initialize symbol table
	shld	memt
	if	rtpak
	call	clea1		;initialize
	else			;NOT RTPAK
	call	new		;initialize source, interrupt
	if	bendx and not debug
	lxi	h,dmod2
	shld	brsad		;reset reentry address to DMODE
	endif
	lxi	h,stop
	shld	dmodc+1		;change ^C branch from JMP INIT to JMP STOP
	endif
	if	not rom
	lxi	h,dmod2
	shld	start+1		;change JMP INIT to JMP DMOD2 to allow reentry
	endif

;load program if specified on CP/M command line
	if	cpm and not compl
	lda	fcbad+1
	cpi	' '		;check if filename specified on command line
	jz	init7		;no
	lxi	h,fcbft
	mvi	m,'X'
	inx	h
	mvi	m,'Y'
	inx	h
	mvi	m,'B'		;reset filetype to .XYB
	call	loadz		;reset LNNUM and TEXTP in case not found
	call	cdkop		;open the file
	lxi	h,nexts
	push	h		;push normal statement return for RUN
	mvi	b,1		;set B for ,R to RUN after loading
	jmp	lod0b		;and load it
	endif

;print BYTES FREE message
	if	not (wild and rtpak)
init7:	call	fre		;free memory bytes to BC
	call	wrtbu		;write (BC) unsigned
	call	prntm		;print BYTES FREE message
	db	' BYTES FREE', cr, lf or 80H
	endif			;end of NOT (WILD AND RTPAK) conditional

;Wild version default program execution
	if	wild
	pop	psw		;recover initial A value
	if	not rtpak
	ora	a
	jz	dmode		;0 gets direct mode in development version
	endif
	mov	c,a
	mvi	b,0		;desired program # to BC
	call	execw		;find selected program
	lhld	sourc
	dcx	h
	shld	textp		;reset TEXTP to selected program
	jmp	nexts		;and execute it
	else			;NOT WILD

;begin at direct mode or execute specified program
	if	romsq
	lhld	dflta		;default prog address to HL
	if	rtpak		;runtime version
	mov	a,m		;fetch first char
	cpi	cr
	jz	init8		;first char is cr, continue as below
	call	prntm		;else print error message
	db	'PROGRAM NOT FOUND', cr, lf or 80H
	jmp	boot		;and exit
init8:	shld	rdptr		;initialize the DATA pointer
	inx	h
	shld	sourc		;set SOURC to program address
	else
	mov	a,h
	ora	l
	jz	dmode		;none specified
	call	exec0		;reset SOURC to specified address
	lhld	sourc
	endif
	dcx	h
	shld	textp		;reset TEXTP to program
	jmp	nexts		;and execute from there
	else			;not ROMSQ
	jmp	dmode
	endif
	endif			;end of NOT WILD conditional

	if	not (wild and rtpak)
;INITP prompts the user and gets a value for WIDTH or END OF MEMORY.
;Call:	DE			min value -1
;	Location after CALL	message to print
;Retn:	Carry			set iff value bad
;	Zero, no Carry		if <cr> typed
;	no Zero, no Carry	value ok,  returned in HL
initp:	pop	h		;message address to HL
	call	prtst		;print message
	push	h		;save return addr
	push	d		;save min value -1
	call	gtlin		;get a line from console
	call	gtdec		;and get decimal value to DE
	xchg			;value typed to HL
	pop	d		;min -1 to DE
	jc	inip1		;no digit -- check for <cr>
	stc
	rz			;too big, return Carry
	call	cmdhu
	cmc			;Carry set iff min >= value typed
	ret
inip1:	cpi	cr
	rz			;<cr>, return Zero and no Carry
	if	camac
	lxi	h,mwcma
	call	prtst		;print copyright message in CAMAC version
	endif
	stc
	ret			;return Carry
	endif			;end of NOT (WILD AND RTPAK) conditional

	endif			;end of NOT COMPL OR RTPAK conditional

;EOMEM gets default end of RAM to HL.
eomem:	if	cpm		;fetch end of memory from BDOS
	lhld	bdos+1
	dcx	h
	endif
	if	isis2
	call	mechk		;top of memory to AB
	mov	h,b
	mov	l,a		;then to HL
	endif
	if	nonst		;search for end of RAM
	if	compl		;first loc to test to HL
	lhld	eofad
	else
	if	rom
	lxi	h,srcad
	else
	lxi	h,eoram
	endif
	endif
eome1:	inx	h
	mov	a,h
	ora	l
	jz	eome2		;quit if at 64K boundary
	call	rtest		;check if RAM at location
	jz	eome1		;RAM, try next
eome2:	dcx	h		;top of memory to HL
	endif
eoram:	ret

	if	rom and (rtpak or not compl)
patch	equ	eoram+1		;patch area for ROM versions
	endif

	end			;end of XYBASIC source

;end of INITIAL
