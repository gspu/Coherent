/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Machine dependent definitions.
 * 80386 Coherent, IBM PC.
 */
#ifndef	 MACHINE_H
#define	MACHINE_H	MACHINE_H

#include <sys/types.h>
#include <sys/param.h>

/*
 * Alloc definitions.
 */
#define	align(p)	((ALL *) ((int) (p) & ~1))
#define	link(p)		align((p)->a_link)
#define	tstfree(p)	(((int) (p)->a_link&1) == 0)
#define	setfree(p)	((p)->a_link = (int) (p)->a_link & ~1)
#define	setused(p)	((p)->a_link = (int) (p)->a_link | 1)


/*
 * Functions.
 * blockn - block number from byte number
 * blocko - block offset from byte number
 * nbnrem - offset into indirect block from block number
 * nbndiv - residual indirect mapping from block number
 * btocru - byte to click (saddr_t) rounding up
 * btocrd - byte to click rounding down
 * ctob - click to byte
 * stod - saddr_t to daddr_t conversion for swapper.
 */

#define	NIVEC	192

#define	blockn(n)		((n)>>9)
#define	blocko(n)		((n)&(512-1))
#define nbnrem(b)		((int)(b)&(128-1))
#define nbndiv(b)		((b)>>7)


#define	btos(n)		((((unsigned long)(n))+(1<<BPSSHIFT)-1) >> BPSSHIFT)
#define	btosrd(n)	(((unsigned long)(n)) >> BPSSHIFT)
#define	stob(n)		(((long)(n)) << BPSSHIFT)

#define	btoc(n)		((((unsigned long)(n))+(1<<BPCSHIFT)-1) >> BPCSHIFT)
#define	btocrd(n)	(((unsigned long)(n)) >> BPCSHIFT)
#define	ctob(n)		(((long)(n)) << BPCSHIFT)

#define ctos(x)		(((x) + (1<<SG1SHIFT)-1) >> SG1SHIFT)
#define	ctosrd(x)	((x) >> SG1SHIFT)
#define	stoc(x)		((x) << SG1SHIFT)

/*
 * The saved registers are accessed
 * via constant offsets from the top of the
 * user area stack. The symbols defined below,
 * are the offsets, in words, from the initial system
 * stack. The offsets depend on the actual save order
 * defined by "tsave" in the assist.
 */
#define	SS	18
#define	UESP	17
#define	EFL	16
#define	CS	15
#define	EIP	14
#define	ERR	13
#define TRAPNO	12
#define	EAX	11
#define	ECX	10
#define	EDX	9
#define	EBX	8
#define	ESP	7
#define	EBP	6
#define	ESI	5
#define	EDI	4
#define	DS	3
#define	ES	2
#define	FS	1
#define	GS	0

/*
 * Buffers are not mapped.
 */
#define	bsave(o)
#define	brest(o)
#define	bmapv(p)
#define	bconv(p)	(p)

/*
 * Drivers are not mapped.
 */
#define	dsave(o)
#define	drest(o)
#define	dmapv(s)
#define	dvirt()		0
#define dcopy(dst,src)

#define	mfixcon(pp)			/* do nothing */

/*
 * Register structure.
 */
typedef union mreg_u {
	unsigned m_reg[1];
	unsigned m_int;
} MREG;

/*
 * Segmenation prototype.
 */
typedef struct mproto {
	unsigned mp_csl;
	unsigned mp_dsl;
	vaddr_t	mp_svb;
	vaddr_t	mp_svl;
} MPROTO;

/*
 * Set jump and return structure.
 */
typedef	struct menv_s {
	int	me_di;
	int	me_si;
	int	mc_bx;
	int	me_bp;
	int	me_sp;
	int	me_pc;
	int	me_fw;
}	MENV;

/*
 * Context structure.
 */
typedef	struct mcon_s {
	int	mc_di;
	int	mc_si;
	int	mc_bx;
	int	mc_bp;
	int	mc_sp;
	int	mc_pc;
	int	mc_fw;
}	MCON;

/*
 * General register structure.
 */
typedef int MGEN[1];

/*
 * Useful definitions.
 */
#define	PIC	0x20			/* 8259 command port */
#define	PICM	0x21			/* 8259 mask port */
#define	SPIC	0xA0			/* Slave 8259 command port */
#define	SPICM	0xA1			/* Slave 8259 mask port */
#define	MFTTB	0x0100			/* Trace trap bit */
#define	MFINT	0x0200			/* Interupt enable */
#define	MUERR	0x0002			/* Location of errno */
#define	MFCBIT	0x0001			/* Carry bit */

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
#define SIPF	14			/* page fault */
#define	SISYS	32			/* System call */
#define	SIRAN	33			/* Random interrupt */
#define	SIOSYS	34			/* System call */
#define	SIDEV	64			/* Device interrupt */

/*
 * For accessing high and low words of a long.
 */
struct l {
	int	l_lo;
	int	l_hi;
};
#define	msetppc(v)	u.u_regl[EIP] = (int)v

#endif
