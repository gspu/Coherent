/	sdsounds.s:
/	various sounds  that support the speaking device driver

	.text

timer1:	pushf
	cli
	mov eax,$0
outb $0x43
	outb  $0xe0
	inb $0x40
	movb ah,al
	outb $0xe0
	inb $0x40
	outb $0xe0
	xchg al,ah
	popf
	ret

/	Wait for timer1, edx ticks
t1wait:
/	get initial value of timer 1
	call timer1
	xchg eax,ebx
next_t1:
	call timer1
	xchg ebx,eax
	sub eax,ebx
	jnc ?nowrap
	add eax,$11932
?nowrap:	sub edx,eax
	jnc next_t1 / not yet finished
	ret

/	C interface to the above routine
	.globl sdpause
sdpause:
	push ebp
	mov ebp,esp
	mov edx,8[ebp]
	push ebx
	call t1wait
	pop ebx
	leave
	ret

/	toggle the speaker.
/	Do not change a thing if a tone is in progress.
toggle:
	pushf
	cli
	inb $0x61
	test al,$1
	jnz notgl / cannot interrupt a tone with a click
	xor al,$2
	outb $0x61
notgl:	popf
	ret

/	the click of a character being put on the screen
	.globl sdclick
sdclick:
	push ebx
	call toggle
	mov edx,sdtm_click
	call t1wait
	call toggle
sndret:	pop ebx
	ret

	.globl sdcrsnd
sdcrsnd:  / the sound of a carriage return
	push ebx
	mov ebx,sdtm_topcr
rs1:	call toggle
	mov edx,ebx
	push ebx
	call t1wait
	pop ebx
	sub ebx,sdtm_inccr
	cmp ebx,sdtm_botcr
	ja rs1
	jmp sndret

