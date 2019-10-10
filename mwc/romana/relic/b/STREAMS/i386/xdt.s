	.unixorder
	.llen	132
	.include	as.inc
	.text
	.alignoff
	.align	4

/	the init gdt is in the text segment since the data segment
/	is relocated after we switch into 386 mode (when a
/	valid gdt is required)

gdtinit:				/ used before turning on paging
	.value	gdtend-gdt-1		/ limit of gdt
					/ gdt physical addr is 0x0040_2000+gdt
	.long	[[[-SBASE]+PBASE]<<BPCSHIFT]+gdt
	.value	0
gdtmap:					/ used after paging is enabled
	.value	gdtend-gdt-1		/ limit of gdt
	.long	gdt
	.value	0

idtmap:					/ used after paging is enabled
	.value	2047
	.long	idt
	.value	0

////
/
/ Macro MEM_SEG specifies a memory segment descriptor.
/ base is 32 bits
/ limit is 20 bits
/ type is 4 bits
/ dpl is 2 bits
/ flags is the 4 high bits of byte at descriptor + 6
/   8's bit of flags is limit granularity (0:byte 1:click)
/   4's bit of flags is default op size (0:16 bit 1:32 bit)
/
////

MEM_SEG	.macro	base,limit,type,dpl,flags
	.value	limit
	.value	base
	.byte	[base] >> 16
	.byte	0x90 | [[[dpl] & 3] << 5] | [[type] & 0xF]
	.byte	[[[flags] << 4] & 0xF0] | [[[limit] >> 16] & 0xF]
	.byte	[base] >> 24
	.endm

////
/
/ Macro TSS_SEG specifies a tss segment descriptor.
/ base is 32 bits
/ limit is 20 bits
/ [type is 0x9]
/ [dpl is 0]
/ gran is limit granularity (0:byte 1:click)
/
////

TSS_SEG	.macro	base,limit,gran
	.value	limit
	.value	base
	.byte	[base] >> 16
	.byte	0x89
	.byte	[[[gran] << 7] & 0x80] | [[[limit] >> 16] & 0xF]
	.byte	[base] >> 24
	.endm

////
/
/ Macro LDT_SEG specifies a tss segment descriptor.
/ base is 32 bits
/ limit is 20 bits
/ [type is 0x2]
/ [dpl is 0]
/ gran is limit granularity (0:byte 1:click)
/
////

LDT_SEG	.macro	base,limit,gran
	.value	limit
	.value	base
	.byte	[base] >> 16
	.byte	0x82
	.byte	[[[gran] << 7] & 0x80] | [[[limit] >> 16] & 0xF]
	.byte	[base] >> 24
	.endm

////
/
/ Macro CALL_GATE specifies a call gate descriptor.
/ selector is 16 bits
/ offset is 32 bits
/ dwdcount is 5 bits
/ dpl is 2 bits
/
/ Would like the following, but can't shift offset since it's a symbol.
/	.value	offset
/	.value	selector
/	.value	0x8C00 | [[dwdcount] & 0x1F] | [[[dpl] & 3] << 13]
/	.value	[offset] >> 16
/
/ IMPORTANT!!!
/ This macro does not create a proper call gate.  
/ Count on idtinit() to swap 16-bit words at macro+2, macro+6.
/
////

CALL_GATE	.macro	selector,offset,dwdcount,dpl

	.long	offset
	.value	0x8C00 | [[dwdcount] & 0x1F] | [[[dpl] & 3] << 13]
	.value	selector
	.endm

////
/
/ "segment xxxx" below gives the value in a segment register corresponding
/ to the given descriptor.  The low 3 bits in a segment register are not
/ used in indexing into the descriptor table.
/
////

	.set	DPL_0,0
	.set	DPL_1,1
	.set	DPL_2,2
	.set	DPL_3,3

gdt:
	/ segment 0000
	.long	0,0			/ null entry

	/ segment 0008 - SEG_386_UI
	MEM_SEG	0,0x7FFFF,0xB,DPL_3,0xC

	/ segment 0010 - SEG_386_UD
	MEM_SEG	0,0x8FFFF,0x3,DPL_3,0xC

	/ segment 0018 - SEG_RNG0_TXT/SEG_386_KI
	MEM_SEG	0,0xFFFFF,0xB,DPL_0,0xC

	/ segment 0020 - SEG_386_KD
	MEM_SEG	0,0xFFFFF,0x3,DPL_1,0xC

	/ segment 0028 - SEG_286_UI
	MEM_SEG	0,0xF,0xB,DPL_3,0x8

	/ segment 0030 - SEG_286_UD
	MEM_SEG	0,0xF,0x3,DPL_3,0x8

	/ segment 0038 - SEG_TSS
	/ limit is 0x68 + (number of bytes in iomap)
	/ This allows the required filler byte beyond desired addressing.
	TSS_SEG	0xFFC00000,TSS_IOMAP_OFF+[TSS_IOMAP_LEN .div 8]+3,0

	/ segment 0040 - SEG_ROM
	MEM_SEG	0xFFFC0000,0xF,0x3,DPL_0,0x8

	/ segment 0048 - SEG_VIDEOa
	MEM_SEG	0xFFFB0000,0xF,0x3,DPL_1,0x8

	/ segment 0050 - SEG_VIDEOb
	MEM_SEG	0xFFFA0000,0xF,0x3,DPL_1,0x8

	/ segment 0058 - SEG_386_II	/ init code (text)
	MEM_SEG	0x400000+[PBASE<<BPCSHIFT],0xFFFFF,0xB,DPL_0,0xC

	/ segment 0060 - SEG_386_ID	/ init code (data)
	MEM_SEG	0x400000+[PBASE<<BPCSHIFT],0xFFFFF,0x3,DPL_0,0xC

	/ segment 0068 - SEG_286_UII
	MEM_SEG	0x400000,0xF,0xB,DPL_3,0x8

	/ segment 0070 - SEG_LDT
	LDT_SEG	0xFFC00000,0xF,0

	/ segment 0078 - SEG_RNG0_STK
	MEM_SEG	0,0xFFBFF,0x7,DPL_0,0xC

	/ segment 0080 - SEG_RNG1_TXT
	MEM_SEG	0,0xFFFFF,0xB,DPL_1,0xC

	/ segment 0088 - SEG_RNG1_STK
	MEM_SEG	0,0xFFFFE,0x7,DPL_1,0xC

	/ Call gates need idtinit() to fix them before they can be used.
	.globl	gdtFixBegin
	.globl	gdtFixEnd
gdtFixBegin:
	/ segment 0090 - SEG_MMUUPD - Call gate for mmu update
	CALL_GATE	SEG_RNG0_TXT,mmuupdfR0,0,DPL_1

	/ segment 0098 - SEG_SET_EM - Call gate for writing CR0 EM bit
	CALL_GATE	SEG_RNG0_TXT,setEmfR0,1,DPL_1
gdtFixEnd:
gdtend:

/	The two entries in the ldt are call gates whose format is somewhat
/	different from the other segment descriptors
/
/	BCS compatibility requires an LDT

ldt:
	/ ldt + 0000
	CALL_GATE	SEG_RNG1_TXT,syc32,1,DPL_3
/	.long	syc32				/ call gate for system call
/	.long	0xFFC0EC01

	/ ldt + 0008
/	.long	sig32				/ call gate for signal return
/	.long	0xFFC0EC01
	CALL_GATE	SEG_RNG1_TXT,sig32,1,DPL_3
ldtend:

////
/
/ Macro IRPT_GATE specifies a call gate descriptor.
/ selector is 16 bits
/ offset is 32 bits
/ dwdcount is 5 bits
/ dpl is 2 bits
/
/ Would like the following, but can't shift offset since it's a symbol.
/	.value	offset
/	.value	selector
/	.value	0x8E00 | [[dwdcount] & 0x1F] | [[[dpl] & 3] << 13]
/	.value	[offset] >> 16
/
/ IMPORTANT!!!
/ This macro does not create a proper interrupt gate.  
/ Count on idtinit() to swap 16-bit words at macro+2, macro+6.
/
////

IRPT_GATE	.macro	selector,offset,dwdcount,dpl

	.long	offset
	.value	0x8E00 | [[dwdcount] & 0x1F] | [[[dpl] & 3] << 13]
	.value	selector
	.endm

idt:
	IRPT_GATE	SEG_RNG1_TXT,trap0,0,DPL_3
/	IRPT_GATE	SEG_RNG0_TXT,trap1_ker,0,DPL_3	/ Ring 0
	IRPT_GATE	SEG_RNG1_TXT,trap1_usr,0,DPL_3	/ Ring 1
	IRPT_GATE	SEG_RNG1_TXT,trap2,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap3,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap4,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap5,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap6,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap7,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap8,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap9,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap10,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap11,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,trap12,0,DPL_3
	IRPT_GATE	SEG_RNG0_TXT,trap13,0,DPL_3	/ Ring 0!
	IRPT_GATE	SEG_RNG1_TXT,trap14,0,DPL_3
	.long	0,0
	IRPT_GATE	SEG_RNG1_TXT,trap16,0,DPL_3
	.org	.+0x78
	IRPT_GATE	SEG_RNG1_TXT,clk,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev1,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev9,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev3,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev4,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev5,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev6,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev7,0,DPL_3
	.org	.+0x240
	IRPT_GATE	SEG_RNG1_TXT,dev8,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev9,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev10,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev11,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev12,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev13,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev14,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,dev15,0,DPL_3
	.org	.+0x40
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	IRPT_GATE	SEG_RNG1_TXT,syc,0,DPL_3
	.long	0
idtend:
	.align	4
	.long	0,0,0,0
