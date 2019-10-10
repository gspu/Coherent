;------------------------------------------------------------------------------
;	Talking BIOS device driver for the AT&T PC6300.
;	Copyright (C) Karl Dahlke 1987
;	This software may be freely used and distributed
;	for any non-profit purpose.
;------------------------------------------------------------------------------

;	talkbios.asm: assembly base and support for talking BIOS routines


WDFIXLEN	equ	4096 ; length of word correction table
KBSIZ	equ 120 ; length  of type ahead keyboard buffer
ADDR	equ offset DGROUP

;	These routines are bundled into a pseudo device driver,
;	which is installed at init time.
;	The device cannot be meaningfully read or written.

;	Compatible with ansi.sys, ctrm, and most other applications.

;	Device Driver skeleton taken from public domain,
;	written by Frank Whaley.

;	The real time interrupt switches stacks,
;	since there are a lot of function calls in it,
;	and it holds the CPU for quite a while.
;	The other interrupts and the device driver
;	use the existing stacks.  In general, there is very little
;	documentation regarding stacks.  How much room is required
;	for DOS interrupts, worst case?  How much room is available on system
;	stacks (e.g. when device drivers get control)?
;	It makes it difficult to write robust programs.
;	Well, MS-DOS drivers seem quite unconcerned about these things,  so,
;	For now, we just press on.
;	The real time interrupt stack tromps right over the
;	initialization code, saving memory.

;	here come the segments
;	segment structure is compatible with microsoft C, small memory model
_TEXT	segment  byte public 'CODE'
_TEXT	ends
;	empty segments, but C uses them, so declare them
CONST	segment  word public 'CONST'
CONST	ends
_BSS	segment  word public 'BSS'
_BSS	ends
_DATA	segment  word public 'DATA'
_DATA	ends
_EDATA	segment  para public 'EDATA'
_EDATA	ends

DGROUP	group CONST, _BSS, _DATA, _EDATA
	assume  cs: _TEXT, ds: DGROUP


;	make microsoft C/link phase happy (see crt0.asm)
	public	__acrtused	
	__acrtused = 9876h	; funny value not easily matched in SYMDEB


_TEXT	segment

	extrn _sdoutchar:near, _sdinkey:near, _sdioctl:near
	extrn   _sdload:near, _sdtime:near, _dosline:near
	public myds

	org     0 ; Device drivers start at 0

HDR     proc    far
;	Device Header
	dd      -1 ; -> next device
	dw	0c000h ; attributes: character, ioctl
	DW      strategy ; -> device strategy
	DW      interrupt ; -> device interrupt
	db  "SPEAK$  " ; talking BIOS routines

;	the following variables are  accessed through cs.
rh1ptr	label word ; to access by words
rhptr   dd      0 ; -> Request Header
orig_rti dw 0,0 ; original real time interrupt vector
orig_kbi dw 0,0 ; original keyboard interrupt vector
orig_bkb dw 0,0 ; original bios keyboard interrupt vector
orig_bsc dw 0,0 ; original bios screen interrupt vector
;	save ss and sp when switching to real time interrupt stack
save_ss	dw 0
save_sp	dw 0
myds	dw 0 ;	ds segment when running this system
nestrti	db 1 ; check for nested real time interrupts

;	functions for device driver commands
functab	label word
	DW  init
	dw badf
	dw badf
	DW  ioctl
	dw nulf, nulf, nulf, nulf, nulf, nulf, nulf, nulf



strategy:
;	squirrel away pointer to the msdos request
;	I don't understand why dos doesn't just call the interrupt routine directly,
;	and bypass this intermediate, seemingly unnecessary strategy routine.
	mov    rh1ptr,BX ; save request header ptr
	mov    rh1ptr + 2,ES
	ret

interrupt:
;	device driver interrupt routine,  *not* a true interrupt routine.
;	Perform the action requested by dos,
;	request structure is pointed to by rhptr.
;	We save all registers, gather relevant information
;	from the request header,
;	call the appropriate routine,
;	restore registers, and return.

	pushf
	push ds
	push es
	push ax
	push bx
	push cx
	push dx
	push si
	push di
	cld
	sti
	mov ds,myds

;	get relevant information from request header,
;	for the functions to use
	les di,rhptr
	mov al,es:[di+2] ; ax = Command Code
	cbw
;	set default status = DONE
	mov es:[di+3],100h
	mov cx,es:[di+18]
	les di,es:[di+14]

	; call our functions
	cmp al,12
	jb cmdok
;	command out of range
	mov al,2 ; replace with an illegal command
cmdok:	mov si,offset _TEXT:functab
	add si,ax
	add si,ax
	call cs:[si]

	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax
	pop es
	pop ds
	popf
	ret

HDR     endp


;	coherent kernel functions
cohker	proc near
	public _sphi, _spl, _inb, _outb, _isin, _kalloc
	public _memcpy, _setivec, _read_t0

_read_t0:	pushf
	cli
	mov al,0
out 43h,al
	out  0e0h,al
	in al,40h
	mov ah,al
	out 0e0h,al
	in al,40h
	out 0e0h,al
	xchg al,ah
	popf
	ret

;	a simplistic emulation of setivec(), clrivec() is not provided.
;	may only be called once, with either 2 or 3.
_setivec:
	push bp
	mov bp,sp
;	change parameters if on com2
	cmp word ptr [bp+4],4
	jz oncom1
	mov intofs,2ch
	mov irm,0f7h
	dec eoi
oncom1:
	mov ax,[bp+6]
	mov ivec_fn,ax

;	set interrupt vector
	xor ax,ax
	mov es,ax
	mov bx,intofs
	mov word ptr es:[bx],offset _TEXT: ih_com
	mov es:[bx+2],cs

	in al,21h                     ;set enable bit on 8259
	and al,irm
	nop ; timing
	nop
	out 21h,al

	pop bp
	ret

	_sphi:	pushf
	cli
	pop ax
	ret

_spl:	push bp
	mov bp,sp
	mov ax,[bp+4]
	push ax
	popf
	pop bp
	ret

_inb:	push bp
	mov bp,sp
	mov dx,[bp+4]
	mov ah,0
	in al,dx
	pop bp
	ret

_outb:	push bp
	mov bp,sp
	mov ax,[bp+6]
	mov dx,[bp+4]
	out dx,al
	pop bp
	ret

_memcpy:	push bp
	mov bp,sp
	push ds
	pop es
	push si
	push di
	mov si,[bp+6]
	mov di,[bp+4]
	mov cx,[bp+8]
	repz movsb
	pop di
	pop si
	pop bp
	ret

;	store character in our internal BIOS-like keyboard buffer
_isin:	push bp
	mov bp,sp
	pushf
	cli
	mov ax,[bp+4]
;	-1 means flush the buffer
	cmp ax,-1
	jnz isinflush
	mov bx,ADDR: kbbuf
	mov kbhead,bx
	mov kbtail,bx
	mov ax, 0
	jmp short endisin
isinflush:
;	work around an MSDOS bug, control C goes right to the top
;	of the input queue, so you can blow away a running program,
;	even if you have accidentally typed another character.
	cmp al,3
	jnz normchar
	mov bx,ADDR: kbbuf
	mov kbhead,bx
	mov kbtail,bx
normchar:	mov bx,kbhead
	mov [bx],ax
	inc bx
	inc bx
	cmp bx,ADDR: kbbuf+KBSIZ*2
	jnz wrap3
	mov bx,ADDR: kbbuf
wrap3:	cmp bx,kbtail
	mov ax,-1
	jz endisin
	mov ax,0
	mov kbhead,bx
endisin:	popf
	pop bp
	ret

_kalloc:	push bp
	mov bp,sp
	test byte ptr inload,0ffh
	jz kalerr
	mov cx,[bp+4]
	mov ax,endbound
	add ax,cx
	jc kalerr
	xchg ax,endbound
	jmp short kalend
kalerr:	mov ax,0
kalend:	pop bp
	ret
cohker	endp

;	null function, for irrelevant comands
badf	proc near
	les di,rhptr
	mov es:[di+3],8103h ; unknown function for this device driver
nulf:	ret
badf	endp

;	init function, set buffer, set interrupt vectors, set memory usage
init	proc near
;	set up our own ds
	mov ax,offset _TEXT: DGROUP
	mov cl,4
	shr ax,cl
	mov bx,cs
	add ax,bx
	mov ds,ax
	mov myds,ax

;	zero .bss area
	mov es,ax
	xor ax,ax
	mov cx,ADDR:databeg
	mov di,ADDR:bssbeg
	sub cx,di
	shr cx,1
	rep stosw

;	get buffer size from config.sys line
;	size specified in 1024 byte blocks, consistent with ramdisk.dev
	les di,rhptr
	les di,es:[di+18] ; es:di = config.sys command line
	push es
	push di
	call _dosline
	add sp,4

;	C initialization routine
	call _sdload

	les si,rhptr
	mov ax,endbound
	mov es:[si+14],ax
	mov es:[si+16],ds
	mov inload,0

;	set interrupt vectors
;	should be done using dos calls 25h and 35h,
;	but only calls 00h through 0ch and 30h are supported
;	during device driver init phase.
;	Disk Operating System Technical Reference Programming Manual
;	[IBM inc.] chapter 3.
;	Therefore, we disable interrupts,
;	and write the lowmem vectors directly
	xor bx,bx
	mov ds,bx
	cli ; rest of init procedure has interrupts disabled
	mov ax,[bx+20h] ; save original real time interrupt vector
	mov orig_rti,ax
	mov ax,[bx+22h]
	mov orig_rti+2,ax
	mov word ptr [bx+20h],offset _TEXT: ih_rti  ;  real time interrupt
	mov [bx+22h],cs
	mov ax,[bx+24h] ; save original keyboard interrupt vector
	mov orig_kbi,ax
	mov ax,[bx+26h]
	mov orig_kbi+2,ax
	mov word ptr [bx+24h], offset _TEXT: ih_kbi
	mov [bx+26h],cs

;	take over BIOS keyboard interrupt,
;	to accommodate our internal type ahead buffer (long).
	mov ax,[bx+58h] ; save original bios keyboard interrupt vector
	mov orig_bkb,ax
	mov ax,[bx+5ah]
	mov orig_bkb+2,ax
	mov word ptr [bx+58h],offset _TEXT: ih_bkb ; BIOS keyboard interrupt
	mov [bx+5ah],cs

;	same for screen BIOS routine
	mov ax,[bx+40h] ; save original bios screen interrupt vector
	mov orig_bsc,ax
	mov ax,[bx+42h]
	mov orig_bsc+2,ax
	mov word ptr [bx+40h],offset _TEXT: ih_bsc ; BIOS screen interrupt
	mov [bx+42h],cs

;	empty our own keyboard buffer
	mov ds,myds
	mov kbhead,ADDR: kbbuf
	mov kbtail,ADDR: kbbuf
	sti

;	call original bios routine to clear any accumulated MS-DOS characters
drain:	mov ah,1
	pushf
;	call far ptr cs:[orig_bkb], I can't figure out the masm syntax for this
	db 2eh, 0ffh, 01eh
	dw orig_bkb
	jz endinit
	mov ah,0 ; remove character
	pushf
;	call far ptr cs:[orig_bkb]
	db 2eh, 0ffh, 01eh
	dw orig_bkb
	jmp short drain

endinit:	ret
init	endp

;	read ioctl values
;	used by a calling process to copy the text buffer to a file
ioctl	proc near
;	returns pointers to the start and end of the internal text buffer
;	plus the head and tail pointers.
;	thus only 16-byte ioctl reads are allowed
;	calling process must understand this driver implicitly.
	cmp cx,16
	jnz badioctl
;	call C routine
	push es
	push di
	mov ax, 0
	push ax
	call _sdioctl
	add sp,6
	or ax,ax
	jz ioctlok
badioctl:
	les di,rhptr
	mov word ptr es:[di+18],0 ; 0 bytes transfered
	mov es:[di+3],810bh ; read fault, cannot return that many bytes
ioctlok:
	ret
ioctl	endp

;	screen BIOS routine
;	save characters written to screen, and pass all other commands through
ih_bsc	proc far
	cmp ah,14
	jnz go10
	cmp bx,7
	jz scrchar

;	jmp far ptr cs:[orig_bsc]
go10:	db 2eh, 0ffh, 02eh
	dw orig_bsc

scrchar:
	push ds
	push ax
	push bx
	push cx
	push dx
	mov ds,myds
	pushf
	sti

	push ax
;	console is owner
	mov ax,0
	push ax
	call _sdoutchar
	add sp,4
	popf

	test al,2
	jz noesc
	pop dx
	pop cx
	push cx
	push dx
	push ax
	mov ax,0e1bh
	mov bx,7
	pushf
;	call far ptr cs:[orig_bsc]
	db 2eh, 0ffh, 01eh
	dw orig_bsc
	pop ax
noesc:	test al,1
	pop dx
	pop cx
	pop bx
	pop ax
	pop ds
	jz noscreen
	jmp short go10

noscreen: iret
ih_bsc	endp

;	interrupt routines

;	read the virtual screen, in real time
;	called at real time interrupt level
;	Since this mess could easily take more than 50ms,
;	and I don't feel like counting instructions for all possible paths,
;	we call the original real time interrupt right away,
;	then we interpret keyboard commands, read text, etc.
ih_rti	proc far ; user real time interrupt handler

	pushf ; call the original interrupt routine
;	call far ptr cs:[orig_rti]
	db 2eh, 0ffh, 01eh
	dw orig_rti

;	simulate Coherent lbolt
	push ds
	mov ds,myds
	add word ptr _lbolt,5
	adc word ptr _lbolt+2,0
	pop ds

;	check for nested real time interrupts
	dec nestrti
	jz go_rti
	inc nestrti
	iret ; previous routine still reading, or whatever
go_rti:

;	switch stacks
	mov save_ss,ss
	mov save_sp,sp
;	New stack overlays initialization code, about 200 bytes.
	push cs
	pop ss
	mov sp,offset _TEXT: drain

	cld
	push ds
	push ax
	push bx
	push cx
	push dx
	push es

	mov ds,myds
	sti

call _sdtime

	cli
	pop es
	pop dx
	pop cx
	pop bx
	pop ax
	pop ds
	mov ss,save_ss ; restore old stack
	mov sp,save_sp
	inc nestrti
	iret
ih_rti	endp


ih_com	proc far
	cld
	push ds
	push ax
	push bx
	push cx
	push dx

	mov ds,myds
	cld

	mov bx,ivec_fn
	call bx

	mov al,eoi
	out 20h,al
	pop dx
	pop cx
	pop bx
	pop ax
	pop ds
	iret
ih_com	endp


ih_kbi	proc far ; keyboard interrupt handler
	pushf ; call the original hardware interrupt routine
;	call far ptr cs:[orig_kbi]
	db 2eh, 0ffh, 01eh
	dw orig_kbi
;	if a key was entered into the MS-DOS buffer,
;	and the key represents a reading command,
;	remove it from the buffer, and place the corresponding
;	command into the talkcmd location
;	we assume no real time (or other) interrupt routine will
;	grab the key from the buffer before this routine can snag it.
	cld
	push ax
	push bx
	push cx
	push dx
	push ds
;	C routines preserve si, di, and bp

;	call original bios routine to get the entered character
	mov ah,1
	pushf
;	call far ptr cs:[orig_bkb]
	db 2eh, 0ffh, 01eh
	dw orig_bkb
	jz nokey ; no new characters typed
;	now get the character
	mov ah,0
	pushf
;	call far ptr cs:[orig_bkb]
	db 2eh, 0ffh, 01eh
	dw orig_bkb

	mov ds,myds
	push ax
	cmp word ptr _sdcontrol,0
	jz notactive
;	console is owner
	mov ax,0
	push ax
	call _sdinkey ; turn key into command code
	add sp,4
	jmp short nokey
notactive:
	call _isin
	add sp,2

nokey:	pop ds
	pop dx
	pop cx
	pop bx
	pop ax
	iret
ih_kbi	endp

;	new BIOS keyboarde routine, to accommodate new type ahead buffer.
;
;	the original bios routine enables interrupts,
;	and when ah = 1, leaves them that way,
;	regardless of the initial conditions.
;	don't know if this is a feature or a bug, but I emulate it.
;
;	For some bizarre reason, dos4.0 calls this BIOS routine
;	with ah = 10h, 11h, or 12h, instead of 0, 1, or 2.
;	I have no idea what the extra bit means.
;	I simply clear it out, and things seem to work.
;	Thus this bios routine works under all versions of dos,
;	as of this writing.
ih_bkb	proc far
	sti
	and ah,0efh
	cmp ah,1
	jb getc
	jz nbget
;	jmp far ptr cs:[orig_bkb]
	db 2eh, 0ffh, 02eh
	dw orig_bkb

nbget:	;	non blocking getchar
	push ds
	push bx
	mov ds,myds
	mov bx,kbtail
	cmp bx,kbhead
	mov ax,[bx]
	pop bx
	pop ds
	ret 2

getc:	;	blocking getchar
	push ds
	push bx
	mov ds,myds
	mov bx,kbtail
	cmp bx,kbhead
	mov ax,[bx]
	jz getc
	inc bx
	inc bx
	cmp bx,ADDR: kbbuf+KBSIZ*2
	jnz wrap6
	mov bx,ADDR: kbbuf
wrap6:	mov kbtail,bx
	pop bx
	pop ds
	iret
ih_bkb	endp

_TEXT    ends


_BSS	segment
bssbeg	dw ?
_BSS	ends


_DATA	segment

	extrn _lbolt:word, _sdcontrol:word

databeg	label word

;	we implement our own interrupt level keyboard buffer
;	the ms-dos buffer (15 characters) is frustratingly small
kbbuf	dw KBSIZ dup(?)
kbhead	dw  kbbuf
kbtail	dw  kbbuf

ivec_fn	dw 0 ; setivec function

endbound dw ADDR: boundary
inload	db 1 ; are we in load phase?

intofs	dw 30h ; offset for interrupt vector
irm	db 0efh ; interrupt register mask
eoi	db 64h ; end of interrupt code

_DATA    ends


;	to find the end of DGROUP, and the start of word table and buffer
_EDATA	segment
boundary	label word
_EDATA	ends

	end HDR
