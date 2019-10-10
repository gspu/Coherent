/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__KERNEL_REG_H__
#define	__KERNEL_REG_H__

#include <common/__caddr.h>

/*
 * Functions.
 * blockn - block number from byte number
 * blocko - block offset from byte number
 * nbnrem - offset into indirect block from block number
 * nbndiv - residual indirect mapping from block number
 * btocru - byte to page (saddr_t) rounding up
 * btocrd - byte to page rounding down
 * ctob - page to byte
 */

#define	NIVEC	192

#define	BPC1SHIFT	10
#define	BPSSHIFT	(32 - BPC1SHIFT)
#define	BPCSHIFT	(BPSSHIFT - BPC1SHIFT)

#define	NBPC		(1UL << BPCSHIFT)
#define	NBPS		(1UL << BPSSHIFT)

#define	btosru(n)	(((unsigned long) (n) + (1 << BPSSHIFT) - 1) >> BPSSHIFT)
#define	btosrd(n)	((unsigned long) (n) >> BPSSHIFT)
#define	stob(n)		((long) (n) << BPSSHIFT)

#define	btocru(n)	(((unsigned long) (n) + NBPC - 1) >> BPCSHIFT)
#define	btocrd(n)	((unsigned long) (n) >> BPCSHIFT)
#define	ctob(n)		((long) (n) << BPCSHIFT)


#define	UPASIZE		ctob (1)	/* Size in bytes of user area */

#define	ISP_286		0x10000
#define	ISP_386		stob (512)


/*
 * These addresses are all in pages.
 */

#define	ROM		0xFFFC0	/* BIOS virtual address.		*/
#define	VIDEOa		0xFFFB0	/* CGA video virtual address.		*/
#define	VIDEOb		0xFFFA0	/* Mono video virtual address.		*/

#define MAX_VADDR	ctob(VIDEOb)	/* Highest allocatable virtual address.  */

/*
 * Temporary virtual pages WORK0 and WORK1 are no longer used.
 * Instead there is a range of page pairs starting at START_WORK
 * (which is currently 0xFFFFA000) and working down, managed in work.c.
 *
 * Addresses in kernel data for the RAM disk are now in rm.c.
 * As of 92/12/23, they are
 *	RAM0	0x88000		Ram disk 0 virtual page address.
 *	RAM1	0x88800		Ram disk 1 virtual page address.
 *	RAMSIZE	0x00800		Number of pages in each ram disk.
 */

#define	PBASE		0x00002	/* Start of kernel, physical address.	*/
#define	SBASE		0xFFC00	/* Start of kernel, virtual address.	*/
#define	PTABLE0_V	0xFFFFE	/* Page directory virtual address.	*/
#define	PTABLE1_V	0xFF800	/* Start of virtual page table.		*/

/*
 * ptable0_v[] is the page directory (master page table).
 * ptable1_v[] is the virtual page table.
 */

#define	ptable0_v	((long *) ctob (PTABLE0_V))
#define	ptable1_v	((long *) ctob (PTABLE1_V))


/*
 * Useful definitions.
 */

#define	PIC	0x20		/* 8259 command port */
#define	PICM	0x21		/* 8259 mask port */
#define	SPIC	0xA0		/* Slave 8259 command port */
#define	SPICM	0xA1		/* Slave 8259 mask port */
#define	MUERR	0x0002		/* Location of errno (286 user space) */

#define NUM_IRQ_LEVELS		16	/* counting master & slave PIC's */
#define LOWEST_SLAVE_IRQ	8	/* master is 0-7; slave is 8-15 */


/*
 * The following have been modified to permit the extensions needed
 * to support the DDI/DDK interrupt architecture.
 *
 * The two macros below are used by setivec () and clrivec () in "i386/md.c" to
 * set the PIC mask values for the base level.  Under the DDI/DKI scheme, this
 * also affects some global data which is used by the spl... () functions
 * so they can safely manipulate the PIC mask registers rather than
 * the CPU global mask bit.
 */

#include <sys/types.h>

__EXTERN_C_BEGIN__

void		DDI_BASE_SLAVE_MASK	__PROTO ((uchar_t _mask));
void		DDI_BASE_MASTER_MASK	__PROTO ((uchar_t _mask));

__EXTERN_C_END__


/*
 * Set jump and return structure.
 */

struct __menv {
	int	me_di;
	int	me_si;
	int	mc_bx;
	int	me_bp;
	int	me_sp;
	int	me_pc;
	int	me_ipl;
	int	me_space;
};


/*
 * Context structure.
 */

struct __mcon {
	int	mc_di;
	int	mc_si;
	int	mc_bx;
	int	mc_bp;
	int	mc_sp;
	int	mc_pc;
	int	mc_ipl;
	int	mc_space;
};


/*
 * Trap codes.
 * Passed in the upper 8 bits of
 * the "id" passed to "trap".
 */

#define	SIDIV	0			/* Divide overflow */
#define SISST	1			/* Single step */
#define	SINMI	2			/* NMI (parity) */
#define	SIBPT	3			/* Breakpoint */
#define	SIOVF	4			/* Overflow */
#define	SIBND	5			/* Bound */
#define	SIOP	6			/* Invalid opcode */
#define	SIXNP	7			/* Processor extension not available */
#define	SIDBL	8			/* Double exception */
#define	SIXS	9			/* Processor extension segment overrun */
#define	SITS	10			/* Invalid task state segment */
#define	SINP	11			/* Segment not present */
#define	SISS	12			/* Stack segment overrun/not present */
#define	SIGP	13			/* General protection */
#define SIPF	14			/* Page Fault */
#define SIFP	16			/* Floating Point */
#define	SISYS	32			/* System call */
#define	SIRAN	33			/* Random interrupt */
#define	SIOSYS	34			/* System call */
#define	SIDEV	64			/* Device interrupt */

#endif
