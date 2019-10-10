/*
 * sh/eval.c
 * Bourne shell.
 * Evaluation of parameter substitution, command substitution,
 * blank interpretation, and file name generation.
 */

#include <sys/compat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sh.h"

struct eval_context {
	char	* arcp;		/* character position in argument */
	int	argf;		/* First character of argument flag */
	int	argg;		/* Glob seek, escape quoted glob chars */
	int	argq;		/* Quotation flag, no blanks or glob */
} _eval [1];

LOCAL	void	variable	();
LOCAL	void	special		();
LOCAL	void	graves		();

LOCAL	void	add_char	();
LOCAL	void	add_quoted	();
LOCAL	void	add_arg		();
LOCAL	void	end_arg		();

#define	EVAL_NO_QUOTE			0
#define	EVAL_QUOTED			1
#define	EVAL_FORCE_QUOTE		2

#define	EVAL_INPUT_SOURCE(eval,cp)	(void) ((eval)->arcp = (cp))
#define	EVAL_NEXT_CHAR(eval)		(* (eval)->arcp ++)
#define	EVAL_UNGET_CHAR(eval,ch)	(void) ((eval)->arcp --)

#define	EVAL_SET_QUOTING(eval,q)	(void) ((eval)->argq = (q))
#define	EVAL_ADD_QUOTE(eval,q)	\
		(void) ((eval)->argq = ((eval)->argq & ~ EVAL_QUOTED)  | (q))
#define	EVAL_IS_QUOTED(eval)		((eval)->argq)

#define	EVAL_SET_GLOB(eval,g)		(void) ((eval)->argg = (g))
#define	EVAL_IS_GLOBSEEK(eval)		((eval)->argg != 0)

#define	EVAL_MADE_ARGUMENT(eval)	(void) ((eval)->argf = 0)
#define	EVAL_ARGUMENT_IF_QUOTED(eval)	(void) ((eval)->argf &= \
						~ (eval)->argq)
#define	EVAL_NO_ARGUMENT(eval)		(void) ((eval)->argf = 1, strp = strt)
#define	EVAL_IS_NO_ARGUMENT(eval)	((eval)->argf != 0)


/*
 * Evaluate a string.
 */

void
eval(cp, f)
char *cp;
{
	register int m, c;

	EVAL_INPUT_SOURCE (_eval, cp);
	EVAL_NO_ARGUMENT (_eval);

	if (f == EHERE) {
		m = MHERE;
		EVAL_SET_QUOTING (_eval, EVAL_FORCE_QUOTE);
	} else if (f == EWORD) {
		m = MNQUO;
		EVAL_SET_QUOTING (_eval, EVAL_FORCE_QUOTE);
	} else {
		m = MNQUO;
		EVAL_SET_QUOTING (_eval, EVAL_NO_QUOTE);
	}
	EVAL_SET_GLOB (_eval, f == EARGS || f == EPATT);

	while ((c = EVAL_NEXT_CHAR (_eval)) != '\0') {
		if (! class (c, m)) {
			add_char (c);
			continue;
		}
		switch (c) {
		case '"':	/* m == MNQUO || m == MDQUO */
			if ((m ^= (MDQUO ^ MNQUO)) == MDQUO) {

				EVAL_ADD_QUOTE (_eval, EVAL_QUOTED);

				if ((c = EVAL_NEXT_CHAR (_eval)) == '\"')
					EVAL_MADE_ARGUMENT (_eval);

				EVAL_UNGET_CHAR (_eval, c);
			} else
				EVAL_ADD_QUOTE (_eval, 0);
			continue;

		case '\'':	/* m == MNQUO */
			while ((c = EVAL_NEXT_CHAR (_eval)) != '\'' && c != 0)
				add_quoted (c);
			EVAL_MADE_ARGUMENT (_eval);
			continue;

		case '\\':	/* m == MDQUO || m == MNQUO */
			c = EVAL_NEXT_CHAR (_eval);
			if (m != MNQUO && ! class (c, m)) {
				add_char ('\\');
				add_char (c);
			} else
				add_quoted (c);
			EVAL_MADE_ARGUMENT (_eval);
			continue;

		case '$':	/* m == MNQUO || m = MDQUO */
			variable ();
			continue;

		case '`':	/* m == MNQUO || m = MDQUO */
			graves (m);
			continue;

		default:
			add_char (c);
			continue;
		}
	}

	if (f == EARGS)
		end_arg ();
	else
		* strp ++ = '\0';
}


/*
 * Read the name of a shell variable and perform the appropriate substitution.
 */

LOCAL void
variable()
{
	VAR *vp;
	int s;
	char *sav;
	register int c, count, quote;
	register char * pp;
	char	      *	name_start = strp;
	char	      *	alternate_value;
	int		colon_test = 0;

	s = '\0';
	c = EVAL_NEXT_CHAR (_eval);

	if (strchr (SPECIAL_VAR_CHARS, c) != 0) {
		special (c);
		return;
	} else if (class (c, MRVAR)) {
		do {
			add_arg (c);
			c = EVAL_NEXT_CHAR (_eval);
		} while (class (c, MRVAR));

		EVAL_UNGET_CHAR (_eval, c);
	} else if (c != '{') {
		/* Not a legal variable name, put it back. */
		add_char ('$');
		add_char (c);
		return;
	} else {
		/* c == '{' */

		c = EVAL_NEXT_CHAR (_eval);

		if (strchr (SPECIAL_VAR_CHARS, c) != NULL) {
			int		peek;

			if ((peek = EVAL_NEXT_CHAR (_eval)) == '}') {

				/* Allow specials of the form "${?}" etc. */
				special (c);
				return;
			}
			EVAL_UNGET_CHAR (_eval, peek);
		}

		for (;;) {
			if (! class (c, MRVAR))
				break;
			add_arg (c);
			c = EVAL_NEXT_CHAR (_eval);
		}

		if ((colon_test = c == ':') != 0)
			c = EVAL_NEXT_CHAR (_eval);

		if (strchr ("-=?+", c) != NULL) {
			/*
			 * ${VAR [-=?+] word}
			 * Stash away the value of the intemediate character
			 * and store a '=' character to stroke the variable-
			 * storage machinery.
			 */

			s = c;
			add_arg ('=');
			alternate_value = strp;
			if ((quote = EVAL_NEXT_CHAR (_eval)) != '"' &&
			    quote != '\'') {

				EVAL_UNGET_CHAR (_eval, quote);
				quote = 0;
			}

			for (count = 1; ; ) {
				c = EVAL_NEXT_CHAR (_eval);
				if (c == '}' && count -- == 1)
					break;
				else if (c == '$' && quote != '\'') {
/*
 * steve 6/24/92
 * This truly sleazy hack handles e.g. "${V1-$V2}", oy.
 * It doesn't do it very well, paying no attention to quotes (for example).
 */
					variable ();
					continue;
				} else if (c == '{')
					++ count;
				else if (quote != 0 && c == quote) {
					quote = 0;
					continue;
				}
				add_arg (c);
			}
		} else if (colon_test || c != '}') {
			eillvar (strp = name_start);
			return;
		}
	}
	add_arg ('\0');

	c = * name_start;

	if (class (c, MDIGI)) {
		if ((c -= '1') >= sargc)
			pp = NULL;
		else
			pp = sargp [c];
	} else if (namevar (name_start) == 0) {
		eillvar (strp = name_start);
		return;
	} else {
		pp = NULL;
		if ((vp = findvar (name_start)) != NULL) {
			pp = convvar (vp);
			if (* pp == '\0' && colon_test)
				pp = NULL;	/* regard value "" as not set */
		}
	}

	switch (s) {
	case '\0':
		if (uflag != 0 && pp == NULL)
			enotdef (name_start);
		break;

	case '-':
		if (pp == NULL)
			pp = alternate_value;
		break;

	case '=':
		if (pp == NULL) {
			if (class (* name_start, MDIGI) ||
			    strchr (SPECIAL_VAR_CHARS,
				    * name_start) != NULL) {

				printe ("Illegal substitution");
				strp = name_start;
				return;
			}
			setsvar (name_start);
			pp = alternate_value;
		}
		break;

	case '?':
		if (pp != NULL)
			break;
		if (* alternate_value != '\0')
			prints ("%s\n", alternate_value);
		else {
			alternate_value [-1] = '\0';
			enotdef (name_start);
		}
		reset (RUABORT);
		NOTREACHED;

	case '+':
		if (pp != NULL)
			pp = alternate_value;
		break;
	}

	strp = name_start;

	if (pp != NULL)
		while ((c = * pp ++) != '\0')
			add_char (c);

	EVAL_ARGUMENT_IF_QUOTED (_eval);
}


/*
 * Return the value of the special shell variables.
 * No check for end of buffer.
 */

LOCAL void
special(n)
int		n;
{
	int		flag;
	char	      *	sp;

	switch (n) {
	case '#':
		n = sargc;
		goto maked;

	case '?':
		n = slret;
		goto maked;

	case '$':
		n = shpid;
		goto maked;

	case '!':
		n = sback;
		goto maked;

	maked:
		sprintf (strp, "%d", n);
		sp = strp;
		break;

	case '-':

		for (sp = & eflag; sp <= & xflag ; sp ++)
			if (* sp)
				add_char (* sp);

		EVAL_ARGUMENT_IF_QUOTED (_eval);
		return;

	case '@':
	case '*':
		flag = EVAL_IS_QUOTED (_eval) == EVAL_QUOTED && n == '@';
		for (n = 0; n < sargc; n++) {
			if (n) {
				if (flag)
					end_arg ();
				else
					add_char (' ');
			}

			sp = sargp [n];
			while (* sp)
				add_char (* sp ++);

			/*
			 * Make sure that arguments like "" get handled
			 * properly when expanding "$@"
			 */

			if (flag)
				EVAL_ARGUMENT_IF_QUOTED (_eval);
		}
		return;

	case '0':
		sp = sarg0;
		break;

	default:
		if (n - '1' >= sargc) {
			if (uflag)
				printe ("Unset parameter: %c", n);
			EVAL_ARGUMENT_IF_QUOTED (_eval);
			return;
		}
		sp = sargp [n - '1'];
		break;
	}

	while (* sp)
		add_char (* sp ++);

	EVAL_ARGUMENT_IF_QUOTED (_eval);
}


/*
 * Read and evaluate a command found between graves.
 *
 * NB : Backslash-quoting inside graves was not supported properly before,
 * and I'm not sure that I've got it right. The idea is that before passing
 * things off to session (), we process the appropriate backslash-escapes.
 * What consists of an "appropriate" escape depends on whether the graves
 * appeared within a double-quoted section (in which case we recognise the
 * specials appropriate to that) or not (in which case we recognised the
 * characters $`\ as specials. We also process '$'-expansions in graves now,
 * but not globs.
 *
 * Since we are removing some bashslashes here, we should be building into
 * a temporary buffer. We append our work onto the global "strp" buffer and
 * cut it back once we have finished.
 */

LOCAL void
graves (quotemode)
int		quotemode;
{
	int pipev[2], f, oslret;
	char	      *	ostrp;
	register FILE *fp;
	register int c;
	register int nnl;

	ostrp = strp;
	oslret = slret;

	while ((c = EVAL_NEXT_CHAR (_eval)) != '`') {
		if (c != '\\') {
			add_arg (c);
			continue;
		}
		c = EVAL_NEXT_CHAR (_eval);

		if (! (quotemode == MDQUO && class (c, MDQUO)) &&
		    (c != '$' && c != '\\' && c != '`'))
			add_arg ('\\');

		add_arg (c);
	}
	* strp = 0;

	if ((f = pipeline (pipev)) == 0) {
		slret = oslret;		/* in case grave command uses $? */
		dup2 (pipev [1], 1);
		close (pipev [0]);
		close (pipev [1]);
		exit (session (SARGS, ostrp));
		NOTREACHED;
	}

	close (pipev [1]);
	if ((fp = fdopen (pipev [0], "r")) == NULL) {
		close (pipev [0]);
		ecantfdop ();
		return;
	}
	strp = ostrp;
	nnl = 0;

	while ((c = getc (fp)) != EOF) {
		if (! recover (IEVAL)) {
#ifdef VERBOSE
			if (xflag) prints ("Interrupt in eval\n");
#endif
			errflag ++;
			break;
		}
		if (c == '\n')
			++ nnl;
		else {
			while (nnl) {
				nnl --;
				add_char ('\n');
			}
			add_char (c);
		}
	}

	/*
	 * If we expanded to something, we have an arg. If we are in double-
	 * quotes, we have an arg. Otherwise, we have an arg if we had an arg
	 * before.
	 */

	EVAL_ARGUMENT_IF_QUOTED (_eval);
	fclose (fp);
	waitc (f);
}


/*
 * Add a character to the current argument.
 * If no quotation is set, pick off blanks and globs.
 */

LOCAL void
add_char(c)
register int c;
{
	if (EVAL_IS_QUOTED (_eval) == 0) {
		if (strchr (vifs, c) != NULL) {
			end_arg ();
			return;
		}
		if (EVAL_IS_GLOBSEEK (_eval) && class (c, MGLOB)) {
			add_arg (c);
			EVAL_MADE_ARGUMENT (_eval);
			return;
		}
	}
	add_quoted (c);
}


/*
 * Add a quoted character to the current argument.
 * if argg is set, then glob characters are quoted with a \,
 * as well as \ itself.
 */

LOCAL void
add_quoted(c) register int c;
{
	if (EVAL_IS_GLOBSEEK (_eval) && (class (c, MGLOB) || c == '\\'))
		add_arg ('\\');

	add_arg (c);
	EVAL_MADE_ARGUMENT (_eval);
}


/*
 * Add a character to the current argument
 * and check for end of buffer.
 */

LOCAL void
add_arg (c) register int c;
{
	if (strp >= strt + STRSIZE)	/* Should do more */
		etoolong ("in add_arg ()");
	else
		* strp ++ = c;
}


/*
 * Terminate the current argument if it is non-empty.
 * If argg is set, then glob the argument to expand globs
 * or to simply remove any quotes.
 */

LOCAL void
end_arg ()
{
	if (EVAL_IS_NO_ARGUMENT (_eval))
		return;

	* strp ++ = '\0';
	if (EVAL_IS_GLOBSEEK (_eval))
		glob1 (duplstr (strt, 0));
	else {
		nargv = addargl (nargv, duplstr (strt, 0));
		nargc += 1;
	}

	EVAL_NO_ARGUMENT (_eval);
}


/*
 * Evaluate a here document.
 */

int
evalhere(u2)
{
	register int u1;
	register FILE *f2;
	char buf[128];
	char *tmp;

	tmp = shtmp ();
	if ((u1 = creat (tmp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		ecantmake (tmp);
		return -1;
	}
	if ((f2 = fdopen (u2, "r")) == NULL) {
		ecantfdop ();
		close (u1);
		close (u2);
		return -1;
	}
	while (fgets (buf, sizeof (buf), f2) != NULL) {
		eval (buf, EHERE);
		write (u1, strt, strp - 1 - strt);
	}
	close (u1);
	fclose (f2);
	if ((u2 = open (tmp, O_RDONLY)) < 0) {
		ecantopen (tmp);
		u2 = -1;
	}
	unlink (tmp);
	return u2;
}
