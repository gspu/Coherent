/*
 * This file contains a fairly general writer for Intel iAPX-86 OMF.
 * The 'out...' routines write to a temporary file during code assembly.
 * At the end of assembly, 'copycode' uses the 'put...' routines
 * to write the object file.
 * This is necessary: SEGDEF records must be written before
 * LEDATA and FIXUPP records, but each SEGDEF includes the size
 * of the segment in bytes.
 */

/*
 * References:  "8086 Relocatable Object Formats,"
 * Intel 121748-001 (C) Intel 1981.
 * "Type Definition Records for High Level Languages,"
 * Version 2.0, 6/25/84, Intel 114834 (Intel proprietary).
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

/*
 * As of 4/29/85, PSCOPE does not support unnamed blocks correctly.
 * The output writer therefore suppresses unnamed blocks #ifdef NOABLOCKS.
 */
#if	INTEL
#define	NOABLOCKS 1
int	blkcount;
#endif

#define	NTXT	256			/* Text buffer size */
#define	NREL	256			/* Relocation buffer size */

#ifndef __LINE__
#define	__LINE__	0
#endif

/*
 * Scratch file opcodes.
 * Flags are or'ed in to TBYTE and TWORD opcodes only.
 */
#define	TTYPE	07			/* Type */
#define	TPCR	010			/* PC relative flag */
#define	TSYM	020			/* Symbol based flag */

#define	TEND	00			/* End marker */
#define	TENTER	01			/* Enter new area */
#define	TBYTE	02			/* Byte data */
#define	TWORD	03			/* Word data */
#define	TCODE	04			/* Code segment base */
#define	TDATA	05			/* Data segment base */
#define	TBASE	06			/* External segment base */
#define	TDLAB	07			/* Debug label */
#define	TDLOC	010			/* Debug location */

#if	EMUFIXUPS
/*
 * If EMUFIXUPS is defined at compile time, the generated output writer
 * targets 8087 instructions with magic "M:..." fixups.
 * The linker can then create objects which use either 8087 hardware
 * or software floating point emulation; in the latter case, the
 * linker changes the 8087 instructions into traps to the emulator.
 */
#define	T8087	0340			/* 8087 flags */
#define	TWT	0040			/* M:_WT  */
#define	TWST	0100			/* M:_WST */
#define	TWES	0140			/* M:_WES */
#define	TWCS	0200			/* M:_WCS */
#define	TWSS	0240			/* M:_WSS */
#define	TWDS	0300			/* M:_WDS */
#endif

/*
 * OMF types.
 */
#define	BLKDEF	0x7A			/* Block definition */
#define	BLKEND	0x7C			/* Block end */
#define	DEBSYM	0x7E			/* Debug symbol */
#define	THEADR	0x80			/* Translator module header */
#define	COMENT	0x88			/* Comment */
#define	MODEND	0x8A			/* Module end */
#define	EXTDEF	0x8C			/* External definition (reference) */
#define	TYPDEF	0x8E			/* Type definition */
#define	PUBDEF	0x90			/* Public definition (definition) */
#define	LINNUM	0x94			/* Line # */
#define	LNAMES	0x96			/* List of names */
#define	SEGDEF	0x98			/* Segment definition */
#define	GRPDEF	0x9A			/* Group definition */
#define	FIXUPP	0x9C			/* Fixups */
#define	LEDATA	0xA0			/* Enumerated data */
#define	LIDATA	0xA2			/* Iterated data */

/*
 * Some OMF fields and flags.
 */
#define	PARA	(0x03<<5)		/* Paragraph alignment */
#define	BIG	0x02			/* 64k */
#define	CCON	(0x02<<2)		/* C field, concatenate */
#define	CMEM	(0x01<<2)		/* C field, memory */
#define	CSTACK	(0x05<<2)		/* C field, stack */

#define	LOCAT	0x80			/* Locat flag bit */
#define	M	0x40			/* M bit; 0=self rel, 1=seg rel */
#define	LOBYTE	(0x00<<2)		/* Locat */
#define	OFFSET	(0x01<<2)		/* Locat */
#define	BASE	(0x02<<2)		/* Locat */
#define	POINTER	(0x03<<2)		/* Locat */
#define	HIBYTE	(0x04<<2)		/* Locat */

#define	SECWAY	0x04			/* Secondary way */
#define	FD8087	0x36			/* FIXDAT for 8087 fixup */
#define	FSI	(0<<4)			/* Frame segment index */
#define	FGI	(1<<4)			/* Frame group index */
#define	FEI	(2<<4)			/* Frame external index */
#define	FFN	(3<<4)			/* Frame frame number */
#define	FLSEG	(4<<4)			/* Frame of LSEG */
#define	FTARGET	(5<<4)			/* Frame of TARGET object */
#define	FNONE	(6<<4)			/* No frame */
#define	TSI	0			/* Target: segment index */
#define	TGI	1			/* Target: group index */
#define	TEI	2			/* Target: external index */
#define	TFN	3			/* Target: frame */
#define	F	0x80			/* Frame thread used */
#define	T	0x08			/* Target thread used */
#define	FT0	0x00			/* 0 */
#define	FT1	0x10			/* 1 */

/*
 * Type representations for TYPDEF records.
 */
#if	INTEL
/* Intel proprietary. */
#define	OTSFUNC	0x5E			/* SHORT$NOPOP */
#define	OTLFUNC	0x5F			/* LONG$NOPOP */
#define	OTUPACK	0x66			/* Unpacked */
#define	OTLABEL	0x71			/* LABEL */
#define	OTLONG	0x72			/* LONG */
#define	OTSHORT	0x73			/* SHORT */
#define	OTPROC	0x74			/* PROCEDURE */
#define	OTPARA	0x75			/* PARAMETER */
#define	OTARRAY	0x77			/* ARRAY */
#define	OTSTRUC	0x79			/* STRUCTURE */
#define	OTPOINT	0x7A			/* POINTER */
#define	OTSCAL	0x7B			/* SCALAR */
#define	OTUINT	0x7C			/* UNSINT */
#define	OTSINT	0x7D			/* SGNINT */
#define	OTREAL	0x7E			/* REAL */
#define	OTLIST	0x7F			/* LIST */
#endif

#define	OTNIL	0x80			/* NIL */
#define	OTUNUM2	0x81			/* Unsigned numeric (2 byte) leaf */
#define	OTNAME	0x82			/* Name leaf */
#define	OTINDEX	0x83			/* Index leaf */
#define	OTUNUM3	0x84			/* Unsigned numeric (3 byte) leaf */

/*
 * A linked list of TINFO records containing OMF type definitions
 * is generated while reading DLABEL records.
 * The type index corresponds to the position in the TINFO list.
 * The tflag keeps track of whether a forward reference
 * tagname type ever gets resolved.
 * The length must directly precede the info for puttypdef() to work right.
 */
typedef	struct	typeinfo {
	struct	typeinfo *t_tp;		/* link */
	char	t_tflag;		/* tag name flag */
	int	t_length;		/* length */
	char 	t_info[];		/* info */
} TINFO;

/*
 * To generate correct references to tagged types,
 * the tag name is entered first as a type and
 * all references to the type are made by tag name.
 * When types are dumped, a list of these patch
 * records is consulted to substitute the tag definition
 * for the tag name.
 */
typedef struct	typefix {
	struct	typefix *t_tp;		/* link */
	int	t_ttype;		/* dummy definition */
	int	t_type;			/* fixed definition */
} TYFIX;

static	TINFO	*typelistp;		/* Typdef list */
static	TYFIX	*typefixp;		/* Type fix list */
static	TINFO	*gettype();
static	int	checksum;		/* For putrecord and putbyte */
static	char	txt[NTXT];		/* Text buffer */
static	char	rel[NREL];		/* Relocation buffer */
static	ADDRESS	code_size;		/* Size of CODE */
static	ADDRESS	data_size;		/* Size of DATA */
static  ADDRESS	const_size;		/* Size of CONST */
static	ADDRESS	bss_size;		/* Size of BSS */
static	char	*txtp = txt;		/* Text pointer */
static	char	*relp = rel;		/* Relocation pointer */

#if	EMUFIXUPS
static	int	has8087;		/* Has 8087 code flag */
static	SYM	*wsp[6];		/* Links to emulator symbols */
#endif

#if	VAXFMT
/*
 * The Intel cross software packs the OMF
 * data into 512 byte blocks as 510 byte variable length
 * records, with the last record short.
 */
#define	NVDATA	510			/* Blocksize - 2 */
static	char	vaxbuf[NVDATA];		/* Buffer */
static	char	*vaxptr = vaxbuf;	/* Pointer */
#endif

extern	char	*setsymid();
extern	char	*setindex();
extern	char	*setsize();
extern	char	*setnumb();
extern	long	lastsize;

/*
 * These tables are indexed by a compiler segment name (SCODE et al)
 * to get the OMF segment index.
 * These tables understand the way the SSTRN segment moves
 * around when the ROM option is given, and
 * are dependent on the order of the calls to putsegdef() in copycode().
 */
static	char	lnonseg[] = {
	1,				/* SCODE => module_CODE */
	1,				/* SLINK => module_CODE */
	1,				/* SPURE => module_CODE */
	2,				/* SSTRN => module_DATA */
	2,				/* SDATA => module_DATA */
	2				/* SBSS  => module_DATA */
};

static	char	lromseg[] = {
	1,				/* SCODE => module_CODE */
	1,				/* SLINK => module_CODE */
	1,				/* SPURE => module_CODE */
	1,				/* SSTRN => module_CODE */
	2,				/* SDATA => module_DATA */
	2				/* SBSS  => module_DATA */
};

static	char	lramseg[] = {
	1,				/* SCODE => module_CODE */
	1,				/* SLINK => module_CODE */
	2,				/* SPURE => module_DATA */
	2,				/* SSTRN => module_DATA */
	2,				/* SDATA => module_DATA */
	2				/* SBSS  => module_DATA */
};

static	char	snonseg[] = {
	1,				/* SCODE => CODE */
	1,				/* SLINK => CODE */
	2,				/* SPURE => CONST */
	3,				/* SSTRN => DATA */
	3,				/* SDATA => DATA */
	3				/* SBSS  => DATA */
};

static	char	sromseg[] = {
	1,				/* SCODE => CODE */
	1,				/* SLINK => CODE */
	2,				/* SPURE => CONST */
	2,				/* SSTRN => CONST */
	3,				/* SDATA => DATA */
	3				/* SDATA => DATA */
};

static	char	sramseg[] = {
	1,				/* SCODE => CODE */
	1,				/* SLINK => CODE */
	3,				/* SPURE => DATA */
	3,				/* SSTRN => DATA */
	3,				/* SDATA => DATA */
	3				/* SDATA => DATA */
};

/*
 * Same game, but for group index codes.
 * This is used only in SMALL model.
 */
static	char	grpindex[] = {
	1,				/* SCODE => CGROUP */
	1,				/* SLINK => CGROUP */
	2,				/* SPURE => DGROUP */
	2,				/* SSTRN => DGROUP */
	2,				/* SDATA => DGROUP */
	2				/* SBSS  => DGROUP */
};

/*
 * Some prefabricated OMF items.
 * These are just blasted out via a call to putrecord().
 */
static	char	lthred[]	= { 0x40, 0x01,		/* module_CODE */
				    0x41, 0x02 };	/* module_DATA */
static	char	lnames[]	= { 0,
				    4, 'C', 'O', 'D', 'E',
				    4, 'D', 'A', 'T', 'A',
				    5, 'C', 'O', 'N', 'S', 'T',
				    5, 'S', 'T', 'A', 'C', 'K',
				    6, 'M', 'E', 'M', 'O', 'R', 'Y',
				    6, 'C', 'G', 'R', 'O', 'U', 'P',
				    6, 'D', 'G', 'R', 'O', 'U', 'P'
				};
static	char	gdef1[]		= { 7, 0xFF, 1 };
static	char	gdef2[]		= { 8, 0xFF, 2, 0xFF, 3, 0xFF, 4, 0xFF, 5 };
static	char	sthred[]	= { 0x00, 0x01,		/* Code */
				    0x01, 0x02,		/* Const */
				    0x02, 0x03,		/* Data */
				    0x44, 0x01,		/* Frame -> cgroup */
				    0x45, 0x02		/* Frame -> dgroup */
				};
static	char	modend[]	= { 0x00 };
static	int	blkindex;		/* current block index */

/*
** First pass routines.
** Output items to the temporary file.
*/
/*
 * Initialize the code writer.
 */
outinit()
{
	register TINFO *tp;
#if	MONOLITHIC
	register int	i;

	txtp = &txt[0];
	relp = &rel[0];
#if	EMUFIXUPS
	has8087 = 0;
	for (i=0; i<6; ++i)
		wsp[i] = NULL;
#endif
#if	VAXFMT
	vaxptr = &vaxbuf[0];
#endif
#endif
	typelistp = tp = (TINFO *)malloc(sizeof(TINFO)+1);
	tp->t_tp = NULL;
	tp->t_tflag = 0;
	tp->t_length = 1;
	tp->t_info[0] = OTNIL;
	typefixp = NULL;
}

/*
 * Output a segment switch.
 */
outseg(s)
{
	bput(TENTER);
	bput(s);
}

/*
 * Output an absolute byte.
 */
outab(b)
{
	bput(TBYTE);
	bput(b);
	++dot;
}

/*
 * Output an absolute word.
 */
outaw(w)
{
	bput(TWORD);
	iput(w);
	dot += 2;
}

/*
 * Output a full byte.
 */
outxb(sp, b, pcrf)
register SYM *sp;
{
	register opcode;

	opcode = TBYTE;
	if (sp != NULL)
		opcode |= TSYM;
	if (pcrf)
		opcode |= TPCR;
	bput(opcode);
	bput(b);
	if (sp != NULL)
		pput(sp);
	++dot;
}

/*
 * Output a full word.
 */
outxw(sp, w, pcrf)
register SYM *sp;
{
	register opcode;

	opcode = TWORD;
	if (sp != NULL)
		opcode |= TSYM;
	if (pcrf)
		opcode |= TPCR;
	bput(opcode);
	iput(w);
	if (sp != NULL)
		pput(sp);
	dot += 2;
}

#if	EMUFIXUPS
/*
 * Output a one byte opcode for the 8087.
 */
outfb(b)
{
	has8087 = 1;
	bput(TBYTE|TWT);
	bput(b);
	++dot;
}

/*
 * Output a two byte opcode for the 8087.
 */
outfw(w, prefix)
{
	has8087 = 1;
	bput(TWORD|((prefix>>3)+TWST));
	iput(w);
	dot += 2;
}
#endif

#if	0
/*
 * Because the compiler now generates LARGE model code with links,
 * the following routines are currently unused.
 * They are retained for future reference.
 */
/*
 * Output a 1 word object containing
 * the base address of the current code segment.
 */
outcb()
{
	bput(TCODE);
	dot += 2;
}

/*
 * Output a 1 word object containing
 * the base address of the current data segment.
 */
outdb()
{
	bput(TDATA);
	dot += 2;
}
#endif

/*
 * Output a 1 word object containing
 * the base address of the external symbol pointed to by 'sp'.
 */
outsb(sp)
SYM *sp;
{
	bput(TBASE);
	pput(sp);
	dot += 2;
}

/*
 * Read a dlabel record from ifp.
 * If l(evel) == 0, then only the DLABEL and class bytes have been read;
 * For GDEF's and SEX's in SCODE, we save the symbol pointer, accumulate the
 * argument list types, and back patch the type appropriately.
 * For everything else at level zero, we translate the type to Oomf
 * and write a truncated record to the scratch file.
 */
SYM *curfsp;	/* function sp */
int curfna;	/* nargs */
int curfar[NARGS];	/* arg types */

outdlab(l, class)
{
	register int value;
	register int nline;
	register SYM *sp;
	register int type;
	register int seg;
	register int ttype;

	/* Get line number */
	nline = iget();

	if (l != 0)
		dbfatal(__LINE__);
	value = 0;
	seg = dotseg;
	/* Get value */
	if (class < DC_AUTO)
		;
	else if (class < DC_MOS)
		value = iget();
	else
		dbfatal(__LINE__);

	/* Get name */
	sget(id, NCSYMB);

	/* Check for symbol table entry */
	sp = NULL;
	if (class==DC_GDEF || class==DC_SEX || class==DC_GREF) {
		sp = glookup(id, 0);
	} else {
		/* Make all c names the same, hopefully */
		strcat(id, "_");
	}
	if (sp != NULL) {
		seg = sp->s_seg;
		value = sp->s_value;
	}
	/* Check for tag definition */
	if (class==DC_ETAG
	 || class==DC_STAG
	 || class==DC_UTAG)
		ttype = tlookup(id, strlen(id)+1);
	else
		ttype = 0;

	/* Write part of truncated record */
	bput(TDLAB);
	bput(class);
	iput(nline);
	iput(value);
	bput(seg);
	sput(id);

	/* Get type and enter into table */
	/* This will possibly overwrite id[], so it must come after glookup */
	type = dbtmap(ttype);

	/* Write out truncated record. */
	iput(type);

	/* Store the type for PUBDEF and EXTDEF records. */
	if (sp != NULL) {
		sp->s_type = type;
		/* Begin collecting parameter types */
		if ((class == DC_GDEF || class == DC_SEX)
		 && sp->s_seg == SCODE) {
			curfsp = sp;
			curfna = 0;
		}
	}
	/* Collect parameter types */
	if (class == DC_PAUTO || class == DC_PREG)
		curfar[curfna++] = type;
	/* Dump the parameter list, tricky stuff. */
	if (class == DC_LINE && id[0] == '{' && curfna >= 0) {
		arglisttype();
		curfna = -1;
	}
	/* Back patch tag definition */
	if (ttype != 0)
		tpatch(ttype, type);
}

/*
 * Enter a patch into the typefix list.
 */
tpatch(ttype, type)
{
	register TYFIX *tp;

	tp = (TYFIX *) malloc(sizeof(*tp));
	if (tp == NULL) dbmemory(__LINE__);
	tp->t_ttype = ttype;
	tp->t_type = type;
	tp->t_tp = typefixp;
	typefixp = tp;
}

/*
 * Return type index associated with type definition
 * at infop of length len.
 * Define a new TINFO item if necessary.
 */
tlookup(infop, len)
register char *infop;
int len;
{
#if	INTEL
	register TINFO *tp, *lasttp;
	register int i, index;
	register char *cp1, *cp2;
	tp = typelistp;
	index = 1;
	/* Search the TINFO list for the type. */
	while (tp != NULL) {
		if (tp->t_length == len) {
			cp1 = infop;
			cp2 = tp->t_info;
			i = len;
			while ((*cp1++ == *cp2++) && (--i > 0))
				;
			if (i == 0) return (index);
		}
		index++;
		lasttp = tp;
		tp = tp->t_tp;
	}
	/* Not found, add new type to TINFO list. */
	if ((tp=(TINFO *)malloc(sizeof(TINFO)+len)) == NULL)
		cnomem("tlookup");
	tp->t_tp = NULL;
	tp->t_tflag = 0;
	tp->t_length = len;
	cp1 = tp->t_info;
	while (len--)
		*cp1++ = *infop++;
	/* Outinit puts the nil type on the list so never empty */
	lasttp->t_tp = tp;
	return (index);
#else
	return (1);
#endif
}

/*
 * Length and OMF type of scalar C types.
 * The order corresponds to the order of DT_ types in ops.h.
 */
#if	INTEL
static char ntypes [DT_DOUBLE+1][2] = {
	{ 0x00, OTNIL },		/* DT_NONE */
	{ 0x08, OTSINT },		/* DT_CHAR */
	{ 0x08, OTUINT },		/* DT_UCHAR */
	{ 0x10, OTSINT },		/* DT_SHORT */
	{ 0x10, OTUINT },		/* DT_USHORT */
	{ 0x10, OTSINT },		/* DT_INT */
	{ 0x10, OTUINT },		/* DT_UINT */
	{ 0x20, OTSINT },		/* DT_LONG */
	{ 0x20, OTUINT },		/* DT_ULONG */
	{ 0x20, OTREAL },		/* DT_FLOAT */
	{ 0x40, OTREAL }		/* DT_DOUBLE */
};
#endif

/*
 * Read the dimension/type/info list from input
 * and return the Ooomf type index.
 * Store the size back through the pointer supplied.
 * This routine is reentrant so that
 * types of structure members are mappable while
 * the structure definition is mapped.
 */

dbtmap(ttype)
{
	int		type;
	unsigned	size;
	register char	*bp;
	char		bb[10];

	bp = bb;
	type = bget();
	size = iget();
	switch (type) {
	case DT_NONE:
#if	INTEL
		*bp++ = OTNIL;
#endif
		break;
	case DT_CHAR:
	case DT_UCHAR:
	case DT_SHORT:
	case DT_USHORT:
	case DT_INT:
	case DT_UINT:
	case DT_LONG:
	case DT_ULONG:
	case DT_FLOAT:
	case DT_DOUBLE:
#if	INTEL
		*bp++ = OTSCAL;			/* SCALAR */
		bp = setsize(bp, (long)(ntypes[type][0]));	/* length */
		*bp++ = ntypes[type][1];	/* OMF type */
#endif
		break;
	case DT_VOID:
#if	INTEL
		*bp++ = OTNIL;
#endif
		break;
	case DT_STRUCT:
	case DT_UNION:
	case DT_ENUM:
		if (bget() == DX_NAME) {
			lastsize = ((long)size) * NBPBYTE;
			return (dbxname(ttype));
		}
		else if (type == DT_STRUCT)
			return (dbxstruct(size, ttype));
		else if (type == DT_UNION)
			return (dbxunion(size, ttype));
		else
			return (dbxenum(size, ttype));
	case DD_PTR:
#if	INTEL
		*bp++ = OTPOINT;
		*bp++ = OTINDEX;		
#endif
		type = dbtmap(ttype);
#if	INTEL
		bp = setindex(bp, type);
		type = tlookup(bb, bp-bb);
		bp = bb;
		*bp++ = OTSCAL;
		bp = setsize(bp, isvariant(VLARGE) ? 0x20L : 0x10L);
		*bp++ = OTINDEX;
		bp = setindex(bp, type);
#endif
		break;
	case DD_FUNC:
#if	INTEL
		*bp++ = OTPROC;
		*bp++ = OTNIL;
		*bp++ = OTINDEX;
#endif
		type = dbtmap(ttype);
#if	INTEL
		bp = setindex(bp, type);
		if (isvariant(VLARGE)) *bp++ = OTLFUNC;
		else *bp++ = OTSFUNC;
		*bp++ = 0;	/* Zero args, possibly corrected later */
#endif
		break;
	case DD_ARRAY:
		type = dbtmap(ttype);
#if	INTEL
		*bp++ = OTARRAY;
		bp = setsize(bp, ((long)size)*NBPBYTE);
		*bp++ = OTINDEX;
		bp = setindex(bp, type);
#endif
		break;
	default:
		dbfatal(__LINE__);
	}
	return (tlookup(bb, bp-bb));
}

dbfatal(lineno)
{
	cbotch("debug label format in %d", lineno);
}

dbmemory(lineno)
{
	cnomem("dbmemory %d", lineno);
}

/*
 * Construct typdef for enum, struct, or union.
 */
dbxname(ttype)
{
	register char	*bp;
	register TINFO	*tp;
	register int i;

	/* DX_NAME records supply tag name of type which
	 * will be defined later.
	 * Tag references point to type consisting of
	 * the tag name; type will be backpatched into
	 * a struct/union/enum typdef */
	iget();		/* Dummy size */
	bp = &id[0];
	while (*bp = bget())
		++bp;
	*bp = '_';
	*++bp = 0;
	i = tlookup(id, ++bp-id);
	tp = gettype(i);
	tp->t_tflag = 1;
	return(i);
}

dbxstruct(s, ttype)
unsigned	s;	/* Size of structure in bytes */
{
	int	nmemb;	/* The number of members */
	int	i;	/* Member counter */
	unsigned	bwid;	/* Bit width of current field */
	unsigned	boff;	/* Bit offset of current field */
	unsigned	valu;	/* Byte offset of current member */
	int	type;	/* type index of current member */
#if	INTEL
	int	size;	/* Size of name field */
	long	offs;	/* Bit offset computed from valu and boff */
	long	loffs;	/* Bit offset computed from previous member */
	int	nname;	/* Number of bytes required for name list */
	int	tlist;	/* Type list index */
	int	nlist;	/* Name list index */
	struct	slist {
		struct slist *sl_sp;
		int	sl_type;
		char	sl_id[];
	};
	struct slist *sp1;
	register struct slist *sp2;
	char *lp;
	register char *bp;
	char bb[64];
#endif

	/* Struct member list */
#if	INTEL
	sp1 = NULL;
	offs = loffs = 0;
	nname = 0;
#endif
	nmemb = iget();		/* Number of members */
				/* Does not include filler fields */
	/* Read in member descriptions */
	for (i = nmemb; i > 0; i -= 1) {
		if (bget() != DC_MOS)
			dbfatal(__LINE__);
		iget();		/* forget line */
		bwid = bget();	/* bit width */
		boff = bget();	/* bit offset */
		valu = iget();	/* byte offset */
		sget(id, NCSYMB);
#if	INTEL
		strcat(id, "_");
		/* Enter into list, possibly prefixing nil filler field */
		offs = (long)valu * (long)NBPBYTE + boff;
		if (offs != loffs) {
			/* Nil named filler field so pscope doesn't lose */
			/* its place in the structure */
			nmemb += 1;
			nname += 2;	/* Bytes required for nlist */
			sp2 = (struct slist *)malloc(sizeof(*sp2)+1);
			if (sp2 == NULL)
				dbmemory(__LINE__);
			sp2->sl_type = dbfmap((int)(offs-loffs), DT_UINT);
			sp2->sl_id[0] = 0;
			sp2->sl_sp = sp1;
			sp1 = sp2;
		}
		size = strlen(id)+2;	/* OTNAME, count/nul */
		nname += size;
		sp2 = (struct slist *)malloc(sizeof(*sp2)+size);
		if (sp2 == NULL) dbmemory(__LINE__);
		strcpy(sp2->sl_id, id);
#endif
		/* Now get the type with its possible id field */
		if (bwid != 0) {
			/* Only fields have non-zero widths and offsets */
			type = dbfmap(bwid, bget());
			iget();	/* Subtype size */
			lastsize = bwid;
		} else {
			/* Non field member, must lookup its width */
			type = dbtmap(0);
		}
#if	INTEL
		sp2->sl_type = type;
		sp2->sl_sp = sp1;
		sp1 = sp2;
		loffs = offs + lastsize;
#endif
	}
#if	INTEL
	/* Reverse member list */
	sp2 = sp1;
	sp1 = NULL;
	while (sp2 != NULL) {
		struct slist *tsp;

		tsp = sp2->sl_sp;
		sp2->sl_sp = sp1;
		sp1 = sp2;
		sp2 = tsp;
	}
	/* Construct type list */
	lp = bp = malloc(nmemb*3+nmemb/8+8);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTLIST;
	sp2 = sp1;
	for (i = 0; i < nmemb; i += 1) {
		if ((i%8) == 7) *bp++ = 0;	/* EN byte */
		*bp++ = OTINDEX;
		bp = setindex(bp, sp2->sl_type);
		sp2 = sp2->sl_sp;
	}
	tlist = tlookup(lp, bp-lp);
	free(lp);
	/* Construct name list */
	lp = bp = malloc(nname+nmemb/8+8);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTLIST;
	sp2 = sp1;
	for (i = 0; i < nmemb; i += 1) {
		if (sp2==NULL) dbfatal(__LINE__);
		if ((i%8) == 7) *bp++ = 0;	/* EN byte */
		*bp++ = OTNAME;
		bp = setsymid(bp, sp2->sl_id);
		sp2 = sp2->sl_sp;
		free(sp1);
		sp1 = sp2;
	}
	nlist = tlookup(lp, bp-lp);
	free(lp);
	/* Construct the true type record */
	bp = &bb[0];
	*bp++ = OTSTRUC;
	/* Set length of structure */
	bp = setsize(bp, ((long)s)*NBPBYTE);
	/* Set number of members */
	bp = setnumb(bp, (long)nmemb);
	*bp++ = OTINDEX;
	bp = setindex(bp, tlist);
	*bp++ = OTINDEX;
	bp = setindex(bp, nlist);
	*bp++ = OTNAME;
	if (ttype == 0)
		*bp++ = 0;
	else
		bp = setsymid(bp, gettype(ttype)->t_info);
	return (tlookup(bb, bp-bb));
#else
	return (1);
#endif
}

/*
 * Map a bitfield type.
 * Should only have CHAR, UCHAR, sort of type.
 */
dbfmap(bw, bt)
int bw;		/* Bit width */
int bt;		/* Base type */
{
#if	INTEL
	char bb[4];

	bb[0] = OTSCAL;
	bb[1] = bw;
	switch (bt) {
	case DT_CHAR:	case DT_SHORT:	case DT_INT:	case DT_LONG:
		bb[2] = OTSINT;
		break;
	case DT_UCHAR:	case DT_USHORT:	case DT_UINT:	case DT_ULONG:
		bb[2] = OTUINT;
		break;
	default:
		dbfatal(__LINE__);
	}
	return (tlookup(bb, 3));
#else
	return (1);
#endif
}

dbxunion(s, ttype)
unsigned	s;	/* Size of union in bytes */
{
	int	nmemb;
	int	i;
#if	INTEL
	char	*lp;
	register char *bp;
	int size;
#endif

	/* Union member list */
	nmemb = iget();		/* Number of members in union */
#if	INTEL
	size = (ttype) ? strlen(gettype(ttype)->t_info) : 0;
	lp = bp = malloc(18+7*nmemb+(10+3*nmemb)/8+1+size);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTSTRUC;
	bp = setsize(bp, ((long)s)*NBPBYTE);
	*bp++ = 0;		/* Invariant records */
	*bp++ = OTNIL;		/* Invariant type list */
	*bp++ = OTNIL;		/* Invariant name list */
	*bp++ = OTNAME;
	if (ttype == 0)
		*bp++ = 0;
	else
		bp = setsymid(bp, gettype(ttype)->t_info);
	*bp++ = OTUPACK;	/* Unpacked or something */
	bp = setnumb(bp, (long)nmemb);
	*bp++ = 0;		/* EN byte, for more leaves */
	*bp++ = OTNIL;		/* Selector field type */
	*bp++ = OTNIL;		/* Selector field name */
#endif
	i = 1;
	while (--nmemb >= 0) {
		int	type;
#if	INTEL
		int	tlist;
		int	nlist;
		char	bb[50];
		register char *tbp;
#endif

		if (bget() != DC_MOU) dbfatal(__LINE__);
		iget();		/* forget line */
		bget();		/* forget bit width */
		bget();		/* forget bit offset */
		iget();		/* forget byte offset */
		sget(id, NCSYMB);
#if	INTEL
		/* Make name list item */
		strcat(id, "_");
		tbp = bb;
		*tbp++ = OTLIST;
		*tbp++ = OTNAME;
		tbp = setsymid(tbp, id);
		nlist = tlookup(bb, tbp-bb);
#endif
		type = dbtmap(0);
#if	INTEL
		/* Make type list item */
		tbp = bb;
		*tbp++ = OTLIST;
		*tbp++ = OTINDEX;
		tbp = setindex(tbp, type);
		tlist = tlookup(bb, tbp-bb);
		/* Append member items to typdef */
		if ((++i%8) == 0) *bp++ = 0;	/* EN extend list */
		*bp++ = 1;
		if ((++i%8) == 0) *bp++ = 0;
		*bp++ = OTINDEX;
		bp = setindex(bp, tlist);
		if ((++i%8) == 0) *bp++ = 0;
		*bp++ = OTINDEX;
		bp = setindex(bp, nlist);
#endif
	}
#if	INTEL
	lastsize = ((long)s) * NBPBYTE;
	i = tlookup(lp, bp-lp);
	free(lp);
	return (i);
#else
	return (1);
#endif
}

dbxenum(s, ttype)
unsigned	s;	/* Size of enum type in bytes */
{
	int	nmemb;
	int	i;
	int	valu;
	int	nname;
#if	INTEL
	int	minvalu;
	int	maxvalu;
	int	range;
	int	nlist;
	struct el {
		struct el *el_ep;
		int	el_valu;
		char	el_id[];
	};
	struct el **epp;
	register char *bp;
	char *lp;
	char bb[16];
#endif
	
	/* Enum member list */
	nname = 0;
	nmemb = iget();		/* Number of members in enum */
#if	INTEL
	epp = (struct el **)malloc(nmemb * sizeof(*epp));
	if (epp == NULL) dbmemory(__LINE__);
	maxvalu = -MAXIV-1;
	minvalu = MAXIV;
#endif
	for (i = 0; i < nmemb; i += 1) {
#if	INTEL
		int size;
		register struct el *ep;
#endif


		if (bget() != DC_MOE) dbfatal(__LINE__);
		iget();		/* forget line */
		valu = iget();	/* value */
		sget(id, NCSYMB);
#if	INTEL
		strcat(id, "_");
		size = strlen(id)+2;
		nname += size;
#endif
		dbtmap(0);
#if	INTEL
		ep = malloc(sizeof(*ep)+size);
		if (ep == NULL) dbmemory(__LINE__);
		ep->el_valu = valu;
		strcpy(ep->el_id, id);
		if (valu < minvalu)
			minvalu = valu;
		else if (valu > maxvalu)
			maxvalu = valu;
		epp[i] = ep;
#endif
	}
#if	INTEL
	/* Construct name list */
	range = maxvalu - minvalu + 1;
	if (range > nmemb)
		nname += (range-nmemb)*2;
	nname += range/8 + 3;
	lp = bp = malloc(nname);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTLIST;
	for (valu = minvalu; valu <= maxvalu; valu += 1) {
		if (((valu-minvalu)%8) == 7) *bp++ = 0;	/* EN byte */
		*bp++ = OTNAME;
		for (i = 0; i < nmemb; i += 1)
			if (epp[i]->el_valu == valu)
				break;
		if (i < nmemb) {
			bp = setsymid(bp, epp[i]->el_id);
			free(epp[i]);
		} else
			*bp++ = 0;
		if (bp > lp + nname) cbotch("enum overrun");
	}
	free(epp);
	nlist = tlookup(lp, bp-lp);
	free(lp);
	/* Construct type record */
	bp = &bb[0];
	*bp++ = OTSCAL;
	bp = setsize(bp, ((long)s)*NBPBYTE);
	*bp++ = OTSINT;
	*bp++ = OTNAME;
	if (ttype == 0)
		*bp++ = 0;
	else
		bp = setsymid(bp, gettype(ttype)->t_info);
	*bp++ = OTINDEX;
	bp = setindex(bp, nlist);
	bp = setnumb(bp, (long)minvalu);
	bp = setnumb(bp, (long)maxvalu);
	return (tlookup(bb, bp-bb));
#else
	return (1);
#endif
}

/*
 * Construct list of arg types and return its type.
 * The list may contain more than 8 leaves, requiring extra EN bytes.
 * The function's type with zero args has already been computed.
 * We must expand the type and append nargs, arglist type, and
 * reenter.
 */
arglisttype()
{
#if	INTEL
	int type;
	int oldtypesize;
	char *oldtype;

	if (curfna == 0)
		return;
	{	/* Compute argument list type */
		register char *bp;
		register int i;
		register char *sbp;
		char b[3*NARGS+3];

		bp = &b[0];
		*bp++ = OTLIST;
		for (i=0; i<curfna; ++i) {
			if (i%8 == 7)
				*bp++ = 0;		/* EN byte */
			*bp++ = OTINDEX;
			sbp = bp;
			*bp++ = OTPARA;
			*bp++ = OTINDEX;
			bp = setindex(bp, curfar[i]);
			bp = setindex(sbp, tlookup(sbp, bp-sbp));
		}
		type = tlookup(b, bp-b);
	}
	{	/* Find old type */
		register TINFO *tp;

		tp = gettype(curfsp->s_type);
		oldtype = tp->t_info;
		oldtypesize = tp->t_length;
	}
	{	/* Expand and compute new type */
		register char *bp;
		char b[16];

		bp = b;
		while (--oldtypesize > 0)	/* Last byte should be zero */
			*bp++ = *oldtype++;
		bp = setnumb(bp, (long)curfna);	/* Number of args */
		*bp++ = OTINDEX;
		bp = setindex(bp, type);	/* Arg types */
		curfsp->s_type = tlookup(b, bp-b);
	}
#else
	return;
#endif
}

/*
 * Output a debug line record.
 */
outdlin(op)
{
	bput(TDLAB);
	bput(DC_LINE);
	iput(line);
	bput(op);
	bput(0);
	bput(1);	/* Nil type */
}

/*
 * Output a debug relocation record.
 * This 'n' is an index into the list of debug records written.
 */
outdloc(n)
{
	bput(TDLOC);
	iput(n);
}

/*
 * Finish up the code.
 */
outdone()
{
	if (notvariant(VASM))
		bput(TEND);
#if	EMUFIXUPS
	/*
	 * Put magic names into the symbol table
	 * if any 8087 code was generated.
	 */
	if (has8087) {
		wsp[0] = glookup("M:_WT",  0);
		wsp[1] = glookup("M:_WST", 0);
		wsp[2] = glookup("M:_WES", 0);
		wsp[3] = glookup("M:_WCS", 0);
		wsp[4] = glookup("M:_WSS", 0);
		wsp[5] = glookup("M:_WDS", 0);
	}
#endif
}

/*
** Second pass routines.
** Read the temporary file and write the output.
*/
/*
 * Finish up.
 * Figure out the sizes and final values
 * of everything. Copy the code from the scratch
 * file back to the output file.
 */
copycode()
{
	register int	op;
	register SYM	*sp;
	register int	i;
	register ADDRESS v;
	int		fixup;
	int		nd;
	int		data;
	char		coment[30];

	/*
	 * Assign the base addresses to the
	 * compiler's logical segments, based on the
	 * compilation model and the setting of the VROM
	 * variant flag. The VROM option only changes
	 * where the strings are placed.
	 */
	seg[SCODE].s_mseek = 0;
	v  = rup(seg[SCODE].s_dot);
	seg[SLINK].s_mseek = v;
	v += rup(seg[SLINK].s_dot);
	if ((isvariant(VSMALL))  || (isvariant(VRAM))) {
		code_size = v;
		v = 0;
	}
	seg[SPURE].s_mseek = v;
	v += rup(seg[SPURE].s_dot);
	if (notvariant(VROM) && notvariant(VRAM)) {
		if (isvariant(VSMALL))
			const_size = v;
		else
			code_size  = v;
		v = 0;
	}
	seg[SSTRN].s_mseek = v;
	v += rup(seg[SSTRN].s_dot);
	if (isvariant(VROM)) {
		if (isvariant(VSMALL))
			const_size = v;
		else
			code_size  = v;
		v = 0;
	}
	seg[SDATA].s_mseek = v;
	v += rup(seg[SDATA].s_dot);
	seg[SBSS].s_mseek  = v;
	bss_size  = rup(seg[SBSS].s_dot);
	data_size = v + bss_size;
	/*
	 * Put out the preamble.
	 */
	puttheadr();
	coment[0] = coment[1] = '\0';
	sprintf(&coment[2], "%s %s %s %s",
#ifdef vax
		"VAX",
#else
#ifdef UDI
		"UDI",
#else
#ifdef MSDOS
		"MS-DOS",
#else
		"COHERENT",
#endif
#endif
#endif
#if	INTEL
		"iC86", VERSINT,
#else
		"MWC86", VERSMWC,
#endif
		(isvariant(VSMALL)) ? "SMALL" : "LARGE");
	putrecord(COMENT, coment, strlen(&coment[2])+2);
	if (isvariant(VLARGE)) {
		putlnames();
		putsegdef(PARA|CCON, code_size, 2, 4);
		putsegdef(PARA|CCON, data_size, 3, 5);
	} else {
		putrecord(LNAMES, lnames, sizeof(lnames));
		putsegdef(PARA|CCON, code_size,  2, 2);
		putsegdef(PARA|CCON, const_size, 4, 4);
		putsegdef(PARA|CCON, data_size,  3, 3);
		putsegdef(PARA|CSTACK,       0,  5, 5);
		putsegdef(PARA|CMEM,	 0,  6, 6);
		putrecord(GRPDEF, gdef1, sizeof(gdef1));
		putrecord(GRPDEF, gdef2, sizeof(gdef2));
	}
	/*
	 * Write out type and symbol definitions.
	 */
	puttypdef();
	putsymdef();

	/*
	 * Copy out code.
	 */
	if (isvariant(VLARGE))
		putrecord(FIXUPP, lthred, sizeof(lthred));
	else
		putrecord(FIXUPP, sthred, sizeof(sthred));
	for (i=0; i<NSEG; ++i)
		seg[i].s_dot = seg[i].s_mseek;
	dotseg = SCODE;
	dot = seg[SCODE].s_dot;
	while ((op = bget()) != TEND) {
		switch (op) {

		case TENTER:
			notenuf();
			seg[dotseg].s_dot = dot;
			dotseg = bget();
			dot = seg[dotseg].s_dot;
			continue;
		case TDLAB:
			getdlab();
			continue;
		case TDLOC:
			getdloc();
			continue;
#if	0
/* See comment on outcb()/outdb() above. */
		case TCODE:
		case TDATA:
			if (isvariant(VSMALL))
				cbotch("code:data");
			enuf(2, 4);
			fixup = txtp - (txt+3);
			*txtp++ = 0;
			*txtp++ = 0;
			*relp++ = LOCAT|M|BASE|(fixup>>8);
			*relp++ = fixup;
			*relp++ = FTARGET|SECWAY|TSI;
			*relp++ = (op==TCODE) ? 1 : 2;
			dot += 2;
			continue;
#endif
		case TBASE:
			if (isvariant(VSMALL))
				cbotch("base");
			sp = pget();
			if ((sp->s_flag&S_DEF) != 0) {
				enuf(2, 4);
				fixup = txtp - (txt+3);
				*txtp++ = 0;
				*txtp++ = 0;
				*relp++ = LOCAT|M|BASE|(fixup>>8);
				*relp++ = fixup;
				*relp++ = FTARGET|SECWAY|TSI;
				*relp++ = getsegindex(sp->s_seg);
				dot += 2;
				continue;
			}
			enuf(2, 5);
			fixup = txtp - (txt+3);
			*txtp++ = 0;
			*txtp++ = 0;
			*relp++ = LOCAT|M|BASE|(fixup>>8);
			*relp++ = fixup;
			*relp++ = FTARGET|SECWAY|TEI;
			relp = setindex(relp, sp->s_ref);
			dot += 2;
			continue;
		}
		if ((op&TTYPE) == TBYTE) {
			nd = 1;
			data = bget();
		} else {
			nd = 2;
			data = iget();
		}
#if	EMUFIXUPS
		/*
		 * 8087 opcodes.
		 * Apply magic relocation for the emulator.
		 */
		if ((op&T8087) != 0) {
			sp = wsp[((op&T8087)-TWT)>>5];
			enuf(nd, 7);
			fixup = txtp - (txt+3);
			*txtp++ = data;
			if (nd != 1) {
				*txtp++ = data >> 8;
				fixup |= OFFSET << 8;	/* Locat */
			}
			fixup |= M << 8;		/* Seg rel */
			*relp++ = (fixup>>8) | LOCAT;	/* Not thread */
			*relp++ = fixup;
			*relp++ = FD8087;		/* Symbol based */
			*relp++ = 0;			/* Frame # */
			*relp++ = 0;
			relp = setindex(relp, sp->s_ref);
			dot += nd;
			continue;
		}
#endif
		/*
		 * Absolute.
		 */
		if ((op&(TSYM|TPCR)) == 0) {
			enuf(nd, 0);
			*txtp++ = data;
			if (nd != 1)
				*txtp++ = data >> 8;
			dot += nd;
			continue;
		}
		/*
		 * Absolute PC rel.
		 */
		if ((op&TSYM) == 0) {
			enuf(nd, 6);
			fixup = txtp - (txt+3);
			*txtp++ = 0;
			if (nd != 1) {
				*txtp++ = 0;
				fixup |= OFFSET << 8;
			}
			*relp++ = (fixup>>8) | LOCAT;	/* Not thread */
			*relp++ = fixup;
			*relp++ = FTARGET|TFN;		/* Absolute frame */
			*relp++ = 0;			/* Frame number */
			*relp++ = data;			/* Disp */
			*relp++ = data >> 8;
			dot += nd;
			continue;
		}
		/*
		 * Symbol based.
		 * I would like to always do this
		 * in the primary way. However, LINK-86
		 * doesn't do 3 byte offsets, so I cannot
		 * do a negative one.
		 */
		sp = pget();
		if ((sp->s_flag&S_DEF) != 0) {
			data += sp->s_value;
			/* Absolute */
			if ((op&TPCR)!=0 && sp->s_seg==dotseg) {
				enuf(nd, 0);
				data -= dot+nd;
				*txtp++ = data;
				if (nd != 1)
					*txtp++ = data >> 8;
				dot += nd;
				continue;
			}
			/* Segment relative */
			enuf(nd, 6);
			fixup = txtp - (txt+3);
			*txtp++ = 0;
			if (nd != 1) {
				*txtp++ = 0;
				fixup |= OFFSET << 8;	/* Loc */
			}
			if ((op&TPCR) == 0)
				fixup |= M << 8;	/* Seg rel */
			*relp++ = (fixup>>8) | LOCAT;	/* Not thread */
			*relp++ = fixup;
			fixup = F|FT0|TSI;		/* F code, seg ind */
			if (getsegindex(sp->s_seg) != 1)
				fixup |= FT1;		/* F data */
			*relp++ = fixup;
			*relp++ = getsegindex(sp->s_seg);
			*relp++ = data;			/* Disp */
			*relp++ = data >> 8;
			dot += nd;
			continue;
		}
		/*
		 * Symbol relative.
		 */
		enuf(nd, 5);
		fixup = txtp - (txt+3);
		*txtp++ = data;
		if (nd != 1) {
			*txtp++ = data >> 8;
			fixup |= OFFSET << 8;		/* Loc */
		}
		if ((op&TPCR) == 0)
			fixup |= M << 8;		/* Seg rel */
		*relp++ = LOCAT|(fixup>>8);		/* Not THREAD */
		*relp++ = fixup;
		*relp++ = FTARGET|SECWAY|TEI;		/* Fix dat */
		relp = setindex(relp, sp->s_ref);
		dot += nd;
	}
	notenuf();					/* Flush */
	/*
	 * If the size of the SBSS segment
	 * is non zero, use an LIDATA item to get it
	 * zeroed when the program is run.
	 */
	if (bss_size != 0) {
		txtp = &txt[0];
		if (isvariant(VSMALL))
			*txtp++ = 3;			/* DATA */
		else
			*txtp++ = 2;			/* module_DATA */
		*txtp++ = seg[SBSS].s_mseek;		/* Offset */
		*txtp++ = seg[SBSS].s_mseek >> 8;
		*txtp++ = bss_size;			/* Length of loop */
		*txtp++ = bss_size >> 8;
		*txtp++ = 0;
		*txtp++ = 0;
		*txtp++ = 1;
		*txtp++ = 0;
		putrecord(LIDATA, txt, 9);		/* Emit OMF */
	}
	dbdump();	/* Clean up db tables */
	putrecord(MODEND, modend, sizeof(modend));
#if	VAXFMT
	vaxflush();
#endif
}

/*
** Header writing routines.
*/
/*
 * Put out a THEADR item.
 * The name is just the module name that
 * the compiler has been keeping. It must be
 * mapped into upper case before writing
 * it to the file.
 */
puttheadr()
{
	register char *p1, *p2;
	register c;
	char b[40];

	p1 = module;
	p2 = &b[0];
	*p2++ = strlen(p1);
	while ((c = *p1++) != '\0') {
		if (c>='a' && c<='z')
			c -= 'a'-'A';
		*p2++ = c;
	}
	putrecord(THEADR, b, p2-b);
}

/*
 * Make up the list of names
 * record for the LARGE model of segmentation.
 * The names are set up like this: 1 => "",
 * 2 => "module_CODE", 3 => "module_DATA", 4 => "CODE"
 * 5 => "DATA".
 */
putlnames()
{
	register char *p1, *p2;
	register c;
	char *movelname();
	char b[40+40+11];

	p1 = &b[0];
	*p1++ = 0;					/* EMPTY */
	p1 = movelname(p1, "CODE");
	p1 = movelname(p1, "DATA");
	p2 = "\004CODE\004DATA";
	while ((c = *p2++) != '\0')
		*p1++ = c;
	putrecord(LNAMES, b, p1-b);
}

/*
 * Move the module name with the supplied suffix to the supplied buffer.
 * Used by putlnames().
 */
char *
movelname(p1, p2)
register char *p1;
char *p2;
{
	register char *p3;
	register c;

	p3 = module;
	*p1++ = strlen(p3) + 5;		/* "_CODE" or "_DATA" */
	while ((c = *p3++) != '\0') {
		if (c>='a' && c<='z')
			c -= 'a'-'A';
		*p1++ = c;
	}
	*p1++ = '_';
	p3 = p2;
	while ((c = *p3++) != '\0')
		*p1++ = c;
	return (p1);
}

/*
 * This routine formats and outputs a SEGDEF
 * item. There is no checking for BIG segments (these
 * are segments that are 64k in size).
 * The overlay index is set to point to a null name.
 */
putsegdef(flag, size, segi, classi)
{
	register char *bp;
	char b[6];

	bp = &b[0];
	*bp++ = flag;
	*bp++ = size;
	*bp++ = size >> 8;
	*bp++ = segi;
	*bp++ = classi;
	*bp++ = 1;
	putrecord(SEGDEF, b, 6);
}

/*
 * Write TYPDEF records.
 * This assumes that the length member directly precedes the info array.
 */
puttypdef()
{
	register TINFO *tp;
	register int length;
	register TINFO *ttp;

	fixpatch();
	tp = typelistp;
	while (tp != NULL) {
		length = tp->t_length + 2;
		tp->t_length = 0;
		if (tp->t_tflag) {
			length = 3;
			tp->t_info[0] = OTNIL;
		}
		putrecord(TYPDEF, tp->t_info - 2, length);
		ttp = tp;
		tp = tp->t_tp;
		free(ttp);
	}
}

fixpatch()
{
	register TYFIX *fp;
#if	INTEL
	register TINFO *ttp, *pttp, *tp, *ptp;
#endif

	while ((fp = typefixp) != NULL) {
#if	INTEL
		if (fp->t_ttype >= fp->t_type) dbfatal(__LINE__);
		/*
		 * This depends on NIL being at type index 1.
		 * Excise the dummy type def.
		 */
		pttp = gettype(fp->t_ttype-1);
		ttp = pttp->t_tp;
		pttp->t_tp = ttp->t_tp;
		/*
		 * Excise the real type def.
		 */
		ptp = gettype(fp->t_type-2);
		tp = ptp->t_tp;
		ptp->t_tp = tp->t_tp;
		/*
		 * Insert the dummy and blank it.
		 */
		ttp->t_tp = ptp->t_tp;
		ptp->t_tp = ttp;
		ttp->t_length = 1;
		ttp->t_tflag = 0;
		ttp->t_info[0] = OTNIL;
		/*
		 * Insert the real definition.
		 */
		tp->t_tp = pttp->t_tp;
		pttp->t_tp = tp;
#endif
		/*
		 * Release the memory.
		 */
		typefixp = fp->t_tp;
		free(fp);
	}
}

static TINFO *
gettype(index)
{
	register TINFO *tp;

	tp = typelistp;
	while (--index != 0)
		if ((tp = tp->t_tp) == NULL)
			dbfatal(__LINE__);
	return (tp);
}

/*
 * Sweep through the symbol table:
 *	1) Make symbols LSEG relative.
 *	2) Assign reference numbers.
 *	3) Output EXTDEF or PUBDEF.
 *	4) Output DEBSYM.
 */
putsymdef()
{
	register SYM *sp;
	register int refnum;
	register int i;

	refnum = 1;
	for (i=0; i<NSHASH; ++i) {
		sp = hash2[i];
		while (sp != NULL) {
			if ((sp->s_flag&S_DEF) != 0)
				sp->s_value += seg[sp->s_seg].s_mseek;
			if ((sp->s_flag&S_GBL)!=0 || (sp->s_flag&S_DEF)==0) {
				sp->s_ref = 0;
				if ((sp->s_flag&S_DEF) != 0) {
					putpubdef(sp);
				} else {
					sp->s_ref = refnum++;
					putextdef(sp);
				}
			}
			sp = sp->s_fp;
		}
	}
}

/*
 * Put out a PUBDEF item for symbol 'sp'.
 * All defs are tagged with the group
 * so that a FTARGET,EI fixup can determine the correct frame.
 */
putpubdef(sp)
register SYM *sp;
{
	register char *bp;
	char b[1+1+40+2+1];

	bp = &b[0];
	*bp++ = getgrpindex(sp->s_seg);
	*bp++ = getsegindex(sp->s_seg);
	bp = setsymid(bp, sp->s_id);
	*bp++ = sp->s_value;
	*bp++ = sp->s_value >> 8;
	if (sp->s_type == 0)
		*bp++ = 1;
	else
		bp = setindex(bp, sp->s_type);
	putrecord(PUBDEF, b, bp-b);
}

/*
 * Put out an EXTDEF item for symbol 'sp'.
 */
putextdef(sp)
register SYM *sp;
{
	register char *bp;
	char b[40+2];

	bp = setsymid(b, sp->s_id);
	if (sp->s_type == 0)
		*bp++ = 1;
	else
		bp = setindex(bp, sp->s_type);
	putrecord(EXTDEF, b, bp-b);
}

/*
 * Translate compiler segment codes
 * to OMF segment index value, using tables.
 * The table is selected by the model and the
 * setting of the VROM variant.
 */
getsegindex(s)
register int	s;
{
	if (isvariant(VSMALL)) {
		if (isvariant(VRAM))
			return (sramseg[s]);
		if (notvariant(VROM))
			return (snonseg[s]);
		return (sromseg[s]);
	}
	if (isvariant(VRAM))
		return (lramseg[s]);
	if (notvariant(VROM))
		return (lnonseg[s]);
	return (lromseg[s]);
}

/*
 * Translate compiler segment codes
 * to OMF group index value, using table and model.
 */
getgrpindex(s)
register int s;
{
 	if (isvariant(VSMALL)) return (grpindex[s]);
	else return (0);
}

/*
** Debug information processors.
*/
/*
 * Structure to contain debug items on this pass.
 */
struct dbgt {
	struct dbgt *d_dp;		/* Link */
	int d_ref;			/* Index number */
	unsigned char d_class;		/* Storage class */
	unsigned char d_flag;		/* Flags */
	unsigned char d_level;		/* Bracket level */
	unsigned char d_seg;		/* Segment */
	ADDRESS d_value;		/* Offset in segment */
	int d_type;			/* Type index */
	int d_line;			/* Line number */
	char d_id[];			/* Name */
};

struct dbgt *getdbgt();
struct dbgt *dbase;
struct dbgt *dnext;
#if	!TINY
#define	dbrpt(x,y)	_dbrpt(x,y)
#else
#define	dbrpt(x,y)	/* _dbrpt(x,y) */
#endif

/*
 * Read debug table item(s) into memory.
 * These are buffered until appropriate moments.
 */
getdlab()
{
	register struct dbgt *dp;
	static int refnum = 0;

	dp = getdbgt();
	if (dp->d_class == DC_LINE || dp->d_class == DC_LAB)
		dp->d_ref = refnum;
	refnum += 1;
	if (dbase == NULL)
		dbase = dp;
	if (dnext != NULL)
		dnext->d_dp = dp;
	dbrpt("enter", dp);
	while (dp->d_dp != NULL) {
		dp = dp->d_dp;
		dbrpt("enter", dp);
	}
	dnext = dp;
}

/*
 * Read in debug item in form abbreviated by outdlab().
 */
struct dbgt *
getdbgt()
{
	register char *cp;
	register struct dbgt *dp;
	static int level = 0;
	struct dbgt d;
	SYM *sp;

	d.d_level = level;

	/*
	 * Read the values in.
	 */
	d.d_class = bget();
	d.d_line = iget();
	d.d_value = iget();
	d.d_seg = bget();

	/*
	 * Read name.
	 */
	for (cp = id; *cp = bget(); cp += 1);

	/*
	 * Read type.
	 */
	d.d_type = iget();

	/*
	 * Check level change.
	 */
	if (d.d_class == DC_LINE) {
		switch (id[0]) {
		case '{': level += 1; break;
		case '}': level -= 1; break;
		}
	}

	/*
	 * Find location of static internal items.
	 */
	if (d.d_class == DC_SIN) {
		sp = llookup(d.d_value, 0);
		d.d_value = sp->s_value;
		d.d_seg = sp->s_seg;
	}

	/*
	 * Copy into dynamic storage.
	 */
	dp = (struct dbgt *)malloc(sizeof(struct dbgt) + cp - id + 1);
	if (dp == NULL)
		cnomem("getdbgt");
	dp->d_level = d.d_level;
	dp->d_class = d.d_class;
	dp->d_line = d.d_line;
	dp->d_seg = d.d_seg;
	if (d.d_seg < SANY)
		d.d_value += seg[d.d_seg].s_mseek;
	dp->d_value = d.d_value;
	dp->d_type = d.d_type;
	dp->d_dp = NULL;
	dp->d_ref = -1;
	while (cp >= id) {
		dp->d_id[cp-id] = *cp;
		cp -= 1;
	}
	return (dp);
}

/*
 * Read a debug relocation item and patch current 'dot'
 * into the appropriate debug/symbol table item.
 * If the debug item is '}' and level 1, then dump everything in the list.
 */
getdloc()
{
	register struct dbgt *dp;
	register int ref;

	ref = iget();
	for (dp = dbase; ; dp = dp->d_dp) {
		if (dp == NULL)
			cbotch("getdloc: %d", ref);
		if (dp->d_ref == ref)
			break;
	}
	dp->d_seg = dotseg;
	dp->d_value = dot;

	dbrpt("reloc", dp);
	if (dp->d_id[0] == '}' && dp->d_level == 1)
		dbdump();
}

/*
 * Dump all the debug symbols.
 */
dbdump()
{
	register struct dbgt *dp, **dpp;
	extern char *regnames[];

	dpp = &dbase;
	dnext = NULL;
	while ((dp = *dpp) != NULL) {
		switch (dp->d_class) {
		case DC_GDEF:		/* Global def. */
		case DC_SEX:		/* Static external */
			/* Function definitions replace initial { */
			if (dp->d_seg == SCODE) {
				/* The initial BLKDEF occurs before PROLOG
				 * at the function entry point.  Could be
				 * moved after PROLOG if desired.
				 * The last BLKEND occurs just
				 * before the EPILOG, and can also be moved.
				 */
				SYM *sp;

				sp = glookup(dp->d_id, 0);
				dp->d_type = sp->s_type;
				putlno(dp);
				putblkdef(dp);
			} else
				putdebsym(dp);
			break;
		case DC_GREF:		/* Global ref. */
			break;
		case DC_TYPE:		/* Typedef name */
		case DC_STAG:		/* Structure tag */
		case DC_UTAG:		/* Union tag */
		case DC_ETAG:		/* Enumeration tag */
		case DC_FILE:		/* Source file name */
			break;
		case DC_AUTO:		/* Auto */
		case DC_PAUTO:		/* Parametric auto */
			putlocsym(dp);
			break;
		case DC_SIN:		/* Static internal */
		case DC_LAB:		/* Label */
			putdebsym(dp);
			break;
		case DC_REG:		/* Register */
		case DC_PREG:		/* Parametric register */
#if	INTEL
			line = dp->d_line;
			cwarn("variable \"%s\" is in register %s",
				dp->d_id, regnames[dp->d_value]);
#endif
			break;
		case DC_MOE:		/* Member of enumeration */
			break;
		case DC_LINE:		/* Line number */
			switch (dp->d_id[0]) {
			case '{':	/* Block begins */
				/* GDEF or SEX takes first blkdef */
				if (dp->d_level != 0) {
					putlno(dp);
					putblkdef(dp);
				}
				break;
			case ';':	/* Line number */
				putlno(dp);
				break;
			case '}':	/* Block ends */
#ifdef NOABLOCKS
				if (blkcount > 0) {
					blkcount--;
					break;
				}
#endif
			 	putrecord(BLKEND, NULL, 0);
				break;
			default:
				cbotch("bad line record (dbdump)");
			}
			break;
		case DC_MOS:		/* Member of structure */
		case DC_MOU:		/* Member of union */
		case DC_CALL:		/* Function call */
			break;
		default:
			cbotch("bad class (dbdump)");
		}
		*dpp = dp->d_dp;
		free(dp);
	}
}

/*
 * Put out a DEBSYM item.
 * The symbol pointed to by 'dp' is a public
 * definition in this module.
 * This only gets executed under the VDEBUG option.
 */
putdebsym(dp)
register struct dbgt *dp;
{
	register char *bp;
	char b[1+1+1+40+2+1];

	bp = &b[0];
	*bp++ = 0;				/* B=0, L=0, FM=0 */
	*bp++ = getgrpindex(dp->d_seg);
	*bp++ = getsegindex(dp->d_seg);
	bp = setsymid(bp, dp->d_id);
	*bp++ = dp->d_value;
	*bp++ = dp->d_value >> 8;
	bp = setindex(bp, dp->d_type);
	putrecord(DEBSYM, b, bp-b);
}

/*
 * Process a local symbol definition.
 * Put out a DEBSYM record for symbol sp.
 */
putlocsym(dp)
register struct dbgt *dp;
{
	register char *bp;
	char b[1+2+40+2+1];

	bp = &b[0];
	*bp++ = 2;				/* B=0, L=0, FM=2 */
	bp = setindex(bp, blkindex);
	bp = setsymid(bp, dp->d_id);
	*bp++ = dp->d_value;
	*bp++ = dp->d_value >> 8;
	bp = setindex(bp, dp->d_type);
	putrecord(DEBSYM, b, bp-b);
}

/*
 * Process a block definition item.
 */
putblkdef(dp)
register struct dbgt *dp;
{
 	char b[1+1+40+2+2+1+2+1];
	register char *bp;
	char info = 0;
	int isproc;
	int episize;

	isproc = dp->d_id[0] != '{';
#ifdef NOABLOCKS
	if (!isproc) {
		blkcount++;
		return;
	}
#endif
	++blkindex;			/* update block index */
	bp = &b[0];
	*bp++ = getgrpindex(dp->d_seg);
	*bp++ = getsegindex(dp->d_seg);
	if (isproc)
		bp = setsymid(bp, dp->d_id);	/* name */
	else
		*bp++ = 0;			/* no name */
	*bp++ = dp->d_value;
	*bp++ = dp->d_value >> 8;		/* offset */
	dp->d_value = blklength(dp, &episize);
	if (isproc)
		dp->d_value += episize;
	*bp++ = dp->d_value;
	*bp++ = dp->d_value >> 8;		/* length */
	if (isproc) {
		info |= 0x80;			/* procedure bit */
		if (isvariant(VLARGE)) {
			info |= 0x40;
		}
		*bp++ = info;
		*bp++ = 6;
		*bp++ = 0;			/* return address offset */
		bp = setindex(bp, dp->d_type);	/* type index */
	} else
		*bp++ = info;
	putrecord(BLKDEF, b, bp-b);
}

blklength(dp, epp)
register struct dbgt *dp;
int *epp;
{
	register int level;
	register ADDRESS length;
	int autoseen;

	autoseen = 0;
	level = dp->d_level + 1;
	length = dp->d_value;
	while ((dp = dp->d_dp) != NULL) {
		if (dp->d_class == DC_AUTO)
			++autoseen;
		else if (dp->d_class == DC_LINE
		 && dp->d_id[0] == '}'
		 && dp->d_level == level) {
			*epp = (isvariant(V80186)) ? 4 : (autoseen) ? 6 : 4;
			return ((int) (dp->d_value - length));
		}
	}
	cbotch("missing blkend");
}

/*
 * Put out a line number item.
 */
putlno(dp)
register struct dbgt *dp;
{
	register char *p1;
	char b[10];
	static last = 0;

	if ((dp->d_line) == last)
		return;
	last = dp->d_line;
	p1 = &b[0];
	*p1++ = getgrpindex(dp->d_seg);
	*p1++ = getsegindex(dp->d_seg);
	*p1++ = dp->d_line;
	*p1++ = dp->d_line>>8;
	*p1++ = dp->d_value;
	*p1++ = dp->d_value>>8;
	putrecord(LINNUM, b, p1-b);
}

#if	!TINY
_dbrpt(p, dp)
char *p;
register struct dbgt *dp;
{
	if (xflag > 5)
		printf("%s:	%d %s %d %d %o %d\n",
			p,
			dp->d_class,
			dp->d_id,
			dp->d_ref,
			dp->d_level,
			dp->d_flag,
			dp->d_type,
			0);
}

#endif

/*
** Low level record formatting.
*/
/*
 * Splat a bit size for the people.
 */
long lastsize;
char *
setsize(bp, size)
char *bp;
long size;
{
	lastsize = size;
	return (setnumb(bp, size));
}

/*
 * Splat an unsigned numeric leaf.
 * This assumes that the leaf will fit in at most 3 bytes,
 * since the biggest objects the compiler knows about are
 * 64K bytes or NBPBYTE*64K = 512K bits.
 */
char *
setnumb(bp, numb)
char *bp;
unsigned long numb;
{
	if (numb < 128)
		*bp++ = numb;
	else  if (numb < 0x10000L) {
		*bp++ = OTUNUM2;
		*bp++ = numb;
		*bp++ = numb >> 8;
	}
	else {
		*bp++ = OTUNUM3;
		*bp++ = numb;
		*bp++ = numb >> 8;
		*bp++ = numb >> 16;
	}
	return (bp);
}

/*
 * Put OMF index i at cp and return updated cp.
 */
char *
setindex(cp, i)
register char *cp;
register unsigned i;
{
	if (i >= 128)
		*cp++ = (i >> 8) | 0x80;
	*cp++ = i;
	return (cp);
}

/*
 * This routine takes the name pointed to by 'cp' and
 * moves it into an OMF record. The 'bp' argument
 * is the OMF buffer pointer. The updated value
 * of 'bp' is returned.
 */
char *
setsymid(bp, cp)
register char *bp;
register char *cp;
{
	register c, n;

	if ((n=strlen(cp)) > 39) {
		cwarn("symbol \"%s\" truncated to 39 characters", cp);
		n = 39;
	}
	*bp++ = n;
	while (n--) {
		if ((c = *cp++)>='a' && c<='z')
			c -= 'a'-'A';
		*bp++ = c;
	}
	return (bp);
}

/*
 * Put a record.
 * The 'type' argument is the record type.
 * The body of the record starts at 'p' and is
 * 'n' bytes long. The checksum is added.
 */
putrecord(type, p, n)
register unsigned char *p;
register n;
{
	checksum = 0;
	putbyte(type);
	putbyte(n+1);		/* +1 for the checksum */
	putbyte((n+1)>>8);
	while(n--)
		putbyte(*p++);
	putbyte(-checksum);
}

putbyte(b)
register b;
{
#if	VAXFMT
	if (vaxptr >= &vaxbuf[NVDATA]) {
		vaxflush();
		vaxptr = vaxbuf;
	}
	*vaxptr++ = b;
#else
	bput(b);
#endif
	checksum += b;
}

/*
** Output buffering.
*/
/*
 * Check if there is enough room in the text and relocation
 * buffers to hold 'nt' bytes of text and 'nr' bytes of relocation.
 */
enuf(nt, nr)
{
	if (txtp+nt>&txt[NTXT] || relp+nr>&rel[NREL])
		notenuf();
	if (txtp == txt) {
		*txtp++ = getsegindex(dotseg);
		*txtp++ = dot;
		*txtp++ = dot >> 8;
	}
}

/*
 * Flush the buffer.
 */
notenuf()
{
	register n;

	if ((n = txtp-txt) > 3) {
		putrecord(LEDATA, txt, n);
		if ((n = relp-rel) != 0) {
			putrecord(FIXUPP, rel, n);
			relp = rel;
		}
	}
	txtp = txt;
}

/*
 * Round up to the next word.
 */
rup(a)
ADDRESS a;
{
	return ((a+01)&~01);
}

#if	VAXFMT
/*
 * Flush out a record on the VAX. The record is a standard,
 * RMS variable length record. This routine checks for no bytes
 * in the buffer, so it can be unconditionally called at the
 * end to flush the buffer out.
 * The record is padded out to NVDATA bytes with zeros. This
 * only happens on the last block; the data beyond the end
 * is zeroed.
 */
vaxflush()
{
	register int	n;

	if ((n = vaxptr-vaxbuf) != 0) {
		if (fwrite(vaxbuf, n, 1, ofp) != 1
		||  fflush(ofp) == EOF)
			cfatal("output write error");
	}
}
#endif

