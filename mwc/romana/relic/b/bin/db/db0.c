/*
 * db/db0.c
 * A debugger.
 * Global variables.
 */

#include "db.h"

/*
 * Global variables.
 */
ADDR_T	add;				/* Address used by getb		*/
char	*addr_fmt = ADDR_FMT;		/* ADDR_T format for printf	*/
int	aop_size;			/* Default address/operand size	*/
BPT	bpt[NBPT];			/* Breakpoint table		*/
#ifndef	NOCANON
int	cantype;			/* Canonization type		*/
#endif
char	*cfn;				/* Core file name		*/
FILE	*cfp;				/* Core file pointer		*/
FILEHDR	coff_hdr;			/* COFF file header		*/
int	cseg;				/* Current segment		*/
ADDR_T	dot;				/* Current address		*/
MAP	*endpure;			/* End of pure area		*/
int	execflag;			/* Program is in execution	*/
int	file_type;			/* LOUT_FILE or COFF_FILE	*/
INP	*inpp;				/* Input pointer		*/
int	intflag;			/* Interrupt count		*/
char	*lasterr;			/* Last error			*/
struct ldheader	ldh;			/* l.out header			*/
char	*lfn;				/* l.out file name		*/
FILE	*lfp;				/* l.out file pointer		*/
char	miscbuf[MISSIZE];		/* Miscellaneous buffer		*/
int	modsize;			/* Size of last display mode	*/
long	nsyms;				/* Number of symbols		*/
ADDR_T	old_add;			/* Previous address		*/
int	pid;				/* Current process id		*/
char	*prompt = PROMPT;		/* Interactive command prompt	*/
int	reg_flag;			/* R_INVALID, R_SOME, R_GEN, R_ALL */
int	rflag;				/* Read only flag		*/
char	seg_format[NSEGS][FORSIZE];	/* Segment formats		*/
MAP	*seg_map[NSEGS];		/* Segment descriptors		*/
char	*seg_name[NSEGS] = { "Data space", "Instruction space", "User area" };
					/* Segment names		*/
int	sflag;				/* Don't read symbol table	*/
FILE	*sfp;				/* Symbol table file pointer	*/
char	*step_cmd;			/* Command for single step	*/
int	step_count;			/* Single step count		*/
int	step_mode;			/* Single step mode		*/
int	step_prev;			/* Last mode run (if single)	*/
SYM	*symhash[NHASH];		/* SYM table hash buckets	*/
int	ungotc;				/* Ungot character		*/

/* end of db/db0.c */
