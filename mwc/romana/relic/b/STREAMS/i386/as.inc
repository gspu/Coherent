	.globl	_canl
	.globl	aicodep		/ Position of code.
	.globl	aicodes		/ Size of code.
	.globl	boot
	.globl	clk
	.globl	clrq
	.globl	cltinit
	.globl	conrest
	.globl	consave
	.globl	cs_sel
	.globl	dev1
	.globl	dev10
	.globl	dev11
	.globl	dev12
	.globl	dev13
	.globl	dev14
	.globl	dev15
	.globl	dev3
	.globl	dev4
	.globl	dev5
	.globl	dev6
	.globl	dev7
	.globl	dev8
	.globl	dev9
	.globl	envrest
	.globl	envsave
	.globl	eoi
	.globl	gdt
	.globl	gdtinit
	.globl	gdtmap
	.globl	getubd
	.globl	getupd
	.globl	getusd
	.globl	getuwd
	.globl	halt
	.globl	idt
	.globl	idtend
	.globl	idtmap
	.globl	inb
	.globl	int11
	.globl	io2seg
	.globl	kucopy
	.globl	ldt
	.globl	ldtend
	.globl	mmuupd
	.globl	mmuupdfR0
	.globl	mmuupdnR0
	.globl	msysgen
	.globl	outb
	.globl	putubd
	.globl	putusd
	.globl	putuwd
	.globl	putuwi
	.globl	read_cmos
	.globl	regcr2
	.globl	regfp
	.globl	sdata
	.globl	seg2io
	.globl	setEmfR0
	.globl	setspace
	.globl	sig32
	.globl	sphi
	.globl	spl
	.globl	splo
	.globl	stext
	.globl	syc
	.globl	syc32
	.globl	trap0
	.globl	trap1_ker
	.globl	trap1_usr
	.globl	trap10
	.globl	trap11
	.globl	trap12
	.globl	trap13
	.globl	trap14
	.globl	trap16
	.globl	trap2
	.globl	trap3
	.globl	trap4
	.globl	trap5
	.globl	trap6
	.globl	trap7
	.globl	trap8
	.globl	trap9
	.globl	trapcode
	.globl	tsave
	.globl	tss
	.globl	u
	.globl	ukcopy
	.globl	vecs
	.globl	vret
	.globl	waitq
	.globl	write_cmos

	.set	BPC1SHIFT,	10	/ BPCSHIFT - log2(sizeof(long))
	.set	BPCSHIFT,	12
	.set	BSY_ST,		0x80
	.set	CMOSA,		0x70	/ Real-time Clock/CMOS addr I/O port.
	.set	CMOSD,		0x71	/ Real-time Clock/CMOS data I/O port.
	.set	DRQ_ST,		0x08
	.set	EFAULT,		14	/ Bad argument
	.set	IBMNBSY,	0x80
	.set	JMPF,		0xEA	/ Jump far, direct.
	.set	KBCTRL,		0x64	/ 8042 keyboard mpu ctrl I/O port.
	.set	KBDATA,		0x60	/ 8042 keyboard mpu data I/O port.
	.set	LPDATA,		0x378
	.set	NBPC,		4096
	.set	NCPCL,		124	/ # of characters in a clist block
	.set	NIDT,		256
	.set	NINIT,		0x04
	.set	PBASE,		0x00002
	.set	PIC,		0x20	/ 8259 CSR  I/O port.
	.set	PICM,		0x21	/ 8259 IMR  I/O port.
	.set	PIT,		0x40	/ 8253 base I/O port.
	.set	PTABLE0_P,	0x00001
	.set	PTABLE1_V,	0xFF800
	.set	PX_ADDR,	0x67	/ displacements
	.set	PX_OPND,	0x66	/ register values & immediates
	.set	R_USR,		3	/ user level ring number
	.set	SBASE,		0xFFC00
	.set	TSS_IOMAP_LEN,	0x0800	/ number of port addresses mapped
	.set	TSS_IOMAP_OFF,	0x68	/ byte offset of io bitmap in tss
	.set	VIDEO,		0x10	/ Video BIOS interrupt for early debugging.

	.set	LPSTAT,		LPDATA+1
	.set	LPCSR,		LPDATA+2

////
/
/ Selector index values.
/
////
	.set	SEG_386_UI,	0x08	/ 32 bit code segment descriptor (user)
	.set	SEG_386_UD,	0x10 	/ 32 bit data segment descriptor (user)
	.set	SEG_386_KI,	0x18	/ 32 bit code segment descriptor (kernel)
	.set	SEG_RNG0_TXT,	0x18	/ synonym for SEG_386_KI
	.set	SEG_386_KD,	0x21	/ 32 bit data segment descriptor (kernel)
	.set	SEG_286_UI,	0x28 	/ 16 bit code segment descriptor (user)
	.set	SEG_286_UD,	0x30 	/ 16 bit data segment descriptor (user)
	.set	SEG_TSS,	0x38 	/ task segment descriptor (386 mode)
	.set	SEG_ROM,	0x40	/ ROM descriptor
	.set	SEG_VIDEOa,	0x48	/ 0x48: video descriptor @ B0000
	.set	SEG_VIDEOb,	0x50	/ 0x50: video descriptor @ B8000
	.set	SEG_386_II,	0x58	/ 32 bit code segment (kernel init)
	.set	SEG_386_ID,	0x60	/ 32 bit data segment (kernel init)
	.set	SEG_286_UII,	0x68 	/ 16 bit code, sep I/D (user)
	.set	SEG_LDT,	0x70	/ local descriptor table index in GDT
	.set	SEG_RNG0_STK,	0x78	/ Stack for ring 0 CPL
	.set	SEG_RNG1_TXT,	0x81	/ Text for ring 1
	.set	SEG_RNG1_STK,	0x89	/ Stack for ring 1 CPL
	.set	SEG_MMUUPD,	0x91	/ call gate into ring 0
	.set	SEG_SET_EM,	0x99	/ call gate into ring 0

	.set	SEG_SRW,	0x03

	.set	SEL,	0x08
	.set	SPIC,		0xA0	/ Slave 8259 CSR I/O port.
	.set	SPICM,		0xA1	/ Slave 8259 IMR I/O port.
	.set	STROBE,	0x01
	.set	UADDR,		0x3FFFFC
	.set	U_ERROR,	2	/ Offset into u for u_error
	.set	X_ERR,		52
	.set	X_TRAPNO,	48
