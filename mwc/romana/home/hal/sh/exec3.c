/*
 * Bourne shell.
 * Builtin commands and shell functions.
 */

#include <common/tricks.h>
#include <sys/times.h>
#include <ulimit.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>

#include "shellio.h"
#include "sh.h"

#define HOUR	(60L * 60L * CLK_TCK)
#define MINUTE	(60L * CLK_TCK)
#define SECOND	CLK_TCK

/*
 * NIGEL: Encapsulate the behaviour of functions and '.' in saving and
 * restoring the $# and $* information.
 */

struct arginfo {
	int		a_argc;		/* $# */
	char	      *	a_arg0;		/* $0 */
	char	     **	a_argp;		/* arguments $1 .. $n */
	char	     **	a_original_argp;
};

/*
 * We keep the following copy of sargp (what the hell does that stand for,
 * anyway?) around so that "shift" and other such things can freely modify
 * the original.
 */

static	char	     **	original_sargp;

#if	USE_PROTO
LOCAL void push_arginfo (struct arginfo * save, int argc, char * arg0,
			 char * CONST * argv)
#else
LOCAL void
push_arginfo (save, argc, arg0, argv)
struct arginfo
	     **	save;
int		argc;
char	      *	arg0;
char  * CONST *	argv;
#endif
{
	if (save != NULL) {
		save->a_argc = sargc;
		save->a_arg0 = sarg0;
		save->a_argp = sargp;
		save->a_original_argp = original_sargp;
	} else if (original_sargp != NULL)
		vfree (original_sargp, 1);

	sargc = argc;
	sarg0 = arg0;
	original_sargp = vdupl (argv, 1);
	original_sargp [0] = arg0;
	sargp = original_sargp + 1;
}


#if	USE_PROTO
LOCAL void pop_arginfo (struct arginfo * save)
#else
LOCAL void
pop_arginfo (save)
struct arginfo * save;
#endif
{
	if (original_sargp != NULL)
		vfree (original_sargp, 1);

	sargc = save->a_argc;
	sarg0 = save->a_arg0;
	sargp = save->a_argp;
	original_sargp = save->a_original_argp;
}


/*
 * Actual builtin functions.
 */

#if	USE_PROTO
LOCAL int s_colon (void)
#else
LOCAL int
s_colon ()
#endif
{
	return 0;
}


#if	USE_PROTO
LOCAL int s_command (void)
#else
LOCAL int
s_command ()
#endif
{
	int		retval;
	int		i;

	if (nargc < 2) {
		if (cmderr_begin ("command")) {
			cmderr_outstr ("usage: command [-p] <command_name> "
				       "[<argument> ...]");
			cmderr_endl ();
		}

		slret = 1;
		return 2;
	}

	retval = strcmp (nargv [1], "-p") == 0 ? -2 : -1;

	/*
	 * Move things down so the new command name is in the right place.
	 */

	for (i = 0 ; i <= nargc + retval ; i ++)
		nargv [i] = nargv [i - retval];
	nargc += retval;

	return retval;
}


#if	USE_PROTO
LOCAL int s_dot (void)
#else
LOCAL int
s_dot ()
#endif
{
	int		retval;
	struct arginfo	info;
	CONST char    *	file;

	if (nargc == 1)
		return 0;
		
	ffind (NULL, NULL, 0);
	if ((file = ffind (vpath, nargv [1], R_OK)) == NULL) {
		ecantfind (nargv [1]);
		return 1;
	}

	/*
	 * NIGEL: Create arguments for the '.'-script
	 */

	push_arginfo (& info, nargc - 2, nargv [1], nargv + 2);

	retval = session (SFILE, duplstr (file, 0));

	pop_arginfo (& info);

	return retval;
}


#if	USE_PROTO
LOCAL int s_break (void)
#else
LOCAL int
s_break ()
#endif
{
	CON * cp;
	int n;

	n = nargc > 1 ? strtol (nargv [1], NULL, 0) : 1;

	for (cp = sesp->s_con; cp; cp = cp->c_next) {
		switch (cp->c_node->n_type) {
		case NWHILE:
		case NFOR:
		case NUNTIL:
			if (-- n == 0) {
				sesp->s_con = cp;
				longjmp (cp->c_envl,
					 nargv [0][0] == 'b' ? 2 : 1);
				break;
			}
			break;

		default:
			break;
		}

		DESTROY_CON (cp);
	}

	if (cmderr_begin (nargv [0])) {
		cmderr_outstr ("no matching control structure");
		cmderr_endl ();
	}

	return 255;
}


/*
 * Change to given directory.
 * Update global variable CWD accordingly.
 * Return NULL if bad, otherwise full pathname of the directory.
 */

#if	USE_PROTO
LOCAL CONST char * cd (CONST char * name, CONST char * dir)
#else
LOCAL CONST char *
cd (name, dir)
CONST char    *	name;
CONST char    *	dir;
#endif
{
	char		temp [PATH_MAX];

	if (chdir (dir) < 0) {
		if (cmderr_begin (name)) {
			cmderr_outstr ("cannot change to directory \"");
			cmderr_outstr (dir);
			cmderr_outchar ('"');
			cmderr_endl ();
		}

		return NULL;
	}

#if	DIRECTORY_STACK || MAINTAIN_CWD
	if (* dir != '/') {
		/*
		 * Find an absolute pathname for the dstack and $CWD.
		 * Avoid _getwd () in this case, it can undo the chdir () above.
		 */

		if ((dir = getcwd (temp, sizeof (temp))) == NULL)
			return NULL;
	}
#if	MAINTAIN_CWD
	assnvar (sizeof ("CWD") - 1, "CWD", dir);
#endif
#endif
	return dir;
}


#if	DIRECTORY_STACK

char	      *	dstack [DSTACKN];	/* Directory stack */
int		dstkp;			/* Directory stack pointer */

/*
 * Pop the directory stack and change to the previous stacked directory.
 */

#if	USE_PROTO
LOCAL int popd (void)
#else
LOCAL int
popd ()
#endif
{
	if (dstkp == 0) {
		if (cmderr_begin ("popd")) {
			cmderr_outstr ("directory stack underflow");
			cmderr_endl ();
		}
		return -1;
	}
	if (dstack [dstkp] != NULL)
		sfree (dstack [dstkp]);
	return cd ("popd", dstack [-- dstkp]) == NULL ? -1 : 0;
}


/*
 * Change to given directory and add it to the directory stack.
 */

#if	USE_PROTO
LOCAL int pushd (CONST char * dir)
#else
LOCAL int
pushd (dir)
CONST char    *	dir;
#endif
{
	if ((dir = cd ("pushd", dir)) == NULL)
		return -1;			/* cd failed */

	if (++ dstkp >= DSTACKN) {
		-- dstkp;
		if (cmderr_begin ("pushd")) {
			cmderr_outstr ("directory stack overflow");
			cmderr_endl ();
		}

		return -1;
	}
	dstack [dstkp] = duplstr (dir, 1);
	return 0;
}


#if	USE_PROTO
LOCAL int s_dirs (void)
#else
LOCAL int
s_dirs ()
#endif
{
	int		i;

	for (i = dstkp ; i >= 0; i --) {
		cmd_outstr (dstack [i]);
		cmd_outchar (' ');
	}
	cmd_endl ();
}


#if	USE_PROTO
LOCAL int s_popd (void)
#else
LOCAL int
s_popd ()
#endif
{
	register int i, j, n, ret;

	if (nargc == 1)
		return popd ();
	/*
	 * Kludge to pop one or more specific dir stack elements.
	 * Do args backwards so e.g. "popd 2 3 4" works as expected.
	 * Internal indices [0, dstkp] are user indices [dstkp, 0].
	 */

	for (ret = 0, i = nargc - 1; i > 0; i --) {
		if ((n = atoi (nargv [i], NULL, 0)) == 0)
			ret |= popd ();
		else if (n <= 0 || n > dstkp) {
			if (cmderr_begin ("popd")) {
				cmderr_outstr ("argument \"");
				cmderr_outint (n);
				cmderr_outstr ("\" out of range");
				cmderr_endl ();
			}

			ret = -1;
			continue;
		} else {
			j = dstkp - n;
			if (dstack [j] != NULL)
				sfree (dstack [j]);
			for ( ; j < dstkp; j ++)
				dstack [j] = dstack [j + 1];
			-- dstkp;
		}
	}
	return ret;
}


#if	USE_PROTO
LOCAL int s_pushd (void)
#else
LOCAL int
s_pushd ()
#endif
{
	char * dir;
	int i, ret;

	if (nargc == 1) {
		/* Exchange top two stack elements. */
		if (dstkp == 0)
			return 1;		/* only one element on stack */
		dir = dstack [dstkp - 1];
		dstack [dstkp - 1] = dstack [dstkp];
		dstack [dstkp] = dir;		/* exchange top two */
		return cd ("pushd", dir) == NULL ? -1 : 0;	/* and cd accordingly */
	}

	/* Push one or more directories to stack. */
	for (ret = 0, i = 1; i < nargc; i ++)
		ret |= pushd (nargv [i]);
	return ret;
}

#endif	/* DIRECTORY_STACK */


#if	USE_PROTO
LOCAL int s_cd (void)
#else
LOCAL int
s_cd ()
#endif
{
	CONST char * dir;

	if ((dir = cd ("cd", nargc < 2 ? vhome : nargv [1])) == NULL)
		return -1;			/* cd failed */

#if	DIRECTORY_STACK
	if (dstack [dstkp] != NULL)
		sfree (dstack [dstkp]);
	dstack [dstkp] = duplstr (dir, 1);	/* update dir stack */
#endif
	return 0;
}



#if	USE_PROTO
LOCAL int s_echo (void)
#else
LOCAL int
s_echo ()
#endif
{
	int		nl = 1;

	if (nargc > 1 && strcmp (nargv [1], "-n") == 0) {
		nl = 0;
		nargv ++;
	}
	nargv ++;

	while (* nargv != NULL) {
		char * s = * nargv;
		int	c;
		while ((c = * s ++) != '\0') {
			if (c != '\\') {
				cmd_outchar (c);
				continue;
			}
			switch (c = * s ++) {
			case 'b': c = '\b'; break;
			case 'c': nl = 0; continue;
			case 'f': c = '\f'; break;
			case 'n': c = '\n'; break;
			case 'r': c = '\r'; break;
			case 't': c = '\t'; break;
			case 'v': c = '\v'; break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
				c = c - '0';
				if (* s >= '0' && * s <= '7')
					c = 8 * c + * s ++ - '0';
				if (* s >= '0' && * s <= '7')
					c = 8 * c + * s ++ - '0';
				break;
			case '\\':
				break;
			default:
				cmd_outchar ('\\');
			}
			cmd_outchar (c);
		}
		if (* ++ nargv != NULL)
			cmd_outchar (' ');
	}
	if (nl)
		cmd_endl ();

	return 0;
}


#if	USE_PROTO
LOCAL int s_eval (void)
#else
LOCAL int
s_eval ()
#endif
{
	return nargc > 1 ? session (SARGV, ++ nargv) : 0;
}


#if	USE_PROTO
LOCAL int s_exec (void)
#else
LOCAL int
s_exec ()
#endif
{
	if (redirect (niovp, NULL) < 0) {
		if (nargc > 1)
			exit (1);
		return 1;
	}

	if (nargc == 1)
		return 0;

	if (no1flag) {
		cleanup_shell_fns ();
		unlink_temp (capture_temp ());
	}

	dflttrp (ICMD);
	++ nargv;
	-- nargc;
	nenvp = envlvar (nenvp);
	flexec ();
	exit (1);
}


#if	USE_PROTO
LOCAL int s_exit (void)
#else
LOCAL int
s_exit ()
#endif
{
	if (nargc > 1)
		slret = strtol (nargv [1], NULL, 0);
	reset (RUEXITS);
}


#if	USE_PROTO
LOCAL int s_export (void)
#else
LOCAL int
s_export ()
#endif
{
	int flag;
	char ** varv;

	flag = nargv [0][0] == 'e' ? VAR_EXPORT : VAR_READONLY;

	/*
	 * NIGEL: Permit POSIX.2 "-p" flag.
	 */

	if (nargc < 2)
		tellvar (flag, 0);
	else if (nargc == 2 && strcmp (nargv [1], "-p") == 0)
		tellvar (flag, 1);
	else {
		varv = ++ nargv;
		while (* varv) {
			if (namevar (* varv))
				flagvar (* varv ++, flag);
			else
				eillvar (* varv ++);
		}
	}
	return 0;
}


#if	USE_PROTO
LOCAL int s_false (void)
#else
LOCAL int
s_false ()
#endif
{
	return 1;
}


#if	USE_PROTO
LOCAL int s_getopts (void)
#else
LOCAL int
s_getopts ()
#endif
{
	VAR	      *	vp;
	CONST char    *	cp;
	int		i;
	char		buf [10];

	if (nargc < 3) {
		if (cmderr_begin ("getopt")) {
			cmderr_outstr ("usage: getopt <optstring> <name> [<arg> ...]");
			cmderr_endl ();
		}
		return 1;
	}
	if ((vp = findvar ("OPTIND")) != NULL &&
	    (cp = convvar (vp)) != NULL &&
	    (i = strtol (cp, NULL, 0)) > 0)
	    	optind = i;

	cp = nargv [2];
	
	if (nargc > 3) {
		nargv [2] = sarg0;
		i = getopt (nargc - 2, nargv + 2, nargv [1]);
	} else {
		/*
		 * Use the fact that push_arginfo () creates a spare slot in
		 * sargp [] for sarg0 to pass the outer command name into
		 * getopt ().
		 */

		sargp [- 1] = sarg0;
		i = getopt (sargc + 1, sargp - 1, nargv [1]);
	}

	if (optarg != NULL)
		assnvar (sizeof ("OPTARG") - 1, "OPTARG",
			 optarg);
	else
		unsetvar ("OPTARG");

	switch (i) {

	case -1:
		break;

	case '-':
		/*
		 * Support long-options by making $OPTOPT the index of the
		 * long option in the option-string.
		 */

		if (nargv [0][0] == 'l') {
			assnvar (sizeof ("OPTOPT") - 1, "OPTOPT",
				 itoa (buf, sizeof (buf), optopt, 10, 0));
			break;
		}
		/* FALL THROUGH */

	default:
		if (nargv [0][0] == 'l')
			unsetvar ("OPTOPT");
		buf [0] = i;
		buf [1] = 0;
		assnvar (strlen (cp), cp, buf);
	}

	assnvar (sizeof ("OPTIND") - 1, "OPTIND",
		 itoa (buf, sizeof (buf), optind, 10, 0));

	return i == -1;
}


#if	USE_PROTO
LOCAL int s_login (void)
#else
LOCAL int
s_login ()
#endif
{
	char * cmd;

	cmd = nargv [0][0] == 'l' ? "/bin/login" : "/bin/newgrp";
	execve (cmd, nargv, envlvar (nenvp));
	ecantfind (cmd);
	return 1;
}


/*
 * Built-in version of 'pwd'.
 */

#if	USE_PROTO
LOCAL int s_pwd (void)
#else
LOCAL int
s_pwd ()
#endif
{
	char		temp [PATH_MAX];

	if (getcwd (temp, sizeof (temp)) != NULL) {
		cmd_outstr (temp);
		cmd_endl ();
	}

	return 0;
}


/*
 * Helper function for s_read ().
 */

#define	READ_EOF	-1

#if	USE_PROTO
LOCAL int read_getc (void)
#else
LOCAL int
read_getc ()
#endif
{
	unsigned char	ch;

	if (read (STDIN_FILENO, & ch, 1) == 1)
		return ch;
	return EOF;
}


/*
 * NB: Originally, this code used yylex () to break the input up into words.
 * This was a bad idea, because this caused the IFS variable to have no
 * useful effect. Now we just read from the input directly, because the
 * extra machinery for session-management doesn't seem to buy anything.
 */

#if	USE_PROTO
LOCAL int s_read (void)
#else
LOCAL int
s_read ()
#endif
{
	char	     **	argp;
	char	     **	arg_end = nargv + nargc;
	CONST char    *	delimiters;
	int		ch;
	int		no_backslash = 0;
	enum	{
		EAT_WS,
		WORD,
		AT_END
	} state = EAT_WS;

	while ((ch = getopt (nargc, nargv, "r")) != EOF) {
		switch (ch) {
		case 'r':
			no_backslash = 1;
			break;

		default:
			if (cmderr_begin ("read")) {
				cmderr_outstr ("usage: read [-r] [<variable> ...]");
				cmderr_endl ();
			}
			return 1;
		}
	}

	argp = nargv + optind;
	while (argp < arg_end)
		if (! namevar (* argp ++))
			eillvar (* (argp - 1));

	delimiters = vifs == NULL ? " \t" : vifs;

	argp = nargv + optind;

	while (argp < arg_end)  {
		lex_init ();

		lex_add_string (* argp ++);
		lex_add ('=');

		if (state != AT_END) {
			state = EAT_WS;

			while ((ch = read_getc ()) != EOF &&
			       ch != '\n') {

				if (! no_backslash && ch == '\\') {
					if ((ch = read_getc ()) == EOF)
						break;
				} else if (strchr (delimiters, ch) != NULL) {

					if (state == EAT_WS)
						continue;
					break;
				}


				if (argp == arg_end)
					delimiters = "";
				lex_add (ch);
				state = WORD;
			}

			if (ch == EOF && state == EAT_WS) {
				lex_done ();
				return 1;
			} else if (ch == '\n')
				state = AT_END;
		}

		lex_add (0);

		setsvar (lex_done ());
	}

	return 0;
}


/*
 * The set command.  This is also called from `main' to set
 * options from the command line.  In this case `flag' is
 * true.
 */

#if	USE_PROTO
int set (int argc, char * CONST * argv, int flag)
#else
int
set (argc, argv, flag)
int		argc;
char * CONST  *	argv;
int		flag;
#endif
{
	int		n;
	int		enable;

	n = 0;

	if (argc > 0)
		n = 1;

	if (argc > 1 &&
	    ((enable = argv [1][0] == '-') != 0 || argv [1][0] == '+')) {
		struct flag   *	flagp;
		char	      *	cp;

		n ++;

		if (strcmp (argv [1], "--") == 0)
			goto setargs;

		for (cp = & argv [1][1] ; * cp ; cp ++) {
			for (flagp = shflags ;
			     flagp < shflags + __ARRAY_LENGTH (shflags) ;
			     flagp ++) {

			     	if (flagp->f_char != * cp)
			     		continue;
			     	if (flagp->f_startup && flag == 0)
			     		break;

			     	if (flagp->f_obsolete) {
					cmderr_outstr ("Warning : option '");
					cmderr_outchar (* cp);
					cmderr_outstr (" is obsolete\n");
				}

			     	flagp->f_value = enable;
			     	flagp = NULL;
			     	break;
			}
			if (flagp) {
				if (cmderr_begin ("set")) {
					cmderr_outstr ("bad option '");
					cmderr_outchar (* cp);
					cmderr_outchar ('\'');
					cmderr_endl ();
				}
		     		break;
		     	}
		}
		if (cp == & argv [1][1]) {
			vflag = 0;
			xflag = 0;
		}
		if (flag == 0 && argc == 2)
			return errflag;
	}
	if (errflag)
		return 1;

setargs:
	push_arginfo (NULL, argc - n, sarg0, argv + n);
	return 0;
}


#if	USE_PROTO
LOCAL int s_set (void)
#else
LOCAL int
s_set ()
#endif
{
	if (nargc < 2) {
		tellvar (0, 1);
		return 0;
	}
	return set (nargc, nargv, 0);
}


#if	USE_PROTO
LOCAL int s_shift (void)
#else
LOCAL int
s_shift ()
#endif
{
	int n;

	n = nargc > 1 ? strtol (nargv [1], NULL, 0) : 1;

	if (n > sargc)
		n = sargc;
	if (n > 0) {
		sargc -= n;
		sargp += n;
	}
	return n != 0;
}


/*
 * print the time as XmX.Xs
 * as fixed by Randall.
 */

#if	USE_PROTO
LOCAL void ptime (time_t t)
#else
LOCAL void
ptime (t)
time_t		t;
#endif
{
	int ticks, tenths, seconds;

	ticks = t % MINUTE;
	seconds = ticks / SECOND;
	tenths = (ticks % SECOND + SECOND / 20) / (SECOND / 10);
	if (tenths == 10) {
		tenths = 0;
		seconds ++;
	}

	cmd_outlong (t / MINUTE);
	cmd_outchar ('m');
	cmd_outint (seconds);
	cmd_outchar ('.');
	cmd_outint (tenths);
	cmd_outchar ('s');
}


#if	USE_PROTO
LOCAL int s_times (void)
#else
LOCAL int
s_times ()
#endif
{
	struct	tms	tb;

	times (& tb);
	ptime (tb.tms_cutime);
	cmd_outchar (' ');
	ptime (tb.tms_cstime);
	cmd_endl ();

	return 0;
}


#if	USE_PROTO
LOCAL int s_trap (void)
#else
LOCAL int
s_trap ()
#endif
{
	char ** vp;
	char * cp;
	int err;

	if (nargc == 1)
		return telltrp ();
	vp = ++ nargv;
	cp = * vp;

	if (class (cp [0], MDIGI) &&
	    (cp [1] == 0 || (class (cp [1], MDIGI) && cp [2] == 0)))
		cp = NULL;
	else
		++ vp;

	err = 0;
	while (* vp) {
		if (class (vp [0][0], MDIGI)) {
			err |= setstrp (strtol (* vp ++, NULL, 0), cp);
			continue;
		}

		if (cmderr_begin ("trap")) {
			cmderr_outstr ("bad trap \"");
			cmderr_outstr (* vp ++);
			cmderr_outchar ('"');
			cmderr_endl ();
	
			err |= 1;
		}
	}
	return err;
}


#if	USE_PROTO
LOCAL int s_ulimit (void)
#else
LOCAL int
s_ulimit ()
#endif
{
	long		newlimit;

	if (nargc < 2) {
		cmd_outlong (ulimit (UL_GETFSIZE));
		cmd_endl ();
	} else if ((newlimit = atol (nargv [1])) > 0)
		ulimit (UL_SETFSIZE, newlimit);
	return 0;
}


#if	USE_PROTO
LOCAL int s_umask (void)
#else
LOCAL int
s_umask ()
#endif
{
	if (nargc < 2) {
		char		buf [4];

		buf [0] = ((ufmask >> 6) & 7) + '0';
		buf [1] = ((ufmask >> 3) & 7) + '0';
		buf [2] = (ufmask & 7) + '0';
		buf [3] = 0;

		cmd_outstr (buf);
		cmd_endl ();
	} else
		umask (ufmask = strtol (nargv [1], NULL, 0));
	return 0;
}


#if	USE_PROTO
LOCAL int s_wait (void)
#else
LOCAL int
s_wait ()
#endif
{
	int f;

	f = nargc > 1 ? strtol (nargv [1], NULL, 0) : 0;
	if (f >= 0)
		waitc (f, WAIT_INTERRUPTIBLE);
	return slret;
}


/* User-defined shell functions. */

/*
 * Lookup a shell function name.
 */

#if	USE_PROTO
LOCAL SHFUNC * lookup_sh_fn (int len, CONST char * name)
#else
LOCAL SHFUNC *
lookup_sh_fn (len, name)
int		len;
CONST char    *	name;
#endif
{
	SHFUNC	      *	fnp;

	for (fnp = sh_fnp ; fnp != NULL ; fnp = fnp->fn_link)
		if (CSTRING_LENGTH (fnp->fn_name) == len &&
		    memcmp (name, CSTRING_STRING (fnp->fn_name), len) == 0)
			return fnp;
	return NULL;
}


/*
 * Unset a shell function, freeing the function body and detaching any
 * temporary files.
 */

#if	USE_PROTO
LOCAL void cleanup_shell_fn (SHFUNC * fnp, int mode)
#else
LOCAL void
cleanup_shell_fn (fnp, mode)
SHFUNC	      *	fnp;
int		mode;
#endif
{
	free_node (fnp->fn_body, mode);
}


/*
 * When a subshell is created, the functions are available in the subshell
 * but we don't want to lose the here-documents. Making links to the temp
 * files would be a very safe approach, but we aren't ready for that yet,
 * so we just forget about the files and assume our parent will hold them
 * open.
 */

#if	USE_PROTO
void subshell_shell_fns (void)
#else
void
subshell_shell_fns ()
#endif
{
	SHFUNC	      *	scan;

	scan = sh_fnp;

	while (scan != NULL) {
		cleanup_shell_fn (scan, DETACH);
		scan = scan->fn_link;
	}
}


/*
 * Delete a shell function. Returns -1 on failure.
 */

#if	USE_PROTO
LOCAL int unset_shell_fn (CONST char * name)
#else
LOCAL int
unset_shell_fn (name)
CONST char    *	name;
#endif
{
	SHFUNC * fnp, * prev;

	if ((fnp = lookup_sh_fn (strlen (name), name)) == NULL)
		return -1;

	if (fnp == (prev = sh_fnp))
		sh_fnp = fnp->fn_link;
	else {
		while (prev->fn_link != fnp)
			prev = prev->fn_link;
		prev->fn_link = fnp->fn_link;
	}

	cleanup_shell_fn (fnp, DEALLOCATE);
	sfree (fnp);
	return 0;
}


/*
 * Before final exit, clean up all shell functions. Freeing the memory of
 * the function bodies is a waste of time here, so we leave that alone.
 */

#if	USE_PROTO
void cleanup_shell_fns (void)
#else
void
cleanup_shell_fns ()
#endif
{
	SHFUNC	      *	scan;
	SHFUNC	      *	next;

	for (scan = sh_fnp ; scan != NULL ; scan = next) {
		next = scan->fn_link;
		cleanup_shell_fn (scan, CLEAN);
	}

	sh_fnp = NULL;
}


/*
 * Recursively allocate a fresh copy of a NODE. Node that the old node is
 * modified such that it is no longer useful, but since we are called only
 * to copy nodes for preservation in functions, that's no big deal.
 */

#if	USE_PROTO
NODE * copy_node (NODE * np)
#else
NODE *
copy_node (np)
NODE	      *	np;
#endif
{
	NODE	      *	newnp;

	if (np == NULL)
		return NULL;

	newnp = (NODE *) salloc (sizeof (* np));	/* allocate new NODE */
	newnp->n_type = np->n_type;

	if (NODE_LOOPS (np))
		np->n_next->n_next = NULL;	/* zap the loop for recursion */

	switch (NODE_UNION_TAG (np)) {

	case STRP:
		newnp->n_strp = duplstr (np->n_strp, 1);
		break;

	case NODEP:
		newnp->n_auxp = copy_node (np->n_auxp);
		break;

	case NULLP:
		break;

	case CSTRP:
		newnp->n_cstrp = dupcstr (np->n_cstrp, 1);
		break;

	case FILEP:
		newnp->n_filep = dup_temp_files (np->n_filep);
		break;

	default:
		panic (9);
	}

	newnp->n_next = copy_node (np->n_next);

	if (NODE_LOOPS (np))
		newnp->n_next->n_next = newnp;		/* restore loop */
	return newnp;
}


/*
 * Undo the above.
 */

#if	USE_PROTO
void free_node (NODE * np, int mode)
#else
void
free_node (np, mode)
NODE	      *	np;
int		mode;
#endif
{
	if (np == NULL)
		return;

	if (NODE_LOOPS (np))
		np->n_next->n_next = NULL;	/* don't free forever! */

	switch (NODE_UNION_TAG (np)) {

	case STRP:
		if (mode == DEALLOCATE)
			sfree (np->n_strp);
		break;

	case NODEP:
		free_node (np->n_auxp, mode);
		break;

	case NULLP:
		break;

	case CSTRP:
		if (mode == DEALLOCATE)
			sfree (np->n_cstrp);
		break;

	case FILEP:
		switch (mode) {
		case DETACH:
			np->n_filep = NULL;
			break;

		case DEALLOCATE:
		case CLEAN:
			unlink_temp (np->n_filep);
			break;
		}
		break;

	default:
		panic (10);
	}

	free_node (np->n_next, mode);

	if (mode == DEALLOCATE)
		sfree (np);
	else if (mode == DETACH && NODE_LOOPS (np))
		np->n_next->n_next = np;	/* relink loop */
}


/*
 * Define a shell function.
 */

#if	USE_PROTO
void def_shell_fn (NODE * np)
#else
void
def_shell_fn (np)
NODE	      *	np;
#endif
{
	SHFUNC	      *	fnp;

	if ((fnp = lookup_sh_fn (CSTRING_LENGTH (np->n_cstrp),
				 CSTRING_STRING (np->n_cstrp))) != NULL)
		cleanup_shell_fn (fnp, DEALLOCATE); /* free old body */
	else {
		fnp = (SHFUNC *) salloc (sizeof (* fnp));
						/* allocate new function */
		fnp->fn_link = sh_fnp;		/* add it to list */
		sh_fnp = fnp;
		fnp->fn_name = dupcstr (np->n_cstrp, 1);
	}

	fnp->fn_body = copy_node (np->n_next);	/* and copy function body */
}


/*
 * Look for a shell function, execute it if found. Let's not forget that
 * shell functions have to be redirected in a similar manner to builtins.
 */

#if	USE_PROTO
int shell_function (int flags)
#else
int
shell_function (flags)
int		flags;
#endif
{
	SHFUNC * fnp;
	CON * ocon;
	REDIR_UNDO    *	undo = NULL;
	struct arginfo	info;

	if (nargv [0] == NULL)
		return 0;
	if ((fnp = lookup_sh_fn (strlen (nargv [0]), nargv [0])) == NULL)
		return 0;

	/* Set up sargc, sarg0, sargv, sargp here for $1 etc. to work. */

	push_arginfo (& info, nargc - 1, nargv [0], nargv + 1);

	ocon = sesp->s_con;
	sesp->s_con = NULL;

	if (redirect (niovp, & undo) < 0)
		slret = 1;
	else  {
		++ in_sh_fn;
		slret = command (fnp->fn_body, flags);	/* execute it */
		-- in_sh_fn;

		redirundo (undo);
	}

	ret_done = 0;

	sesp->s_con = ocon;
	pop_arginfo (& info);
	return 1;
}


extern int unsetvar ();

#if	USE_PROTO
LOCAL int s_unset (void)
#else
LOCAL int
s_unset ()
#endif
{
	int		i;

	if (nargc > 1 && strcmp (nargv [1], "-f") == 0) {
		/*
		 * Unsetting functions.
		 */

		for (i = 2 ; i < nargc ; i ++)
			(void) unset_shell_fn (nargv [i]);
	} else {
		/*
		 * NIGEL: Permit POSIX.2 "-v" option.
		 */
		if (strcmp (nargv [i = 1], "-v") == 0)
			i = 2;
		while (i < nargc)
			(void) unsetvar (nargv [i ++]);
	}

	return 0;
}


/* Special built-in functions. */

static struct builtin {
	CONST char    *	i_name;
	int	     (*	i_func)	PROTO ((void));
	int		i_hash;
} builtins [] = {
	{ "cd",		s_cd },
#if	DIRECTORY_STACK
	{ "dirs",	s_dirs },
#endif
	{ "echo",	s_echo },
	{ "false",	s_false },
	{ "getopts",	s_getopts },
	{ "lgetopts",	s_getopts },
	{ "login",	s_login },
	{ "newgrp",	s_login },
#if	DIRECTORY_STACK
	{ "popd",	s_popd },
	{ "pushd",	s_pushd },
#endif
	{ "pwd",	s_pwd },
	{ "read",	s_read },
	{ "times",	s_times },
	{ "true",	s_colon },
	{ "ulimit",	s_ulimit },
	{ "umask",	s_umask },
	{ "wait",	s_wait }
};


static struct builtin spec_builtins [] = {
	{ ":",		s_colon },
	{ ".",		s_dot },
	{ "break",	s_break },
	{ "command",	s_command },
	{ "continue",	s_break },
	{ "eval",	s_eval },
	{ "exec",	s_exec },
	{ "exit",	s_exit },
	{ "export",	s_export },
	{ "readonly",	s_export },
	{ "set",	s_set },
	{ "shift",	s_shift },
	{ "trap",	s_trap },
	{ "unset",	s_unset }
};


#if	USE_PROTO
int ihash (CONST char * cp)
#else
int
ihash (cp)
CONST char    *	cp;
#endif
{
	int i;
	for (i = 0 ; * cp ; i += * cp ++)
		/* DO NOTHING */ ;
	return i;
}


#if	USE_PROTO
LOCAL struct builtin * find_builtin (struct builtin * table, size_t len,
				     CONST char * name)
#else
LOCAL struct builtin *
find_builtin (table, len, name)
struct builtin * table;
size_t		len;
CONST char    *	name;
#endif
{
	int		ahash;
	struct builtin * ip;

	if (table->i_hash == 0) {
		for (ip = table ; ip != table + len ; ip ++)
			ip->i_hash = ihash (ip->i_name);
	}
	if (name == NULL)
		return NULL;

	ahash = ihash (name);

	ip = table;
	while (ip->i_hash != ahash || strcmp (nargv [0], ip->i_name) != 0) {

		if (++ ip == table + len)
			return NULL;
	}

	dflttrp (IBUILTIN);
	return ip;
}


#if	USE_PROTO
int shell_builtin (void)
#else
int
shell_builtin ()
#endif
{
	struct builtin * ip;
	REDIR_UNDO    *	undo = NULL;

	if ((ip = find_builtin (builtins, ARRAY_LENGTH (builtins),
				nargv [0])) == NULL)
		return 0;

	if (redirect (niovp, & undo) < 0)
		slret = 1;
	else 
		slret = (* ip->i_func) ();

	redirundo (undo);

	return 1;
}


#if	USE_PROTO
int special_builtin (void)
#else
int
special_builtin ()
#endif
{
	struct builtin * ip;

	if ((ip = find_builtin (spec_builtins, ARRAY_LENGTH (spec_builtins),
				nargv [0])) == NULL)
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

	if (ip->i_func == s_exec)
		slret = s_exec ();
	else if (ip->i_func == s_command)
		return s_command ();
	else {
		REDIR_UNDO    *	undo = NULL;

		if (redirect (niovp, & undo) < 0)
			slret = 1;
		else 
			slret = (* ip->i_func) ();

		redirundo (undo);
	}
	return 1;
}

