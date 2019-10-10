/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __COFF_H__
#define __COFF_H__

#include <common/ccompat.h>

/*
 * Common Object File Format (COFF) header for COHERENT.
 */

/* File header. */
typedef	struct	filehdr	{
	unsigned short	f_magic;		/* Magic number		*/
	unsigned short	f_nscns;		/* Number of sections	*/
	long		f_timdat;		/* Time and date	*/
	long		f_symptr;		/* Seek to symbol table	*/
	long		f_nsyms;		/* Number of symbols	*/
	unsigned short	f_opthdr;		/* Optional header size	*/
	unsigned short	f_flags;		/* Flags		*/
}	FILEHDR;

/* Magic number. */
#define	C_386_MAGIC	0x14C			/* Intel iAPX 80386	*/
#define ISCOFF(x)	((x) == C_386_MAGIC)

/* Flags for f_flags field. */
#define	F_RELFLG	0x0001			/* No relocation info	*/
#define	F_EXEC		0x0002			/* Executable		*/
#define	F_LNNO		0x0004			/* No line numbers	*/
#define	F_LSYMS		0x0008			/* No local symbols	*/
#define F_MINMAL	0x0010			/* Product of strip	*/
#define	F_AR32WR	0x0100			/* i80x86 byte order	*/
#define F_KER		0x0800			/* Loadable driver	*/

/* Optional header. */
typedef struct aouthdr	{
	short		magic;			/* AOUT_MAGIC		*/
	short		vstamp;			/* Version stamp	*/
	long		tsize;			/* .text size in bytes	*/
	long		dsize;			/* .data size in bytes	*/
	long		bsize;			/* .bss  size in bytes	*/
	long		entry;			/* Entry point		*/
	long		text_start;		/* Base of .text data	*/
	long		data_start;		/* Base of .data data	*/
} AOUTHDR;

/* Magic numbers for aouthdr.magic;  COHERENT only supports Z_MAGIC. */
#define O_MAGIC	0x107	/* .text and .data are contiguous, .text not shared */
#define N_MAGIC 0x108	/* .data starts at seg after .text .text is protected */
#define I_MAGIC 0x109	/* sep id */
#define Z_MAGIC 0x10B	/* .text and .data aligned for direct page */

/* Section header. */
typedef	struct	scnhdr	{
	char		s_name[8];		/* Section name		*/
	long		s_paddr;		/* Physical address	*/
	long		s_vaddr;		/* Virtual address	*/
	long		s_size;			/* Size			*/
	long		s_scnptr;		/* Data pointer		*/
	long		s_relptr;		/* Reloc pointer	*/
	long		s_lnnoptr;		/* Line number pointer	*/
	unsigned short	s_nreloc;		/* Reloc entries	*/
	unsigned short	s_nlnno;		/* Line number entries	*/
	long		s_flags;		/* Flags		*/
}	SCNHDR;

/* Names of special sections */
#define _TEXT	".text"
#define _DATA	".data"
#define _BSS	".bss"
#define	_COMMENT ".comment"
#define _TV	".tv"
#define _INIT	".init"
#define _FINI	".fini"
#define _RODATA ".rodata"

/* Flags for s_flags field. */
#define STYP_GROUP	0x004L			/* Grouped section	*/
#define STYP_PAD	0x008L			/* Padding section	*/
#define STYP_COPY	0x010L			/* Copy section		*/
#define	STYP_TEXT	0x020L			/* Code segment		*/
#define	STYP_DATA	0x040L			/* Data segment		*/
#define	STYP_BSS	0x080L			/* BSS segment		*/
#define STYP_INFO	0x200L			/* Comment section	*/
#define STYP_OVER	0x400L			/* Overlay section	*/
#define STYP_LIB	0x800L			/* Shared library	*/

/*
 * Shared Library Section.
 * Followed by implementation dependent data
 * and a path name aligned on a dword boundary.
 */
typedef	struct	shrlib	{
	long		entsz;			/* Entry size in longs	*/
	long		pathndx;		/* Path offset in longs	*/
}	SHRLIB;


/* Relocation items. */
typedef	struct	reloc	{
	long		r_vaddr __ALIGN (2);	/* Address (where)	*/
	long		r_symndx __ALIGN (2);	/* Symbol index (what)	*/
	unsigned short	r_type;			/* Type (how)		*/
#pragma	align 2
}	RELOC;
#pragma	align	/* control structure padding with Coherent 'cc' */

#define RELSZ	(sizeof (RELOC))

/* Relocation types. */
#define	R_DIR8		0x07			/* 8-bit direct		*/
#define	R_DIR16		0x01			/* 16-bit direct	*/
#define	R_DIR32		0x06			/* 32-bit direct	*/
#define R_RELBYTE	0x0f			/* 8 bit direct		*/
#define R_RELWORD	0x10			/* 16 bit direct	*/
#define R_RELLONG	0x11			/* 32 bit direct	*/
#define	R_PCRBYTE	0x12			/* 8-bit PC-relative	*/
#define	R_PCRWORD	0x13			/* 16-bit PC-relative	*/
#define	R_PCRLONG	0x14			/* 32-bit PC-relative	*/
#define R_NONREL	0x00			/* Fake item for ld -r	*/

/* Line number. */
typedef struct lineno {
	union	{
		long	l_symndx;		/* Fn name symbol index	*/
		long	l_paddr;		/* Physical address	*/
	} l_addr __ALIGN (2);
	unsigned short l_lnno;			/* Line num., 0 for fn	*/
#pragma	align 2
}	LINENO;
#pragma	align	/* control structure padding with Coherent 'cc' */

#define	LINESZ	(sizeof (LINENO))

/* Symbol table. */
#define	SYMNMLEN	8			/* Symbol name length	*/
#define FILNMLEN	14			/* Chars in file name	*/
#define DIMNUM		4			/* Dims in aux entry	*/

typedef	struct	syment	{
	union	{
		char	_n_name [SYMNMLEN];	/* Name			*/
		struct	{
			long	_n_zeroes;	/* If name[0-3] zero,	*/
			long	_n_offset;	/* string table offset	*/
		} _n_n;
		char	* _n_nptr [2];
	} _n __ALIGN (2);
	long		n_value __ALIGN (2);	/* Value		*/
	short		n_scnum;		/* Section number	*/
	unsigned short	n_type;			/* Type			*/
	char		n_sclass;		/* Storage class	*/
	char		n_numaux;		/* Auxilliary entries	*/
#pragma	align 2
}	SYMENT;
#pragma	align	/* control structure padding with Coherent 'cc' */

#define SYMESZ	(sizeof (SYMENT))
#define	n_name		_n._n_name
#define	n_zeroes	_n._n_n._n_zeroes
#define	n_offset	_n._n_n._n_offset

/* Special section numbers */
#define N_UNDEF	 0				/* Undefined		*/
#define N_ABS	-1				/* Absolute		*/
#define N_DEBUG	-2				/* Debug		*/
#define N_TV	-3		/* Needs preload transfer vector	*/
#define P_TV	-4		/* Needs postload transfer vector	*/

/* Storage classes. */
#define C_EFCN		-1			/* End of function	*/
#define C_NULL		0			/* None			*/
#define C_AUTO		1			/* Automatic		*/
#define	C_EXT		2			/* External		*/
#define	C_STAT		3			/* Static		*/
#define C_REG		4			/* Register		*/
#define C_EXTDEF	5			/* External definition  */
#define C_LABEL		6			/* Label		*/
#define C_ULABEL	7			/* Undefined label	*/
#define C_MOS		8			/* Member of structure	*/
#define C_ARG		9			/* Function argument	*/
#define C_STRTAG	10			/* Structure tag	*/
#define	C_MOU		11			/* Member of union	*/
#define C_UNTAG		12			/* Union tag		*/
#define C_TPDEF		13			/* Typedef		*/
#define C_USTATIC	14			/* Undefined static	*/
#define C_ENTAG		15			/* Enum tag		*/
#define C_MOE		16			/* Member of enum	*/
#define C_REGPARM	17			/* Register parameter	*/
#define C_FIELD		18			/* Bit field		*/
#define C_BLOCK		100			/* Block (.bb or .eb)	*/
#define	C_FCN		101			/* Function (.bf or .ef) */
#define C_EOS		102			/* End of structure	*/
#define C_FILE		103			/* File name		*/

#define ISTAG(x)	(C_STRTAG==(x) || C_UNTAG==(x) || C_ENTAG==(x))

/* Fundimental types. */
#define T_NULL		0
#define T_ARG		1
#define T_CHAR		2
#define T_SHORT		3
#define T_INT		4
#define T_LONG		5
#define T_FLOAT		6
#define T_DOUBLE	7
#define T_STRUCT	8
#define T_UNION		9
#define T_ENUM		10
#define T_MOE		11			/* Member of enumeration */
#define T_UCHAR		12
#define T_USHORT	13
#define T_UINT		14
#define T_ULONG		15

/* Derived types in n_type. */
#define DT_NON	0				/* Non-derived type	*/
#define DT_PTR	1				/* Pointer		*/
#define DT_FCN	2				/* Function		*/
#define DT_ARY	3				/* Array		*/

/* Type packing constants. */
#define N_BTMASK	0x0F			/* Mask for basic type	*/
#define N_TMASK 	0x30			/* Derived type mask	*/
#define N_TSHIFT	2			/* Shift for more derived bits */
#define	N_BTSHFT	4			/* Shift for derived type */
 
/* Type processing macros. */
#define BTYPE(x)  ((x) & N_BTMASK)		/* Base type		*/
#define ISPTR(x)  ((DT_PTR<<N_BTSHFT)==((x)&N_TMASK))	/* Pointer?	*/
#define ISFCN(x)  ((DT_FCN<<N_BTSHFT)==((x)&N_TMASK))	/* Function?	*/
#define ISARY(x)  ((DT_ARY<<N_BTSHFT)==((x)&N_TMASK))	/* Array?	*/
#define INCREF(x) ((((x)&~N_BTMASK)<<N_TSHIFT)|(DT_PTR<<N_BTSHFT)|((x)&N_BTMASK))
#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/* Symbol aux entries. */
typedef union auxent	{
	struct	{
		long	x_tagndx;	/* struct/union/enum tag index	*/
		union	{
			struct	{
				unsigned short	x_lnno;	/* Decl. line	*/
				unsigned short	x_size;	/* Size		*/
			} x_lnsz;
			long	x_fsize;	/* Function size	*/
		} x_misc;
		union	{
			struct	{		/* Functions		*/
				long	x_lnnoptr;	/* line # ptr	*/
				long	x_endndx;	/* index of .eb	*/
			} x_fcn;
			struct	{		/* Arrays		*/
				unsigned short x_dimen [DIMNUM];/* Dims	*/
			} x_ary;
		} x_fcnary;
		unsigned short	x_tvndx;	/* TV index		*/
#pragma	align 2
	} x_sym;
#pragma	align	/* control structure padding with Coherent 'cc' */

	struct {				/* File names		*/
		char x_fname [FILNMLEN];	/* File name		*/
	} x_file;

	struct	{				/* Sections		*/
		long	x_scnlen;		/* Section length	*/
		unsigned short	x_nreloc;	/* Reloc entrys		*/
		unsigned short	x_nlinno;	/* Line number entries	*/
	} x_scn;

#pragma	align	2
}	AUXENT;
#pragma	align	/* control structure padding with Coherent 'cc' */

#define	ae_tagndx	x_sym.x_tagndx
#define	ae_lnno		x_sym.x_misc.x_lnsz.x_lnno
#define	ae_size		x_sym.x_misc.x_lnsz.x_size
#define	ae_fsize	x_sym.x_misc.x_fsize
#define	ae_lnnoptr	x_sym.x_fcnary.x_fcn.x_lnnoptr
#define	ae_endndx	x_sym.x_fcnary.x_fcn.x_endndx
#define	ae_dimen	x_sym.x_fcnary.x_ary.x_dimen
#define	ae_tvndx	x_sym.x_tvndx
#define	ae_fname	x_file.x_fname
#define	ae_scnlen	x_scn.x_scnlen
#define	ae_nreloc	x_scn.x_nreloc
#define	ae_nlinno	x_scn.x_nlinno
#define AUXESZ	(sizeof (AUXENT))

#endif	/* ! defined (__COFF_H__) */
