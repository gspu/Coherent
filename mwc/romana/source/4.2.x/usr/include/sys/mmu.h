/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_MMU_H__
#define	__SYS_MMU_H__

/*
 * Paging and other mmu support. Please do not write code which uses any of
 * the contents of this header; most of the definitions in here will change
 * as paging support is added to the COHERENT kernel.  None of the symbol
 * names or definitions will survive to the next major release.
 */

#include <common/_gregset.h>
#include <common/ccompat.h>
#include <common/feature.h>
#include <common/__caddr.h>
#include <common/__paddr.h>
#include <sys/seg.h>

#if	! _KERNEL
# error	You must be compiling the kernel to use this header
#endif

#ifndef	__CSEG_T
#define	__CSEG_T
typedef	long		cseg_t;
#endif

/*
 * The following is for non-demand-paging kernels - will be phased out soon.
 * It is a typedef for system global addresses.
 */
typedef unsigned long	__sg_addr_t;

#define UII_BASE	0x00400000L	/* base for sep I/D l.out text */

#define	SEG_ILL		0x00	/* The empty page table entry.	*/
#define SEG_PRE		0x01	/* Present bit.			*/
#define	SEG_SRO		0x01	/* Read by system only.		*/
#define	SEG_SRW		0x03	/* Read/Write by system only.	*/
#define	SEG_RO		0x05	/* Read only by anybody.	*/
#define	SEG_BITS	0x07	/* Permissions bits for a pte.	*/
#define	SEG_RW		0x07	/* Read/Write by anybody.	*/
#define	SEG_PWT		0x08	/* Page Write Through, 486 only.*/
#define	SEG_PCD		0x10	/* Page Cache Disable, 486 only.*/
#define	SEG_ACD		0x20	/* Page has been accessed.	*/
#define	SEG_UPD		0x40	/* Page has been updated.	*/

/*
 * SEG_NPL is for pages which are not from the sysmem pool.
 * This includes pages representing video memory attached to user data.
 * The SEG_NPL bit is not a real page table entry flag, and so is
 * masked out when CPU page tables are loaded from process data.
 */

#define	SEG_NPL		0x80	/* Page is not in sysmem pool.	*/

#define	SEL_386_UI	__MAKE_SELECTOR_ARITH (1, _GLOBAL_DESCRIPTOR, \
					       _USER_PRIVILEGE)
#define	SEL_386_UD	__MAKE_SELECTOR_ARITH (2, _GLOBAL_DESCRIPTOR, \
					       _USER_PRIVILEGE)
#define	SEL_386_KI	__MAKE_SELECTOR_ARITH (3, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_1)
#define	SEL_386_KD	__MAKE_SELECTOR_ARITH (4, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_1)
#define	SEL_286_UI	__MAKE_SELECTOR_ARITH (5, _GLOBAL_DESCRIPTOR, \
					       _USER_PRIVILEGE)
#define	SEL_286_UD	__MAKE_SELECTOR_ARITH (6, _GLOBAL_DESCRIPTOR, \
					       _USER_PRIVILEGE)
#define	SEL_TSS		__MAKE_SELECTOR_ARITH (7, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_ROM		__MAKE_SELECTOR_ARITH (8, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_VIDEOa	__MAKE_SELECTOR_ARITH (9, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_1)
#define	SEL_VIDEOb	__MAKE_SELECTOR_ARITH (10, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_1)
#define	SEL_386_II	__MAKE_SELECTOR_ARITH (11, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_386_ID	__MAKE_SELECTOR_ARITH (12, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_286_UII	__MAKE_SELECTOR_ARITH (13, _GLOBAL_DESCRIPTOR, \
					       _USER_PRIVILEGE)
#define	SEL_LDT		__MAKE_SELECTOR_ARITH (14, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_RNG0_STK	__MAKE_SELECTOR_ARITH (15, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_RNG0_TXT	__MAKE_SELECTOR_ARITH (16, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_0)
#define	SEL_RNG1_STK	__MAKE_SELECTOR_ARITH (17, _GLOBAL_DESCRIPTOR, \
					       _PRIVILEGE_RING_1)

#define	SEG_VIRT	0x100		/* pseudo bit for kxcopy */


/*
 * This is the base address of an area of kernel virtual space that
 * can be used to directly map 128Mb or thereabouts of physical memory. The
 * actual amount that we map in is controlled by the configuration system,
 * but anything over 128 Mb probably is not correct.
 */

#define	__PHYSICAL_MAP_BASE	0xF0000000UL
#define	__PHYSICAL_MAP_LEN	0x08000000UL

#define __PTOV_DX	1	/* Diagnostic mode. */

#if __PTOV_DX

/* Error reporting is in the function call, in ff.c */
#define	__PTOV(phys)	__ptov((__paddr_t) phys)
#define	P2P(addr)	__sg_to_p((__sg_addr_t)addr)

__caddr_t	__ptov		__PROTO ((__paddr_t phys));

#else

#define	__PTOV(phys)	((__caddr_t) phys < (__caddr_t) __PHYSICAL_MAP_LEN ? \
			 (__caddr_t) (phys) + __PHYSICAL_MAP_BASE : \
			 (__caddr_t) -1)
#define	P2P(addr) ((sysmem.u.pbase[btocrd(addr)]&~(NBPC-1)) |(addr&(NBPC-1)))

#endif


#define	__xmode_286(regsetp) \
		(__SELECTOR_ARITH ((regsetp)->_i386._ds) == SEL_286_UD)

/*
 * These macros assume segment size <= 4 megabytes.
 *
 * MAPIO:absolute page table address, offset ->
 *       relative page table page# (20 bits) ... offset (12 bits)
 * MAPIO converts (SEG.s_vmem, byte offset) to system global addr.
 *
 * P2P converts a system global address to a physical address.
 */
#define	MAPIO(seg, off)	(((seg)+((int)(off)>>BPCSHIFT) - sysmem.u.pbase) << \
		BPCSHIFT | ((off) & (NBPC-1)))

struct __blocklist {
	struct __blocklist * back;
	struct __blocklist * forw;
	int	kval;
	int	fill;			/* sizeof(BLOCKLIST) :: power of 2 */
};

#define	NBUDDY	12	/* segments of 2^NBUDDY 4 page chunks (16 megabytes) */
#define	WCOUNT	32			/* number of bits in an int */
#define	WSHIFT	5

struct __sysmem {
	union {
		struct __blocklist *budtab;
		cseg_t	*pbase;
	} u;				/* beginning of pointer area */
	int	budfree[1 << (NBUDDY - WSHIFT)];	
	struct __blocklist	bfree [NBUDDY];

	unsigned short	*tfree, *efree, *pfree;
		/* vector of page descriptors (base, end, current pointer) */
	unsigned short lo, hi;	/* valid physical memory (min,max) */
	__caddr_t vaddre;		/* end of system */
};

extern struct __sysmem sysmem;


struct sr     *	loaded ();
cseg_t	      *	c_begin ();
cseg_t	*c_alloc();
cseg_t	*c_extend();
struct __blocklist *arealloc();


/*
 * How many pages are free for allocation?
 */
#define allocno()	(sysmem.pfree - sysmem.tfree)

__EXTERN_C_BEGIN__

__paddr_t	__coh_vtop	__PROTO ((__caddr_t vaddr));
void		areainit	__PROTO ((int budArenaBytes));
void		doload		__PROTO ((SR * srp));
unsigned int	read16_cmos	__PROTO ((unsigned int addr));
void		unload		__PROTO ((SR * srp));

/* From k0.s */

void		mmuupdnR0	__PROTO ((void));
void		mmuupd		__PROTO ((void));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_MMU_H__) */
