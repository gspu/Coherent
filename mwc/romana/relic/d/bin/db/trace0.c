/*
 * A debugger.
 * Tables and stuff.
 */
#include <stdio.h>
#include "trace.h"

/*
 * Global variables.
 */
BPT	bpt[NBPT];			/* Breakpoint table */
FILE	*cfp;				/* Core file pointer */
FILE	*lfp;				/* l.out file pointer */
FILE	*sfp;				/* Symbol table file pointer */
INP	*inpp;				/* Input pointer */
MAP	*endpure;			/* End of pure area */
MAP	*segmapl[NSEGM];		/* Segment descriptors */
SYM	*ssymp;				/* Pointer to core symbol table */
char	*errrstr;			/* Last error */
char	*lfn;				/* l.out file name */
char	*sinp;				/* Command for single step */
char	*trapstr;			/* Fault type */
char	miscbuf[MISSIZE];		/* Miscellaneous buffer */
char	segform[NSEGM][FORSIZE];	/* Formats for segments */
int	bitflag;			/* Single step next instruction */
int	cantype;			/* Canonization type */
int	cseg;				/* Current segment */
int	excflag;			/* Programme is in execution */
int	intflag;			/* Interrupt count */
int	lastc;				/* Character for ungetn */
int	modsize;			/* Size of last display mode */
int	objflag;			/* Programme can run */
int	pid;				/* Current process id */
int	regflag;			/* Registers exist */
int	hflag;				/* debugging coherent */
int	rflag;				/* Read only flag */
int	sflag;				/* Don't read symbol table */
int	sincmod;			/* Last mode ran (if single) */
int	sindecr;			/* Single step count */
int	sinmode;			/* Single step mode */
long	add;				/* Address used by getb */
long	dot;				/* Current address */
long	lad;				/* Last address of dot */
fsize_t	sbase;				/* Base of symbols */
fsize_t	snsym;				/* Number of symbols */
int	dbase = INPBASE;		/* input/ref addr radix */
int	fflag;				/* map as file */

/*
 * Segment names.
 */
char *segname[] ={
	"Data space",
	"Instruction space",
	"User area"
};
