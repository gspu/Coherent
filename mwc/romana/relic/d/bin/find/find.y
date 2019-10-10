%{
/*
 * Find all files in the given
 * directory hierarchies that
 * satisfy the given expression
 * primaries.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <pwd.h>
#include <grp.h>
#include "findnode.h"

#define	NPRIM	(sizeof(primaries)/sizeof(primaries[0]))
#define	NARG	50
#define	NRECUR	14		/* Maximum recursion depth before forking */
#define	NFNAME	600		/* size of filename buffer */
#define	FILEARG	((char *)EOF)
#define	DAYSEC	(60L*60L*24L)	/* seconds in a day */
#define	inode(f,v)	lnode(FUN,f,v,NULL)
#define	snode(f,s)	lnode(FUN,f,0,s)
%}
%start	command

%union	{
	NODE  *nodeptr;
	}

%left	OR
%left	AND
%left	'!'
%token	<nodeptr> NAME PERM TYPE LINKS USER GROUP SIZE INUM
%token	<nodeptr> ATIME CTIME MTIME EXEC OK PRINT NEWER FUN NOP
%type	<nodeptr> exp

%%

command:
	exp '\n'		{ if (seflag)
					code = $1; else
					code = bnode(AND,$1,snode(xprint,NULL));
				  return;
				}
      | '\n'			{ code = snode(xprint, NULL); return; }
	;

exp:
        '(' exp ')'		{ $$ = $2; }
      | '!' exp			{ $$ = bnode('!', $2, NULL); }
      | exp OR exp		{ $$ = bnode(OR, $1, $3); }
      | exp AND exp		{ $$ = bnode(AND, $1, $3); }
      | NAME  			{ $$ = snode(xname, next()); }
      | PERM			{ $$ = onode(xperm); }
      | TYPE			{ $$ = snode(xtype, next()); }
      | LINKS			{ $$ = nnode(xlinks); }
      | USER			{ $$ = getuser(); }
      | GROUP			{ $$ = getgroup(); }
      | SIZE			{ $$ = nnode(xsize); }
      | INUM			{ $$ = nnode(xinum); }
      | ATIME			{ $$ = nnode(xatime); }
      | CTIME			{ $$ = nnode(xctime); }
      | MTIME			{ $$ = nnode(xmtime); }
      | EXEC			{ $$ = enode(0); }
      | OK			{ $$ = enode(1); }
      | PRINT			{ $$ = snode(xprint, NULL); seflag++; }
      | NEWER			{ $$ = getnewer(); }
      | NOP			{ $$ = snode(xnop, NULL); seflag++; }
	;

%%
struct	primary	{
	char	*p_name;
	int	p_lval;
}	primaries[] = {
	"-name", NAME,
	"-perm", PERM,
	"-type", TYPE,
	"-links", LINKS,
	"-user", USER,
	"-group", GROUP,
	"-size", SIZE,
	"-inum", INUM,
	"-atime", ATIME,
	"-ctime", CTIME,
	"-mtime", MTIME,
	"-exec", EXEC,
	"-ok", OK,
	"-print", PRINT,
	"-newer", NEWER,
	"-nop", NOP,
	"-o", OR,
	"-a", AND,
};

char	**gav;
int	gac;
int	depth;			/* Recursive depth */

struct	stat	sb;
char	fname[NFNAME];
char	*prompt;

char	toodeep[] = "directory structure too deep to traverse";
char	nospace[] = "out of memory";

time_t	curtime;

NODE	*code;
int	seflag;			/* Set if a side effect (print, exec) found */

char	*next();
NODE	*bnode();
NODE	*enode();
NODE	*lnode();
NODE	*nnode();
NODE	*onode();
NODE	*getuser();
NODE	*getgroup();
NODE	*getnewer();
int	xname();
int	xperm();
int	xtype();
int	xlinks();
int	xuser();
int	xgroup();
int	xsize();
int	xinum();
int	xatime();
int	xctime();
int	xmtime();
int	xnewer();
int	xexec();
int	xprint();
int	xnop();
char	*getenv();

main(argc, argv)
char *argv[];
{
	register int i;
	register char *ap;
	register int eargc;

	for (i=1; i<argc; i++) {
		ap = argv[i];
		if (*ap == '-')
			break;
		if (ap[1]=='\0' && (*ap=='!' || *ap=='('))
			break;
	}
	if ((eargc=i) < 2)
		usage();
	gav = argv+i;
	gac = argc-i;
	yyparse();
	time(&curtime);
	if ((prompt = getenv("PS1")) == NULL)
		prompt = "> ";
	for (i=1; i<eargc; i++)
		find(argv[i]);
}

/*
 * Lexical analyser
 */
yylex()
{
	static int binop = 0;
	static int ntoken = 0;
	register char *ap;
	struct primary *pp;
	register int token;

	if (ntoken) {
		token = ntoken;
		ntoken = 0;
	} else if ((ap = next()) == NULL)
		token = '\n';
	else if (ap[1] == '\0')
		token = ap[0];
	else if (*ap == '-') {
		for (pp = primaries; pp < &primaries[NPRIM]; pp++)
			if (strcmp(pp->p_name, ap) == 0) {
				token = pp->p_lval;
				break;
			}
		if (pp >= &primaries[NPRIM])
			ferr("`%s' is an illegal primary", ap);
	} else
		ferr("Illegal expression %s\n", ap);
	if (binop && token!=')' && token!='\n' && token!=OR && token!=AND) {
		binop = 0;
		ntoken = token;
		return (AND);
	}
	if (token!=OR && token!=AND && token!='!' && token!='\n' && token!='(')
		binop = 1; else
		binop = 0;
	return (token);
}

yyerror()
{
	fprintf(stderr, "Primary expression syntax error\n");
	usage();
}

/*
 * Return the next argument from the arg list.
 */
char *
next()
{
	if (gac < 1)
		return (NULL);
	gac--;
	return (*gav++);
}

/*
 * Produce a node consisting
 * of an octal number.
 */
NODE *
onode(fun)
int (*fun)();
{
	register char *ap;
	register int num;
	register NODE *np;
	register int type;
	char *aap;

	if ((ap = next()) == NULL)
		ferr("Missing octal permission");
	aap = ap;
	if (*ap == '-') {
		ap++;
		type = -1;
	} else
		type = 0;
	num = 0;
	while (*ap>='0' && *ap<='7')
		num = num*8 + *ap++-'0';
	if (*ap != '\0')
		ferr("%s: bad octal permission", aap);
	np = inode(fun, num);
	np->n_un.n_val = num;
	np->n_type = type;
	return (np);
}

/*
 * Get a number -- it also may be
 * prefixed by `+' or `-' to
 * represent quantities greater or
 * less.
 */
NODE *
nnode(fun)
int (*fun);
{
	register char *ap;
	register int num = 0;
	register int type = 0;
	register NODE *np;
	char *aap;

	if ((ap = next()) == NULL)
		ferr("Missing number");
	aap = ap;
	if (*ap == '+') {
		type = 1;
		ap++;
	} else if (*ap == '-') {
		type = -1;
		ap++;
	}
	while (*ap>='0' && *ap<='9')
		num = num*10 + *ap++ - '0';
	if (*ap != '\0')
		ferr("%s: invalid number", aap);
	np = inode(fun, num);
	np->n_type = type;
	return (np);
}

/*
 * Get a user name or number.
 */
NODE *
getuser()
{
	register struct passwd *pwp;
	register char *cp;
	register int uid;

	if ((cp = next()) == NULL)
		ferr("Missing username");
	if (*cp>='0' && *cp<='9')
		uid = atoi(cp);
	else {
		if ((pwp = getpwnam(cp)) == NULL)
			ferr("%s: bad user name", cp);
		uid = pwp->pw_uid;
	}
	return (lnode(FUN, xuser, uid, NULL));
}

/*
 * Get group
 */
NODE *
getgroup()
{
	register struct group *grp;
	register char *cp;
	register int gid;

	if ((cp = next()) == NULL)
		ferr("Missing group name");
	if (*cp>='0' && *cp<='9')
		gid = atoi(cp);
	else {
		if ((grp = getgrnam(cp)) == NULL)
			ferr("%s: bad group name", cp);
		gid = grp->gr_gid;
	}
	return (lnode(FUN, xgroup, gid, NULL));
}

/*
 * Get the time for the file used in
 * the `-newer' primary.
 */
NODE *
getnewer()
{
	register NODE *np;
	register char *fn;

	if ((fn = next()) == NULL)
		ferr("Missing filename for `-newer'");
	if (stat(fn, &sb) < 0)
		ferr("%s: nonexistent", fn);
	np = inode(xnewer, 0);
	np->n_un.n_time = sb.st_mtime;
	return (np);
}

/*
 * Build an expression tree node (non-leaf).
 */
NODE *
bnode(op, left, right)
int op;
NODE *left, *right;
{
	register NODE *np;

	if ((np = malloc(sizeof (NODE))) == NULL)
		ferr(nospace);
	np->n_op = op;
	np->n_left = left;
	np->n_right = right;
	np->n_un.n_val = 0;
	return (np);
}

/*
 * Build a leaf node in expression tree.
 */
NODE *
lnode(op, fn, val, str)
int (*fn)();
char *str;
{
	register NODE *np;

	if ((np = malloc(sizeof (NODE))) == NULL)
		ferr(nospace);
	np->n_left = np->n_right = NULL;
	np->n_op = op;
	np->n_fun = fn;
	if (str != NULL)
		np->n_un.n_str = str; else
		np->n_un.n_val = val;
	return (np);
}

/*
 * Build an execution node
 * for -ok or -exec.
 */
NODE *
enode(type)
{
	register NODE *np;
	register char **app;
	register char *ap;

	seflag++;
	np = snode(xexec, NULL);
	np->n_type = type;
	if ((np->n_un.n_strp = (char**)malloc(sizeof(char*[NARG])))==NULL)
		ferr(nospace);
	app = np->n_un.n_strp;
	for (;;) {
		if ((ap = next()) == NULL)
			ferr("Non-terminated -exec or -ok command list");
		if (strcmp(ap, "{}") == 0)
			ap = FILEARG;
		else if (strcmp(ap, ";") == 0)
			break;
		if (app-np->n_un.n_strp >= NARG-1)
			ferr("Too many -exec or -ok command arguments");
		*app++ = ap;
	}
	*app = NULL;
	return (np);
}

/*
 * Execute find on a single
 * pathname hierarchy.
 */
find(dir)
char *dir;
{
	register char *ep, *cp;

	cp = dir;
	ep = fname;
	while (*cp)
		*ep++ = *cp++;
	*ep = '\0';
	if (stat(dir, &sb) < 0)
		ferr("Cannot find directory `%s'", dir);
	if ((sb.st_mode&S_IFMT) != S_IFDIR)
		ferr("%s: not a directory", dir);
	fentry(ep, &sb);
}

/*
 * The pointer is the end pointer
 * into the fname buffer.
 * And the stat buffer is passed to this
 * which traverses the directory hierarchy.
 */
fentry(ep, sbp)
char *ep;
struct stat *sbp;
{
	char *buildname();
	register char *np;
	register struct direct *dp;
	register int nb;
	int fd;
	int dirflag;
	char *iobuf;

	if (sbp != NULL) {
		dirflag = (sbp->st_mode&S_IFMT)==S_IFDIR;
		execute(code);
	} else
		dirflag = 1;
	if (dirflag) {
		if (++depth >= NRECUR) {
			depth = 0;
			ffork(ep, sbp);
			return;
		}
		if ((fd = open(fname, 0)) < 0) {
			fmsg("%s: cannot open directory", fname);
			return;
		}
		if ((iobuf = malloc(BUFSIZ)) == NULL)
			ferr(nospace);
		while ((nb = read(fd, iobuf, BUFSIZ)) > 0)
			for (dp = iobuf; dp < &iobuf[nb]; dp++) {
				if (dp->d_ino == 0)
					continue;
				np = dp->d_name;
				if (*np++=='.'
				  && (*np=='\0' || (*np++=='.' && *np=='\0')))
					continue;
				np = buildname(dp, ep);
				if (stat(fname, &sb) < 0) {
					fmsg("%s: cannot stat", fname);
					continue;
				}
				fentry(np, &sb);
			}
		if (nb < 0)
			fmsg("%s: directory read error", fname);
		free(iobuf);
		*ep = '\0';
		close(fd);
		depth--;
	}
}

/*
 * Fork to do a find on recursive directory
 * structure that is too deep to fit into
 * user's open files.
 */
ffork(ep, sbp)
char *ep;
struct stat *sbp;
{
	register int i;
	register int pid;
	int status;

	fflush(stdout);
	if ((pid = fork()) < 0) {
		fmsg(toodeep);
		return;
	}
	if (pid) {
		while (wait(&status) >= 0)
			;
		if (status)
			fmsg("panic: child failed: %o", status);
		return;
	}
	for (i=3; i<_NFILE; i++)
		close(i);
	fentry(ep, (struct stat *)NULL);
	fflush(stdout);
	exit(0);
}

/*
 * Build up the next entry
 * in the name.
 */
char *
buildname(dp, ep)
struct direct *dp;
register char *ep;
{
	register char *cp = dp->d_name;
	register unsigned n = DIRSIZ;

	if (ep+DIRSIZ+2 >= &fname[NFNAME]) {
		fmsg(toodeep);
		return (NULL);
	}
	if (ep>fname && ep[-1]!='/')
		*ep++ = '/';
	do {
		if (*cp == '\0')
			break;
		*ep++ = *cp++;
	} while (--n);
	*ep = '\0';
	return (ep);
}

/*
 * Execute compiled code.
 */
execute(np)
register NODE *np;
{
	switch (np->n_op) {
	case AND:
		if (execute(np->n_left) && execute(np->n_right))
			return (1);
		return (0);

	case OR:
		if (execute(np->n_left) || execute(np->n_right))
			return (1);
		return (0);

	case '!':
		return (!execute(np->n_left));

	case FUN:
		return ((*np->n_fun)(np));

	default:
		ferr("Panic: bad expression tree (op %d)", np->n_op);
	}
	/* NOTREACHED */
}

/*
 * Check for a match on the filename
 */
xname(np)
NODE *np;
{
	register char *ep;

	ep = fname;
	while (*ep != '\0')
		ep++;
	while (ep>fname && *--ep!='/')
		;
	if (*ep == '/')
		ep++;
	return (pnmatch(ep, np->n_un.n_str, 0));
}

/*
 * Compare the mode for a match again
 * octal number `np->n_un.n_val'.
 */
xperm(np)
NODE *np;
{
	register int onum;
	register int mode;

	mode = np->n_type<0 ? sb.st_mode&017777 : sb.st_mode&0777;
	onum = np->n_un.n_val;
	if (np->n_type < 0)
		return ((mode&onum) == onum);
	return (mode == onum);
}

/*
 * Compare again filetypes
 */
xtype(np)
NODE *np;
{
	register char *type;
	register int ftype;

	type = np->n_un.n_str;
	ftype = sb.st_mode&S_IFMT;
	if (type[1] == '\0')
		switch (type[0]) {
		case 'b':
			return (ftype == S_IFBLK);
	
		case 'c':
			return (ftype == S_IFCHR);
	
		case 'd':
			return (ftype == S_IFDIR);
	
		case 'f':
			return (ftype == S_IFREG);
	
		case 'm':
			return (ftype==S_IFMPB || ftype==S_IFMPC);

		case 'p':
			return (ftype == S_IFPIP);
		}
	ferr("Bad file type  `%s'", type);
}

/*
 * Compare link counts.
 */
xlinks(np)
NODE *np;
{
	return (ncomp(np, sb.st_nlink));
}

/*
 * Compare uid.
 */
xuser(np)
NODE *np;
{
	return (np->n_un.n_val == sb.st_uid);
}

/*
 * Compare group id of file
 * with given one.
 */
xgroup(np)
NODE *np;
{
	return (np->n_un.n_val == sb.st_gid);
}

/*
 * Compare size of file in blocks
 * with given.
 */
xsize(np)
NODE *np;
{
	register int fsize;

	fsize = (sb.st_size+BUFSIZ-1)/BUFSIZ;
	return (ncomp(np, fsize));
}

/*
 * Compare the i-number of the file
 * with that given.
 */
xinum(np)
NODE *np;
{
	return (ncomp(np, sb.st_ino));
}

/*
 * Return true if file has been accessed
 * in `n' days.
 */
xatime(np)
NODE *np;
{
	return (ndays(np, sb.st_atime));
}

/*
 * Return non-zero if file has been created
 * in `n' days.
 */
xctime(np)
NODE *np;
{
	return (ndays(np, sb.st_ctime));
}

/*
 * Return true if file has been modified
 * in `n' days.
 */
xmtime(np)
NODE *np;
{
	return (ndays(np, sb.st_mtime));
}

/*
 * Execute a command based on the filename
 */
xexec(np)
NODE *np;
{
	static char command[200];
	register char *ap, **app;
	register int c;
	int ok;

	command[0] = '\0';
	app = np->n_un.n_strp;
	while (*app != NULL) {
		if ((ap = *app++) == FILEARG)
			ap = fname;
		strcat(command, ap);
		if (*app != NULL)
			strcat(command, " ");
	}
	if (np->n_type) {
		printf("%s%s? ", prompt, command);
		ok = (c = getchar()) == 'y';
		while (c!='\n' && c!=EOF)
			c = getchar();
		if (!ok)
			return (0);
	}
	return (!system(command));
}

/*
 * Print the filename.
 */
/* ARGSUSED */
xprint(np)
NODE *np;
{
	printf("%s\n", fname);
	return (1);
}

xnop(np)
NODE *np;
{
	return (1);
}

/*
 * Return true if the file is newer than
 * the given one.
 */
xnewer(np)
register NODE *np;
{
	return (sb.st_mtime > np->n_un.n_time);
}

/*
 * Do a numerical comparison on dates.
 */
ndays(np, t)
register NODE *np;
time_t t;
{
	register int days;

	days = (curtime-t+DAYSEC/2)/DAYSEC;
	return (ncomp(np, days));
}

/*
 * Numerical compare.
 */
ncomp(np, val)
register NODE *np;
register unsigned val;
{
	if (np->n_type == 0)
		return (np->n_un.n_val == val);
	if (np->n_type > 0)
		return (val > np->n_un.n_val);
	return (val < np->n_un.n_val);
}

/*
 * Errors
 */
/* VARARGS */
ferr(x)
{
	fprintf(stderr, "find: %r\n", &x);
	exit (1);
}

/* VARARGS */
fmsg(x)
{
	fprintf(stderr, "find: %r\n", &x);
}

usage()
{
	fprintf(stderr, "Usage: find directory ... [ expression ]\n");
}
