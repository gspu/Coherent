/*
 * sh/main.c
 * The Bourne shell.
 * Main program, initialization and miscellaneous routines.
 */

#include <sys/param.h>
#include "sh.h"

main(argc, argv, envp)
char *argv[];
char *envp[];
{

	sarg0 = argc>0 ? argv[0] : "";
	fakearg(0, argc, argv, envp);
	if (argc>0 && argv[0][0]=='-') {
		lgnflag = 1;
		umask(ufmask=022);
	} else if (argc>0 && argv[0][0]=='+') {
		lgnflag = 2;
		umask(ufmask=022);
	} else {
		umask(ufmask=umask(ufmask));
	}

	if (setjmp(restart) != 0) {
		/* reentry for shell command file execution */
		fakearg(1, nargc, nargv, nenvp);
		argc = nargc;
		argv = nargv;
		envp = nenvp;
		cmdflag++;
		nllflag = 0;
	}

	shpid = getpid();
	initvar(envp);
#if 0
	cleanup(1, NULL);
#endif
	if (set(argc, argv, 1))
		return(1);
	if (cflag) {
		if (sargp[0]==NULL) {
			printe("No string for -c?");
			return(1);
		}
		--sargc;
		session(SARGS, *sargp++);
	} else if (!sflag && !iflag && sargc!=0) {
		sarg0 = *sargp++;
		--sargc;
		if (scmdp == NULL)
			scmdp = sarg0;
		session(SFILE, scmdp);
	} else {
		session(SSTR, stdin);
	}
	cleanup_shell_fns ();
	unlink_temp (capture_temp ());
	return (slret);
}

/*
 * Make the arg listing of ps come out right.
 *	f == 0, first entry, determine buffer limits.
 *	f != 0, later entry, fill buffer with lies.
 */
fakearg(f, argc, argv, envp)
int f, argc;
char **argv, **envp;
{
	static char *fbuf;
	static int nbuf;
	register int n;

	if (f == 0) {
		fbuf = argv[0];
		nbuf = 0;
		if (envp != NULL && envp[0] != NULL) {
			while (envp[1] != NULL)
				envp += 1;
			nbuf = envp[0] - fbuf + strlen(envp[0]) - 1;
		} else if (argc > 0)
			nbuf = argv[argc-1] - fbuf + strlen(argv[argc-1]) - 1;
	} else {
		if (fbuf == NULL || nbuf == 0)
			return;
		n = 0;
		fbuf[0] = 0;
		while (--argc > 0) {
			argv += 1;
			n += strlen(argv[0]) + 1;
			if (n >= nbuf)
				break;
			strcat(fbuf, argv[0]);
			strcat(fbuf, " ");
		}
		strcat(fbuf, "\1");	/* non-ascii terminator */
	}
}

/*
 * Loop on input.
 */
session(t, p)
register char *p;
{
	SES s;
	register int rcode;

	s.s_next = sesp;
	sesp = &s;
	s.s_bpp = savebuf();

	switch (s.s_type = t) {
	case SARGS:
		s.s_strp = p;
		s.s_flag = 0;
		break;
	case SARGV:
		s.s_argv = (char **) p;
		if ((s.s_strp = s.s_argv[0]) == NULL)
			return (0);
		s.s_flag = 0;
		break;
	case SFILE:
		s.s_strp = p;
		if ((s.s_ifp = fopen(s.s_strp, "r")) == NULL) {
			ecantopen(s.s_strp);
			return (1);
		}
		s.s_flag = isatty(fileno(s.s_ifp)) && isatty(2);
		break;
	case SSTR:
		s.s_strp = NULL;
		s.s_ifp = (FILE *) p;
		s.s_flag = isatty(fileno(s.s_ifp)) && isatty(2);
		break;
	}

	if (s.s_next == NULL) {		/* Initial entry */
		if (iflag)
			s.s_flag = iflag;
		else
			iflag = s.s_flag;
		dflttrp(IRDY);
	}

	/* Loop on input */
	for (;;) {
		unlink_temp (capture_temp ());

		rcode = setjmp(s.s_envl);
		switch (rcode) {
		case RSET:	/* initial setjmp call */
			switch (lgnflag) {
			case 1:		/* - sign invocation */
				lgnflag = 0;
				if (ffind("/etc", "profile", 4))
					session(SFILE, duplstr(strt, 0));
				recover(IPROF);
				if (*vhome && ffind(vhome, ".profile", 4))
					session(SFILE, duplstr(strt, 0));
				break;
			case 2:		/* + sign invocation */
				lgnflag = 0;
				if (ffind("/etc", "profile", 4))
					session(SFILE, duplstr(strt, 0));
				recover(IPROF);
				return exshell( findvar("SHELL") );
			}
			checkmail();
			comflag = 1;
			errflag = 0;
			recover(IRDY);
			freebuf(s.s_bpp);
			s.s_bpp = savebuf();
			if (yyparse() != 0)
				syntax();
		case REOF:
			recover(IRDY);
			break;
		case RCMD:
			recover(IRDY);
			s.s_con = NULL;
			command(s.s_node);
			if ((tflag && tflag++ >= 2))
				break;
			continue;
		case RERR:
			recover(IRDY);
			if ( ! errflag)
				syntax();
			if ( ! iflag || (tflag && tflag++ >= 2))
				break;
			continue;
		case RINT:
			if (s.s_next != NULL) {
				sesp = s.s_next;
				reset(RINT);
				NOTREACHED;
			}
			prpflag = 2;
			if ( ! iflag || (tflag && tflag++ >= 2))
				break;
			continue;
		case RUEXITS:
		case RUABORT:
			if (s.s_next != NULL) {
				sesp = s.s_next;
				reset(rcode);
				NOTREACHED;
			}
			if (rcode == RUEXITS || !iflag || (tflag && tflag++ >= 2))
				break;
			continue;
		case RNOSBRK:
		case RSYSER:
		case RBRKCON:
		case RNOWAY:
		default:
			if (s.s_next!=NULL)
				break;
			if ( ! iflag || (tflag && tflag++ >= 2))
				break;
			continue;
		}
		break;
	}
	freebuf(s.s_bpp);
	if (s.s_type == SFILE)
		fclose(s.s_ifp);
	if (s.s_next == NULL) {
		sigintr(0);
		recover(IRDY);
	}
	sesp = s.s_next;
	return (slret);
}

reset(f)
{
	longjmp(sesp->s_envl, f);
	NOTREACHED;
}


/*
 * Global head of list of temporary files.
 */

static TEMP_FILE      *	temp_list;

ALLOC_COUNT (temp)

/*
 * Remember the name of a temporary file.
 */

void remember_temp (filename)
char * filename;
{
	TEMP_FILE     *	temp = (TEMP_FILE *) salloc (sizeof (* temp));

	temp->tf_name = duplstr (filename, 1);
	temp->tf_next = temp_list;
	temp_list = temp;

	ALLOC_ALLOC (temp)
}


/*
 * Return a pointer to the current global list of temporary files and clear
 * the global pointer to that list.
 */

TEMP_FILE * capture_temp ()
{
	TEMP_FILE     *	temp = temp_list;

	temp_list = NULL;
	return temp;
}


/*
 * Deallocate a list of temporary files.
 */

void forget_temp (templist)
TEMP_FILE     *	templist;
{
	TEMP_FILE     *	temp;

	while ((temp = templist) != NULL) {

		templist = temp->tf_next;

		ALLOC_FREE (temp);
		sfree (temp->tf_name);
		sfree (temp);
	}
}


/*
 * Walk over a list of temporary files, unlinking the files and deallocating
 * the list nodes.
 */

void unlink_temp (templist)
TEMP_FILE     *	templist;
{
	TEMP_FILE     *	temp;

	while ((temp = templist) != NULL) {

		templist = temp->tf_next;
		unlink (temp->tf_name);

		ALLOC_FREE (temp)
		sfree (temp->tf_name);
		sfree (temp);
	}
}


/*
 * Make a temp file name.
 */
char *
shtmp()
{
	static char tmpfile[] = "/tmp/shXXXXXX";
	static int tmpflag = 0;

	sprintf(tmpfile+6, "%05d%c", shpid, (tmpflag++%26) + 'a');
	return (tmpfile);
}

/*
 * Print formatted.
 */
/*
printv(av)
register char **av;
{
	while (*av) prints("\t%s\n", *av++);
}
*/

prints(a1)
char *a1;
{
	fprintf(stderr, "%r", &a1);
}

/*
 * Make a core dump in /tmp and longjmp back to session -
 *	there's a possibility we'll die horribly.
 */
panic(i) register int i;
{
#ifdef PARANOID
	register int f;

	if ((f=fork())==0) {
		abort();
		NOTREACHED;
	}
	waitc(f);
#endif
	printe("Internal shell assertion %d failed", i);
	reset(RNOWAY);
	NOTREACHED;
}

/*
 * Print out an error message.
 */
printe(a1)
char *a1;
{
	errflag += 1;
	if (! noeflag)
		fprintf(stderr, "%r\n", &a1);
}

/*
 * Some familiar errors.
 */
ecantopen(s) char *s; { printe("Cannot open %s", s); }
ecantfind(s) char *s; { printe("Cannot find %s", s); }
e2big(s) char *s; { printe("File to big to execute: %s", s); }
ecantmake(s) char *s; { printe("Cannot create %s", s); }
emisschar(c) { printe("Missing `%c'", c); }
ecantfdop() { printe("Fdopen failed"); }
enotdef(s) char *s; { printe("Cannot find variable %s", s); }
eillvar(s) char *s; { printe("Illegal variable name: %s", s); }
eredir() { printe("Illegal redirection"); }
etoolong() { printe("Argument too long: %.*s", STRSIZE, strt); }
eredirundo() {
  printe ("Unable to preserve redirection state when redirecting builtin");
}

/*
 * Don't print out an error message.
 */
yyerror()
{
}

/*
 * print out the prompt given the prompt to write
 */
prompt(vps)
char *vps;
{
	prints("%s", vps);
#if RSX
	fflush(stdout);
#endif
}

/*
 * Syntax error message - print line number and file if
 *	not interactive.
 */
syntax()
{
	if (sesp->s_type == SFILE) {
		if (feof(sesp->s_ifp))
			printe("%s: Syntax error at EOF", sesp->s_strp);
		else
			printe("%s: Syntax error in line %d", sesp->s_strp, yyline);
	} else
		printe("Syntax error");
}

/* end of sh/main.c */
