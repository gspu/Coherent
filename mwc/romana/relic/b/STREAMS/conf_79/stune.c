/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		USE_PROTO
 *		ARGS ()
 *	<stdlib.h>
 *		NULL
 *		malloc ()
 *		free ()
 */

#include <sys/compat.h>
#include <stdlib.h>

#include "ehand.h"
#include "symbol.h"
#include "read.h"
#include "mtune.h"

#include "stune.h"

LOCAL stune_t *	_stunes;


/*
 * Read lines from an "stune" file.
 */

#ifdef	USE_PROTO
LOCAL int (read_stune) (input_t * input, lex_t * lexp)
#else
LOCAL int
read_stune ARGS ((input, lexp))
input_t	      *	input;
lex_t	      *	lexp;
#endif
{
	int		ch = '\n';
	stune_t	      *	stunep;
	ehand_t		err;

	if ((stunep = (stune_t *) malloc (sizeof (* stunep))) == NULL)
		throw_error ("out of memory in read_stune ()");

	if (PUSH_HANDLER (err) == 0) {
		/*
		 * If the first thing on the line works out to be an EOF,
		 * then bail out without an error.
		 */

		ch = read_symbol (input, lexp, & stunep->st_name);

		if (stunep->st_name == NULL) {
			/*
			 * We allow an EOF at the beginning of a line and we
			 * also allow a blank line.
			 */

			goto at_eof;
		}
		check_not_eol (ch);

		if ((stunep->st_mtune = find_mtune (stunep->st_name)) == NULL)
			throw_error ("Parameter name not in 'mtune' file");

		if (stunep->st_mtune->mt_stune != NULL)
			throw_error ("Two values configured for parameter");

		/*
		 * Pull in the parameter value
		 */

		ch = read_longs (input, lexp, & stunep->st_value, NO_RANGE);

		if (stunep->st_value < stunep->st_mtune->mt_min ||
		    stunep->st_value > stunep->st_mtune->mt_max)
			throw_error ("Parameter value outside configurable range");

		ch = expect_eol (input, lexp, ch);

		/*
		 * Having passed all the reasonableness checks, we link the
		 * new entry into the chain.
		 */

		stunep->st_mtune->mt_stune = stunep;

		stunep->st_next = _stunes;
		_stunes = stunep;
	} else {

		free (stunep);
		CHAIN_ERROR (err);
	}

at_eof:
	POP_HANDLER (err);
	return ch;
}


#if 0
/*
 * Regenerate an 'stune' line from the stored record.
 */

#ifdef	USE_PROTO
void (write_stune) (stune_t * stunep, FILE * out)
#else
void
write_stune ARGS ((stunep, out))
stune_t	      *	stunep;
FILE	      *	out;
#endif
{
	(void) fprintf (out, "%-8s %-7ld\n",
			stunep->st_name->s_data,
			stunep->st_value);
}
#endif


/*
 * Suck in a complete 'stune' file.
 */

#ifdef	USE_PROTO
void (read_stune_file) (CONST char * name)
#else
void
read_stune_file ARGS ((name))
CONST char    *	name;
#endif
{
	read_dev_file (name, read_stune);
}
