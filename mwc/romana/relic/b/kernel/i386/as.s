	.unixorder
	.llen	132
	.include	as.inc

IODELAY	.macro
	jmp	.+2		/ DELAY
	jmp	.+2		/ DELAY
	.endm

/ Most places where %cr3 is refreshed, it can be done through a Ring 0 gate.
MMUUPD	.macro
	pushfl
	cli
	lcall	$SEG_MMUUPD,$0	/ gate to mmuupdfR0
	popfl
	.endm

/
/ USTART and ESP_START map kernel stack and u area within top 4k page
/ of virtual space.
/ NDP context starts 0x100 bytes below u area.
/ See also U_OFFSET, NDP_OFFSET in uproc.h
/
	.set	USTART,0xFFFFFC00
	.set	ESP0_START,0xFFFFF300
	.set	ESP1_START,USTART

	.set	u,USTART
	.set	PSW_VAL,0x1200	/ set system IOPL to 1, enable IRQ
/	.set	PSW_VAL,0x3200	/ set system IOPL to 3, enable IRQ

/ (lgl-
/	The information contained herein is a trade secret of Mark Williams
/	Company, and  is confidential information.  It is provided  under a
/	license agreement,  and may be  copied or disclosed  only under the
/	terms of  that agreement.  Any  reproduction or disclosure  of this
/	material without the express written authorization of Mark Williams
/	Company or persuant to the license agreement is unlawful.

/	Copyright (c) 1982, 1992.
/	An unpublished work by Mark Williams Company, Chicago.
/	All rights reserved.

/	Intel 386 port and extensions
/	Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
/ -lgl)
/ 
/ $Log:	as.s,v $
/Revision 1.18  93/06/14  13:42:14  bin
/Hal: kernel 78 update
/
/ Revision 1.17  92/12/08  16:43:10  root
/ ker 70
/ 
/ Revision 1.16  92/11/12  10:04:31  root
/ Ker #68
/ 
/ Revision 1.15  92/11/09  17:08:28  root
/ Just before adding vio segs.
/ 
/ Revision 1.13  92/10/06  23:47:48  root
/ Ker #64
/ 
/ Revision 1.12  92/10/06  20:45:40  root
/ Ker #63d
/ 
/ Revision 1.10  92/07/27  18:15:43  hal
/ Kernel #59
/ 
/ Revision 1.9  92/07/16  16:38:14  hal
/ Kernel #58
/ 
/ Revision 1.8  92/07/15  13:50:55  root
/ COH 4.0.0
/ 
/ Revision 1.6  92/04/03  11:05:28  hal
/ Fix missed IRQ bug.
/ Add read_t0(), read_psw(), getusd(), putusd().
/ 

///////
/ Machine language assist for
/ Intel 80386/80486 Coherent. This contains the parts
/ that are common to all machines as well as the machine-specific code
/ for the IBM PC-386


///////

/ System entry point.

/ When this code is entered, the boot program has done the following:
/	Relocate itself above where the kernel will be (e.g., 0x20600).
/	Load as.s binary; text at 0x02000, data at next paragraph (16-byte)
/		boundary at or beyond end of kernel text.
/	CS = 0x02000 <- start of kernel text ("physical" stext)
/	ES = 0x02xxx <- start of kernel data
/	SS,DS = 0x20xxx <- ....some address in boot data space....

/ Due to the way the kernel has been linked (see ld.master), symbol "stext"
/ has a value of 0xFFC0_0000, which is the start of the last 4 meg segment.
/ This value is the address in linear space once we have entered paging mode,
/ but until that time relocation arithmetic is necessary:
/
/ Before segmentation is turned on, symbols in kernel text or data space
/ must be relocated by -SBASE<<BPCSHIFT for memory reference instructions
/ to work.

///////

stext:					/ kernel code starts at stext+0x100
	.org .+0x100			/ reserve stack space
	cli				/ No interrupts, please.

/ put up a debugging "!" on the screen.  We can still use the BIOS.
	push    %si                      / Save registers.
	push    %di

	movb	$'!', %al

	movw	$0x0007, %bx             / Page 0, white on black
	movb	$0x0E, %ah               / Write TTY.
	int	$VIDEO                   / Call video I/O in ROM.
	pop	%di
	pop	%si

	/ equipment status word to AX
	int	$0x11			/ Obtain int 11 value before printf().
	movl	%eax,%ecx		/ esw -> cx

	/ val11 is a long, initially zero, in the CS.
	/ copy (long)esw to val11
	.byte	PX_ADDR			/ 32-bit address
	.byte	PX_OPND			/ 32-bit operand
	movl	%ecx,%cs:[[-SBASE]<<BPCSHIFT]+val11

   					/ last use of boot block's stack
	.byte	PX_ADDR			/ 32-bit address
	.byte	PX_OPND			/ 32-bit operand
	lgdtl	%cs:[[-SBASE]<<BPCSHIFT]+gdtinit

	/ turn on lsbit of cr0 - Protection Enable
	mov	%cr0,%eax
	orb	$1,%al
	mov	%eax,%cr0

	/ intersegment jump (48-bit address)
	/ jumping flushes the cache...
	/ 
	.byte	PX_OPND
	ljmp	$SEG_386_II, $next

next:
	mov	$SEG_386_ID,%eax
	movw	%ax,%ds
	movw	%ax,%ss
	mov	$SEG_386_UD|R_USR,%eax
	movw	%ax,%es
	mov	$stext+0x100,%eax	/ 256 byte stack for initialization
	mov	%eax,%esp

	push	$'@'			/ Debugging checkpoint:
	call	chirp			/ protected mode is on
	pop	%ecx

/ Enable the A20 address line, which is normally disabled by the ROM BIOS.
/ This line is under the control of the 8042 keyboard interface controller.

	sub	%ecx, %ecx
loc0:	inb	$KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	$2,%al
	loopne	loc0
	IODELAY

	movb	$0xD1, %al 	/ Request next output byte to be
	outb	$KBCTRL		/	sent to the 8042 output port.
	IODELAY

	sub	%ecx, %ecx
loc1:	inb	$KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	$2, %al
	loopne	loc1
	IODELAY

	movb	$0xDF,%al	/ Enable A20 address line.
	outb	$KBDATA		/ See Page 1-44, IBM-AT Tech Ref.
	IODELAY

	sub	%ecx, %ecx
loc2:	inb	$KBCTRL		/ Wait for 8042 input buffer to empty.
	testb	$2,%al		/ NOTE: A20 not enabled for up to 20 us.
	loopne	loc2


/ Reprogram the 8253 timer so that channel 0, 
/ which is used as the clock, interrupts at exactly
/ 100 HZ, instead of 18.2 HZ.

	movb	$0x36,%al	/ Timer 0, LSB, MSB, mode 3
	outb	$PIT+3
	IODELAY
	movb	$0x9C,%al	/ Lsb of 59659/5 = 11932
	outb	$PIT
	IODELAY
	movb	$0x2E,%al	/ Msb of 59659/5 = 11932
	outb	$PIT
	IODELAY

/ Reprogram the 1st programmable interrupt controller.
/ Its default vector table collides with iAPX 286 protection vectors.
	movb	$0x11,%al	/ ICW1 - edge, ICW4
	outb	$PIC
	IODELAY
	movb	$0x20,%al	/ ICW2 - Reserve 1st 32 vectors for 286
	outb	$PICM
	IODELAY
	movb	$0x04,%al	/ ICW3 - master level 2
	outb	$PICM
	IODELAY
	movb	$0x01,%al	/ ICW4 - 8086 mode, master.
	outb	$PICM
	IODELAY

/ NIGEL: The original code here (and related code in "i386/md.c") turned off
/ the chain bit in the first PIC by default (and at every subsequent
/ opportunity) even though all the mask bits in the slave PIC are set to off.
/ In order to support an enhanced interrupt architecture for the STREAMS and
/ DDI/DDK subsystems I want to remove the state knowledge from the code in
/ "i386/md.c" so that the chain bit is always left on.
/ In order to do this, I have modified the startup code below so that the
/ system by default allows the slave PIC to interrupt (of course, it still
/ won't interrupt unless it is enabled to; the masking I have removed was
/ totally redundant).

	movb	$0xFA,%al	/ Disable interrupts from master PIC.
	outb	$PICM		/ (except for clock and slave PIC interrupt).

	movb	$0x11,%al	/ ICW1 - edge, ICW4
	outb	$SPIC
	IODELAY
	movb	$0x70,%al	/ ICW2 - slave starts at 0x70th interrupt
	outb	$SPICM
	IODELAY
	movb	$0x02,%al	/ ICW3 - master level 2
	outb	$SPICM
	IODELAY
	movb	$0x01,%al	/ ICW4 - 8086 mode.
	outb	$SPICM
	IODELAY
	movb	$0xFF,%al
	outb	$SPICM		/ Disable interrupts from slave PIC.
/DEBUG
	cli
	call	__cinit
	call	mchinit		/ C initialization
	mov	%cr0,%eax	/ Turn on paging
/ use 80000001 to allow FP
/	or	$0x80000001,%eax
/ use 80000005 to disallow FP
	or	$0x80000005,%eax
	mov	%eax,%cr0
	ljmp	$SEG_RNG0_TXT,$loc3	/ clear pipeline; jump far, direct

/
/ Ring 0 startup code
/
loc3:
	movw	$SEG_386_KD,%ax
	movw	%ax,%ds
	movw	$SEG_RNG0_STK,%ax
	movw	%ax,%ss
	movl	$ESP0_START,%esp	/ Stack pointer for init
	clts				/ Clear task switched flag.

/ Call the machine setup code.
/ Call Coherent main.
/ On return, send control off to the user
/ at its entry point.

	sub	%eax, %eax	/ Load local descriptor table register.
	lldt	%ax

/	movw	$tss,%ax	/ Fix low 16 bits of tss base in gdt
/	movw	%ax,gdt+SEG_TSS+2

	/ Fix tss base in gdt
	movl	$tss,%eax
	movw	%ax,gdt+SEG_TSS+2	/ Fix bits  0..15
	rorl	$16,%eax		/ Get tss bits 16..31
	movb	%al,gdt+SEG_TSS+4	/ Fix bits 16..23
	movb	%ah,gdt+SEG_TSS+7	/ Fix bits 24..31

	movw	$SEG_TSS,%ax		/ Load task state segment register.
	ltr	%ax
	lidt	idtmap			/ Load interrupt descriptor table
	lgdt	gdtmap

/	movw	$ldt,%ax		/ Relocate ldt in gdt
/	movw	%ax,gdt+SEG_LDT+2

	/ Fix ldt base in gdt
	movl	$ldt,%eax
	movw	%ax,gdt+SEG_LDT+2	/ Fix bits  0..15
	rorl	$16,%eax		/ Get ldt bits 16..31
	movb	%al,gdt+SEG_LDT+4	/ Fix bits 16..23
	movb	%ah,gdt+SEG_LDT+7	/ Fix bits 24..31

	movw	$SEG_LDT,%ax 
	lldt	%ax

	call	i8086			/ i8086() does fixup of tss_sp0

/
/ Enter Ring 1 kernel from Ring 0
/
	push	$SEG_RNG1_STK		/ SS
	push	$ESP1_START		/ ESP
	push	$PSW_VAL		/ PSW
	push	$SEG_RNG1_TXT		/ CS
	push	$__xmain__		/ IP
	movw	$SEG_386_KD,%ax		/ DS, ES
	movw	%ax, %ds		/ Map data segment
	movw	%ax, %es		/ Map extra segment
	iret				/ Go to user state.

/
/ Start of Ring 1 kernel.
/ Need Ring 1 because interrupts are about to turn on, and all irpt gates
/ have DPL (descriptor privilege level) 1.
/
__xmain__:
	sti				/ Interrupts on, and
	call	main			/ call Coherent mainline.
	cli				/ Interrupts off.

/
/ Enter User mode from Ring 1 kernel
/
	push	$SEG_386_UD|R_USR	/ SS
	push	$NBPC			/ ESP
	push	$PSW_VAL		/ PSW
	push	$SEG_386_UI|R_USR	/ CS
	push	$0			/ IP
	movw	$SEG_386_UD|R_USR,%ax	/ DS, ES
	movw	%ax, %ds		/ Map data segment
	movw	%ax, %es		/ Map extra segment
	iret				/ Go to user state.

///////
/
/ Trap and interrupt save.
/
/ This version of tsave runs from Ring 1 trap/irpt gates.
/
///////
	.globl	disflag

tsave:					/ What level of interrupt ? 
	pusha
	push	%ds			/ Save current state
	push	%es
	push	%fs
	push	%gs

	mov	$SEG_386_KD,%eax	/ Map ds
	movw	%ax,%ds
	mov	$SEG_386_UD|R_USR,%eax	/ Map es
	movw	%ax,%es			/ to system ds

	sti
	icall	X_TRAPNO(%esp)		/ and call the caller
	cli

/ if got here from user mode or from idle process, call stand()
/ else just do cleanup and return

	movb	X_ERR+8(%esp),%al	/ trapped CS:  user RPL?
	andb	$3,%al
	cmpb	$R_USR,%al
	je	tsave1a			/ jmp if user mode
	cmpl	$__idle__,X_ERR+4(%esp)	/ trapped EIP == idle process?
	jnz	tsave1b			/ Call stand() only if idle
	movl	$1,disflag

tsave1a:
	sti
	call	stand
tsave1b:
	cli				/ No more interrupts
	pop	%gs			/ Restore
	pop	%fs
	pop	%es
	pop	%ds
	popa
	add	$8,%esp			/ forget err, trapno
	iret				/ Done.

/
/ Here is another version of tsave, called only from the GP vector (RING 0)
/

BYPASS	.macro	addr
	cmpl	$addr,X_ERR+4(%esp)	/ trapped EIP
	jz	tsave0b
	.endm
	
tsave0:					/ What level of interrupt ? 
	pusha
	push	%ds			/ Save current state
	push	%es
	push	%fs
	push	%gs

	mov	$SEG_386_KD,%eax	/ Map ds
	movw	%ax,%ds
	mov	$SEG_386_UD|R_USR,%eax	/ Map es
	movw	%ax,%es			/ to system ds

	jmp	tsave0b

//The following lines help find traps during startup.
	BYPASS	read_cr0
	BYPASS	read_cr2
	BYPASS	read_cr3
tsave0q:
 	mov	52(%esp),%eax		/ Print fault code.
 	cmpb	$0x40,%al
 	je	tsave0b			/ Skip over hardware interrupts.
 	push	%eax
 	call	print32
 	pop	%ecx
 
 	push	$' '
 	call	mchirp
 	pop	%ecx
 
 	mov	56(%esp),%eax		/ Print eip.
 	push	%eax
 	call	print32
 	pop	%ecx

 	push	$' '
 	call	mchirp
 	pop	%ecx

 	push	%esp			/ Print esp.
 	call	print32
 	pop	%ecx
 
 tsave0a:	jmp	tsave0a
 tsave0b:
//

	icall	X_TRAPNO(%esp)		/ and call the caller

	pop	%gs			/ Restore
	pop	%fs
	pop	%es
	pop	%ds
	popa
	add	$8,%esp			/ forget err, trapno
	iret				/ Done.

///////

/ Save the environment of a process
/ envsave(p)
/ MENV *p;

/ Save the context of a process
/ consave(p)
/ MCON *p;

///////

envsave:
consave:
	mov	%edi,%ecx		/ Hide di.
	mov	4(%esp), %edi 		/ di at the MCON block.

	movw	%es,%dx		/ save = setspace(SEG_386_KD) -- should be %edx
	movw	$SEG_386_KD,%ax
	movw	%ax,%es

	cld				/ Ensure increment.
	mov	%ecx, %eax		/ Save di
	stosl
	mov	%esi, %eax		/ Save si
	stosl
	mov	%ebx, %eax		/ Save bx
	stosl
	mov	%ebp, %eax		/ Save bp
	stosl
	mov	%esp, %eax		/ Save sp
	stosl
	mov	(%esp), %eax		/ Save ra as pc
	stosl
	pushfl				/ Save fw
	pop	%eax
	stosl
	mov	%ecx, %edi		/ Put di back,
	sub	%eax, %eax		/ indicate a state save and
	movw	%dx,%es			/ setspace(save)
	ret				/ return to caller.

///////

/ Restore the environment of a process.
/ envrest(p)
/ MENV *p;

///////

envrest:
	cli
	cld
	mov	4(%esp),%esi		/ Pointer to context
	lodsl				/ Restore di
	mov	%eax, %edi
	lodsl				/ Restore si
	mov	%eax, %ecx		/ Save for later
	lodsl				/ Restore bx
	mov	%eax, %ebx
	lodsl				/ Restore bp
	mov	%eax, %ebp
	lodsl				/ Restore sp
	mov	%eax, %esp
	push	%cs			/ Push current CS
	lodsl				/ Restore pc
	push	%eax
	lodsl				/ Restore flags
	mov	%eax, 8(%esp)		/ Stack now in form PSW,CS,IP.
	mov	%ecx, %esi		/ Restore si
	mov	$1,%eax			/ We are restoring
	iret				/ Return through PSW,CS,IP.

///////

/ Restore the context of a process.
/ Called with interrupts disabled from dispatch.
/ conrest(u, o)
/ saddr_t u;

///////

conrest:
	mov	8(%esp), %esi		/ Fetch syscon offset

	cli				/ Interrupts on hold
	cld

	/ Map new u area into linear space and update paging hardware

	mov	4(%esp),%eax		/ Fetch new u area saddr_t
	orb	$SEG_SRW,%al
	mov	%eax,[PTABLE1_V<<BPCSHIFT]+UADDR

	lcall	$SEG_MMUUPD,$0	/ strobe CR3

	/ Restore context

	lodsl				/ Restore di
	mov	%eax,%edi
	lodsl		 		/ Restore si
	mov	%eax,%ecx		/ Save for later
	lodsl				/ Restore bx
	mov	%eax,%ebx
	lodsl				/ Restore bp
	mov	%eax,%ebp
	lodsl				/ Restore sp
	mov	%eax,%esp
	push	%cs			/ Push current CS
	lodsl				/ Restore pc
	push	%eax

	lodsl				/ Restore flags
	mov	%eax,8(%esp)		/ Stack now in form PSW,CS,IP.
	mov	%ecx,%esi		/ Restore si
	mov	$1,%eax			/ We are restoring
	iret				/ Return through PSW,CS,IP.

/ Save useful registers.

/ msysgen(p)
/ MGEN *p;

msysgen:
	ret				/ Nothing useful to save

/ Disable interrupts.  Previous value is returned.

sphi:
	pushf				/ Save flags
	pop	%eax			/ Return current value
	cli				/ Disable interrupts
	ret				/ And return

/ Enable interrupts.  Previous value is returned.

splo:
	pushf
	pop	%eax
	sti
	ret

/ Change interrupt flag.  Previous value is returned.

spl:
	pop	%eax			/ ip
	pop	%edx			/ psw
	push	%edx
	push	%edx			/ push psw, cs, ip for iret
	push	%cs
	push	%eax
	pushf				/ old psw
	pop	%eax
	iret

///////

/ Idle routine.
/ Enable interupts, and wait for something to
/ happen. Does not do anything to the 8259, bacause
/ this will be set up correctly.

///////
	.globl	idle
idle:
	sti				/ Interupts on.
__idle__:
	jmp	__idle__		/ Wait for an interrupt
	ret				/ and return.

///////

/ The world is indeed grim.
/ Hang. Keep the interrupts on so that the
/ keyboard can get int.

///////

halt:	sti				/ Be safe,
__halt__
	jmp	__halt__		/ And hang.

///////

/ Basic port level I/O.

/ Byte I/O (8 bits)
/ int	inb(port);
/ int	outb(port, data);

/ Word I/O (16 bits)
/ int	inw(port);
/ int	outw(port, data);

/ Long I/O (32 bits)
/ int	inl(port);
/ int	outl(port, data);

///////

inb:	mov	4(%esp),%edx
	sub	%eax,%eax
	inb	(%dx)
	ret

outb:	movl	4(%esp),%edx
	movl	8(%esp),%eax
	outb	(%dx)
	ret

inw:	mov	4(%esp),%edx
	sub	%eax,%eax
	inw	(%dx)
	ret

outw:	movl	4(%esp),%edx
	movl	8(%esp),%eax
	outw	(%dx)
	ret

inl:	mov	4(%esp),%edx
	sub	%eax,%eax
	inl	(%dx)
	ret

outl:	movl	4(%esp),%edx
	movl	8(%esp),%eax
	outl	(%dx)
	ret

///////

/ AT Hard Disk Assembler Support

/ atsend( va ) - send 512 bytes from virtual address to hard disk
/ atrecv( va ) - receive 512 bytes from hard disk into virtual address
/ DRQ is not checked.  DRQ must be true before atsend/atrecv are called.

/ va is a system global address
///////

	.globl	atsend
atsend:
	enter	$0,$4	/ reserve 4 bytes (1 int) of local storage
	push	%esi
	push	%es
	call	workAlloc	/ get a temp virt page
	movl	%eax,-4(%ebp)	/ this is "work0" - a click number

	movw	$SEG_386_KD,%ax
	movw	%ax,%es			/ save = setspace(SEG_386_KD)

	cld
	mov	20(%esp),%eax		/ fetch argument va
	shr	$BPCSHIFT,%eax		/ get page table index from va
	mov	sysmem,%edx
	leal	(%edx,%eax,4),%esi 	/ base = sysmem.u.pbase + btocrd(va)

/ Since the requested transfer may span a click boundary, have two clicks
/ ready in the page table - the one containing the virtual address of the
/ start of the user area, and the click which follows in virtual memory.

	lodsl				/ ptable1_V[WORK0] = *base++ | SEG_SRW
	or	$SEG_SRW,%eax
	movl	-4(%ebp),%edx		/ work0
	movl	%eax,[PTABLE1_V<<BPCSHIFT](%edx,4)

	lodsl				/ ptable1_V[WORK1] = *base++ | SEG_SRW
	or	$SEG_SRW,%eax
	inc	%edx			/ work1
	movl	%eax,[PTABLE1_V<<BPCSHIFT](%edx,4)

	MMUUPD

/ Now that page boundaries are set, work on the offsets.

	mov	20(%esp),%esi		/ va = ctob(WORK0) + (va & (NBPC-1))
	and	$NBPC-1,%esi		/ get click offset part of va
	movl	-4(%ebp),%edx		/ work0
	shl	$BPCSHIFT,%edx		/ ctob(work0)
	add	%edx,%esi

	mov	$256, %ecx		/ copy one disk block
	mov	$0x1F0, %edx

/ Do the block transfer.

	rep
	outsw

	MMUUPD

	push	-4(%ebp)		/ workFree(work0)
	call	workFree
	pop	%edx
	pop	%es			/ setspace(save) 
	pop	%esi
	leave
	ret

	.globl	atrecv
atrecv:
	enter	$0,$4	/ reserve 4 bytes (1 int) of local storage
	push	%esi
	push	%es
	call	workAlloc	/ get a temp virt page
	movl	%eax,-4(%ebp)	/ this is "work0" - a click number

	movw	$SEG_386_KD,%ax
	movw	%ax,%es			/ save = setspace(SEG_386_KD)

	cld
	mov	20(%esp),%eax
	shr	$BPCSHIFT,%eax
	mov	sysmem,%edx
	leal	(%edx,%eax,4),%esi	/ base = sysmem.u.pbase + btocrd(va)

	lodsl				/ ptable1_V[WORK1] = *base++ | SEG_SRW
	or	$SEG_SRW,%eax
	movl	-4(%ebp),%edx		/ work0
	movl	%eax,[PTABLE1_V<<BPCSHIFT](%edx,4)

	lodsl				/ ptable1_V[WORK1] = *base++ | SEG_SRW
	or	$SEG_SRW,%eax
	inc	%edx			/ work1
	movl	%eax,[PTABLE1_V<<BPCSHIFT](%edx,4)

	MMUUPD

	mov	20(%esp),%esi		/ va = ctob(WORK0) + (va & (NBPC-1))
	and	$NBPC-1,%esi
	movl	-4(%ebp),%edx		/ work0
	shl	$BPCSHIFT,%edx		/ ctob(work0)
	add	%edx,%esi

	mov	$256, %ecx		/ copy one disk block
	mov	$0x1F0, %edx

	xchg	%esi,%edi
	rep				/ Value of the ECX register is not
/	insw	(%dx)			/ updated correctly
	insw
	xchg	%esi,%edi		

	MMUUPD

	push	-4(%ebp)		/ workFree(work0)
	call	workFree
	pop	%edx
	pop	%es			/ setspace(save) 
	pop	%esi
	leave
	ret

///////

/ This dummy routine is put in vector
/ table slots that are unused. All it does is
/ return to the caller.

///////

vret:	ret

/ mmuupd() uses a call gate.
mmuupd:
	pushf
	cli
	lcall	$SEG_MMUUPD,$0	/ gates to mmuupdfR0
	popf
	ret

/ Ring 0 far mmu update.  Called via a gate.  Uses %eax.
/ Want interrupts off when we arrive since the interrupt gates
/ lead into Ring 1.
mmuupdfR0:
	mov	$PTABLE0_P<<BPCSHIFT,%eax
	mov	%eax,%cr3
	lret

/ Ring 0 near mmu update.  Called from ring 0 startup.  Uses %eax.
mmuupdnR0:
	mov	$PTABLE0_P<<BPCSHIFT,%eax
	mov	%eax,%cr3
	ret

///////
/ Get cs selector - return 0 if in kernel, CS if not in kernel.
/ This version is for resident drivers.
/ There is a different version (cs_self.s) for loadable drivers.
/ int	cs_sel();
///////

cs_sel:
	sub	%eax, %eax
	ret

/	load the 'alternate address space register' (es)
/	with the segment reference passed as an argument. 

/	The value returned is the old value of the 'es' register

setspace:
	movl	%esp,%edx
	sub	%eax,%eax
	movw	%es,%ax
	movl	4(%edx),%edx
	movw	%dx,%es
	ret

/////////////////////////
/
/ From __xtrap_on__ to __xtrap_off__, GP fault and page fault will not
/ cause panic.  Normally, these two traps coming from kernel text result
/ in panic.
/
/////////////////////////
	.globl	__xtrap_on__
	.globl	__xtrap_break__
	.globl	__xtrap_off__
__xtrap_on__:

///////

/ Fetch a short from the user's data space.
/	Coherent 386 fetches a 16 bit short

/ getusd(u)
/ char *u;

///////

getusd:
	call	start_copy
	mov	4(%edx),%ecx
	sub	%eax,%eax
	movw	%es:(%ecx),%ax
	jmp	end_copy

///////

/ Fetch a word from the user's data space.
/	Coherent 386 fetches a 32 bit word

/ getuwd(u)
/ char *u;

///////

getuwd:
getupd:
	call	start_copy
	mov	4(%edx),%ecx
	mov	%es:(%ecx),%eax
	jmp	end_copy

///////

/ Fetch a byte from the user's data space.

/ getubd(u)
/ char *u;

///////

getubd:
	call	start_copy
	mov	4(%edx),%ecx
	sub	%eax,%eax
	movb	%es:(%ecx),%al
	jmp	end_copy

///////

/ Store a short into the user's data space.
/	Coherent 386 stores a 16 bit short

/ putusd(u, w)
/ char *u;
/ int w;

///////

putusd:
	call	start_copy
	mov	8(%edx),%eax
	mov	4(%edx),%ecx		/ eax
	movw	%ax,%es:(%ecx)
	jmp	end_copy

///////

/ Store a word into the user's data space.
/	Coherent 386 stores a 32 bit word

/ putuwd(u, w)
/ char *u;
/ int w;

///////

putuwi:
putuwd:
	call	start_copy
	mov	8(%edx),%eax
	mov	4(%edx),%ecx		/ eax
	mov	%eax,%es:(%ecx)
	jmp	end_copy

///////

/ Store a byte into the user's data space.

/ putubd(u, w)
/ char *u;
/ int w;

///////

putubd:
	call	start_copy
	mov	8(%edx),%eax	/ get data
	mov	4(%edx),%ecx	/ get addr
	movb	%al,%es:(%ecx)
	jmp	end_copy

///////

/ Read a byte from a selector and offset.

/ selkcopy(sel, off)
/ unsigned long sel;
/ unsigned long off;

///////
	.globl	selkcopy
selkcopy:
	call	start_copy
	push	%es
	sub	%eax,%eax
	mov	4(%edx), %es
	mov	8(%edx), %ecx
	movb	%es:(%ecx), %al
	pop	%es
	jmp	end_copy

/	startup routine for 1-element (byte/word) move

/ Block transfer "n" bytes from location
/ "k" in the system map to location "u" in the
/ user's data space. Return the number of bytes
/ transferred.

/ kucopy(k, u, n)
/ char *k;
/ char *u;
/ int n;

///////

/	.globl	udat
kucopy:
/	mov	8(%esp),%eax		/ verify user address
/	push	%eax
/	call	udat
/	cmp	$0,%eax
/	pop	%eax
/	jne	xx00
/	ret
/xx00:

	call	start_copy

	movl	4(%edx),%esi		/ esi
	movl	8(%edx),%edi		/ edi

	mov	12(%edx),%ecx
	sar	$2,%ecx
	je	loc6

	rep
	movsl

loc6:
	mov	12(%edx),%ecx
	andl	$3,%ecx
	je	loc7

	rep
	movsb

loc7:
	mov	12(%edx),%eax
	jmp	end_copy

///////

/ Block copy "n" bytes from location "u" in
/ the user data space to location "k" in the system
/ data space. Return the actual number of bytes
/ moved.

/ ukcopy(u, k, n)
/ char *u;
/ char *k;
/ int n;

///////

ukcopy:
	call	start_copy

	mov	4(%edx),%esi		/ esi
	mov	8(%edx),%edi		/ edi
	mov	12(%edx),%ecx

	push	%ds			/ exchange ds,es
	movw	%es,%ax			/ don't assume ss=ds
	movw	%ax,%ds
	pop	%es

	sar	$2,%ecx
	je	loc8

	rep
	movsl

loc8:	mov	%ss:12(%edx),%ecx
	andl	$3,%ecx
	je	loc9

	rep
	movsb

loc9:	mov	%ss:12(%edx),%eax
	jmp	end_copy		/ Return

////////
/
/ Block copy "n" bytes from far location "src" in
/ an arbitrary (but valid) to location "dst" in 
/ data space. Return the actual number of bytes
/ moved.
/
/ ffcopy(src, dst, n)
/ char far *src;
/ char far *dst;
/ int n;
/
////////
	.globl	ffcopy
ffcopy:
	call	start_copy

	mov	4(%edx),%esi		/ esi
	mov	8(%edx),%ebx		/  ds
	mov	12(%edx),%edi		/ edi
	mov	16(%edx),%eax		/  es
	mov	20(%edx),%ecx

	movw	%ax,%es			/ now load segment regs
	movw	%bx,%ds

	sar	$2,%ecx
	je	ff01

	rep
	movsl

ff01:	mov	%ss:20(%edx),%ecx
	andl	$3,%ecx
	je	ff01

	rep
	movsb

ff02:	mov	%ss:20(%edx),%eax
	jmp	end_copy		/ Return

////////
/
/ Read a byte from a selector and offset.
/
/ ffbyte(off, sel)
/ unsigned long sel;
/ unsigned long off;
/
////////
	.globl	ffbyte
ffbyte:
	call	start_copy
	push	%es
	sub	%eax,%eax
	mov	4(%edx), %ecx
	mov	8(%edx), %es
	movb	%es:(%ecx), %al
	pop	%es
	jmp	end_copy

////////
/
/ write a byte using a selector and offset.
/
/ sfbyte(off, sel, byte)
/ unsigned long sel;
/ unsigned long off;
/ int byte;
/
////////
	.globl	sfbyte
sfbyte:
	call	start_copy
	push	%es
	mov	4(%edx), %ecx
	mov	8(%edx), %es
	mov	12(%edx), %eax
	movb	%al, %es:(%ecx)
	pop	%es
	jmp	end_copy

////////
/
/ Read a word from a selector and offset.
/
/ ffword(off, sel)
/ unsigned long sel;
/ unsigned long off;
/
////////
	.globl	ffword
ffword:
	call	start_copy
	push	%es
	sub	%eax,%eax
	mov	4(%edx), %ecx
	mov	8(%edx), %es
	movw	%es:(%ecx), %ax
	pop	%es
	jmp	end_copy

////////
/
/ write a word using a selector and offset.
/
/ sfword(off, sel, word)
/ unsigned long sel;
/ unsigned long off;
/ int word;
/
////////
	.globl	sfword
sfword:
	call	start_copy
	push	%es
	mov	4(%edx), %ecx
	mov	8(%edx), %es
	mov	12(%edx), %eax
	movw	%ax, %es:(%ecx)
	pop	%es
	jmp	end_copy

/	startup routine for n-element copy

start_copy:
	pop	%eax
	movl	%esp,%edx
	push	%esi
	push	%edi
	push	%ds
	push	%es
/	lidt	%cs:bdtmap
	ijmp	%eax

///////

/ The n-element copy routines jump here
/ with the stack untouched, if they detect
/ a bounds error on a user address.

///////
__xtrap_break__:

/	add	$16,%esp		/ pop error code, IP, CS, PSW
/	movb	$EFAULT,%ss:u+U_ERROR	/ Bad parameter error
	subl	%eax,%eax		/ Return 0 to indicate error condition.

/	cleanup routine for n-byte copy

end_copy:
/	lidt	%cs:idtmap
	pop	%es
	pop	%ds
	pop	%edi
	pop	%esi
	ret				/ Return

__xtrap_off__:				/ See __xtrap_on__ above.

/	the following four routines are used by [386/fakedma.c]
/	and will disappear when a 386 assembler implementation
/	is available

/	clearseg_b(nbytes, vaddr_t p, long fill)	(byte clear)
/	clearseg_d(nbytes, vaddr_t p, long fill)	(double word clear)


clearseg_b:				/ setspace in caller
	mov	%esp,%edx
	push	%edi
	push	%es

	mov	8(%edx),%edi
	mov	$SEG_386_KD,%eax
	movw	%ax,%es
	mov	12(%edx),%eax
	mov	4(%edx),%ecx

	cld
	rep
	stosb

	pop	%es
	pop	%edi
	ret

clearseg_d:				/ setspace in caller
	mov	%esp,%edx
	push	%edi
	push	%es

	mov	8(%edx),%edi
	mov	$SEG_386_KD,%eax
	movw	%ax,%es
	mov	12(%edx),%eax		/ mov eax,2(dx)
	mov	4(%edx),%ecx
	sar	$2,%ecx			/ char to long

	cld
	rep
	stosl

	pop	%es
	pop	%edi
	ret

/	copyseg_b(nbytes, p, q)				(byte copy)
/	copyseg_d(nbytes, p, q)				(double word copy)

/	copy the user page @ virtual address "p" to virtual address "q"

copyseg_d:				/ setspace in caller
	movl	%esp,%edx
	push	%esi
	push	%edi

	mov	8(%edx),%esi
	mov	12(%edx),%edi
	mov	4(%edx),%ecx 
	sar	$2,%ecx

	cld
	rep
	movsl

	pop	%edi
	pop	%esi
	ret

copyseg_b:				/ setspace in caller
	movl	%esp,%edx
	push	%esi
	push	%edi

	mov	8(%edx),%esi
	mov	12(%edx),%edi
	mov	4(%edx),%ecx 

	cld
	rep
	movsb

	pop	%edi
	pop	%esi
	ret


/	seg2io(long nbytes, vaddr_t p, long port)

/	nbytes must be a short word multiple

seg2io:					/ setspace in caller
	movl	%esp,%edx
	push	%esi

	mov	4(%edx),%ecx
	mov	8(%edx),%esi
	mov	12(%edx),%edx		/ mov edx,2(dx)
	sar	$1,%ecx			/ char to short

	cld
	rep
	outsw

	pop	%esi
	ret



/	io2seg(long nbytes, vaddr_t p, long port)

/	nbytes must be a short word multiple

io2seg:					/ setspace in caller
	mov	%esp,%edx
	push	%edi

	mov	4(%edx),%ecx
	mov	8(%edx),%edi
	mov	12(%edx),%edx		/ mov edx,2(dx)
	sar	$1,%ecx			/ char to short

	cld
	rep				/ Value of the ECX register is not
/	insw	(%dx)			/ updated correctly
	insw

	pop	%edi
	ret


///////
/
/ Profile scaling - special multiply routine is used for speed.
/
/ pscale(a,b) is product a*b shifted right 16 bits
/
///////

	.globl	pscale
pscale:
	mov	4(%esp),%eax	/ fetch first argument
	mull	8(%esp)		/ unsigned multiply by second argument
	shrd	$16,%edx,%eax	/ shift 64-bit product right 16 bits
	ret

///////

/ Trap an interrupt linkage.
/ Each of the machine traps has a special little
/ linkage, that sets up the type code and sends
/ control off to the common trap processor. Device
/ interrupts, other than the clock (IR0), are
/ done here.

///////

trap0:
	push	$0x00			/ Divide error.
	call	tsave
	jmp	trap

/ The debug vector is tricky.
/
/ If single stepping user code, the vector must point into Ring 1 code
/ so that a ptraced child can be synchronized with its parent.
/	use trap1_usr for this
/
/ If single stepping the kernel, the vector must point into Ring 0 code
/ so context switches switch out the debug stack frame.
/	use trap1_ker for this

	.globl	__debug_usr__
trap1_usr:
	push	$0x01			/ Single step.
	call	tsave
	jmp	__debug_usr__

	.globl	__debug_ker__
trap1_ker:
	push	$0x01			/ Single step.
	call	tsave0
	jmp	__debug_ker__

trap2:
	push	$0x02			/ Non-maskable interrupt.
	call	tsave
	jmp	trap

trap3:
	push	$0x03			/ INT 3 (breakpoint).
	call	tsave
	jmp	trap

trap4:
	push	$0x04			/ Overflow.
	call	tsave
	jmp	trap

trap5:
	push	$0x05			/ Bound check.
	call	tsave
	jmp	trap

trap6:
	push	$0x06			/ Invalid opcode.
	call	tsave
	jmp	trap

trap7:
	push	$0x07			/ Processor Extension not available.
	call	tsave
	jmp	emtrap

trap8:
/	pop	%ss:trapcode		/ Get error code from stack [always 0]
	add	$4,%esp
	push	$0x08			/ Double Exception detected
	call	tsave
	jmp	trap

trap9:
	push	$0x09			/ Processor extension segment overrun
	call	tsave
	jmp	trap

trap10:
/	pop	%ss:trapcode		/ Get error code from stack
	add	$4,%esp
	push	$0x0A			/ Invalid task state segment
	call	tsave
	jmp	trap

trap11:
/	pop	%ss:trapcode		/ Get error code from stack
	add	$4,%esp
	push	$0x0B			/ Segment not present
	call	tsave
	jmp	trap

trap12:
/	pop	%ss:trapcode		/ Get error code from stack
	add	$4,%esp
	push	$0x0C			/ Stack segment overrun or not present
	call	tsave
	jmp	trap

trap13:
/	pop	%ss:trapcode		/ Get error code from stack
/	add	$4,%esp
/	push	$0x0D			/ General protection
	call	tsave0
	jmp	gpfault

trap14:
/	pop	%ss:trapcode		/ Get error code from stack
/	add	$4,%esp
/	push	$0x0E			/ Page Fault
	call	tsave
	jmp	pagefault

trap16:
	push	$0x10			/ Floating point error
	call	tsave
	jmp	fptrap

syc:
	push	$0x22			/ Old format system calls.
	call	tsave
	jmp	trap

	.set	FAKE_EFL,12
syc32:
	push	%eax			/ save %eax
	pushf				/ modify current flags
	pop	%eax
	orw    	$PSW_VAL,%ax		/ set IF=1, IOPL=1 (user) on iret
	mov	%eax,FAKE_EFL(%esp)
	pop	%eax			/ restore %eax
	push	$0x20			/ New format system calls.
	call	tsave
	jmp	trap

sig32:
	push	%eax
	pushf
	pop	%eax
	orw    	$PSW_VAL,%ax
	mov	%eax,FAKE_EFL(%esp)
	pop	%eax
	push	$0x20			/ New format signal return.
	call	tsave
	jmp	msigend

ran:
	push	$0x21			/ Random trap.
	call	tsave
	jmp	trap

dev1:
	push	$0x0140			/ Device 1: keyboard
	call	tsave
	icall	[1<<2]+vecs
	jmp	eoi			/ Dismiss interrupt

					/ Device 2: mapped into device 9
dev3:
	push	$0x0340			/ Device 3: al1
	call	tsave
	icall	[3<<2]+vecs
	jmp	eoi			/ Dismiss interrupt

dev4:
	push	$0x0440			/ Device 4: al0
	call	tsave
	icall	[4<<2]+vecs
	jmp	eoi			/ Dismiss interrupt

dev5:
	push	$0x0540			/ Device 5: hard disk
	call	tsave
	icall	[5<<2]+vecs
	jmp	eoi			/ Dismiss interrupt

dev6:
	push	$0x0640			/ Device 6: floppy
	call	tsave
	icall	[6<<2]+vecs
	jmp	eoi			/ Dismiss interrupt

dev7:
	push	$0x0740			/ Device 7: lp
	call	tsave
	icall	[7<<2]+vecs
	jmp	eoi			/ Dismiss interrupt

dev8:
	push	$0x0840			/ Device 8:
	call	tsave
	icall	[8<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

dev9:
	push	$0x0940			/ Device 9:
	call	tsave
	icall	[9<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

dev10:
	push	$0x0A40			/ Device 10:
	call	tsave
	icall	[10<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

dev11:
	push	$0x0B40			/ Device 11:
	call	tsave
	icall	[11<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

dev12:
	push	$0x0C40			/ Device 12:
	call	tsave
	icall	[12<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

	.align	4
dev13:
	/ Used to be coprocessor exception interrupt
	/ Coprocessor err had to be cleared by writing a 0 byte to port 0xF0
	/
	push	$0x0D40			/ Device 13:
	call	tsave
	icall	[13<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

dev14:
	push	$0x0E40			/ Device 14:
	call	tsave
	icall	[14<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

dev15:
	push	$0x0F40			/ Device 15:
	call	tsave
	icall	[15<<2]+vecs
	jmp	eoi2			/ Dismiss interrupt

///////

/ Clock interrupt.

///////

clk:
	push	$0x0040
	call	tsave			/ Perform trap save.
	mov	X_ERR+12(%esp),%eax	/ ECS at tick time
	and	$3,%eax			/ This will be R_USR iff user mode
	push	%eax
	mov	X_ERR+12(%esp),%eax	/ EIP at tick time
	push	%eax
	call	clock			/ clock(eip, umode)
	add	$8,%esp			/ pop arguments.
	jmp	eoi			/ Dismiss interrupt

///////

/ This co-routine is called to dismiss an interrupt.
/ The interrupt code is in X_ERR(%esp)

/ Control returns to "tsave"

///////

	.globl	eoi2
eoi2:
	cli
	movb	$0x20,%al	/ Send a non specific EOI
	outb	$SPIC		/ to the slave PIC.
	IODELAY
	movb	$0x0B,%al	/ OCW3 - read isr
	outb	$SPIC
	IODELAY
/	inb	$SPIC		/ in-service register to %eax:8..15
/	testb	%al,%al
/	jnz	eoi2x		/ no EOI to master if slave isr nonzero
eoi:
	cli
	movb	$0x20,%al	/ Send a non specific EOI
	outb	$PIC		/ to the master PIC.
	IODELAY
eoi2x:	ret			/ Done.

///////

/ Read the equipment description. Use
/ the "int 11" interface, so that the IBM
/ ROM will do all the details.

///////

int11:	mov	%cs:val11,%eax		/ Ask the ROM
	ret				/ to put stuff in AX

///////

/ Bootstrap.
/ Called by the keyboard driver on control-alt-del.
/ Requests the 8042 controller to initiate a processor reset,
/ which is the only way to terminate protected mode operation.

/	Reference: IBM-AT Technical Reference Manual,
/			Real-time Clock/CMOS RAM [Page 1-45]
/			Keyboard controller [Page 1-40]
/			Test 3, Page 5-68.

///////

boot:
	cli				/ Disable interrupts.

	subl	%ecx,%ecx
loc12:	inb	$KBCTRL			/ Wait for 8042 input buffer to empty.
	testb	$2, %al
	loopne	loc12
	IODELAY

	movb	$0xFE,%al		/ Issue a shutdown command
	outb	$KBCTRL			/ to the 8042 control port.

loc13:	hlt				/ Halt until processor reset occurs.
	jmp	loc13

	.globl	putchar

/	Comment in the line below if debugging output is to go to the
/	printer

/putchar:

	movb	4(%esp),%al
	cmpb	$0xa,%al
	jne	loc18
	push	$0xd
	call	putchar
	add	$4,%esp
loc18:	mov	$LPSTAT,%edx
	inb	(%dx)
	testb	$IBMNBSY,%al
	je	loc18

	mov	$LPCSR,%edx
	movb	$SEL+NINIT, %al
	outb	(%dx)

	mov	$LPDATA, %edx
	movb	4(%esp),%al
	outb	(%dx)

	mov	$LPCSR,%edx
	movb	$SEL+NINIT+STROBE,%al 
	outb	(%dx)
	movb	$8, %cl
l_1:	decb	%cl
	jne	l_1
	movb	$SEL+NINIT, %al
	outb	(%dx)
	ret

/ long _canl(l) long l;
/ This is called by the routines that
/ transform longs to and from the
/ canonical formats.

_canl:
	mov	4(%esp),%eax
	rol	$16,%eax
	ret

regcr2:	mov	%cr2,%eax
	ret

regfp:	mov	%ebp,%eax
	ret

	.align	4		/ CPU resets if val11 isn't aligned.
val11:	.long	0		/ Value obtained from int11 [in code].

aicodep:
	sub	%ebx,%ebx
	sub	$aicodep,%ebx
	lea	fn(%ebx),%eax
	mov	%eax,argl(%ebx)
	lea	a1(%ebx),%eax
	mov	%eax,argl+4(%ebx)
	lea	argl+8(%ebx),%eax	/ No environment
	push	%eax
	lea	argl(%ebx),%eax		/ Argument list
	push	%eax
	lea	fn(%ebx),%eax		/ File name
	push	%eax
	sub	$4,%esp			/ Dummy word for exec
	movl	$59, %eax
	lcall	$0x7,$0
	jmp	.			/ Instant page fault if exec failed!
	.alignoff
	.align	2
argl:	.long	0			/ argv[0] = "/etc/init";
	.long	0			/ argv[1] = "";
	.long	0			/ argv[2] = NULL;

fn:	.byte	"/etc/init",0
a1:	.byte	0
sb:
	.set	aicodes, .-aicodep

///////

/ Task State Segment - Coherent runs as a single protected mode 386 task.

///////
	.alignon
	.align	4
	.globl	tss_sp0		/ Use run-time fixup for tss_sp0
	.globl	tssIoMap
	.globl	tssIoEnd
tss:				/ Task State Segment.
tss_lnk:.long	0		/  0: Back link selector to TSS.
tss_sp0:.long	ESP0_START	/  4: SP for CPL 0.
tss_ss0:.long	SEG_RNG0_STK	/  8: SS for CPL 0.
tss_sp1:.long	ESP1_START	/  C: SP for CPL 1.
tss_ss1:.long	SEG_RNG1_STK	/ 10: SS for CPL 1.
tss_sp2:.long	u+NBPC		/ 14: SP for CPL 2.
tss_ss2:.long	SEG_386_KD	/ 18: SS for CPL 2.
tss_cr3:.long	PTABLE0_P<<BPCSHIFT / 1C: CR3 (PDBR)
tss_ip:	.long	0		/ 20: EIP (Entry point).
tss_psw:.long	0		/ 24: Flag word.
tss_ax:	.long	0		/ 28: Register AX.
tss_cx:	.long	0		/ 2C: Register CX.
tss_dx:	.long	0		/ 30: Register DX.
tss_bx:	.long	0		/ 34: Register BX.
tss_bp:	.long	0		/ 38: Register BP.
tss_sp:	.long	0		/ 3C: Register SP.
tss_si:	.long	0		/ 40: Register SI.
tss_di:	.long	0		/ 44: Register DI.
tss_es:	.long	0		/ 48: Register ES.
tss_cs:	.long	0		/ 4C: Register CS.
tss_ss:	.long	0		/ 50: Register SS.
tss_ds:	.long	0		/ 54: Register DS.
tss_fs:	.long	0		/ 58: Register FS.
tss_gs:	.long	0		/ 5C: Register GS.
tss_ldt:.long	SEG_LDT		/ 60: Task LDT Selector.
	.long	TSS_IOMAP_OFF	/ 64: T bit & I/O map base
/ I/O map is part of tss.
/ Bitmap up to port address TSS_IOMAP_LEN.
/ Initialize to all 1's, meaning no I/O allowed.
/ tss + 0x68 = tssIoMap
tssIoMap:
	.long	[TSS_IOMAP_LEN .div 32] # -1
tssIoEnd:
	.long	-1
///////

/ Data.

///////
	.data
sdata:

vecs:	.long	16 # vret	/ Interrupt vector table

trapcode:.long	0

	.text
///////

/ i8086 coherent clist hack.

///////

/LXXX:	.long	NCPCL+4

/cltinit:
/	pushf			/ s = sphi()
/	cli
/	mov	NCLIST,%eax
/	imull	LXXX,%eax
/	addl	clistp,%eax
/	sub	%ecx,%ecx
/	jmp	loc32

/loc33:	mov	%ecx,(%eax)
/	mov	%eax,%ecx
/loc32:	sub	$NCPCL+4, %eax
/	cmp	clistp,%eax
/	jnb	loc33

/	mov	%ecx,cltfree
/	call	spl
/	add	$0x04,%esp
/	ret

/getq:
/	push	%esi

/	mov	8(%esp),%edx
/	sub	%ecx,%ecx
/	cmp	%ecx,(%edx)	/ if (cqp->cq_cc==0)
/	je	loc21

/	pushf			/ s = sphi()
/	cli
/	mov	12(%edx),%esi	/ op = cqp->cq_op	[%esi]
/	mov	16(%edx),%eax	/ ox = cqp->cq_ox	[%eax]
/	movb	4(%eax,%esi),%cl
/	push	%ecx		/ save = op->cl_ch[ox]
/	decl	(%edx)		/ if (--cqp->cq_cc == 0)
/	je	loc23
/	inc	%eax		/ ++ox
/	mov	%eax,16(%edx)	/ cqp->cq_ox = ox
/	cmp	$NCPCL,%eax	/ if (ox == NCPL)
/	jne	loc24

/loc23:	sub	%eax,%eax
/	mov	%eax,16(%edx)	/ cqp->cq_ox = 0
/	mov	(%esi),%ecx	/ np = op->cl_fp
/	mov	%ecx,12(%edx)	/ cqp->cq_op = np
/	cmp	%eax,%ecx	/ if (np==0)
/	jne	loc25
/	mov	%eax,4(%edx)		/ cqp->cq_ip = 0
/	mov	%eax,8(%edx)		/ cqp->cq_ix = 0

/loc25:	mov	cltfree,%ecx
/	mov	%ecx,(%esi)	/ op->cl_fp = cltfree
/	mov	%esi,cltfree	/ cltfree = op
/	cmp	%eax,cltwant	/ if (cltwant)
/	je	loc24
/	mov	%eax,cltwant		/ cltwant = 0
/	push	$cltwant
/	call	wakeup			/ wakeup(&cltwant)
/	pop	%eax

/loc24:	pop	%esi
/	call	spl		/ spl(s)
/	pop	%eax
/	mov	%esi,%eax	/ return save
/	pop	%esi
/	ret
/loc21:
/	mov	$-1,%eax			/ return -1
/	pop	%esi
/	ret

/putq:
/	push	%esi
/	sub	%eax,%eax
/	pushf			/ s = sphi();
/	cli
/	mov	12(%esp),%edx	/ ebp = cqp
/	mov	4(%edx),%esi	/ ip = cqp->cq_ip	[%esi]
/	mov	8(%edx),%ecx	/ ix = cqp->cq_ix	[%ecx]
/	cmp	%eax,%ecx	/ if (ix==0) {
/	jne	loc26
/	mov	cltfree,%esi		/ ip = cltfree
/	cmp	%eax,%esi		/ if (ip==0)
/	je	loc27			/ goto bad;
/	mov	(%esi),%ecx
/	mov	%ecx,cltfree		/ cltfree = cltfree->cl_fp
/	mov	%eax,(%esi)		/ ip->cl_fp = 0
/	mov	4(%edx),%ecx		/ np = cqp->cq_ip
/	cmp	%eax,%ecx		/ if (np==0)
/	jne	loc29
/	mov	%esi,12(%edx)			/ cqp->cq_op = ip
/	jmp	loc30
/					/ else
/loc29:	mov	%esi,(%ecx)			/ np->cl_fp = ip

/loc30:	mov	%esi,4(%edx)		/ cqp->cq_ip = ip
/	mov	%eax,%ecx		/ ix = 0
/				/ }

/loc26:
/	movb	16(%esp),%al	/ ip->cl_ch[ix] = c
/	movb	%al, 4(%esi,%ecx) 
/	inc	%ecx		/ ix++
/	cmp	$NCPCL,%ecx	/ if (ix==NCPCL)
/	jne	loc31
/	sub	%ecx,%ecx		/ ix = 0

/loc31:	mov	%ecx,8(%edx)	/ cqp->cq_ix = ix
/	incl	(%edx)		/ cqp->cq_cc++
/	call	spl		/ spl(s)
/	add	$4,%esp
/	mov	12(%esp),%eax	/ return (c)
/loc28:
/	pop	%esi
/	ret
/loc27:
/	call	spl		/ 		spl(s)
/	add	$4,%esp
/	mov	$-1,%eax	/ 		return -1
/	jmp	loc28

/clrq:
/	mov	4(%esp),%edx
/	pushf
/	cli

/loc34:	push	%edx
/	call	getq
/	pop	%edx
/	or	%eax,%eax
/	jge	loc34

/	call	spl
/	pop	%eax
/	ret

/loc35:	movl	$0x01,cltwant
/	push	%eax
/	push	%eax
/	push	$0x0100
/	push	$cltwant
/	call	sleep
/	add	$16,%esp
/waitq:
/	sub	%eax,%eax
/	cmp	%eax,cltfree
/	jne	loc35
/	ret

///////

/ atbsyw()	-- wait for AT disk controller to become not busy

/	Return:	0 = timeout
/		* = not busy

///////
/atbsyw:
/	mov	$0x3FFFF, %ecx 
/	mov	ATSREG, %edx
/loc16:	inb	(%dx)
/	testb	$BSY_ST, %al
/	loopne	loc16
/	mov	%ecx, %eax 
/	ret

///////

/ AT Hard Disk Assembler Support
/ atbsyw()	     - wait while controller is busy
/ atdrqw()	     - wait for controller to request data transfer

///////
///////

/ atdrqw()	-- wait for AT disk controller to initiate data request

/	Return:	0 = timeout
/		* = data requested

///////

/atdrqw:
/	mov	$0x3FFFF, %ecx
/	mov	ATSREG, %edx 
/loc17:	inb	(%dx)
/	testb	$DRQ_ST, %al 
/	loope	loc17
/	mov	%ecx, %eax 
/	ret

/       Read a byte from the CMOS.  Takes one argument--the
/       CMOS address to read from as an int; returns the
/       value read as a char.
/
/	int read_cmos(int addr);

read_cmos:
	push	%esi
	push	%edi
        movb    12(%esp), %al	/ Fetch address from stack.
        outb    $CMOSA		/ Send address to CMOS.
	IODELAY
	sub	%eax, %eax	/ Zero out everything we don't want.
        inb     $CMOSD		/ Get Value from CMOS into al.
	pop	%edi
	pop	%esi
        ret                     / Return from read_cmos().

/       Write a byte to the CMOS.
/
/	void write_cmos(int addr, int data)

write_cmos:
	push	%esi
	push	%edi
        movb    12(%esp), %al	/ Fetch address from stack.
        outb    $CMOSA		/ Send address to CMOS.
	IODELAY
        movb    16(%esp), %al	/ Fetch address from stack.
        outb     $CMOSD		/ Get Value from CMOS into al.
	IODELAY
	pop	%edi
	pop	%esi
        ret                     / Return from read_cmos().

/ Read timer channel 0 into int value.  
/ Clock counts down from 11932 to 0 with each clock tick.
	.globl	read_t0
read_t0:
	pushfl
	cli
	xorl	%eax,%eax	/ Counter latch timer 0 and clear return val
	outb	$PIT+3
	IODELAY
	inb	$PIT		/ low byte of counter latch
	IODELAY
	movb	%al,%ah
	inb	$PIT		/ high byte of counter latch
	IODELAY
	xchgb	%al,%ah
	popfl
	ret

/ return current contents of psw
	.globl	read_psw
read_psw:
	pushfl
	popl	%eax
	ret

/ Read master PIC state
/ return 00:xx:yy:zz 4-byte int value
/	xx: interrupt mask
/	yy: isr
/	zz: irr

/	.globl	mchirp
/FOO	.macro	ch
/	push	ch
/	call	mchirp
/	add	$4,%esp
/	.endm

/	.globl	rd_m_pic
/rd_m_pic:
/	pushfl
/	cli
/	sub	%eax,%eax
/	inb	$PICM		/ interrupt mask to %eax:16..23
/	shl	$8,%eax
/	movb	$0x0B,%al	/ OCW3 - read isr
/	outb	$PIC
/	IODELAY
/	inb	$PIC		/ in-service register to %eax:8..15
/	shl	$8,%eax
/	movb	$0x0A,%al	/ OCW3 - read irr
/	outb	$PIC
/	IODELAY
/	inb	$PIC		/ irpt request register to %eax:0..7
/	popfl
/	ret

/ Read slave PIC state
/ return 00:xx:yy:zz 4-byte int value
/	xx: interrupt mask
/	yy: isr
/	zz: irr

/	.globl	rd_s_pic
/rd_s_pic:
/	pushfl
/	cli
/	sub	%eax,%eax
/	inb	$SPICM		/ interrupt mask to %eax:16..23
/	shl	$8,%eax
/	movb	$0x0B,%al	/ OCW3 - read isr
/	outb	$SPIC
/	IODELAY
/	inb	$SPIC		/ in-service register to %eax:8..15
/	shl	$8,%eax
/	movb	$0x0A,%al	/ OCW3 - read irr
/	outb	$SPIC
/	IODELAY
/	inb	$SPIC		/ irpt request register to %eax:0..7
/	popfl
/	ret

/ return current contents of cr0
	.globl	read_cr0
read_cr0:
	movl	%cr0,%eax
	ret

/ return current contents of cr2
	.globl	read_cr2
read_cr2:
	movl	%cr2,%eax
	ret

/ return current contents of cr3
	.globl	read_cr3
read_cr3:
	movl	%cr3,%eax
	ret

/////////
/
/ Debugging support.
/
/////////
	.globl	write_dr0
	.globl	write_dr1
	.globl	write_dr2
	.globl	write_dr3
	.globl	write_dr6
	.globl	write_dr7

	.globl	read_dr0
	.globl	read_dr1
	.globl	read_dr2
	.globl	read_dr3
	.globl	read_dr6
	.globl	read_dr7

/ write arg to dr0
write_dr0:
	movl	4(%esp),%eax
	movl	%eax,%dr0
	ret

/ write arg to dr1
write_dr1:
	movl	4(%esp),%eax
	movl	%eax,%dr1
	ret

/ write arg to dr2
write_dr2:
	movl	4(%esp),%eax
	movl	%eax,%dr2
	ret

/ write arg to dr3
write_dr3:
	movl	4(%esp),%eax
	movl	%eax,%dr3
	ret

/ write arg to dr6
write_dr6:
	movl	4(%esp),%eax
	movl	%eax,%dr6
	ret

/ write arg to dr7
write_dr7:
	movl	4(%esp),%eax
	movl	%eax,%dr7
	ret

read_dr0:
	movl	%dr0,%eax
	ret

read_dr1:
	movl	%dr1,%eax
	ret

read_dr2:
	movl	%dr2,%eax
	ret

read_dr3:
	movl	%dr3,%eax
	ret

read_dr6:
	movl	%dr6,%eax
	ret

read_dr7:
	movl	%dr7,%eax
	ret

/ write to the EM bit of CR0
/ this routine is a stub for the ring 0 code
/ argument is 0 or 1
/
/	void setEm(int bit)
	.globl	setEm
setEm:
	movl	4(%esp),%eax	/ fetch argument
	pushf
	cli
	pushl	%eax
	lcall	$SEG_SET_EM,$0	/ gate to setEmfR0
	/ setEmfR0 will delete 4 bytes worth of args
	popf
	ret

/ Ring 0 write to CR0 EM bit.  Called via a gate.
/ Want interrupts off when we arrive since the interrupt gates
/ lead into Ring 1.
setEmfR0:
	movb	8(%esp),%cl	/ fetch argument

	cmpb	$0,%cl
	movl	%cr0,%eax
	jz	se00
	orb	$4,%al		/ set EM bit
	andb	$0xDF,%al	/ clear NE bit
	jmp	se01
se00:
	andb	$0xFB,%al	/ clear EM bit
	orb	$0x20,%al	/ set NE bit
se01:
	mov	%eax,%cr0
	/ make 4-byte arg list disappear
	lret	$4

/ return nonzero if paging is turned on
	.globl	paging
paging:
	movl	(%esp),%eax		/ fetch return address
	cmpl	$[SBASE<<BPCSHIFT],%eax	/ is it >= unsigned FFC0_0000?
	jae	pagingMaybe
	xorl	%eax,%eax		/ if not, no paging
	ret
pagingMaybe:
	movw	%cs,%ax			/ if return addr high, cs is a selector
	cmpw	$0x58,%ax		/ selectors 58-6F are nonpaging
	jb	pagingYes
	cmpw	$0x6F,%ax		/ selectors 58-6F are nonpaging
	ja	pagingYes
	xorl	%eax,%eax		/ no paging
	ret
pagingYes:
	movl	$1,%eax
	ret
