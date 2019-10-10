/* $Header: /kernel/kersrc/i286/RCS/mmu.c,v 1.1 92/07/17 15:21:33 bin Exp Locker: bin $
 *
 *	The  information  contained herein  is a trade secret  of INETCO
 *	Systems, and is confidential information.   It is provided under
 *	a license agreement,  and may be copied or disclosed  only under
 *	the terms of that agreement.   Any reproduction or disclosure of
 *	this  material  without  the express  written  authorization  of
 *	INETCO Systems or persuant to the license agreement is unlawful.
 *
 *	Copyright (c) 1987
 *	An unpublished work by INETCO Systems, Ltd.
 *	All rights reserved.
 */

/*
 * Coherent.
 * Memory Management Unit.
 *
 * $Log:	mmu.c,v $
 * Revision 1.1  92/07/17  15:21:33  bin
 * Initial revision
 * 
 * Revision 1.1	88/03/24  17:33:41	src
 * Initial revision
 * 
 * 88/03/04	Allan Cornish	/usr/src/sys/i8086/ibm_at/mmu.c
 * Real-mode code ifdef'ed out [REAL_MODE].
 *
 * 88/02/16	Allan Cornish	/usr/src/sys/i8086/ibm_at/mmu.c
 * vtop() now incorporates offset from virtual address into physical address.
 *
 * 87/11/13	Allan Cornish	/usr/src/sys/i8086/ibm_at/mmu.c
 * Initial version.
 */
#include <sys/coherent.h>
#include <sys/seg.h>
#include <sys/mmu.h>

/**
 *
 * static faddr_t
 * gdtalloc()		-- search for empty global descriptor table entry
 */
static faddr_t
gdtalloc()
{
	register saddr_t sel;
	static saddr_t osel;
	faddr_t fp;
	int s;

	/*
	 * Disable interrupts.
	 */
	s = sphi();

	/*
	 * Search for idle virtual selector.
	 */
	for ( fp = 0, sel = osel + 8; sel != osel; sel += 8 ) {

		/*
		 * Selector 0 is NOT usable.
		 */
		if ( sel == 0 )
			continue;

		/*
		 * Selector is available.
		 */
		if ( ffbyte( sel+5, gdtsel ) != 0 )
			continue;

		/*
		 * Mark selector as being Descriptor[10].
		 */
		sfbyte( sel+5, gdtsel, 0x10 );

		/*
		 * Record selector for next search.
		 */
		osel = sel;

		FP_SEL(fp) = sel;
		break;
	}

	/*
	 * Enable interrupts.
	 */
	spl( s );

	return( fp );
}

/**
 *
 * void
 * vprint( fp )			-- print virtual address information.
 * faddr_t fp;
 *
 *	Input:	fp = segment:offset pair for virtual address in question.
 *
 *	Action:	Print information about virtual address.
 */

void
vprint( fp )
faddr_t fp;
{
	faddr_t gp;
	paddr_t paddr;

#if REAL_MODE > 0
	/*
	 * Virtual Address Mode Disabled.
	 */
	if ( gdtsel == 0 )
		return;
#endif

	/*
	 * Create far pointer to appropriate gdt entry.
	 */
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = FP_SEL(fp) & ~7;

	FP_OFF(paddr) = ffword(gp+2);
	FP_SEL(paddr) = ffbyte(gp+4);

	/*
	 * Print information about gdt entry.
	 */
	printf("sel=%x paddr=%X lim=%x flags=%x\n",
		FP_OFF(gp), paddr, ffword(gp), ffbyte(gp+5) );
}

/**
 *
 * void
 * vremap( sp )			-- (re)map segment virtual address
 * SEG * sp;
 *
 *	Input:	sp = pointer to segment structure to be (re)mapped.
 *
 *	Action:	Update segment information.
 */
void
vremap( segp )
SEG * segp;
{
	register SEG * sp = segp;
	register int m;
	faddr_t gp;
	int s;

#if REAL_MODE > 0
	/*
	 * Virtual Address Mode disabled.
	 */
	if ( gdtsel == 0 ) {
		/*
		 * Calculate virtual address as shifted physical address.
		 */
		FP_SEL(sp->s_faddr) = sp->s_paddr >> 4;
		FP_OFF(sp->s_faddr) = sp->s_paddr & 15;
		return;
	}
#endif

	/*
	 * Create far pointer to appropriate gdt entry.
	 */
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = FP_SEL(sp->s_faddr);

	/*
	 * Allocate virtual selector if not already specified.
	 */
	if ( FP_SEL(sp->s_faddr) == 0 )  {
		if ( (sp->s_faddr = gdtalloc()) == 0 )
			panic( "vremap: out of gdt's\n" );
		FP_OFF(gp) = FP_SEL(sp->s_faddr);
	}

	/*
	 * Ensure selector is valid gdt at privilege level 0.
	 */
	if ( FP_SEL(sp->s_faddr) & 7 ) {
		panic("vremap( faddr=%X, ip=%x ) - not gdt at level 0\n",
			sp->s_faddr, (&segp)[-1] );
	}

	/*
	 * Verify selector is not free.
	 */
	if ( ffbyte( gp+5 ) == 0 ) {
		panic("vremap( faddr=%X, ip=%x ) - selector is free\n",
			sp->s_faddr, (&segp)[-1] );
	}

	/*
	 * Disable interrupts.
	 */
	s = sphi();

	/*
	 * Set limit.
	 */
	sfword( gp+0, (unsigned) (sp->s_size - 1) );

	/*
	 * Set low word of base, high byte of base.
	 */
	sfword( gp+2, FP_OFF(sp->s_paddr) );
	sfbyte( gp+4, FP_SEL(sp->s_paddr) );

	/*
	 * Set access byte:
	 *	Code = Present[80],Descriptor[10],Executable[08],Readable[02].
	 *	Data = Present[80],Descriptor[10], 		 Writable[02].
	 */
	m = 0x12;
	if ( sp->s_flags & SFCORE )
		m |= 0x80;
	if ( sp->s_flags & SFTEXT )
		m |= 0x08;
	sfbyte( gp+5, m );

	/*
	 * Clear reserved word.
	 */
	sfword( gp+6, 0 );

	/*
	 * Enable interrupts.
	 */
	spl(s);
}

/**
 *
 * faddr_t
 * ptov( paddr, n )		-- physical to virtual [address]
 * paddr_t paddr;
 * fsize_t n;
 *
 *	Input:	paddr = physical address.
 *		n = size in bytes.
 *
 *	Return:	Corresponding segment:offset virtual address, or 0.
 *
 *	Notes:	Limited to 20 bit physical addresses.
 *		This routine is not functional in protected mode.
 */

faddr_t
ptov( paddr, n )
paddr_t paddr;
fsize_t n;
{
	faddr_t fp;
	faddr_t gp;
	int s;

#if REAL_MODE > 0
	/*
	 * Virtual Address Mode Disabled.
	 */
	if ( gdtsel == 0 )
		return( ((paddr >> 4) << 16) + (paddr % 16) );
#endif

	/*
	 * Allocate virtual selector.
	 */
	if ( (fp = gdtalloc()) == 0 )
		panic( "ptov:ip=%x: out of gdt's\n", ((char*) &paddr)[-1] );

	/*
	 * Create far pointer to appropriate gdt entry.
	 */
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = FP_SEL(fp);

	/*printf("ptov: gp=%lx, paddr=%lx, n=%lx\n", gp, paddr, n); */

	/*
	 * Disable interrupts.
	 */
	s = sphi();

	/*
	 * Set limit.
	 */
	sfword( gp+0, (unsigned) (n - 1) );

	/*
	 * Set low word of base, high byte of base.
	 */
	sfword( gp+2, FP_OFF(paddr) );
	sfbyte( gp+4, FP_SEL(paddr) );

	/*
	 * Set access byte: Present[80], Descriptor[10], Writable[02].
	 */
	sfbyte( gp+5, 0x92 );

	/*
	 * Clear reserved word.
	 */
	sfword( gp+6, 0 );

	/*
	 * Enable interrupts.
	 */
	spl( s );

	/*
	 * Return virtual address.
	 */
	return( fp );
}

/**
 *
 * faddr_t
 * ptovx(paddr)	-- physical to virtual [address] for executable segment
 * paddr_t paddr;
 *
 *	Input:	paddr = physical address.
 *
 *	Return:	Corresponding segment:offset virtual address, or 0.
 *
 */

faddr_t
ptovx(paddr)
paddr_t paddr;
{
	faddr_t fp;
	faddr_t gp;
	int s;

#if REAL_MODE > 0
	/*
	 * Virtual Address Mode Disabled.
	 */
	if ( gdtsel == 0 )
		return( ((paddr >> 4) << 16) + (paddr % 16) );
#endif

	/*
	 * Allocate virtual selector.
	 */
	if ( (fp = gdtalloc()) == 0 )
		panic( "ptov:ip=%x: out of gdt's\n", ((char*) &paddr)[-1] );

	/*
	 * Create far pointer to appropriate gdt entry.
	 */
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = FP_SEL(fp);

	/*
	 * Disable interrupts.
	 */
	s = sphi();

	/*
	 * Set limit.
	 */
	sfword( gp+0, (unsigned) (0xffff) );

	/*
	 * Set low word of base, high byte of base.
	 */
	sfword( gp+2, FP_OFF(paddr) );
	sfbyte( gp+4, FP_SEL(paddr) );

	/*
	 * Set access byte: Present[80], Descriptor[10], Conforming[4]
	 *                  Executable[08], Readable[02].
	 */
	sfbyte( gp+5, 0x9a );

	/*
	 * Clear reserved word.
	 */
	sfword( gp+6, 0 );

	/*
	 * Enable interrupts.
	 */
	spl( s );

	/*
	 * Return virtual address.
	 */
	return( fp );
}

/**
 *
 * paddr_t
 * vtop( fp )			-- virtual to physical [address]
 * faddr_t fp;
 *
 *	Input:	fp = segment:offset virtual address.
 *
 *	Return:	 * = corresponding physical address.
 *		-1 = invalid virtual address.
 */
paddr_t
vtop( fp )
faddr_t fp;
{
	faddr_t gp;
	paddr_t paddr;

#if REAL_MODE > 0
	/*
	 * Virtual Address Mode Disabled.
	 */
	if ( gdtsel == 0 )
		return( (FP_SEL(fp) << 4L) + FP_OFF(fp) );
#endif

	/*
	 * Convert virtual address to point to appropriate gdt entry.
	 */
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = FP_SEL(fp);

	/*
	 * Validity check - Selector must be Present[80] and Descriptor[10].
	 */
	if ( (ffbyte(gp+5) & 0x90) != 0x90 ) {
		panic( "vtop:ip=%x: sel %x invalid\n",
			((char **) &fp)[-1],
			FP_SEL(fp) );
	}

	/*
	 * Extract physical address from gdt entry.
	 */
	FP_OFF(paddr) = ffword(gp+2);
	FP_SEL(paddr) = ffbyte(gp+4);

	/*
	 * Adjust physical address by virtual address offset.
	 */
	paddr += FP_OFF(fp);

	/*
	 * Return physical address.
	 */
	return( paddr );
}

/**
 *
 * void
 * vrelse( fp )		-- release virtual address
 * faddr_t fp;
 *
 */

void
vrelse( fp )
faddr_t fp;
{
	faddr_t gp;

#if REAL_MODE > 0
	/*
	 * Virtual Address Mode Disabled.
	 */
	if ( gdtsel == 0 )
		return;
#endif

	/*
	 * Validity check.
	 */
	if ( (FP_SEL(fp) == 0) || (FP_SEL(fp) & 7) )
		panic( "vrelse: sel %x invalid\n", FP_SEL(fp) );

	/*
	 * Construct virtual address to point to appropriate gdt entry.
	 */
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = FP_SEL(fp);

	/*
	 * Virtual selector already released, or not a segment descriptor.
	 */
	if ( (ffbyte(gp+5) & 0x10) != 0x10 ) {
		panic( "vrelse:ip=%x: sel %x already released\n",
			((char **) &fp)[-1],
			FP_SEL(fp) );
	}

	/*
	 * Release virtual selector.
	 */
	sfbyte( gp+5, 0 );
}
