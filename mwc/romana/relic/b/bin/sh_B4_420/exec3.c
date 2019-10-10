/*
 * sh/exec3.c
 * Bourne shell.
 * Builtin commands and shell functions.
 */

#include "sh.h"
#include <sys/times.h>
#include <sys/const.h>		/* HZ defined here */

#define HOUR	(60L*60L*HZ)
#define MINUTE	(60L*HZ)
#define SECOND	HZ

NODE	*copy_node();
char	*cd();

extern	s_colon();
extern	s_dot();
extern	s_break();
#define s_continue	s_break
extern	s_cd();
extern	s_dirs();
extern	s_eval();
extern	s_exec();
extern	s_exit();
extern	s_export();
extern	s_login();
#define s_newgrp	s_login
extern	s_popd();
extern	s_pushd();
extern	s_read();
#define s_readonly	s_export
extern	s_set();
extern	s_shift();
extern	s_times();
extern	s_trap();
extern	s_umask();
extern	s_wait();
#define SNULL	((int (*)())0)

/* Built-in functions. */
typedef struct {
	int	i_hash;
	char	*i_name;
	int	(*i_func)();
} INLINE;

INLINE inls[] = {
	0,	":",		s_colon,
	0,	".",		s_dot,
	0,	"break",	s_break,
	0,	"continue",	s_continue,
	0,	"cd",		s_cd,
	0,	"dirs",		s_dirs,
	0,	"eval",		s_eval,
	0,	"exec",		s_exec,
	0,	"exit",		s_exit,
	0,	"export",	s_export,
	0,	"login",	s_login,
	0,	"newgrp",	s_newgrp,
	0,	"popd",		s_popd,
	0,	"pushd",	s_pushd,
	0,	"read",		s_read,
	0,	"readonly",	s_readonly,
	0,	"set",		s_set,
	0,	"shift",	s_shift,
	0,	"times",	s_times,
	0,	"trap",		s_trap,
	0,	"umask",	s_umask,
	0,	"wait",		s_wait,
	0,	NULL,		SNULL
};

inline()
{
	register int (*s_func)();
	register INLINE *ip;
	register int ahash;

	if (inls[0].i_hash==0)
		for (ip=inls; ip->i_name!=NULL; ip++)
			ip->i_hash = ihash(ip->i_name);
	if (nargv[0] == NULL)
		return 0;
	ahash = ihash(nargv[0]);
	for (ip=inls; ip->i_name!=NULL; ip++)
		if (ip->i_hash==ahash && strcmp(nargv[0], ip->i_name)==0)
			break;
	if ((s_func=ip->i_func) == SNULL)
		return 0;
	/*
	 * Process exec specially, because it has unique semantics for
	 * redirection (among other things). Redirection of builtins is done
	 * using the "undo" facility because builtins must execute in the
	 * top-level environment. The original code here permitted a small
	 * subset of builtins to be redirected, but executed them in a
	 * subshell, giving bogus semantics (eg, it permitted "export" to be
	 * redirected, but in a subshell it had no effect).
	 */
	if (s_func == s_exec)
		slret = (* s_func) ();
	else {
		REDIR_UNDO    *	undo = NULL;

		if (redirect (niovp, & undo) < 0)
			slret = 1;
		else 
			slret = (* s_func) ();

		/*
		 * NB: The rationale for the following kludge is not clear,
		 * but with the ability to properly redirect builtins and
		 * execute the in the top-level context, it no longer works.
		 * It could be added back in, but since it conflicts with
		 * POSIX.2 anyway, why bother.
		 */
#if 0
		if (s_func == s_eval)
			slret = (*s_func)();
		else {
			/* Kludge stderr output to stdout. */
			dup2(1, 2);
			close(1);
			slret = (*s_func)();
		}
#endif

		redirundo (& undo);
	}
	return 1;
}

ihash(cp)
register char *cp;
{
	register int i;
	for (i=0; *cp; i+=*cp++);
	return i;
}


/*
 * Actual builtin functions.
 */
s_colon()
{
	return 0;
}

s_dot()
{
	if (nargc==2) {
		ffind(NULL);
		if (ffind(vpath, nargv[1], 4))
			return session(SFILE, duplstr(strt, 0));
		else {
			ecantfind(nargv[1]);
			return 1;
		}
	} else if (nargc==1)
		return 0;
	syntax();
	return 1;
}

s_break()
{
	register CON *cp;
	register int t;
	register int n;
	int ret;

	ret = nargv[0][0]=='b' ? 2 : 1;
	n = nargc>1 ? atoi(nargv[1]) : 1;
	for (cp = sesp->s_con; cp; cp = cp->c_next) {
		t = cp->c_node->n_type;
		if ((t==NWHILE || t==NFOR || t==NUNTIL) && --n == 0) {
			sesp->s_con = cp;
			longjmp(cp->c_envl, ret);
			break;
		}
		freebuf(cp->c_bpp);
	}
	printe("%s out of bounds", ret==1 ? "Continue" : "Break");
}

/* s_continue is overlaid with s_break */

s_cd()
{
	register char *dir;

	if ((dir = cd((nargc<2) ? vhome : nargv[1])) == NULL)
		return -1;			/* cd failed */
	if (dstack[dstkp] != NULL)
		sfree(dstack[dstkp]);
	dstack[dstkp] = duplstr(dir, 1);	/* update dir stack */
	return 0;
}

s_dirs()
{
	register int i;

	for (i = dstkp; i >= 0; i--)
		printf (stdout, "%s ", dstack[i]);
	fputc('\n', stderr);
}

s_eval()
{
	if (nargc>1)
		return session(SARGV, ++nargv);
	else
		return 0;
}

s_exec()
{
	if (redirect (niovp, NULL) < 0) {
		if (nargc>1) {
			exit(1);
			NOTREACHED;
		}
		return 1;
	}
	if (nargc==1)
		return 0;
	if (no1flag) {
		cleanup_shell_fns ();
		unlink_temp (capture_temp ());
	}
	dflttrp(ICMD);
	++nargv;
	--nargc;
	nenvp = envlvar(nenvp);
	flexec();
	exit(1);
	NOTREACHED;
}

s_exit()
{
	if (nargc > 1)
		slret = atoi(nargv[1]);
	reset(RUEXITS);
	NOTREACHED;
}

s_export()
{
	register int flag;
	register char **varv;

	flag = nargv[0][0]=='e' ? VEXP : VRDO;
	if (nargc < 2)
		tellvar(flag);
	else
		for (varv=++nargv; *varv; )
			if (namevar(*varv))
				flagvar(*varv++, flag);
			else
				eillvar(*varv++);
	return 0;
}

s_login()
{
	register char *cmd;

	cmd = nargv[0][0]=='l' ? "/bin/login" : "/bin/newgrp";
	execve(cmd, nargv, envlvar(nenvp));
	ecantfind(cmd);
	return 1;
}

/* s_newgrp is overlaid with s_login */

s_popd()
{
	register int i, j, n, ret;

	if (nargc == 1)
		return popd();
	/*
	 * Kludge to pop one or more specific dir stack elements.
	 * Do args backwards so e.g. "popd 2 3 4" works as expected.
	 * Internal indices [0, dstkp] are user indices [dstkp, 0].
	 */
	for (ret = 0, i = nargc-1; i > 0; i--) {
		if ((n = atoi(nargv[i])) == 0)
			ret |= popd();
		else if (n < 0 || n > dstkp) {
			printe("Illegal arg: %d", n);
			ret = -1;
			continue;
		} else {
			j = dstkp - n;
			if (dstack[j] != NULL)
				sfree(dstack[j]);
			for ( ; j < dstkp; j++)
				dstack[j] = dstack[j+1];
			--dstkp;
		}
	}
	return ret;
}

s_pushd()
{
	register char *dir;
	register int i, ret;

	if (nargc == 1) {
		/* Exchange top two stack elements. */
		if (dstkp == 0)
			return 1;		/* only one element on stack */
		dir = dstack[dstkp-1];
		dstack[dstkp-1] = dstack[dstkp];
		dstack[dstkp] = dir;		/* exchange top two */
		return ((cd(dir) == NULL) ? -1 : 0);	/* and cd accordingly */
	}
	/* Push one or more directories to stack. */
	for (ret = 0, i = 1; i < nargc; i++)
		ret |= pushd(nargv[i]);
	return ret;
}

/*
 * NB: For s_read (), wrap up the temporary-space global bullshit. Later we
 * can add parameters to this stuff to get the effect of string streams.
 */

#define	temp_string_begin()	((void) (strp = strt))
#define	temp_string_max()	STRSIZE
#define	temp_string_add(c) \
		(strp < strt + temp_string_max () ? (void) (* strp ++ = c) : \
		 (void) etoolong ())
#define	temp_string_end()	(strt)
#define	temp_string_end_copy()	(duplstr (temp_string_end (), 1))
#define	temp_string_temp_copy()	(duplstr (temp_string_end (), 0))
#define	temp_mark()		(0)
#define	temp_release(x)		((void) x)

typedef	int		temp_mark_type;

#define	ARGS(x)		x
void temp_string_begin ARGS (()) {
	temp_string_begin ();
}
int temp_string_max ARGS (()) {
	return temp_string_max ();
}
void temp_string_add ARGS ((c)) int c; {
	temp_string_add (c);
}
char * temp_string_end ARGS (()) {
	return temp_string_end ();
}
char * temp_string_end_copy ARGS (()) {
	return temp_string_end_copy ();
}
char * temp_string_temp_copy ARGS (()) {
	return temp_string_temp_copy ();
}
temp_mark_type temp_mark ARGS (()) {
	return temp_mark ();
}
void temp_release ARGS ((m)) temp_mark_type m; {
	temp_release (m);
}

/*
 * NB: Originally, this code used yylex () to break the input up into words.
 * This was a bad idea, because this caused the IFS variable to have no
 * useful effect. Now we just read using <stdio.h> functions, because the
 * extra machinery for session-management doesn't seem to buy anything.
 */

s_read()
{
	char	     **	argp = nargv + 1;
	char	     **	arg_end = argp + (nargc - 1);
	char	      *	delimiters;
	int		ch;

	while (argp < arg_end)
		if (! namevar (* argp ++))
			eillvar (argp - 1);

	delimiters = vifs == NULL ? " \t" : vifs;

	argp = nargv + 1;
	while (argp < arg_end)  {
		temp_mark_type	mark;
		enum	{
			EAT_WS,
			WORD
		} state = EAT_WS;

		mark = temp_mark ();
		temp_string_begin ();

		if (argp + 1 == arg_end)
			delimiters = "";

		while ((ch = getc (stdin)) != EOF && ch != '\n') {

			if (strchr (delimiters, ch) != NULL) {

				if (state == EAT_WS)
					continue;
				break;
			}

			temp_string_add (ch);
			state = WORD;
		}
		temp_string_add (0);

		assnvar (* argp ++, temp_string_temp_copy ());
		temp_release (mark);

		if (ch == EOF || ch == '\n')
			break;
	}

	while (argp < arg_end)
		assnvar (* argp ++, "");

	return ch == EOF;
}


/* s_readonly overlaid with s_export */

s_set()
{
	if (nargc < 2) {
		tellvar(0);
		return 0;
	}
	return set(nargc, nargv, 0);
}

s_shift()
{
	register int n;

	n = nargc > 1 ? atoi(nargv[1]) : 1;
	while (n > 0 && sargc > 0) {
		n -= 1;
		sargc -= 1;
		sargp += 1;
	}
	return n!=0;
}

s_times()
{
#if	_I386
#define	tb_cutime	tms_cutime
#define	tb_cstime	tms_cstime
	struct	tms	tb;
#else
	struct tbuffer tb;

#endif

	times(&tb);
	ptime(tb.tb_cutime);
	ptime(tb.tb_cstime);
	puts ("\n");
	return 0;
}

s_trap()
{
	register char **vp;
	register char *cp;
	register int err;

	err = 0;
	if (nargc==1)
		return telltrp();
	vp = ++nargv;
	cp = *vp;
	if (class(cp[0], MDIGI)
	 && (cp[1]=='\0' || (class(cp[1], MDIGI) && cp[2]=='\0')))
		cp = NULL;
	else
		++vp;
	while (*vp) {
		if (class(vp[0][0], MDIGI))
			err |= setstrp(atoi(*vp++), cp);
		else {
			printe("Bad trap: %s", *vp++);
			err |= 1;
		}
	}
	return err;
}

s_umask()
{
	if (nargc < 2)
		printf ("%03o\n", ufmask);
	else
		umask(ufmask = atoi(nargv[1]));
	return 0;
}

s_wait()
{
	register int f;

	f = (nargc > 1) ? atoi(nargv[1]) : 0;
	if (f > 0)
		f = -f;
	waitc(f);
	return slret;
}

/*
 * Change to given directory.
 * Update global variable CWD accordingly.
 * Return NULL if bad, otherwise full pathname of the directory.
 */
char *
cd(dir) register char *dir;
{
	if (chdir(dir) < 0) {
		printe("%s: bad directory", dir);
		return NULL;
	}
	if (*dir != '/') {
		/*
		 * Find an absolute pathname for the dstack and $CWD.
		 * The directory now in dstack[dstkp] is "." if _getwd() failed
		 * for any reason (e.g., the user lacks search permission
		 * down the path to "/", or "." was rm'ed by another process).
		 * Avoid _getwd() in this case, it can undo the chdir() above.
		 */
		if ((strcmp(dstack[dstkp], ".") == 0)
		 || ((dir = _getwd()) == NULL))
			return NULL;
	}
	assnvar("CWD", dir);
	return dir;
}

/*
 * Pop the directory stack and change to the previous stacked directory.
 */
popd()
{
	if (dstkp == 0) {
		printe("Directory stack underflow");
		return -1;
	}
	if (dstack[dstkp] != NULL)
		sfree(dstack[dstkp]);
	return (cd(dstack[--dstkp]) == NULL ? -1 : 0);
}

/*
 * Change to given directory and add it to the directory stack.
 */
pushd(dir) register char *dir;
{
	if ((dir = cd(dir)) == NULL)
		return -1;			/* cd failed */
	if (++dstkp >= DSTACKN) {
		--dstkp;
		printe("Directory stack overflow");
		return -1;
	}
	dstack[dstkp] = duplstr(dir, 1);
	return 0;
}

/*
 * The set command.  This is also called from `main' to set
 * options from the command line.  In this case `flag' is
 * set.
 */
set(argc, argv, flag)
register char *argv[];
{
	int n;
	register char *cp;

	if (flag) {
		cflag = 0;
		iflag = 0;
		sflag = 0;
	}
	n = 0;
	if (argc > 0)
		n = 1;
	if (argc>1 && argv[1][0]=='-') {
		register char *fp;

		n++;
		for (cp = &argv[1][1]; *cp; cp++) {
			if ((fp=index(shfnams, *cp)) == NULL
			 || (fp+=shflags-shfnams) == NULL
			 || (fp != &lgnflag && fp > &xflag && flag == 0))
				printe("-%c: Bad option", *cp);
			else if (fp != &lgnflag)
				*fp = *cp;
		}
		if (cp == &argv[1][1]) {
			vflag = 0;
			xflag = 0;
		}
		if (flag == 0 && argc == 2)
			return errflag;
	}
	if (errflag)
		return 1;
	if (sargv != NULL)
		vfree(sargv);
	sargv = vdupl(argv);
	sargc = argc - n;
	sargp = sargv + n;
	return 0;
}

/*
 * print the time as XmX.Xs
 * as fixed by Randall.
 */
ptime(t)
long t;
{
	register int ticks, tenths, seconds;

	printf ("%ldm", t/MINUTE);
	ticks = t%MINUTE;
	seconds = ticks/SECOND;
	tenths = (ticks%SECOND + SECOND/20)/(SECOND/10);
	if (tenths == 10) {
		tenths = 0;
		seconds++;
	}
	printf ("%d.%ds ", seconds, tenths);
}

/* User-defined shell functions. */

/*
 * Lookup a shell function name.
 * The hashing is probably irrelevant.
 */
SHFUNC *
lookup_sh_fn(name) char *name;
{
	register SHFUNC *fnp;
	register int ahash;

	ahash = ihash(name);
	for (fnp=sh_fnp; fnp != NULL; fnp = fnp->fn_link)
		if (fnp->fn_hash==ahash && strcmp(name, fnp->fn_name)==0)
			return fnp;
	return NULL;
}


/*
 * When a subshell is created, the functions are available in the subshell
 * but we don't want to lose the here-documents.
 */

void subshell_shell_fns ()
{
	SHFUNC	      *	scan;

	scan = sh_fnp;

	while (scan != NULL) {
		forget_temp (scan->fn_temp);
		scan->fn_temp = NULL;
		scan = scan->fn_link;
	}
}


/*
 * Unset a shell function, freeing the function body and detaching any
 * temporary files.
 */

void cleanup_shell_fn (fnp)
SHFUNC	      *	fnp;
{
	free_node (fnp->fn_body);
	unlink_temp (fnp->fn_temp);
}


/*
 * Before final exit, clean up all shell functions. Freeing the memory of
 * the function bodies is a waste of time here, so we leave that alone.
 */

void cleanup_shell_fns ()
{
	SHFUNC	      *	scan;
	SHFUNC	      *	next;

	for (scan = sh_fnp ; scan != NULL ; scan = next) {
		next = scan->fn_link;
		unlink_temp (scan->fn_temp);
#if 0
		cleanup_shell_fn (scan);
		sfree (scan->fn_name);
		sfree (scan);
#endif
	}

	sh_fnp = NULL;
}


/*
 * Define a shell function.
 */
def_shell_fn(np) register NODE *np;
{
	register char *name;
	register SHFUNC *fnp;

	name = np->n_strp;
	if ((fnp = lookup_sh_fn(name)) != NULL)
		cleanup_shell_fn (fnp);	/* redeclared, free old body */
	else {
		fnp = salloc(sizeof *fnp);	/* allocate new function */
		fnp->fn_link = sh_fnp;		/* add it to list */
		sh_fnp = fnp;
		fnp->fn_hash = ihash(name);	/* and set member info */ 
		fnp->fn_name = duplstr(name, 1);
	}
	fnp->fn_temp = capture_temp ();
	fnp->fn_body = copy_node(np->n_next);	/* and copy function body */
}

/*
 * Look for a shell function, execute it if found.
 */
int
sh_fn()
{
	register SHFUNC *fnp;
	CON *ocon;
	int oargc;
	char *oarg0;
	char **oargv, **oargp;

	if (nargv[0] == NULL)
		return 0;
	if ((fnp = lookup_sh_fn(nargv[0])) == NULL)
		return 0;

	/* Set up sargc, sarg0, sargv, sargp here for $1 etc. to work. */
	oarg0 = sarg0;
	oargc = sargc;
	oargv = sargv;
	oargp = sargp;
	sarg0 = nargv[0];
	sargc = nargc - 1;
	sargp = sargv = vdupl(nargv+1);
	ocon = sesp->s_con;
	sesp->s_con = NULL;

	++in_sh_fn;
	slret = command(fnp->fn_body);		/* execute it */
	--in_sh_fn;
	ret_done = 0;
	vfree(sargv);

	sesp->s_con = ocon;
	sarg0 = oarg0;
	sargc = oargc;
	sargv = oargv;
	sargp = oargp;
	return 1;
}

/*
 * Recursively allocate a fresh copy of a NODE.
 * Examines type to decide if node uses strp or auxp.
 * Watch out for nodes which contain loops: NFOR2, NWHILE, NUNTIL.
 */
NODE *
copy_node(np) NODE *np;
{
	register NODE *newnp;
	int flag;

#if	0
	printf("copy_node(%x)", np);
	if (np != NULL)
		printf(" type=%d", np->n_type);
	printf("\n");
#endif
	if (np == NULL)
		return NULL;
	flag = 0;
	newnp = salloc(sizeof *np);			/* allocate new NODE */
	newnp->n_type = np->n_type;
	switch(np->n_type) {

	/* The following cases use the strp member. */
	case NRET:
	case NFOR:
	case NARGS:
	case NASSG:
	case NCASE:
	case NCASE3:
	case NIORS:
	case NFUNC:
		newnp->n_strp = duplstr(np->n_strp, 1);
		break;

	/* The following cases use the auxp member. */
	case NWHILE:
	case NUNTIL:
	case NFOR2:
		flag++;
		np->n_next->n_next = NULL;	/* zap the loop for recursion */
		/* fall through... */
	case NNULL:
	case NCOMS:
	case NCTRL:
	case NBRAC:
	case NPARN:
	case NIF:
	case NELSE:
	case NCASE2:
	case NLIST:
	case NANDF:
	case NORF:
	case NBACK:
	case NPIPE:
		newnp->n_auxp = copy_node(np->n_auxp);
		break;

	case NRPIPE:
	case NWPIPE:
	default:
		/* ??? */
		printf("type=%d\n", np->n_type);
		panic(9);
	}
	newnp->n_next = copy_node(np->n_next);
	if (flag)
		newnp->n_next->n_next = newnp;		/* restore loop */
	return newnp;
}

/*
 * Undo the above.
 */
free_node(np) register NODE *np;
{
	if (np == NULL)
		return;
	switch(np->n_type) {

	/* strp */
	case NRET:
	case NFOR:
	case NARGS:
	case NASSG:
	case NCASE:
	case NCASE3:
	case NIORS:
	case NFUNC:
		sfree(np->n_strp);
		break;

	/* auxp */
	case NWHILE:
	case NUNTIL:
	case NFOR2:
		np->n_next->n_next = NULL;	/* zap the loop for recursion */
		/* fall through... */
	case NNULL:
	case NCOMS:
	case NCTRL:
	case NBRAC:
	case NPARN:
	case NIF:
	case NELSE:
	case NCASE2:
	case NLIST:
	case NANDF:
	case NORF:
	case NBACK:
	case NPIPE:
		free_node(np->n_auxp);
		break;

	case NRPIPE:
	case NWPIPE:
	default:
		/* ??? */
		printf("type=%d\n", np->n_type);
		panic(10);
	}
	free_node(np->n_next);
	sfree(np);
}

/* end of sh/exec3.c */
