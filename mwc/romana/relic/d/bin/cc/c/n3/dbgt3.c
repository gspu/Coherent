/*
 * Print debug information.
 * In VASM mode, we buffer DLABELS until their DLOCAT arrives.
 */
#ifdef	vax
#include "INC$LIB:cc3.h"
#else
#include "cc3.h"
#endif

struct dsave {
	int	d_ref;
	struct dsave *d_next;
	struct dsave *d_more;
	char	d_buf[];
};

extern struct dsave *dbtget();
extern struct dsave *dbtmake();
extern struct dsave *dbtmore();
struct dsave *dsave = NULL;
int	drefnum;
int	dbtisline;

/*
 * Debug type, dim, class names.
 */
char *dnames[] = {
	NULL,		/* No type (yet) */
	"char",		/* Char */
	"uchar",	/* Unsigned char */
	"short",	/* Short */
	"ushort",	/* Unsigned short */
	"int",		/* Int */
	"uint",		/* Unsigned int */
	"long",		/* Long */
	"ulong",	/* Unsigned long */
	"float",	/* Float */
	"double",	/* Double */
	"void",		/* void */
	"struct",	/* Struct */
	"union",	/* Union */
	"enum",		/* Enumeration */
	"ptr",		/* Pointer */
	"func",		/* Function returning */
	"array",	/* Array */
	NULL,		/* Structure member list */
	NULL,		/* Structure name */
	NULL,		/* Unused */
	"static",	/* Static external */
	NULL,		/* Global def. */
	"extern",	/* Global ref. */
	"typedef",	/* Typedef name */
	"struct",	/* Structure tag */
	"union",	/* Union tag */
	"enum",		/* Enumeration tag */
	"source file",	/* Source file name */
	NULL,		/* Line number item */
	"label",	/* Label */
	"auto",		/* Auto */
	NULL,		/* Parametric auto */
	"register",	/* Register */
	"register",	/* Parametric register */
	"static",	/* Static internal */
	NULL,		/* Member of enumeration */
	NULL,		/* Function call */
	NULL,		/* Member of structure */
	NULL		/* Member of union */
};



/*
 * Print out or save debug name and type information.
 */
cc3dbug()
{
	struct dsave *dp;

	dbtisline = 0;
	dp = dbtget(1);
	dp->d_ref = drefnum;
	dbtnext(dp);
	if (notvariant(VASM)
	 || (dbtisline==0 && dp == dsave))
		dbtput(drefnum);
	drefnum += 1;
}

/*
 * Report the location of a debug label
 * or print the label information saved.
 */
cc3dloc()
{
	register int n;

	n = iget();
	if (isvariant(VASM))
		dbtput(n);
	else
		fprintf(ofp, " debug location #%d\n", n);
}

char dbtbuf[2048];	/* Buffer for accumulating information */

struct dsave *
dbtget(indent)
{
	register char *p;
	register int n;
	struct dsave *dp;
	int nline;
	static char vstr[] = " %d";
	static char sstr[] = " %s";
	static char qstr[] = " '%s'";
	static char fstr[] = ":%d";

	dp = NULL;
	p = dbtbuf;
	*p = 0;
	/* Indent */
	if (isvariant(VASM) || indent > 1)
		for (n=0; n<indent; n+=1)
			*p++ = '\t';
	/* Get class */
	n = bget();

	/* Get line number */
	nline = iget();

	sprintf(p, "line %d:", nline);
	p += strlen(p);
	if (dnames[n] != NULL) {
		sprintf(p, sstr, dnames[n]);
		p += strlen(p);
	}

	if (n==DC_LINE || n==DC_LAB)
		dbtisline = 1;
	/* Get value */
	if (n < DC_AUTO)
		;
	else if (n < DC_MOS) {
		sprintf(p, vstr, iget());
		p += strlen(p);
	} else {
		int w, s;

		w = bget();		/* Width */
		s = bget();		/* Offset */
		if (w != 0) {
			sprintf(p, " (width:%d, shift:%d)", w, s);
			p += strlen(p);
		}
		sprintf(p, fstr, iget());		/* Value */
		p += strlen(p);
	}

	/* Get name */
	sget(id, NCSYMB);
	sprintf(p, qstr, id);
	p += strlen(p);

	/* Get type */
	for (;;) {
		n = bget();
		if (n < DC_SEX) {
			if (dnames[n] != NULL) {
				sprintf(p, sstr, dnames[n]);
				p += strlen(p);
			}
			if (n < DX_MEMBS) {
				sizeof_t size;

				size = zget();
				if (n != DT_NONE && n != DD_FUNC) {
					sprintf(p, ":%ld", (long)size);
					p += strlen(p);
				}
			}
			if (n == DX_MEMBS) {
				++indent;
				n = zget();
				sprintf(p, " %d {\n", n);
				dp = dbtmake();
				for ( ; n > 0; n-=1)
					dbtmore(dp, dbtget(indent));
				--indent;
				p = dbtbuf;
				strcpy(p, "\t}");
				p += strlen(p);
				break;
			} else if (n == DX_NAME) {
				zget();
				sget(id, NCSYMB);
				sprintf(p, qstr, id);
				p += strlen(p);
				break;
			} else if (n < DT_STRUCT)
				break;
		} else
			cbotch("unrecognized type byte: %d", n);
	}

	/* Done */
	sprintf(p, "\n");
	return (dbtmore(dp, dbtmake()));
}

dbtnext(dp)
struct dsave *dp;
{
	register struct dsave **dpp;

	dpp = &dsave;
	while (*dpp != NULL)
		dpp = &(*dpp)->d_next;
	*dpp = dp;
}
dbtput(n)
{
	register struct dsave *dp, *dp1;

	while ((dp = dsave) != NULL) {
		if (dp->d_ref > n)
			break;
		dsave = dp->d_next;
		if (isvariant(VASM))
			fputs(CMTSTR, ofp);
		fputs(dp->d_buf, ofp);
		dp1 = dp->d_more;
		free(dp);
		while (dp1 != NULL) {
			if (isvariant(VASM))
				fputs(CMTSTR, ofp);
			fputs(dp1->d_buf, ofp);
			dp = dp1;
			dp1 = dp->d_more;
			free(dp);
		}
	}
}
struct dsave *
dbtmake()
{
	register struct dsave *dp;

	dp = (struct dsave *)malloc(sizeof(*dp) + strlen(dbtbuf) + 1);
	if (dp == NULL)
		cnomem("dbtmake");
	dp->d_ref = -1;
	dp->d_next = dp->d_more = NULL;
	strcpy(dp->d_buf, dbtbuf);
	return (dp);
}

struct dsave *
dbtmore(dp, mdp)
struct dsave *dp, *mdp;
{
	register struct dsave **dpp;

	if (dp == NULL)
		return (mdp);
	for (dpp = &dp->d_more; *dpp != NULL; dpp = &(*dpp)->d_more)
		;
	*dpp = mdp;
	return (dp);
}
