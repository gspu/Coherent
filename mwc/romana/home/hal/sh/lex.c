/*
 * Bourne shell.
 * Lexical analysis.
 */

#include <stdlib.h>

#include "shellio.h"
#include "sh.h"
#include "y.tab.h"
#include "buildobj.h"

#define	LEX_EOF		-1


/*
 * Local externals.
 */

build_t	      *	lex_heap;
int		lastget = 0;		/* Pushed back character */


/*
 * For processing here documents.
 */

typedef struct here	here_t;

struct here {
	here_t	      *	h_next;		/* next here-document request */
	char	      *	h_eof;		/* document EOF mark */
	int		h_fd;		/* document fd */
	char	      *	h_tmp;		/* document tempfile name */
	int		h_quoted;	/* document quoted */
	int		h_tabs;		/* remove leading tabs */
};

here_t	      *	here_list;
here_t	      *	last_here;


/*
 * Keyword table.
 */

struct	key {
	int		k_lexv;		/* Lexical value */
	char	      *	k_name;		/* Keyword name */
	int		k_keyf;		/* can be followed by a keyword */
	int		k_hash;		/* Hash */
} keytab [] = {
	{ _CASE,	"case",   0 },
	{ _DO,		"do",	  1 },
	{ _DONE,	"done",	  1 },
	{ _ELIF,	"elif",   1 },
	{ _ELSE,	"else",   1 },
	{ _ESAC,	"esac",   1 },
	{ _FI,		"fi",	  1 },
	{ _FOR,		"for",    0 },
	{ _IF,		"if",	  1 },
	{ _IN,		"in",	  0 },
	{ _RET,		"return", 0 },
	{ _THEN,	"then",	  1 },
	{ _UNTIL,	"until",  1 },
	{ _WHILE,	"while",  1 },
	{ _OBRAC,	"{",      1 },
	{ _CBRAC,	"}",	  1 },
	{ _NOT,		"!",	  1 }
};
#define	NKEYS	(sizeof (keytab) / sizeof (keytab [0]))

void		read_here	();

CSTRING	      *	sh_yytext;


/*
 * Complain about missing characters.
 */

#if	USE_PROTO
LOCAL void emisschar (char ch)
#else
LOCAL void
emisschar (ch)
char		ch;
#endif
{
	if (shellerr_begin ("missing character")) {
		shellerr_outchar ('\'');
		shellerr_outchar (ch);
		shellerr_outchar ('\'');
		shellerr_endl ();
	}
}


/*
 * Unget a character.
 */

#if	USE_PROTO
void ungetn (char c)
#else
void
ungetn(c)
char		c;
#endif
{
	lastget = c;
}


#if	USE_PROTO
LOCAL void lex_alloc (void)
#else
LOCAL void
lex_alloc ()
#endif
{
	if (lex_heap == NULL) {
		if ((lex_heap = builder_alloc (512, 0)) == NULL) {
			shellerr_simple ("out of memory",
					 "unable to get lexer heap");
			reset (RUABORT);
		}
	}
}

#if	USE_PROTO
void lex_release (void)
#else
void
lex_release ()
#endif
{
	if (sh_yytext != NULL) {
		builderr_t	error;

		if ((error = build_release (lex_heap,
					    sh_yytext)) != BUILD_OK) {
			shellerr_simple ("lex_release ()",
					 build_error (error));
			reset (RUABORT);
		}
		sh_yytext = NULL;
	}
}


/*
 * Set up the global context for lexing.
 */

#if	USE_PROTO
void lex_init (void)
#else
void
lex_init ()
#endif
{
	builderr_t	error;

	lex_alloc ();
	lex_release ();

	if ((error = build_begin (lex_heap, sizeof (size_t),
				  INIT_ZERO)) != BUILD_OK) {
		shellerr_simple ("lex_init ()", build_error (error));
		reset (RUABORT);
	}
}


/*
 * Finish up lexing of a token.
 */

#if	USE_PROTO
char * lex_done (void)
#else
char *
lex_done ()
#endif
{
	size_t		length;

	sh_yytext = build_end (lex_heap, & length);
	* (size_t *) sh_yytext = length - sizeof (size_t);
	return CSTRING_STRING (sh_yytext);
}


/*
 * Add some data to the token.
 */

#if	USE_PROTO
void lex_add (unsigned char c)
#else
void
lex_add (c)
unsigned char	c;
#endif
{
	builderr_t	error;

	if ((error = build_addchar (lex_heap, c)) != BUILD_OK) {
		shellerr_simple ("lex_add ()", build_error (error));
		reset (RUABORT);
	}
}


/*
 * Add a string to the output token.
 */

#if	USE_PROTO
void lex_add_string (CONST char * str)
#else
void
lex_add_string (str)
CONST char    *	str;
#endif
{
	builderr_t	error;

	if ((error = build_add (lex_heap, strlen (str), str)) != BUILD_OK) {
		shellerr_simple ("lex_add_string ()", build_error (error));
		reset (RUABORT);
	}
}


/*
 * Trim the last character from the token.
 */

#if	USE_PROTO
void lex_unadd (void)
#else
void
lex_unadd ()
#endif
{
	builderr_t	error;

	if ((error = build_reduce (lex_heap, 1)) != BUILD_OK) {
		shellerr_simple ("lex_unadd ()", build_error (error));
		reset (RUABORT);
	}
}


#if	USE_PROTO
void lex_push (void)
#else
void
lex_push ()
#endif
{
	builderr_t	error;

	/*
	 * This can be the first entry to the lexer, by way of prompt ().
	 */

	lex_alloc ();
	lex_release ();

	if ((error = build_push (lex_heap, & keyflag,
				 sizeof (keyflag))) != BUILD_OK) {

		shellerr_simple ("lex_push ()", build_error (error));
		reset (RUABORT);
	}
	keyflag = 0;
}


#if	USE_PROTO
void lex_pop (void)
#else
void
lex_pop ()
#endif
{
	builderr_t	error;
	size_t		count = sizeof (keyflag);

	lex_release ();
	if ((error = build_pop (lex_heap, & keyflag, & count)) != BUILD_OK) {

		shellerr_simple ("lex_pop ()", build_error (error));
		reset (RUABORT);
	}
}


#if	USE_PROTO
char * lex_getstart (size_t * lengthp)
#else
char *
lex_getstart (lengthp)
size_t	      *	lengthp;
#endif
{
	char	      *	where;
	builderr_t	error;

	if ((where = build_end (lex_heap, lengthp)) == NULL) {
		shellerr_simple ("lex_getstart ()", "cannot halt build");
		reset (RUABORT);
	}

	if ((error = build_resume (lex_heap)) != BUILD_OK) {
		shellerr_simple ("lex_getstart ()", build_error (error));
		reset (RUABORT);
	}

	if (lengthp)
		* lengthp -= sizeof (size_t);

	return where + sizeof (size_t);
}


/*
 * Get the next lexical token. Here we eat whitespace and do similar nasty
 * things to remove things of no interest to the parser.
 */

#if	USE_PROTO
int yylex (void)
#else
int
yylex ()
#endif
{
	int		c;

	for (;;) {
		if ((c = getn ()) == ' ' || c == '\t')
			continue;

		/*
		 * Escaped newlines are whitespace to be consumed here as
		 * well. Unescaped newlines are important in the grammar.
		 */

		if (c == '\\') {
			if ((c = getn ()) == '\n')
				continue;

			if (c >= 0)
				ungetn (c);
			c = '\\';
		}

		break;
	}

	if (c == '#') {
		/*
		 * Ignore a '#'-delimited comment line. Note that the end-of-
		 * line character passed on to later stages of this routine
		 * to trigger here-document processing. POSIX.2 says that the
		 * end-of-line terminating a comment is the real thing.
		 */

		do
			c = getn ();
		while (c > 0 && c != '\n');
	}

	lex_init ();
	c = do_lex (c, LEX_REDIRS);
	(void) lex_done ();

	return c;
}


/*
 * This entry point into the lexical analyser is here to support lexing of the
 * POSIX.2 $() construct (not in quite the full form, see getparens ()) by
 * doing all the lexing stuff except that we append to the existing lexed
 * token rather than starting anew.
 */

int
do_lex (c, flag)
int		c;
int		flag;
{
	struct key    *	kp;

	/*
	 * Initialize the keywords on the first entry to the lexer.
	 */

	if (keytab [0].k_hash == 0)
		for (kp = keytab ; kp < keytab + NKEYS ; kp ++)
			kp->k_hash = ihash (kp->k_name);

	if (c > 0)
		lex_add (c);

	if (class (c, MDIGI)) {
		if ((c = getn ()) >= 0)
			lex_add (c);

		keyflag = 0;
		return (c == '>' || c == '<') &&
			(flag & LEX_REDIRS) != 0 ? lexiors (c) :
						   lexname (c, flag);
	}

	if (! class (c, MNAME)) {
		int		hash;
		char	      *	yytext;

		if ((c = lexname (c, flag)) < 0)
			return c;
		yytext = lex_getstart (NULL);

		if (keyflag) {
			hash = ihash (yytext);

			for (kp = keytab; kp < keytab + NKEYS ; kp ++) {
				if (hash == kp->k_hash &&
				    strcmp (yytext, kp->k_name) == 0) {

					keyflag = kp->k_keyf;
					return kp->k_lexv;
				}
			}
			keyflag = 0;
		}

		return c;
	}

	/*
	 * This needs to be after any calls we make to lexname (), because
	 * otherwise we stand to overwrite part of the global string buffer
	 * that is being passed to us as input from graves () ... yuck.
	 */

	lex_add (0);
	lex_unadd ();

	switch (c) {
	case ';':
		c = isnext (c, _DSEMI, c);
		break;

	case '>':
	case '<':
		return (flag & LEX_REDIRS) != 0 ? lexiors (c) : c;

	case '&':
		c = isnext (c, _ANDF, c);
		break;

	case '|':
		c = isnext (c, _ORF, c);
		break;

	case '(':
		c = isnext (')', _PARENS, c);
		break;

	case LEX_EOF:
	case '\n':
		if ((flag & LEX_REDIRS) != 0) {
			while (here_list != NULL)
				read_here ();
		}
		break;

	default:
		break;
	}

	if ((flag & ~ LEX_REDIRS) == 0)
		keyflag = 1;

	return c;
}

void read_here () {
	int		c;

	for (;; lex_pop ()) {
		char	      *	begin;
		size_t		length;

		lex_push ();

		if ((c = build_begin (lex_heap, 0, NULL)) != BUILD_OK) {
			shellerr_simple ("read_here ()", build_error (c));
			reset (RUABORT);
		}

		if (collect ('\n', here_list->h_quoted ? NO_BACKSLASH :
							 BACKSLASH_END) < 0)
			break;

		begin = build_end (lex_heap, & length);

		/*
		 * Change the '\n' to a NUL for a while.
		 */

		* (begin + length - 1) = 0;

		if (here_list->h_tabs)
			while (* begin && * begin == '\t') {
				begin ++;
				length --;
			}

		if (strcmp (begin, here_list->h_eof) == 0) {
			lex_pop ();
			break;
		}

		if (here_list->h_fd < 0) /*  || * begin == 0) */
			continue;

		if (! here_list->h_quoted && * begin == '\\' &&
		    strcmp (here_list->h_eof, begin + 1) == 0) {
		    	begin ++;
		    	length --;
		}

		/*
		 * Change the NUL back to a '\n'.
		 */

		* (begin + length - 1) = '\n';
		write (here_list->h_fd, begin, length);
	}

	close (here_list->h_fd);
	remember_temp (here_list->h_tmp);

	/*
	 * Unlink and release the here structure
	 */

	{
		struct here   *	temp = here_list;
		if ((here_list = here_list->h_next) == NULL)
			last_here = NULL;
		sfree (temp);
	}
}

isnext (c, newtok, oldtok)
int	c;
int	newtok;
int	oldtok;
{
	int c2;

	if ((c2 = getn ()) == c) {
		lex_add (c2);
		lex_add (0);
		lex_unadd ();
		return newtok;
	}
	ungetn (c2);
	return oldtok;
}


/*
 * Begin a substring.
 */

size_t
begin_substring ()
{
	int		error;

	lex_add (0);		/* flag a counted substring */

	if ((error = build_add (lex_heap, sizeof (size_t),
				NULL)) != BUILD_OK) {
		shellerr_simple ("begin_substring ()", build_error (error));
		reset (RUABORT);
	}

	return build_offset (lex_heap) - sizeof (size_t) * 2;
}


/*
 * End a counted substring. A counted substring ALWAYS has a 0 terminator
 * after the substring, not included in the count. This extra 0 is part of
 * the embedding protocol, and there must be one for every substring.
 */

CSTRING *
end_substring (string_start)
size_t		string_start;
{
	char	      *	where;
	int		abort;
	size_t		length;

	lex_unadd ();
	lex_add (0);

	where = lex_getstart (& length);
	if (length < string_start)
		panic (21);
	* (size_t *) (where + string_start) = length - string_start -
						sizeof (size_t) - 1;

	return (CSTRING *) (where + string_start);
}


/*
 * Collect a $-expansion
 */

void		get_curlies ();
void		get_parens ();

#if	USE_PROTO
void get_dollar (int flag)
#else
void
get_dollar (flag)
int		flag;
#endif
{
	int		c;

	if ((c = getn ()) == '{') {
		lex_add (c);
		get_curlies (flag);
	} else if (c == '(') {
		if (udo_dammit) {
			/*
			 * Replace the '$' with '\$'.
			 */

			lex_unadd ();
			lex_add ('\\');
			lex_add ('$');
			ungetn (c);
		} else {
			lex_add (c);
			get_parens (flag);
		}
	} else if (c >= 0)
		ungetn (c);
}


/*
 * Get stuff delimited by a close-paren. Here we recursively call one of the
 * high-level lexing functions so that we store the text resulting from here-
 * document processing, '#' comment removal and quoting. This allows us to
 * match only a ')' that would be spare as the result of a parse *except* that
 * we match the first parenthesis in the following situation:
 *	$( case word in ; foo) bar ;; esac )
 * POSIX.2 draft 11 suggests in the $() rationale that we should be able to
 * match the second, but gives no useful rules for doing so. Rather than do
 * this with a full parse (with the attendant difficulties of making the
 * parser and lexer interact properly), we just use the lexer. We observe that
 * writing the case-expression so:
 *	$( case word in ; (foo) bar ;; esac )
 * solves the problem, even though POSIX.2 draft 11 says that this isn't
 * necessary (section 3.9.4.3.1).
 *
 * Here we also deal with recognizing the difference between $(()) and $().
 */

#if	USE_PROTO
void get_parens (int flag)
#else
void
get_parens (flag)
int		flag;
#endif
{
	int		c;
	int		doubleparens;
	size_t		substring = begin_substring ();
	CSTRING	      *	cstrp;
	int		parencount;

	if ((doubleparens = (c = getn ()) == '(') == 0)
		ungetn (c);
	else
		lex_add (' ');	/* flag for paren type */

	parencount = 1;
	for (;;) {
		if ((c = do_lex (getn (), LEX_QUOTED)) < 0) {
			emisschar (')');
			break;
		}

		if (c == ')') {
			if (-- parencount != 0)
				continue;

			if (doubleparens == 1) {
				if ((c = getn ()) == ')')
					break;

				/*
				 * Something like $((foo);(bar))
				 */

				ungetn (c);
				doubleparens = -1;
				parencount = 1;
			} else
				break;
		} else if (c == '(')
			parencount ++;
	}

	cstrp = end_substring (substring);

	switch (doubleparens) {
	case 1:
		/*
		 * Play some games; move the substring forward by one byte
		 * so that outer-level code sees $((<nul>
		 */

		* (size_t *) ((char *) cstrp + 1) = * (size_t *) cstrp - 1;
		((char *) cstrp) [- 1] = '(';
		* (char *) cstrp = 0;
		break;

	case -1:
		* CSTRING_STRING (cstrp) = '(';
		break;

	default:
		break;
	}
}


/*
 * Collect a grave sequence.
 */

#if	USE_PROTO
void get_graves (int flag, int mode)
#else
void
get_graves (flag, mode)
int		flag;
int		mode;
#endif
{
	size_t		substring = begin_substring ();
	int		c;

	for (;;) {
		if ((c = getn ()) < 0 /* || c == '\n'*/) {
			if (c >= 0)
				emisschar ('`');
			lex_add ('`');
			(void) end_substring (substring);
			return;
		}

		lex_add (c);

		switch (c) {
		case '\\':
			if ((c = getn ()) < 0) {
				syntax ("backslash at end of input");
				continue;
			}
			if (c == '\n') {
				lex_unadd ();
				continue;
			}

			/*
			 * If we are in double-quotes or a here-document,
			 * strip the backslashes out now.
			 */

			if (((flag & LEX_HERE) != 0 && class (c, MDQUO)) ||
			    (mode == MDQUO && class (c, mode)))
				lex_unadd ();

			lex_add (c);
			continue;

		case '$':
			get_dollar (flag);
			continue;

		case '`':
			(void) end_substring (substring);
			return;
		}
	}
}


/*
 * Read stuff delimited by (possibly nested) '{' '}' pairs.
 */

#if	USE_PROTO
void get_curlies (int flag)
#else
void
get_curlies (flag)
int		flag;
#endif
{
	int		c;
	int		quote = 0;
	size_t		substring = begin_substring ();

	for (;;) {
		if ((c = getn ()) < 0 || c == '\n') {
			emisschar ('}');
			(void) end_substring (substring);
			return;
		}

		lex_add (c);

		switch (c) {
		case '}':
			if (! quote) {
				(void) end_substring (substring);
				return;
			}
			continue;

		case '"':
			quote ^= 1;
			continue;

		case '\'':
			if ((c = collect ('\'', NO_BACKSLASH)) != '\'')
				continue;
			continue;

		case '\\':
			if ((c = getn ()) < 0) {
				syntax ("backslash at end of input");
				continue;
			}
			if (c == '\n') {
				lex_unadd ();
				continue;
			}
			lex_add (c);
			continue;

		case '$':
			get_dollar (flag);
			continue;

		case '`':
			get_graves (flag, MDQUO);
			continue;
		}
	}
}


/*
 * Scan a single argument.
 *	Return LEX_EOF if LEX_EOF is found, or _NAME or _ASGN if any part of an
 *	argument is found.
 */

int
lexname (c, flag)
int		c;
int		flag;
{
	int		quote;
	int		asgn;
	int		mode;

	quote = 0;
	asgn = 0;
	mode = (flag & (LEX_QUOTED | LEX_HERE)) != 0 ? MHERE : MNQUO;

	if (c >= 0)
		lex_unadd ();

	for (;; c = getn ()) {
		/*
		 * State machine for recognizing {MBVAR}{MRVAR}*'='
		 */

		if (asgn == 0)
			asgn = class (c, MBVAR) ? 1 : -1;
		else if (asgn == 1)
			asgn = class (c, MRVAR) ? 1 : c == '=' ? 2 : -1;

		lex_add (c);

		if (! class (c, mode))
			continue;

		switch (c) {
		case '"':
			mode = (quote ^= 1) ? MDQUO : MNQUO;
			continue;

		case '\'':
			c = collect ('\'', NO_BACKSLASH);
			continue;

		case '\\':
			if ((c = getn ()) < 0) {
				syntax ("backslash at end of input");
				break;
			}
			if (c == '\n') {
				lex_unadd ();
				ungetn ((c = getn ()) < 0 ? '\n' : c);
				continue;
			}
			lex_add (c);
			continue;

		case '$':
			get_dollar (flag);
			continue;

		case '`':
			get_graves (flag, mode);
			continue;

		case '\n':
			if (quote)
				continue;
			break;
		}
		break;
	}

	if (quote) {
		/*
		 * As usual, assume things are closed off at EOF.
		 */

		if (c >= 0)
			emisschar ('"');
		lex_add (0);
		lex_unadd ();
	} else {
		lex_unadd ();
		lex_add (0);
		lex_unadd ();
	}

	if (c >= 0)
		ungetn (c);

	if (errflag)
		return _NULL;
	else if (asgn == 2)
		return _ASGN;
	else
		return _NAME;
}

/*
 * Lex an io redirection string, including the file name if any.
 *	Called with one '>' or '<' in buffer, optionally preceded by
 *	a digit.
 */

enum {
	REGULAR,		/* '>' or '<' */
	APPEND_OUT,		/* '>>' */
	CLOBBER_OUT,		/* '>|' */
	REDIR_IO,		/* '<>' */
	READ_HERE,		/* '<<' */
	READ_HERE_TAB		/* <<-' */
};

lexiors (c1)
int		c1;
{
	int		c;
	int		redir;
	size_t		name;
	size_t		length;
	char	      *	iors;
	struct here   *	new_here;

	lex_add (c = getn ());

	if (c == '&') {
		do {
			if ((c = getn ()) < 0)
				return c;
		} while (c == ' ' || c == '\t');

		lex_add (c);
		lex_add (0);
		lex_unadd ();

		if (c != '-' && ! class (c, MDIGI)) {
			eredir ("invalid descriptor");
			return -1;
		}

		return _IORS;
	}

	/*
	 * Work out exactly what kind of iors we are dealing with here. Note
	 * that in the '<<-' case the '-' is never stored in the output iors.
	 */

	if (c1 == '>' && c == '>')
		redir = APPEND_OUT;
	else if (c1 == '>' && c == '|')
		redir = CLOBBER_OUT;
	else if (c1 == '<' && c == '>')
		redir = REDIR_IO;
	else if (c1 == '<' && c == '<') {
		if ((c = getn ()) != '-') {
			ungetn (c);
			redir = READ_HERE;
		} else
			redir = READ_HERE_TAB;
	} else {
		redir = REGULAR;
		lex_unadd ();
		lex_add (0);
		lex_unadd ();
		ungetn (c);
	}

	/* Collect file name */
	do
		c = getn ();
	while (c == ' ' || c == '\t');

	if (c == '\n') {
		eredir ("missing filename/EOF string");
		return _IORS;
	}

	name = build_offset (lex_heap) - sizeof (size_t);

	lex_add (c);
	while ((c = lexname (c, 0)) == 0)
		/* DO NOTHING */ ;

	if (c < 0)
		return c;

	if (redir != READ_HERE && redir != READ_HERE_TAB)
		return _IORS;

	/*
	 * NIGEL: Add support for <<- to strip leading tabs in the here
	 * document (see POSIX.2 3.7.4). Also add support for multiple here-
	 * documents; the original version of this botched here-documents with
	 * a destination fd digit in addition to only supporting one document.
	 *
	 * Link new here document into here work list at the end.
	 */

	new_here = (struct here *) salloc (sizeof (struct here));

	new_here->h_next = NULL;
	if (here_list != NULL) {
		last_here->h_next = new_here;
	} else
		here_list = new_here;
	last_here = new_here;

	iors = lex_getstart (& length);

	if ((new_here->h_quoted = strpbrk (iors + name,
					   "\"\\'") != NULL) != 0) {
		/*
		 * Simplify quoted here document iors from [n]<<file to
		 * [n]<file.
		 */

		* (iors + name - 1) = ' ';
	}


	/*
	 * Word-expand the end-of-file string.
	 */

	new_here->h_eof = duplstr (eval_buf (iors + name, length - name,
					     EWORD), 0);

	/*
	 * Replace the EOF string with a temporary file-name.
	 */

	(void) build_reduce (lex_heap, length - name);
	new_here->h_tmp = duplstr (shtmp (), 0);

	if ((new_here->h_fd = creat (new_here->h_tmp, 0666)) < 0)
		ecantmake (new_here->h_tmp);
	new_here->h_tabs = redir == READ_HERE_TAB;

	/*
	 * Put the temporary-file name in where the original eof-of-file word
	 * was, so that the parser sees [n]<[<] temp_file
	 */

	iors = new_here->h_tmp;
	while (* iors)
		lex_add (* iors ++);

	return _IORS;
}

/*
 * Collect characters until the end character is found.  If 'f' is
 * CONSUME_BACKSLASH, '\' escapes the next character and newline is
 * not allowed. If 'f' is BACKSLASH_END, backslashes are retained but
 * suppress recognition of the end-character. If 'f' is NO_BACKSLASH,
 * all characters are passed through. If 'f' is NO_ERRORS, behave as
 * NO_BACKSLASH but be quiet about errors.
 */

int
collect (ec, f)
int ec;
int f;
{
	int		c;

	while ((c = getn ()) != ec) {
backslashed:
		/*
		 * NIGEL: Originally, this routine complained about missing
		 * characters at EOF, but System V just implicitly closes off
		 * any strings in this situation. In order to stroke the
		 * lexer, we mutate EOF's into EOL's.
		 */

		if (c < 0)
			return c;

		if (c == '\n' && f == CONSUME_BACKSLASH) {
			emisschar (ec);
			return c;
		}

		if (c == '\\' && f == CONSUME_BACKSLASH) {
			if ((c = getn ()) < 0) {
				syntax ("backslash at end of input");
				return c;
			}
			if (c == '\n')
				continue;
		}

		lex_add (c);

		if (c == '\\' && f == BACKSLASH_END) {
			if ((c = getn ()) >= 0)
				lex_add (c);
		}
	}

	lex_add (ec);
	return ec;
}


/*
 * Get a character.
 */

int
getn ()
{
	int		c;

	if (lastget != 0) {
		c = lastget;
		lastget = 0;
		return c;
	}

	switch (sesp->s_type) {
	case SSTR:
	case SFILE:
		if ((sesp->s_flag & SESSION_EOL) != 0) {
			sesp->s_line ++;
			sesp->s_flag &= ~ SESSION_EOL;
		}

		if (prpflag && (sesp->s_flag & INTERACTIVE) != 0) {
			if (prpflag -= 1) {
				prompt ("\n");
				prpflag -= 1;
			}
			prompt (comflag ? vps1 : vps2);
			comflag = 0;
		}

		if (sesp->s_count == 0) {
			sesp->s_count = read (sesp->s_ifd, sesp->s_argv [0],
					      SESBUFSIZ);
			if (sesp->s_count < 1) {
				sesp->s_count = 0;
				sesp->s_flag |= SESSION_EOF;
				return LEX_EOF;
			}
			sesp->s_flag &= ~ SESSION_EOF;
			sesp->s_argv [1] = sesp->s_argv [0];
		}

		c = * sesp->s_argv [1] ++;
		sesp->s_count --;

		if (c == '\n') {
			if ((sesp->s_flag & INTERACTIVE) != 0) {
				prpflag = 1;
				sesp->s_line = 1;
			} else
				sesp->s_flag |= SESSION_EOL;
		}

		if (vflag)
			shell_outchar (c);
		return c;

	case SARGS:
	case SARGV:
		if ((sesp->s_flag & SESSION_EOF) != 0)
			return LEX_EOF;
		if ((c = * sesp->s_strp ++) == 0) {
			if (sesp->s_type == SARGV &&
			    (sesp->s_strp = * ++ sesp->s_argv) != NULL)
				c = ' ';
			else {
				sesp->s_flag |= SESSION_EOF;
				c = LEX_EOF;
			}
		}

		if (vflag)
			shell_outchar (c);
		return c;

	default:
		panic (23);
	}
}
