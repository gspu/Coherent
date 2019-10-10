/*
 * The Bourne shell.
 * Main program, initialization and miscellaneous routines.
 */

#include <common/tricks.h>
#include <stdarg.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>

#include "shellio.h"
#include "sh.h"

NO_RETURN	reset		();
void		panic		();

/*
 * Under Coherent versions at least up to 4.2, this is required to avoid
 * having the stdio library linked in. Library dependencies in Coherent are
 * totally fucked up.
 */
 
void _finish () { }


/*
 * It used to be that when the shell exec'ed a script it would try and write
 * a (possibly partial) copy of the parameters over the top of the original
 * parameter set, apparently for the benefit of 'ps'.
 *
 * Since it blindly assumed that the original parameter and environment space
 * formed a nice contiguous block of memory for it to screw with, it was
 * totally nonportable in addition to not actually doing anything with the
 * 4.x version of 'ps' anyway.
 *
 * However, I'm leaving the function in, but making it do something different.
 * Whenever a shell script is exec'ed, previous shell versions would allocate
 * a fresh argument and environment vector every time. Now I properly arrange
 * for the old duplicate argument and environment to be freed.
 */

#if	USE_PROTO
LOCAL void fakearg (int flag, int NOTUSED (argc), char ** argv, char ** envp)
#else
LOCAL void
fakearg (flag, argc, argv, envp)
int		flag;
int		argc;
char	     **	argv;
char	     **	envp;
#endif
{
static	int		oldflag;
static	char	     **	oldargv;
static	char	     **	oldenvp;

	if (oldflag) {
		vfree (oldargv, 0);
		vfree (oldenvp, 0);
	}
	oldflag = flag;
	oldargv = argv;
	oldenvp = envp;
}


int
main (argc, argv, envp)
int		argc;
char	     **	argv;
char	     **	envp;
{
	int		i;

	sarg0 = argc > 0 ? argv [0] : "";
	fakearg (0, argc, argv, envp);

	if (argc > 0 && argv [0][0] == '-') {
		lgnflag = 1;
		umask (ufmask = 022);
#if	LOGINHACK
	} else if (argc > 0 && argv [0][0] == '+') {
		lgnflag = 2;
		umask (ufmask = 022);
#endif
	} else
		umask (ufmask = umask (ufmask));

	if (setjmp (restart) != 0) {
		/*
		 * We come here on an ENOEXEC from lower down to see if we
		 * are running a script. We need to check the magic numbers
		 * on the file, because ENOEXEC can happen for good reasons
		 * and we don't want to always start interpreting a binary!
		 */

		extern BUF * bufap;
		fakearg (1, argc = nargc, argv = nargv, envp = nenvp);
	}


	/*
	 * Always reset the flag settings. For top-level exec (), we
	 * need to avoid permitting the previous session having any
	 * unexpected impact on the new session; this should of course
	 * include such things as the interactivity of the session.
	 */

	for (i = 0 ; i < ARRAY_LENGTH (shflags) ; i ++)
		shflags [i].f_value = 0;

	shpid = getpid ();
	initvar (envp, 0);

	if (set (argc, argv, 1))
		return 1;

	if (cflag) {
		if (sargp [0] == NULL) {
			if (shellerr_begin ("command line")) {
				shellerr_outstr ("no string for -c option");
				shellerr_endl ();
			}
			return 1;
		}
		-- sargc;
		session (SARGS, duplstr (* sargp ++, 1));
	} else if (! sflag && ! iflag && sargc != 0) {
static	int	sargdup;

		if (sargdup)
			sfree (sarg0);
		sarg0 = duplstr (* sargp ++, 1);
		-- sargc;
		sargdup = 1;

		session (SFILE, scmdp == NULL ? sarg0 : scmdp);
	} else
		session (SSTR, STDIN_FILENO);

	cleanup_shell_fns ();
	unlink_temp (capture_temp ());
	return slret;
}


/*
 * Create buffer for session, for SSTR and SFILE types, such that:
 *	s_argv [0] points at the start of buffer data, and
 *	s_argv [1] points at the next buffered datum
 */

#if	USE_PROTO
LOCAL int buffer_session (SES * session)
#else
LOCAL int
buffer_session (session)
SES	      *	session;
#endif
{
	char	      *	tmp = salloc (SESBUFSIZ + 2 * sizeof (char *));

	session->s_argv = (char **) tmp;
	tmp += 2 * sizeof (char *);

	session->s_argv [0] = tmp;
	session->s_count = 0;
}


/*
 * Push a session of the indicated type. A non-zero return value is the
 * appropriate return value for the session, but offset by one.
 */

#if	USE_PROTO
int push_session (int type, VOID * info, SES * session)
#else
int
push_session (type, info, session)
int		type;
VOID	      *	info;
SES	      *	session;
#endif
{
	int		scan;

	session->s_bpp = savebuf ();
	session->s_line = 1;

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

		if ((session->s_ifd = open (session->s_strp, O_RDONLY)) < 0) {
			if (shellerr_begin ("cannot open script")) {
				shellerr_outstr (session->s_strp);
				shellerr_endl ();
			}

			return 126 + 1;
		}

		scan = FOPEN_MAX;
		while (-- scan > 3) {
			if (fcntl (scan, F_GETFD) == -1) {
				/*
				 * Move the new FD to the new place.
				 */

				dup2 (session->s_ifd, scan);
				close (session->s_ifd);
				session->s_ifd = scan;
				break;
			}
		}

		session->s_flag = isatty (session->s_ifd) &&
				  isatty (STDERR_FILENO) ? INTERACTIVE : 0;

		if (! isatty (session->s_ifd)) {
			char		buf [2];

			/*
			 * Non-interactive, check to see that it contains
			 * no bogus characters.
			 */

			if (read (session->s_ifd, buf,
				  sizeof (buf)) == sizeof (buf) &&
			    ((iscntrl (buf [0]) && ! isspace (buf [0])) ||
			     (iscntrl (buf [1]) && ! isspace (buf [1])))) {

				close (session->s_ifd);
				if (shellerr_begin ("file not executable")) {
					shellerr_outstr (session->s_strp);
					shellerr_endl ();
				}

				return 126 + 1;
			}

			lseek (session->s_ifd, 0, SEEK_SET);
		}

		fcntl (session->s_ifd, F_SETFD, FD_CLOEXEC);
		buffer_session (session);
		break;

	case SSTR:
		session->s_strp = NULL;
		session->s_ifd = (int) info;
		session->s_flag = isatty (session->s_ifd) &&
				  isatty (STDERR_FILENO) ? INTERACTIVE : 0;
		buffer_session (session);
		break;
	}

	session->s_node = NULL;
	session->s_con = NULL;

	/*
	 * This must be the last thing we do, otherwise an abnormal return
	 * will leave garbage linked in.
	 */

	session->s_next = sesp;
	sesp = session;

	return 0;
}


/*
 * Pop the passed-in session, which must be the current session.
 */

void
pop_session (session)
SES	      *	session;
{
/*	assert (sesp == session); */

	if (session->s_type == SFILE || session->s_type == SSTR) {
		/*
		 * Discard space allocated for buffering after "returning"
		 * unused data by rewinding over it.
		 */

		lseek (session->s_ifd, SEEK_CUR, - session->s_count);
		sfree (session->s_argv);
	}

	if (session->s_type == SFILE)
		close (session->s_ifd);

	if (session->s_next == NULL) {
		sigintr (0);
		recover (IRDY);
	}

	freebuf (session->s_bpp);
	free_node (session->s_node, CLEAN);

	sesp = session->s_next;
}


/*
 * Loop on input.
 */

int
session (t, p)
int t;
char * p;
{
	SES s;
	int rcode;

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

		if ((rcode = setjmp (s.s_envl)) == RSET && s.s_next == NULL) {
			/*
			 * Special-case code for the first time through the
			 * loop; cause the /etc/profile to be run, and maybe
			 * hand control over to the /real/ shell.
			 */

			switch (lgnflag) {
				CONST char    *	file;
			case 1:
				/*
				 * '-' sign invocation.
				 */
				if ((file = ffind ("/etc", "profile",
						   R_OK)) != NULL)
					session (SFILE, duplstr (file, 0));
				recover (IPROF);

				lgnflag = 0;
				if (* vhome && (file = ffind (vhome, ".profile",
							      R_OK)) != NULL)
					session (SFILE, duplstr (file, 0));
				break;
#if	LOGINHACK
			case 2:
				/*
				 * '+' sign invocation, where /bin/login has
				 * run us purely to process /etc/profile on
				 * behalf of the non-Bourne login shells (in
				 * particular, uucico).
				 */

				if ((file = ffind ("/etc", "profile",
						   R_OK)) != NULL)
					session (SFILE, duplstr (file, 0));
				recover (IPROF);
				return exshell (findvar ("SHELL"));
#endif
			}
		}

		/*
		 * If we get here with lgnflag non-zero, at the outermost
		 * session level, then something bad like a user interrupt
		 * happened while the login shell was running /etc/profile, so
		 * we just exit.
		 */

		if (s.s_next == NULL && lgnflag != 0) {
			unlink_temp (capture_temp ());
			exit (1);
		}

		switch (rcode) {
		case RSET:	/* initial setjmp call */
			if (iflag)
				checkmail ();
			comflag = 1;
			errflag = 0;
			recover (IRDY);
			freebuf (s.s_bpp);
			free_node (s.s_node, CLEAN);
			s.s_bpp = savebuf ();
			if (yyparse () != 0)
				syntax ("command syntax bad");
			/* FALL THROUGH */

		case REOF:
			recover (IRDY);
			break;

		case RCMD:
			recover (IRDY);
			s.s_con = NULL;
			command (s.s_node, COMMAND_NORMAL);
			if (tflag && tflag ++ >= 2)
				break;
			continue;

		case RERR:
			recover (IRDY);
			if (! errflag)
				syntax ("(nonspecific)");
			if (! iflag || (tflag && tflag ++ >= 2))
				break;
			continue;

		case RINTDIE:
		case RINT:
		case RUEXITS:
		case RUABORT:
			if (s.s_next != NULL) {
				pop_session (& s);
				reset (rcode);
			}

			if (rcode == RINT)
				prpflag = 2;

			if (rcode == RUEXITS || rcode == RINTDIE || ! iflag ||
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
	return slret;
}


#if	USE_PROTO
NO_RETURN reset (int f)
#else
NO_RETURN
reset (f)
int		f;
#endif
{
	longjmp (sesp->s_envl, f);
}


/*
 * Global head of list of temporary files. Each entry will have a reference
 * count field, but once a set of files is captured only the head of the
 * list will have the count actually looked at.
 */

static TEMP_FILE      *	temp_list;

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
	temp->tf_refs = 1;
	temp_list = temp;
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

	if (templist == NULL || -- templist->tf_refs != 0)
		return;

	do {
		templist = (temp = templist)->tf_next;

		unlink (temp->tf_name);

		sfree (temp->tf_name);
		sfree (temp);
	} while (templist != NULL);
}


/*
 * Get a duplicate set of reference counts for a file.
 */

TEMP_FILE *
dup_temp_files (templist)
TEMP_FILE     *	templist;
{
	templist->tf_refs ++;
	return templist;
}


/*
 * Make a temp file name.
 */

char *
shtmp ()
{
static	char tmpfile [] = "/tmp/shXXXXXXX";
static	int tmpflag = 0;

	tmpflag ++;

	(void) ltoa (tmpfile + 7, sizeof (tmpfile) - 9, shpid, 10, 5);
	tmpfile [7 + 5] = tmpflag % 26 + 'a';
	tmpfile [7 + 6] = (tmpflag / 26) % 26 + 'a';
	return tmpfile;
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

	if ((f = fork ()) == 0)
		abort ();

	waitc (f, WAIT_NORMAL);
#endif
	if (shellerr_begin ("internal assertion failure")) {
		shellerr_outint (i);
		shellerr_endl ();
	}

	reset (RNOWAY);
}


/*
 * error message prologue - print line number and file if
 *	not interactive.
 */

#if	USE_PROTO
LOCAL void print_vicinity (void)
#else
LOCAL void
print_vicinity ()
#endif
{
	if (sesp->s_type == SFILE) {
		shellerr_outstr (sesp->s_strp);
		shellerr_outstr (": in shell script at ");
		if ((sesp->s_flag & SESSION_EOF) != 0)
			shellerr_outstr ("EOF");
		else {
			shellerr_outstr ("line ");
			shellerr_outint (sesp->s_line);
		}
		shellerr_outstr (": ");
	} else
		shellerr_outstr ("sh: ");
}


#if	USE_PROTO
LOCAL void shellerr_os (CONST unsigned char * locus, CONST unsigned char * str)
#else
LOCAL void
shellerr_os (locus, str)
CONST unsigned char   *	locus;
CONST unsigned char   *	str;
#endif
{
	int		error = errno;

	if (shellerr_begin (locus)) {

		shellerr_outstr (str);
		shellerr_outstr (": OS says \"");
		shellerr_outstr (strerror (error));
		shellerr_outchar ('"');
		shellerr_endl ();
	}
}


/*
 * Some familiar errors.
 */

#if	USE_PROTO
void ecantopen (CONST char * file)
#else
void
ecantopen (file)
CONST char    *	file;
#endif
{
	shellerr_os ("cannot open file", file);
}


#if	USE_PROTO
void ecantfind (CONST char * file)
#else
void
ecantfind (file)
CONST char    *	file;
#endif
{
	shellerr_simple ("cannot find file", file);
}


#if	USE_PROTO
void ecantmake (CONST char * file)
#else
void
ecantmake (file)
CONST char    *	file;
#endif
{
	shellerr_os ("cannot create file", file);
}


#if	USE_PROTO
void eillvar (CONST char * var)
#else
void
eillvar (var)
CONST char    *	var;
#endif
{
	shellerr_simple ("illegal variable name", var);
}


#if	USE_PROTO
void eredir (CONST char * desc)
#else
void
eredir (desc)
CONST char    *	desc;
#endif
{
	shellerr_simple ("bad redirection", desc);
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
	SES		s;
	int		oldslret = slret;

	/*
	 * The lex_push () is required, because our attempt to evaluate the
	 * prompt string will involve the lexer, even though we are called
	 * from within the lexer.
	 */

	lex_push ();
	push_session (SARGS, vps, & s);

	for (;;) {
		int		c;
		char	      *	temp;

		/*
		 * Initialize the lexer context.
		 */

		lex_init ();

		for (;;) {
			if ((c = getn ()) < 0)
				break;		/* hit EOF */
			if ((c = do_lex (c, LEX_HERE)) == '\n')
				break;
		}

		lex_done ();

		/*
		 * Now we have read the input, perform EHERE expansions. We
		 * copy the input somewhere else first.
		 */

		temp = eval_buf (CSTRING_STRING (sh_yytext),
				 CSTRING_LENGTH (sh_yytext), EHERE);

		shell_outstr (temp);

		if (c < 0)
			break;
	}

	pop_session (& s);
	lex_pop ();

	slret = oldslret;
}


/*
 * Syntax error message - print line number and file if
 *	not interactive.
 */

void
syntax (desc)
CONST char    *	desc;
{
	if (shellerr_begin ("syntax error")) {

		shellerr_outstr (desc);
		shellerr_endl ();
	}
}
