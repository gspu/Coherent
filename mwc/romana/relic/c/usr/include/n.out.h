/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * This is the format of the 'l.out' file with 32-bit addressing
 * (for assembler, linkage editor, and compiler).
 * It also has the namelist structure for the nlist routine.
 */
#ifndef	 L_OUT_H
#define	 L_OUT_H
#include <sys/types.h>

#define	NCPLN	16		/* Chars in loader name */
#define NLSEG	9		/* No. of segments */
#define	NXSEG	6		/* No. of segments used by exec */
#define	L_MAGIC	0407		/* Magic number */

/*
 * This is the header.
 * It sits at the front of the file,
 * holding control information and
 * the sizes of the other segments
 * of the file.
 * The l_u union can be used for different things, but
 * keep the size of members to longs.  
 */

struct	ldheader {
	short	l_magic;	/* Magic number */
	short	l_flag;		/* Flags */
	short	l_machine;	/* Type of target machine */
	short	l_tbase;	/* Text starts here */
	long	l_ssize[NLSEG];	/* Segment sizes */
	union {
		long lu_entry;	/* Entry point */
		long lu_nhwrel;	/* Num HIWORD/LONG rels, for M_LRG8086 */
	} l_u;
};

#define	l_entry		l_u.lu_entry
#define	l_nhwrel	l_u.lu_nhwrel

/* Flags */
#define LF_SHR	01		/* Bound shared */
#define LF_SEP	02		/* Bound separated */
#define LF_NRB	04		/* No reloc. bits */
#define	LF_KER	010		/* Loadable driver */
#define	LF_32	020		/* 32-bit format l.out */
#define	LF_SLREF 040		/* References shared library */
#define	LF_SLIB	0100		/* Is the shared library */
#define LF_DEBUG 0200		/* Include debug segment flag to linker */
#define LF_FAKE	0400		/* header is displaced to end of exe or prg */

/* Formats */
#define	AFMT	"%08lx"		/* Address */

/* Machines */
#ifndef	MTYPE_H
#include <mtype.h>
#endif

/* Segments */
#define	L_SHRI	0		/* Shared Instruction space */
#define	L_PRVI	1		/* Private Instruction space */
#define	L_BSSI	2		/* Uninitialised Instruction */
#define	L_SHRD	3		/* Shared Data space */
#define	L_PRVD	4		/* Private Data space */
#define	L_BSSD	5		/* Uninitalised Data */
#define L_DEBUG 6		/* Debug tables */
#define L_SYM	7		/* Symbols */
#define L_REL	8		/* Relocation */

#define L_ABS	9		/* Absolute (symbol table) */
#define L_REF	10		/* Reference (symbol table) */

/*
 * Symbol.
 * These live in the 'L_SYM' section
 * of the file; the size of this section
 * determines the number of symbols.
 */
struct	ldsym {
	char	ls_id[NCPLN];	/* Symbol name */
	short	ls_type;	/* Global + Seg. */
	long	ls_addr;	/* Value of symbol */
};

/*
 * The nlist structure for the nlist routine.
 */
struct nlist	{
	char	n_name[NCPLN];	/* Symbol name */
	short	n_type;		/* Type flag */
	long	n_value;	/* Value */
};

#define	L_GLOBAL	020

/*
 * Relocation.
 * Relocation is a byte stream.
 * The first is an opcode.
 * The next 4 bytes are the address
 * of relocation.
 * If L_SYM-based relocation, the next two
 * bytes are symbol number.
 */
#define LR_SEG	017		/* Seg., L_SYM => Sym. based */
#define LR_PCR	020		/* PC Rel. flag */

#define LR_OP		0340		/* Opcode */
#define LR_BYTE		(0<<5)		/* Rel. a byte */
#define LR_WORD		(1<<5)		/* Rel. a word */
#define	LR_LONG		(2<<5)		/* Rel. a long */
#define LR_HIWORD	(3<<5)		/* Rel. via hiword of long */
#endif
