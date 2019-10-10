/*
 * fakeff.c
 * Simulate all the calls for far memory access from COH 286.
 * Far pointers are simulated with virtual addresses.
 */
#include <sys/coherent.h>
#include <sys/seg.h>
#include <sys/fakeff.h>

extern SR allocp;

/*
 * Initialize a virtual address to access physical memory at location
 * 'paddr', of size 'len' bytes.  It provides read and write (but not
 * execute) access.  When no longer required, a virtual address should be
 * released by vrelse.
 */
faddr_t
map_pv(paddr, len)
__paddr_t paddr;
fsize_t len;
{
	int s;			/* Return value of sphi().  */
	int npage;		/* Number of pages we must allocate.  */
	faddr_t	chunk_start;	/* Start of allocated segment in vmem.  */
	faddr_t retval;		/* Address of desired physical memory in vmem.  */
	int base1;		/* Offset into ptable1_v[].  */
	cseg_t	pte;		/* Build page table entries here.  */

	/* Figure out how many clicks we need to map.
	 *	   [		    ]		What we want.
	 *	[	|	|	]	What we get.
	 * Total number of clicks is:
	 *	(click up from (paddr+len)) - (click down from paddr)
	 */
		
	npage = btoc(paddr+len) - btocrd(paddr);

	/* Note that sysmem.vaddre is ALWAYS click aligned.  */
	
	/*
	 * Allocate the required chunk of virtual memory space.
	 * This could be a lot more sophisticated.  For expedience,
	 * there is no way to free this after it has been allocated,
	 * and there are no checks to see if we ran out of virtual space.
	 */
	s = sphi();
	chunk_start = sysmem.vaddre;
	sysmem.vaddre += ctob(npage);
	spl(s);

	/*
	 * Figure out where the desired physical address ends up in vmem.
	 */
	retval = chunk_start + (paddr - ctob(btoc(paddr)));
	
	/*
	 * Load the page table.
	 */
	base1 = btocrd(chunk_start);
	pte = ctob(btocrd(paddr));
	do {
		ptable1_v[base1] = pte | SEG_SRW;
		base1++;
		pte += ctob(1);	/* Bump up to next physical click.  */
	} while (--npage > 0);
	mmuupd();	/* Tell the mmu about the new map.  */

	return(retval);
} /* map_pv() */

/*
 * Release a virtual address that we previously obtained with function
 * map_pv().
 */
void
unmap_pv(faddr)
faddr_t faddr;
{
	/* For the moment, this function does nothing.  */
}		

/*
 * Translate virtual address to physical address.
 * Returns the current physical address associated with virtual address 'vaddr'.
 * Returns 0 if that portion of virtual address space is not associated with
 * any physical memory.
 */
paddr_t
vtop(vaddr)
caddr_t vaddr;
{
	paddr_t	retval;
	unsigned int ptable_idx;	/* Index into ptable1_v[].  */

	ptable_idx = btocrd(vaddr);

	/*
	 * There is a 4Mbyte virtual page table ptable1_v[] which is
	 * all the bottom level page tables appended into a big array.
	 * Note that there are huge holes in this data structure, for
	 * unmapped virtual address space.
	 *
	 * We are going to look up 'vaddr' in the virtual page table
	 * ptable1_v[].
	 *
	 * But first, we have to see if the portion of page table we are
	 * going to look at exists.  We do this by looking at the one click
	 * long page table that maps the virtual page table, PPTABLE1_V[].
	 */
	retval = 0;	/* Assume entry not found.  */

	if (ptable0_v[btosrd(vaddr)] & SEG_SRO) {
		/*
		 * ASSERTION:  The portion of ptable1_v[] we want is valid.
		 */
		if (ptable1_v[ptable_idx] & SEG_SRO) {

			/*
			 * ASSERTION:  'vaddr' corresponds to some
			 * physical memory.
			 *
			 * Note that the address of a physical click is
			 * all above bit 11 in the PTE.
			 */
			retval = (ptable1_v[ptable_idx] & ~(NBPC - 1));
			retval += ((long) vaddr & (NBPC - 1));
		}
	}

	T_PIGGY( 0x10, printf("vtop(%x)=%x, ", vaddr, retval); );

	return(retval);
} /* vtop() */	

/*
 * Translate system global address 'vpaddr' to physical address.
 *
 * May cause a panic if 'vpaddr' does not correspond to a real physical
 * address.
 */
paddr_t
vptop(vpaddr)
paddr_t vpaddr;
{
	paddr_t	retval;
	cseg_t pte;	/* Page table entry from sysmem.u.pbase[].  */

	T_PIGGY( 0x10, printf("vptop(%x)=", vpaddr); );

	pte = sysmem.u.pbase[btocrd(vpaddr)];
	pte &= ~(NBPC - 1);	/* Strip off the non-address information.  */

	retval = pte | (vpaddr & (NBPC - 1));

	T_PIGGY( 0x10, printf("%x,", retval); );

	return(retval);
} /* vptop() */

/*
 * Convert from virtual address to system global address.  Similar to MAPIO(),
 * but does not require separate segment and offset.
 *
 * Only works for Kernel Space virtual addresses.
 */
paddr_t
vtovp(vaddr)
caddr_t vaddr;
{
	paddr_t	retval;

	T_PIGGY( 0x10, printf("vtopvp(%x)=", vaddr); );

	retval = MAPIO((allocp.sr_segp->s_vmem), (vaddr - allocp.sr_base));

	T_PIGGY( 0x10, printf("%x, ", retval); );

	return( retval );
}
