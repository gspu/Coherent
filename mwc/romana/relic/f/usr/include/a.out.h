/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * /usr/include/a.out.h
 *
 * COFF excerpted from Intel 386 Architecture BCS Supplement, draft 3/3/90
 *
 * Revised Mon Mar 22 15:52:26 1993 CST
 */
#ifndef __A_OUT_H__
#define __A_OUT_H__

struct	filehdr {
	unsigned short	f_magic;	/* I386MAGIC */
	unsigned short	f_nscns;		/* unused by system */
	long	f_timdat;			/* unused by system */
	long	f_symptr;			/* unused by system */
	long	f_nsyms;			/* unused by system */
	unsigned short	f_opthdr;	/* sizeof(opthdr) */
	unsigned short	f_flags;		/* unused by system */
};

#define	I286MAGIC	0x152
#define	I386MAGIC	0x14C

#define	F_RELFLG	00001
#define	F_EXEC		00002
#define	F_LNNO		00004
#define	F_LSYMS		00008
#define	F_SWABD		00100
#define	F_AR16WR	00200
#define	F_AR12WR	00400

struct aouthdr {
	short	magic;			/* [ONZ]_MAGIC */
	short	vstamp;				/* unused by system */
	long	tsize;				/* unused by system */
	long	dsize;				/* unused by system */
	long	bsize;				/* unused by system */
	long	entry;			/* address to start execution */
	long	tstart;				/* unused by system */
	long	dstart;				/* unused by system */
};

#define	O_MAGIC		0407
#define	N_MAGIC		0410
#define	I_MAGIC		0411
#define	Z_MAGIC		0413

struct scnhdr {
	char	s_name[8];			/* unused by system */
	long	s_paddr;			/* unused by system */
	long	s_vaddr;		/* offset of section - memory(bytes)*/
	long	s_size;			/* section size (bytes) */
	long	s_scnptr;		/* offset of section - file (bytes) */
					/* if zero -> BSS segment */
	long	s_relptr;			/* unused by system */
	long	s_lnnoptr;			/* unused by system */
	unsigned short s_nreloc;		/* unused by system */
	unsigned short s_lnno;			/* unused by system */
	long	s_flags;		/* see below */
};

#define	STYP_REG	0x000
#define	STYP_DSECT	0x001
#define	STYP_NOLOAD	0x002
#define	STYP_GROUP	0x004
#define	STYP_PAD	0x008
#define	STYP_COPY	0x010
#define	STYP_TEXT	0x020
#define	STYP_DATA	0x040
#define	STYP_BSS	0x080
#define	STYP_INFO	0x200
#define	STYP_OVER	0x400			/* not supported */
#define	STYP_LIB	0x800			/* not supported */

#if	NUSEG
/*
 * It is not at all clear who uses this stuff below, or why the magic user-
 * namespace symbol NUSEG is needed. NUSEG is actually part of the deep
 * kernel internal stuff...
 */
	/*
	 * xechdr corresponds to the least commmon denominator
	 * of the COFF format and the <l.out> format 286 Coherent used
	 */ 

struct	xecseg {
	unsigned	mbase;
	unsigned	size;
	unsigned	fbase;
};

struct	xechdr {
	unsigned	magic;
	unsigned	entry;
	unsigned	initsp;
	struct	xecseg segs[NUSEG+1];		/* text, data, bss, stack */ 
};

struct	xecnode {
	int	segtype;			/* 0..NUSEG */
	struct	xecseg xseg;
	struct	xecnode *xn;
};

#endif	/* NUSEG */

#define	XMAGIC(a, b)	((a<< 16) | b)

#endif
