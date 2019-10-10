/*
 * external variables for YACC
 */
#include "yacc.h"

struct resv restab[] =
{
	"start", START,
	"left", LEFT,
	"right", RIGHT,
	"prec", PREC,
	"nonassoc", NONASSOC,
	"union", UNION,
	"token", TOKEN,
	"type", TYPE,
	NULL
};
struct genre gtab[MAXT] =
{
	0, MAXTERM, 0, &trmptr, "terminals", 
	0, MAXNTERM, NTBASE, &ntrmptr, "non-terminals", 
	0, MAXTYPE, 0, &typeptr, "types" 
};

struct sym **symtab;
struct sym **ntrmptr;
struct sym **trmptr;
struct sym **typeptr;
struct state *states;
struct prod **prdptr;
struct sitem **items;
int yyline = 1;
int nerrors = 0;
int yydebug = 0;
int verbose = 0;
int pstat = 0;
FILE *defin, *tabout, *actout, *listout, *optout, *fhdr;
int tno = 256;
int maxitem = MAXITEM;
int maxprodl = MAXPRODL;
int maxprod = MAXPROD;
int maxstates = MAXSTATE;
int maxreds = MAXREDS;

int nprod;
int nstates;
int startsym = NTBASE+1;
int predlev;
char *gramy = NULL;
#ifdef MSDOS
char * ytabc = "y_tab.c";
char *youtput = "y.output";
char *ytabh = "y_tab.h";
char *parser = "yyparse.c";

#else
char *ytabc = "y.tab.c";
char *youtput = "y.output";
char *ytabh = "y.tab.h";
char *parser = "/lib/yyparse.c";
#endif
#ifdef MSDOS
char acttmp[] = "yaXXXXXX";
char opttmp[] = "yoXXXXXX";
#else
char acttmp[] = "/tmp/yaXXXXXX";
char opttmp[] = "/tmp/yoXXXXXX";
#endif
char	bounderr[] = "too many %s, actual limit %d";
struct sitem *nititem;
struct prod *nitprod;
int nrrconf, nsrconf;
int ndupgos, ndupacts;
int yyredns;
int yygodef;
int yypact;
int yydefact;

char bcount[256] =
{
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 
};

char ltab[256] =
{
	-1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 
};


