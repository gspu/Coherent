/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * l.out.h
 * This defines the format of the 'l.out' file
 * (assembler output, linkage editor input and output).
 * It also gives the namelist structure for the nlist routine.
 */

#ifndef	 L_OUT_H
#define	 L_OUT_H	L_OUT_H

#include <sys/types.h>

#define	NCPLN	16		/* Chars in loader name		*/
#define NLSEG	9		/* No. of segments		*/
#define	NXSEG	6		/* No. of segments used by exec	*/
#define	L_MAGIC	0407		/* Magic number			*/

/*
 * Header.
 * This lives at the front of the file and gives control information
 * and the sizes of the other segments of the file.
 */
struct	ldheader {
	short		l_magic;	/* Magic number		*/
	short		l_flag;		/* Flags		*/
	short		l_machine;	/* Type of target machine */
	unsigned short	l_entry;	/* Entrypoint		*/
	fsize_t		l_ssize[NLSEG];	/* Segment sizes	*/
};

/* Flags. */
#define LF_SHR	01		/* Bound shared			*/
#define LF_SEP	02		/* Bound separated		*/
#define LF_NRB	04		/* No reloc. bits		*/
#define	LF_KER	010		/* Loadable driver		*/

#define	LF_286	0		/* dummy mode [386/exec.c]	*/
#define	LF_386	0x80000000	/* dummy mode [386/exec.c]	*/

/* Formats. */
#define	AFMT	"%06o"		/* Address			*/

/* Machines. */
#include <mtype.h>

/* Segments. */
#define	L_SHRI	0		/* Shared Instruction space	*/
#define	L_PRVI	1		/* Private Instruction space	*/
#define	L_BSSI	2		/* Uninitialized Instruction	*/
#define	L_SHRD	3		/* Shared Data space		*/
#define	L_PRVD	4		/* Private Data space		*/
#define	L_BSSD	5		/* Uninitialized Data space	*/
#define L_DEBUG 6		/* Debug tables			*/
#define L_SYM	7		/* Symbols			*/
#define L_REL	8		/* Relocation			*/
#define L_ABS	9		/* Absolute (symbol table)	*/
#define L_REF	10		/* Reference (symbol table)	*/

/*
 * Symbols.
 * These live in the 'L_SYM' section of the file;
 * the size of this section determines the number of symbols.
 */
struct	ldsym {
	char		ls_id[NCPLN];	/* Symbol name		*/
	short		ls_type;	/* Global + Seg.	*/
	unsigned short	ls_addr;	/* Value of symbol	*/
};

/*
 * The nlist structure for the nlist routine.
 */
struct nlist	{
	char		n_name[NCPLN];	/* Symbol name		*/
	short		n_type;		/* Type flag		*/
	unsigned short	n_value;	/* Value		*/
};

#define	L_GLOBAL	020

/*
 * Relocation.
 * Relocation is a byte stream.
 * The first is an opcode.
 * The next two bytes are the address of relocation.
 */

#define LR_SEG	017		/* Seg., L_SYM => Sym. based	*/
#define LR_PCR	020		/* PC Rel. flag			*/
#define LR_OP	0340		/* Opcode			*/

#define LR_BYTE	(0<<5)		/* Rel. a byte			*/
#define LR_WORD	(1<<5)		/* Rel. a word			*/

#endif

/* end of l.out.h */
