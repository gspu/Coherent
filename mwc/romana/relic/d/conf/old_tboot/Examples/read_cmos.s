CMOSA	=	0x70	/ Real-time Clock/CMOS addr I/O port.
CMOSD	=	0x71	/ Real-time Clock/CMOS data I/O port.
/
/       Read a byte from the CMOS.  Takes one argument--the
/       CMOS address to read from as an int; returns the
/       value read as a char.
/
        .globl	read_cmos_
read_cmos_:
	push	si
	push	di
        push    bp
        mov     bp, sp
        movb    al, 8(bp)       / Fetch address from stack.
        outb    CMOSA, al       / Send address to CMOS.
        jmp     .+2             / DELAY
        inb     al, CMOSD       / Get Value from CMOS.
	movb	ah, $0
        pop     bp
	pop	di
	pop	si
        ret                     / Return from read_cmos().

