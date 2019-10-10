/*
 * /usr/include/sys/reg.h
 *
 * Machine dependent definitions, 80386 Coherent, IBM PC.
 *
 * Revised: Mon Jul 19 12:25:37 1993 CDT
 */
#ifndef	__KERNEL_REG_H__
#define	__KERNEL_REG_H__

#include <common/__caddr.h>
#include <kernel/param.h>

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

#define	blockn(n)		((n) >> 9)
#define	blocko(n)		((n) & (512 - 1))
#define nbnrem(b)		((int) (b) & (128 - 1))
#define nbndiv(b)		((b) >> 7)


#define	btos(n)		((((unsigned long)(n))+(1<<BPSSHIFT)-1) >> BPSSHIFT)
#define	btosrd(n)	(((unsigned long)(n)) >> BPSSHIFT)
#define	stob(n)		(((long)(n)) << BPSSHIFT)

#define	btoc(n)		((((unsigned long)(n))+NBPC-1) >> BPCSHIFT)
#define	btocrd(n)	(((unsigned long)(n)) >> BPCSHIFT)
#define	ctob(n)		(((long)(n)) << BPCSHIFT)

#define ctos(x)		(((x) + (1<<SG1SHIFT)-1) >> SG1SHIFT)
#define	ctosrd(x)	((x) >> SG1SHIFT)
#define	stoc(x)		((x) << SG1SHIFT)

/*
 * These are not put on the stack, but they have slots in the table
 * global_reg[].  These numbers are the offsets into that table.
 */
#define RCR0	19
#define RCR1	20
#define RCR2	21
#define RCR3	22

/*
 * How many register slots do we recognise?
 */
#define NUM_REG 23

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
	unsigned	mp_csl;
	unsigned	mp_dsl;
	__caddr_t	mp_svb;
	__caddr_t	mp_svl;
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
	int	me_cs;
	int	me_pc;
	int	me_space;
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
	int	mc_cs;
	int	mc_pc;
	int	mc_space;
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

#define NUM_IRQ_LEVELS		16	/* counting master & slave PIC's */
#define LOWEST_SLAVE_IRQ	8	/* master is 0-7; slave is 8-15 */

#if	_ENABLE_STREAMS

#include <sys/types.h>

/*
 * NIGEL: I have made some small modifications here to allow me to slot in the
 * extensions necessary to support the DDI/DDK rational interrupt architecture.
 *
 * The two macros below are used by setivec () and clrivec () in "i386/md.c" to
 * set the PIC mask values for the base level. Under the rational scheme, this
 * also affects some global data which is used by the DDI/DDK spl... ()
 * functions so they can safely manipulate the PIC mask registers rather than
 * the CPU global mask bit.
 */

__EXTERN_C_BEGIN__

void		DDI_BASE_SLAVE_MASK	__PROTO ((uchar_t _mask));
void		DDI_BASE_MASTER_MASK	__PROTO ((uchar_t _mask));

__EXTERN_C_END__

#else	/* if ! _ENABLE_STREAMS */

#define	DDI_BASE_SLAVE_MASK(m)		outb (SPICM, m)
#define	DDI_BASE_MASTER_MASK(m)		outb (PICM, m)

#endif	/* ! _ENABLE_STREAMS */

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

/*
 * For accessing high and low words of a long.
 */
struct l {
	int	l_lo;
	int	l_hi;
};

#endif
