#include	<stdio.h>

#ifdef vax
#include	<ssdef.h>
#include	<stsdef.h>
#define	NNAME	500
#define	NSEG	500
#define	NBREC	10000
#define	SRMODE	"r"
#define	OK	(STS$M_INHIB_MSG|SS$_NORMAL)
#define	BAD	(STS$M_INHIB_MSG|SS$_ABORT)
#else
#define	NNAME	250
#define	NSEG	250
#define	NBREC	5000
#define	SRMODE	"rb"
#define	OK	0
#define	BAD	1
#endif

#define	LNAMES	0x96
#define	SEGDEF	0x98
#define	MODEND	0x8A

typedef	struct	SEG	{
	int	s_index;		/* Index into LNAMES */
	long	s_size;			/* Size */
}	SEG;

typedef	struct	REC	{
	int	r_type;			/* Type code */
	int	r_length;		/* Length of data (no checksum) */
	char	r_data[NBREC];		/* Data */
}	REC;

REC	rec;				/* OMF record */
char	*name[NNAME];			/* Names, indexed by LNAMES index */
int	nname;				/* Highest name index */
SEG	seg[NSEG];			/* Segments */
int	nseg;				/* Highest segment index */
FILE	*ifp;				/* Input file */

extern char *malloc();

main(argc, argv)
char	*argv[];
{
	register char	*np;
	register SEG	*sp;
	register long	total;

	if (argc < 2)
		fatal(NULL, "Usage: size file\n");
	if ((ifp=fopen(argv[1], SRMODE)) == NULL)
		fatal(argv[1], "cannot open input file");
	while (getrec()!=0 && rec.r_type!=MODEND) {
		if (rec.r_type == LNAMES)
			lnames();
		else if (rec.r_type == SEGDEF)
			segdef();
	}
	total = 0;
	for (sp = &seg[0]; sp < &seg[nseg]; ++sp) {
		printf("\t0x%04lx\t", sp->s_size);
		if (sp->s_index>nname || (np=name[sp->s_index-1])==NULL)
			np = "[Bonzo]";
		printf("%s\n", np);
		total += sp->s_size;
	}
	if (nseg > 1) {
		printf("\t0x%04lx\tTotal (hex)\n", total);
		printf("\t%6ld\tTotal (decimal)\n", total);
	}
	exit(OK);
}

fatal(fn, msg)
char	*fn;
char	*msg;
{
	fprintf(stderr, "size: ");
	if (fn != NULL)
		fprintf(stderr, "%s: ", fn);
	fprintf(stderr, "%s\n", msg);
	exit(BAD);
}

getrec()
{
	register int	b1;
	register int	b2;
	register int	i;
	register int	nbytes;
	register int	checksum;

	if ((rec.r_type=getc(ifp)) == EOF)
		return (0);
	checksum = rec.r_type;
	if ((b1=getc(ifp))==EOF || (b2=getc(ifp))==EOF)
		fatal(NULL, "eof in length");
	checksum += b1;
	checksum += b2;
	nbytes = (b2<<8) | b1;
	if (nbytes >= NBREC)
		fatal("omf item too long");
	rec.r_length = nbytes-1;
	for (i=0; i<nbytes; ++i) {
		if ((b1=getc(ifp)) == EOF)
			fatal(NULL, "eof in data");
		rec.r_data[i] = b1;
		checksum += b1;
	}
	if ((checksum&0xFF) != 0)
		fatal(NULL, "checksum error");
	return (1);
}

lnames()
{
	register char	*namep;
	register int	length;
	register int	index;

	index = 0;
	while (index < rec.r_length) {
		if (nname >= NNAME)
			fatal(NULL, "too many LNAMES items");
		length = rec.r_data[index++];
		if ((namep=malloc(length+1)) == NULL)
			fatal(NULL, "out of space for LNAMES strings");
		name[nname++] = namep;
		while (length--)
			*namep++ = rec.r_data[index++];
		*namep = 0;
	}
}

segdef()
{
	register int	acbp;
	register int	index;
	register int	segindex;
	register long	segsize;

	if (nseg >= NSEG)
		fatal(NULL, "too many SEGDEF items");
	index = 0;
	acbp = rec.r_data[index++];
	if ((acbp&0xE0)==0 || (acbp&0xE0)==0xA0)	/* Has FRAME */
		index += 3;
	if ((acbp&0xE0) == 0xC0)			/* LTL */
		index += 5;
	if ((acbp&0x02) != 0)				/* "Big" */
		segsize = 65536L;
	else
		segsize = (unsigned)((rec.r_data[index+0]&0xFF)
			  | ((rec.r_data[index+1]&0xFF) << 8));
	index += 2;
	segindex = rec.r_data[index++]&0xFF;
	if ((segindex&0x80) != 0) {
		segindex  = (segindex&0x7F) << 8;
		segindex |= rec.r_data[index++]&0xFF;
	}
	seg[nseg].s_index = segindex;
	seg[nseg].s_size  = segsize;
	++nseg;
}
