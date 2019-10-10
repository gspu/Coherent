/* (-lgl
 * 	COHERENT Driver Kit Version 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/mmu.h
 *
 * Paging and other mmu support.
 *
 * Revised: Wed Apr  7 15:12:21 1993 CDT
 */
#ifndef	__SYS_MMU_H__
#define	__SYS_MMU_H__

#include <common/_gregset.h>
#include <common/feature.h>

#if	! __KERNEL__
# error	You must be compiling the kernel to use this header
#endif

#if	_I386

#include <common/__paddr.h>
#include <sys/types.h>
#include <sys/uproc.h>

/*
 * NIGEL: for some reason the type "cseg_t" was in <sys/types.h>. It belongs
 * here and in <sys/seg.h> as much as it belongs anywhere.
 */

#ifndef	__CSEG_T
#define	__CSEG_T
typedef	long		cseg_t;
#endif

	/* A click is a 4K byte paragraph.
	 * a segment is a 4 megabyte paragraph (level 1 page table entry)
	 */

#define UII_BASE	0x00400000L	/* base for sep I/D l.out text */

#define	SEG_ILL		0x00	/* The empty page table entry.	*/
#define SEG_PRE		0x01	/* Present bit.			*/
#define	SEG_SRO		0x01	/* Read by system only.		*/
#define	SEG_SRW		0x03	/* Read/Write by system only.	*/
#define	SEG_RO		0x05	/* Read only by anybody.	*/
#define	SEG_BITS	0x07	/* Permissions bits for a pte.	*/
#define	SEG_RW		0x07	/* Read/Write by anybody.	*/
#define	SEG_ACD		0x20	/* Page has been accessed.	*/
#define	SEG_UPD		0x40	/* Page has been updated.	*/
/*
 * SEG_NPL is for pages which are not from the sysmem pool.
 * This includes pages representing video memory attached to
 * user data.
 * The SEG_NPL bit is not a real page table entry flag, and so is
 * masked out when CPU page tables are loaded from process data.
 */
#define	SEG_NPL		0x80	/* Page is not in sysmem pool.	*/

#define	DIR_RW		0x07 /* us=us0|us1; rw=rw0&rw1; Intel's sOOO logical*/

#define	SEG_386_UI	0x08	/* [ 0000 0000 .. FFFF FFFF ]		*/
#define	SEG_386_UD	0x10
#define	SEG_386_KI	0x18
#define	SEG_386_KD	0x21	/* kernel data in ring 1		*/
#define	SEG_286_UI	0x28
#define	SEG_286_UD	0x30
#define	SEG_TSS		0x38
#define	SEG_ROM		0x40
#define	SEG_VIDEOa	0x48
#define	SEG_VIDEOb	0x50
#define	SEG_386_II	0x58
#define	SEG_386_ID	0x60
#define	SEG_286_UII	0x68		/* UI -i */
#define	SEG_LDT		0x70
#define SEG_RNG0_STK	0x78	/* lower limit of 0xFFFFF000		*/
#define SEG_RNG0_TXT	0x80
#define SEG_RNG1_STK	0x88

#define	SEG_VIRT	0x100		/* pseudo bit for kxcopy */

#define	R_USR		0x03		/* user privilege level		*/
#define	SEG_PL		0x03		/* privilege level mask		*/

#define	DPL_0		0x00		/* privilege level 0		*/
#define	DPL_1		0x01		/* privilege level 1		*/
#define	DPL_2		0x02		/* privilege level 2		*/
#define	DPL_3		0x03		/* privilege level 3		*/

/*
 * These addresses are all in clicks.
 */
#define	ROM		0xFFFC0	/* BIOS virtual address.		*/
#define	VIDEOa		0xFFFB0	/* CGA video virtual address.		*/
#define	VIDEOb		0xFFFA0	/* Mono video virtual address.		*/

#define	PTABLE0_P	0x00001	/* Page directory physical address.	*/
#define	PBASE		0x00002	/* Start of kernel, physical address.	*/

#define	PTABLE0_V	0xFFFFE	/* Page directory virtual address.	*/
#define	PPTABLE1_V	0xFFFFC	/* Virtual address of the page table
				 * for the virtual page table.
				 */

#define MAX_VADDR	ctob(VIDEOb)	/* Highest allocatable virtual address.  */
/*
 * Temporary virtual clicks WORK0 and WORK1 are no longer used.
 * Instead there is a range of click pairs starting at START_WORK
 * (which is currently 0xFFFFA000) and working down, managed in work.c.
 *
 * Addresses in kernel data for the RAM disk are now in rm.c.
 * As of 92/12/23, they are
 *	RAM0	0x88000		Ram disk 0 virtual click address.
 *	RAM1	0x88800		Ram disk 1 virtual click address.
 *	RAMSIZE	0x00800		Number of clicks in each ram disk.
 */

#define	SBASE		0xFFC00	/* Start of kernel, virtual address.	*/
#define	PTABLE1_V	0xFF800	/* Start of virtual page table.		*/

/*
 * ptable0_v[] is the page directory (master page table).
 * ptable1_v[] is the virtual page table.
 */
#define	ptable0_v	((long *)ctob(PTABLE0_V))
#define	ptable1_v	((long *)ctob(PTABLE1_V))


#define	SZDT		8		/* size of a segment descriptor */

#define	clickseg(n)	(((long)n) << BPCSHIFT)
#define	segclick(n)	((long)(n) >> BPCSHIFT)

#define	regread(n)	ptable0_v[(n)>>BPC1SHIFT]
#define	regload(n, v)	{ ptable0_v[(n)>>BPC1SHIFT] = v; mmuupd(); }

#define	__xmode_286(regp)	((regp)->_i386._ds == (SEG_286_UD | R_USR))


/*
 * These macros assume segment size <= 4 megabytes.
 *
 * MAPIO:absolute page table address, offset ->
 *       relative page table click# (20 bits) ... offset (12 bits)
 * MAPIO converts (SEG.s_vmem, byte offset) to system global addr.
 */
#define	MAPIO(seg, off)	(((seg)+((int)(off)>>BPCSHIFT) - sysmem.u.pbase) << \
		BPCSHIFT | ((off) & (NBPC-1)))
#define	P2P(addr) ((sysmem.u.pbase[btocrd(addr)]&~(NBPC-1)) |(addr&(NBPC-1)))

#define BUDDY(addr,size)	((addr) ^ (1 << (size)))
#define	NBUDDY	12	/* segments of 2^NBUDDY 4 click chunks (16 megabytes) */
#define	SPLASH	3
#define	NDATA	4	/* process data segments			*/
#define	BLKSZ	2	/* log2 sizeof(BLOCKLIST)/sizeof(cseg_t)	*/

SR	*loaded();
cseg_t	*c_begin();

#define	INSERT2(t, p, pp) { \
	(p)->forw = (pp); \
	(p)->back = (pp)->back; \
	(pp)->back->forw = (p); \
	(pp)->back= (p); \
	}

#define	DELETE2(p) ((p)->forw->back = (p)->back, (p)->back->forw = \
	(p)->forw, (p)->forw = (p)->back = (p))

#define	INIT2(lp)	((lp)->forw = (lp)->back = (lp))

typedef struct blocklist
{
	struct	blocklist	*back;
	struct	blocklist	*forw;
	int	kval;
	int	fill;			/* sizeof(BLOCKLIST) :: power of 2 */
} BLOCKLIST;

#define	WCOUNT	32			/* number of bits in an int */
#define	WSHIFT	5

typedef struct {
	union {
		BLOCKLIST *budtab;
		cseg_t	*pbase;
	} u;				/* beginning of pointer area */
	int	budfree[1 << (NBUDDY-WSHIFT)];	
	BLOCKLIST bfree[NBUDDY];
	unsigned short	*tfree, *efree, *pfree;
		/* vector of page descriptors (base, end, current pointer) */
	unsigned short lo, hi;	/* valid physical memory (min,max) */
	caddr_t vaddre;		/* end of system */
} SYSMEM;

extern SYSMEM	sysmem;
cseg_t	*c_alloc();
cseg_t	*c_extend();
BLOCKLIST	*arealloc();

/*
 * Declare and initialize an in-memory segment structure.
 */
#define	MAKESR(sr, seg) SEG seg; SR sr = { 0, 0, 0, &seg }

/*
 * Is 'p' a valid physical click address?
 */
#define	pvalid(p)	((p) >= sysmem.lo && (p) < sysmem.hi)

/*
 * How many clicks are free for allocation?
 */
#define allocno()	(sysmem.pfree - sysmem.tfree)

/*
 * IS_POW2() works for negative n only if the CPU uses 2's complement.
 */
#define IS_POW2(n)	(!((n) & ((n) - 1)))	/* Is n a power of 2?  */

typedef struct {
	int	pid;
	int	r[SS+1];
	int	(*func)();
	int	a[5];
	int	res;
	int	err;
} EVENT;

#define	NEV	32
extern	EVENT	evtab[NEV];
EVENT	*evtrap();

#else	 /* if ! _I386 */

/*
 * The following macros facilitate independent access
 * to the selector and offset of a faddr_t (far *) pointer.
 */
#define	FP_OFF(f)	( ((unsigned short *) &(f))[0] )
#define	FP_SEL(f)	( ((unsigned short *) &(f))[1] )

#if	__KERNEL__

/*
 * The following selector accesses the global descriptor table.
 */
extern saddr_t	gdtsel;

/*
 * The following functions manipulate virtual address translation tables.
 */
extern	faddr_t	ptov();		/* faddr_t ptov( paddr_t, fsize_t );	*/
extern	faddr_t	ptovx();	/* faddr_t ptovx( paddr_t );		*/
extern	__paddr_t vtop();	/* __paddr_t vtop( faddr_t );		*/
extern	void	vrelse();	/* void    vrelse( faddr_t );		*/
extern	void	vremap();	/* void    vremap( SEG * );		*/

#endif	/* __KERNEL__ */

#endif	/* ! _I386 */

#endif	/* ! defined (__SYS_MMU_H__) */
