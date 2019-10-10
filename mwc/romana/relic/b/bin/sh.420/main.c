/*
 * sh/main.c
 * The Bourne shell.
 * Main program, initialization and miscellaneous routines.
 */

#include <assert.h>
#include <stdarg.h>
#include "sh.h"
#include <fcntl.h>

void		fakearg		();
void		reset		();
void		ecantopen	();
void		panic		();
void		printe		();
void		syntax		();


int
main(argc, argv, envp)
char *argv[];
char *envp[];
{

	sarg0 = argc > 0 ? argv [0] : "";
	fakearg (0, argc, argv, envp);

	if (argc > 0 && argv [0][0] == '-') {
		lgnflag = 1;
		umask (ufmask = 022);
	} else if (argc > 0 && argv [0][0] == '+') {
		lgnflag = 2;
		umask (ufmask = 022);
	} else
		umask (ufmask = umask (ufmask));

	/*
	 * NIGEL: we must *force* stdin to be unbuffered to defeat some
	 * excessive cleverness in the stdio library. Note that line-buffering
	 * is a mode that only applies to output.
	 */

	if (setvbuf (stdin, NULL, _IONBF, BUFSIZ) != 0)
		printe ("Cannot set stdin buffering\n");

	if (setjmp (restart) != 0) {
		/* reentry for shell command file execution */
		fakearg (1, nargc, nargv, nenvp);
		argc = nargc;
		argv = nargv;
		envp = nenvp;
		cmdflag ++;
		nllflag = 0;
	}

	shpid = getpid ();
	initvar (envp);

	if (set (argc, argv, 1))
		return 1;

	if (cflag) {
		if (sargp [0] == NULL) {
			printe ("No string for -c?");
			return 1;
		}
		-- sargc;
		session (SARGS, * sargp ++);
	} else if (! sflag && ! iflag && sargc != 0) {
		sarg0 = * sargp ++;
		--sargc;
		if (scmdp == NULL)
			scmdp = sarg0;
		session (SFILE, scmdp);
	} else
		session (SSTR, stdin);

	cleanup_shell_fns ();
	unlink_temp (capture_temp ());
	return slret;
}


/*
 * Make the arg listing of ps come out right.
 *	f == 0, first entry, determine buffer limits.
 *	f != 0, later entry, fill buffer with lies.
 */

void
fakearg(f, argc, argv, envp)
int f, argc;
char **argv, **envp;
{
	static char *fbuf;
	static int nbuf;
	register int n;

	if (f == 0) {
		fbuf = argv [0];
		nbuf = 0;
		if (envp != NULL && envp [0] != NULL) {
			while (envp [1] != NULL)
				envp += 1;
			nbuf = envp [0] - fbuf + strlen (envp [0]) - 1;
		} else if (argc > 0)
			nbuf = argv [argc - 1] - fbuf +
				strlen (argv [argc - 1]) - 1;
	} else {
		if (fbuf == NULL || nbuf == 0)
			return;
		n = 0;
		fbuf [0] = 0;
		while (-- argc > 0) {
			argv += 1;
			n += strlen (argv [0]) + 1;
			if (n >= nbuf)
				break;
			strcat (fbuf, argv [0]);
			strcat (fbuf, " ");
		}
		strcat (fbuf, "\1");	/* non-ascii terminator */
	}
}


/*
 * Push a session of the indicated type. A non-zero return value is the
 * appropriate return value for the session, but offset by one.
 */

int
push_session (type, info, session)
int		type;
__VOID__      *	info;
SES	      *	session;
{
	session->s_next = sesp;
	sesp = session;
	session->s_bpp = savebuf ();

	switch (session->s_type = type) {
	case SARGS:
		session->s_strp = (char *) info;
		session->s_flag = 0;
		break;

	case SARGV:
		session->s_argv = (char **) info;
		if ((session->s_strp = session->s_argv [0]) == NULL)
			return 1;
		session->s_flag = 0;
		break;

	case SFILE:
		session->s_strp = (char *) info;

		/*
		 * NIGEL: We should take steps to ensure that the file
		 * descriptors created for these input files lie above the
		 * range that can be redirected in scripts!
		 */

		if ((type = open (session->s_strp, O_RDONLY)) < 0) {
			ecantopen (session->s_strp);
			return 2;
		}
		if (type > 9) {
			int		scan;
			for (scan = 10 ; scan < FOPEN_MAX ; scan ++) {
				if (fcntl (scan, F_GETFD) == -1) {
					/*
					 * Move the new FD to the new place.
					 */

					dup2 (type, scan);
					close (type);
					type = scan;

					break;
				}
			}
		}
		if ((session->s_ifp = fdopen (type, "r")) == NULL) {
			ecantopen (session->s_strp);
			return 2;
		}
		session->s_flag = isatty (fileno (session->s_ifp)) &&
					  isatty (2);
		break;

	case SSTR:
		session->s_strp = NULL;
		session->s_ifp = (FILE *) info;
		session->s_flag = isatty (fileno (session->s_ifp)) &&
					  isatty (2);
		break;
	}

	return 0;
}


/*
 * Pop the passed-in session, which must be the current session.
 */

void
pop_session (session)
SES	      *	session;
{
	assert (sesp == session);

	freebuf (session->s_bpp);

	if (session->s_type == SFILE)
		fclose (session->s_ifp);

	if (session->s_next == NULL) {
		sigintr (0);
		recover (IRDY);
	}

	sesp = session->s_next;
}


/*
 * Loop on input.
 */

int
session(t, p)
register char *p;
{
	SES s;
	register int rcode;

	if ((rcode = push_session (t, p, & s)) > 0)
		return rcode - 1;

	if (s.s_next == NULL) {		/* Initial entry */
		if (iflag)
			s.s_flag = iflag;
		else
			iflag = s.s_flag;
		dflttrp (IRDY);
	}

	/* Loop on input */
	for (;;) {
		unlink_temp (capture_temp ());

		rcode = setjmp (s.s_envl);
		switch (rcode) {
		case RSET:	/* initial setjmp call */
			switch (lgnflag) {
			case 1:		/* - sign invocation */
				lgnflag = 0;
				if (ffind ("/etc", "profile", 4))
					session (SFILE, duplstr (strt, 0));
				recover (IPROF);
				if (* vhome && ffind (vhome, ".profile", 4))
					session (SFILE, duplstr (strt, 0));
				break;

			case 2:		/* + sign invocation */
				lgnflag = 0;
				if (ffind ("/etc", "profile", 4))
					session (SFILE, duplstr (strt, 0));
				recover (IPROF);
				return exshell (findvar ("SHELL"));
			}
			checkmail ();
			comflag = 1;
			errflag = 0;
			recover (IRDY);
			freebuf (s.s_bpp);
			s.s_bpp = savebuf ();
			if (yyparse () != 0)
				syntax ();

		case REOF:
			recover (IRDY);
			break;

		case RCMD:
			recover (IRDY);
			s.s_con = NULL;
			command (s.s_node);
			if (tflag && tflag ++ >= 2)
				break;
			continue;

		case RERR:
			recover (IRDY);
			if (! errflag)
				syntax ();
			if (! iflag || (tflag && tflag ++ >= 2))
				break;
			continue;

		case RINT:
			if (s.s_next != NULL) {
				sesp = s.s_next;
				reset (RINT);
				NOTREACHED;
			}
			prpflag = 2;
			if (! iflag || (tflag && tflag ++ >= 2))
				break;
			continue;

		case RUEXITS:
		case RUABORT:
			if (s.s_next != NULL) {
				sesp = s.s_next;
				reset (rcode);
				NOTREACHED;
			}
			if (rcode == RUEXITS || ! iflag ||
			    (tflag && tflag ++ >= 2))
				break;
			continue;

		case RNOSBRK:
		case RSYSER:
		case RBRKCON:
		case RNOWAY:
		default:
			if (s.s_next != NULL)
				break;
			if (! iflag || (tflag && tflag ++ >= 2))
				break;
			continue;
		}
		break;
	}

	pop_session (& s);
	return (slret);
}


void
reset (f)
{
	longjmp (sesp->s_envl, f);
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

void
remember_temp (filename)
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

TEMP_FILE *
capture_temp ()
{
	TEMP_FILE     *	temp = temp_list;

	temp_list = NULL;
	return temp;
}


/*
 * Deallocate a list of temporary files.
 */

void
forget_temp (templist)
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

void
unlink_temp (templist)
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
shtmp ()
{
	static char tmpfile [] = "/tmp/shXXXXXXX";
	static int tmpflag = 0;

	tmpflag ++;
	sprintf (tmpfile + 6, "%05d%c%c", shpid, tmpflag % 26 + 'a',
		 (tmpflag / 26) % 26 + 'a');
	return tmpfile;
}


/*
 * Print formatted.
 */

void
prints (format /* , ... */)
char * format;
{
	va_list		args;

	va_start (args, format);
	vfprintf (stderr, format, args);
	va_end (args);
}


/*
 * Make a core dump in /tmp and longjmp back to session -
 *	there's a possibility we'll die horribly.
 */

void
panic (i)
int i;
{
#ifdef PARANOID
	register int f;

	if ((f = fork ()) == 0) {
		abort ();
		NOTREACHED;
	}
	waitc (f);
#endif
	printe ("Internal shell assertion %d failed", i);
	reset (RNOWAY);
	NOTREACHED;
}


/*
 * Print out an error message.
 */

void
printe (format /* , ... */)
char * format;
{
	errflag += 1;
	if (! noeflag) {
		va_list		args;

		va_start (args, format);
		vfprintf (stderr, format, args);
		va_end (args);
		fprintf (stderr, "\n");
	}
}

/*
 * Some familiar errors.
 */

void	ecantopen(s) char *s; { printe ("Cannot open %s", s); }
void	ecantfind(s) char *s; { printe ("Cannot find %s", s); }
void	e2big(s) char *s; { printe ("File to big to execute: %s", s); }
void	ecantmake(s) char *s; { printe ("Cannot create %s", s); }
void	emisschar(c) { printe ("Missing `%c'", c); }
void	ecantfdop() { printe ("Fdopen failed"); }
void	enotdef(s) char *s; { printe ("Cannot find variable %s", s); }
void	eillvar(s) char *s; { printe ("Illegal variable name: %s", s); }
void	eredir() { printe ("Illegal redirection"); }
void	etoolong(s) char *s; {
	printe ("Argument too long %s: %.*s", s, STRSIZE, strt);
}
void	eredirundo() {
  printe ("Unable to preserve redirection state when redirecting builtin");
}


/*
 * Don't print out an error message.
 */

void
yyerror()
{
}


/*
 * print out the prompt given the prompt to write
 */

void
prompt(vps)
char *vps;
{
	prints ("%s", vps);
#if RSX
	fflush (stdout);
#endif
}


/*
 * Syntax error message - print line number and file if
 *	not interactive.
 */

void
syntax()
{
	if (sesp->s_type == SFILE) {
		if (feof (sesp->s_ifp))
			printe ("%s: Syntax error at EOF", sesp->s_strp);
		else
			printe ("%s: Syntax error in line %d", sesp->s_strp,
				yyline);
	} else
		printe ("Syntax error");
}
