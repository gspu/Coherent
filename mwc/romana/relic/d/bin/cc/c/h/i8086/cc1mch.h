/*
 * This header file contains
 * machine specific macros, types and
 * definitions that are only used by the
 * code generator (cc1).
 * SMALL and LARGE model Intel iAPX-86.
 */
typedef	char	COST;		/* Cost of evaluation */
typedef	char	TYPE;		/* Machine type */
typedef	long	FLAG;		/* Flags */
typedef	char	REGNAME;	/* Register name */
typedef	int	TYPESET;	/* Set of TYPE */
typedef	char	PHYSREG;	/* Physical register name */
typedef	int	PREGSET;	/* Physical reg set */
typedef	unsigned long PATFLAG;	/* Pattern flags, at least 16 bits */
typedef	int	KIND;		/* Register kind */
typedef	int	MASK;		/* Field masks */
typedef	char	INDEX;		/* Index type */

#define	DOWN	1		/* Grow stack downwards */
#define	BITS	0		/* The n'th bit code is not needed */
#define LONGREL	1		/* The long relational code is needed */
#define	SWREG	AX		/* Switch register */
#define	FNUSED	(BAX|BBX|BCX|BDX|BFPAC|BDS|BES)	/* Clobbers all */
#define NOFREE	(BSP|BBP|BCS|BSS)	/* Always occupied registers */
#define	MBLARG	MFNARG		/* Block argument context */
#define	MBLREG	ANYR		/* Block argument register */
#define	ICALLS	1		/* Free level on call */
#define	DVALIS	0		/* Index into dval for DVAL sign */
#define	DVALMS	0200		/* Bit to flip for DVAL sign */
#define	NBPCH	8		/* # of bits in char (out.c) */

#define	upper(n) 	((ival_t)((unsigned long)(n) >> 16))
#define	lower(n)	((ival_t)(n))
#define	isblkp(t)	((t)==LPTB || (t)==SPTB)
#if DECVAX
#define poolseg(op)	((op!=DCON) ? SLINK : SDATA)
#else
#define poolseg(op)	((notvariant(VRAM)||(op!=DCON)) ? SLINK : SDATA)
#endif

#if !TINY
/*
 * Debug printout macros.
 * Explained in snap1.c
 */
#define isnap(x)	printf(" %d", (x))
#define lsnap(x)	printf(" %ld", (x))
#define csnap(x)	((x)!=0?printf(" cost=%d", (x)):0)
#define fsnap(x)	((x)!=0?printf(" flag=%lx", (x)):0)
#define mdlsnap(x)	snaptype((x), "Bad leaf")
#define mdosnap(x)	snaptype((x), "Bad op")
#endif

/*
 * Tree flags.
 * The 'FLAG' type must be big enough
 * to hold all of these bits. After the machine
 * specific flags are the machine independent ones,
 * and then a number of handy combinations of the
 * flags, which are used all over.
 */
#define	T_0	0x00000001L	/* 0 */
#define	T_1	0x00000002L	/* 1 */
#define	T_2	0x00000004L	/* 2 */
#define	T_BYTE	0x00000008L	/* [-128 ... 127] */
#define	T_ICN	0x00000010L	/* ICON */
#define	T_LCN	0x00000020L	/* LCON */
#define	T_UHS	0x00000040L	/* LCON, 0xFFFF.... */
#define T_LHS	0x00000080L	/* LCON, 0x....FFFF */
#define	T_UHC	0x00000100L	/* LCON, 0x0000.... */
#define	T_LHC	0x00000200L	/* LCON, 0x....0000 */
#define	T_DCN	0x00000400L	/* DCON */
#define	T_ACS	0x00000800L	/* ADDR, code segment */
#define	T_ADS	0x00001000L	/* ADDR, data segment */
#define	T_RREG	0x00002000L	/* REG, rvalue */
#define	T_LREG	0x00004000L	/* REG, lvalue */
#define	T_SREG	0x00008000L	/* REG, stack  */
#define	T_LEA	0x00010000L	/* Looks like a LEA */
#define	T_LSS	0x00020000L	/* Looks like a LEA, off SS */
#define	T_DIR	0x00040000L	/* Direct */
#define	T_OFS	0x00080000L	/* Offset */

#define	T_TREG	0x80000000L	/* Need a temporary register */
#define	T_LV	0x40000000L	/* Lvalue context */
#define	T_MMX	0x20000000L	/* Must match shape exactly */
#define	T_INDIR	0x10000000L	/* Fake indirect flag */

#define	T_NUM	(T_ICN|T_LCN)
#define	T_CON	(T_NUM|T_ACS|T_ADS)
#define	T_IMM	(T_CON|T_DCN)
#define T_REG	(T_RREG|T_LREG|T_SREG)
#define	T_ADR	(T_DIR|T_RREG|T_LREG|T_SREG)
#define	T_LEAF	(T_ADR|T_IMM|T_OFS|T_LEA|T_LSS)
#define	T_EASY	(T_DIR|T_IMM|T_OFS|T_LREG|T_RREG|T_SREG)
#define T_NLEAF	(T_IMM|T_ADR)

/*
 * These type testing macros use a
 * table that is hidden in file "table1.c". Although
 * you might think they should be in the "PERTYPE"
 * table (and you are right) they are not so that the
 * code that is generated does not have a multiply
 * in it.
 */
#define islong(t)	((tinfo[t]&01)  != 0)
#define isword(t)	((tinfo[t]&02)  != 0)
#define	isworl(t)	((tinfo[t]&03)  != 0)
#define isuns(t)	((tinfo[t]&04)  != 0)
#define	isflt(t)	((tinfo[t]&010) != 0)
#define isint(t)	((tinfo[t]&020) != 0)
#define isbyte(t)	((tinfo[t]&040) != 0)
#define isworb(t)       ((tinfo[t]&042) != 0)
#define	issized(t)	((tinfo[t]&0100)!= 0)
#define	ispoint(t)	((tinfo[t]&0200)!= 0)

/*
 * Machine-dependent pattern flags.
 * These must not overlap with the pattern flags in cc1.h.
 */
#define	P80186		0x10000L	/* 80186 */
#define	P8087		0x20000L	/* 8087 */
#define	P80287		0x40000L	/* 80287 */
#define	MDPFLAGS	0x70000L	/* Machine-dependent pattern flags */

/*
 * Register kinds.
 * Used by the register allocator.
 */
#define	KB	01		/* Byte */
#define	KW	02		/* Word */
#define	KL	04		/* Long */
#define	KD	010		/* Double */
#define	KLP	020		/* Large pointer */
#define	KSP	040		/* Small pointer */

#define	KWB	(KW|KB)		/* Word or byte */

/*
 * Macros for machine dependent stuff.
 * out.c, pool.c.
 */
#define GIDFMT	(A_GID|A_DIR)		/* Afield ival for GID output */
#define LIDFMT	(A_LID|A_DIR)		/* Afield ival for LID output */
#define CONFMT	(A_OFFS|A_IMM)		/* Afield ival for constant output */
#define mapssize(i)	(i)		/* Stack roundup */
					/* Escape to map mch opcodes */
#define mapcode(c, tp)	(c==ZLDES ? mapzldes(tp) : c)
#define gentos(x,y)			/* No top of stack required */
#define genstar(x,y,z,zz)		/* No star address required */
#define getstar(tp,nse,npfx,pfx)	/* No star address required */
#if ONLYSMALL				/* Supply pointer type for IEXPR */
#define iptrtype()	SPTR
#else
#define iptrtype()	(isvariant(VSMALL) ? SPTR : LPTR)
#endif

/*
 * Externals.
 */
#if	!YATC
extern	char	tinfo[];
extern	PREGSET	regbusy;
extern	PREGSET	maxbusy;
extern	PREGSET	curbusy;
extern	PREGSET	curxreg;
extern	int	blkflab;
#endif
