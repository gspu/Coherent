/*
 *	trace0.c
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 *
 *	Tables and stuff.
 */
#include <stdio.h>
#include <l.out.h>
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
char	segform[NSEGM][FORSIZE];/* Formats for segments */
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
int	rflag;				/* Read only flag */
int	sflag;				/* Don't read symbol table */
int	sincmod;			/* Last mode ran (if single) */
int	sindecr;			/* Single step count */
int	sinmode;			/* Single step mode */
long	add;				/* Address used by getb */
long	dot;				/* Current address */
long	lad;				/* Last address of dot */
off_t	sbase;				/* Base of symbols */
off_t	snsym;				/* Number of symbols */

HDRINFO hdrinfo;			/* File header info */
off_t	sngblsym;			/* Number of coff.h global symbols */
off_t	*gblsymMap;			/* Maps global symbols to symbol table 
					   area of the coff header */

/*
 * Segment names.
 */
char *segname[] = {
	"Data space",
	"Instruction space",
	"User area"
};
