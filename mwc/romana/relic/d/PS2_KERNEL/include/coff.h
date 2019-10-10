/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * coff.h
 * Common Object File Format (COFF) header for COHERENT.
 */

#ifndef COFF_H	/* Rest of file.  */
#define COFF_H

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
#define F_MINMAL	0x0010			/* product of strip	*/
#define	F_AR32WR	0x0100			/* i80x86 byte order	*/
#define F_KER		0x0800			/* Loadable driver	*/

/* Optional header. */
typedef struct aouthdr	{
	short		magic;			/* AOUT_MAGIC		*/
	short		vstamp;			/* version stamp	*/
	long		tsize;			/* .text size in bytes	*/
	long		dsize;			/* .data size in bytes	*/
	long		bsize;			/* .bss  size in bytes	*/
	long		entry;			/* entry point		*/
	long		text_start;		/* base of .text data	*/
	long		data_start;		/* base of .data data	*/
} AOUTHDR;

/* Magic numbers for aouthdr. Coherent only supports Z_MAGIC */
#define O_MAGIC	0x107	/* Text and data are contiguous text not shared */
#define N_MAGIC 0x108	/* .data starts at seg after .text .text is protected */
#define I_MAGIC 0x109	/* sep id */
#define Z_MAGIC 0x10B	/* Text and data aligned within a.out for direct page */

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
#define _TV	".tv"
#define _INIT	".init"
#define _FINI	".fini"

/* Flags for s_flags field. */
#define	STYP_TEXT	0x20L			/* Code segment		*/
#define	STYP_DATA	0x40L			/* Data segment		*/
#define	STYP_BSS	0x80L			/* BSS segment		*/

/* Relocation items. */
typedef	struct	reloc	{
	long		r_vaddr;		/* Address (where)	*/
	long		r_symndx;		/* Symbol index (what)	*/
	unsigned short	r_type;			/* Type (how)		*/
#pragma align 2
}	RELOC;
#pragma align

#define RELSZ	10				/* sizeof(RELOC)	*/

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
#define R_NONREL	0x00			/* fake record for ld -r */

/* Line number. */
typedef struct lineno {
	union	{
		long	l_symndx;		/* sym table ix of fun name */
		long	l_paddr;		/* physical addr	*/
	} l_addr;
	unsigned short l_lnno;			/* line number or 0 for fun */
#pragma align 2
}	LINENO;
#pragma align

#define	LINESZ	6				/* sizeof(LINENO)	*/

/* Symbol table. */
#define	SYMNMLEN	8			/* Symbol name length	*/
#define FILNMLEN	14			/* Characters in a file name */
#define DIMNUM		4			/* dimensions in aux entry */

typedef	struct	syment	{
	union	{
		char	_n_name[SYMNMLEN];	/* Name			*/
		struct	{
			long	_n_zeroes;	/* If name[0-3] zero,	*/
			long	_n_offset;	/* string table offset	*/
		} _n_n;
		char	*_n_nptr[2];
	} _n;
	long		n_value;		/* Value		*/
	short		n_scnum;		/* Section number	*/
	unsigned short	n_type;			/* Type			*/
	char		n_sclass;		/* Storage class	*/
	char		n_numaux;		/* Auxilliary entries	*/
#pragma align 2
}	SYMENT;
#pragma align

#define SYMESZ	18	/* sizeof(SYMENT) */
#define	n_name		_n._n_name
#define	n_zeroes	_n._n_n._n_zeroes
#define	n_offset	_n._n_n._n_offset

/* Special section numbers */
#define N_UNDEF	 0	/* undefined symbol */
#define N_ABS	-1	/* absolute symbol */
#define N_DEBUG	-2	/* special debugging symbol */
#define N_TV	-3	/* symbol needs transfer vector (preload) */
#define P_TV	-4	/* symbol needs transfer vector (postload) */

/* Storage classes. */
#define C_EFCN		-1			/* end of function	*/
#define C_NULL		0			/* none given		*/
#define C_AUTO		1			/* automatic variable	*/
#define	C_EXT		2			/* External		*/
#define	C_STAT		3			/* Static		*/
#define C_REG		4			/* register variable	*/
#define C_EXTDEF	5			/* External definition  */
#define C_LABEL		6			/* label		*/
#define C_ULABEL	7			/* undefined label	*/
#define C_MOS		8			/* member of structure	*/
#define C_ARG		9			/* function argument	*/
#define C_STRTAG	10			/* structure tag	*/
#define	C_MOU		11			/* member of union	*/
#define C_UNTAG		12			/* union tag		*/
#define C_TPDEF		13			/* typedef		*/
#define C_USTATIC	14			/* undefined static	*/
#define C_ENTAG		15			/* enum tag		*/
#define C_MOE		16			/* member of enum	*/
#define C_REGPARM	17			/* register parm	*/
#define C_FIELD		18			/* bit field		*/
#define C_BLOCK		100			/* .bb or .eb		*/
#define	C_FCN		101			/* .bf or .ef		*/
#define C_EOS		102			/* end of structure	*/
#define C_FILE		103			/* file name		*/

/* is x a tag */
#define ISTAG(x)	(C_STRTAG==(x) || C_UNTAG==(x) || C_ENTAG==(x))

/* fundimental type */
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
#define T_MOE		11	/* member of enumeration */
#define T_UCHAR		12
#define T_USHORT	13
#define T_UINT		14
#define T_ULONG		15

/* Derived Types in n_type */
#define DT_NON	0	/* non derived type */
#define DT_PTR	0x10	/* pointer	    */
#define DT_FCN	0x20	/* function	    */
#define DT_ARY	0x30	/* array	    */

/* Type packing constants */
#define N_BTMASK	0x15	/* Mask for basic type	*/
#define N_TMASK 	0x30	/* Derived type mask	*/
#define N_TSHIFT	2	/* Shift to get at more derivations */

/* Type processing macros */
#define BTYPE(x)	((x) & N_BTMASK)		/* Basic type of x */
#define ISPTR(x)	(DT_PTR == ((x) & N_TMASK))	/* Is x a pointer */
#define ISFCN(x)	(DT_FCN == ((x) & N_TMASK))	/* Is x a function */
#define ISARY(x)	(DT_ARY == ((x) & N_TMASK))	/* Is x an array */
#define INCREF(x)	((((x)&~N_BTMASK)<<N_TSHIFT)|DT_PTR|((x)&N_BTMASK))
#define DECREF(x)	((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/* Aux symbols */
typedef union auxent {
	struct {
		long	x_tagndx;	/* struct, union or enum tag index */
		union {
			struct {
				unsigned short	x_lnno;	/* declaration line */
				unsigned short	x_size;	/* size */
			} x_lnsz;
			long	x_fsize;	/* size of function */
		} x_misc;
		union {
			struct {	/* if ISFCN tag or .bb */
				long	x_lnnoptr;	/* ptr to fcn line # */
				long	x_endndx;	/* index past block */
			} x_fcn;
			struct {	/* if ISARY */
				unsigned short x_dimen[DIMNUM];
			} x_ary;
		} x_fcnary;
		unsigned short	x_tvndx;	/* tv index */
	} x_sym;
	struct {
		char x_fname[FILNMLEN];
	} x_file;
	struct {
		long	x_scnlen;		/* section length */
		unsigned short	x_nreloc;	/* reloc entrys */
		unsigned short	x_nlinno;	/* line number entries */
	} x_scn;
#pragma align 2
}	AUXENT;
#pragma align

#define AUXESZ	18	/* sizeof(auxent) */

#endif /* COFF_H */
/* end of coff.h */
