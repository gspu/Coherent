/////////////////////////////////////////////////////////////////////////
/               							/
/									/
/ File:        DESSUBS.S						/
/									/
/ Author:      JN							/
/									/
/ Date:        7/18/86							/
/									/
/ Description: Assembly language version of COHERENT (v. 2.3.35) DES    /
/	       (Data Encryption Standard) subroutines converted from    /
/	       'C' for speed improvement purposes.  The following	/
/     	       routines are replaced:                         		/
/									/
/			permute						/
/			bcopy						/
/			lrot						/
/			m2add						/
/			dosboxes					/
/									/
/									/	       			
/////////////////////////////////////////////////////////////////////////

/ 
/ Modifications:
/	04/01/87; JN; Fixed bug in lrot() where the array wasn't getting
/		      rotated properly for shift counts of 2.
/				

/
/ Note that not all of these subroutines use the same algorithms as their 'C'
/ counter-parts (for speed), although the result is the same.
/

///////
/
/ permute(src, dest, tptr, nbits)
/ char *src, *dest, *tptr;
/ int nbits;
/
/ 	Input:  dest = pointer to place to store permuted bit array
/		src  = pointer to bit array to permute
/		tptr = pointer to table containing permution data
/		nbits = number of bits to permute
/
/	Action: Using pointer to passed table, stores permuted source
/		bit array into destination.
/
/	Returns: nothing.
/
///////	
	
	.globl 	permute_	/ visible by all
	.shri			/ shared instruction

permute_:
	push	si
	push	di
	push	bp
	mov	bp,sp
	mov	cx,14(bp)	/ get number of bits
	mov	si,12(bp)	/ get pointer to table
	mov	di,10(bp)	/ get pointer to dest
	mov	dx,8(bp)	/ get pointer to source
	shr	cx,$1		/ always even
	cld		
	
perm_0:
	lodsw			/ get 2 bit positions from table
	movb	bl,al		/ get 1st bit position
	xorb	bh,bh		/ zero bh
	add	bx,dx   	/ bx = pointer to specified source bit
	movb	al,(bx)		/ get bit
						
	movb	bl,ah		/ get 2nd bit position
	xorb	bh,bh		/ zero bh
	add	bx,dx 		/ bx = pointer to specified source bit
	movb	ah,(bx)		/ get bit
	stosw			/ store both bits
	loop	perm_0	
	
	pop	bp
	pop	di
	pop	si
	ret



///////
/
/ bcopy(src, dest, n)
/ char *src, *dest;
/ int n;
/
/ 	Input:  dest = pointer to dest string
/		src  = pointer to source string
/		n    = number of bytes to copy
/
/	Action: copy source string to destination.
/
/	Returns: nothing.
/
///////	
	
	.globl 	bcopy_   	/ visible by all
	.shri			/ shared instruction

bcopy_:
	push	si
	push	di
	push	bp
	mov	bp,sp
	mov	cx,12(bp)	/ get count
	mov	si,8(bp)	/ get pointer to source
	mov	di,10(bp)	/ get pointer to dest
	cld			 
	
bcpy_0:
	shr	cx,$1		/ copy words for speed
	jnc	bcpy_1		/ even if no carry
	movsb			/ move odd byte

bcpy_1:
	rep 				
	movsw 			/ copy the rest
	pop	bp
	pop	di
	pop	si
	ret


///////
/
/ lrot(bits, ns, nbits)
/ char *bits;
/ int ns, nbits;
/
/ 	Input:  bits = pointer to bit array
/		ns   = number of shifts
/		nbits = number of bits in bit array (1 or 2)
/
/	Action: rotate bit array left 'ns' number of times.  'ns'
/ 		must be a 1 or 2.
/
/	Returns: nothing.
/
///////	
	
	.globl 	lrot_   	/ visible by all
	.shri			/ shared instruction

lrot_:
	push	si
	push	di
	push	bp
	mov	bp,sp
	movb	dl,10(bp)	/ get number of times to rotate
	mov	cx,12(bp)	/ get number of bits
	cld
	mov	si,8(bp)	/ point to bit array
	mov	di,si
	decb	dl		/ dl = 1 or 2 
	jnz	lr2		
	call	lrot_1		/ rotate left by one
	jmp	lr_ret

lr2:
	call	lrot_2		/ rotate left by two

lr_ret:
	pop 	bp
	pop	di
	pop	si
	ret

	/
	/ subroutine to rotate bit array left by one
	/

lrot_1:
	lodsb			/ get first bit (carry)
	movb	ah,al		/  and save
	dec	cx		/ already got carry bit
	rep
	movsb			/ shift left by one
	movb	al,ah		
 	stosb			/ store carry bit
	ret


	/
	/ subroutine to rotate bit array left by two
	/  					

lrot_2:
	lodsw			/ get first bits (carry)
	shr	cx,$1		/ always even
	dec	cx		/ already got carry bits
       	rep
	movsw			/ shift left by two
	stosw			/ store carry bits
	ret


///////
/
/ m2add(i1, i2, o, nbits)
/ char *i1, *i2, *o; 
/ int nbits;
/
/ 	Input:  i1  =  pointer to 1st bit array
/		i2  =  pointer to 2nd bit array 
/		o   =  place to store the result
/		nbits = number of bits in each bit array
/
/	Action: modulo 2 addition of bit arrays (exclusive or)
/
/	Returns: nothing.
/
///////	
	
	.globl 	m2add_   	/ visible by all
	.shri			/ shared instruction

m2add_:
	push	si
	push	di
	push	bp
	mov	bp,sp
      	mov	cx,14(bp)	/ get number of bits 
	mov	si,8(bp)	/ get i1
	mov	bx,10(bp)	/ get i2
	mov	di,12(bp)	/ get dest
	shr	cx,$1		/ always even
      	cld			

m2add_1:
	lodsw			/ get 2 bits from i1
	xor	ax,(bx)	 	/ o = i1 xor i2
	inc	bx		/ inc ptr to i2	
	inc	bx
	stosw			/ store into o
	loop	m2add_1
	pop	bp
	pop	di
	pop	si
	ret


///////
/
/ dosboxes(ibits, obits)
/ char *ibits, *ibits; 
/
/ 	Input:  ibits  =  pointer to source (48 bit array)
/		obits  =  pointer to dest (32 bit array) 
/
/	Action: convert source array into destination array using
/		the 'S-box' functions.
/
/	Returns: nothing.
/
///////	
	
	.globl 	dosboxes_   	/ visible by all
	.shri			/ shared instruction

dosboxes_:
	push	si
	push	di
	push	bp
	mov	bp,sp
	sub	sp,$2		/ allocate local space
	cld

	/
	/ initialize variables
	/
	mov	-2(bp),$0	/ count	(0 - 7)
	mov	si,8(bp)	/ pointer to source
	mov	di,10(bp)	/ pointer to place to store result

	/
	/ parse the source array in 6 bit pieces-- the first and last
	/ bits are the S-box row, the middle 4 bits are the S-box column.
	/

dosbox_0:
	lodsb			/ r1
	movb	dl,al		/ dl = row
	lodsb			/ c1
	movb	dh,al		/ dh = column
	shlb	dh,$1
	lodsb 			/ c2
	orb	dh,al
	shlb	dh,$1
	lodsb 			/ c3
	orb	dh,al
	shlb	dh,$1
	lodsb			/ c4
	orb	dh,al	
	shlb	dl,$1
	lodsb			/ r2
	orb	dl,al

	/
	/ get S-box entry using row & column address
	/
	mov	bx,-2(bp)	/ get count
	movb	cl,$6		/ sbox = count * 64 (size of each sbox)
	shl	bx,cl		
	movb	al,dl		/ get row number
	xorb	ah,ah		/ zero ah
	movb	cl,$4		/ row = row # * 16
	shl	ax,cl		
	add	bx,ax		
	movb	al,dh	 	/ get column number
	xorb	ah,ah		/ zero ah
	add	bx,ax		/ bx now points to the proper sbox entry
	movb	ah,Sboxes_(bx)	/ get sbox entry

	/
	/ unpack sbox entry and store in dest
	/
	mov	cx,$4 		/ # of significant bits in sbox entry
	shlb	ah,cl		/ move to upper nybble 
							
sbox_0:
	xorb	al,al		/ default to zero bit 
	shlb	ah,$1	 	/ shift left bit into carry
	jnc	sbox_1		
	incb	al		/ a 1 was shifted out

sbox_1:
	stosb			/ store bit into dest
	loop	sbox_0

	/
	/ do until count (bp-2) = 8 
	/				
	inc	-2(bp)
	cmp	-2(bp),$8
	jb	dosbox_0
	mov	sp,bp		/ de-allocate local space
	pop	bp
	pop	di
	pop	si
	ret




