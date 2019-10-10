/*
 * Evaluation of parameter substitution, command substitution,
 * blank interpretation, and file name generation.
 */

#include <sys/compat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "buildobj.h"

#include "shellio.h"
#include "sh.h"

typedef	struct eval_context	eval_t;

struct eval_style {
	int		es_argf;	/* First character of argument flag */
	int		es_argg;	/* Glob seek, escape quoted globs */
	int		es_argq;	/* Quotation flag, no blanks or glob */
	int		es_quotemode;	/* quotation mode */
	int		es_args;	/* argument splitting? */
};

struct eval_context {
	struct eval_style
			e_style [1];
	unsigned char *	e_inbuf;	/* character position in input */
	unsigned char *	e_inbufend;	/* end of input buffer */
	build_t	      *	e_heap;		/* output storage */
	unsigned char *	e_last;		/* last thing built */
} _eval [1];

LOCAL	void	add_char	PROTO ((eval_t * evalp, unsigned char ch));
LOCAL	void	add_quoted	PROTO ((eval_t * evalp, unsigned char ch));
LOCAL	void	add_arg		PROTO ((eval_t * evalp, unsigned char ch));
LOCAL	void	end_arg		PROTO ((eval_t * evalp));

LOCAL	void	eval0		PROTO ((eval_t * evalp));

LOCAL	void	eval_push	PROTO ((eval_t * evalp));
LOCAL	void	eval_pop	PROTO ((eval_t * evalp));

#define	EVAL_NO_QUOTE			0
#define	EVAL_QUOTED			1
#define	EVAL_FORCE_QUOTE		2
#define	EVAL_LEAVE_QUOTED		4

#define	EVAL_STYLE(eval)	((eval)->e_style)

#define	EVAL_INPUT_SOURCE(eval,buf,len)	\
		((void) ((eval)->e_inbuf = (buf), \
		 (eval)->e_inbufend = (eval)->e_inbuf + (len)))
#define	EVAL_UNGET_CHAR(eval,ch)	((void) ((eval)->e_inbuf --))
#define	EVAL_NEXT_CHAR(eval) \
	((eval)->e_inbuf ++ < (eval)->e_inbufend ? (eval)->e_inbuf [- 1] : 0)

#define	EVAL_GET_CSTREND(eval) \
		((eval)->e_inbuf += sizeof (size_t), \
		 * ((size_t *) (eval)->e_inbuf - 1) + (eval)->e_inbuf)
#define	EVAL_SEEK_INPUT(eval,pos)	((void) ((eval)->e_inbuf = (pos)))

#define	EVAL_SET_QUOTING(eval,q) \
		(void) (EVAL_STYLE (eval)->es_argq = (q))
#define	EVAL_ADD_QUOTE(eval,q)	\
		(EVAL_STYLE (eval)->es_argq = \
		 (EVAL_STYLE (eval)->es_argq & ~ EVAL_QUOTED) | (q))
#define	EVAL_IS_QUOTED(eval)		(EVAL_STYLE (eval)->es_argq != 0)
#define	EVAL_QUOTING(eval)		(EVAL_STYLE (eval)->es_argq & \
					 ~ EVAL_QUOTED)

#define	EVAL_SET_QUOTEMODE(eval,q) \
		((void) (EVAL_STYLE (eval)->es_quotemode = (q)))
#define	EVAL_QUOTEMODE(eval)		(EVAL_STYLE (eval)->es_quotemode)

#define	EVAL_SET_GLOB(eval,g) \
		((void) (EVAL_STYLE (eval)->es_argg = (g)))
#define	EVAL_IS_GLOBSEEK(eval)		(EVAL_STYLE (eval)->es_argg != 0)

#define	EVAL_SET_ARGS(eval,a) \
		((void) (EVAL_STYLE (eval)->es_args = (a)))
#define	EVAL_SPLIT_ARGS(eval)		(EVAL_STYLE (eval)->es_args != 0)

#define	EVAL_MADE_ARGUMENT(eval) \
		((void) (EVAL_STYLE (eval)->es_argf = 0))
#define	EVAL_ARGUMENT_IF_QUOTED(eval) \
		((void) (EVAL_STYLE (eval)->es_argf &= \
			 ~ EVAL_STYLE (eval)->es_argq))
#define	EVAL_NO_ARGUMENT(eval) \
		((void) (EVAL_STYLE (eval)->es_argf = 1))
#define	EVAL_IS_NO_ARGUMENT(eval)	(EVAL_STYLE (eval)->es_argf != 0)


/*
 * Abort from an evaluation.
 */

#if	USE_PROTO
LOCAL NO_RETURN eval_abort (eval_t * evalp)
#else
LOCAL NO_RETURN
eval_abort (evalp)
eval_t	      *	evalp;
#endif
{
	size_t		size;

	do
		size = sizeof (struct eval_style);
	while (build_pop (evalp->e_heap, EVAL_STYLE (evalp),
			  & size) == BUILD_OK);
	evalp->e_last = NULL;
	(void) build_abandon (evalp->e_heap);
	reset (RUABORT);
}


#if	USE_PROTO
LOCAL char * eval_end (eval_t * evalp)
#else
LOCAL char *
eval_end (evalp)
eval_t	      *	evalp;
#endif
{
	if ((evalp->e_last = build_end (evalp->e_heap, NULL)) == NULL) {
		shellerr_simple ("internal error",
				  "build_end () could not end object");
		eval_abort (evalp);
	}

	return evalp->e_last;
}


/*
 * Evaluate a node.
 */

char *
eval_node (nodep, f)
NODE	      *	nodep;
int		f;
{
	switch (NODE_UNION_TAG (nodep)) {
	case STRP:
		return eval_buf (nodep->n_strp, strlen (nodep->n_strp), f);

	case CSTRP:
		return eval_buf (CSTRING_STRING (nodep->n_cstrp),
				 CSTRING_LENGTH (nodep->n_cstrp), f);
	default:
		panic (14);
	}
}


/*
 * Evaluate a string.
 */

char *
eval_buf (inbuf, inbufsize, f)
char	      *	inbuf;
size_t		inbufsize;
int		f;
{
	builderr_t	error;

	EVAL_INPUT_SOURCE (_eval, inbuf, inbufsize);

	if (_eval->e_heap == NULL) {
		if ((_eval->e_heap = builder_alloc (512, 0)) == NULL) {
			shellerr_simple ("internal error",
					 "cannot allocate heap");
			return NULL;
		}
	}
	if (_eval->e_last != NULL) {

		error = build_release (_eval->e_heap, _eval->e_last);
		_eval->e_last = NULL;
		if (error != BUILD_OK) {
			shellerr_simple ("unable to release old data",
					 build_error (error));
			return NULL;
		}
	}
	if ((error = build_begin (_eval->e_heap, 0, NULL)) != BUILD_OK) {
		shellerr_simple ("cannot begin evaluation",
				 build_error (error));
		eval_abort (_eval);
	}

	EVAL_NO_ARGUMENT (_eval);

	EVAL_SET_QUOTEMODE (_eval, MNQUO);
	EVAL_SET_QUOTING (_eval, EVAL_NO_QUOTE);
	EVAL_SET_GLOB (_eval, 1);
	EVAL_SET_ARGS (_eval, 0);

	switch (f) {
	case EHERE:
		EVAL_SET_QUOTEMODE (_eval, MHERE);
		/* FALL THROUGH */

	case EWORD:
		EVAL_SET_QUOTING (_eval, EVAL_FORCE_QUOTE);
		EVAL_SET_GLOB (_eval, 0);
		break;

	case EARGS:
		EVAL_SET_ARGS (_eval, 1);
		break;
	}

	eval0 (_eval);

	if (EVAL_SPLIT_ARGS (_eval))
		end_arg (_eval);

	/*
	 * Always null-terminate the output string.
	 */

	add_arg (_eval, 0);
	return eval_end (_eval);
}


/*
 * Evaluate a here document.
 */

int
evalhere (u2)
int u2;
{
	int		u1;
	char	      *	tmp;
	SES		s;

	tmp = shtmp ();
	if ((u1 = creat (tmp, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		ecantmake (tmp);
		return -1;
	}

	/*
	 * Push the temporary file as a session so the lexer can use it as
	 * input.
	 */

	if (push_session (SSTR, u2, & s) != 0)
		return -1;

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

		if (xflag)
			dump_cstring (sh_yytext);
		temp = eval_buf (CSTRING_STRING (sh_yytext),
				 CSTRING_LENGTH (sh_yytext), EHERE);
		write (u1, temp, strlen (temp));

		if (c < 0)
			break;
	}

	pop_session (& s);

	close (u1);
	close (u2);
	if ((u2 = open (tmp, O_RDONLY)) < 0) {
		ecantopen (tmp);
		u2 = -1;
	}
	unlink (tmp);
	return u2;
}


/*
 * Return the value of the special shell variables, and return the length of
 * the amount sent to the output.
 *
 * NOTE: If the variable expansion was for an unset variable, we return -1 as
 * a length instead, and leave the text of the unset variable in a sub-heap.
 * If we are in SPEC_CURLY mode and the character following the variable is
 * either '%' or '#', then we expand the result unto a sub-heap.
 */

enum {
	SPEC_LENGTH,
	SPEC_EXPAND,
	SPEC_CURLY,
	SPEC_QUOTED_ATSIGN,
	SPEC_PATTERN
};


/*
 * Make sure the given variable name is valid.
 * Only user settable names are accepted.
 */

#if	USE_PROTO
int namevar (CONST char * np)
#else
int
namevar (np)
CONST char    *	np;
#endif
{
	int c;
	CONST char * cp;

	cp = np;
	if (class (* cp, MBVAR)) {
		do
			c = * cp ++;
		while (class (c, MRVAR));

		if (c == 0 || c == '=')
			return 1;
	}
	return 0;
}


LOCAL int
special (evalp, n, flag)
eval_t	      *	evalp;
int		n;
int		flag;
{
	CONST unsigned char   *	sp;
	unsigned char *	cp;
	struct flag   *	flagp;
	int		len;
	VAR	      *	vp;
	unsigned char	number [sizeof (int) * 3 + 1];

	eval_push (evalp);
	add_arg (evalp, n);

	switch (n) {
	case '#':
		len = sargc;
		goto maked;

	case '?':
		len = slret;
		goto maked;

	case '$':
		len = shpid;
		goto maked;

	case '!':
		len = sback;
		/* FALL THROUGH */

maked:
		sp = itoa (number, sizeof (number), len, 10, 0);
		break;

	case '-':
		cp = number;
		for (flagp = shflags ;
		     flagp < shflags + __ARRAY_LENGTH (shflags) ;
		     flagp ++) {
			if (flagp->f_value && ! flagp->f_startup)
				* cp ++ = flagp->f_char;
		}
		* cp ++ = 0;

		sp = number;
		break;

	case '*':
	case '@':
		if (flag == SPEC_CURLY) {
			len = EVAL_NEXT_CHAR (evalp);
			EVAL_UNGET_CHAR (evalp, len);
			if (len == ':' && nargc == 0) {
				sp = NULL;
				break;
			}
		}

		if (flag != SPEC_LENGTH && 
		    EVAL_IS_QUOTED (evalp) == EVAL_QUOTED && n == '@')
			flag = SPEC_QUOTED_ATSIGN;

		len = sargc;
		if (flag == SPEC_LENGTH) {
			flag = SPEC_EXPAND;
			goto maked;
		}

		eval_pop (evalp);

		if (n == '*') {
			/*
			 * Previously this was done above before falling into
			 * the common code; however, now that the semantics of
			 * push ()/pop () include more of the state, we take
			 * care to do this in the right context!.
			 */

			EVAL_ARGUMENT_IF_QUOTED (evalp);
		}

		for (n = 0 ; n < sargc ; n ++) {
			if (n) {
				if (flag == SPEC_QUOTED_ATSIGN)
					end_arg (evalp);
				else
					add_char (evalp, ' ');
			}

			sp = sargp [n];
			while (* sp)
				add_char (evalp, * sp ++);

			/*
			 * Make sure that arguments like "" get handled
			 * properly when expanding "$@"
			 */

			if (flag == SPEC_QUOTED_ATSIGN)
				EVAL_ARGUMENT_IF_QUOTED (evalp);
		}
		return len;

	case '0':
		sp = sarg0;
		break;

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if (n - '1' >= sargc)
			sp = NULL;
		else
			sp = sargp [n - '1'];
		break;

	default:
		if (! class (n, MBVAR)) {
			if (flag == SPEC_LENGTH) {
				len = 0;
				goto maked;
			}
			eval_pop (evalp);
			add_arg (evalp, '$');
			add_arg (evalp, n);

			EVAL_MADE_ARGUMENT (evalp);
			return 2;
		}

		for (;;) {
			n = EVAL_NEXT_CHAR (evalp);
			if (! class (n, MRVAR))
				break;

			add_arg (evalp, n);
		}

		EVAL_UNGET_CHAR (evalp, n);

		add_arg (evalp, 0);
		sp = eval_end (evalp);
		(void) build_resume (evalp->e_heap);
		(void) build_reduce (evalp->e_heap, 1);

		if (namevar (sp) == 0) {
			eillvar (sp);
			sp = NULL;		/* as if unset */
		} else if ((vp = findvar (sp)) == NULL)
			sp = NULL;		/* unset */
		else
			sp = convvar (vp);	/* whatever */
		break;
	}

	if (flag == SPEC_CURLY) {
		/*
		 * We need to deal with the colon-forms of the parameter
		 * expansions and the pattern forms as well. Each of these
		 * will require some special handling of the evaluator state.
		 */

		switch (len = EVAL_NEXT_CHAR (evalp)) {
		case ':':
			if (sp != NULL && * sp == NULL)
				sp = NULL;
			if ((len = EVAL_NEXT_CHAR (evalp)) != '+') {
				EVAL_UNGET_CHAR (evalp, len);
				break;
			}

		case '+':	/* Use alternate value */
			EVAL_UNGET_CHAR (evalp, len);
			flag = SPEC_PATTERN;
			break;

		case '%':
		case '#':
			eval_pop (evalp);
			eval_push (evalp);
			EVAL_SET_ARGS (evalp, 0);
			flag = SPEC_PATTERN;
			/* FALL THROUGH */

		default:
			EVAL_UNGET_CHAR (evalp, len);
			break;
		}
	}

	if (sp == NULL) {
		if (flag != SPEC_LENGTH) {
			add_arg (evalp, 0);
			(void) build_reduce (evalp->e_heap, 1);
			EVAL_ARGUMENT_IF_QUOTED (evalp);
			return -1;
		}

		len = 0;
	} else
		len = strlen (sp);

	if (flag == SPEC_LENGTH) {
		flag = SPEC_EXPAND;
		goto maked;
	}

	if (flag != SPEC_PATTERN)
		eval_pop (evalp);

	while (* sp)
		add_char (evalp, * sp ++);

	EVAL_ARGUMENT_IF_QUOTED (evalp);
	return len;
}


#if	USE_PROTO
LOCAL void eunexpected (char ch)
#else
LOCAL void
eunexpected (ch)
char		ch;
#endif
{
	if (shellerr_begin ("unexpected character")) {
		shellerr_outchar ('\'');
		shellerr_outchar (ch);
		shellerr_outchar ('\'');
		shellerr_endl ();
	}
}


/*
 * Recursively glue counted strings back together.
 */

LOCAL void
reconstitute (evalp, slashmode)
eval_t	      *	evalp;
int		slashmode;
{
	unsigned char *	foo;
	unsigned char	c;

	if ((c = EVAL_NEXT_CHAR (evalp)) != 0) {
		eunexpected (c);
		panic (17);
	}

	foo = EVAL_GET_CSTREND (evalp);

	for (;;) {
		size_t		len = strlen (evalp->e_inbuf);
		int		endchar;
		char		c = 0;

		while (len --) {
			switch (c = * evalp->e_inbuf ++) {
			case '\\':
				if (len == 0 || slashmode == 0)
					break;

#if	1
				if (* evalp->e_inbuf == '`' ||
				    * evalp->e_inbuf == '\\') {
#else
				if (class (* evalp->e_inbuf, MBQUO)) {
#endif
					len --;
					add_arg (evalp, * evalp->e_inbuf ++);
					continue;
				}
				break;

			default:
				break;
			}

			add_arg (evalp, c);
		}

		if (evalp->e_inbuf == foo)
			break;

		switch (c) {
		case '(':
			endchar = ')';
			break;

		case '{':
			endchar = '}';
			break;

		case '`':
			endchar = '`';
			break;

		default:
			eunexpected (c);
			panic (18);
		}

		reconstitute (evalp, slashmode);
		add_arg (evalp, endchar);
	}

	if (EVAL_NEXT_CHAR (evalp) != 0)
		panic (19);
}


/*
 * Read and evaluate a command output for command substitutions.
 */

LOCAL void
graves (evalp, slashmode)
eval_t	      *	evalp;
int		slashmode;
{
	int pipev[2], f, oslret;
	int nnl;
	int		quotemode;
	char		ch;

	/*
	 * Until we can deal with text in parse-node form, the easiest way
	 * to deal with command substitution is to simply glue all the text
	 * (including nested substrings) back together.
	 */

	eval_push (evalp);
	reconstitute (evalp);
	add_arg (evalp, 0);

	oslret = slret;

	if ((f = pipeline (pipev)) == 0) {
		char	      *	temp;

		slret = oslret;		/* in case grave command uses $? */
		dup2 (pipev [1], 1);
		close (pipev [0]);
		close (pipev [1]);

		/*
		 * The easiest way to deal with the eval heap in this case is
		 * to simply dump it. Don't forget that this means '_eval'
		 * rather than "evalp"!
		 */

		temp = duplstr (eval_end (evalp), 1);
		builder_free (_eval->e_heap);
		_eval->e_heap = NULL;
		_eval->e_last = NULL;

		exit (session (SARGS, temp));
	}
	eval_pop (evalp);

	close (pipev [1]);

	nnl = 0;
	quotemode = MNQUO;

	while (read (pipev [0], & ch, 1) == 1) {
		if (! recover (IEVAL)) {
			errflag ++;
			break;
		}

		switch (ch) {
		case '\n':
			nnl ++;
			break;

		default:
			while (nnl) {
				nnl --;
				add_char (evalp, '\n');
			}
			add_char (evalp, ch);
			break;
		}
	}

	close (pipev [0]);

	/*
	 * If we expanded to something, we have an arg. If we are in double-
	 * quotes, we have an arg. Otherwise, we have an arg if we had an arg
	 * before.
	 */

	EVAL_ARGUMENT_IF_QUOTED (evalp);

	waitc (f, WAIT_NORMAL);
}

typedef	CONST unsigned char  **	source_t;

LOCAL	long		eval_cond	PROTO ((source_t src));


/*
 * Evaluate a +, -, ~, !, (), <number> part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_unary (source_t src)
#else
LOCAL long
eval_unary (src)
source_t	src;
#endif
{
	for (;;) {
		char		c;
		long		tmp;

		switch (c = * (* src) ++) {
		case '+':
			break;

		case '-':
			return - eval_unary (src);

		case '~':
			return ~ eval_unary (src);

		case '!':
			return ! eval_unary (src);

		case '(':
			tmp = eval_cond (src);
			while (isspace (c = ** src))
				(* src) ++;
			if (c != ')')
				shellerr_simple ("bad syntax",
						 "missing ) in $(())");
			else {
				(* src) ++;
				while (isspace (c = ** src))
					(* src) ++;
			}
			return tmp;

		case 0:
			(* src) --;
			shellerr_simple ("bad syntax",
					 "missing value in $(())");
			return 0;

		default:
			if (isspace (c))
				continue;
			(* src) --;
			if (isdigit (c)) {
				CONST unsigned char   *	num = * src;
				errno = 0;

				tmp = strtol (num, src, 0);

				if (errno == ERANGE)
					tmp = strtoul (num, src, 0);

				while (isspace (** src))
					(* src) ++;
				return tmp;
			}

			eunexpected (c);
			return 0;
		}
	}
}


/*
 * Evaluate a *, /, % part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_mul (source_t src)
#else
LOCAL long
eval_mul (src)
source_t	src;
#endif
{
	long		left = eval_unary (src);

	for (;;) {
		switch (** src) {
		case '*':
			(* src) ++;
			left *= eval_unary (src);
			break;

		case '/':
			(* src) ++;

			left /= eval_unary (src);
			break;

		case '%':
			(* src) ++;

			left %= eval_unary (src);
			break;

		default:
			return left;
		}
	}
}


/*
 * Evaluate a +, - part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_add (source_t src)
#else
LOCAL long
eval_add (src)
source_t	src;
#endif
{
	long		left = eval_mul (src);

	for (;;) {
		switch (** src) {
		case '+':
			(* src) ++;
			left += eval_mul (src);
			break;

		case '-':
			(* src) ++;

			left -= eval_mul (src);
			break;

		default:
			return left;
		}
	}
}


/*
 * Evaluate a <, <=, >, >= part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_shift (source_t src)
#else
LOCAL long
eval_shift (src)
source_t	src;
#endif
{
	long		left = eval_add (src);

	for (;;) {
		if (strncmp (* src, "<<", 2) == 0) {
			(* src) += 2;
			left <<= eval_add (src);
		} else if (strncmp (* src, ">>", 2) == 0) {
			(* src) += 2;
			left >>= eval_add (src);
		} else
			return left;
	}
}


/*
 * Evaluate a <, <=, >, >= part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_relat (source_t src)
#else
LOCAL long
eval_relat (src)
source_t	src;
#endif
{
	long		left = eval_shift (src);

	for (;;) {
		switch (** src) {
		case '<':
			(* src) ++;
			if (** src == '=') {
				(* src) ++;
				left = left <= eval_shift (src);
				break;
			}
			left = left < eval_shift (src);
			break;

		case '>':
			(* src) ++;
			if (** src == '=') {
				(* src) ++;
				left = left >= eval_shift (src);
				break;
			}
			left = left > eval_shift (src);
			break;

		default:
			return left;
		}
	}
}


/*
 * Evaluate an ==, != part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_equals (source_t src)
#else
LOCAL long
eval_equals (src)
source_t	src;
#endif
{
	long		left = eval_relat (src);

	for (;;) {
		switch (** src) {
		case '!':
			if ((* src) [1] != '=')
				return left;
			(* src) += 2;
			left = left != eval_relat (src);
			break;

		case '=':
			if ((* src) [1] != '=')
				return left;
			(* src) += 2;

			left = left == eval_relat (src);
			break;

		default:
			return left;
		}
	}
}


/*
 * Evaluate an & part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_bitand (source_t src)
#else
LOCAL long
eval_bitand (src)
source_t	src;
#endif
{
	long		left = eval_equals (src);

	for (;;) {
		if (** src != '&' || (* src) [1] == '&')
			return left;

		(* src) ++;
		left &= eval_equals (src);
	}
}


/*
 * Evaluate an ^ part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_bitxor (source_t src)
#else
LOCAL long
eval_bitxor (src)
source_t	src;
#endif
{
	long		left = eval_bitand (src);

	for (;;) {
		if (** src != '^')
			return left;

		(* src) ++;
		left ^= eval_bitand (src);
	}
}


/*
 * Evaluate an | part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_bitor (source_t src)
#else
LOCAL long
eval_bitor (src)
source_t	src;
#endif
{
	long		left = eval_bitxor (src);

	for (;;) {
		if (** src != '|' || (* src) [1] == '|')
			return left;

		(* src) ++;
		left |= eval_bitxor (src);
	}
}


/*
 * Evaluate an && part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_logand (source_t src)
#else
LOCAL long
eval_logand (src)
source_t	src;
#endif
{
	long		left = eval_bitor (src);

	for (;;) {
		if (strncmp (* src, "&&", 2) != 0)
			return left;

		(* src) += 2;

		/*
		 * We let the C && operator do all the work for us.
		 */

		left = eval_bitor (src) && left;
	}
}


/*
 * Evaluate an || part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_logor (source_t src)
#else
LOCAL long
eval_logor (src)
source_t	src;
#endif
{
	long		left = eval_logand (src);

	for (;;) {
		if (strncmp (* src, "||", 2) != 0)
			return left;

		(* src) += 2;

		/*
		 * We let the C || operator do all the work for us.
		 */

		left = eval_logand (src) || left;
	}
}


/*
 * Evaluate a ?: part of an expression.
 */

#if	USE_PROTO
LOCAL long eval_cond (source_t src)
#else
LOCAL long
eval_cond (src)
source_t	src;
#endif
{
	long		tmp = eval_logor (src);
	long		left;
	long		right;

	if (** src != '?')
		return tmp;

	(* src) ++;

	if (** src != ':') {
		left = eval_cond (src);
	} else
		left = tmp;

	if (** src != ':')
		right = tmp;
	else {
		(* src) ++;
		right = eval_cond (src);
	}

	return tmp ? left : right;
}


/*
 * Evaluate a $(()) expression.
 */

#if	USE_PROTO
LOCAL void eval_expr (eval_t * evalp)
#else
LOCAL void
eval_expr (evalp)
eval_t	      *	evalp;
#endif
{
	long		number;
	unsigned char	buf [sizeof (int) * 3 + 1];
	CONST unsigned char   *	tmp;
	
	eval_push (evalp);

	EVAL_SET_GLOB (evalp, 0);
	EVAL_SET_QUOTING (evalp, EVAL_FORCE_QUOTE);

	eval0 (evalp);
	add_arg (evalp, 0);

	tmp = eval_end (evalp);

	number = eval_cond (& tmp);

	if (* tmp != 0 && ! errflag)
		shellerr_simple ("junk at end of expression", tmp);

	eval_pop (evalp);

	if (errflag)
		eval_abort (evalp);

	tmp = ltoa (buf, sizeof (buf), number, 10, 0);

	while (* tmp != 0)
		add_char (evalp, * tmp ++);
}


/*
 * Complain about an undefined variable.
 */

#if	USE_PROTO
LOCAL void enotdef (CONST char * var)
#else
LOCAL void
enotdef (var)
CONST char    *	var;
#endif
{
	shellerr_simple ("variable not defined", var);
}


/*
 * Read the name of a shell variable and perform the appropriate substitution.
 */

#if	USE_PROTO
LOCAL void variable (eval_t * evalp)
#else
LOCAL void
variable (evalp)
eval_t	      *	evalp;
#endif
{
	int		s;
	int		c;
	int		count;
	int		patt_mode;
	unsigned char *	input_end;

	s = 0;

	switch (c = EVAL_NEXT_CHAR (evalp)) {
	case '{':
		break;

	case '(':
		/*
		 * Invoke $() command and $(()) arithmetic expansions.
		 */

		if ((c = EVAL_NEXT_CHAR (evalp)) == '(') {
			if (EVAL_NEXT_CHAR (evalp) != 0)
				panic (15);

			input_end = EVAL_GET_CSTREND (evalp);
			eval_expr (evalp);
			goto done;
		}
		EVAL_UNGET_CHAR (evalp, c);

		graves (evalp, 0);
		return;

	default:
		if (special (evalp, c, SPEC_EXPAND) == -1) {
			if (uflag) {
				eval_end (evalp);
				enotdef (evalp->e_last);
			}
			eval_pop (evalp);
			EVAL_ARGUMENT_IF_QUOTED (evalp);
		}
		return;
	}

	if (EVAL_NEXT_CHAR (evalp) != 0)
		panic (15);
	input_end = EVAL_GET_CSTREND (evalp);

	if ((c = EVAL_NEXT_CHAR (evalp)) == '#') {
		/*
		 * Measure length of expansion.
		 */

		if ((c = EVAL_NEXT_CHAR (evalp)) == 0) {
			EVAL_UNGET_CHAR (evalp, c);
			c = '#';	/* make ${#} -> $# */
		}
		if (special (evalp, c, SPEC_LENGTH) == -1)
			panic (20);
		goto done;
	}

	count = special (evalp, c, SPEC_CURLY);
	patt_mode = 0;

	/*
	 * For some of the expansions, doing it in a nested context
	 * and copying will not work; this is mainly true of '+' and
	 * '-', where we must take great care to preserve the
	 * semantics of e.g. "$@" in the alternate part.
	 */

	switch (s = EVAL_NEXT_CHAR (evalp)) {
	case '%':
		if ((c = EVAL_NEXT_CHAR (evalp)) == '%')
			s = '^';	/* something distinct */
		else
			EVAL_UNGET_CHAR (evalp, c);
		patt_mode = 1;
		count = -1;
		add_arg (evalp, 0);
		break;

	case '#':
		if ((c = EVAL_NEXT_CHAR (evalp)) == '#')
			s = '$';	/* something distinct */
		else
			EVAL_UNGET_CHAR (evalp, c);
		patt_mode = 1;
		count = -1;
		add_arg (evalp, 0);
		break;

	case '?':
		if (count == -1)
			add_arg (evalp, 0);
		break;

	case '-':
		if (count == -1) {
			eval_pop (evalp);
			eval0 (evalp);
		}
		goto done;

	case '=':
		if (count == -1)
			add_arg (evalp, '=');
		break;

	case '+':
		/*
		 * Code in the special () function has arranged for the
		 * expansion of the parameter to be left on the sub-context
		 * rather than on the top context. We'll discard it either
		 * way.
		 */

		eval_pop (evalp);
		if (count != -1)
			eval0 (evalp);
		goto done;

	case 0:
		if (count == -1) {
			if (uflag) {
				eval_end (evalp);
				enotdef (evalp->e_last);
			}
			eval_pop (evalp);
			count = 0;
		}
		break;

	default:
		shellerr_simple ("syntax error", "illegal ${} expansion");
		eval_abort (evalp);
	}

	if (count < 0) {
		int		alternate_pos;
		unsigned char *	name_start;
		unsigned char *	alternate_value;
		unsigned char *	temp;

		/*
		 * Now expand whatever follows, up to the closing curly. We
		 * do the expansion in EWORD mode or EPATT mode.
		 */

		alternate_pos = build_offset (evalp->e_heap);

		EVAL_NO_ARGUMENT (evalp);
		EVAL_SET_ARGS (evalp, 0);
		EVAL_SET_QUOTEMODE (evalp, MNQUO);

		if (patt_mode) {
			EVAL_SET_GLOB (evalp, 1);
			EVAL_SET_QUOTING (evalp, EVAL_NO_QUOTE);
		} else {
			EVAL_SET_GLOB (evalp, 0);
			EVAL_SET_QUOTING (evalp, EVAL_FORCE_QUOTE);
		}

		eval0 (evalp);
		add_arg (evalp, 0);

		name_start = eval_end (evalp);
		alternate_value = name_start + alternate_pos;

		switch (s) {
		case '=':
			if (! class (* name_start, MBVAR)) {
				shellerr_simple ("illegal substitution",
						 name_start);
				return;
			}
			setsvar (name_start);
			break;

		case '?':
			if (* alternate_value != 0) {
				shell_outstr (alternate_value);
				shell_endl ();
			} else
				enotdef (name_start);

			eval_pop (evalp);
			eval_end (evalp);
			eval_abort (evalp);

		case '%':	/* remove smallest suffix */
			for (temp = alternate_value ; -- temp >= name_start ;)
				if (match (alternate_value, temp)) {
					* temp = 0;
					break;
				}
			alternate_value = name_start;
			break;

		case '^':	/* code for '%%', remove largest suffix */
			for (temp = name_start ; * temp ; temp ++)
				if (match (alternate_value, temp)) {
					* temp = 0;
					break;
				}
			alternate_value = name_start;
			break;

		case '#':	/* remove smallest prefix */
			for (temp = name_start ; temp < alternate_value ;
			     * temp ++ = c) {
				c = * temp;
				* temp = 0;
				if (match (alternate_value, name_start)) {
					* (name_start = temp) = c;
					break;
				}
			}
			alternate_value = name_start;
			break;

		case '$':	/* code for '##', remove largest prefix */
			for (temp = alternate_value ; -- temp >= name_start ;
			     * temp = c) {
				c = * temp;
				* temp = 0;
				if (match (alternate_value, name_start)) {
					* (name_start = temp) = c;
					break;
				}
			}
			alternate_value = name_start;
			break;
		}

		/*
		 * Copy new value down into expansion position, and perform
		 * whatever glob-expansion and fieldname splitting is required
		 * by our context.
		 */

		eval_pop (evalp);
		alternate_value = temp = duplstr (alternate_value, 1);
		while (c = * temp ++)
			add_char (evalp, c);
		sfree (alternate_value);
	}

done:
	EVAL_SEEK_INPUT (evalp, input_end);
	if (EVAL_NEXT_CHAR (evalp) != 0)
		panic (17);
	EVAL_ARGUMENT_IF_QUOTED (evalp);
}


/*
 * Main expander loop.
 */

LOCAL void
eval0 (evalp)
eval_t	      *	evalp;
{
	unsigned char	c;

	while ((c = EVAL_NEXT_CHAR (evalp)) != 0) {
		if (! class (c, EVAL_QUOTEMODE (evalp))) {
			add_char (evalp, c);
			continue;
		}

		switch (c) {

		case '"':	/* m == MNQUO || m == MDQUO */
			switch (EVAL_QUOTEMODE (evalp)) {
			case MNQUO:
				EVAL_SET_QUOTEMODE (evalp, MDQUO);
				EVAL_ADD_QUOTE (evalp, EVAL_QUOTED);

				if ((c = EVAL_NEXT_CHAR (evalp)) == '"')
					EVAL_MADE_ARGUMENT (evalp);

				EVAL_UNGET_CHAR (evalp, c);
				break;

			case MDQUO:
				EVAL_ADD_QUOTE (evalp, 0);
				EVAL_SET_QUOTEMODE (evalp, MNQUO);
				break;

			default:
				panic (13);
			}
			break;

		case '\'':	/* m == MNQUO || m == MBQUO */
			if (EVAL_QUOTEMODE (evalp) == MBQUO)
				add_arg (evalp, '\'');

			while ((c = EVAL_NEXT_CHAR (evalp)) != '\'' && c != 0)
				add_quoted (evalp, c);

			if (EVAL_QUOTEMODE (evalp) == MBQUO)
				add_arg (evalp, '\'');

			EVAL_MADE_ARGUMENT (evalp);
			break;

		case '\\':	/* m == MDQUO || m == MNQUO || m == MBQUO */
			c = EVAL_NEXT_CHAR (evalp);
			if (EVAL_QUOTEMODE (evalp) != MNQUO &&
			    ! class (c, EVAL_QUOTEMODE (evalp))) {
				add_char (evalp, '\\');
				add_char (evalp, c);
			} else
				add_quoted (evalp, c);
			EVAL_MADE_ARGUMENT (evalp);
			break;

		case '$':	/* m == MNQUO || m == MDQUO || m == MBQUO */
			variable (evalp);
			break;

		case '`':	/* m == MNQUO || m == MDQUO */
			graves (evalp, 1);
			break;

		default:
			add_char (evalp, c);
			break;
		}
	}
}


/*
 * Add a character to the current argument.
 * If no quotation is set, pick off blanks and globs.
 */

LOCAL void
add_char (evalp, c)
eval_t	      *	evalp;
unsigned char	c;
{
	if (EVAL_IS_QUOTED (evalp) == 0) {
		if (EVAL_SPLIT_ARGS (evalp) && strchr (vifs, c) != NULL) {
			end_arg (evalp);
			return;
		}
		if (! nopathexp_flag && EVAL_IS_GLOBSEEK (evalp) &&
		    class (c, MGLOB)) {
			add_arg (evalp, c);
			EVAL_MADE_ARGUMENT (evalp);
			return;
		}
	}
	add_quoted (evalp, c);
}


/*
 * Add a quoted character to the current argument.
 * if argg is set, then glob characters are quoted with a \,
 * as well as \ itself.
 */

LOCAL void
add_quoted (evalp, c)
eval_t	      *	evalp;
unsigned char	c;
{
	if (EVAL_IS_GLOBSEEK (evalp) && (class (c, MGLOB) || c == '\\'))
		add_arg (evalp, '\\');

	add_arg (evalp, c);
	EVAL_MADE_ARGUMENT (evalp);
}


/*
 * Add a character to the current argument
 */

LOCAL void
add_arg (evalp, c)
eval_t	      *	evalp;
unsigned char	c;
{
	builderr_t	error;

	if ((error = build_addchar (evalp->e_heap, c)) != BUILD_OK) {
		shellerr_simple ("internal error", build_error (error));
		eval_abort (evalp);
	}
}


/*
 * Terminate the current argument if it is non-empty.
 * If argg is set, then glob the argument to expand globs
 * or to simply remove any quotes.
 */

LOCAL void
end_arg (evalp)
eval_t	      *	evalp;
{
	char	      *	temp;
	builderr_t	error;

	if (EVAL_IS_NO_ARGUMENT (evalp))
		return;

	add_arg (evalp, 0);
	if ((temp = build_end (evalp->e_heap, NULL)) == NULL) {
		shellerr_simple ("internal error", "unable to end argument");
		eval_abort (evalp);
	}

	if (EVAL_IS_GLOBSEEK (evalp))
		glob1 (duplstr (temp, 0));
	else {
		nargv = addargl (nargv, temp, 0);
		nargc += 1;
	}

	EVAL_NO_ARGUMENT (evalp);

	if ((error = build_release (evalp->e_heap, temp)) != BUILD_OK) {
		shellerr_simple ("build_release ()", build_error (error));
		eval_abort (evalp);
	}
	if ((error = build_begin (evalp->e_heap, 0, NULL)) != BUILD_OK) {
		shellerr_simple ("build_begin ()", build_error (error));
		eval_abort (evalp);
	}
}


LOCAL void
eval_push (evalp)
eval_t * evalp;
{
	builderr_t	error;

	if ((error = build_push (evalp->e_heap, EVAL_STYLE (evalp),
				 sizeof (struct eval_style))) != BUILD_OK) {
		shellerr_simple ("build_push ()", build_error (error));
		eval_abort (evalp);
	}

	if ((error = build_begin (evalp->e_heap, 0, NULL)) != BUILD_OK) {
		shellerr_simple ("build_begin ()", build_error (error));
		eval_abort (evalp);
	}
}


LOCAL void
eval_pop (evalp)
eval_t * evalp;
{
	builderr_t	error;
	size_t		size = sizeof (struct eval_style);

	if ((error = build_pop (evalp->e_heap, EVAL_STYLE (evalp),
				& size)) != BUILD_OK) {
		shellerr_simple ("build_pop ()", build_error (error));
		eval_abort (evalp);
	}
}
