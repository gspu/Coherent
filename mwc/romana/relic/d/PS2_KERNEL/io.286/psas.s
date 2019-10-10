/ (lgl-
/	COHERENT Driver Kit Version 1.1.0
/ 	Copyright (c) 1982, 1990 by Mark Williams Company.
/ 	All rights reserved. May not be copied without permission.
/ -lgl)
////////
/
/ PS Hard Disk Assembler Support


	.globl	com_data_, fcn_trn0_, fcn_trn1_,fcn_trn2_,fcn_trn15_
	.globl  i0_, i1_, i2_, i15_, end_table_
	
	.shri

	.globl	iresult_, mlkh_
iresult_:
	mov	ax,cs:mlkh_
	ret

mlkh_:	.word	0x1234

	.blkb 15
extram:	.blkb 15

	.blkb 15
com_data_:
	.blkb 0x80
fcn_trn0_:
	.blkb 0xb0		;	.blkb 0x10
dev_blk0_:
	.blkb 0xb0		;	.blkb 0x10
fcn_trn1_:
	.blkb 0xb0		;	.blkb 0x50
dev_blk1_:
	.blkb 0xb0		;	.blkb 0x56
fcn_trn2_:
	.blkb 0xb0		;	.blkb 0x40
dev_blk2_:
	.blkb 0xf0		;	.blkb 0x96
fcn_trn15_:
	.blkb 0xb0		;	.blkb 0x58
dev_blk15_:
	.blkb 0xb0		;	.blkb 0x2e
end_table_:

i0_:	.word	0
i1_:	.word	0
i2_:	.word	0
i15_:	.word	0

sp_:	.blkw	0x24			
it_:	.blkw	576			/This gives room for 24 items


	.globl iabios_r_
iabios_r_:
	mov	ax,cs
	mov	ds,ax
	mov	es,ax

	mov	di,$sp_		/ Destination for the system parameter table
	call	build_sys_p
	cmp	ax,0
	jne	3f		/ Quit on an error

	mov	di,$it_		/ Destination for the Initialization tables
	call	build_itab
	cmp	ax,0
	jne	3f		/ Quit on an error

	mov	si,0
	movb	cl,$24
0:	
	mov	ax, si
	cmp	ax, sp_+30
	jge	2f

	imulb	cl
	mov	di, ax
	add	di,$it_

	cmp	(di), $0
	jne	1f
	mov	i0_, di
1:
	cmp	(di), $1
	jne	1f
	mov	i1_, di
1:
	cmp	(di), $2
	jne	1f
	mov	i2_, di
1:
	cmp	(di), $15
	jne	1f
	mov	i15_, di
1:
	inc	si
	jmp	0b
2:
	call	init_dev
3:
	mov	cs:mlkh_, ax
	ret


build_sys_p:
	push	ds

	xor	ax,ax	/ Set Ds to 0, meaning that there are no Ram
	mov	ds,ax	/   extentions to be loaded
	movb	ah,$4
	int	0x15	/ Bios call to build the System parameter table

	jc	1f	/ Error?

	xor	ax,ax	/ No error

0:	pop	ds	/ Restore registers and return
	ret

1:	mov	ax,$0xffff	/ Return that an error has occured
	jmp	0b



build_itab:
	push	ds

	mov	ax,$extram	/ Set up extram so that it starts at the
	shr	ax,$4		/   begining of a segment. This is becuase
	mov	bx,ds		/   the Cbios is expecting a pointer of the
	add	ax,bx		/   sort DS:0
	mov	ds,ax
	mov	0,$0xaa55	/  Extram header value.
	movb	ah,$5
	int	0x15		/ Cbios call to build init table

	jc	1f		/ Error?

	xor	ax,ax		/ No error

0:	pop	ds		/ Restore registers and return
	ret

1:	mov	ax,$0xfffe	/ Return that an error has occured
	jmp	0b


init_dev:
	mov	ax,$com_data_/ Set up com_data so that it starts at the
	shr	ax,$4		/   begining of a segment. This is becuase
	mov	dx,ds		/   the Cbios is expecting a pointer of the
	add	ax,dx		/   sort DS:0 for the Common data table
	mov	ds,ax

	mov	 0,$64		/ Pointer to pointer 0. Note that as we add
				/  devices to the system, this will move
				/  farther down by 8 for every db/ftt pair
				/  and 6 for every data pointer.

	mov	 2,$6		/ Number of pointer pairs in the table
	mov	 8,$0		/ Dummy dev blk and fcn tbl pntr for Id 1
	mov	10,$0		/  This is required for initialization
	mov	12,$0		/  Remember only to have one for Id 1. This
	mov	14,$0		/  Fails if you also have one for Id 0.
	mov	16,$dev_blk0_	/\
	mov	18,dx		/ \
	mov	20,$fcn_trn0_	/  \
	mov	22,dx		/   |
	mov	24,$dev_blk1_	/   |
	mov	26,dx		/   |
	mov	28,$fcn_trn1_	/   | long ptrs to the device blocks and
	mov	30,dx		/   | the function transfer tables for
	mov	32,$dev_blk15_	/   | devs 0, 1, 15, 2
	mov	34,dx		/   |
	mov	36,$fcn_trn15_	/   |
	mov	38,dx		/   |
	mov	40,$dev_blk2_	/   |
	mov	42,dx		/   |
	mov	44,$fcn_trn2_	/  /
	mov	46,dx		/ /

			      / At this point we should initialize the pointer
				/   part of the table, but since these values
				/   are already 0, we can skip it.


	mov	bx, cs:i0_	/ Initialize the internal functions
	mov	cx,$1
	mov	dx,$0x2		/ Logical device 2

	.word	0xff26, 0x065f	/ calll  es:(bx+6)

	cmpb	al,$0		/ Was there an error?
	jne	0f

	mov	bx, cs:i1_	/ Initialize the diskette
	mov	cx,$1
	mov	dx,$3		/ Logical device 3

	.word	0xff26, 0x065f 	/ calll  es:(bx+6)
	
	cmpb	al,$0		/ Was there an error?
	jne	0f

	mov	bx, cs:i15_	/ Initialize the DMA
	mov	cx,$1
	mov	dx,$4		/ Logical device 5

	.word	0xff26, 0x065f 	/ calll  es:(bx+6)
	
	cmpb	al,$0		/ Was there an error?
	jne	0f

	mov	bx, cs:i2_	/ Initialize the hard disk
	mov	cx,$1
	mov	dx,$5		/ Logical device 6

	.word	0xff26, 0x065f 	/ calll  es:(bx+6)
	
	cmpb	al,$0		/ Was there an Error?
	je	1f

0:	mov	ax,$0xfffd	/ Inform caller that we could not init
1:	ret
	

	.globl	d2_func_, com_data_p_, fcn2_, fcn2p_, dev2p_
d2_func_:
	push	bp
	mov	bp,sp

	push	com_data_p_+2	/ save the anchor pointer segment
	
	push	sds_		/ Save the request block segment
	push	4(bp)		/ Save the request block offset

	push	fcn2p_+2	/ Save the Function transfer table Segment
	push	fcn2p_		/ Save the Function transfer table Offset

	push	dev2p_+2	/ Save the device block Segment
	push	dev2p_		/ Save the device block Offset

	mov	bx,6(bp)	/ Bx points to start, interrupt or time-out
	.word	0x9fff,fcn2_	/ Call far (bx + fcn2_)
1:	
	add	sp,$0xe		/ Clean up the stack

	pop	bp
	ret



	.globl	d1_func_, fcn1_, fcn1p_, dev1p_
d1_func_:
	push	bp
	mov	bp,sp

	push	com_data_p_+2	/ save the anchor pointer segment
	
	push	sds_		/ Save the request block segment
	push	4(bp)		/ Save the request block offset

	push	fcn1p_+2	/ Save the Function transfer table Segment
	push	fcn1p_		/ Save the Function transfer table Offset

	push	dev1p_+2	/ Save the device block Segment
	push	dev1p_		/ Save the device block Offset

	mov	bx,6(bp)	/ Bx points to start, interrupt or time-out
	.word	0x9fff,fcn1_	/ Call far (bx + fcn2_)
1:	
	add	sp,$0xe		/ Clean up the stack

	pop	bp
	ret

