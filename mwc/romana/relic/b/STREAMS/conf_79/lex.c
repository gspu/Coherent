/*
 * Simple ad-hoc lexer.
 */

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		USE_PROTO
 *		ARGS ()
 *	<string.h>
 *		strchr ()
 */

#include <sys/compat.h>
#include <string.h>

#include "ehand.h"

#include "lex.h"


CONST char	LEX_WILD [1];		/* special wildcard value */

lex_t	WHITESPACE [] = {
	{ NULL, STD_FLUSH, STD_SEP, LEX_WILD, NULL }
};


/*
 * Here the code that uses the "lex_t" structure to classify a character has
 * been factored out.
 */

#ifdef	USE_PROTO
int (classify) (lex_t * lexp, int ch, int flush)
#else
int
classify ARGS ((lexp, ch, flush))
lex_t	      *	lexp;
int		ch;
int		flush;
#endif
{
	int		valid = 0;
	int		invalid = 0;
	CONST lex_t   *	scan = lexp;

	if (ch < 0)
		throw_error ("Bad character passed to classify ()");

	do {
		/*
		 * Classify the new character.
		 */

		if (flush && scan->l_flush != NULL &&
		    strchr (scan->l_flush, ch) != 0)
			return CLASS_FLUSH;

		if (scan->l_sep != NULL && strchr (scan->l_sep, ch) != 0)
			return CLASS_SEP;

		if (scan->l_valid == LEX_WILD ||
				(scan->l_valid != NULL &&
				 strchr (scan->l_valid, ch) != NULL))
			valid = 1;

		if (scan->l_error == LEX_WILD)
			invalid = 1;
		else if (! valid && scan->l_error != NULL &&
				 strchr (scan->l_error, ch) != NULL) {

			invalid = 1;
			break;
		}
	} while ((scan = scan->l_prev) != NULL);


	if (! valid) {

		if (invalid)
			throw_error ("invalid character value 0x%x\n", ch);

		throw_error ("unexpected character value 0x%x\n", ch);
	}

	return CLASS_VALID;
}

