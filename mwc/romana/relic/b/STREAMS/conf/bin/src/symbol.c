/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		USE_PROTO
 *		LOCAL
 *	<stdio.h>
 *		NULL
 *		FILE
 *		fopen ()
 *		fclose ()
 *	<stdlib.h>
 *		malloc ()
 *	<string.h>
 *		strerror ()
 *		memcmp ()
 *	<errno.h>
 *		errno
 *	"buildobj.h"
 *		BUILD_OK
 *		build_t
 *		builder_alloc ()
 *		build_begin ()
 *		build_error ()
 *	"ehand.h"
 *		ehand_t
 *		CHAIN_ERROR ()
 *		POP_HANDLER ()
 *		PUSH_HANDLER ()
 *		throw_error ()
 *	"mkinput.h"
 *		input_t
 *		make_file_input ()
 *	"read.h"
 *		READ_EOF
 *		token_t
 *		read_token ()
 *		token_copy ()
 *		token_discard ()
 *		token_end ()
 *	"lex.h"
 *		WHITESPACE
 *		lex_t
 */

#include <sys/compat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "buildobj.h"
#include "ehand.h"
#include "mkinput.h"
#include "read.h"
#include "lex.h"

#include "symbol.h"


/*
 * Symbols in the symbol table are all built here. The symbols are kept in
 * buckets sorted by size so it's easy to detect duplicates when adding more.
 */

LOCAL	build_t	      *	data_heap;
LOCAL	symbol_t      *	symbols [10];
LOCAL	symbol_t      *	bigsyms;


/*
 * Initialise the symbol data memory (also used as temporary space by various
 * reader routines below). Returns 0 on success, 1 on error.
 */

#if	USE_PROTO
int (symbol_init) (void)
#else
int
symbol_init ARGS (())
#endif
{
	return (data_heap = builder_alloc (256, 0)) == NULL;
}


/*
 * Try and find a match for a symbol.
 */

#if	USE_PROTO
symbol_t * (sym_find) (CONST unsigned char * data, size_t len)
#else
symbol_t *
sym_find ARGS ((data, len))
CONST unsigned char
	      *	data;
size_t		len;
#endif
{
	symbol_t      *	scan;

	if (len <= (sizeof (symbols) / sizeof (* symbols)))
		scan = symbols [len - 1];
	else
		scan = bigsyms;

	/*
	 * Because we might be scanning the "bigsyms" bucket, remember that we
	 * have to check the lengths for equality.
	 */

	while (scan != NULL) {

		if (len == scan->s_size &&
		    memcmp (scan->s_data, data, len) == 0)
			return scan;

		scan = scan->s_next;
	}

	return NULL;
}


/*
 * Common constructor code from string_to_symbol () and sym_intern ().
 */

#if	USE_PROTO
LOCAL symbol_t * (make_sym) (CONST unsigned char * str, size_t len)
#else
LOCAL symbol_t *
make_sym ARGS ((str, len))
CONST unsigned char
	      *	str;
size_t		len;
#endif
{
	symbol_t      *	scan;

	if ((scan = (symbol_t *) malloc (sizeof (* scan))) == NULL)
		throw_error ("Unable to allocate symbol record");

	scan->s_size = len;
	scan->s_data = str;

	if (len <= (sizeof (symbols) / sizeof (* symbols))) {

		scan->s_next = symbols [len - 1];
		symbols [len - 1] = scan;
	} else {

		scan->s_next = bigsyms;
		bigsyms = scan;
	}

	return scan;
}


/*
 * Turn a string constant into a symbol.
 */

#if	USE_PROTO
symbol_t * (string_to_symbol) (CONST unsigned char * str, size_t len)
#else
symbol_t *
string_to_symbol ARGS ((str, len))
CONST unsigned char
	      *	str;
size_t		len;
#endif
{
	symbol_t      *	scan;

	return (scan = sym_find (str, len)) != NULL ?
			scan : make_sym (str, len);
}


/*
 * Turn a newly-read token into an interned symbol.
 */

#if	USE_PROTO
symbol_t * (sym_intern) (token_t * tok)
#else
symbol_t *
sym_intern ARGS ((tok))
token_t	      *	tok;
#endif
{
	symbol_t      *	scan;

	if ((scan = sym_find (tok->tok_data, tok->tok_len)) != NULL) {
		/*
		 * If the token data was copied to the heap, discard
		 * it.
		 */

		token_discard (tok);
		return scan;
	}


	/*
	 * Add a new symbol to this symbol bucket. If the token data wasn't
	 * copied to the heap, then do that now.
	 */

	token_copy (tok, data_heap);
	return make_sym (tok->tok_data, tok->tok_len);
}


/*
 * Read and intern a symbol.
 */

#if	USE_PROTO
int (read_symbol) (input_t * input, lex_t * lexp, symbol_t ** sym)
#else
int
read_symbol ARGS ((input, lexp, sym))
input_t	      *	input;
lex_t	      *	lexp;
symbol_t     **	sym;
#endif
{
	token_t		temp;
	int		ch;

	if ((ch = build_begin (data_heap, 0, 0)) != 0)
		throw_error ("Problem beginning symbol construction, %s",
			     build_error (ch));

	ch = read_token (input, lexp, data_heap, & temp);

	token_end (& temp);

	* sym = temp.tok_len > 0 ? sym_intern (& temp) : NULL;

	return ch == READ_EOF ? SYM_EOF : ch;
}


/*
 * Suck in an entire configuration file.
 */

#if	__COHERENT__
# define	TEXT_MODE
#else
# define	TEXT_MODE	"t"
#endif


#if	USE_PROTO
void (read_dev_file) (CONST char * inname, CONST char * outname,
		      dev_func_p devfuncp, VOID * extra)
#else
void
read_dev_file ARGS ((inname, outname, devfuncp, extra))
CONST char    *	inname;
CONST char    *	outname;
dev_func_p	devfuncp;
VOID	      *	extra;
#endif
{
	FILE	      *	infile;
	FILE	      *	outfile;
	input_t	      *	input;
	ehand_t		err;

	if ((infile = fopen (inname, "r" TEXT_MODE)) == NULL)
		throw_error ("cannot open file '%s', OS error %d \"%s\"",
			     inname, errno, strerror (errno));

	outfile = NULL;

	if (outname != NULL &&
	    (outfile = fopen (outname, "w" TEXT_MODE)) == NULL) {
		throw_error ("cannot open file '%s', OS error %d \"%s\"",
			     outname, errno, strerror (errno));
	}


	if ((input = make_filter (infile, inname, 1, '#',
				  outfile, 1)) == NULL) {

		(void) fclose (infile);
		if (outfile != NULL)
			(void) fclose (outfile);
		throw_error ("could not create input stream in read_dev_file ()");
	}

	if (PUSH_HANDLER (err) == 0) {

		while ((* devfuncp) (input, WHITESPACE, extra) == '\n')
			;
	} else {

		read_error (input);
		read_close (input);
		CHAIN_ERROR (err);
	}

	read_close (input);
	POP_HANDLER (err);
}

/*
 * Suck in a single configuration file entry from a string.
 */

#if	USE_PROTO
void (read_dev_string) (CONST char * string, dev_func_p devfuncp,
			VOID * extra)
#else
void
read_dev_string ARGS ((string, devfuncp, extra))
CONST char    *	string;
dev_func_p	devfuncp;
VOID	      *	extra;
#endif
{
	input_t	      *	input;
	ehand_t		err;

	if ((input = make_string_input (string, 0)) == NULL)
		throw_error ("could not create input stream in read_dev_string ()");

	if (PUSH_HANDLER (err) == 0)
		(* devfuncp) (input, WHITESPACE, extra);
	else {
		read_error (input);
		read_close (input);
		CHAIN_ERROR (err);
	}

	read_close (input);
	POP_HANDLER (err);
}
