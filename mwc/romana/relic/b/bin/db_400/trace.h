/*
 * trace.h
 */

#include <sys/types.h>
#include "i8086trace.h"

/*
 * Adjustable constants.
 */
#define NBPT	16			/* Number of breakpoints */
#define	CCSSIZE	8			/* C compiler symbol size */
#define	ARGSIZE	20			/* Max number of args for `:e' */
#define VALSIZE	10			/* Number of expressions allowed */
#define DISSIZE	64			/* Terminal display size */
#define FORSIZE	64			/* Size of format for segments */
#define COMSIZE	128			/* Size of command sequence */
#define MISSIZE	256			/* Size of miscellaneous buffer */

/*
 * Modes for single step.
 */
#define SNULL	0			/* Do nothing */
#define SSTEP	1			/* Single step */
#define SCONT	2			/* Continuous step */
#define SCSET	3			/* Continuous step setup */
#define SWAIT	4			/* Wait for function return */

/*
 * Breakpoint flags.
 */
#define BBPT	1			/* Breakpoint set */
#define BRET	2			/* Breakpoint return set */
#define BSIN	4			/* Breakpoint single continue set */

/*
 * Breakpoint table.
 */
typedef struct b_st {
	int	b_flag;			/* Flag word */
	caddr_t	b_badd;			/* Breakpoint address */
	caddr_t	b_rfpt;			/* Return frame pointer */
	caddr_t	b_sfpt;			/* Single continue frame pointer */
	char	*b_bcom;		/* Breakpoint command */
	char	*b_rcom;		/* Return command */
	BIN	b_bins;			/* Instruction we replaced */
} BPT;

/*
 * Segment definitions.
 */
#define NSEGM	3			/* Number of segments */

#define DSEG	0
#define ISEG	1
#define USEG	2

#define	DSPACE	segmapl[DSEG]		/* Data space */
#define ISPACE	segmapl[ISEG]		/* Instruction space */
#define USPACE	segmapl[USEG]		/* User area */

/*
 * Segmentation map table structure.
 */
typedef struct map {
	struct	map *m_next;		/* Pointer to next */
	off_t	m_base;			/* Segmentation base */
	off_t	m_bend;			/* End of base */
	off_t	m_offt;			/* Offset */
	int	(*m_getf)();		/* Read function */
	int	(*m_putf)();		/* Write function */
	int	m_segi;			/* Index for segment */
} MAP;

/*
 *Default segment attribute definitions.
 */
#define DSA16	16		/* 16 bit default operand and address size */
#define DSA32	32		/* 32 bit default operand and address size */

/*
 * Storing some important file header information in memory.
 */
typedef	struct	fileheaderInfo {
	unsigned short magic;
	unsigned short defsegatt; /*Default segment attribute definitions */
} HDRINFO;

/*
 * Extension of symbols.
 */
#define	L_REG	11			/* Register symbol type */

/*
 * Symbol header structure in memory.
 */
struct	LDSYM {
	char		ls_id[NCPLN];	/* Symbol name		*/
	short		ls_type;	/* Global + Seg.	*/
	off_t		ls_addr;	/* Value of symbol	*/
};

/*
 * Symbol in memory.
 */
typedef struct sym {
	unsigned char s_hash;		/* Hash value */
	unsigned char s_type;		/* Type */
	unsigned s_sval;		/* Value of symbol */
} SYM;

/*
 * Default file names.
 */
#define DEFLT_OBJ	"l.out"		/* object file */
#define DEFLT_AUX	"core"		/* auxiliary file */

/*
 * Flags in value.
 */
#define VNULL	1			/* Value is null */
#define VLVAL	2			/* Value is a left value */

/*
 * Value descriptor.
 */
typedef struct val {
	unsigned char v_flag;		/* Flag word */
	unsigned char v_segn;		/* Segment number */
	long	 v_nval;		/* Value */
} VAL;

/*
 * Types of input.
 */
#define IFILE	1			/* File I/O */
#define ICORE	2			/* Core I/O */

/*
 * Next input stack.
 */
typedef union inp {
	struct {
		union	inp *i_next;	/* Pointer to next */
		int	i_type;		/* Type of input */
		FILE	*i_filp;	/* File pointer */
	} i_st1;

	struct {
		union	inp *i_next;	/* Pointer to next */
		int	i_type;		/* Type of input */
		char	*i_strp;	/* String pointer */
	} i_st2;
} INP;

/*
 * To find a structure offset.
 */
#define offset(s, m)	((int) &(((struct s *) 0)->m))

/*
 * Functions defined by the machine dependent routines.
 */
caddr_t	getpc();			/* Get programme counter */

/*
 * Function for the C compiler.
 */
int	armsint();			/* trace1.c */
MAP	*setsmap();			/* trace1.c */
MAP	*clrsmap();			/* trace1.c */
FILE	*openfil();			/* trace1.c */
int	getp();				/* trace2.c */
int	putp();				/* trace2.c */
char	*conform();			/* trace3.c */
char	*getform();			/* trace3.c */
char	*gettime();			/* trace3.c */
long	lvalue();			/* trace5.c */
long	rvalue();			/* trace5.c */
int	getb();				/* trace6.c */
int	putb();				/* trace6.c */
int	getf();				/* trace6.c */
int	putf();				/* trace6.c */
MAP	*mapaddr();			/* trace6.c */
char	*conaddr();			/* trace6.c */
char	*nalloc();			/* trace6.c */

/*
 * Variables defined by machine dependent routine.
 */
extern	char	*signame[];		/* Names of signals */

/*
 * Global variables.
 */
extern	BIN	bin;			/* Breakpoint instruction */
extern	BPT	bpt[NBPT];		/* Breakpoint table */
extern	FILE	*cfp;			/* Core file pointer */
extern	FILE	*lfp;			/* l.out file pointer */
extern	FILE	*sfp;			/* Symbol table file pointer */
extern	INP	*inpp;			/* Input pointer */
extern	MAP	*endpure;		/* End of pure area */
extern	MAP	*segmapl[NSEGM];	/* Segment descriptors */
extern	SYM	*ssymp;			/* Pointer to core symbol table */
extern	char	*errrstr;		/* Last error */
extern	char	*lfn;			/* l.out file name */
extern	char	*segname[NSEGM];	/* Names of segments */
extern	char	*sinp;			/* Command for single step */
extern	char	*trapstr;		/* Fault type */
extern	char	miscbuf[MISSIZE];	/* Miscellaneous buffer */
extern	char	segform[NSEGM][FORSIZE];/* Formats for segments */
extern	int	bitflag;		/* Single step next instruction */
extern	int	cantype;		/* Canonization type */
extern	int	cseg;			/* Current segment */
extern	int	excflag;		/* Programme is in execution */
extern	int	intflag;		/* Interrupt count */
extern	int	lastc;			/* Character for ungetn */
extern	int	modsize;		/* Size of last display mode */
extern	int	objflag;		/* Programme can run */
extern	int	pid;			/* Current process id */
extern	int	regflag;		/* Registers exist */
extern	int	rflag;			/* Read only flag */
extern	int	sflag;			/* Don't read symbol table */
extern	int	sincmod;		/* Last mode ran (if single) */
extern	int	sindecr;		/* Single step count */
extern	int	sinmode;		/* Single step mode */
extern	long	add;			/* Address used by getb */
extern	long	dot;			/* Current address */
extern	long	lad;			/* Last address of dot */
extern	off_t	sbase;			/* Base of symbols */
extern	off_t	snsym;			/* Number of symbols */

extern	HDRINFO hdrinfo;		/* important file header info */
extern	off_t	sngblsym;		/* Number of symbols of type global */
extern	off_t	*gblsymMap;		/* Maps global symbols to symbol table 
					   area of the coff header */
