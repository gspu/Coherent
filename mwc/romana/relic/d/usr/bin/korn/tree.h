/*
 * command trees for compile/execute
 */

/* $Header: tree.h,v 3.1 88/11/03 09:14:02 egisin Exp $ */

#define	NOBLOCK	((struct op *)NULL)
#define	NOWORD	((char *)NULL)
#define	NOWORDS	((char **)NULL)

/*
 * Description of a command or an operation on commands.
 */
struct op {
	int	type;			/* operation type, see below */
	char  **args;			/* arguments to a command */
	char  **vars;			/* variable assignments */
	struct ioword	**ioact;	/* IO actions (eg, < > >>) */
	struct op *left, *right; 	/* descendents */
	char   *str;		/* identifier for case and for (use vars[0]) */
};

/* Tree.type values */
#define	TEOF	0
#define	TCOM	1		/* command */
#define	TPAREN	2		/* (c-list) */
#define	TPIPE	3		/* a | b */
#define	TLIST	4		/* a [&;] b */
#define	TOR	5		/* || */
#define	TAND	6		/* && */
#define	TFOR	7
#define	TCASE	9
#define	TIF	10
#define	TWHILE	11
#define	TUNTIL	12
#define	TELIF	13
#define	TPAT	14		/* pattern in case */
#define	TBRACE	15		/* {c-list} */
#define	TASYNC	16		/* c & */
#define	TFUNCT	17		/* function name { command; } */
#define	TTIME	18		/* time pipeline */
#define	TEXEC	19		/* fork/exec eval'd TCOM */

/*
 * prefix codes for words in command tree
 */
#define	EOS	0		/* end of string */
#define	CHAR	1		/* unquoted character */
#define	QCHAR	2		/* quoted character */
#define	COMSUB	3		/* $() substitution (0 terminated) */
#define	OQUOTE	4		/* opening " or ' */
#define	CQUOTE	5		/* closing " or ' */
#define	OSUBST	6		/* opening ${ substitution */
#define	CSUBST	7		/* closing } of above */

/*
 * IO redirection
 */
struct ioword {
	short	unit;	/* unit affected */
	short	flag;	/* action (below) */
	char   *name;	/* file name */
};

/* ioword.flag */
#define	IOREAD	BIT(0)	/* < */
#define	IOHERE	BIT(1)	/* << (here file) */
#define	IOWRITE	BIT(2)	/* > */
#define	IOCAT	BIT(3)	/* >> */
#define	IOXHERE	BIT(4)	/* expand in << */
#define	IODUP	BIT(5)	/* [<>]& */

/* values for E_LOOP longjmp */
#define	LBREAK	1
#define	LCONTIN	2

/* execute/exchild flags */
#define	XEXEC	BIT(0)		/* execute without forking */
#define	XFORK	BIT(5)		/* fork before executing */
#define	XBGND	BIT(1)		/* command & */
#define	XPIPEI	BIT(2)		/* input is pipe */
#define	XPIPEO	BIT(3)		/* output is pipe */
#define	XPIPE	(XPIPEI|XPIPEO)	/* member of pipe */
#define	XXCOM	BIT(4)		/* dup2 xcomfd to 1 */

/*
 * flags to control expansion of words
 */
#define	DOBLANK	BIT(1)		/* perform blank interpretation */
#define	DOGLOB	BIT(2)		/* expand [?* */
#define	DOPAT	BIT(3)		/* quote *?[ */
#define	DOTILDE	BIT(5)		/* expand ~ */

/* job.c: job control primatives */
int	execute ARGS((struct op *, int flags));	/* execute tree */
int	exchild ARGS((struct op *, int flags));	/* execute tree as child */
int	waitfor ARGS((int job)); 		/* wait for job completion */
int	waitlast ARGS((void));			/* wait for last job */

/* eval.c: word expansion */
char  **eval ARGS((char **wv, int flag)); 	/* expand words */
char   *evalstr ARGS((char *wp, int flags));	/* expand word */
char   *substitute ARGS((Const char *s, int flags)); /* compile and expand string */

/* tree.c: command trees */
void	ptree ARGS((struct op *t, FILE *f));	/* print tree */
char   *wdscan ARGS((char *wp, int c));		/* scan word for prefix */
char   *wdcopy ARGS((char *wp, Area *));	/* copy word */
struct op *tcopy ARGS((struct op *t, Area *));	/* copy tree */
void	tfree ARGS((struct op *t, Area *));	/* free tree */

