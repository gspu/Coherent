/*
 * Shell part of execution.
 */

#include <unistd.h>

#include "shellio.h"
#include "sh.h"

static char	no_last_command [] = "";
static char   *	lastcmd = no_last_command;

char * skipredir ();

#if	USE_PROTO
LOCAL int print_arg (int flag, CONST char * str)
#else
LOCAL int
print_arg (flag, str)
int		flag;
CONST char    *	str;
#endif
{
	if (! xflag)
		return 0;

	if (flag)
		shell_outchar (' ');

	shell_outstr (str);
	return 1;
}


/*
 * Run a simple command.
 *	Anything but.
 */

#define	FARGS	1
#define	FIORS	2
#define	FASSG	4



#if	USE_PROTO
LOCAL int comscom (NODE * np, int flags)
#else
LOCAL int
comscom (np, flags)
NODE	      *	np;
int		flags;
#endif
{
	int		f;
	char	     **	app;
	char	      *	s;
	char	      *	s1;
	int		nargs;
	int		pathflag = 0;
	int		printed = 0;

	nargc = 1;
	nargv = makargl ();
	nargv = addargl (nargv, "sh", 0);
	niovp = makargl ();
	nenvp = makargl ();
	nctlp = NULL;

	/*
	 * Scan for arguments.
	 * or a control node.
	 */

	f = 0;
	for ( ; np; np = np->n_next) {

		switch (np->n_type) {
		case NIORS:
			f |= FIORS;
			/*
			 * New code by steve 1/24/91.
			 * This allows globs in redirection args.
			 */

			s = skipredir (s1 = CSTRING_STRING (np->n_cstrp));
			nargs = nargc;
			(void) eval_buf (s, CSTRING_SIZE (np->n_cstrp) -
					    (s - s1), EARGS);
			lex_init ();
			while (s1 < s)
				lex_add (* s1 ++);
			lex_add_string (nargv [nargs]);	/* build redir arg */
			lex_add (0);
			s = lex_done ();

			niovp = addargl (niovp, s, 0);

			printed = print_arg (printed, s);

			-- nargc;
			for (app = nargv + nargs; * app; app ++) {
				* app = app [1]; /* shift remaining args */
				if (* app != NULL)
					printed = print_arg (printed, * app);
			}
			continue;

		case NASSG:
			if (kflag || (f & FARGS) == 0) {
				f |= FASSG;
				s = eval_node (np, EWORD);
				nenvp = addargl (nenvp, s, 0);
				printed = print_arg (printed, s);

				continue;
			}
			/* FALL THROUGH */

		case NARGS:
			f |= FARGS;
			nargs = nargc;
			(void) eval_node (np, EARGS);

			for (app = nargv + nargs; * app; )
				printed = print_arg (printed, * app ++);
			continue;

		case NCTRL:
			if (nctlp != NULL)
				panic (3);

			f |= FARGS;
			nctlp = np;
			continue;

		default:
			panic (4);
		}
	}
	if (printed)
		shell_endl ();

	nargv += 1;	/* Skip over "sh" */
	nargc -= 1;

	/* Last chance to quit */
	if (errflag || nflag)
		return 0;

	/* And away we go */
	if (lastcmd != no_last_command)
		sfree (lastcmd);
	if (* nargv)
		lastcmd = duplstr (* nargv, 1);
	else
		lastcmd = no_last_command;
	switch (f) {
	case 0:
		return 0;

	case FARGS:
		if (nctlp) {
			command (nctlp->n_auxp, flags);
			return 0;
		}

	case FARGS | FIORS:
	case FARGS | FASSG:
	case FARGS | FIORS | FASSG:
		switch (special_builtin ()) {

		case -2:
			pathflag = 1;
		case -1:
			if (special_builtin ())
				return 0;
			break;		/* suppress function lookup */

		case 1:
			/*
			 * After a special built-in, variable settings are
			 * preserved.
			 */

			for (app = nenvp ; * app != NULL; )
				setsvar (* app ++);
		case 2:
			return 0;

		case 0:
			if (shell_function (flags))
				return 0;
			break;
		}
		if (shell_builtin ())
			return 0;
		break;

	case FIORS:
		break;

	case FASSG:
	case FIORS | FASSG:
		for (app = nenvp ; * app != NULL; )
			setsvar (* app ++);
		if ((f & ~ FASSG) == 0) {
			slret = 0;
			return 0;
		}
		break;

	default:
		panic (5);
	}

	if ((flags & COMMAND_NO_FORK) != 0 || (f = clone ()) == 0) {
		if (pathflag)
			vpath = "/bin:/usr/bin";

		if (redirect (niovp, NULL) < 0) {
			slret = 1;
		} else if (nargc) {
			dflttrp (ICMD);
			nenvp = envlvar (nenvp);
			flexec ();
			slret = 127;	/* cannot find */
		} else if (nctlp)
			command (nctlp->n_auxp, flags);

		exit (slret);
	}
	return f;
}


/*
 * Execute a pipe command.
 * Fork, if necessary, a subshell to execute the pipe.
 * Fork each segment off.
 * Wait for last, save slret, wait for all, return slret from last.
 */

#if	USE_PROTO
LOCAL int pipecoms (NODE * np, int flags)
#else
LOCAL int
pipecoms (np, flags)
NODE	      *	np;
int		flags;
#endif
{
	int f;
	int p1st = 0;
	int		pipev [2];
	int		nextstdin;
	int		retval;
	REDIR_UNDO    *	undo;

	nextstdin = 0;

	while (np->n_type == NPIPE) {
		if ((f = pipeline (pipev)) != 0) {
			/* Parent takes right hand side */
			np = np->n_next;
			if (p1st == 0)
				p1st = f;
			nextstdin = pipev [0];
			close (pipev [1]);
		} else {
			/* Child takes left hand side */
			if (nextstdin != 0) {
				dup2 (nextstdin, STDIN_FILENO);
				close (nextstdin);
			}
			if (pipev [1] != 1) {
				dup2 (pipev [1], STDOUT_FILENO);
				close (pipev [1]);
			}
			close (pipev [0]);

			dflttrp (IPIPE);
			exit (command (np->n_auxp, COMMAND_NO_FORK));
		}
	}

	undo = remember_undo (NULL, 0);
	if (nextstdin != 0) {
		dup2 (nextstdin, STDIN_FILENO);
		close (nextstdin);
	}

	/* take the pipe tail */
	retval = command (np, flags);
	redirundo (undo);

	return retval;
}


/*
 * Execute the given node, wait for completion, return status.
 */

#if	USE_PROTO
int command (NODE * np, int flags)
#else
int
command (np, flags)
NODE	      *	np;
int		flags;
#endif
{
	int		f;	/* Pid of command */
	CON		con;	/* Control link for break / continue */
	char	     **	inlp;	/* for NFOR */
	CSTRING	      *	innp;	/* for NFOR */
	NODE	      *	cnode;	/* for NCASE */
	char	      *	cname;	/* for NCASE */
	char	      *	tempstr;
	int		not_flag = 0;	/* for '!' in pipelines */

	if (ret_done)
		return;
	innp = NULL;
	inlp = NULL;
	cnode = NULL;
	f = 0;

	con.c_next = sesp->s_con;
	con.c_node = np;
	con.c_bpp = savebuf ();
	sesp->s_con = & con;

	switch (setjmp (con.c_envl)) {
	case 0:		/* initial setjmp */
		break;

	case 1:		/* continue */
		np = con.c_node;
		break;

	case 2:		/* break */
		goto break2;

	default:
		panic (1);
	}

	for ( ; np != NULL; np = np->n_next) {
	if (ret_done)
		break;		/* "return" interrupts control flow */
	recover (ICMD);
	f = 0;

	switch (np->n_type) {

	case NFILE:		/* no effect on control flow */
		continue;

	case NCOMS:
		f = comscom (np->n_auxp, flags);
		break;

	case NFOR:
		/*
		 * We need this test because when a 'continue' is executed
		 * this is the node that we longjmp to...
		 */

		if (innp == NULL) {
/*			assert (np->n_next->n_type == NFOR2); */

 			innp = np->n_cstrp;
			nargc = 0;
			nargv = makargl ();
			for (np = np->n_next->n_auxp ; np ; np = np->n_next)
				eval_node (np, EARGS);
			inlp = nargv;
			np = con.c_node;
		}
		continue;

	case NFOR2:
		/* do_done_list->n_next == this node */
		if (* inlp == NULL ||
		    assnvar (CSTRING_LENGTH (innp), CSTRING_STRING (innp),
			     * inlp ++) == NULL)
			break;
		continue;

	case NWHILE:
		/* do_done_list->n_next == this node */
		if (command (np->n_auxp, COMMAND_TRUTH_CONTEXT))
			break;
		continue;

	case NUNTIL:
		/* do_done_list->n_next == this node */
		if (! command (np->n_auxp, COMMAND_TRUTH_CONTEXT))
			break;
		continue;

	case NBRAC:
		command (np->n_auxp, flags & ~ COMMAND_NO_FORK);
		break;

	case NPARN:
		if ((flags & COMMAND_NO_FORK) != 0 || (f = clone ()) == 0)
			exit (command (np->n_auxp, flags));
		break;

	case NIF:
		if (! command (np->n_auxp->n_auxp, COMMAND_TRUTH_CONTEXT))
			np = np->n_auxp;
		else if (np->n_next == NULL)
			slret = 0;	/* exit status 0 if no elsepart */
		continue;

	case NELSE:
		command (np->n_auxp,  flags & ~ COMMAND_NO_FORK);
		break;

	case NCASE:
		cname = eval_node (np, EWORD);
		if (errflag)
			break;
		cname = duplstr (cname, 0);
		continue;

	case NCASE2:
		cnode = np->n_auxp;
		continue;

	case NCASE3:
		/* end of pattern list == next NCASE2 node */
		tempstr = eval_node (np, EPATT);
		if (errflag)
			break;
		if (match (tempstr, cname)) {
			command (cnode,  flags & ~ COMMAND_NO_FORK);
			break;
		}
		continue;

	case NLIST:
		command (np->n_auxp, flags & ~ COMMAND_NO_FORK);
		continue;

	case NANDF:
		if (command (np->n_auxp, COMMAND_TRUTH_CONTEXT))
			break;
		continue;

	case NORF:
		if (! command (np->n_auxp, COMMAND_TRUTH_CONTEXT))
			break;
		continue;

	case NBACK:
		if ((flags & COMMAND_NO_FORK) != 0 || (f = clone ()) == 0) {
			static char * iov [] = { "0</dev/null", NULL };

			dflttrp (IBACK);
			redirect (iov, NULL);

			exit (command (np->n_auxp, flags | COMMAND_NO_FORK));
		}
		sback = f;

		shell_outint (f);
		shell_endl ();

		f = 0;
		continue;

	case NNOT:
		not_flag = 1;
		continue;

	case NPIPE:
		slret = pipecoms (np, flags);
		break;

	case NFUNC:
		def_shell_fn (np);
		break;

	case NRET:
		if (in_sh_fn == 0) {
			shellerr_simple ("return", "not inside shell function");
			break;
		}
		ret_done ++;		/* to interrupt control flow */
		if (np->n_cstrp != NULL)
			slret = strtol (eval_node (np, EWORD), NULL, 0);

		return slret;

	default:
		panic (2);
	}
	break;

	}
break2:
	if (sesp->s_con != NULL)
		sesp->s_con = sesp->s_con->c_next;
	if (f)
		waitc (f, WAIT_NORMAL);

	if (slret)
		assnvar (sizeof ("LASTERROR") - 1, "LASTERROR", lastcmd);

	DESTROY_CON (& con);

	if ((flags & COMMAND_TRUTH_CONTEXT) == 0 && eflag &&
	    (slret || errflag))
		reset (RUABORT);

	if (not_flag)
		slret = ! slret;
	return slret;
}


/*
 * Skip a redirection arg, return pointer to following nonspace.
 */

char *
skipredir (s)
char * s;
{
	if (* s >= '1' && * s <= '9')
		++ s;
	if (* s == '>' || * s == '<')
		++ s;
	if (* s == '>' || * s == '<')
		++ s;
	if (* s == '&')
		++ s;
	while (* s == ' ' || * s == '\t')
		++ s;
	return s;
}
