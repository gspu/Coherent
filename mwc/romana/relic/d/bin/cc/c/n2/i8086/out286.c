/*
 * This file contains an output writer for Intel iAPX-80286 OMF.
 * Some of this code is from the i8086 OMF output writer.
 *
 * References:  "The Concrete Representation of 80286
 * Object Module Formats," Rev. 1.91 (c) Intel 1983.
 * "Type Definition Records for High Level Languages,"
 * Version 2.0, 6/25/84, #114834 (Intel proprietary).
 *
 * During code assembly, the 'out...' routines write to a temporary file.
 * At the end of assembly, 'copycode' writes the object file
 * using the 'write...' routines.
 * The 'put...' routines copy bits to the object file.
 * The object file contains a single linkable module
 * which in turn contains a single partition.
 * It may include SEGDEF, TYPDEF, PUBDEF, EXTDEF and TXTFIX sections.
 * The section lengths and locations in the table of contents
 * and the segment attributes and lengths in the SEGDEF section
 * are initially 0; 'fixoutput' seeks back in the output file
 * (or copies it back, in the case of the VAX version)
 * and patches them with the correct values.
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#include <time.h>
#else
#include "cc2.h"
#endif
#ifdef	COHERENT
#include <time.h>
#include <sys/timeb.h>
#endif
#ifdef UDI
#include <udi.h>
#endif
#ifndef __LINE__
#define	__LINE__	0
#endif
#if !TINY
#define dbrpt(x,y)	_dbrpt(x,y)
#else
#define dbrpt(x,y)	/* _dbrpt(x,y) */
#endif

/*
 * As of 4/29/85, PSCOPE does not support unnamed blocks correctly.
 * The output writer therefore suppresses unnamed blocks #ifdef NOABLOCKS.
 */
#if INTEL
#define	NOABLOCKS 1
int	blkcount;
#endif

#define	NTXT	256			/* Text buffer size */
#define	NFIX	256			/* Fixup buffer size */

/*
 * Scratch file opcodes.
 */
#define	TTYPE	0x0F			/* Type */
#define	TPCR	0x10			/* PC rel flag, ored in */
#define	TSYM	0x20			/* Symbol based flag, ored in */
#define	TEND	0			/* End marker */
#define	TENTER	1			/* Enter new area */
#define	TBYTE	2			/* Byte data */
#define	TWORD	3			/* Word data */
#define	TCODE	4			/* Code segment base */
#define	TDATA	5			/* Data segment base */
#define	TBASE	6			/* External segment base */
#define	TDLAB	7			/* Debug label */
#define TDLOC	8			/* Debug location */

/*
 * Sections, in the order of the table of contents.
 */
#define	NSECS	6			/* Number of sections */
#define	SEGSEC	0			/* SEGDEF section */
#define	GATSEC	1			/* GATDEF section */
#define	TYPSEC	2			/* TYPDEF section */
#define	PUBSEC	3			/* PUBDEF section */
#define	EXTSEC	4			/* EXTDEF section */
#define	TXTSEC	5			/* TXTFIX section */

/*
 * Segment indices, in the order of 'writesegdef'.
 */
#define	NSEGS	6			/* Number of segments if DEBUG */
#define	CODESEG	1			/* CODE */
#define	DATASEG	2			/* DATA */
#define	STKSEG	3			/* STACK */
#define	MODSEG	4			/* MODULES: */
#define	SYMSEG	5			/* SYMBOLS: */
#define	LINESEG	6			/* LINES: */

/*
 * 80286 OMF bits.
 */
#define	TID	6			/* C-286 translator id */
#define	SEGIN	(1<<13)			/* Segment internal name field */
#define	EXTIN	(3<<13)			/* External symbol int name field */
#define	BTEXT	0			/* TXTFIX text block */
#define	BFIXUP	1			/* TXTFIX fixup block */
#define	BITEXT	2			/* TXTFIX iterated text block */
#define	FORMSEL	(0<<2)			/* Fixup format selector only */
#define	FORMOFF	(1<<2)			/* Fixup format offset only */
#define	FORMSRO	(2<<2)			/* Fixup format self-relative offset */
#define	FORMFVA	(3<<2)			/* Fixup format full virtual addr */
#define	FIXGEN	0			/* Fixup type general */
#define	FIXISEG	1			/* Fixup type intrasegment */
#define	FIXCALL	2			/* Fixup type call */
#define	SYMBBEG	0			/* SYMBOLS block start */
#define	SYMPBEG	1			/* SYMBOLS procedure start */
#define	SYMBEND	2			/* SYMBOLS block end */
#define	SYMBASE	3			/* SYMBOLS base */
#define	SYMBPRB	4			/* SYMBOLS BP-relative base */
#define	SYMSYMB	5			/* SYMBOLS symbol */
#define	SEGARO	(0<<13)			/* SEGDEF access readonly */
#define	SEGARW	(1<<13)			/* SEGDEF access read-write */
#define	SEGAXR	(3<<13)			/* SEGDEF access execute-read */
#define	SEGDPL	(3<<11)			/* SEGDEF DPL */
#define	SEGEXT	(1<<6)			/* SEGDEF external */
#define	SEGNOC	0			/* SEGDEF combine type no-combine */
#define	SEGCOM	1			/* SEGDEF combine type normal */
#define	SEGSTK	2			/* SEGDEF combine type stack */
#define	SEGDEB	6			/* SEGDEF combine type debug */

/*
 * Type representation constants for i80286 TYPDEF records.
 * Some differ from the corresponding i8086 OMF constants.
 */
#if INTEL
/* Intel proprietary. */
#define OTSFUNC	0x63			/* SHORT$NOPOP */
#define OTLFUNC	0x64			/* LONG$NOPOP */
#define OTUPACK	0x69			/* UNPACKED */
#define OTLABEL	0x72			/* LABEL */
#define OTLONG	0x73			/* LONG */
#define OTSHORT	0x74			/* SHORT */
#define OTPROC	0x75			/* PROCEDURE */
#define OTPARA	0x76			/* PARAMETER */
#define OTARRAY	0x78			/* ARRAY */
#define OTSTRUC	0x79			/* STRUCTURE */
#define OTPOINT	0x7A			/* POINTER */
#define OTSCAL	0x7B			/* SCALAR */
#define OTUINT	0x7C			/* UNSINT */
#define OTSINT	0x7D			/* SGNINT */
#define OTREAL	0x7E			/* REAL */
#define OTLIST	0x7F			/* LIST */
#endif
#define OTNIL	0x80			/* NIL */
#define OTNAME	0x82			/* Name leaf */
#define OTINDEX	0x83			/* Index leaf */
#define OTUNUM2	0x85			/* Unsigned numeric 2 byte leaf */
#define OTUNUM4	0x86			/* Unsigned numeric 4 byte leaf */

/*
 * Length and OMF type of scalar C types.
 * The order corresponds to the order of DT_ types in ops.h.
 */
#if INTEL
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
 * The compiler generates a linked list of TINFO records
 * containing OMF type definitions while reading DLABEL records.
 * The type index corresponds to the position in the TINFO list.
 * The tflag keeps track of whether a forward reference
 * tagname type ever gets resolved.
 */
typedef	struct	typeinfo {
	struct	typeinfo *t_tp;		/* link */
	char	t_tflag;		/* tag name flag */
	int	t_length;		/* length */
	char 	t_info[];		/* info */
} TINFO;

/*
 * To generate correct references to tagged types, the compiler
 * enters the tag name as a type and refers to the type by tag name.
 * When the compiler dumps the types, it consults the patch list
 * and substitutes the tag definition for the tag name.
 */
typedef struct	typefix {
	struct	typefix *t_tp;		/* Link */
	int	t_ttype;		/* Dummy definition */
	int	t_type;			/* Fixed definition */
} TYFIX;

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

/*
 * Globals.
 */
static	int	checksum;		/* For putbyte */
static	int	lastline;		/* Last line number */
static	long	lastsize;		/* Last number size */
static	int	txtseg;			/* Current text segment */
static	unsigned char	txt[NTXT];	/* Text buffer */
static	unsigned char	*txtp = txt;	/* Text pointer */
static	int	fixseg;			/* Current fixup segment */
static	unsigned char	fix[NFIX];	/* Fixup buffer */
static	unsigned char	*fixp = fix;	/* Fixup pointer */
static	unsigned char	*ofixp = NULL;	/* Previous fixup pointer for fva kludge */
static	int	symbase;		/* Symbol base, -1 for BP-rel */
static	ADDRESS	seg_size;		/* Size of segment thus far */
static	ADDRESS	bss_size;		/* Size of BSS */
static	ADDRESS	seglen[NSEGS+1];	/* Segment text length */
static	long	secloc[NSECS];		/* Section locations */
static	long	seclen[NSECS];		/* Section lengths */
static	long	oflength;		/* Output file length */
static	int	nsegs;			/* Number of SEGDEFs */
static	int	npubs;			/* Number of PUBDEFs */
static	int	nexts;			/* Number of EXTDEFs */
static	TINFO	*typelistp;		/* Typdef list */
static	TYFIX	*typefixp;		/* Type fix list */
static	SYM	*curfsp;		/* Current function sp */
static	int	curfna;			/* Current function nargs */
static	int	curfar[NARGS];		/* Current function arg types */
static	struct dbgt	*dbase;		/* First debug table entry */
static	struct dbgt	*dnext;		/* Last debug table entry */

/*
 * Object file offsets.
 * Used by fixoutput() to fix information in output file.
 */
static	long	mhdseek;		/* Module header offset */
static	long	tocseek;		/* Table of contents offset */
static	long	segseek[NSEGS+1];	/* SEGDEF attribute byte offsets */
static	long	lenseek;		/* SYMBOLS module length offset */

/*
 * Functions.
 */
extern	struct dbgt	*getdbgt();
extern	TINFO	*gettype();
extern	long	putsegdef();
extern	char	*setindex();
extern	char	*setname();
extern	char	*setnumb();
extern	char	*setsize();
extern	char	*setsymid();

#if VAXFMT
/*
 * The Intel cross software packs the OMF data into 512 byte blocks
 * as 510 byte variable length records, with the last record short.
 */
#define	NVDATA	510			/* Blocksize - 2 */
static	char	vaxbuf[NVDATA];		/* Buffer */
static	char	*vaxptr = vaxbuf;	/* Pointer */
extern	int	filo;
extern	char	*wfn[];
extern	char	ofn[];
extern	int	trace();
#endif

/*
 * Routine 'segindex' uses the following tables
 * to find the OMF segment index corresponding
 * to a given compiler internal segment name.
 * The tables understand the ROM and RAM options.
 * SMALL model always uses ramseg[] (because the compiler assumes
 * SMALL model SPURE, SSTRN, SDATA and SBSS references to be DS-relative).
 */
static	char	nonseg[] = {
	CODESEG,			/* SCODE => CODE */
	CODESEG,			/* SLINK => CODE */
	CODESEG,			/* SPURE => CODE */
	DATASEG,			/* SSTRN => DATA */
	DATASEG,			/* SDATA => DATA */
	DATASEG				/* SBSS  => DATA */
};
static	char	romseg[] = {
	CODESEG,			/* SCODE => CODE */
	CODESEG,			/* SLINK => CODE */
	CODESEG,			/* SPURE => CODE */
	CODESEG,			/* SSTRN => CODE */
	DATASEG,			/* SDATA => DATA */
	DATASEG				/* SDATA => DATA */
};
static	char	ramseg[] = {
	CODESEG,			/* SCODE => CODE */
	CODESEG,			/* SLINK => CODE */
	DATASEG,			/* SPURE => DATA */
	DATASEG,			/* SSTRN => DATA */
	DATASEG,			/* SDATA => DATA */
	DATASEG				/* SDATA => DATA */
};

/*
** First pass routines.
** Output items to the temporary file.
 */

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
 * Initialize the code writer.
 */
outinit()
{
	register TINFO *tp;
#if MONOLITHIC
	/* UNDONE:  initialize all globals here */
	txtp = &txt[0];
	fixp = &fix[0];
#if VAXFMT
	vaxptr = &vaxbuf[0];
#endif
#endif
	/* Initialize type list to contain NIL type as type 1. */
	typelistp = tp = (TINFO *)malloc(sizeof(TINFO)+1);
	typefixp = tp->t_tp = NULL;
	tp->t_tflag = 0;
	tp->t_length = 1;
	tp->t_info[0] = OTNIL;
}

/*
 * Finish up the code.
 * If writing binary code, put out a TEND item
 * to tell 'copycode' when to stop.
 */
outdone()
{
	if (notvariant(VASM))
		bput(TEND);
}

/*
 * Read a dlabel record from ifp.
 * If l(evel) == 0, then only the DLABEL and class bytes have been read.
 * For GDEF's and SEX's in SCODE, save the symbol pointer, accumulate the
 * argument list types, and back patch the type appropriately.
 * For everything else at level zero, translate the compiler type
 * to OMF type and write a truncated record to the scratch file.
 */
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

	/* Write out truncated record */
	iput(type);

	/* Store the type for pubdef and extdef records */
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
#if INTEL
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
			if (i == 0)
				return (index);
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
	/* outinit puts NIL type on the list, so it's never empty. */
	lasttp->t_tp = tp;
	return (index);
#else
	return (1);
#endif
}

/*
 * Read the dimension/type/info list from input and return the type index.
 * Store the size back through the pointer supplied.
 * This routine is reentrant so it can map the types of structure members
 * while mapping the structure definition.
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
#if INTEL
		/* NIL */
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
#if INTEL
		/* SCALAR length type */
		*bp++ = OTSCAL;
		bp = setsize(bp, (long)(ntypes[type][0]));
		*bp++ = ntypes[type][1];
#endif
		break;
	case DT_VOID:
#if INTEL
		*bp++ = OTNIL;			/* undone */
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
#if INTEL
		/* SCALAR length @pointer */
		/* POINTER @type */
		*bp++ = OTPOINT;
#endif
		type = dbtmap(ttype);
#if INTEL
		bp = setindex(bp, type);
		type = tlookup(bb, bp-bb);
		bp = bb;
		*bp++ = OTSCAL;
		bp = setsize(bp, isvariant(VLARGE) ? 0x20L : 0x10L);
		bp = setindex(bp, type);
#endif
		break;
	case DD_FUNC:
#if INTEL
		/* PROCEDURE NIL @type calltype count @plist */
		/* arglisttype will fix the count and @plist later. */
		*bp++ = OTPROC;
		*bp++ = OTNIL;
#endif
		type = dbtmap(ttype);
#if INTEL
		bp = setindex(bp, type);
		*bp++ = (isvariant(VLARGE)) ? OTLFUNC : OTSFUNC;
		*bp++ = 0;	/* Zero args, possibly corrected later */
#endif
		break;
	case DD_ARRAY:
		type = dbtmap(ttype);
#if INTEL
		/* ARRAY length @type */
		*bp++ = OTARRAY;
		bp = setsize(bp, ((long)size)*NBPBYTE);
		bp = setindex(bp, type);
#endif
		break;
	default:
		dbfatal(__LINE__);
	}
	return (tlookup(bb, bp-bb));
}

/*
 * Construct typdef for enum, struct, or union.
 * DX_NAME records supply tag name of type which will be defined later.
 * Tag references point to type consisting of the tag name;
 * the type will be backpatched into a struct/union/enum typdef.
 */
dbxname(ttype)
{
	register char	*bp;
	register TINFO	*tp;
	register int i;
	iget();		/* Dummy size */
	bp = &id[0];
	while (*bp++ = bget())
		;
	i = tlookup(id, bp-id);
	tp = gettype(i);
	tp->t_tflag = 1;
	return(i);
}

dbxstruct(s, ttype)
unsigned	s;		/* Size of structure in bytes */
{
	int	nmemb;	/* Number of members */
	int	i;	/* Member counter */
	unsigned	bwid;	/* Bit width of current field */
	unsigned	boff;	/* Bit offset of current field */
	unsigned	valu;	/* Byte offset of current member */
	int	type;	/* Type index of current member */
#if INTEL
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
#if INTEL
	/* STRUCT length count @tlist @nlist name */
	/* tLIST @type1 ... @typen */
	/* nLIST name1 ... namen */
	sp1 = NULL;
	offs = loffs = 0;
	nname = 0;
#endif
	nmemb = iget();		/* Number of members */
				/* Does not include filler fields */
	/* Read in member descriptions */
	for (i = nmemb; i > 0; i--) {
		if (bget() != DC_MOS)
			dbfatal(__LINE__);
		iget();		/* forget line */
		bwid = bget();	/* bit width */
		boff = bget();	/* bit offset */
		valu = iget();	/* byte offset */
		sget(id, NCSYMB);
#if INTEL
		/* Enter into list, possibly prefixing nil filler field */
		offs = (long)valu * (long)NBPBYTE + boff;
		if (offs != loffs) {
			/* Add filler field with name "" so PSCOPE */
			/* does not lose its place in the structure. */
			nmemb++;	/* Add a member */
			nname += 2;	/* Bytes required for nlist */
			sp2 = (struct slist *)malloc(sizeof(*sp2)+1);
			if (sp2 == NULL) dbmemory(__LINE__);
			sp2->sl_type = dbfmap((int)(offs-loffs), DT_UINT);
			sp2->sl_id[0] = 0;
			sp2->sl_sp = sp1;
			sp1 = sp2;
		}
		size = strlen(id)+2;	/* OTNAME+count+length */
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
#if INTEL
		sp2->sl_type = type;
		sp2->sl_sp = sp1;
		sp1 = sp2;
		loffs = offs + lastsize;
#endif
	}
#if INTEL
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
	lp = bp = malloc(nmemb*3+1);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTLIST;
	sp2 = sp1;
	for (i = 0; i < nmemb; i++) {
		bp = setindex(bp, sp2->sl_type);
		sp2 = sp2->sl_sp;
	}
	tlist = tlookup(lp, bp-lp);
	free(lp);
	/* Construct name list */
	lp = bp = malloc(nname+1);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTLIST;
	sp2 = sp1;
	for (i = 0; i < nmemb; i++) {
		if (sp2==NULL) dbfatal(__LINE__);
		bp = setname(bp, sp2->sl_id);
		sp2 = sp2->sl_sp;
		free(sp1);
		sp1 = sp2;
	}
	nlist = tlookup(lp, bp-lp);
	free(lp);
	/* Construct the true type record */
	bp = &bb[0];
	*bp++ = OTSTRUC;			/* STRUCT */
	bp = setsize(bp, ((long)s)*NBPBYTE);	/* length */
	bp = setnumb(bp, (long)nmemb);		/* count */
	bp = setindex(bp, tlist);		/* @tlist */
	bp = setindex(bp, nlist);		/* @nlist */
	bp = setname(bp, (ttype)?gettype(ttype)->t_info:"");	/* name */
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
#if INTEL
	/* SCALAR length type */
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
unsigned	s;			/* Size of union in bytes */
{
	int	nmemb;
	int	i;
#if INTEL
	char	*lp;
	register char *bp;
	int namelen;
#endif
	/* Union member list */
	nmemb = iget();			/* Number of members in union */
#if INTEL
	/* STRUCT length 0 NIL NIL name UNPACKED count NIL NIL
		1 @tlist1 @nlist1 ... 1 @tlistn @nlistn */
	namelen = (ttype) ? strlen(gettype(ttype)->t_info) : 0;
	lp = bp = malloc(17+7*nmemb+namelen);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTSTRUC;		/* STRUCT */
	bp = setsize(bp, ((long)s)*NBPBYTE);	/* length */
	*bp++ = 0;			/* Invariant records */
	*bp++ = OTNIL;			/* Invariant type list */
	*bp++ = OTNIL;			/* Invariant name list */
	bp = setname(bp, (ttype)?gettype(ttype)->t_info:"");	/* name */
	*bp++ = OTUPACK;		/* UNPACKED */
	bp = setnumb(bp, (long)nmemb);	/* count */
	*bp++ = OTNIL;			/* Selector field type */
	*bp++ = OTNIL;			/* Selector field name */
#endif
	i = 1;
	while (--nmemb >= 0) {
		int	type;
#if INTEL
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
#if INTEL
		/* Make name list item */
		tbp = bb;
		*tbp++ = OTLIST;
		tbp = setname(tbp, id);
		nlist = tlookup(bb, tbp-bb);
#endif
		type = dbtmap(0);
#if INTEL
		/* Make type list item */
		tbp = bb;
		*tbp++ = OTLIST;
		tbp = setindex(tbp, type);
		tlist = tlookup(bb, tbp-bb);
		/* Append member items to typdef */
		*bp++ = 1;
		bp = setindex(bp, tlist);
		bp = setindex(bp, nlist);
#endif
	}
#if INTEL
	lastsize = ((long)s) * NBPBYTE;
	i = tlookup(lp, bp-lp);
	free(lp);
	return (i);
#else
	return (1);
#endif
}

dbxenum(s, ttype)
unsigned	s;			/* Size of enum type in bytes */
{
	int	nmemb;
	int	i;
	int	valu;
	int	nname;
#if INTEL
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
#if INTEL
	/* SCALAR size type @list lbound ubound */
	/* LIST name1 ... namen */
	epp = (struct el **)malloc(nmemb * sizeof(*epp));
	if (epp == NULL) dbmemory(__LINE__);
	maxvalu = -MAXIV-1;
	minvalu = MAXIV;
#endif
	for (i = 0; i < nmemb; i++) {
#if INTEL
		int size;
		register struct el *ep;
#endif


		if (bget() != DC_MOE) dbfatal(__LINE__);
		iget();		/* forget line */
		valu = iget();	/* value */
		sget(id, NCSYMB);
#if INTEL
		size = strlen(id)+2;
		nname += size;
#endif
		dbtmap(0);
#if INTEL
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
#if INTEL
	/* Construct name list */
	range = maxvalu - minvalu + 1;
	if (range > nmemb)
		nname += (range-nmemb)*2;
	nname += range/8 + 3;
	lp = bp = malloc(nname);
	if (bp == NULL) dbmemory(__LINE__);
	*bp++ = OTLIST;
	for (valu = minvalu; valu <= maxvalu; valu++) {
		*bp++ = OTNAME;
		for (i = 0; i < nmemb; i++)
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
	bp = setname(bp, (ttype)?gettype(ttype)->t_info:"");
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
 * The function's type with zero args has already been computed.
 * Expand the type and append nargs and arglist type.
 */
arglisttype()
{
#if INTEL
	/* LIST @param1 ... @paramn */
	/* PARAM @type1 */
	/* ... */
	/* PARAM @typen */
	int type;
	int oldtypesize;
	char *oldtype;
	register char *bp;
	register int i;
	register char *sbp;
	char b[3*NARGS+3];
	register TINFO *tp;
	char b2[16];

	if (curfna == 0)
		return;
	/* Compute argument list type */
	bp = &b[0];
	*bp++ = OTLIST;
	for (i=0; i<curfna; ++i) {
		*bp++ = OTINDEX;
		sbp = bp;
		*bp++ = OTPARA;
		bp = setindex(bp, curfar[i]);
		bp = setindex(sbp-1, tlookup(sbp, bp-sbp));
	}
	type = tlookup(b, bp-b);

	/* Find old type */
	tp = gettype(curfsp->s_type);
	oldtype = tp->t_info;
	oldtypesize = tp->t_length;

	/* Expand and compute new type */
	bp = b2;
	while (--oldtypesize > 0)	/* Last byte should be zero */
		*bp++ = *oldtype++;
	bp = setnumb(bp, (long)curfna);	/* Number of args */
	bp = setindex(bp, type);	/* Arg types */
	curfsp->s_type = tlookup(b2, bp-b2);
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
 * Finish up.
 * Figure out the sizes and final values of everything.
 * Write the output file.
 */
copycode()
{
	register SYM *sp;
	register i;

	/*
	 * Assign base addresses to the compiler's logical segments
	 * based on the settings of the VROM and VRAM flags.
	 */
	seg_size = 0;
	bss_size = seg[SBSS].s_dot;
	lseg(SCODE);
	lseg(SLINK);
	if (isvariant(VSMALL) || isvariant(VRAM))
		seg_size = 0;
	lseg(SPURE);
	if (notvariant(VSMALL) && notvariant(VROM) && notvariant(VRAM))
		seg_size = 0;
	lseg(SSTRN);
	if (notvariant(VSMALL) && isvariant(VROM))
		seg_size = 0;
	lseg(SDATA);
	lseg(SBSS);

	/*
	 * Adjust symbol base addresses to make them LSEG relative
	 * instead of compiler segment relative.
	 */
	for (i=0; i<NSHASH; ++i) {
		sp = hash2[i];
		while (sp != NULL) {
			if ((sp->s_flag&S_DEF) != 0)
				sp->s_value += seg[sp->s_seg].s_mseek;
			sp = sp->s_fp;
		}
	}

	/*
	 * Write the object file.
	 * Information in header and SEGDEF section is fixed ex post facto.
	 */
	putbyte(0xA0);			/* file type = linkable module */
	checksum = 0;			/* file type byte not in checksum */
	writeheader();			/* file header */
	writesegdef();			/* SEGDEF section */
	writetypdef();			/* TYPDEF section */
	writesymdef(PUBSEC);		/* PUBDEF section */
	writesymdef(EXTSEC);		/* EXTDEF section */
	writetxtfix();			/* TXTFIX section */
	fixoutput();			/* fix output file */
	putbyte(-checksum);		/* and write checksum */
#if VAXFMT
	vaxflush();
#endif
}

/*
 * Write linkable module header and table of contents.
 * Some of this gets fixed up after writing the object.
 */
writeheader()
{
	register int n;
	mhdseek = oflength;		/* save header location */
	putrept (0, 13);		/* 13 bytes of 0, fixed later */
	putdate();			/* date and time */
	n = strlen(module);
	putbyte(n);
	putupper(module, n);		/* module name */
	putrept(' ', 40 - n);		/* padded with spaces */
	putcreator();			/* module creator */
	tocseek = oflength;		/* save toc location */
	putrept(0, 64);			/* table of contents, fixed later */
}

/*
 * Write SEGDEF section.
 */
writesegdef()
{
	char b[NMNAME+5];
	secloc[SEGSEC] = oflength;
	if (isvariant(VSMALL)) {
		segseek[CODESEG] = putsegdef(0, "CODE");
		segseek[DATASEG] = putsegdef(0, "DATA");
		putsegdef(SEGDPL|SEGARW|SEGSTK|SEGEXT, "DATA");
	}
	else {
		strcpy(b, module);
		strcat(b, "_CODE");
		segseek[CODESEG] = putsegdef(0, b);
		strcpy(b, module);
		strcat(b, "_DATA");
		segseek[DATASEG] = putsegdef(0, b);
		putsegdef(SEGDPL|SEGARW|SEGSTK|SEGEXT, "STACK");
	}
	if (isvariant(VDEBUG)) {
		segseek[MODSEG] = putsegdef(0, "MODULES:");
		segseek[SYMSEG] = putsegdef(0, "SYMBOLS:");
		segseek[LINESEG] = putsegdef(0, "LINES:");
	}
	endsec(SEGSEC);
}

/*
 * Write TYPDEF section.
 */
writetypdef()
{
	register TINFO *tp;
	register int length;
	register TINFO *ttp;
	secloc[TYPSEC] = oflength;
	fixpatch();
	tp = typelistp;
	while (tp != NULL) {
		putbyte(0xFF);		/* linkage = TRUE */
		if (tp->t_tflag) {
			length = 1;
			tp->t_info[0] = OTNIL;
		}
		else
			length = tp->t_length;
		putword(length);	/* length */
		putblock(tp->t_info, length);	/* type string */
		ttp = tp;
		tp = tp->t_tp;
		free(ttp);
	}
	endsec(TYPSEC);
}

/*
 * Write PUBDEF or EXTDEF section.
 */
writesymdef(n)
int n;
{
	register int isdefined;
	register SYM *sp;
	int flag, i, type, ispubsec;
	secloc[n] = oflength;
	ispubsec = (n == PUBSEC);
	for (i=0; i<NSHASH; i++) {
		sp = hash2[i];
		while (sp != NULL) {
			flag = sp->s_flag;
			isdefined = ((flag&S_DEF)!=0);
			if ((flag&S_GBL)!=0 || !isdefined) {
				type = sp->s_type;
				if (type == 0)
					++type;
				if (isdefined && ispubsec) {
					/* Public symbol. */
					putword(sp->s_value);
					putword(segindex(sp->s_seg));
					putword(type);
					putbyte(0);	/* wordcount */
					putname(sp->s_id);
					sp->s_ref = 0;
					++npubs;
				}
				else if (!isdefined && !ispubsec) {
					/* External symbol. */
					putword(0);	/* segment unknown */
					putword(type);
					putbyte(0);
					putname(sp->s_id);
					sp->s_ref = ++nexts;
				}
			}
			sp = sp->s_fp;
		}
	}
	endsec(n);
}

/*
 * Write TXTFIX section.
 */
writetxtfix()
{
	register int op, data, nd, i;
	register char *cp;
	register SYM *sp;

	secloc[TXTSEC] = oflength;
	if (isvariant(VDEBUG)) {
		need(SYMSEG, 5+strlen(module)+1);
		lenseek = oflength+10;		/* length fixed later */
		*txtp++ = SYMBBEG;		/* block begin */
		fixsym(FORMOFF, CODESEG, 0);	/* offset fixup */
		text2(0);			/* offset */
		text2(0);			/* length */
		txtp = setsymid(txtp, module);	/* name */
	}
	dotseg = SCODE;
	dot = seg[SCODE].s_dot;
	while ((op=bget()) != TEND) {
		switch(op) {
		case TENTER:
			flush();
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
			need(segindex(dotseg), 2);
			text2(0);
			fixdot(FORMSEL, SEGIN | ((op == TCODE) ? CODESEG : DATASEG), 0);
			dot += 2;
			continue;
#endif
		case TBASE:
			if (isvariant(VSMALL))
				cbotch("base");
			sp = pget();
			need(segindex(dotseg), 2);
			text2(0);
			fixdot(FORMSEL, ((sp->s_flag&S_DEF)!=0) ? segindex(sp->s_seg) : EXTIN|(sp->s_ref), 0);
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
		need(segindex(dotseg), nd);
		/*
		 * Absolute.
		 */
		if ((op&(TSYM|TPCR)) == 0) {
			text(data, nd);
			dot += nd;
			continue;
		}
		/*
		 * Absolute PC rel.
		 */
		if ((op&TSYM) == 0) {
			text(dot+data, nd);
			fixdot(FORMOFF, txtseg, dot+data);
			dot += nd;
			continue;
		}
		/*
		 * Symbol based.
		 */
		sp = pget();
		if ((sp->s_flag&S_DEF) != 0) {
			data += sp->s_value;
			/* Absolute */
			if ((op&TPCR)!=0 && sp->s_seg==dotseg) {
				text(data-(dot+nd), nd);
				dot += nd;
				continue;
			}
			/* Segment relative */
			text(data, nd);
			fixdot((op&TPCR)?FORMSRO:FORMOFF, segindex(sp->s_seg), data);
			dot += nd;
			continue;
		}
		/*
		 * Symbol relative.
		 */
		text(data, nd);
		fixdot((op&TPCR)?FORMSRO:FORMOFF, EXTIN|(sp->s_ref), data);
		dot += nd;
	}

	/*
	 * Write SYMBOLS: and MODULES: text and fixups if DEBUG.
	 */
	if (isvariant(VDEBUG)) {
		dbdump();			/* dump debug symbols */
		putbend();			/* block end */
		need(MODSEG, 33+1+strlen(module));
		for (i=1; i<=26; i++)
			*txtp++ = 0;
		text2(1);			/* first line number = 1 */
		*txtp++ = TID;			/* translator ID */
#if INTEL
		cp = VERSINT;
#else
		cp = VERSMWC;
#endif
		for (i=1; i<=4; i++)
			*txtp++ = *cp++;	/* version */
		txtp = setsymid(txtp, module);
		fixup(FORMFVA, MODSEG, 0x02, CODESEG|SEGIN, 0);
		fixup(FORMFVA, MODSEG, 0x0A, SYMSEG|SEGIN, 0);
		fixup(FORMFVA, MODSEG, 0x0E, LINESEG|SEGIN, 0);
	}
	flush();

	/*
	 * Write an ITEXT block to initialize BSS if nonempty.
	 */
	if (bss_size != 0) {
		putbyte(BITEXT);		/* iterated text block */
		putword(seg[SBSS].s_mseek);	/* offset */
		putword(segindex(SDATA));	/* segment */
		putword(bss_size);		/* iteration count */
		putword(1);			/* template length */
		putbyte(0);			/* template */
	}
	endsec(TXTSEC);
}

/*
 * Fix the output file.
 */
fixoutput()
{
	register int i;
	long len;

#ifdef	vax
	/* On the VAX, copy the temp file to the output file. */
	/* This is because of problems with seeking in the output file. */
#if VAXFMT
	vaxflush();
#endif
	setin();
	outopen(1);
	trace("repacking");
#endif

	/* Fix the module header. */
	len = oflength;			/* save module length */
	fakeseek(mhdseek, 12);		/* seek to module header */
	putlong(len);			/* fix total module length */
	putword(nsegs);			/* fix SEGDEFs */
	putword(0);			/* GATDEFs */ 
	putword(npubs);			/* fix PUBDEFs */
	putword(nexts);			/* fix EXTDEFs */
	/* Fix the table of contents. */
	fakeseek(tocseek, 8*NSECS);	/* seek to table of contents */
	for (i=0; i<NSECS; i++) {
		if (secloc[i] != 0L)
			--secloc[i];	/* location without file type byte */
		putlong(secloc[i]);	/* fix location */
		putlong(seclen[i]);	/* fix length */
	}
	/* Fix the SEGDEF section. */
	seglen[DATASEG] += bss_size;	/* DATA includes text and BSS */
	fixsegdef(SEGDPL|SEGAXR|SEGCOM, CODESEG);
	fixsegdef(SEGDPL|SEGARW|SEGCOM, DATASEG);
	if (isvariant(VDEBUG)) {
		fixsegdef(SEGDPL|SEGARO|SEGDEB, MODSEG);
		fixsegdef(SEGDPL|SEGARO|SEGDEB, SYMSEG);
		fixsegdef(SEGDPL|SEGARO|SEGDEB, LINESEG);
		/* Fix the SYMSEG text. */
		fakeseek(lenseek, 2);
		putword(seglen[CODESEG]);
	}
	/* Seek to end. */
	fakeseek(len, 0);
}

/*
 * Seek to 'where' and disregard next 'ignore' bytes.
 * Should just be lseek(), but complicated for VAX.
 */
fakeseek(where, ignore)
long where;
int ignore;
{
#ifdef vax
	register int i;
	static long pos = 0L;

	for (; pos < where; pos++)
		putbyte(bget());	/* copy bytes */
	for (i=0; i<ignore; i++)
		bget();			/* skip old bytes */
	pos += (long) ignore;		/* and update position */
	if (where == mhdseek)
		checksum = 0;		/* reset checksum */
#else
	if (fseek(ofp, where, 0) != 0)	/* seek to location */
		cfatal("seek error");
#endif
}

/*
 * Fix the attribute and length words of a segment definition.
 */
fixsegdef(attrib, seginx)
int attrib, seginx;
{
	register ADDRESS size;

	size = seglen[seginx];
	if (size == 0)
		attrib |= SEGEXT;	/* external if empty */
	else
		--size;			/* length-1 gives length */
	fakeseek(segseek[seginx], 4);	/* seek to attribute byte */	
	putword(attrib);		/* attributes */
	putword(size);			/* length */
}

/*
 * Write LINES text for line number.
 * 'lastline' is the last line for which LINES text was written.
 * Watch for the case where 'line' is much larger than 'lastline'.
 */
putline(dp)
struct dbgt *dp;
{
	register int i, value, line;
	line = dp->d_line;
	value = dp->d_value;
	while (line > lastline) {		/* need to write LINES text */
		i = 2 * (line - lastline);	/* bytes of LINES text needed */
		if (i > NTXT)
			i = NTXT;		/* cannot buffer more than NTXT */
		need(LINESEG, i);
		while (i > 0) {
			text2(value);
			i -= 2;
			lastline++;
		}
	}
}

/*
 * Write SYMBOLS text for a block or procedure begin.
 */
putbbeg(dp)
register struct dbgt *dp;
{
	if (dp->d_id[0] != '{') {		/* procedure block */
		need(SYMSEG, 10+1+strlen(dp->d_id));
		*txtp++ = SYMPBEG;		/* procedure begin */
		fixsym(FORMOFF, CODESEG, dp->d_value);	/* offset fixup */
		text2(dp->d_value);		/* offset */
		text2(dp->d_type);		/* type */
		*txtp++ = (isvariant(VSMALL)?0:1);	/* near or far */
		text2(6);			/* r. a. offset */
		text2(blklength(dp,1));		/* length */
		txtp = setsymid(txtp, dp->d_id);	/* name */
	}
	else {					/* non-procedure block */
#ifdef NOABLOCKS
		blkcount++;
#else
		need(SYMSEG, 6);
		*txtp++ = SYMBBEG;		/* block begin */
		fixsym(FORMOFF, CODESEG, dp->d_value);	/* offset fixup */
		text2(dp->d_value);		/* offset */
		text2(blklength(dp, 0));	/* length */
		*txtp++ = 0;			/* name */
#endif
	}
}

/*
 * Return the blocklength of block dp.
 * Add the epilog size if epiflag.
 */
blklength(dp, epiflag)
register struct dbgt *dp;
int epiflag;
{
	register int level;
	register ADDRESS length;
	register int episize;

	/* Set episize to epilog length (8086: without autos) if epiflag. */
	episize = (epiflag) ? 4 : 0;
	level = dp->d_level + 1;
	length = dp->d_value;
	while ((dp = dp->d_dp) != NULL) {
		if (epiflag && dp->d_class == DC_AUTO && notvariant(V80186))
			episize = 6;	/* epilog length with autos, 8086 */
		else if (dp->d_class == DC_LINE
		 && dp->d_id[0] == '}'
		 && dp->d_level == level)
			return ((int) (dp->d_value - length + episize));
	}
	cbotch("missing blkend");
}

/*
 * Write SYMBOLS text for block end.
 */
putbend()
{
#ifdef NOABLOCKS
	if (blkcount > 0) {
		blkcount--;
		return;
	}
#endif
	need(SYMSEG, 1);
	*txtp++ = SYMBEND;
	if (symbase == -1)
		symbase = 0;
}

/*
 * Write SYMBOLS text for debug symbol.
 */
putsym(dp)
register struct dbgt *dp;
{
	register char *cp;
	register int n;
	int class = dp->d_class;
	if ((class == DC_AUTO) || (class == DC_PAUTO)) {
		/* BP-relative symbol */
		if (symbase != -1) {
			need(SYMSEG, 1);
			*txtp++ = SYMBPRB;	/* BP-relative base */
			symbase = -1;
		}
	}
	else if (segindex(dp->d_seg) != symbase) {
		/* Need to change symbol segments */
		symbase = segindex(dp->d_seg);
		need(SYMSEG, 5);
		*txtp++ = SYMBASE;		/* base */
		fixsym(FORMFVA, symbase, 0);	/* full virt addr fixup */
		text2(0);			/* offset */
		text2(symbase);			/* segment */
	}
	cp = dp->d_id;
	n = strlen(cp);
	need(SYMSEG, 5+1+n);
	*txtp++ = SYMSYMB;
	text2(dp->d_value);			/* offset */
	text2(dp->d_type);			/* type */
	txtp = setsymid(txtp, cp);		/* name */
}

/*
 * Put out a byte, updating checksum.
 */
putbyte(b)
register b;
{
	++oflength;
	checksum += b;
#if	VAXFMT
#if	TEMPBUF
	if (ofp == NULL) {
		bput(b);
		return;
	}
#endif
	if (vaxptr >= &vaxbuf[NVDATA])
		vaxflush();
	*vaxptr++ = b;
#else
	bput(b);
#endif
}

/*
 * Put out a word.
 */
putword(n)
register int n;
{
	putbyte(n);
	putbyte(n>>8);
}

/*
 * Put out a long.
 */
putlong(n)
long n;
{
	putword((int)n);
	putword((int)(n>>16));
}

/*
 * Put out a block of n bytes from cp.
 */
putblock(cp, n)
register unsigned char *cp;
register int n;
{
	while (n--)
		putbyte(*cp++);
}

/*
 * Put out a block of n bytes from cp,
 * mapping lower case to upper case.
 */
putupper(cp, n)
register char *cp;
register int n;
{
	while (n--)
		putbyte(uppercase(*cp++));
}

/*
 * Put out n bytes of character c.
 */
putrept(c, n)
register int c;
register int n;
{
	while (n--)
		putbyte(c);
}

/*
 * Put out a PL/M-format name.
 * Warn and truncate if the name is longer than 39 characters.
 */
putname(cp)
register char *cp;
{
	register int len;
	if ((len = strlen(cp)) > 39) {
		cwarn("symbol \"%s\" truncated to 39 characters", cp);
		len = 39;
	}
	putbyte(len);
	putupper(cp, len);
}

/*
 * Put out date and time.
 */
putdate()
{
#ifdef	UDI
	extern struct gt udigtime;
	putblock(udigtime.gt_date, 8);
	putblock(udigtime.gt_time, 8);
#else					/* COHERENT or VAX */
#ifdef	COHERENT
	time_t now;
#else					/* VAX */
	struct tm *localtime();
	long time();
	long now;
#endif
	char b[17];
	register struct tm *tmp;
	time(&now);
	tmp = localtime(&now);
	sprintf (b, "%02d/%02d/%02d%02d:%02d:%02d",
		tmp->tm_mon+1,
		tmp->tm_mday,
		tmp->tm_year,
		tmp->tm_hour,
		tmp->tm_min,
		tmp->tm_sec);
	putblock(b, 16);
#endif
}

/*
 * Put out module creator.
 */
putcreator()
{
	register int n;
	char b[40];
#ifdef	UDI
	extern char udisysid[];
	cstr(b, &udisysid[0]);
#endif
#ifdef	COHERENT
	strcpy(b, "COHERENT");
#endif
#ifdef	vax
	strcpy(b, "VAX");
#endif
#if INTEL
	strcat (b, " iC286 ");
	strcat (b, VERSINT);
#else
	strcat (b, " MWC286 ");
	strcat (b, VERSMWC);
#endif
	n = strlen(b);
	putbyte(n);
	putblock(b, n);
	putrept(' ', 40 - n);
}

/*
 * Put out a segment definition.
 * The attribute and length may be fixed later.
 * Return the location of the attribute byte.
 */
long
putsegdef(attrib, namep)
int attrib;
char *namep;
{
	long l;

	l = oflength;
	++nsegs;
	putword(attrib);		/* attributes, may be fixed later */
	putrept(0, 6);			/* length, dlength, LDT position */
	putname(namep);			/* combine name */
	return(l);
}

/*
 * End a section.
 * Compute section length;
 * reset section location to 0 if empty.
 */
endsec(n)
register int n;
{
	seclen[n] = oflength - secloc[n];
	if (seclen[n] == 0L)
		secloc[n] = 0L;
}

/*
 * Return OMF segment index internal name corresponding to
 * compiler logical segment code.
 */
segindex(n)
register int n;
{
	return(SEGIN |	((isvariant(VSMALL)||isvariant(VRAM))?ramseg[n]
			: ((isvariant(VROM))?romseg[n]
			: nonseg[n])));
}

/*
 * Convert character to upper case.
 */
uppercase(c)
register c;
{
	if (c < 'a' || c > 'z')
		return(c);
	return(c - 'a' + 'A');
}

/*
 * Assign a base address to a compiler logical segment.
 * Bump seg_size and reset segment location counter to base.
 * Round sizes up to even boundaries because of 80287 word alignment bug.
 */
lseg(n)
register int n;
{
	seg[n].s_mseek = seg_size;
	if (((seg[n].s_dot)&1) == 1) {
		++seg[n].s_dot;		/* round dot up to even boundary */
		++seglen[segindex(n)&~SEGIN];	/* and bump segment length */
	}
	seg_size += seg[n].s_dot;
	seg[n].s_dot = seg[n].s_mseek;
}

/*
 * Flush the text and fixup buffers.
 */
flush()
{
	register int n, sindex;
	if (txtp != txt) {
		n = txtp - txt;
		sindex = txtseg&~SEGIN;		/* mask off segment IN */
		putbyte(BTEXT);			/* text block */
		if (sindex == CODESEG || sindex == DATASEG) {
			putword(seg[dotseg].s_dot);
			seg[dotseg].s_dot += n;
		}
		else putword(seglen[sindex]);	/* offset */
		putword(txtseg);		/* segment IN */
		seglen[sindex] += n;		/* bump seg length */
		putword(n);			/* length */
		txtp = txt;
		while (n--)
			putbyte(*txtp++);	/* text */
		txtp = txt;			/* reset pointer */
	}
	if (fixp != fix) {
		putbyte(BFIXUP);		/* fixup block */
		putword(fixseg);		/* where-IN */
		n = fixp - fix;
		putword(n);			/* length */
		fixp = fix;
		while (n--)
			putbyte(*fixp++);	/* fixup */
		fixp = fix;			/* reset pointer */
		ofixp = NULL;			/* and reset ofixp */
	}
}

/*
 * Leave n bytes of space for text in given seg.
 * Flush if buffer is too full or contains text from a different segment.
 */
need(seg, n)
register int seg, n;
{
	if (n == 0)
		return;
	if (n > NTXT)
		cbotch("need");
	seg |= SEGIN;
	if ((txtp+n > &txt[NTXT]) || ((seg != txtseg) && (txtp != txt)))
		flush();
	txtseg = seg;
}

/*
 * Write n (1 or 2) bytes of data as text.
 */
text(data, n)
register int data, n;
{
	*txtp++ = data;
	if (n == 2)
		*txtp++ = data >> 8;
}

/*
 * Write 2 bytes of data as text.
 */
text2(data)
register int data;
{
	*txtp++ = data;
	*txtp++ = data >> 8;
}

/*
 * Write a fixup.
 * Flush if buffer is too full or contains fixups on a different segment.
 * An offset fixup followed by a selector fixup can get kludged into
 * a full virtual address fixup.
 */
fixup(format, wherein, whereoffs, whatin, whatoffs)
int format;
ADDRESS wherein, whereoffs, whatin, whatoffs;
{
	register int type;
	static ADDRESS owhereoffs, owhatin;
	wherein |= SEGIN;
	/* Flush if necessary. */
	if ((fixp+7 > &fix[NFIX]) || ((wherein != fixseg) && (fixp != fix)))
		flush();
	fixseg = wherein;
	/* Patch offset/selector fixup pairs to full virtual address fixups. */
	if (format == FORMOFF) {
		ofixp = fixp;
		owhereoffs = whereoffs;
		owhatin = whatin;
	}
	else if ((ofixp != NULL) && (format == FORMSEL) && (whatin == owhatin)
		&& (whereoffs == owhereoffs + 2)) {
		*ofixp |= FORMFVA;
		ofixp = NULL;
		return;
	}
	else ofixp == NULL;
	/* Determine the fixup type. */
	if (whatin == wherein)
		type = FIXISEG;		/* intrasegment */
	else if (whatoffs == 0)
		type = FIXCALL;		/* call */
	else type = FIXGEN;		/* general */
	/* Write the fixup. */
	*fixp++ = format | type;	/* kind */
	*fixp++ = whereoffs;
	*fixp++ = whereoffs >> 8;	/* where-offset */
	if (type != FIXCALL) {
		*fixp++ = whatoffs;
		*fixp++ = whatoffs >> 8;	/* what-offset */
	}
	if (type != FIXISEG) {
		*fixp++ = whatin;
		*fixp++ = whatin >> 8;	/* what-IN */
	}
}

/*
 * Write a symbol fixup at the current text location in SYMSEG.
 */
fixsym(format, whatin, whatoffs)
int format;
ADDRESS whatin, whatoffs;
{
	fixup(format, SYMSEG, seglen[SYMSEG]+txtp-txt, SEGIN|whatin, whatoffs);
}

/*
 * Perform fixup at current location.
 */
fixdot(format, whatin, whatoffs)
int format;
ADDRESS whatin, whatoffs;
{
	fixup(format, segindex(dotseg), dot, whatin, whatoffs);
}

#if VAXFMT
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

#if	TEMPBUF
	if (ofp == NULL)
		return;
#endif
	if ((n = vaxptr-vaxbuf) != 0) {
		if (fwrite(vaxbuf, n, 1, ofp) != 1
		||  fflush(ofp) == EOF)
			cfatal("output write error");
		vaxptr = vaxbuf;
	}
}
#endif

/*
** Debug information processors.
*/

fixpatch()
{
	register TYFIX *fp;
#if INTEL
	register TINFO *ttp, *pttp, *tp, *ptp;
#endif

	while ((fp = typefixp) != NULL) {
#if INTEL
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
		ttp->t_tflag = 0;
		ttp->t_length = 1;
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
				putline(dp);
				putbbeg(dp);
			} else
				putsym(dp);
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
			putsym(dp);
			break;
		case DC_SIN:		/* Static internal */
		case DC_LAB:		/* Label */
			putsym(dp);
			break;
		case DC_REG:		/* Register */
		case DC_PREG:		/* Parametric register */
#if INTEL
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
					putline(dp);
					putbbeg(dp);
				}
				break;
			case ';':	/* Line number */
				putline(dp);
				break;
			case '}':	/* Block ends */
			 	putbend();
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

#if !TINY
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
 * Splat an index leaf.
 */
char *
setindex(cp, i)
register char *cp;
register unsigned i;
{
	*cp++ = OTINDEX;
	*cp++ = i;
	*cp++ = (i >> 8);
	return (cp);
}

/*
 * Splat a name leaf.
 */
char *
setname(bp, cp)
register char *bp;
register char *cp;
{
	*bp++ = OTNAME;
	return(setsymid(bp, cp));
}

/*
 * Splat an unsigned numeric leaf.
 * This assumes that the leaf always fits in 4 bytes,
 * since any object the compiler knows about must fit in
 * 64K bytes or 64K * NBPBYTE = 512K bits.
 */
char *
setnumb(bp, numb)
char *bp;
unsigned long numb;
{
	if (numb < 128)
		*bp++ = numb;
	else if (numb < 0x10000L) {
		*bp++ = OTUNUM2;
		*bp++ = numb;
		*bp++ = numb >> 8;
	}
	else {
		*bp++ = OTUNUM4;
		*bp++ = numb;
		*bp++ = numb >> 8;
		*bp++ = numb >> 16;
		*bp++ = numb >> 24;
	}
	return (bp);
}

/*
 * Splat a bit size.
 */
char *
setsize(bp, size)
char *bp;
long size;
{
	lastsize = size;
	return (setnumb(bp, size));
}

/*
 * Copy name through bp and return updated bp.
 */
char *
setsymid(bp, cp)
register char *bp;
register char *cp;
{
	register len;
	if ((len=strlen(cp)) > 39) {
		cwarn("symbol \"%s\" truncated to 39 characters", cp);
		len = 39;
	}
	*bp++ = len;
	while (len--)
		*bp++ = uppercase(*cp++);
	return (bp);
}

dbfatal(lineno)
{
	cbotch("debug label format in %d", lineno);
}

dbmemory(lineno)
{
	cnomem("dbmemory %d", lineno);
}
