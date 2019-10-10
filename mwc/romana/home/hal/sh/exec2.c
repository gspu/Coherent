/*
 * Bourne shell.
 * System part of execution.
 */

#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "shellio.h"
#include "sh.h"

/*
 * Wait for the given process to complete. If "intflag" is true, then we can
 * be interrupted. Unless we are waiting interruptibly, don't fiddle with the
 * signal dispositions!
 */

#if	USE_PROTO
int waitc (int pid, int intflag)
#else
int
waitc (pid, intflag)
int		pid;
int		intflag;
#endif
{
	unsigned s;
	int w, n;

/*	assert (intflag == WAIT_INTERRUPTIBLE ? pid >= 0 : pid > 0); */

	dflttrp (intflag == WAIT_INTERRUPTIBLE ? IWAITINT : IWAIT);

	for (;;) {
		if ((w = waitpid (-1, & s, WUNTRACED)) >= 0) {
			if (WIFSIGNALED (s) &&
			    (n = WTERMSIG (s)) != SIGINT) {

				if (w != pid) {
					shell_outint (w);
					shell_outstr (": ");
				}
				if (n == SIGSYS && ! __WCOREDUMP (s))
					shell_outstr ("exec failed");
				else if (signame (n) != NULL)
					shell_outstr (signame (n));
				else {
					shell_outstr ("status");
					shell_outint (n);
				}
				if (__WCOREDUMP (s))
					shell_outstr (" -- core dumped");
				shell_endl ();
				s = 200 + n;
			} else if (WIFSIGNALED (s))
				s = 200 + SIGINT;
			else
				s = WEXITSTATUS (s);
			if (w == pid) {
				slret = s;
				break;
			}
		} else if (errno == EINTR) {
			if (intflag) {
				slret = 0;
				break;
			}
		} else if (errno == ECHILD) {
			if (pid == 0)	
				slret = 0;
			else
				slret = ECHILD;
			break;
		} else
			panic (6);
	}

	dflttrp (IDONE);
	return slret;
}

/*
 * Make an imperfect copy of ourself.
 */

#if	USE_PROTO
int clone (void)
#else
int
clone ()
#endif
{
	int f;
	SES *sp;

	dflttrp (IPARENT);

	if ((f = fork ()) < 0) {
		shellerr_simple ("cannot fork", "too many processes");
		reset (RSYSER);
	} else if (f == 0) {
		/*
		 * Child process.
		 */

		sflag = iflag = cflag = no1flag = lgnflag = 0;
		slret = 0;
		sp = sesp;

		if (sp->s_con != NULL)
			sp->s_con->c_next = NULL;

		/*
		 * Originally this function suppressed function definitions
		 * in child processes. This is wrong. Note that we have to
		 * detach the temporary-file stuff from the child functions,
		 * however, so that functions with here-documents work (as
		 * long as the top-level shell is alive, that is).
		 */

		subshell_shell_fns ();

		while (sp) {
			if (sp->s_type == SFILE)
				close (sp->s_ifd);
			sp = sp->s_next;
		}
		dflttrp (IFORK);
	}
	return f;
}


/*
 * Open a pipe, panic on failure.
 * Otherwise return as a clone.
 */

int
pipeline(pv)
int *pv;
{
	if (pipe (pv) < 0) {
		shellerr_simple ("cannot create pipeline",
				 strerror (errno));

		reset (RSYSER);
	}
	return clone ();
}

/*
 * Try to execute a file in several ways.
 *	A return is always an error.
 *	Used by exec in inline.
 */

#if	USE_PROTO
int flexec (void)
#else
int
flexec ()
#endif
{
	CONST char    *	file;

	ffind (NULL, NULL, 0);
	while ((file = ffind (vpath, * nargv, X_OK)) != NULL) {
		execve (file, nargv, nenvp);

		if (errno == ENOEXEC) {
			/*
			 * The nargc [] array has a -1'th element we back up
			 * over. This means that the argv [] array that we
			 * throw to main () has the command we want to run
			 * appearing as the first argument to the shell, and
			 * set () can't confuse arguments to the command with
			 * arguments to the shell.
			 */

			if (scmdp != NULL)
				sfree (scmdp);
			scmdp = duplstr (file, 1);

			nargc += 1;
			nargv = vdupl (nargv - 1, 0);
			nenvp = vdupl (nenvp, 0);

			while (sesp != NULL)
				pop_session (sesp);

			longjmp (restart, 1);
		}
		if (errno == E2BIG) {
			shellerr_simple ("file too big to execute",
					 nargv [0]);
			return -1;
		}
	}
	ecantfind (nargv [0]);
	return -1;
}


/*
 * Here we deal with the effects of redirection on an open session by trying
 * to "put back" buffered characters using lseek (). If the case of sessions
 * attached to interactive terminals, the lseek () will do nothing, which is
 * about what we really want.
 *
 * Let's be general about it, and make this function handle the low-level
 * details of redirection for both redirect () and redir_undo (), too.
 */

#if	USE_PROTO
void lowlevel_redir (int from, int to)
#else
void
lowlevel_redir (from, to)
int		from;
int		to;
#endif
{
	SES	      *	scan;

	for (scan = sesp ; scan != NULL ; scan = scan->s_next) {
		if ((scan->s_type == SFILE || scan->s_type == SSTR) &&
		    scan->s_ifd == to) {
		    	/*
		    	 * Discard buffered input, and reset the "interactive"
		    	 * setting for the session.
		    	 */

		    	lseek (to, SEEK_CUR, - scan->s_count);
		    	if (isatty (from) && isatty (STDERR_FILENO))
		    		scan->s_flag |= INTERACTIVE;
		    	else
		    		scan->s_flag &= ~ INTERACTIVE;

		    	/*
		    	 * There can only be one match...
		    	 */

		    	break;
		}
	}

	if (from != to)
		dup2 (from, to);
}


/*
 * Create an undo node for a redirection of a file descriptor over the
 * current state of file descriptor "save".
 */

#if	USE_PROTO
REDIR_UNDO * remember_undo (REDIR_UNDO * next, int save)
#else
REDIR_UNDO *
remember_undo (next, save)
REDIR_UNDO    *	next;
int		save;
#endif
{
	REDIR_UNDO    *	undo_node;

	/*
	 * Create and link in the undo node now. Stash away a spare copy of
	 * the original fd . Note that this may cause semantic changes in
	 * attempts to redirect from fds that would otherwise be closed,
	 * but that error was never diagnosed before anyway...
	 *
	 * Note that salloc () never returns NULL.
	 */

	undo_node = (REDIR_UNDO *) salloc (sizeof (REDIR_UNDO));

	undo_node->ru_next = next;
	undo_node->ru_oldfd = save;
	if ((undo_node->ru_newfd = dup (save)) == -1) {
		if (errno != EBADF) {
			shellerr_simple ("cannot redirect",
					 "out of file descriptors");

			reset (RUABORT);
		}
	} else {
		fcntl (undo_node->ru_newfd, F_SETFD,
		       fcntl (undo_node->ru_newfd, F_GETFD, 0) | FD_CLOEXEC);
	}

	return undo_node;
}


/*
 * Process a redirection vector.
 * Abort and return -1 at the first failure, return 0 for success.
 *
 * NB: In ordr to support redirection of builtins, an extra argument to this
 * function has been addded. If undo is NULL, things remain as before, but if
 * non-NULL it is taken to be the head of a list of undo items. As the list of
 * redirections is processed, undo entries will be added to the head of the
 * list, so that the caller will see the list in the appropriate order for
 * undoing the redirections.
 */

enum redir_op {
	REDIR_OUT,
	REDIR_OUT_CLOBBER,
	REDIR_OUT_APPEND,
	REDIR_IN,
	REDIR_IN_HERE,
	REDIR_IN_OUT
};

int
redirect(iovp, undo)
char **iovp;
REDIR_UNDO ** undo;
{
	char	     **	iopp;
	char	      *	io;
	enum redir_op	op;
	int		u1;
	int		u2;

	for (iopp = iovp ; (io = * iopp ++) != NULL; ) {

		if (class (* io, MDIGI))
			u1 = * io ++ - '0';
		else
			u1 = * io == '<' ? 0 : 1;
		if (undo)
			* undo = remember_undo (* undo, u1);

		switch (* io) {
		case '>':
			switch (* ++ io) {
			case '>':
				op = REDIR_OUT_APPEND;
				u2 = O_CREAT | O_WRONLY | O_APPEND;
				io ++;
				break;

			case '|':
				op = REDIR_OUT_CLOBBER;
				io ++;
				u2 = O_CREAT | O_TRUNC | O_WRONLY;
				break;

			case '&':
duplicate:
				io ++;
				u2 = * io ++;
				if (u2 == '-') {
					close (u1);
					continue;
				} else if (! class (u2, MDIGI)) {
					eredir ("invalid descriptor");
					return -1;
				}

				lowlevel_redir (u2 -= '0', u1);
				continue;

			default:
				op = REDIR_OUT;
				u2 = noclobber_flag ?
					O_WRONLY | O_CREAT | O_EXCL :
					O_WRONLY | O_CREAT | O_TRUNC;
				break;
			}
			break;

		case '<':
			switch (* ++ io) {
			case '<':
				op = REDIR_IN_HERE;
				u2 = O_RDONLY;
				io ++;
				break;

			case '&':
				goto duplicate;

			case '>':
				op = REDIR_IN_OUT;
				io ++;
				u2 = O_RDWR | O_CREAT | O_TRUNC;
				break;

			default:
				op = REDIR_IN;
				u2 = O_RDONLY;
				break;
			}
			break;

		default:
			panic (8);
		}
		while (* io == ' ' || * io == '\t')
			io += 1;

		while ((u2 = open (io, u2, S_IRUSR | S_IWUSR | S_IRGRP |
					   S_IWGRP | S_IROTH | S_IWOTH)) < 0) {
			struct stat	buf;
			/*
			 * Permit opening over a file in noclobber mode iff it
			 * is not a regular file.
			 */

			if (errno == EEXIST && op == REDIR_OUT &&
			    noclobber_flag && stat (io, & buf) == 0 &&
			    ! S_ISREG (buf.st_mode)) {
			    	u2 = O_WRONLY;
			    	continue;
			}
			ecantopen (io);
			return -1;
		}

		if (op == REDIR_IN_HERE && (u2 = evalhere (u2)) < 0)
			return -1;

		lowlevel_redir (u2, u1);

		/*
		 * If the file was already in the right place, don't close
		 * the temporary descriptor!
		 */

		if (u2 != u1)
			close (u2);
	}

	return 0;
}


/*
 * Undo a redirection sequence, reclaiming all the space for the undo nodes.
 */

#if	USE_PROTO
void redirundo (REDIR_UNDO * undo)
#else
void
redirundo (undo)
REDIR_UNDO * undo;
#endif
{
	while (undo != NULL) {
		REDIR_UNDO    *	undo_next = undo->ru_next;

		if (undo->ru_newfd == -1)
			close (undo->ru_oldfd);
		else {
			lowlevel_redir (undo->ru_newfd, undo->ru_oldfd);
			close (undo->ru_newfd);
		}

		sfree (undo);

		undo = undo_next;
	}
}


/*
 * Search a path, perhaps repeatedly, for a file with access mode.
 * Is called with paths == NULL to reset pointers.
 */

#if	USE_PROTO
CONST char * ffind (CONST char * paths, CONST char * file, int mode)
#else
CONST char *
ffind (paths, file, mode)
CONST char    *	paths;
CONST char    *	file;
int		mode;
#endif
{
	char c;
static	CONST char    *	ffile;
static	CONST char    *	fpath;

	if (paths == NULL) {
		ffile = fpath = NULL;
		return NULL;
	}

	if (ffile != file) {
		fpath = paths;
		ffile = file;
		if (strchr (ffile, '/') != NULL)
			fpath = "";
	}

	do {
		lex_init ();

		c = 0;
		while (* fpath && * fpath != ':') {
			c = 1;
			lex_add (* fpath ++);
		}

		if (c != 0)
			lex_add ('/');

		c = * fpath ++;

		lex_add_string (ffile);
		lex_add (0);

		if (access (file = lex_done (), mode) >= 0)
			return file;
	} while (c == ':');

	return NULL;
}


#if	LOGINHACK

#include <stdio.h>

/*
 * Execute a non-standard shell.
 */

exshell (vp)
VAR *vp;
{
	char *vshell;
	register char *p;

	vshell = vp->v_strp;
	while (* vshell && * vshell ++ != '=')
		/* DO NOTHING */ ;

	/* Construct -name argv[0] */
	if ((p = strrchr (vshell, '/')) != NULL)
		p += 1;
	else
		p = vshell;

	lex_init ();
	lex_add ('-');
	lex_add_string (p);

	nargv = makargl ();

	nargv = addargl (nargv, "sh", 0);
	nargv = addargl (nargv, lex_done (), 0);
	nargc = 2;

	nenvp = makargl ();
	nenvp = envlvar (nenvp);

	/*
	 * Try exec - If the "shell" is a real executable file, present it
	 * with the base part of its name as the only argument.
	 */

	execve (vshell, nargv + 1, nenvp);

	if (errno == ENOEXEC) {
		/*
		 * Otherwise, run a script; in this case, set 'sh' as argv [0]
		 * and the base-name of the script as argv [1].
		 */

		fakearg (1, nargc, nargv, nenvp);
		sargc = 0;
		sargp = nargv + 2;
		sarg0 = nargv [1];
		return session (SFILE, vshell);
	}
	fprintf (stderr, "No shell: %s\n", vshell);
	exit (1);
}
#endif


/*
 * Check to see if we have mail.
 */

void
checkmail ()
{
	static long mailsize = -1;
	struct stat sbuf;

	if (* vmail == '\0')
		return;

	if (stat (vmail, & sbuf) < 0) {
		mailsize = 0;
	} else {
		if (sbuf.st_size != 0 && sbuf.st_size > mailsize) {
			shell_outstr (mailsize == -1 ?
					"You have mail." :
					"You have new mail.");
			shell_endl ();
		}
		mailsize = sbuf.st_size;
	}
}
