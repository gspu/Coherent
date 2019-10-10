/*
 * these routines are written in C until the 386 compiler/assembler
 * are available
 *
 * Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
 */

#include <sys/coherent.h>
#include <sys/reg.h>
#include <sys/clist.h>
#include <errno.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <signal.h>
#include <sys/uproc.h>
#include <sys/buf.h>

#define	min(a, b)	((a) < (b) ? (a) : (b))

/*
 * dmacopy()
 *
 * Copy "npage" 4 kbyte pages from phys addr "from" to phys addr "to".
 */
dmacopy(npage, from, to) 
long	npage;
cseg_t	*from, *to;
{
	int save = setspace(SEG_386_KD);

	while (npage--) {
		int work = workAlloc();	/* Get a virtual click pair. */
		ptable1_v[work] = *from++ | SEG_SRW;
		ptable1_v[work + 1] = *to++ | SEG_SRW;
		mmuupd();
		copyseg_d(NBPC, ctob(work), ctob(work + 1));
		workFree(work);
	}
	setspace(save);
}

/*
 * dmaclear()
 *
 * Given a byte count, a system global address, and a fill value,
 * write the fill value through the given range of memory.
 */
dmaclear(nbytes, to, fill)
long	nbytes, fill;
paddr_t	to;
{
	unsigned off;
	int	n;
	cseg_t *base;
	int save = setspace(SEG_386_KD);
	int work = workAlloc();	/* Get a virtual click pair. */

	off = to & (NBPC-1);
	base = &sysmem.u.pbase[btocrd(to)];
	n = min(nbytes, NBPC-off);
	ptable1_v[work] = *base++ | SEG_SRW;
	mmuupd();
	
	clearseg_d(n, ctob(work)+off, fill);
	nbytes -= n;

	while (nbytes >= NBPC) {
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
		clearseg_d(NBPC, ctob(work), fill);
		nbytes -= NBPC;
	}

	if (nbytes) {
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
		clearseg_d(nbytes, ctob(work), fill);
	}
	setspace(save);
	workFree(work);
}

/*
 * dmain()
 * 
 * Copy in "nbytes" from system global address "to" to kernel address
 * "vaddr".
 */
dmain(nbytes, to, vaddr)
long	nbytes;
paddr_t	to;
caddr_t	vaddr;
{
	unsigned off;
	unsigned	n, n1;
	cseg_t* base;
	int work = workAlloc();	/* Get a virtual click pair. */
	int save = setspace(SEG_386_KD);

	off = to & (NBPC-1);
	base = &sysmem.u.pbase[btocrd(to)];

	n = min(nbytes, NBPC-off);
	ptable1_v[work] = *base++ | SEG_SRW;
	mmuupd();

	if (nbytes==n) {
		/*
		 * only one page
		 * n = min(n & (sizeof(long)-1), n)
		 * copy n bytes; nbytes -= n;
		 * copy (nbytes >> 2) long words; nbytes &= sizeof(long)-1
		 * copy nbytes bytes
		 */
		if (n >= sizeof(long))
			n &= sizeof(long)-1;
		if (n)
			copyseg_b(n, ctob(work)+off, vaddr);
		off += n;
		vaddr += n;
		nbytes -= n;
		if (n = nbytes & ~(sizeof(long)-1)) {
			copyseg_d(n, ctob(work)+off, vaddr);
			off += n;
			vaddr += n;
			nbytes -= n;
		}
	} else {
		/*
		 * more than one page
		 * copy n&3 bytes
		 * copy n >> 2 long words
		 * in the first page
		 */			
		if (n1 = n & 3)
			copyseg_b(n1, ctob(work)+off, vaddr);
		off += n1;
		vaddr += n1;
		nbytes -= n1;
		if (n = n & ~(sizeof(long)-1)) {
			copyseg_d(n, ctob(work)+off, vaddr);
			off += n;
			vaddr += n;
			nbytes -= n;
		}

		/*
		 * copy nbytes>>BPCSHIFT full pages
		 */
		while (nbytes >= NBPC) {
			ptable1_v[work] = *base++ | SEG_SRW;
			mmuupd();
	
			copyseg_d(NBPC, ctob(work), vaddr);
			vaddr += NBPC;
			nbytes -= NBPC;
		}
		/*
		 * page n-1 (last one)
		 *
		 * copy nbytes>>2 long words
		 * copy nbytes & 3 bytes
		 */
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
	
		if (n = nbytes & ~(sizeof(long)-1)) {
			copyseg_d(n, ctob(work), vaddr);
			vaddr += n;
			nbytes -= n;
		}
		if (nbytes)
			copyseg_b(nbytes, ctob(work)+n, vaddr);
	}

	setspace(save);
	workFree(work);
}

/*
 * dmaout()
 * 
 * Copy out "nbytes" from kernel address "vaddr" to system global address
 * "to".
 */
dmaout(nbytes, to, vaddr)
long	nbytes;
paddr_t	to;
caddr_t	vaddr;
{
	unsigned off;
	unsigned	n, n1;
	cseg_t *base;
	int work = workAlloc();	/* Get a virtual click pair. */
	int save = setspace(SEG_386_KD);

	off = to & (NBPC-1);
	base = &sysmem.u.pbase[btocrd(to)];

	n = min(nbytes, NBPC-off);
	ptable1_v[work] = *base++ | SEG_SRW;
	mmuupd();

	if (nbytes==n) {
		/*
		 * only one page
		 * n = min(n & (sizeof(long)-1), n)
		 * copy n bytes; nbytes -= n;
		 * copy (nbytes >> 2) long words; nbytes &= sizeof(long)-1
		 * copy nbytes bytes
		 */
		if (n1 = n & (sizeof(long)-1))
			copyseg_b(n1, vaddr, ctob(work)+off);
		off += n1;
		vaddr += n1;
		nbytes -= n1;
		if (n = nbytes & ~(sizeof(long)-1)) {
			copyseg_d(n, vaddr, ctob(work)+off);
			off += n;
			vaddr += n;
			nbytes -= n;
		}
	} else {
		/*
		 * more than one page
		 * copy n&3 bytes
		 * copy n >> 2 long words
		 * in the first page
		 */			
		if (n1 = n & (sizeof(long)-1))
			copyseg_b(n1, vaddr, ctob(work)+off);
		off += n1;
		vaddr += n1;
		nbytes -= n1;
		if (n = n & ~(sizeof(long)-1)) {
			copyseg_d(n, vaddr, ctob(work)+off);
			off += n;
			vaddr += n;
			nbytes -= n;
		}

		/*
		 * copy nbytes>>BPCSHIFT full pages
		 */
		while (nbytes >= NBPC) {
			ptable1_v[work] = *base++ | SEG_SRW;
			mmuupd();
			copyseg_d(NBPC, vaddr, ctob(work));
			vaddr += NBPC;
			nbytes -= NBPC;
		}

		/* now the transfer to memory is click-aligned */
		off = 0;

		/*
		 * page n-1 (last one)
		 *
		 * copy nbytes>>2 long words
		 * copy nbytes & 3 bytes
		 */
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
	
		if (n = nbytes & ~(sizeof(long)-1)) {
			copyseg_d(n, vaddr, ctob(work));
			vaddr += n;
			off += n;
			nbytes -= n;
		}
		if (nbytes)
			copyseg_b(nbytes, vaddr, ctob(work)+off);
	}

	setspace(save);
	workFree(work);
}

/*
 * dmaio2()
 * 
 * Copy in "nbytes" from an I/O port "port" to the system global address
 * "to".
 */
dmaio2(nbytes, to, port)
long	nbytes, port;
paddr_t	to;
{
	unsigned off;
	int	n;
	cseg_t *base;
	int save = setspace(SEG_386_KD);
	int work = workAlloc();	/* Get a virtual click pair. */

	off = to & (NBPC-1);
	base = &sysmem.u.pbase[btocrd(to)];

	n = min(nbytes, NBPC-off);
	ptable1_v[work] = *base++ | SEG_SRW;
	mmuupd();
	
	io2seg(n, ctob(work)+off, port);
	nbytes -= n;

	while (nbytes >= NBPC) {
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
		io2seg(NBPC, ctob(work), port);
		nbytes -= NBPC;
	}

	if (nbytes) {
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
		io2seg(nbytes, ctob(work), port);
	}
	setspace(save);
	workFree(work);
}

/*
 * dma2io()
 * 
 * Copy out "nbytes" from the system global address "from" to an I/O port
 * "port".
 */
dma2io(nbytes, to, port)
long	nbytes, port;
paddr_t	to;
{
	unsigned off;
	int	n;
	cseg_t *base;
	int save = setspace(SEG_386_KD);
	int work = workAlloc();	/* Get a virtual click pair. */

	off = to & (NBPC-1);
	base = &sysmem.u.pbase[btocrd(to)];

	n = min(nbytes, NBPC-off);
	ptable1_v[work] = *base++ | SEG_SRW;
	mmuupd();
	
	seg2io(n, ctob(work)+off, port);
	nbytes -= n;

	while (nbytes >= NBPC) {
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
		seg2io(NBPC, ctob(work), port);
		nbytes -= NBPC;
	}

	if (nbytes) {
		ptable1_v[work] = *base++ | SEG_SRW;
		mmuupd();
		seg2io(nbytes, ctob(work), port);
	}
	setspace(save);
	workFree(work);
}

/*
 * pxcopy()
 *
 * copy "n" bytes of data at kernel address "v" from address "uo" in:
 * 	system global address space		(space&SEG_VIRT)
 * 	physical memory				!(space&SEG_VIRT)
 * Rights are determined by (space&~SEG_VIRT):
 * 	"v" can be anywhere in kernel address space	SEG_386_KD
 * 	"v" must be an address accessible to the user	SEG_386_UD
 * Up to one click of data can be copied. No alignment restrictions
 * on "uo" apply.
 */
pxcopy(uo, v, n, space)
unsigned	uo;
char	*v;
register int n;
{
	cseg_t *base;
	register	int save, err;
	int work;

	if (n > NBPC)
		return 0;

	work = workAlloc();
	if (space & SEG_VIRT) {
		space &= ~SEG_VIRT;
		base = &sysmem.u.pbase[btocrd(uo)];
		ptable1_v[work] = *base++ | SEG_SRW;
		ptable1_v[work + 1] = *base++ | SEG_SRW;
	} else {
		ptable1_v[work] = (uo&~(NBPC-1)) + SEG_SRW;
		ptable1_v[work + 1] = (uo&~(NBPC-1)) + NBPC + SEG_SRW;
	}
	mmuupd();
	save = setspace(space);

	err = ukcopy(ctob(work) + (uo&(NBPC-1)), v, n);
	setspace(save);
	workFree(work);
	return err;
}

/*
 * xpcopy()
 * 
 * copy "n" bytes of data from kernel address "v" to address "uo" in:
 *      system global address space                      (space&SEG_VIRT)
 *      physical memory                                 !(space&SEG_VIRT)
 * Rights are determined by (space&~SEG_VIRT):
 *      "v" can be anywhere in kernel address space      SEG_386_KD
 *      "v" must be an address accessible to the user    SEG_386_UD
 * Up to one click of data can be copied. No alignment restrictions on "uo"
 * apply.
 */
xpcopy(v, uo, n, space)
char	*v;
unsigned uo;
register int n;
{
	register cseg_t *base;
	register	int save, err;
	int work;

	if (n > NBPC)
		return 0;

	work = workAlloc();
	if (space & SEG_VIRT) {
		space &= ~SEG_VIRT;
		base = &sysmem.u.pbase[btocrd(uo)];
		ptable1_v[work] = *base++ | SEG_SRW;
		ptable1_v[work + 1] = *base++ | SEG_SRW;
	} else {
		ptable1_v[work] = (uo&~(NBPC-1)) + SEG_SRW;
		ptable1_v[work + 1] = (uo&~(NBPC-1)) + NBPC + SEG_SRW;
	}
	mmuupd();
	save = setspace(space);

	err = kucopy(v, ctob(work) + (uo&(NBPC-1)), n);
	setspace(save);
	workFree(work);
	return err;
}
