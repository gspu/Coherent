/*
 * MMU dependent code for Coherent 386
 *
 * Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
 */

#include <sys/coherent.h>
#include <sys/clist.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/buf.h>
#include <sys/alloc.h>
#include <l.out.h>
#include <ieeefp.h>

/* These defines belong somewhere else:  */
#define LOMEM	0x15	/* CMOS address of size in K of memory below 1MB.  */
#define EXTMEM	0x17	/* CMOS address of size in K of memory above 1MB.  */
#define ONE_K	1024
#define ONE_MEG	1048576
#define USE_NDATA	1

/*
 * DMA will not work to memory above 16M, so limit the amount of memory
 * above 1M to 15M.  A much cleverer scheme should be implemented.
 */
int HACK_LIMIT = (15*ONE_MEG);

/*
 * For 0 < i < 64, buddysize[i] is log(base 2) of nearest power of two
 * which is greater than or equal to i.
 */
char	buddysize[64] = {
	-1, 0, 1, 2, 2, 3, 3, 3,
	3, 4, 4, 4, 4, 4, 4, 4,
	4, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,
	5, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6 };

#define	min(a, b)	((a) < (b) ? (a) : (b))

/*
 * Functions.
 *	Import.
 *	Export.
 *	Local.
 */
void		areacheck();
void		areafree();
void		areainit();
BLOCKLIST *	arealloc();
int		areasize();
cseg_t *	c_alloc();
cseg_t *	c_extend(); 
void		c_free();
int		c_grow();
int		countsize();
void		doload();
char *		getPhysMem();
void		i8086();
void		idtinit();
void		init_phy_seg();
void		mchinit();
void		msigend();
void		msigstart();
void		physMemInit();
SR		*loaded();
unsigned int	read16_cmos();
void		segload();
void		sunload();
void		unload();
void		valloc();

#define		zero_fill(from, len)	memset(from, 0, len)

/*
 * "load" a handle "hp"  to a segment into the space tree for a process
 */
void
doload(srp)
register SR	*srp;
{
	register int	n;
	register	cseg_t *pp;
	register int	base1, flags;
	register int	akey;

	pp = srp->sr_segp->s_vmem;
	flags = srp->sr_segp->s_flags;
	base1 = btocrd(srp->sr_base);
	n = btoc(srp->sr_size);

	/*
	 * we load all pages
	 */
	 /* a shm segment ref may be Read-Write or Read-Only */
	if (srp->sr_flag & SRFRODT)
		akey = SEG_RO;
	else {
		switch (flags&(SFSYST|SFTEXT)) {
		case SFTEXT:	akey = SEG_RO;  break;
		case SFSYST:	akey = SEG_SRW; break;
		default:	akey = SEG_RW;  break;
		}
	}

	do
		ptable1_v[base1++] = (*pp++ & ~SEG_NPL) | akey;
	while (--n);
	mmuupd();
}

/*
 * unload a handle key "key" to a segment from the MMU hardware
 */
void
unload(srp)
register SR *srp;
{
	register int	n, base1;

	base1 = btocrd(srp->sr_base);
	
	n = btoc(srp->sr_size);
	do {
		ptable1_v[base1++] = SEG_ILL;
	} while (--n);
	mmuupd();
}

/*
 * Allocate 'clicks_wanted' clicks of core space.
 * Returns physical segment descriptor if success, else NULL.
 * The physical segment descriptor is a table of page table entries
 * suitable for insertion into a page table.
 */
cseg_t *
c_alloc(clicks_wanted)
unsigned	clicks_wanted;
{
	unsigned	pno;
	cseg_t *pp;
	register cseg_t *qp;

	/* Do we have enough free physical clicks for this request?  */
	if (clicks_wanted > allocno())
		goto no_c_alloc;

	/* Allocate some space for the table to return.  */
	if ((pp = (cseg_t *)arealloc(clicks_wanted)) == 0)
		goto no_c_alloc;
	qp = pp;

	/* fill in entries in the requested table */
	do {
		pno = *--sysmem.pfree;
		if (!pvalid(pno))
			panic("c_alloc");
		*qp++ = (clickseg(pno) & ~SEG_BITS) | SEG_PRE;
	} while (--clicks_wanted);
	return pp;

no_c_alloc:
	return 0;
}

/*
 * Given an array "pp" containing "numClicks" click descriptors,
 *   if "pp" is the click list for a user segment currently loaded
 *     invalidate click entries for "pp" in the current page table
 *   return each click in "pp" to the sysmem pool, if it came from there.
 *   return the array "pp" to the buddy pool.
 */
void
c_free(pp, numClicks)
cseg_t	*pp;
unsigned	numClicks;
{
	unsigned	pno;
	register cseg_t *qp;
	register int	sz;
	SR		*srp;

	if (srp = loaded(pp)) {
		unload(srp);
		srp->sr_segp = 0;
	}
	sz = numClicks;
	if (&sysmem.pfree[sz] > sysmem.efree)
		panic("c_free - nalloc");
	qp = pp;
	do {
		if ((*qp & SEG_NPL) == 0) {
			pno = segclick(*qp);
			if (!pvalid(pno))
				panic("c_free");
			*sysmem.pfree++ = pno;
		} else {
			T_HAL(0x40000, printf("c_free NPL %x ", *qp));
		}
		qp++;
	} while (--sz);
	areafree((BLOCKLIST *)pp, numClicks);
}

/*
 * Given a user virtual address, a physical address, and a byte
 * count, map the specified virtual address into the user data
 * page table for the current process.
 *
 * This is meant to be called from the console ioctl, KDMAPDISP.
 * The user virtual address must be click aligned.
 * The range of physical addresses must lie outside installed RAM
 * or within the "PHYS_MEM" pool.
 *
 * Return 1 on success, else 0.
 */
int
mapPhysUser(virtAddr, physAddr, numBytes)
{
	int ret = 0;
	SR * srp = u.u_segl + SIPDATA;
	SEG * sp = srp->sr_segp;
	cseg_t * pp = sp->s_vmem, * qp;
	int pno, clickOffset, numClicks, i;

	/* Check alignment. */
	if ((virtAddr & (NBPC-1)) || (physAddr & (NBPC-1))) {
		T_HAL(0x40000, printf("mPU: failed alignment "));
		goto mPUdone;
	}

	/* Check validity of range of virtual addresses. */
	if (virtAddr < srp->sr_base ||
	  (virtAddr + numBytes) >= (srp->sr_base + srp->sr_size)) {
		T_HAL(0x40000, printf("mPU: bad vaddr "));
		goto mPUdone;
	}

	/* Check validity of range of physical addresses. */
	/* if not in PHYS_MEM pool... */
	if (!physValid(physAddr, numBytes)) {

		/* get installed RAM physical addresses */
		unsigned int physLow = ctob((read16_cmos(LOMEM) + 3) >> 2);
		unsigned int physHigh = ctob((read16_cmos(EXTMEM) + 3) >> 2)
		  + ONE_MEG;

		T_HAL(0x40000, printf("physLow=%x physHigh=%x ",
		  physLow, physHigh));

		/* Fail if physical range overlaps installed base RAM. */
		if (physAddr < physLow) {
			T_HAL(0x40000, printf("mPU: overlap base RAM "));
			goto mPUdone;
		}

		/* Fail if physical range overlaps installed extended RAM. */
		if (physAddr < physHigh && (physAddr + numBytes) >= ONE_MEG) {
			T_HAL(0x40000, printf("mPU: overlap extended RAM "));
			goto mPUdone;
		}
	}

	/*
	 * For each click in user data segment which is to be remapped
	 *   if current click was taken from sysmem pool
	 *     return current click to sysmem pool
	 *   write new physical address into current click entry
	 *   mark current click as not coming from sysmem pool
	 *   map current click into page table
	 */
	clickOffset = btocrd(virtAddr - srp->sr_base);
	numClicks = numBytes >> BPCSHIFT;
	for (qp = pp + clickOffset, i = 0; i < numClicks; i++, qp++) {
		if ((*qp & SEG_NPL) == 0) {
			pno = segclick(*qp);
			if (!pvalid(pno)) {
				T_HAL(0x40000, printf("mPU: bad release "));
			} else {
				*sysmem.pfree++ = pno;
				T_HAL(0x40000,
				  printf("mPU: freeing virtual click %x ",
				  virtAddr + ctob(i)));
			}
		} else {
			T_HAL(0x40000,
			  printf("mPU: rewriting virtual NPL click %x ",
			  virtAddr + ctob(i)));
		}
		*qp = (physAddr + ctob(i)) | (SEG_RW | SEG_NPL);
		ptable1_v[btocrd(virtAddr) + i] = *qp;
	}
	mmuupd();
	ret = 1;

mPUdone:
	return ret;
}

/*
 * Add a click to a segment.
 * Enlarge buddy table for segment, if needed.
 *
 * Arguments:
 *	pp points to segment reference table (segp->s_vmem, e.g.)
 *	osz is old segment size, in clicks
 *
 * Return pointer to enlarged segment reference table, or NULL if failed.
 */
cseg_t *
c_extend(pp, osz) 
register cseg_t *pp;
int osz;
{
	register	cseg_t *pp1;
	register unsigned	pno;
	register int	i;
	SR		*srp;

	/* Fail if no more free clicks available. */
	if (sysmem.pfree < &sysmem.tfree[1])
		goto no_c_extend;

	/* Don't grow segment beyond hardware segment size (4 megabytes). */
	if (osz >= (NBPS/NBPC))
		goto no_c_extend;

	if (srp = loaded(pp)) {
		unload(srp);
		srp->sr_segp = 0;
	}

	/*
	 * If the old size was a power of 2, it has used up an entire
	 * buddy area, so we will need to allocate more space.
	 */
	if (IS_POW2(osz)) {
		if ((pp1 = (cseg_t*) arealloc(osz+1))==0)
			goto no_c_extend;
		for (i=0; i < osz; i++)
			pp1[i] = pp[i];
		areafree(pp, osz);
		pp = pp1;
	}

	for (i=osz; --i >= 0;)
		pp[i+1] = pp[i];

	pno = *--sysmem.pfree;
	if (!pvalid(pno))
		panic("c_extend");
	pp[0] = clickseg(pno) | SEG_RW;
	return pp;

no_c_extend:
	return 0;
}

/*
 * Given segment size in bytes, estimate total space needed
 * to keep track of the segment (I think - hws).
 *
 * return value is num_bytes plus some overhead...
 */
int
countsize(num_bytes)
int num_bytes;
{
	int ret;

	if (num_bytes <= NBPC/sizeof(long))
		ret = num_bytes+1;
	else
		ret = num_bytes
		  + ((num_bytes + NBPC/sizeof(long) - 1) >> BPC1SHIFT) + 1;
	return ret;
}

/*
 * buddy allocation 
 */

/*
 * Deallocate a segment descriptor area.
 * "sp" is not really a BLOCKLIST*, rather a cseg_t *.
 * "numClicks" is the number of clicks referenced in the area.
 */
void
areafree(sp, numClicks)
BLOCKLIST *sp;
int numClicks;
{
	register int	n;	/* adresse du buddy, taille du reste */
	register int	ix, nx;
	register	BLOCKLIST *buddy;

	areacheck(2, sp);

	/*
	 * Pointer "sp" points to an element in the sysmem table of
	 * free clicks.
	 * Integer "ix" is the index of "sp" into that table.
	 * Will use "ix" to index into one or more buddy tables.
	 */
	ix = sp - sysmem.u.budtab;
	n = areasize(numClicks);
	do {
		/* "nx" is index of buddy element to the one at "ix". */
		nx = BUDDY(ix, n);
		if (sysmem.budfree[nx>>WSHIFT] & 1<<(nx&(WCOUNT-1))) {
			/* coalesce two buddies */
			buddy = sysmem.u.budtab + nx; 
			if (buddy->kval != n)
				break;
			sysmem.budfree[nx>>WSHIFT] &= ~ (1<<(nx & (WCOUNT-1)));
			DELETE2(buddy);
			if (nx < ix) 
				ix = nx;
		} else
			break;
	} while (++n < NBUDDY);
	sysmem.budfree[ix>>WSHIFT] |= 1 << (ix & (WCOUNT-1));
	buddy = sysmem.u.budtab + ix;
	INSERT2(BLOCKLIST, buddy, &sysmem.bfree[n]);
	buddy->kval = n;
	areacheck(3, buddy);
}

/*
 * arealloc()
 *
 * Given size in "clicks" of a segment to manage,
 * return pointer to an array of enough descriptors.
 * If not enough free descriptors available, return 0.
 */
BLOCKLIST *
arealloc(clicks)
register int clicks;
{
	register	BLOCKLIST *sp;
	register	BLOCKLIST *p, *q;
	register int	size;
	BLOCKLIST	*rsp;
	register int	nx;

	areacheck(0, 0);
	size = areasize(clicks);
	/*
	 * 1. Find little end, bloc p, free >= size
	 */
	for (q = p = sysmem.bfree + size;p->forw == p; size++, p++)
		if (p >= sysmem.bfree + NBUDDY - 1) {
			return(0);	/* y en a pas */
		}

	rsp = p->forw;
	DELETE2(rsp);
	nx = rsp - sysmem.u.budtab;
	sysmem.budfree[nx>>WSHIFT] &= ~(1 << (nx & (WCOUNT-1)));
	size = 1<<size;
	sp = rsp + size; /* buddy address */
	while (p-- != q) {
		/*
		 * 2.1 The block is too big, uncouple & free buddy
		 */
		sp -= (size >>= 1);
		nx = sp - sysmem.u.budtab;
		sysmem.budfree[nx>>WSHIFT] |= 1 << (nx & (WCOUNT-1));
		INSERT2(BLOCKLIST, sp, p);
		sp->kval = p - sysmem.bfree;
	}
	areacheck(1, rsp);
	return rsp;
}

void
areainit(n)
{
	extern char __end[];
	register int i;

	for (i=0; i < (1<<(NBUDDY-WSHIFT)); i++)
		sysmem.budfree[i] = 0;
	for (i=0; i<NBUDDY; i++)
		INIT2(&sysmem.bfree[i]);
	sysmem.u.budtab = (BLOCKLIST *)__end;
	n /= sizeof(BLOCKLIST);
	if (n > (1 << NBUDDY))
		panic("areainit");
	for (i=0; i<n; i++)
		areafree(&sysmem.u.budtab[i], sizeof(BLOCKLIST)/sizeof(long));
}

/*
 * areasize()
 *
 * Do a log(base 2) calculation on n.
 * If n is zero, return -1.
 *
 * Else, consider the nearest power of two which is greater than or
 * equal to n
 *	p/2 < n <= p
 * Then set p = 4 * (2**x).  Note BLKSZ is 2.
 * Return max(x,0).
 *
 * If n is too large (more than 3F00), we will go beyond the limits of
 * table buddysize[].
 *
 * In practice, n is the total number of clicks needed in a segment,
 * and the return value will be used to access a buddy system list.
 */
int
areasize(n)
register unsigned int	n;
{
	register int m;
#ifdef FROTZ
	int ret, oldn = n;
#endif

	if (n > 0x3F00)
		panic("areasize");

	n = (n + (1 << BLKSZ) - 1) >> BLKSZ;
	m = n & 0x3F;
#ifdef FROTZ
	if ((n >>= 6) == 0)
		ret = buddysize[m];
	else {
		int index;

		index = n;
		if (m)
			index++;
		ret = buddysize[index] + 6;
	}
	return ret;
#else
	if ((n >>= 6) == 0)
		return buddysize[m];
	return buddysize[n + ((m!=0)?1:0)] + 6;
#endif
}

#define	MAXBUDDY	2048
#define	CHECK(p) ((p>=&sysmem.bfree[0] && p<&sysmem.bfree[NBUDDY]) || \
		(p>=sysmem.u.budtab && p<&sysmem.u.budtab[1<<NBUDDY]))
void
areacheck(flag, sp)
register	BLOCKLIST *sp;
{
	register	BLOCKLIST *next, *start;
	register int i, nx;

	if (sp) {
		if (&sysmem.u.budtab[sp-sysmem.u.budtab] != sp)
		  printf("*check* %d %x %x\n", flag, sp, sysmem.u.budtab);
	}
		
	for (i=0; i<NBUDDY; i++) {
		start = next = &sysmem.bfree[i];
		do {
			next = next->forw;
			if (!CHECK(next))
				printf("next = %x (%d)\n", next, flag);
			if (next->back != start)
				printf("%x->forw->back != %x\n", next, start);
			if (next != &sysmem.bfree[i]) {
				if (next->kval != i)
					printf("bad kval %x, %d (%d)\n",
						next, next->kval, flag);
				nx = next - sysmem.u.budtab;
				if ((sysmem.budfree[nx>>WSHIFT] & (1 << (nx & (WCOUNT-1)))) == 0)
					printf("in bfree but not budfree %x (%d)\n", next, flag);
			}
			start = next;
		} while (next != &sysmem.bfree[i]);
	}
}

MAKESR(physMem, _physMem);
int	PHYS_MEM = 0;		/* Number of bytes of contiguous RAM needed */

/*
 * A block of contiguous physical memory has been allocated for special
 * i/o devices.
 * Problem: clicks of physical memory are in reverse order in the
 * page table.
 * This routine reverses the page table entries for the pages
 * involved.  It relies *heavily* on all pages having virtual addresses
 * in the FFCx xxxx segment.
 *
 * If all goes well, assign physAvailStart to the virtual address of
 * the beginning of the region, and physAvailBytes to the number of bytes
 * in the region.  Otherwise, leave physAvailStart and physAvailBytes at 0.
 *
 * As memory is allocated, physAvailStart advances to point to the next
 * available byte of contiguous memory, physAvailBytes is decremented,
 * and physPoolStart remains set to the virtual address of the start of
 * the contiguous pool.
 */
static int	physPoolStart;	/* start of contiguous memory area */
static int	physAvailStart;	/* next free byte in contiguous memory area */
static int	physAvailBytes;	/* number of bytes in contiguous memory area */

/*
 * Check whether a range of physical addresses lies within the
 * pool of contiguous physical memory.
 */
int
physValid(base, numBytes)
unsigned int base, numBytes;
{
	int vpool;
	int ret = 0;

	if (PHYS_MEM) {
		vpool = vtop(physPoolStart);
		T_HAL(0x40000, printf("PHYS_MEM phys addrs %x to %x  ",
		  vpool, vpool + PHYS_MEM));
		if (base >= vpool && (base + numBytes) <= (vpool + PHYS_MEM))
			ret = 1;
	} else {
		T_HAL(0x40000, printf("No PHYS_MEM "));
	}

	T_HAL(0x40000, printf("physValid(%x, %x) = %d ", base, numBytes, ret));
	return ret;
}

void
physMemInit()
{
	int m, vaddr;
	int err = 0, num_clicks = btoc(PHYS_MEM);
	int prevPaddr, paddr;

	/*
	 * Going half way into page table for physMem
	 *   If entry and its complementary entry aren't both in top segment
	 *     Error exit (no phys mem will be available).
	 *   Get page table entries and swap them.
	 */
	for (m = 0; m < num_clicks/2; m++) {
		int m2 = num_clicks - 1 - m;	/* complementary index */

		/* compute virtual addresses */
		int lo_addr = physMem.sr_base + ctob(m);
		int hi_addr = physMem.sr_base + ctob(m2);

		/* compute indices into page table (ptable1_v) */
		int lo_p1ix = btocrd(lo_addr);
		int hi_p1ix = btocrd(hi_addr);

		/* fetch physical addresses from page table */
		int lo_paddr = ptable1_v[lo_p1ix];
		int hi_paddr = ptable1_v[hi_p1ix];

		/* abort if either address is not in top segment */
		if (btosrd(lo_addr) != 0x3FF) {
			err = 1;
			break;
		}
		if (btosrd(hi_addr) != 0x3FF) {
			err = 1;
			break;
		}

		/* exchange page table entries */
		ptable1_v[lo_p1ix] = hi_paddr;
		ptable1_v[hi_p1ix] = lo_paddr;
	}

	/*
	 * Final sanity check.
	 * In case someone gets creative with startup code, check
	 * again here that the memory is actually contiguous.
	 */
	prevPaddr = vtop(physMem.sr_base);
	for (m = 0; m < num_clicks - 1; m++) {
		paddr = vtop(physMem.sr_base + ctob(m + 1));
		if (paddr - prevPaddr != NBPC) {
			err = 1;
			break;
		}
		prevPaddr = paddr;
	}

	if (!err) {
		physPoolStart = physAvailStart = physMem.sr_base;
		physAvailBytes = PHYS_MEM;
	}
}

/*
 * Return virtual address of block of contiguous physical memory.
 * If request cannot be granted, return 0.
 *
 * Expect physMem resource to be granted during load routine of device
 * drivers.  Once allocated, memory is not returned to the physMem pool.
 */
char *
getPhysMem(numBytes)
unsigned int numBytes;
{
	char * ret = NULL;

	if (numBytes <= physAvailBytes) {
		ret = (char *)physAvailStart;
		physAvailStart += numBytes;
		physAvailBytes -= numBytes;
	} else
		printf("getPhysMem failed - %d additional bytes "
		  "PHYS_MEM needed\n", physAvailBytes - numBytes);
	return ret;
}

/*
 * Return virtual address of aligned block of contiguous physical memory.
 * Mainly for devices using the stupid Intel DMA hardware without
 *   scatter/gather.
 * If request cannot be granted, return 0.
 *
 * Argument "align" says what physical boundary we need alignment on.
 * It must be a power of 2.
 * For 4k alignment, align = 4k, etc.
 * Sorry, but will throw away memory to get to the next acceptable address.
 *
 * Once allocated, memory is not returned to the physMem pool.
 */
char *
getDmaMem(numBytes, align)
unsigned int numBytes;
unsigned int align;
{
	char * ret = NULL;
	int wastedBytes, neededBytes;

	if (align == 0) {
		printf("getDmaMem(0) (?)\n");
		goto getDmaMemDone;
	}

	if (!IS_POW2(align)) {
		printf("getDmaMem(%x) (?)\n", align);
		goto getDmaMemDone;
	}

	/*
	 * Waste RAM from bottom of pool up to physical
	 * address with desired alignment.
	 */
	wastedBytes = align - (vtop(physAvailStart) % align);
	neededBytes = numBytes + wastedBytes;

	if (neededBytes <= physAvailBytes) {
		ret = (char *)physAvailStart + wastedBytes;
		physAvailStart += neededBytes;
		physAvailBytes -= neededBytes;
	} else
		printf("getDmaMem failed - %d additional bytes "
		  "PHYS_MEM needed\n", physAvailBytes - neededBytes);

getDmaMemDone:
	return ret;
}
/***************/

#undef	ptable1_v

/*
 * pageDir is the physical address of the click in use for the page
 * directory, offset by ctob(SBASE - PBASE)
 */
#define	pageDir		((long *)(&stext[ctob(-1)]))

int total_clicks;	/* How many clicks did we start with?  */

void
mchinit()
{
	extern char __end[], __end_data[], stext[], __end_text[], sdata[];
	extern int RAM0, RAMSIZE;

	int lo;		/* Number of bytes of physical memory below 640K.  */
	int hi;		/* Number of bytes of physical memory above 1M.  */
	register char *pe; 
	register int zero = 0;
	register int	i;
	register	long *ptable1_v;
	register unsigned short	base;
	int	sysseg, codeseg, stackseg, ramseg, ptable1;
	int	ptoff;	/* An offset into pageDir[]  */
#if USE_NDATA
	int	dataseg[NDATA];
#else
	int	dataseg;
#endif
	int	nalloc;
	extern char	digtab[];
	static	SEG	uinit;
	int	budArenaBytes;	/* number of bytes in buddy pool */
	int	kerBytes;	/* number of bytes in kernel text and data */

	/*
	 * 1.
	 *   a. Relocate the data on a page boundary (4K bytes) the
	 *      bootstrap relocates it on a paragraph boundary (16 bytes)
	 *
	 *   b. Verify that the data has been relocated correctly 
	 */
	pe = __end_data;					/* 1.a */
	i = (((unsigned)__end_text+15) & ~15) - (unsigned)sdata;
	do {
		pe--;
		pe[0] = pe[i];
	} while (pe != sdata);					/* 1.b */

	/*
	 * Can now access the .data segment from C.
	 * If not, next loop will hang the kernel.
	 */
	CHIRP('A');
	while (digtab[0]!='0');
	CHIRP('*');

	/* Zero the bss. */
	pe = __end_data;
	do
		*pe++ = zero;
	while (pe != __end);

	/*
	 * Zero the level 0 page directory, which occupies the click
	 * of virtual space immediately below kernel text.
	 */
	pe = (char *) pageDir;
	do
		*pe++ = zero;
	while (pe != stext);

	CHIRP('2');

	/*
	 * 3. Calculate total system memory.
	 *    Count the space used by the system and the page
	 *    descriptors, the interrupt stack, and the refresh work area
	 *
	 * a. initialize allocation area and adjust system size
	 *    to take allocation area and free page area into account
	 */

	/*
	 * btoc(__end) - SBASE is the number of clicks in kernel text
	 * plus data, rounded up.
	 * PBASE is the starting physical click number of the kernel.
	 *
	 * Set sysmem.lo to the physical click address just past the kernel.
	 */
	DV(__end);

	kerBytes = __end - ((SBASE - PBASE)<<BPCSHIFT);
	DV(kerBytes);

	sysmem.lo = btoc(kerBytes);
	DV(sysmem.lo);

	/*
	 * lo is the size in bytes of memory between the end of the kernel
	 *	and the end of memory below 640K.
	 * hi is the size in bytes of memory over 1 Megabyte (Extended memory).
	 *
	 * Round the sizes from the CMOS down to the next click.  This
	 * compensates for systems where the CMOS reports sizes that are
	 * not multiples of 4K.
	 */
	DV(read16_cmos(LOMEM));
	lo = ctob(read16_cmos(LOMEM) >> 2) - ctob(sysmem.lo);
	DV(lo);

	DV(read16_cmos(EXTMEM));
	hi = ctob(read16_cmos(EXTMEM) >> 2);
	DV(hi);

	/*
	 * Sometimes, we die horribly if there is too much memory.
	 * Artificially limit hi to HACK_LIMIT.
	 */
	if (hi > HACK_LIMIT)
		hi = HACK_LIMIT;

	/* clear base memory above the kernel */
	CHIRP('z');
	zero_fill(ctob(sysmem.lo+SBASE-PBASE), lo);
	CHIRP('Z');

	/* clear extended memory */
	zero_fill(ONE_MEG+ctob(SBASE-PBASE), hi);
	CHIRP('Y');
	
	/* Record total memory for later use.  */
	total_mem = ctob(sysmem.lo) + lo + hi;
	DV(total_mem);

	/*
	 * sysmem.pfree and relatives will keep track of a pool of 4k pages
	 * assigned to processes, hereinafter known as the sysmem pool.
	 * How many clicks can go into this pool?  nalloc.
	 * Allow NBPC for the click itself, a short for the sysmem pointer,
	 * and SPLASH*sizeof(long) for buddy system overhead.
	 */
	nalloc = (lo+hi) / (sizeof(short) + SPLASH*sizeof(long) + NBPC);
	DV(nalloc);

	/*
	 * ASSERT:
	 * For the moment we want only to assure that the
	 * BUDDY arena and the stack of free pages will fit below
	 * 640K.
	 */
	budArenaBytes = SPLASH*nalloc*sizeof(long);
	DV(budArenaBytes);

#define SIZEOF_FREE_PAGES ((btoc(hi) + btoc(lo))* sizeof(short))
	T_PIGGY(0x800, {
		if (budArenaBytes + SIZEOF_FREE_PAGES >= lo) {
			panic("Too much memory");
		}
	});

	/*
	 * Initialize the buddy system arena.  This memory is used
	 * for the compressed page tables.
	 */
	areainit(budArenaBytes);

	/*
	 * Initialize the stack of free pages.
	 * __end is the virtual address just past kernel data
	 * Point sysmem.tfree to the lowest virtual address just above
	 * the buddy pool, and initialize sysmem.pfree there.
	 */
	sysmem.tfree = sysmem.pfree = 
	  (unsigned short *)(__end + budArenaBytes);
	DV(sysmem.tfree);

	/* sysmem.hi is the physical click number just past high RAM */
	sysmem.hi = btoc(hi+ONE_MEG);
	DV(sysmem.hi);

	/* base is the physical click number just past base RAM */
	base = sysmem.lo + (lo>>BPCSHIFT);
	DV(base);

	/*
	 * Adjust sysmem.lo to be the physical click number just above
	 * not just the kernel, but above sysmem overhead as well.
	 */
	sysmem.lo = btoc(kerBytes + budArenaBytes + nalloc*sizeof(short));
	DV(sysmem.lo);

	/*
	 * sysmem.vaddre is the virtual address of the next click after the
	 * kernel.
	 */
	sysmem.vaddre = ctob(sysmem.lo+SBASE-PBASE);
	DV(sysmem.vaddre);

	/* include in system area pages for arena, free area */

	CHIRP('3');

	/*
	 * 4.
	 *  Free the memory from [end, 640) kilobytes
	 *  Free the memory from [1024, 16*1024) kilobytes
	 *
	 *  We are building a stack of free pages bounded below
	 *  by sysmem.tfree and above by sysmem.efree.  sysmem.pfree
	 *  is the top of the stack.  The stack grows upwards.
	 */
	total_clicks = 0;

	/*
	 * Initialize the sysmem table (phase 1 - base RAM).
	 * Put base RAM above the kernel and sysmem overhead area into
	 * sysmem pool.
	 */
	while (base > sysmem.lo) {
		*sysmem.pfree++ = --base;
		++total_clicks;
	}

	/*
	 * Initialize the sysmem table (phase 2 - extended RAM).
	 * Put all extended RAM into the sysmem pool.
	 */
	base = btoc(ONE_MEG);
	while (base < sysmem.hi && total_clicks < nalloc) {
		*sysmem.pfree++ = base++;
		++total_clicks;
	}
	DV(total_clicks);

	/*
	 * Roundoff error may have made nalloc smaller than necessary.
	 */
	while(base < sysmem.hi) {
		if (sysmem.pfree + 1 >= sysmem.vaddre)
			break;
		*sysmem.pfree++ = base++;
		++total_clicks;
		nalloc++;
	}
	DV(total_clicks);
	DV(nalloc);

	/*
	 * sysmem.efree points just past the last pointer in the sysmem
	 * table.
	 */
	sysmem.efree = sysmem.pfree;
	DV(sysmem.efree);
	DV(allocno());

	T_PIGGY(0x800, {
		/*
		 * ASSERT:  The stack of free pages should end within a click
		 * of the lowest available memory.
		 */
		if ((cseg_t *)ctob(sysmem.lo+SBASE-PBASE) < sysmem.efree) {
			panic("sysmem.lo is too low");
		}

		if (sysmem.efree < (cseg_t *)ctob(sysmem.lo+SBASE-PBASE - 1)){
			panic("sysmem.efree is too low");
		}

		/*
		 * ASSERT:  There should be nalloc total_clicks.
		 */
		if (nalloc != total_clicks) {
			panic("nalloc != total_clicks ");
		}
	});

	CHIRP('4');

	/*
	 * 5. allocate page entries and initialize level 0 ^'s
	 * a. [ 00000000 .. 003FFFFF)		user code segment
	 * b. [ 00400000 .. 007FFFFF)		user data & bss
	 * c. [ 7FC00000 .. 7FFFFFFF)		user stack
	 *c.i.[ 80000000 .. 80FFFFFF)		ram disk
	 * d. [ FF800000 .. FFBFFFFF)		pointers to level 1 page table
	 * e. [ FFC00000 .. FFFFFFFF)		system process addresses
	 */
	codeseg = clickseg(*--sysmem.pfree);		/* 5.a */
	pageDir[0x000] = codeseg  | DIR_RW; 

#if USE_NDATA
	for (i = 0; i < NDATA; i++) {
		dataseg[i] = clickseg(*--sysmem.pfree);	/* 5.b */
		pageDir[0x001+i] = dataseg[i] | DIR_RW;
	}
#else
	dataseg = clickseg(*--sysmem.pfree);		/* 5.b */
	pageDir[0x001] = dataseg | DIR_RW;
#endif

	stackseg = clickseg(*--sysmem.pfree);		/* 5.c */
	pageDir[0x1FF] = stackseg  | DIR_RW; 

	/*
	 * ptable1 is a handle for the click containing page table
	 * entries for the page table.
	 *
	 * allocate a click for ptable1
	 * Then point at it from the page directory.
	 */
	ptable1 = clickseg(*--sysmem.pfree);		/* 5.d */
	pageDir[0x3FE] = ptable1 | DIR_RW; 

	sysseg = clickseg(*--sysmem.pfree);		/* 5.e */
	pageDir[0x3FF] = sysseg  | DIR_RW;

	CHIRP('5');

	/*
	 * 6. initialize  level 2 ^'s to [5.d]
	 */

	ptable1_v  = (long *)(ptable1 + ctob(SBASE-PBASE));
	DV(pageDir);
	DV(ptable1_v);
	ptable1_v[0x000] = codeseg | SEG_SRW;
#if USE_NDATA
	for (i = 0; i < NDATA; i++)
		ptable1_v[0x001+i] = dataseg[i] | SEG_SRW;
#else
	ptable1_v[0x001] = dataseg | SEG_SRW;
#endif
	ptable1_v[0x1FF] = stackseg| SEG_SRW;

	/*
	 * This ram disk stuff should go away once the scheme
	 * for allocating pieces of virtual memory space is in place.
	 */
	for (ptoff = btosrd(RAM0) & 0x3ff;
	  ptoff < (btosrd(RAM0 + 2 * RAMSIZE) & 0x3ff); ++ptoff) {
		ramseg =  clickseg(*--sysmem.pfree);		/* 5.c.i */
		pageDir[ptoff] = ramseg  | DIR_RW; 
		ptable1_v[ptoff] = ramseg | SEG_SRW;
	}

	ptable1_v[0x3FF] = sysseg  | SEG_SRW;

	CHIRP('6');

	/*
	 * 7.
	 * b. map kernel code and data
	 * 	map ^ to:
	 * c. 	level 0 page table
	 * d. 	level 1 page table
	 * e. 	I/O segments (video RAM, ...) 
	 */ 

	ptable1_v  = (long *)(sysseg + ctob(SBASE-PBASE));	/* 7.b */
	DV(ptable1_v);
	for (i = PBASE; i <sysmem.lo; i++)
		ptable1_v[i-PBASE] = clickseg(i) | SEG_SRW;

	ptable1_v[0x3FE] = clickseg(PTABLE0_P) | SEG_SRW;	/* 7.c */
	ptable1_v[0x3FD] = ptable1 | SEG_SRW;			/* 7.d */

	init_phy_seg(ptable1_v, ROM-SBASE,   0x0000F0000);	/* 7.e. */
	init_phy_seg(ptable1_v, VIDEOa-SBASE,0x0000B0000);
	init_phy_seg(ptable1_v, VIDEOb-SBASE,0x0000B8000);

	CHIRP('7');

	/*
	 * 8. allocate and map U area
	 */

	uinit.s_flags = SFSYST|SFCORE;
	uinit.s_size = UPASIZE;
	uinit.s_vmem = c_alloc(btoc(UPASIZE));
	ptable1_v[0x3FF] = *uinit.s_vmem | SEG_SRW;
	procq.p_segp[SIUSERP] = &uinit;

	CHIRP('8');

	/*
	 * 9. make FFC00000 and 00002000 map to the same address
	 * to prevent the prefetch after the instruction turning on
	 * paging from causing a page fault
	 */
	ptable1_v  = (long *)(codeseg + ctob(SBASE-PBASE));
	DV(ptable1_v);
	ptable1_v[PBASE] = clickseg(PBASE) | SEG_SRW;

	CHIRP('9');

	/*
	 * 10. load page table base address into MMU
	 *	fix up the interrupt vectors
	 */
	mmuupdnR0();
	CHIRP('U');
	idtinit();
	CHIRP('I');
}

typedef struct
{
	unsigned short	off_lo;
	unsigned short	seg;
	unsigned short	flags;
	unsigned short	off_hi;
} IDT;

/*
 * ldtinit()
 *
 * Fix up descriptors which are hard to create properly at compile/link time.
 * Apply to idt and ldt.
 *
 * Swap 16-bit words at descriptor+2, descriptor+6.
 */
void
idtinit()
{
	extern IDT	idt[], idtend[];
	extern IDT	ldt[], ldtend[];
	extern IDT	gdtFixBegin[], gdtFixEnd[];

	register IDT *ip;
	register unsigned short tmp;

	for (ip = idt; ip < idtend; ip++) {
		tmp = ip->off_hi;
		ip->off_hi = ip->seg;
		ip->seg = tmp;
	}

	for (ip = ldt; ip < ldtend; ip++) {
		tmp = ip->off_hi;
		ip->off_hi = ip->seg;
		ip->seg = tmp;
	}

	for (ip = gdtFixBegin; ip < gdtFixEnd; ip++) {
		tmp = ip->off_hi;
		ip->off_hi = ip->seg;
		ip->seg = tmp;
	}
}

void
init_phy_seg(ptable1_v, addr, base)
long	*ptable1_v;
{
	register int i;

	for (i=0; i<btoc(0x10000); i++) {
		ptable1_v[addr+i] = base | SEG_SRW; 
		base += NBPC;
	}
}

/*
 * Load up segmentation registers.
 */
SR	ugmtab[NUSEG];

void
segload()
{
	register int i;
	register	SR *start;

	/*
	 * 1. unprogram the currently active UGM user segments
	 *    reset ugmtab
	 */
	for (start = &ugmtab[1]; start < &ugmtab[NUSEG]; start++) {
		if (start->sr_segp)
			unload(start);
		start->sr_segp = 0;
	}

	/*
	 * 2. Load each segment in the p->p_region list into the MMU
	 *    Remember values in ugmtab.
	 */
	start = &ugmtab[1];
	for (i = 1; i < NUSEG; i++) {
		if (u.u_segl[i].sr_segp) {
			*start = u.u_segl[i];
			switch (i) {
			case SIPDATA:
				if (u.u_segl[SISTACK].sr_base)
					start->sr_size = min(start->sr_size,
					  (long)u.u_segl[SISTACK].sr_base-
					  u.u_segl[SISTACK].sr_size);
				break;
			case SISTACK:
				start->sr_base -= start->sr_size;
				break;
			}

			start->sr_segp = 0;
			if (SELF->p_segp[i]) {
				start->sr_segp = SELF->p_segp[i];
				doload(start);
			}
			start++;
		}
	}

	/* 3.  Update shm segment information. */
	shmLoad();
}

SR *
loaded(pp)
register cseg_t *pp;
{
	register SR	*start;

	for (start = ugmtab; start < ugmtab + NUSEG; start++) {
		if (start->sr_segp && start->sr_segp->s_vmem == pp) {
			return start;
		}
	}
	return 0;
}

MAKESR(r0stk, _r0stk);
extern int tss_sp0;

/*
 * General initialization
 */
void
i8086()
{
	unsigned	csize, isize, ssize, allsize;
	caddr_t	base;
	unsigned int	calc_mem, boost;

	/* This is the first C code executed after paging is turned on. */

	workPoolInit();

	/*
	 * Allocate contiguous physical memory if PHYS_MEM is patched
	 * to a nonzero value.
	 */
	if (PHYS_MEM) {
		physMem.sr_size = (PHYS_MEM+NBPC-1)&~(NBPC-1);
		valloc(&physMem);
		physMemInit();
	}

	/*
	 * Allocate a click for ring 0 stack.
	 */
	r0stk.sr_size = NBPC;
	valloc(&r0stk);
	tss_sp0 = r0stk.sr_base + NBPC;

	/*
	 * calc_mem is used for autosizing buffer cache and kalloc pool.
	 * It is total_mem, limited below by 1 meg and above by 12 meg.
	 * The upper limit is a temporary move to allow booting on 16 Meg
	 * systems.
	 *
	 * "boost" is used in autosizing buffer cache and kalloc pool.
	 * It is the number of megabytes of calc_mem above 1 meg, i.e.,
	 * a number between 0 and 11.
	 */
	if (total_mem < ONE_MEG)
		calc_mem = ONE_MEG;
	else if (total_mem > 12 * ONE_MEG)
		calc_mem = 12 * ONE_MEG;
	else
		calc_mem = total_mem;

	boost = (calc_mem - ONE_MEG) / ONE_MEG;

	/*
	 * If the number of cache buffers was not explicitly set (i.e., !0)
	 * then calculate the number of buffers using the simple heuristic:
	 *     128 minimum + 400 per MB of available RAM (i.e., after 1MB)
	 */
	if (NBUF == 0)
		NBUF = 128 + (400 * boost);

	/*
	 * If the amount of kalloc() space was not explicitly set (i.e., !0)
	 * then calculate using the simple heuristic:
	 *     64k minimum + 32k per MB of available RAM (i.e., after 1MB)
	 */
	if (ALLSIZE == 0)
		ALLSIZE = 65536 + (32768 * boost);

	blockp.sr_size = NBUF*BSIZE;
	valloc(&blockp);

	allocp.sr_size= allsize = NBUF*sizeof(BUF) + ALLSIZE;
#if USE_SLOT
	allocp.sr_size += ssize = NSLOT * (sizeof(int) + slotsz);
#else
	ssize = 0;
#endif
	allocp.sr_size += isize = NINODE* sizeof(INODE);
	allocp.sr_size += csize = NCLIST* sizeof(CLIST);
	valloc(&allocp);
	base = allocp.sr_base;
	allkp = setarena(base, allsize);
	base += allsize;
#if USE_SLOT
	slotp = (int *)base;
	base += ssize;
#endif
	inodep = (INODE*) base;
	base += isize;
	clistp = (paddr_t)base;
}

/*
 * Allocate srp->sr_size bytes of physical memory, and map it into
 * virtual memory space.  At the end, the struct at srp will describe
 * the new segment.
 */
void
valloc(srp)
SR	*srp;
{
	register int npage;

	/*
	 * If we've run out of virtual memory space, panic().
	 *
	 * A more graceful solution is needed, but valloc() does
	 * not provide a return value.
	 */
	if (sysmem.vaddre + srp->sr_size > MAX_VADDR) {
		panic("valloc: out of virtual memory space");
	}

	npage = btoc(srp->sr_size);

	srp->sr_base = sysmem.vaddre;
	srp->sr_segp->s_size = srp->sr_size;
	srp->sr_segp->s_vmem = c_alloc(npage);
	srp->sr_segp->s_flags = SFSYST|SFCORE;
	doload(srp);

	sysmem.vaddre += ctob(npage);
}

/*
 * See if the given process may fit in core.
 */
int
testcore(pp)
register PROC *pp;
{
	return 1;
}

/*
 * Calculate segmentation for a
 * new program. If there is a stack segment
 * present merge it into the data segment and
 * relocate the argument list.
 * Make sure that the changes are reflected in the u.u_segl array
 * which sproto sets up.
 */
int
mproto()
{
	return 1;
}

int
accdata(base, count)
unsigned	base, count;
{
	SR *srp;

	srp = &u.u_segl[SIPDATA];
	return base>=srp->sr_base && base+count <= srp->sr_base+srp->sr_size;
}

int
accstack(base, count)
unsigned	base;
{
	SR *srp;

	srp = &u.u_segl[SISTACK];
	return base>=srp->sr_base-srp->sr_size && base+count<=srp->sr_base;
}

int
acctext(base, count)
unsigned	base;
{
	SR *srp;

	srp = &u.u_segl[SISTEXT];
	return base>=srp->sr_base && base+count <= srp->sr_base+srp->sr_size;
}

printhex(v, max)
unsigned long v;
{
	register int i;

	for (i = max-1; i>=0; --i) 
		putchar(digtab[(v >> (i*4)) & 0xF]);
}

/* Read a 16 byte number from the CMOS.  */
unsigned int
read16_cmos(addr)
unsigned int addr;
{
        unsigned char read_cmos();
	
	return((read_cmos(addr+1)<<8) + read_cmos(addr));
} /* read16_cmos() */

int
c_grow(sp, new_bytes)
SEG *sp;
int new_bytes;
{
	register int	i;
	register cseg_t *pp;
	int		new_clicks, pno, nsize, old_clicks;
	SR		*srp;

	T_PIGGY(0x8000000, printf("c_grow(sp: %x, new: %x)", sp, new_bytes););

	new_clicks = btoc(new_bytes);
	old_clicks = btoc(sp->s_size);

	if (new_clicks == old_clicks) {
		goto ok_c_grow;
	}

	if (new_clicks < old_clicks) {
		printf("%s:can't contract segment\n",u.u_comm);
		goto no_c_grow;
	}

	if (new_clicks - old_clicks > allocno()) {
		goto no_c_grow;
	}

	T_PIGGY(0x8000000, printf("nc: %x, oc: %x,",new_clicks,old_clicks););

	/*
	 * Allocate a new descriptor vector if necessary.
	 * pp is the element corresponding to the virtual address
	 * "0"(sr_base)
	 */
	pp = sp->s_vmem;
	nsize = areasize(new_clicks);
	if (nsize != areasize(old_clicks)
	  && !(pp = (cseg_t*)arealloc(new_clicks))) {
		T_PIGGY(0x8000000,
			 printf("Can not allocate new descriptor."););
		goto no_c_grow;
	}

	T_PIGGY(0x8000000, printf("new pp: %x", pp););

	if (0 != (srp = loaded(sp->s_vmem))) {
		T_PIGGY(0x8000000, printf("unloading srp: %x, ", srp););
		unload(srp);
		srp->sr_segp = 0;
	}

	/*
	 * Allocate new descriptors.
	 */
	T_PIGGY(0x8000000, printf("new desc: ["););
	for (i = old_clicks; i < new_clicks; i++) {
		pno = *--sysmem.pfree;
		pp[i] = clickseg(pno) | SEG_RW;
		T_PIGGY(0x8000000, printf("%x, ", pp[i]););
	}
	T_PIGGY(0x8000000, printf("]"););

	/*
	 * Copy unchanged descriptors and free old vector if necessary.
	 */
	if (pp != sp->s_vmem) {
		T_PIGGY(0x8000000, printf("old desc: ["););
		for (i = 0; i < old_clicks; i++) {
			pp[i] = sp->s_vmem[i];
			T_PIGGY(0x8000000, printf("%x, ", pp[i]););
		}
		T_PIGGY(0x8000000, printf("]"););
		areafree((BLOCKLIST*)sp->s_vmem, old_clicks);
	}

	sp->s_vmem = pp;

	/*
	 * clear the added clicks
	 *
	 * MAPIO macro - convert array of page descriptors, offset
	 *   into system global address.
	 */
	T_PIGGY(0x8000000, printf("dmaclear(%x, %x, 0)", 
				ctob(new_clicks - old_clicks),
				MAPIO(sp->s_vmem, ctob(old_clicks))
			   );
	); /* T_PIGGY() */

	dmaclear(ctob(new_clicks - old_clicks),
	  MAPIO(sp->s_vmem, ctob(old_clicks)), 0);

ok_c_grow:
	return 0;

no_c_grow:
	return -1;
}
