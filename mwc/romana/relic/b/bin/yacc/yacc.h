/*
 * COCOA structure definitions
 *
 * This parser generator is dedicated to Reinaldo Braga.
 * May he live a hundred years
 */
/*
 * Modified by nigel to eliminate ill-advised use of "flex-arrays" in the
 * data structures, eliminate use of '%r' in yyerror (), and to use <limits.h>
 * rather than magic internal definitions. Other than that, I had nothing to
 * do with this garbage.
 */
#include <stdio.h>
#include <limits.h>

enum {
	FATAL		= 1,	/* flag for yyerror */
	SKIP,			/* ditto */
	NLNO,			/* no line number on error line */
	WARNING
};

enum {
	TTERM		= 0,	/* "genre" for token */
	TNTERM,			/* non-terminal */
	TTYPE,			/* <type> */
	MAXT			/* number of different genres */
};


		/* defaults -- can be changed with run time options */
enum {
	MAXPROD		= 400,	/* maximum number of productions */
	MAXTERM		= 200,	/* maximum number of different terminals */
	MAXNTERM	= 150,	/* maximum number of non terminal symbols */
	MAXSTATE	= 800,	/* max # of states */
	MAXPRODL	= 20,	/* maximum number of symbols in any prodn */
	MAXITEM		= 160,	/* maximum number of items in any state */
	MAXREDS		= 300,	/* maximum number of reductions per state */
	MAXTYPE		= 20	/* for the union of YYSTYPE */
};

/*
 * This defines the size of some char-based bit-sets declared below, and thus
 * sets a hard upper bound on the configurable number of terminals. The
 * original form of this was:
 */
#if 0
				/* if maxterm is > 127 change LSETSIZE */
#define LSETSIZE 30		/* chars in ws ::= MAXTERM/8 + 1 */
#endif

#define	LSETSIZE	(MAXTERM * 2 / CHAR_BIT + 1)

		/* keyword codings */
enum {
	START		= 1,
		/* %token .. %nonassoc must be contiguously coded */
	TOKEN,
	LEFT,
	RIGHT,
	NONASSOC,
	UNION,
	PREC,
	TYPE,
	SEMICOLON,
	VBAR,		/* production separators */
	LBRAC,		/* beginning of action */
	T_IDENT,
	C_IDENT,
	MARK,
	IDENT,
	COMMA,
	INTEGER
};

	/* precedence associativities */
enum {
	UNASSOC		= 0,
	LASSOC,
	RASSOC,
	BASSOC		/* "binary" associativity, meaning %nonassoc */
};

		/* macros */
		/* character manipulation */

		/* run of the mill manifests */
#define UINT -1			/* "unknown" type -- integer */
#define UNKNOWN -1
#define SYMSIZE 32
#define DERIV 01		/* non terminal derives the empty string */
#define CPRES 02		/* temp flag for closure */
#define nterm gtab[TTERM].g_ordno
#define nnonterm gtab[TNTERM].g_ordno
#define ntype gtab[TTYPE].g_ordno
#define maxterm gtab[TTERM].g_maxord
#define maxnterm gtab[TNTERM].g_maxord
#define maxtype gtab[TTYPE].g_maxord
#define maxsym (maxterm+maxnterm+maxtype)
#define NTBASE 010000		/* base number for non terminal allocation */
#define EOFNO 0
#define ERRNO 1
#define bounded(v,l,name) if( v>=l ) yyerror(FATAL, bounderr, name, l)
#define MAXSYM 353


struct sym
{
	char	s_name[SYMSIZE];
	int	s_no;	/* ordinal number of symbol */
	char	s_prc, s_ass; /* precedence, associativity */
	int	s_type;
	char	s_genre; /* "kind" of symbol -- terminal, nonterminal, type */
		/* remaining flags are only used for non-terminals */

	char	s_flags;	/* for closure and lookahead computations */
	int	s_val;	/* external value, for non terminal only */
	int	s_nprods;	/* number of productions having nt as lhs */
	struct prod **s_prods;
	int	s_nstates;	/* for nt A, # of states with item A->. ai* */
	int	*s_states;	/* list */
};

struct sitem
{
	int		i_nitems;	/* number of items in set */
	int	     **	i_items;
};
#define	SITEM_EXTRA_SIZE(n)	((n) * sizeof (int *))
#define	SITEM_TOTAL_SIZE(n)	(sizeof (struct sitem) + SITEM_EXTRA_SIZE (n))
#define	SITEM_EXTRA_INIT(p)	((p)->i_items = (int **) ((p) + 1))

struct state
{
	int		s_tgo;
	struct tgo    *	s_tgos;
	int		s_ntgo;
	struct ntgo   *	s_ntgos;
	int		s_nred;
	struct redn   *	s_reds;
} ;

struct prod
{
	int		p_prodno;	/* index in prdptr */
	char		p_prc, p_ass;	/* precedence, associativity */
	int	      *	p_ord;		/*
					 * Ordinal numbers for production,
					 * starting with LHS and then the
					 * RHS ordinals terminated by a -1
					 * sentinel.
					 * The LHS ordinal is negated.
					 */
#if	0
	int		p_left;		/* -(ordinal number for lhs) */
	int	      *	p_right;	/*
					 * ordinal numbers for rhs w/ -1
					 * end marker
					 */
#endif
};
#define	p_left		p_ord [0]

/*
 * Macro for determining additional space required for "n" items in "p_right"
 * vector, above.
 */
#define	PROD_EXTRA_SIZE(n)	(((n) + 1) * sizeof (int))
#define	PROD_TOTAL_SIZE(n)	(sizeof (struct prod) + PROD_EXTRA_SIZE (n))
#define	PROD_EXTRA_INIT(p)	((p)->p_ord = (int *) ((p) + 1))
#define	PROD_RIGHT(p)		((p)->p_ord + 1)

struct tgo
{
	int	tg_trm;		/* ordinal number of terminal */
	int	tg_st;
};

struct ntgo
{
	int	ng_nt;		/* ordinal number of non terminal */
	int	ng_st;		/* index of state */
	union {				/* MWC DSC */
		struct rel *reln;	/* pointer to relation set */
		struct lset *look;	/* pointer to lookahead set */
	} cheapo;			/* they're not used at once, so... */
};
#define ng_rel cheapo.reln
#define ng_lset cheapo.look

struct redn
{
	struct prod *rd_prod;	/* production pointer */
	struct lset *rd_lset;	/* lookahead set */
};

/* relation between nt transitions */
struct rel
{
	int		r_count;
	int	      *	r_list;
};
#define	REL_EXTRA_SIZE(n)	((n) * sizeof (int))
#define	REL_TOTAL_SIZE(n)	(sizeof (struct rel) + REL_EXTRA_SIZE (n))
#define	REL_EXTRA_INIT(p)	((p)->r_list = (int *) ((p) + 1))

struct trans
{
	struct ntgo   *	t_trans;
	int		t_level;
};

struct lset
{
	union {
		unsigned char u_bits[LSETSIZE];
		struct	lset *u_next;
	} un;
};
#define l_bits un.u_bits
#define l_next un.u_next

struct resv
{
	char	*r_name;
	int	r_val;
};

struct genre
{
	int	g_ordno;		/* current index for table ptr */
	int	g_maxord;		/* limit value for g_ordno */
	int	g_base;			/* "base" value for s_no */
	struct sym ***g_sptr;	/* pointer to table for type - MWC DSC */
	char	*g_name;
};

typedef union
{
	struct	sym *sptr;
	int	ival;
} YYSTYPE;

extern struct sym **symtab;	/* global symbol table */
extern struct sym **ntrmptr;	/* non terminal pointers into symtab */
extern struct sym **trmptr;	/* " terminal " " */
extern struct sym **typeptr;	/* " type " " */
extern struct state *states;
extern struct sitem **items;
extern struct prod **prdptr;
extern int yyline;
extern nerrors;
extern FILE *defin, *tabout, *actout, *listout, *optout, *fhdr;
extern int tno;
extern char *gramy;	/* input file name */
extern char *ytabc;	/* y.tab.c output file */
extern char *youtput;	/* listing file */
extern char *ytabh;	/* header file for token # definers */
extern char acttmp[], opttmp[];
extern char *parser;
extern	char	bounderr[];
extern struct sitem *nititem;
extern struct prod *nitprod;
extern verbose, yydebug;
extern pstat;
extern int nstates;
extern int nprod;
extern int maxstates;
extern int maxprod;
extern int nrrconf, nsrconf;
extern int ndupgos, ndupacts;
extern struct genre gtab[MAXT];
extern int startsym;
extern int predlev;
extern struct resv restab[];
extern int maxitem, maxprodl, maxreds;

char 	*calloc();
char	*yalloc();
char	*ptosym();
char	*prsym();
char	*nextarg();
struct	ntgo	*findnt();
struct	lset	*getset();
