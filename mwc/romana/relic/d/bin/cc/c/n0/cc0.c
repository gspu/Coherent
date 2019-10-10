/*
 * C compiler.
 * Syntax analyser mainline.
 * Impure stuff.
 */
#ifdef   vax
#include "INC$LIB:cc0.h"
#else
#include "cc0.h"
#endif

/*
 * Impure areas.
 * All external impure data is here
 * and is reinitialized by the overlaid version of the mainline.
 * This permits the compiler to be called many times if loaded as a big block.
 */
#if	!OVERLAID
int	line = 1;		/* Source line */
char	file[NFNAME];		/* Source file name */
TOK	*tfile;			/* Source file token */
char	id[NCSYMB];		/* Identifier buffer */
FILE	*ifp;			/* Input file pointer */
FILE	*ofp;			/* Output file pointer */
VARIANT	variant;		/* Variant template */
#endif

/* Cpp variables */
char	dbuf[NDBUF];		/* Substitution buffer */
char	*dputp = dbuf;			/* Define buffer pointer */
char	*dpshp = dbuf+sizeof(dbuf);	/* Define buffer pointer */
DSTACK	dstack[NDLEV];		/* Substitution stack  */
DSTACK	*dlistp=dstack;		/* Substitution list */
DSTACK	*dstackp=dstack+NDLEV-1;	/* Input stack */
ISTACK	istack[NLEV];		/* Include stack */
ISTACK	*istackp=istack+NLEV;	/* Include stack pointer */
CSTACK	cstack[NLEV];		/* Condition stack pointer */
CSTACK	*cstackp=cstack+NLEV;	/* Condition stack pointer */
int	cstate = 0;		/* Conditional state == true */
int	ndirs;			/* Number of include directories */
char	*incdirs[NDIRS+3];	/* Include directory list */
char	deftrue[]  = "1";
char	deffalse[] = "0";
long	curtime;		/* NB should be time_t */

int	incpp;			/* "in a cpp control" flag */
int	instring;		/* "in a string" flag */
int	notskip;		/* count of not skipped chars */
int	lastchar;		/* Last character read from a file */
int	ininit;			/* "in an initializer" flag */
int	incase;			/* "in a case" flag */
int	nargs;			/* # of args this function */
SYM	*args[NARGS];		/* The args themselves */
int	llex;			/* Lexical level */
int	lsym = SL_MOS;		/* Symbol table level */
ival_t	ival;			/* Return for lex */
lval_t	lval;			/* Return for lex */
dval_t	dval;			/* Return for lex */
short	tval;			/* Return for lex int/long constant type */
unsigned	idhash;		/* Hash number for symbol */
unsigned	idhide;		/* Hide set for symbol */
HIDESET	*hidefree, *hidesets;	/* Hide set management */
int	idsize;			/* Length of symbol */
TOK	*idp;			/* Hashed identifier token */
TOK	*hash0[NHASH];		/* Hash table */
int	cblab;			/* Break label */
int	cclab;			/* Continue label */
SBLOCK	*sbp;			/* Current switch block */
SYM	*cfsym;			/* Current symbol */
int     cflab;                  /* Current return label */
int	s;			/* Lexical window */

/*
 * It all starts here.
 * Open the source file.
 * Open the output stream file.
 * Put reserved words into the symbol table.
 * Parse externals until end of file.
 * Finish off the output stream and exit.
 */
#if !OVERLAID
main(argc, argv)
char *argv[];
{
	register char	*p;
	register int	i;
	extern char *index();
	extern char *getenv();

	passname = "cc0";
	if (argc < 4)
		usage();
	getvariant(argv[1]);
	if ((ifp=fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "%s: cannot open.\n", argv[2]);
		exit(BAD);
	}
	if (isvariant(VCPP)) {
		if (strcmp(argv[3], "-") == 0)
			ofp = stdout;
		else if ((ofp = fopen(argv[3], "w")) == NULL) {
			fprintf(stderr, "%s: cannot create.\n", argv[3]);
			exit(BAD);
		}
	} else if ((ofp=fopen(argv[3], SWMODE)) == NULL) {
		fprintf(stderr, "%s: cannot create.\n", argv[3]);
		exit(BAD);
	}
	vinit();
	kinit();
	time(&curtime);
	newtree(sizeof(TREE));
	cppd(MACHINE, deftrue);
	cppd(SYSTEM, deftrue);
	cppd(LOCATION, deftrue);
	cppi(argv[2]);	/* Source directory will be parsed later */
	for (i=4; i<argc; ++i) {
		if (argv[i][0] != '-')
			break;
		switch (argv[i][1]) {
		case 'I': cppi(argv[i]+2); break;
		case 'U': cppu(argv[i]+2); break;
		case 'D':
			if (p = index(argv[i]+2, '='))
				*p++ = 0;
			else
				p = deftrue;
			cppd(argv[i]+2, p);
			break;
		default:
			usage();
		}
	}
	if ((p = getenv("INCDIR")) == 0)
		p = DEFDISK;
	cppi(p);
	cppi(argv[2]);	/* Second copy of source directory */
	setid(argv[2]);
	setfname();
	/* Now parse the source file directory */
	i = 0;
#if COHERENT
	for (p = argv[2]; *p != 0; p += 1)
		if (*p == '/')
			i = p-argv[2];
#endif
#if GEMDOS || MSDOS
	for (p = argv[2]; *p != 0; p += 1)
		if (*p == '\\')
			i = p-argv[2];
#endif
	argv[2][i] = 0;
	emptyfilep();
	if (notvariant(VCPP)) {
		dbfname(file);
		setmname();
		labgen = 0;
		oldseg = -1;
		while (lex() != EOF)
			xdef();
		llex = -1;
		downlex();
		bput(FINISH);
	} else
		cppwork();
	if (nerr != 0)
		exit(BAD);
	exit(OK);
}

/*
 * Print usage message.
 */
usage()
{
	fprintf(stderr,
	"Usage: cc0 variant in out [-Dname[=value]] [-Idirectory] [-Uname]\n"
	);
	exit(BAD);
}

#else
#assert 0	/* This is not ready for the new conventions */
		/* The argv syntax needs to be worked out */
		/* Cleaning up the HIDESET and TOK and SYM pools as well */
/*
 * This is the mainline of the syntax analysis phase
 * if the compiler is being run as an overlay job.
 * The impure area must be reinitialized if monolithic,
 * because the code can be called again and again.
 */
cc0()
{
	register char	*p;
	register int	i;
	extern char *index();

	passname = "cc0";
	if (setjmp(death) != 0)	{
		llex = -1;
		downlex();
		cppfinal();
		return (ABORT);
	}
	vinit();
	kinit();
	cppi("");
	time(&curtime);
	newtree(sizeof(TREE));
	for (i=4; i<argc; ++i) {
		if (argv[i][0] != '-')
			break;
		switch (argv[i][1]) {
		case 'I':
			cppi(argv[i]+2);
			break;
		case 'U':
			cppu(argv[i]+2);
			break;
		case 'D':
			if (p = index(argv[i]+2, '='))
				*p++ = 0;
			else
				p = deftrue;
			cppd(argv[i]+2, p);
			break;
		default:
			cbotch("cpp args");
		}
	}
	cppi(DEFDISK);
	cppi("");
	labgen = 0;
	oldseg = -1;
	line   = 1;
#if MONOLITHIC
	incpp = 0;
	instring = 0;
	lastchar = 0;
	ininit = 0;
	incase = 0;
	cblab = 0;
	cclab = 0;
	sbp = NULL;
	cfsym = NULL;
	lsym = SL_MOS;
	ndirs	= 0;
	dputp = dbuf;
	dpshp = dbuf+sizeof(dbuf);
	dstackp = dstack + NDLEV-1;
	dlistp = dstack;
	istackp = istack + NLEV;
	cstackp	= cstack + NLEV;
	cstate = 0;
#endif
	setid(argv[2]);
	setfname();
	if (notvariant(VCPP)) {
		setmname();
		while (lex() != EOF)
			xdef();
		llex = -1;
		downlex();
		bput(FINISH);
	} else
		cppwork();
	cppfinal();
	if (nerr != 0)
		return (BAD);
	return (OK);
}
#endif

/*
 * Given the file name token in idp,
 * inform the usual parties of the change.
 */
setfname()
{
	tfile = idp;
	strncpy(file, idp->t_id, NFNAME);
	if (notvariant(VCPP)) {
		bput(FNAME);
		sput(file);
	} else if (notvariant(VCPPE))
		fprintf(ofp, "#line %d \"%s\"\n", line, file);
}

/*
 * Given a file name string,
 * make up the associated module name.
 * And then send it on it's way.
 * This routine is tinkered for UDI, DEC systems, MS-DOS
 * and for the Coherent-like systems.
 * Of course, the MNAME item is pointless
 * to all systems except .omf, but we do
 * it anyway.
 * NB, id[] is used as a buffer
 * to construct the module name.
 */
setmname()
{
	register char *tp, *p1, *p2;

	tp = id;
	p1 = file;
	while (*p1 != 0)
		++p1;
	p2 = p1;
	while (p1>file
	    && (p1[-1]!=':' && p1[-1]!='/' && p1[-1]!=']' && p1[-1]!='\\'))
		--p1;
	while (p2>p1 && p2[0]!='.')
		--p2;
	while (p1 < p2)
		*tp++ = *p1++;
	*tp = 0;
	bput(MNAME);
	sput(id);
}

/*
 * Process an external declaration.
 * On entry, 's' contains the first token of the external.
 * On exit, 's' contains the last token in the declaration.
 */
xdef()
{
	register SYM	*sp;
	SYM		*nsp;
	DIM		*dp, *ndp;
	INFO		*ip;
	int		c;
	int		t;
	int		rf;
	int		seg;

	if (s == SEMI)
		return;
	gcandt(&c, &t, &dp, &ip, &rf);
	if (c == C_NONE)
		c = C_GDEF;
	else if (c == C_SIN)
		c = C_SEX;
	else if (c!=C_GREF && c!=C_TYPE) {
		cerror("bad external storage class");
		c = C_GDEF;
	}
	if (s == SEMI) {
		xdropinfo(t, ip);
		return;
	}
	if (t == T_NONE)
		t = T_INT;
	for (;;) {
		if (!gdecl(&nsp, &ndp, dp, c, SL_VAR)) {
			cerror("external syntax");
			while (s!=EOF && s!=SEMI && s!=LBRACE && s!=RBRACE)
				skip();
			if (s == LBRACE)
				statement();
			break;
		}
		sp = nsp;
		sp = declare(sp, c, t, ndp, ip, rf);
		if (c!=C_GREF && sp->s_class!=C_GREF && sp->s_class!=C_TYPE) {
			if (isfunc(sp)) {
				xfunc(sp);
				break;
			}
			seg = SPURE;
			if (s==EOF || s==COMMA || s==SEMI) {
				if ((rf&S_RONLY) == 0)
					seg = SBSS;
				sp->s_seg = seg;
				if (isvariant(VCOMM)
				 && sp->s_class == C_GDEF
				 && seg == SBSS) {
					sp->s_class = C_GREF;
					bput(COMM);
					nput(sp->s_id);
				} else {
					newseg(seg);
					align(sp);
					xlabel(sp);
					bput(BLOCK);
				}
				zput(ssize(sp));
			} else {
				if ((rf&S_RONLY) == 0)
					seg = SDATA;
				sp->s_seg = seg;
				newseg(seg);
				align(sp);
				xlabel(sp);
				/*
				 * The following gives a warning rather than an
				 * error (over steve's objection) so programs
				 * using old style initializers e.g. "int i 1;"
				 * will compile; see K&R "Anachronisms" p. 212.
				 */
				if (s != ASSIGN)
					cwarn("missing '='");
				else
					lex();
				init(sp, sp->s_dp, (sizeof_t)0, 1);
			}
			dbdecl(sp);
		}
		if (s != COMMA) {
			if (s != SEMI)
				cerror("missing semicolon");
			break;
		}
		lex();
	}
	xdropinfo(t, ip);
}

/*
 * Test if the symbol pointed to by 'sp' is a function.
 */
isfunc(sp)
SYM	*sp;
{
	register DIM	*dp;

	if ((dp=sp->s_dp)!=NULL && dp->d_type==D_FUNC)
		return (1);
	return (0);
}

/*
 * Compile a function.
 * 'sp' points at the symbol table node for the function name.
 * 's' is the first token after the function declaration.
 */
xfunc(sp)
register SYM	*sp;
{
	register SYM	*fsp;
	SYM		*nfsp;
	DIM		*dp, *fdp;
	INFO		*ip;
	int		c;
	int		t;
	int		rf;
	int		fline;

	fline = line;
	for (;;) {
		gcandt(&c, &t, &dp, &ip, &rf);
		if (c==C_NONE && t==T_NONE)
			break;
		if (c == C_NONE)
			c = C_PAUTO;
		else if (c == C_REG)
			c = C_PREG;
		else {
			cerror("bad argument storage class");
			c = C_PAUTO;
		}
		if (t == T_NONE)
			t = T_INT;
		for (;;) {
			if (gdecl(&nfsp, &fdp, dp, c, SL_VAR)) {
				fsp = nfsp;
				fsp = declare(fsp, c, t, fdp, ip, rf);
			}
			if (s != COMMA)
				break;
			lex();
		}
		xdropinfo(t, ip);
		mustbe(SEMI);
	}
	initlocals();
	bindargs();
	newseg(SCODE);
	sp->s_seg = SCODE;
	xlabel(sp);
	bput(PROLOG);
	loadargs();
	dbdecl(sp);
	dbargs();
	dbstat(LBRACE, line);
	cfsym = sp;
	cflab = newlab();
	if (s != LBRACE)
		cerror("compound statement required");
	else
		lex();
	llex = LL_AUTO;
	locals();
	putautos();
	while (s!=EOF && s!=RBRACE)
		statement();
	if (s != RBRACE)
		cerror("missing right brace");
	llex = LL_EXT;
	downlex();
	label(cflab);
	dbstat(RBRACE, line);
	bput(EPILOG);
	cfsym = NULL;
}

/*
 * Emit an external label.
 */
xlabel(sp)
register SYM	*sp;
{
	bput((sp->s_class==C_GDEF) ? GLABEL : SLABEL);
	nput(sp->s_id);
}

/*
 * Conditionally drop a structure information block.
 * Sometimes a non null info pointer is a symbol table pointer!
 */
xdropinfo(t, ip)
register int	t;
register INFO	*ip;
{
	if (t==T_FSTRUCT || t==T_FUNION || t==T_FENUM)
		return;
	dropinfo(ip);
}
