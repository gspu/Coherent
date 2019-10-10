/*
 * these routines are written in C until the 386 compiler/assembler
 * are available
 *
 * Copyright (c) Ciaran O'Donnell, Bievres (FRANCE), 1991
 */

#include <sys/debug.h>
#include <kernel/reg.h>

#include <sys/coherent.h>
#include <sys/clist.h>
#include <sys/errno.h>
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
	int work = workAlloc ();	/* Get a virtual click pair. */

	ASSERT (npage > 0);

	for (;;) {
		ptable1_v [work] = * from ++ | SEG_SRW;
		ptable1_v [work + 1] = * to ++ | SEG_SRW;
		memcpy (ctob (work + 1), ctob (work), NBPC);

		if (-- npage == 0)
			break;

		mmuupd ();		/* flush paging TLB */
	}

	workFree(work);
}

/*
 * dmaclear()
 *
 * Given a byte count, and a system global address, zero-fill the memory.
 */

void
dmaclear (nbytes, to)
long	nbytes;
paddr_t	to;
{
	unsigned off;
	int	n;
	cseg_t *base;
	int work = workAlloc ();	/* Get a virtual click pair. */

	off = to & (NBPC - 1);
	base = & sysmem.u.pbase [btocrd (to)];
	n = min (nbytes, NBPC - off);
	ptable1_v [work] = * base ++ | SEG_SRW;

	memset (ctob (work) + off, 0, n);
	nbytes -= n;

	while (nbytes > 0) {
		n = min (nbytes, NBPC);
		ptable1_v [work] = * base ++ | SEG_SRW;
		mmuupd ();

		memset (ctob (work), 0, n);
		nbytes -= n;
	}

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
	int work = workAlloc ();	/* Get a virtual click pair. */

	off = to & (NBPC - 1);
	base = & sysmem.u.pbase [btocrd (to)];

	n = min (nbytes, NBPC - off);
	ptable1_v [work] = * base ++ | SEG_SRW;

	memcpy (vaddr, ctob (work) + off, n);
	vaddr += n;
	nbytes -= n;

	while (nbytes > 0) {
		n = min (nbytes, NBPC);
		ptable1_v [work] = * base ++ | SEG_SRW;
		mmuupd ();

		memcpy (vaddr, ctob (work), n);
		vaddr += n;
		nbytes -= n;
	}

	workFree (work);
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
	int work = workAlloc ();	/* Get a virtual click pair. */

	off = to & (NBPC - 1);
	base = & sysmem.u.pbase [btocrd (to)];

	n = min (nbytes, NBPC - off);
	ptable1_v [work] = * base ++ | SEG_SRW;

	memcpy (ctob (work) + off, vaddr, n);
	vaddr += n;
	nbytes -= n;

	while (nbytes > 0) {
		n = min (nbytes, NBPC);
		ptable1_v [work] = * base ++ | SEG_SRW;
		mmuupd ();

		memcpy (ctob (work), vaddr, n);
		vaddr += n;
		nbytes -= n;
	}

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
	int work = workAlloc ();	/* Get a virtual click pair. */

	off = to & (NBPC - 1);
	base = & sysmem.u.pbase [btocrd (to)];

	n = min (nbytes, NBPC - off);
	ptable1_v [work] = * base ++ | SEG_SRW;
	
	io2seg (n, ctob (work) + off, port);
	nbytes -= n;

	while (nbytes > 0) {
		n = min (nbytes, NBPC);
		ptable1_v [work] = * base ++ | SEG_SRW;
		mmuupd ();
		io2seg(n, ctob (work), port);
		nbytes -= n;
	}

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
	int work = workAlloc ();	/* Get a virtual click pair. */

	off = to & (NBPC - 1);
	base = & sysmem.u.pbase [btocrd (to)];

	n = min (nbytes, NBPC - off);
	ptable1_v [work] = * base ++ | SEG_SRW;
	
	seg2io (n, ctob (work) + off, port);
	nbytes -= n;

	while (nbytes > 0) {
		n = min (nbytes, NBPC);
		ptable1_v [work] = * base ++ | SEG_SRW;
		mmuupd ();
		seg2io (n, ctob (work), port);
		nbytes -= n;
	}

	workFree(work);
}

/*
 * pxcopy()
 *
 * copy "n" bytes of data at kernel address "v" from address "uo" in:
 * 	system global address space		(space & SEG_VIRT)
 * 	physical memory				! (space & SEG_VIRT)
 * Rights are determined by (space & ~ SEG_VIRT):
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
	cseg_t	      *	base;
	int		save;
	int		err;
	int		work;

	if (n > NBPC)
		return 0;

	work = workAlloc ();

	if (space & SEG_VIRT) {
		space &= ~ SEG_VIRT;
		base = & sysmem.u.pbase [btocrd (uo)];
		ptable1_v [work] = * base ++ | SEG_SRW;
		ptable1_v [work + 1] = * base ++ | SEG_SRW;
	} else {
		ptable1_v [work] = (uo & ~ (NBPC - 1)) + SEG_SRW;
		ptable1_v [work + 1] = (uo & ~ (NBPC - 1)) + NBPC + SEG_SRW;
	}

	save = setspace (space);
	err = kucopy (ctob (work) + (uo & (NBPC - 1)), v, n);
	setspace (save);

	workFree (work);
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
	cseg_t	      *	base;
	int		save;
	int		err;
	int		work;

	if (n > NBPC)
		return 0;

	work = workAlloc ();

	if (space & SEG_VIRT) {
		space &= ~ SEG_VIRT;
		base = & sysmem.u.pbase [btocrd (uo)];
		ptable1_v [work] = * base ++ | SEG_SRW;
		ptable1_v [work + 1] = * base ++ | SEG_SRW;
	} else {
		ptable1_v [work] = (uo & ~ (NBPC - 1)) + SEG_SRW;
		ptable1_v [work + 1] = (uo & ~ (NBPC - 1)) + NBPC + SEG_SRW;
	}

	save = setspace (space);
	err = ukcopy (v, ctob (work) + (uo & (NBPC - 1)), n);
	setspace (save);

	workFree (work);
	return err;
}
