/*
 * i386/shm0.c
 *
 * Shared memory - memory management interface
 *
 * Revised: Thu May 27 08:09:19 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/coherent.h>
#include <sys/reg.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define SHMMAX	0x100000	/* one meg limit on shm seg size */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
SR *	accShm();
void	pdCheck();
void	shmAllDt();
SEG *	shmAlloc();
int	shmAtt();
int	shmAttach();
void	shmDetach();
void	shmDetachP();
void	shmDup();
void	shmFree();
void	shmLoad();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * shmAlloc()
 *
 * Allocate a segment for shared memory that is `bytes_wanted' bytes long.
 *
 * if successful, return allocated SEG *
 * else, return 0
 *
 * This routine is cloned from smalloc(), from which it differs by
 * (a) locking/unlocking seglink,
 * (b) NOT linking the new segment into segmq,
 * (c) rounding segment size up to a multiple of 4k bytes.
 *
 * The reference counts s_urefc and s_lrefc for a shm segment are 1
 * at the time of allocation.  Each attachment to a new process and
 * each fork of an already attached process will increment these.
 */
SEG *
shmAlloc(bytes_wanted)
off_t bytes_wanted;
{
	register SEG *new_seg = 0;
	unsigned int clicks_wanted;

	lock(seglink);
	clicks_wanted = btoc(bytes_wanted);

	/* Limit size of any shm segment to SHMMAX bytes. */
	if (bytes_wanted > SHMMAX)
		goto shmAllocDone;

	/*
	 * Estimate space needed for new segment and its overhead.
	 * Fail if not enough free RAM available.
	 */
	if (countsize(clicks_wanted) > allocno())
		goto shmAllocDone;
	/*
	 * Allocate a new SEG struct to keep track of the segment, if possible.
	 */
	if ((new_seg = kalloc(sizeof (SEG))) == NULL)
		goto shmAllocDone;

	if ((new_seg->s_vmem = c_alloc(clicks_wanted)) == 0) {
		kfree(new_seg);
		goto shmAllocDone;
	}

	new_seg->s_urefc = 1;
	new_seg->s_lrefc = 1;
	new_seg->s_size  = ctob(clicks_wanted);
	new_seg->s_flags = SFCORE;

shmAllocDone:
	unlock(seglink);
	return new_seg;
}

/*
 * shmAtt()
 *
 * Given a pointer "segp" to a SEG which is already allocated, the
 * virtual base address "base" where the segment is to appear, and an
 * index "shm_ix" into p_shmsr[] for a process, set up the
 * SR struct accordingly.
 *
 * Argument "ronflag" is nonzero if segment is to be attached read-only.
 *
 * Return 0 in case of failure, else nonzero.
 */
int
shmAtt(shm_ix, base, segp, ronflag)
unsigned int shm_ix;
caddr_t base;
SEG * segp;
int ronflag;
{
	int numBytes = segp->s_size;
	return shmAttach(shm_ix, numBytes, base, segp, ronflag);
}

/*
 * shmAttach()
 *
 * Given a pointer "segp" to a SEG which is already allocated, the number
 * "numBytes" of bytes in the segment visible in this reference, the
 * virtual base address "base" where the segment is to appear, and an
 * index "shm_ix" into p_shmsr[] for a process, set up the
 * SR struct accordingly.
 *
 * Argument "ronflag" is nonzero if segment is to be attached read-only.
 *
 * Return 0 in case of failure, else nonzero.
 */
int
shmAttach(shm_ix, numBytes, base, segp, ronflag)
unsigned int shm_ix;
off_t numBytes;
caddr_t base;
SEG * segp;
int ronflag;
{
	SR * srp;

	/* sanity checks */
	if (shm_ix >= NSHMSEG || numBytes > segp->s_size)
		return 0;

	/*
	 * You may find that the base address requested is not
	 * supported in the page directory.  Since a shm segment
	 * may straddle a 4 Mb boundary, there is the possibility
	 * of two missing page directory entries.  Check for both.
	 */
	pdCheck(base);
	pdCheck(base + segp->s_size - 1);

	srp = SELF->p_shmsr + shm_ix;
	srp->sr_base = base;
	srp->sr_flag = (SRFDUMP | SRFDATA);
	if (ronflag)
		srp->sr_flag |= SRFRODT;
	srp->sr_size = numBytes;
	srp->sr_segp = segp;

	segp->s_urefc++;
	segp->s_lrefc++;

	shmLoad();
	return 1;
}

/*
 * shmDetachP()
 *
 * Given an index "shm_ix", into the p_shmsr[] for a process,
 * and a PROC *, detach the indicated shared memory segment.
 */
void
shmDetachP(shm_ix, pp)
unsigned int shm_ix;
PROC *pp;
{
	SR * srp;
	SEG * segp;

	if (shm_ix >= NSHMSEG)
		return;

	srp = pp->p_shmsr + shm_ix;
	segp = srp->sr_segp;

	if (segp) {
		segp->s_urefc--;
		segp->s_lrefc--;

		/* We have to set detach time and decrement attachment
		 * count.
		 */
		shmSetDs(segp);	/* shm1.c */

		/* If it was last attachment and segment was marked to be
		 * removed, remove it.
		 */
		if ((segp->s_flags & SRFBERM)
		  && segp->s_urefc == 1 && segp->s_lrefc == 1)
			shmFree(segp);
	}
	srp->sr_base = 0;
	srp->sr_flag = 0;
	srp->sr_size = 0;
	srp->sr_segp = 0;

	if (pp == SELF)
		shmLoad();
}

/*
 * shmDetach()
 *
 * Given an index "shm_ix", into the p_shmsr[] for a process,
 * detach the indicated shared memory segment.
 */
void
shmDetach(shm_ix)
unsigned int shm_ix;
{
	shmDetachP(shm_ix, SELF);
}

/*
 * Scan shared memory for the range of addresses from
 * "base" up to but not including "base" + "count".
 *
 * If any shared memory segment contains the range of addresses, return
 * its SR pointer, otherwise return zero.
 *
 * This routine is used by iomapvp() and sysio().
 */
SR *
accShm(base, numBytes)
caddr_t base;
off_t numBytes;
{
	SR * srp;
	int i;

	for (i = 0; i < NSHMSEG; i++) {
		srp = SELF->p_shmsr + i;
		if (srp->sr_segp && base >= srp->sr_base
		  && base + numBytes <= srp->sr_base + srp->sr_size)
			return srp;
	}
	return 0;
}

/*
 * shmFree()
 *
 * Given a non-null SEG pointer "segp" to a shared memory segment,
 * deallocate the RAM used by that segment.
 *
 * The s_urefc field must be 1 when this routine is called, i.e., there
 * must be no pending attachments to the segment.
 */
void
shmFree(segp)
SEG * segp;
{
	if (segp == NULL) {
		printf("shmFree err: NULL argument\n");
		return;
	}

	if (segp->s_urefc != 1 || segp->s_lrefc != 1) {
		printf("shmFree err: segp=%x count=%d\n", segp, segp->s_urefc);
		return;
	}

	lock(seglink);
	c_free(segp->s_vmem, btoc(segp->s_size));
	unlock(seglink);

	kfree(segp);
}

/*
 * Given a PROC pointer "pp", detach ALL shared memory segments from
 * the process.  Done during exec and exit.
 */
void
shmAllDt()
{
	PROC * pp = SELF;
	int i;

	for (i = 0; i < NSHMSEG; i++)
		shmDetach(i);
}

/*
 * Given a PROC pointer "cpp" (e.g. child-of-current-process),
 * duplicate ALL shared memory segments for the process, and update
 * reference counts.  Done during fork.
 */
void
shmDup(cpp)
PROC * cpp;
{
	int i;
	PROC * pp = SELF;
	SR * srp;

	for (i = 0, srp = pp->p_shmsr; i < NSHMSEG; i++, srp++) {
		cpp->p_shmsr[i] = *srp;
		if (srp->sr_segp) {
			srp->sr_segp->s_urefc++;
			srp->sr_segp->s_lrefc++;
		}
	}
}

/*
 * Load mmu according to shared memory segments.
 */

void
shmLoad()
{
	register int i;
	register SR *srp;
	static SR ushmtab[NSHMSEG];

	/*
	 * Unprogram the currently active segments.
	 * Reset ushmtab.
	 */
	for (i = 0, srp = ushmtab; i < NSHMSEG; i++, srp++) {
		if (srp->sr_segp)
			unload(srp);
		srp->sr_segp = 0;
	}

	/*
	 * Load each segment in the SELF->p_shmsr list into the MMU.
	 * Remember values in ushmtab.
	 */
	for (i = 0, srp = SELF->p_shmsr; i < NSHMSEG; i++, srp++) {
		if (srp->sr_segp) {
			ushmtab[i] = *srp;
			doload(srp);
		}
	}
}

/*
 * Given a virtual address "base", check the page directory.  If the page
 * directory can't access "base", allocate a 4k page table for the segment and
 * point the page directory at the new page table.
 *
 * This routine is really tricky, so here is a picture of virtual memory:
 *
 * +--------------------+
 * |  U area, etc	|
 * |--------------------|	0xFFFF_F000
 * |  Page directory	|
 * |--------------------|	0xFFFF_E000
 * |       ...		|
 * |  Kernel text, data |
 * |--------------------|	0xFFC0_0000
 * |      -----		| <- 4k page table that maps ptable1_v[] (unmapped!)
 * |       ...		|
 * |      -----		| <- 4k page table that maps base (basePTvadd[])
 * |  Page tables	|
 * |   (ptable1_v[])	|
 * |--------------------|	0xFF80_0000
 * |       ...		|
 * |       base		|
 * |       ...		|
 * +--------------------+	0x0000_0000
 *
 * In comments below, "segment number" is a value in range 0..0x3FF.
 */
void
pdCheck(base)
{
	int baseSeg;		/* Segment number of base */
	int ptable1_vSeg;  	/* Segment number of ptable1_v */

	int tabPadd;		/* Physical address of new 4k page table */
	int basePTvadd;		/* Virtual address where we want the new page
				   table click (in ptable1_v[]). */

	int ptable1_vPTpadd;	/* Physical address of page table covering
				   segment ptable1_v[]. */
	int w;			/* Temporary virtual click number for
				 * ptable1_vPTpadd. */
	int basePTindex;	/* Offset of entry for page table for "base" 
				   within its page table. */
	int *unmapped;

	baseSeg = btosrd(base);

	/* If there is already a page table for "base", nothing to do. */
	if (ptable0_v[baseSeg] & SEG_PRE)
		return;

	/* Get a free click. */
	DV(baseSeg);
	tabPadd = clickseg(*--sysmem.pfree);
	DV(tabPadd);

	/* Point the page directory at the new click. */
	ptable0_v[baseSeg] = tabPadd | DIR_RW;

	/* Now update the page tables so we can access the new click. */

	/* Get physical address for the page table for segment ptable1_v[]. */
	ptable1_vSeg = btosrd(ptable1_v)&0x3ff;
	DV(ptable1_vSeg);
	ptable1_vPTpadd = ptable0_v[ptable1_vSeg] & ~SEG_BITS;
	DV(ptable1_vPTpadd);

	/* Map the click at ptable1_vPTpadd into virtual memory somewhere. */
	w = workAlloc();
	DV(w);
	ptable1_v[w] = ptable1_vPTpadd | SEG_SRW;
	mmuupd();

	/* Point page table at new page table click. */
	basePTvadd = (int)(ptable1_v + btocrd(base));
	DV(basePTvadd);
	basePTindex = btocrd(basePTvadd) & 0x3FF;
	DV(basePTindex);
	unmapped = (int *)(ctob(w)) + basePTindex;
	DV(unmapped);
	*unmapped = tabPadd | SEG_SRW;
	mmuupd();

	/* Release the temporary click of virtual space. */
	workFree(w);

	/* Now we can write to the new page table.  Initialize it empty. */
	memset(basePTvadd, 0, NBPC);
}
