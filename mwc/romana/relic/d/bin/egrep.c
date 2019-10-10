/*
 * egrep --  pattern matcher
 *
 *	Lines of input are searched for the user-supplied extended
 * regular expression.  Command line options determine what "egrep"
 * does on a match: usually the line is printed.  Lines over BUFSIZ
 * chars in length may be fatal when read from pipes or raw devices.
 *	The implementation goal is speed and, to this end, a DFA is
 * employed.  Two key strategies reduce the time spent constructing
 * the DFA by a factor of 100, in typical cases:  (1) the use of
 * equivalence classes, (2) construction of the DFA dynamically.
 * These strategies win because of the way "egrep" tends to be used.
 * Most patterns use only 10-20 characters in the ASCII set of 128.
 * Most input exercises only 10% of the total possible transitions
 * (so why compute the remaining 90%?).  As a side benefit, these
 * strategies reduce "egrep"'s gigantic appetite for memory to merely
 * huge.  Strategy (2) has the important psychological effect of making
 * "egrep" appear to start running instantly; the inevitable time spent
 * constructing the DFA is amortized over the running time of the
 * program.
 */
#include	<stdio.h>
#include	<ctype.h>
#ifdef COHERENT
#include	<access.h>
#else
#define DOS 1
#endif

/*
** rico.h
*/
#define	bool	char
#define	TRUE	1
#define	FALSE	0

/*
** egrep.h
*/
/*
 * bit diddling
 */
#define	NCHARS	128		/* chars in ASCII set */
#define	NBCHAR	8		/* bits per char */

#define	bitset(c, p)	((p)[(c)>>3] |= bitmask[(c)&7])
#define	bitclr(c, p)	((p)[(c)>>3] &= ~bitmask[(c)&7])
#define	bitcom(c, p)	((p)[(c)>>3] ^= bitmask[(c)&7])
#define	bittst(c, p)	((p)[(c)>>3] & bitmask[(c)&7])

/*
 * support for bitmaps
 */
readonly char bitmask[] = {
	0001, 0002, 0004, 0010, 0020, 0040, 0100, 0200
};


extern char	*newbits();

/*
** dragon.h
*/
/*
 * DFA state
 *	The egrep DFA is composed of these structs.  Operation of the DFA
 * requires the use of d_success and d_p, only.
 */
struct dragon {
	bool		d_success;	/* this is an accepting state */
	struct newt	**d_s;		/* set of NFA states (newts) */
	char		*d_b;		/* bitmap of d_s */
	int		d_hash;		/* hash of d_s */
	struct dragon	*d_next;	/* next dragon (or 0) */
	struct dragon	*d_last;	/* previous dragon (or 0) */
	struct dragon	**d_p;		/* transition vector */
};

/*
** newt.h
*/
/*
 * NFA state
 *	The rex is converted into an NFA composed of these structs.
 */
struct newt {
	char		n_c;		/* label for transition n_cp */
	char		n_flags;	/* [see below] */
	int		n_uniq;		/* unique # */
	int		n_id;		/* ID # of this newt */
	char		*n_b;		/* alternative label (char class) */
	struct newt	*n_cp;		/* transition labeled n_c */
	struct newt	*n_ep;		/* transition labeled EPSILON */
	struct newt	*n_fp;		/* final newt in this sub-goal */
};

/* n_c
 */
#define	EPSILON	(-1)			/* n_cp is an epsilon transition */

/* n_flags
 */
#define	N_BOL	01		/* beginning-of-line */
#define	N_EOL	02		/* end-of-line */

/*
** equiv.c
*/
/*
 * equivalence class
 *	The set of ASCII chars is refined by the sets of chars used by
 * the regular expression.  The result is a set of equivalence classes:
 * disjoint sets of chars, whose union is the ASCII set.
 *	An eclass struct describes one equivalence class.  The structs
 * are linked, and headed by `eclasses'.
 */
struct eclass {
	char		e_c;		/* used if only one char in set */
	char		e_class;	/* ID # of this eclass */
	char		*e_b;		/* used if many chars in set */
	struct eclass	*e_next;	/* next eclass (or 0) */
};

bool	aflag;		/* use emacs after file with hit */
bool	eflag;		/* next arg is regular expression */
bool	fflag;		/* next arg is file containing rex */
bool	vflag;		/* line matches if rex NOT found */
bool	cflag;		/* only print # matches */
bool	lflag;		/* only print name of files that match */
bool	nflag;		/* also print line # */
bool	bflag;		/* also print block # */
bool	sflag;		/* only provide exit status */
bool	hflag;		/* do not print file names */
bool	yflag;		/* lower case also matches upper case input */

int	uniq,	n_id,	n_ec;

FILE	*ifp;		/* input */
FILE	*tmpFile;	/* tmp file for use with emacs */
char	*tmpFn = NULL;	/* tmp file name */
/*
 * egrep
 *	Return 0 if any matches found in the input files, else 1.
 */
main(argc, argv)
char	**argv;
{
	register struct newt	*np;
	register struct dragon	*dp;
	struct newt		*makenfa();
	struct newt		*npolish();
	struct dragon		*initdfa();
	int	status;
	char	**init();
	bool	search();

#ifdef MSDOS
	msdoscvt("egrep", &argc, &argv);
#endif
	argv = init(argc, argv );
#ifdef MSDOS
	/*
	 * On systems which distinguish between ASCII and binary streams,
	 * egrep reads its input as a binary stream, so it can be used to
	 * look for a string in an object file (for example).  It also
	 * writes its output as a binary stream in most cases, so for ASCII
	 * input files newline mapping happens neither on input nor on output.
	 * However, "-c" and "-l" require ASCII output for newline mapping.
	 * Kludgy; one alternative is to have it read and write ASCII streams,
	 * forget about egrep'ing object files, and futz the seek count when
	 * it sees a newline in the input.
	 */
	if (!cflag && !lflag)
		_setbinary(stdout);
#endif

	np = makenfa();
	ncheck(initdfa(np)->d_s);
	np = npolish(np);
	dp = initdfa(np);

	status = 1;

	if (*argv)
		do {
			if ((ifp = fopen(*argv, "rb")) == NULL) {
				fprintf(stderr, 
				   "egrep: can't open %s\n", *argv);
				continue;
			}
			if (search(*argv, dp, np))
				status = 0;
			fclose(ifp);
			if (aflag && (NULL != tmpFn))
				emacs(*argv);
		} while (*++argv);
	else {
		ifp = stdin;
#ifdef	MSDOS
		_setbinary(ifp);
#endif
		if (search("(stdin)", dp, np))
			status = 0;
	}

	return (status);
}

/*
 * initialization
 *	Process command line.  Return `argv' pointing to first file arg.
 */
static char	**
init(argc, argv)
char	**argv;
{
	register char	**av;
	register char	*a;
	register bool	gotrex;
	extern char	*regexp;
	extern FILE	*rexf;

	gotrex = FALSE;
	av = &argv[1];

	while (a = *av++) {
		if (*a != '-') {
			if (! gotrex) {
				gotrex = TRUE;
				regexp = a;
				a = *av++;
			}
			break;
		}
		while (*++a)
			switch (*a) {
			case 'A':
				aflag = TRUE;
				break;
			case 'e':
				if (gotrex)
					onlyone();
				if (*av == NULL)
					fatal("missing arg to -e");
				regexp = *av++;
				gotrex = TRUE;
				break;
			case 'f':
				if (gotrex)
					onlyone();
				if (*av == NULL)
					fatal("missing arg to -f");
				rexf = fopen(*av, "r");
				if (rexf == NULL)
					fatal("can't open %s", *av);
				++av;
				gotrex = TRUE;
				break;
			case 'v':
				vflag = TRUE;
				break;
			case 'c':
				cflag = TRUE;
				break;
			case 'l':
				lflag = TRUE;
				break;
			case 'n':
				nflag = TRUE;
				break;
			case 'b':
				bflag = TRUE;
				break;
			case 's':
				sflag = TRUE;
				break;
			case 'h':
				hflag = TRUE;
				break;
			case 'y':
			case 'i':
				yflag = TRUE;
				break;
			default:
				fatal("no such flag -%c", *a);
			}
	}

	if (! gotrex) {
		fprintf(stderr,
		    "Usage: egrep [ -bcefhlnsvy ] pattern [ file ...]\n");
		exit(2);
	}

	--av;
	if (argc-(av-argv) <= 1)
		hflag = TRUE;
	return (av);
}

/*
 * call emacs with tmpfile.
 */
emacs(arg)
char *arg;
{
#ifdef GEMDOS
#include <path.h>
	extern  char *path(), *getenv();
	extern char **environ;
	static char* cmda[5] = { NULL, "-e", NULL, NULL, NULL };
#endif
#ifdef MSDOS
	char line[BUFSIZ];
#endif
#ifdef COHERENT
	char line[BUFSIZ];
#endif
	int quit;

	fclose(tmpFile);
#ifdef MSDOS
	sprintf(line, "-e %s %s", tmpFn, arg);
	if (0x7f == (quit = execall("me", line)))
#endif
#ifdef COHERENT
	sprintf(line, "me -e %s %s ", tmpFn, arg);
	if (0x7f == (quit = system(line)))
#endif
#ifdef GEMDOS
	cmda[2] = tmpFn;
	cmda[3] = arg;
	if ((NULL == cmda[0]) &&
	   (NULL == (cmda[0] = path(getenv("PATH"), "me.tos", 1)))) {
		fprintf(stderr, "egrep: Cannot locate me.tos\n");
		quit = 1;
	}
	else if ((quit = execve(cmda[0], cmda, environ)) < 0)
#endif
		fprintf(stderr, "egrep: cannot execute 'me'");
	unlink(tmpFn);
	free(tmpFn);
	tmpFn = NULL;
	if (quit)
		exit(0);
}

static
onlyone()
{
	fatal("exactly one pattern required");
}


nomem()
{

	fatal("out of mem");
}


/*
 * fatal error
 */
fatal(arg0)
char	*arg0;
{
	fflush(stdout);
	fprintf(stderr, "egrep: %r\n", &arg0);
	exit(2);
}

/*
** nfa.c
*/


/*
 * creation of the NFA
 */


/* only one is set by init()
 */
char	*regexp;		/* user's regular expression */
FILE	*rexf;			/* file containing user's rex */


static	curc;			/* current char from rex */

struct newt	*getrex();
struct newt	*getterm();
struct newt	*getfac();
struct newt	*getatom();
struct newt	*newnewt();


/*
 * create NFA from rex
 *	A pointer to the NFA is returned.
 */
struct newt	*makenfa()
{
	register struct newt	*p;

	advance();
	p = getrex();
	if (curc)
		misplaced(curc);
	if (p == NULL)
		fatal("empty pattern");
	return (p);
}


/*
 * check NFA semantics
 *	Ensure correct usage of '^' and '$'.
 */
ncheck(npp)
register struct newt	**npp;
{
	register struct newt	*np;

	++uniq;

	while (np = *npp++)
		if (np->n_cp && np->n_c!=EPSILON) {
			if (np->n_flags & N_EOL)
				nbeeline(np);
			nwalk(np->n_cp);
		}
}


/*
 * add finishing touch to NFA
 *	Prepend a sort of ".*" to the front of the NFA.
 */
struct newt	*npolish(np)
register struct newt	*np;
{
	register struct newt	*p;

	p = newnewt();
	p->n_b = newbits(TRUE);
	p->n_cp = newnewt();
	p->n_cp->n_c = EPSILON;
	p->n_cp->n_cp = p;
	p->n_ep = np;
	p->n_fp = np->n_fp;
	return (p);
}


/*
 * get regular expression
 *	A trailing '\n' is tolerated, to accommodate the "-f" option.
 */
static struct newt	*getrex()
{
	register struct newt	*p;
	register struct newt	*q;
	register struct newt	*start;
	struct newt *final;

	start = getterm();
	if (start == NULL)
		return (start);
	final = newnewt();
	start->n_fp->n_c = EPSILON;
	start->n_fp->n_cp = final;
	start->n_fp = final;

	for (; ; ) {
		switch (curc) {
		case '|':
			advance();
			if (p = getterm())
				break;
			misplaced('|');
		case '\n':
			advance();
			if (p = getterm())
				break;
			if (curc != '\0')
				misplaced('\n');
		default:
			return (start);
		}
		p->n_fp->n_c = EPSILON;
		p->n_fp->n_cp = final;
		q = newnewt();
		q->n_c = EPSILON;
		q->n_cp = p;
		q->n_ep = start;
		q->n_fp = final;
		start = q;
	}
}


/*
 * get term
 */
static struct newt	*
getterm()
{
	register struct newt	*start;
	register struct newt	*p;

	start = getfac();
	if (start == NULL)
		return (start);

	while (p = getfac()) {
		start->n_fp->n_c = EPSILON;
		start->n_fp->n_cp = p;
		start->n_fp = p->n_fp;
	}

	return (start);
}


/*
 * get factor
 */
static struct newt *getfac()
{
	register struct newt	*p;
	register struct newt	*q;
	register struct newt	*start;

	start = getatom();
	if (start == NULL)
		return (start);
	p = start;

	if (curc == '*') {
		advance();
		start = newnewt();
		q = newnewt();
		start->n_c = EPSILON;
		start->n_cp = p;
		start->n_ep = q;
		p->n_fp->n_c = EPSILON;
		p->n_fp->n_cp = q;
		p->n_fp->n_ep = p;
		start->n_fp = q;
	}
	else if (curc == '+') {
		advance();
		q = newnewt();
		p->n_fp->n_c = EPSILON;
		p->n_fp->n_cp = q;
		p->n_fp->n_ep = p;
		start->n_fp = q;
	}
	else if (curc == '?') {
		advance();
		start = newnewt();
		start->n_c = EPSILON;
		start->n_cp = p;
		start->n_ep = p->n_fp;
		start->n_fp = p->n_fp;
	}
	return (start);
}


/*
 * get atom
 *	The interpretation of the "-y" option is given by this example:
 *		egrep -y 'R\i\c\o H[a-z]* Tudor'
 * means
 *		egrep 'Rico H[A-Za-z]* T[Uu][Dd][Oo][Rr]'
 */
static struct newt *getatom()
{
	register struct newt	*start;
	char			*charclass();

	start = newnewt();
	start->n_cp = newnewt();
	start->n_fp = start->n_cp;

	switch (curc) {
	case '\0':
	case '\n':
	case '|':
	case '*':
	case ')':
	case ']':
	case '+':
	case '?':
		nonewts(start);
		return (0);
	case '.':
		start->n_b = newbits(TRUE);
		bitclr('\n', start->n_b);
		equiv(start);
		break;
	case '^':
		start->n_flags |= N_BOL;
		start->n_c = '\n';
		equiv(start);
		break;
	case '$':
		start->n_flags |= N_EOL;
#if DOS
		start->n_c = '\r';
#else
		start->n_c = '\n';
#endif
		equiv(start);
		break;
	case '[':
		advance();
		start->n_b = charclass();
		equiv(start);
		break;
	case '(':
		advance();
		nonewts(start);
		start = getrex();
		if (curc == '\0')
			fatal("missing ')'");
		if ((start == NULL) || curc!=')')
			misplaced(curc);
		break;
	case '\\':
		advance();
		if (curc=='\0' || curc=='\n')
			misplaced('\\');
		start->n_c = curc;
		equiv(start);
		break;
	default:
		if (yflag && islower(curc)) {
			start->n_b = newbits(FALSE);
			bitset(curc, start->n_b);
			bitset(toupper(curc), start->n_b);
		}
		else
			start->n_c = curc;
		equiv(start);
	}
	advance();

	return (start);
}


static char *charclass()
{
	register char	*p;
	register	c;
	register bool	compl;

	compl = FALSE;
	p = newbits(FALSE);
	if (curc == '^') {
		compl = TRUE;
		advance();
	}
	if (curc == '\0')
		badclass();

	do {
		c = curc;
		advance();
		if (c == '\0')
			badclass();
		if (curc == '-') {
			advance();
			if (curc == '\0')
				badclass();
			if (curc == ']') {
				bitset('-', p);
				bitset(c, p);
			}
			else if (c > curc)
				fatal("bad char class range %c-%c", c, curc);
			else {
				do {
					bitset(c, p);
				} while (++c <= curc);
				advance();
			}
		}
		else
			bitset(c, p);
	} while (curc != ']');

	if (bittst('\n', p))
		misplaced('\n');
	if (yflag)
		for (c='a'; c<='z'; ++c)
			if (bittst(c, p))
				bitset(toupper(c), p);
	if (compl) {
		for (c=0; c<NCHARS; ++c)
			bitcom(c, p);
		bitclr('\n', p);
	}
	return (p);
}


/*
 * allocate new newt struct
 */
static struct newt *newnewt()
{
	register struct newt	*p;

	p = malloc(sizeof *p);
	if (p == NULL)
		nomem();
	p->n_c = 0;
	p->n_flags = 0;
	p->n_b = NULL;
	p->n_cp = NULL;
	p->n_ep = NULL;
	p->n_fp = NULL;
	p->n_uniq = 0;
	p->n_id = n_id++;

	return (p);
}


/*
 * free newt structs
 *	Used to free unused atoms.
 */
static nonewts(np)
struct newt	*np;
{

	free((char *)np->n_cp);
	free((char *)np);
	n_id -= 2;
}


/*
 * get next char from rex
 *	The next char from the file or the string is placed in `curc'.
 */
static advance()
{
	register	c;

	if (regexp)
		c = *regexp++;
	else {
		c = getc(rexf);
		if (c == EOF)
			c = '\0';
	}
	curc = c & 0177;
}


/*
 * report misplaced '^'
 */
static nwalk(np)
register struct newt	*np;
{

	while (np->n_cp) {
		if (np->n_uniq == uniq)
			return;
		np->n_uniq = uniq;
		if (np->n_flags & N_EOL)
			nbeeline(np);
		if (np->n_flags & N_BOL)
			misplaced('^');
		if (np->n_ep)
			nwalk(np->n_ep);
		np = np->n_cp;
	}
}


/*
 * report misplaced '$'
 */
static nbeeline(np)
register struct newt	*np;
{

	while (np = np->n_cp) {
		if ((np->n_cp && np->n_c!=EPSILON)
		|| (np->n_ep))
			misplaced('$');
	}
}


static misplaced(c)
{
	static char	s[]	= "`c'";

	s[1] = c;
	fatal("misplaced %s in pattern", c=='\n'? "newline": s);
}


static badclass()
{

	fatal("non-terminated char class");
}
/*
** equiv.c
*/


/*
 * equivalence classes
 */


char		etab[NCHARS];		/* map ASCII to eclass # */
struct eclass	*eclasses;		/* head of eclass list */

bool		intersect();
struct eclass	*neweclass();


/*
 * check equivalence
 *	A set of eclasses must exist such that their union equals the
 * chars in `np'.  If not, the equivalence relation must be "refined".
 */
equiv(np)
register struct newt	*np;
{
	register struct eclass	*ep;
	register struct eclass	*p;
	struct eclass		e;

	if (eclasses == NULL) {
		eclasses = neweclass();
		eclasses->e_c = 0;
		eclasses->e_b = newbits(TRUE);
		eclasses->e_next = NULL;
		eclasses->e_class = n_ec++;
	}

	for (ep=eclasses; ep; ep=ep->e_next) {
		if (! intersect(np, ep, &e))
			continue;
		p = neweclass();
		*p = e;
		p->e_class = n_ec++;
		p->e_next = eclasses;
		eclasses = p;
	}
}


/*
 * is there a transition?
 *	Return TRUE if there is a transition from `np' on the chars given
 * by `ep'.
 */
bool eqtrans(ep, np)
register struct eclass	*ep;
register struct newt	*np;
{
	register	i;

	if (ep->e_b) {
		if (np->n_b) {
			for (i=0; i<NCHARS/NBCHAR; ++i)
				if (ep->e_b[i] & np->n_b[i])
					return (TRUE);
		}
		else
			return (bittst(np->n_c, ep->e_b));
	}
	else {
		if (np->n_b)
			return (bittst(ep->e_c, np->n_b));
		else
			return (ep->e_c == np->n_c);
	}
	return (FALSE);
}


/*
 * intersect char sets
 *	If the intersection of `np' and `ep0' is a proper subset of `ep0',
 * then store the intersection in `ep1', store the difference of
 * `ep0' - `ep1' in `ep0' and return TRUE.
 */
static bool intersect(np, ep0, ep1)
register struct newt	*np;
register struct eclass	*ep0;
struct eclass		*ep1;
{
	register	i;
	bool		classcheck();

	if (ep0->e_b == NULL)
		return (FALSE);

	if (np->n_b == NULL) {
		i = np->n_c;
		if (! bittst(i, ep0->e_b))
			return (FALSE);
		bitclr(i, ep0->e_b);
		ep1->e_c = i;
		etab[i] = n_ec;
		ep1->e_b = NULL;
	}
	else {
		if (! classcheck(np->n_b, ep0->e_b))
			return (FALSE);
		ep1->e_b = newbits(FALSE);
		for (i=0; i<NCHARS/NBCHAR; ++i) {
			ep1->e_b[i] = ep0->e_b[i] & np->n_b[i];
			ep0->e_b[i] &= ~np->n_b[i];
		}
		for (i=0; i<NCHARS; ++i)
			if (bittst(i, ep1->e_b))
				etab[i] = n_ec;
		ep1->e_c = 0;
	}

	return (TRUE);
}


/*
 * check intersection
 *	Specific check for char sets represented as bitmaps.  If the
 * intersection of `p' and `q' is a proper subset of `q' return TRUE.
 * Non-modifying nature of this routine is optimized for the "-y" option,
 * where intersections of [Aa] and [Bb] are commonplace.
 */
static bool classcheck(p, q)
register char	*p;
register char	*q;
{
	register	i;
	bool		k1;
	bool		k2;

	k1 = FALSE;
	k2 = FALSE;

	i = NCHARS / NBCHAR;
	do {
		if (*q & *p)
			k1 = TRUE;
		if (*q++ & ~*p++)
			k2 = TRUE;
	} while (--i);

	return (k1 && k2);
}


/*
 * allocate new eclass
 */
static struct eclass *neweclass()
{
	register struct eclass	*ep;

	ep = malloc(sizeof *ep);
	if (ep == NULL)
		nomem();
	return (ep);
}

/*
** dfa.c
*/


/*
 * creation of the DFA
 */

static			bmsize;		/* size of bitmap `d_b' */
static struct dragon	d;		/* prototype dragon */

static struct dragon	*dragons;	/* head of the dragon list */

struct dragon	*member();
struct dragon	*enter();


/*
 * create the DFA start state
 */
struct dragon	*
initdfa(np)
struct newt	*np;
{

	if (d.d_s)
		free((char *)d.d_s);
	d.d_s = malloc((n_id+1)*sizeof(*d.d_s));
	if (d.d_s == NULL)
		nomem();
	d.d_s[0] = np;
	d.d_s[1] = NULL;
	bmsize = (n_id+NBCHAR-1) / NBCHAR;
	if (d.d_b)
		free((char *)d.d_b);
	d.d_b = malloc(bmsize);
	if (d.d_b == NULL)
		nomem();
	dragons = NULL;
	++uniq;
	e_closure();
	return (enter(np->n_fp));
}


/*
 * construct the DFA
 *	This task is done incrementally, after searching has started.
 * The current dragon `dp' needs a transition on the input `ec' given
 * the NFA `np'.  The new dragon is determined, entered into `dp->d_p',
 * and returned.  If the new dragon has never been encountered, it is
 * remembered.  A special case arises if the current dragon is an accept
 * state: all transitions loop to itself.
 *	Note that `ec' is an equivalence class, not a char.
 */
struct dragon	*
makedfa(dp, ec, np)
struct dragon	*dp;
struct newt	*np;
{
	register struct eclass	*ep;
	register struct dragon	*p;
	extern struct eclass	*eclasses;

	if (dp->d_success) {
		dp->d_p[ec] = dp;
		return (dp);
	}
	for (ep=eclasses; ep->e_class!=ec; ep=ep->e_next)
		;
	++uniq;
	gettrans(dp, ep);
	e_closure();
	p = member();
	if (p == NULL)
		p = enter(np->n_fp);
	dp->d_p[ec] = p;
	return (p);
}


/*
 * get transitions
 *	Construct in `d.d_s' the set of newts to which there is a
 * transition on `ep' from some newt in `dp->d_s'.
 */
static gettrans(dp, ep)
struct dragon	*dp;
struct eclass	*ep;
{
	register struct newt	**pp;
	register struct newt	*np;
	register struct newt	*p;
	struct newt	**npp;
	bool			eqtrans();

	npp = d.d_s;

	pp = dp->d_s;
	while (np = *pp++)
		if ((p = np->n_cp)
		&& (np->n_c != EPSILON)
		&& (eqtrans(ep, np))) {
			*npp++ = p;
			p->n_uniq = uniq;
		}
	*npp = NULL;
}


/*
 * locate a dragon
 *	The set of newts given by `d.d_s' are sought in the DFA.  The
 * appropriate dragon is returned, else 0.  The use of bitmap `d_b' allows
 * comparing (unsorted) `d_s' sets in time O(N).  Hashing of `d_s' and
 * "self-organizing" the DFA also help to keep things fast.
 */
static struct dragon *member()
{
	register struct dragon	*dp;
	register char	*p;
	register char	*q;
	int		i;

	dp = dragons;

	do {
		if (dp->d_hash == d.d_hash) {
			p = dp->d_b;
			q = d.d_b;
			i = bmsize; do {
				if (*p++ != *q++)
					break;
			} while (--i);
			if (i == 0) {
				if (dp == dragons)
					break;
				if (dp->d_next)
					dp->d_next->d_last = dp->d_last;
				dp->d_last->d_next = dp->d_next;
				dragons->d_last = dp;
				dp->d_next = dragons;
				dp->d_last = NULL;
				dragons = dp;
				break;
			}
		}
	} while (dp = dp->d_next);

	return (dp);
}


/*
 * enter the dragon
 *	The prototype dragon `d' is used to create a dragon, which is
 * entered in the DFA.
 */
static struct dragon	*
enter(finalnp)
struct newt	*finalnp;
{
	register struct dragon	*p;
	register		i;

	p = malloc(sizeof *p);
	if (p == NULL)
		nomem();
	p->d_success = FALSE;
	p->d_hash = d.d_hash;
	for (i=0; d.d_s[i]; ++i)
		if (d.d_s[i] == finalnp)
			p->d_success = TRUE;
	p->d_s = malloc((i+1)*sizeof(*p->d_s));
	if (p->d_s == NULL)
		nomem();
	for (i=0; p->d_s[i]=d.d_s[i]; ++i)
		;
	p->d_b = malloc(bmsize);
	if (p->d_b == NULL)
		nomem();
	for (i=0; i<bmsize; ++i)
		p->d_b[i] = d.d_b[i];
	p->d_p = malloc(n_ec*sizeof(*p->d_p));
	if (p->d_p == NULL)
		nomem();
	for (i=0; i<n_ec; ++i)
		p->d_p[i] = NULL;
	if (dragons)
		dragons->d_last = p;
	p->d_next = dragons;
	dragons = p;
	p->d_last = NULL;
	return (p);
}


/*
 * perform epsilon-closure
 *	Epsilon-transition `d.d_s' is computed and the result placed
 * in `d.d_s'.  Ensuring a newt is not already on the list requires
 * time O(N), thanks to `uniq'.
 */
static
e_closure()
{
	register struct newt	**nqq;
	register struct newt	*np;
	register struct newt	*p;
	struct newt		**npp;

	d.d_hash = 0;
	for (nqq=d.d_s; *nqq++; )
		;
	--nqq;

	for (npp=d.d_s; npp<nqq; ) {
		np = *npp++;
		d.d_hash += (int)np;
		if ((p = np->n_cp)
		&& (np->n_c == EPSILON)
		&& (p->n_uniq != uniq)) {
			*nqq++ = p;
			p->n_uniq = uniq;
		}
		if ((p = np->n_ep)
		&& (p->n_uniq != uniq)) {
			*nqq++ = p;
			p->n_uniq = uniq;
		}
	}
	*nqq = NULL;

	bmbuild();
}


/*
 * build bitmap
 *	A bitmap of `d.d_s' is built in `d.d_b'.
 */
static
bmbuild()
{
	register struct newt	*np;
	register char		*p;
	register		i;

	p = d.d_b;
	i = bmsize; do {
		*p++ = 0;
	} while (--i);
	i = 0;
	while (np = d.d_s[i++])
		bitset(np->n_id, d.d_b);
}
/*
** search.c
*/
/*
 * execution of the DFA
 */


static bool		status;		/* set TRUE if any matches */
static char		cbuf[BUFSIZ];	/* first BUFSIZ chars of input line */
static char		*file;		/* input file name */
static long		nlines;		/* input line count */
static long		seekpos;	/* start of current line in file */
static long		matches;	/* # matches */
static struct dragon	*dfa;		/* DFA start state */
static struct newt	*nfa;		/* NFA */


/*
 * search a file
 *	Return TRUE if any matches.
 */
bool
search(fn, dp, np)
char		*fn;
struct dragon	*dp;
struct newt	*np;
{
	bool	match();

	file = fn;
	dfa = dp;
	nfa = np;
	nlines = 0;
	seekpos = 0;
	matches = 0;
	status = FALSE;

	while (match())
		;

	if (! sflag && ! lflag && cflag) {
		printfile();
		printf("%ld\n", matches);
	}
	return (status);
}


/*
 * look for a match
 */
bool
match()
{
	register struct dragon	*dp;
	register		c;
	register char		*p;
	long			nchars;
	struct dragon		*dp2;
	extern char		etab[];
	bool			success();

	if ((dp=dfa->d_p[etab['\n']]) == NULL)
		dp = makedfa(dfa, etab['\n'], nfa);
	nchars = 0;
	p = cbuf;

	while ((c=getc(ifp)) != EOF) {
		++nchars;
		c &= 0177;
		if (p < &cbuf[BUFSIZ])
			*p++ = c;
		dp2 = dp;
		if ((dp=dp->d_p[etab[c]]) == NULL)
			dp = makedfa(dp2, etab[c], nfa);
		if (c == '\n') {
			++nlines;
			if (vflag!=dp->d_success && ! success(file, p))
				break;
			seekpos += nchars;
			if (dp->d_success)
				return (TRUE);
			nchars = 0;
			p = cbuf;
		}
	}

	return (FALSE);
}


/*
 * report a match
 *	If the line is to be printed, and it is over BUFSIZ chars, the
 * input file better be seekable.
 */
bool
success(file, p)
char	*file;
char	*p;
{
	extern char *tempnam();
	register char	*q;
	register	c;
	register	n;

	++matches;
	status = TRUE;
	if (sflag)
		return (FALSE);
	if (lflag) {
		printf("%s\n", file);
		return (FALSE);
	}
	if (aflag) {
		if ((NULL == tmpFn) &&
		   ((NULL == (tmpFn = tempnam(NULL, "egr"))) ||
		    (NULL == (tmpFile = fopen(tmpFn, "w"))))) {
			fprintf(stderr,	"egrep: cant open tmp file");
			exit(1);
		}
		fprintf(tmpFile, "%ld:%s: egrep\n", nlines, file);
		return (TRUE);
	}
	if (cflag)
		return (TRUE);
	printfile();
	if (nflag)
		printf("%ld:", nlines);
	if (bflag)
		printf("%ld:", seekpos/BUFSIZ);
	q = cbuf;
	n = p - q;
	while (q < p)
		putchar(*q++);
	if (*--q != '\n') {
		if (fseek(ifp, seekpos+n, 0) == EOF) {
			putchar('\n');
			fatal("line too long");
		}
		do {
			c = getc(ifp);
			if (c == EOF)
				return (FALSE);
			putchar(c);
		} while (c != '\n');
	}
	return (TRUE);
}


static
printfile()
{

	if (! hflag)
		printf("%s:", file);
}

/*
** bits.c
*/


/*
 * allocate bitmap for character class
 *	If `setbits'==TRUE then the map is initialized to ones, else zeros.
 */
char	*
newbits(setbits)
bool	setbits;
{
	register	i;
	register	c;
	register char	*p;
	register char	*q;

	p = malloc(NCHARS/NBCHAR);
	if (p == NULL)
		nomem();
	c = 0;
	if (setbits)
		c = ~0;
	q = p;
	i = NCHARS / NBCHAR;
	do {
		*p++ = c;
	} while (--i);
	return (q);
}
