/*
 * Trace bit.
 */
#define	TBIT	0x0100

/*
 * Offsets into the users area.
 */
#define UREGOFF	(02000-(sizeof (struct ureg)))

/*
 * Registers from the user area after a core dump.
 */
struct ureg {
	int	ur_bp;			/* bp */
	int	ur_di;			/* di */
	int	ur_si;			/* si */
	int	ur_ra;			/* ra ? */
	int	ur_es;			/* es */
	int	ur_cx;			/* cx */
	int	ur_dx;			/* dx */
	int	ur_ax;			/* ax */
	int	ur_bx;			/* bx */
	int	ur_ds;			/* ds */
	int	ur_sp;			/* sp */
	int	ur_ss;			/* ss */
	int	ur_id;			/* id */
	int	ur_ip;			/* ip */
	int	ur_cs;			/* cs */
	int	ur_fw;			/* fw */
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
	unsigned r_ip;			/* Instruction pointer */
	unsigned r_fw;			/* Status flags */
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
