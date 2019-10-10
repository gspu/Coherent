/*
 * h/var.h
 * Machine-independent variant bits.
 *
 * The function of the compiler may be modified
 * by the high/low setting of variant bits.
 * This header file defines:
 *	the machine independent variant bits,
 *	the base of the machine dependent variant bits,
 *	the maximum number of variant bits,
 *	the typedef of the variant array,
 *	and a macro to access the variant bits.
 * The variants are passed to the compiler phases as a string of hex digits.
 */

#ifndef	YATC
#define YATC	0
#endif

#define VMBASE	34		/* Base of machine dependent variants */
#define VMAXIM	48		/* Least non-inclusive upper bound */
#define VGRANU	8		/* 8 bits per slot */
#define isvariant(x)	((variant[(x)/VGRANU]&(1<<((x)%VGRANU))) != 0)
#define notvariant(x)	((variant[(x)/VGRANU]&(1<<((x)%VGRANU))) == 0)
#define setvariant(x)	(variant[(x)/VGRANU]|=(1<<((x)%VGRANU)))
#define clrvariant(x)	(variant[(x)/VGRANU]&=~(1<<((x)%VGRANU)))
#define chgvariant(x)	(variant[(x)/VGRANU]^=(1<<((x)%VGRANU)))
typedef char VARIANT[VMAXIM/VGRANU];
#if	!YATC
extern VARIANT variant;
#endif

/* Strict checking */
#define	VSUREG	0		/* Unused registers */
#define	VSUVAR	1		/* Unused variables */
#define	VSNREG	2		/* Declared register but auto */
#define	VSRTVC	3		/* Risky things in truth contexts */
#define	VSMEMB	4		/* Strict member checking */
#define	VSBOOK	5		/* Only things in K and R */
#define	VSLCON	6		/* Long constants */
#define VSPVAL	7		/* Pointer value truncated */
#define VSCCON	8		/* Constant conditional */

#define	VSTAT	10		/* Statistics */
#define VWIDEN	11		/* Widened parameter or function value */
#define VPEEP	12		/* Enable peephole code */
#define VCOMM	13		/* Permit .comm data items */
#define VQUIET	14		/* No messages */
#define	VPSTR	15		/* Strings are pure */
#define	VROM	16		/* ROM code */
#define	VASM	17		/* Output assembly code */

/* Debugging variants */
#define	VLINES	18		/* Line # items */
#define VTYPES	19		/* Type items */
#define VDEBUG	20		/* Static and local symbols */
#define VDSYMB	20		/* Preferred synonym for VDEBUG */
#define	VLIB	21		/* Suppress typedefs in debug info */

#define VNOWARN	22		/* Suppress warning messages */
#define	VPROF	23		/* Profile on */
#define VALIEN	24		/* Allow calls to/from alien conventions */
#define VREADONLY	25	/* Recognize READONLY keyword, like const */
#define VSINU	26		/* Implement struct-in-union rules, not UCB 
				 * member resolution rules. */
#define VNOOPT	27		/* Preserve code order in optimization */
#define VCPLUS	28		/* Ignore C++ style online comments */
#define VCPPE	29		/* Run cpp in -E mode */
#define VCPP	30		/* Run cpp */
#define VCPPC	31		/* Run cpp  in -C mode */
#define VTPROF	32		/* Generate code table profiling */
#define V3GRAPH 33		/* Translate tri-graphs */

/* end of h/var.h */
