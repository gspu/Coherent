/*
 * pretty print routines
 */

#include <stdarg.h>
#include "yacc.h"

static char ctab[] = " |";
listgram()
{
	register i, k, j;
	register struct sym *sp;
	struct prod *pp;

	if( !verbose )
		return;
	fprintf(listout,"Input grammar:\n\n");
	for(i=0; i<nnonterm; i++) {
		sp = ntrmptr[i];
		fprintf(listout,"%s:\n\n", sp->s_name);
		for(j=0; j<sp->s_nprods; j++ ) {
			pp = sp->s_prods[j];
			fprintf(listout, "%d:\t%c ", pp->p_prodno, ctab[j!=0]);
			for(k = 0 ; PROD_RIGHT (pp) [k] >= 0 ; k ++)
				fprintf (listout, "%s ",
					 ptosym (PROD_RIGHT (pp) [k]));
			fprintf(listout, "\n");
		}
		fprintf(listout,"\t;\n\n");
	}
}

prstate(i, f)
FILE *f;
{
	register j;
	register struct sitem *itp;

	itp = items[i];
	fprintf(f, "State %d:\n\n", i);
	for(j=0; j<itp->i_nitems; j++)
		pritem(itp->i_items[j],f);
	fprintf(f, "\n");
}

pritem(ip,f)
register int *ip;
FILE *f;
{
	register int *pp;

	pp = ip;
	do ; while( *--pp >= 0 );
	fprintf(f,"\t");
	fprintf(f, "%s <- ", ptosym(-*pp++));
	while( pp!=ip ) {
		fprintf(f, "%s ", ptosym(*pp++));
	}
	fprintf(f, "_ ");
	while( *pp!=-1 )
		fprintf(f, "%s ", ptosym(*pp++));
	fprintf(f,"\n");
}

char *
ptosym(n)
register n;
{
	if (n >= NTBASE)
		return ntrmptr [n - NTBASE]->s_name;
	else
		return trmptr [n]->s_name;
}

char *
prsym(c)
register c;
{
	static char s[10];

	if( c<040 || c>= 0177 ) {
		switch( c ) {
		case '\n':
			sprintf(s, "'\\n'");
			break;

		case '\r':
			sprintf(s, "'\\r'");
			break;

		case '\t':
			sprintf(s, "'\\t'");
			break;

		case '\b':
			sprintf(s,"'\\b'");
			break;

		case '\f':
			sprintf(s, "'\\f'");
			break;

		case '\0':
			sprintf(s, "'\\0'");
			break;

		default:
			sprintf(s, "'\\%3o'", c);		/* MWC DSC */
		}
	} else
		sprintf(s, "'%c'", c);
	return( s );
}

options(argc, argv)
int argc;
char *argv[];
{
	register i;
	register char *ap;

	for(i=1; i<argc; i++) {
		ap = argv[i];
		if( ap[0]!='-' || strcmp(ap,"-")==0 )
			gramy = argv[i];
		else
			if( strcmp(ap,"-v")==0 )
				verbose++;
		else
			if( strcmp(ap,"-l")==0 ) {
				verbose++;
				youtput = nextarg(argv[++i]);
			}
		else
			if( strcmp(ap, "-st")==0 ) {
				pstat++;
			}
		else
			if( strcmp(ap,"-hdr")==0 ) {
				ytabh = nextarg(argv[++i]);
			}
		else
			if( strcmp(ap,"-d")==0 ) {
				yydebug++;
				verbose++;
			}
		else
			if( strcmp(ap, "-items")==0 )
				maxitem = getnum(argv[++i]);
		else
			if( strcmp(ap, "-sprod")==0 )
				maxprodl = getnum(argv[++i]);
		else
			if( strcmp(ap,"-prods")==0 )
				maxprod = getnum(argv[++i]);
		else
			if( strcmp(ap,"-reds")==0 )
				maxreds = getnum(argv[++i]);
		else
			if( strcmp(ap,"-terms")==0 )
				maxterm = getnum(argv[++i]);
		else
			if( strcmp(ap,"-nterms")==0 )
				maxnterm = getnum(argv[++i]);
		else
			if( strcmp(ap,"-states")==0 )
				maxstates = getnum(argv[++i]);
		else
			if( strcmp(ap,"-types")==0 )
				maxtype = getnum(argv[++i]);
		else
			usage();
		if (maxterm >= LSETSIZE * CHAR_BIT)
			yyerror (NLNO|FATAL,
				 "The maximum -terms value is %d\n",
				 LSETSIZE * CHAR_BIT);
	}
/*
 *	if( (maxterm+maxnterm+maxtype) >= MAXSYM )
 *		yyerror(NLNO|FATAL, "increase MAXSYM");
 *
 * MWC DSC - make it all flexible with maxsym - it will also mean less memory
 * allocated, in most cases.
 */
	prdptr = (struct prod **) yalloc (maxprod, sizeof (* prdptr));
	symtab = (struct sym **)
			yalloc(maxsym+1, sizeof *symtab);	/* MWC DSC */
	ntrmptr = (struct sym **) yalloc (maxnterm, sizeof *ntrmptr);
	trmptr = (struct sym **) yalloc (maxterm, sizeof *trmptr);
	typeptr = (struct sym **) yalloc (maxtype, sizeof *typeptr);

	nitprod = (struct prod *) yalloc (1, PROD_TOTAL_SIZE (maxprodl));
	PROD_EXTRA_INIT (nitprod);

	nititem = (struct sitem *) yalloc (1, SITEM_TOTAL_SIZE (maxitem));
	SITEM_EXTRA_INIT (nititem);

	states = (struct state *) yalloc(maxstates, sizeof *states);
	items = (struct sitem **) yalloc(maxstates, sizeof (* items));
}

yyerror(type, format /* , ... */)
char * format;
{
	va_list		argp;

	if( type&FATAL )
		fprintf(stderr, "fatal error: ");
	if( (type&NLNO)==0 )
		fprintf(stderr, "line %d: ", yyline);

	/* Look ma! No more '%r'! */
	va_start (argp, format);
	vfprintf (stderr, format, argp);
	va_end (argp);

	fprintf(stderr, "\n");
	if( type&FATAL )
		cleanup(2);
	if( (type&WARNING)==0 )
		nerrors++;
	if( type&SKIP )
		while( llgetc()!='\n' )		/* MWC DSC */
			;
}

getnum(s)
char *s;
{
	if( s==NULL )
		yyerror(NLNO|FATAL, "missing argument");
	return( atoi(s) );
}

char *			/* MWC DSC */
nextarg(s)
char *s;
{
	if( s==NULL )
		yyerror(NLNO|FATAL, "missing file name");
	return( s );
}

usage()
{
	fprintf(stderr, "Usage: yacc [options] [parameters] grammar\n");
	fprintf(stderr, "Options: -v -d -l file -hdr file -st\n");
	fprintf(stderr,"Parameters: -prods # -terms # - nterms # -states # -types #\n");
	exit(1);
}
