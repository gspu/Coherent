#ifndef	LEX_H
#define	LEX_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		PROTO ()
 *	<stddef.h>
 *		size_t
 */

#include <sys/compat.h>
#include <stddef.h>


#ifndef	LEX_T
#define	LEX_T
typedef struct lexinfo	lex_t;
#endif


/*
 * Structure for communicating lexical parameters to the tokenizer.
 */

struct lexinfo {
	CONST lex_t   *	l_prev;		/* for stacking lexical rules */

	CONST char    *	l_flush;	/* leading flush characters */
	CONST char    *	l_sep;		/* separator/terminator characters */
	CONST char    *	l_valid;	/* characters that are valid */
	CONST char    *	l_error;	/* characters that are errors */
};

extern CONST char	LEX_WILD [1];	/* special wildcard value */


#define	STD_FLUSH	" \t\r"
#define	STD_SEP		" \t\r\n"

extern lex_t		WHITESPACE [];


enum {	CLASS_FLUSH,
	CLASS_SEP,
	CLASS_VALID
};


EXTERN_C_BEGIN

int		classify	PROTO ((lex_t * _lexp, int _char,
					int _flushflag));

EXTERN_C_END

#endif	/* ! defined (LEX_H) */

