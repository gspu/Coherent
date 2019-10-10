/*
 * AWK header - typedefs and c.
 */

#include <stdio.h>
#include <setjmp.h>
#include <ctype.h>

#define	NWORD	400		/* Longest word or identifier in yylex */
#define	NCSET	256		/* Size of character set (ascii + top bit) */
#define	MAXRECORD 2048		/* Longest input record */
#define	NBPC	8		/* Bytes in a char */
#define	NHASH	64		/* Number of symbol table hash buckets */
#define	NSBRK	256		/* Amount sbroken per call in `xalloc' */
#define	NCLASSC	128		/* Number of class characters */
#define	NCLASS	(NCLASSC/NBPC)	/* Chars in a class */
#define	NOFILE	10		/* Limit on the number of open files */
#define	NNEST	15		/* For/while nesting */

typedef	double	FLOAT;		/* Floating point type */
typedef	long	INT;		/* Integer type */
typedef unsigned char CHAR;	/* Character type */
typedef	unsigned char *STRING;	/* String type */

#define	stof	atof		/* String to FLOAT */
#define	stoi	atol		/* String to INT */

/*
 * Used for identifiers, array elements,
 * and just constants. (The name field is
 * not used here).
 */
typedef	struct	TERM {
	CHAR	t_op;			/* Op type */
	CHAR	t_flag;
	struct	TERM	*t_next;	/* Hash chain */
	unsigned t_hval;		/* Hash value for symbol table */
	unsigned t_ahval;		/* Hash value without array index */
	union {
		FLOAT	t_float;
		INT	t_int;
		STRING	t_str;
		struct {
			union	NODE	*(*t_func)();
			CHAR	t_minarg;
			CHAR	t_maxarg;
		}	t_fun;
	}	t_un;
	CHAR	t_name[];		/* Name of array or identifier */
}	TERM;

/* Node types (t_flag) */
#define	T_NUM	01		/* Numeric value vs. string */
#define	T_INT	02		/* Integer vs. floating */
#define	T_VAR	04		/* Variable */
#define	T_ARRAY	010		/* Array */
#define	T_ALLOC	020		/* Allocated string */

/*
 * Expression tree node.
 */
typedef	struct	OPNODE {
	CHAR	n_op;
	CHAR	n_flag;
	union {
		CHAR	*n_charp;
		int	n_char;
		struct	NODE	*n_node;
	}	n_o1, n_o2, n_o3, n_o4;
}	OPNODE;

/* Defines to made operand NODES easier to type in */
#define	n_O1	n_o1.n_node
#define	n_O2	n_o2.n_node
#define	n_O3	n_o3.n_node
#define	n_O4	n_o4.n_node
#define	t_STRING t_un.t_str
#define	t_INT	t_un.t_int
#define	t_FLOAT	t_un.t_float
#define	t_FUNC	t_un.t_fun.t_func
#define	t_MINARG t_un.t_fun.t_minarg
#define	t_MAXARG t_un.t_fun.t_maxarg

/*
 * Basic nodes -- union of a TERM
 * and an expression tree OPNODE.
 */
typedef	union	NODE {
	OPNODE	opnode;
	TERM	term;
}	NODE;

/*
 * An open file.
 */
typedef	struct	OFILE {
	int	of_flag;
	FILE	*of_fp;
	CHAR	*of_name;
}	OFILE;

/* of_flag bits */
#define	OFPIPE	01		/* This is a pipe output */

/* Defined functions */
#define	nel(x)		(sizeof(x)/sizeof((x)[0]))
#define	endof(x)	(&(x)[nel(x)])
#define	streq(s1,s2)	(strcmp(s1,s2)==0)
#define	bump(ip,t)	(ip+=sizeof(t)/sizeof(int))	/* bump for printf */

/* Function declaration */
CHAR	*xalloc();
STRING	awkinput();
NODE	*lookup();
NODE	*alookup();
NODE	*install();
STRING	evalstring();
INT	evalint();
FLOAT	evalfloat();
NODE	*evalexpr();
FLOAT	stof();
INT	stoi();
NODE	*node();
NODE	*cnode();
NODE	*cpnode();
NODE	*snode();
NODE	*fnode();
NODE	*inode();
NODE	*fargn();
NODE	*xadd();
NODE	*xsub();
NODE	*xmul();
NODE	*xdiv();
NODE	*xmod();
NODE	*xcmp();
NODE	*xconc();
NODE	*xarray();
NODE	*xfield();
NODE	*xassign();
NODE	*xinca();
NODE	*xdeca();
NODE	*xneg();

/*
 * Declaration of built-in
 * functions.
 */
NODE	*f_length();
NODE	*f_sqrt();
NODE	*f_log();
NODE	*f_exp();
NODE	*f_int();
NODE	*f_substr();
NODE	*f_index();
NODE	*f_sprintf();
NODE	*f_split();
NODE	*f_abs();

/*
 * External references.
 */
extern	CHAR	*parg;		/* Program argument pointer */
extern	FILE	*pfp;		/* Program file pointer */
extern	int	lexre;		/* Reading regular expression -- yylex */
extern	CHAR	wordbuf[NWORD];
extern	TERM	xzero;
extern	TERM	xone;
extern	NODE	xfield0;
extern	NODE	*codep;
extern	int	beginflag;
extern	int	endflag;
extern	int	runflag;
extern	int	yflag;
extern	int	nlskip;
extern	int	exitflag;
extern	int	lineno;
extern	int	whitesw;
extern	CHAR	*inline;
extern	jmp_buf	nextenv;
extern	jmp_buf	fwenv[NNEST];
extern	int	fwlevel;
extern	int	brlevel;
extern	int	outflag;
extern	TERM	*symtab[NHASH];
extern	NODE	*tempnodes;
extern	CHAR	SNULL[];
extern	OFILE	files[NOFILE];
extern	CHAR	inbuf[BUFSIZ];
extern	CHAR	outbuf[BUFSIZ];

/*
 * References to the built-in
 * values.
 */
extern	NODE	*FILENAMEp;
extern	NODE	*NFp;
extern	NODE	*NRp;
extern	NODE	*FSp;
extern	NODE	*RSp;
extern	NODE	*OFSp;
extern	NODE	*ORSp;
extern	NODE	*OFMTp;

#define	FILENAME (FILENAMEp->t_STRING)
#define	NF	(NFp->t_INT)
#define	NR	(NRp->t_INT)
#define	FS	(FSp->t_STRING)
#define	RS	(RSp->t_STRING)
#define	OFS	(OFSp->t_STRING)
#define	ORS	(ORSp->t_STRING)
#define	OFMT	(OFMTp->t_STRING)
#define	FSMAP	wordbuf		/* Map to speed up field selection */

/*
 * Operators in the expression tree.
 */
#define	AROOT	1		/* Root -- pattern, action */
#define	ARE	2		/* /RE/ */
#define	AREMAT	3		/* s ~ /RE/ */
#define	ARENMAT 4		/* S !~ /RE/ */
#define	AEQ	5		/* a==b */
#define	ANE	6		/* a!=b */
#define	AGT	7		/* a>b */
#define	AGE	8		/* a>=b */
#define	ALT	9		/* a<b */
#define	ALE	10		/* a<=b */
#define	AADD	11		/* a+b */
#define	ASUB	12		/* a-b */
#define	AMUL	13		/* a*b */
#define	ADIV	14		/* a/b */
#define	AMOD	15		/* a%b */
#define	AASGN	21		/* a=b */
#define	ANOT	22		/* !a */
#define	AANDAND 23		/* a&&b */
#define	AOROR	24		/* a||b */
#define	AARRAY	25		/* a[b] */
#define	AINCA	27		/* a++ */
#define	ADECA	29		/* a-- */
#define	ARCLOS	30		/* RE -- a* */
#define	ARNECL	31		/* RE -- a+ */
#define	ARZOCL	32		/* RE -- a? */
#define	AROR	33		/* RE -- a|b */
#define	ARBOL	34		/* RE -- ^ */
#define	AREOL	35		/* RE -- $ */
#define	ARANY	36		/* RE -- . */
#define	ARCON	37		/* RE - ab */
#define	ARCLASS	38		/* RE - [...] */
#define	ARDCLASS 39		/* RE -- class under `-y' */
#define	ARCHAR	40		/* RE -- CHAR */
#define	ARDCHAR	41		/* RE -- dual case CHAR */
#define	AFIELD	42		/* $a */
#define	APRINT	43		/* print */
#define	APRINTF 44		/* printf */
#define	AFUN	45		/* function */
#define	ALIST	46		/* a,b */
#define	ANUM	48		/* number or string */
#define	AFOUT	49		/* > file */
#define	AFAPP	50		/* >> file */
#define	AFPIPE	51		/* | command */
#define	AIF	52		/* if (exp) statement */
#define	AFORIN	53		/* for (x in array ) */
#define	AFOR	54		/* for(e;e;e) s */
#define	AWHILE	55		/* while(e)s */
#define	ABREAK	56		/* break */
#define	ACONTIN	57		/* continue */
#define	ACONC	58		/* s1 s2 */
#define	ABEGIN	59		/* begin */
#define	AEND	60		/* end */
#define	AEXIT	61		/* exit */
#define	ANEXT	62		/* next */
#define	ARANGE	63		/* a,b */
#define	ATERM	64		/* Terminal */
#define	AFUNC	65		/* Function */
#define	AKEYW	66		/* Dummy type for keyword */
#define ANEG	67		/* - a */
