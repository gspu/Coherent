/*
 * i8086.h
 */
#define FPTEST 1

#include <sys/reg.h>
#include <ieeefp.h>

#define	TBIT	0x0100

/*
 * Operand or address size constants
 */
#define RM16	16
#define RM32	32

#define OPD16	16
#define OPD32	32

/*
 * Some debugger types
 */
#define DBBYTE	1
#define DBWORD	2
#define DBLONG	4
#define DBPTR32	4
#define DBPTR48	6

/*
 * Define the number of various register types available
 */
#define NSEGREGS	6
#define NCTRLREGS	4
#define NDBGREGS	8
#define NTSTREGS	8

/*
 * Offsets into the users area.
 */
#if 0
#define UREGOFF	(UPASIZE-(sizeof (struct ureg)))
#endif

/*
 * Registers from the user area after a core dump.
 */
struct ureg {
	long	ur_gs;
	long	ur_fs;
	long	ur_es;
	long	ur_ds;
	long	ur_di;
	long	ur_si;
	long	ur_bp;			/* bp */
	long	ur_fill;		/* sp */
	long	ur_bx;			/* bx */
	long	ur_dx;			/* dx */
	long	ur_cx;			/* cx */
	long	ur_ax;			/* ax */
	long	ur_trapno;
	long	ur_id;			/* id */
	long	ur_ip;			/* ip */
	long	ur_cs;			/* cs */
	long	ur_fw;			/* fw */
	long	ur_sp;			/* sp */
	long	ur_ss;			/* ss */
#if FPTEST
	long	ur_filler;
	struct _fpstate ur_fpstate;
#endif
};

/*
 * Registers.
 */
typedef	struct reg {
	unsigned r_ax;			/* Accumulator */
	unsigned r_bx;			/* Base */
	unsigned r_cx;			/* Count */
	unsigned r_dx;			/* Data */
	unsigned r_sp;			/* Stack pointer */
	unsigned r_bp;			/* Base pointer */
	unsigned r_si;			/* Source index */
	unsigned r_di;			/* Destination index */
	unsigned r_cs;			/* Code segment */
	unsigned r_ds;			/* Data segment */
	unsigned r_ss;			/* Stack segment */
	unsigned r_es;			/* Extra segment */
	unsigned r_fs;
	unsigned r_gs;
	unsigned r_ip;			/* Instruction pointer */
	unsigned r_fw;			/* Status flags */
	unsigned valid;			/* 0 = uninitialized */
} REG;

/*
 * Instruction table.
 */
typedef struct {
	unsigned i_code;		/* Code */
	unsigned i_mask;		/* Mask */
	char	 *i_name;		/* Name */
} INS;

/*
 * Functions.
 */
char	*putifmt();
char	*fmatPercent();

/*
 * Global symbols.
 */
extern	int	cacdata;		/* Current word in cache */
extern	int	cacaddr;		/* Address of word in cache */
extern	int	cacsegn;		/* Segment number of word in cache */
extern	int	sysflag;		/* Executing a system call */
extern	BIN	sin;			/* Instruction after sys call */
extern	REG	reg;			/* General registers */

/*
 * Tables.
 */
extern	char	*formtab[4][4];		/* Format table */
extern	char	regitab[];		/* Register name table */
extern	char	*regbtab[];		/* Byte register table */
extern	char	*regwtab[];		/* Word register table */
extern	char	*indmtab[];		/* Indirect mode table */
extern	char	*jumptab[];		/* Jump table */
extern	char	*sysitab[];		/* System call table */
extern	INS	insrtab[];		/* Instruction table */
extern	INS	fpinstab[];		/* 8087 Instruction table */
extern	INS	vminstab[];		/* Virtual Mode Instruction table */

extern	char	*opStrMap1[];		/* Map 1 of the instruction strings */
extern	char	*opStrMap2[];		/* Map 2 of the instruction strings */
extern	char	*grpStrMap0[];		/* Map "grp0" of the instruction strings */
extern	char	*grpStrMap1[];		/* Map "grp1" of the instruction strings */
extern	char	*grpStrMap2[];		/* Map "grp2" of the instruction strings */
extern	char	*grpStrMap3[];		/* Map "grp3" of the instruction strings */
extern	char	*grpStrMap4[];		/* Map "grp4" of the instruction strings */
extern	char	*grpStrMap5[];		/* Map "grp5" of the instruction strings */
extern	char	*grpStrMap6[];		/* Map "grp6" of the instruction strings */
extern	char	*grpStrMap7[];		/* Map "grp7" of the instruction strings */
extern	char	*grpStrMap8[];		/* Map "grp8" of the instruction strings */
extern	char	**grpStrMap[];		/* Map "grp" of the instruction strings */
extern	char	*segReg[];		/* Strings for the segment regs */
extern	char	*genReg8[];		/* Strings for 8 bit general regs */
extern	char	*genReg16[];		/* Strings for 16 bit general regs */
extern	char	*genReg32[];		/* Strings for 32 bit general regs */
extern	char	**genReg[];		/* Strings for 32 bit general regs */
extern	char	*ctrlReg[];		/* Strings for the control regs */
extern	char	*dbgReg[];		/* Strings for the debug regs */
extern	char	*tstReg[];		/* Strings for the test regs */
extern	char	*modRMtab16[];		/* Strings for 16b indirect addressing */
extern	char	*modRMtab32[];		/* Strings for 16b indirect addressing */
extern	char	**modRMtab[];		/* Strings for indirect addressing */
extern	char	*sibtab0[];		/* Strings for indexed addressing * 1 */
extern	char	*sibtab1[];		/* Strings for indexed addressing * 2 */
extern	char	*sibtab2[];		/* Strings for indexed addressing * 4 */
extern	char	*sibtab3[];		/* Strings for indexed addressing * 8 */
extern	char	**sibtab[];		/* Strings for indexed addressing * 8 */
