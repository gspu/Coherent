#ifndef	INPUT_H
#define	INPUT_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		PROTO ()
 */

#include <sys/compat.h>

#ifndef	INPUT_T
#define	INPUT_T
typedef struct input	input_t;
#endif

#ifndef	LEX_T
#define	LEX_T
typedef	struct lexinfo	lex_t;
#endif

enum {
	IN_EOF		= -1
};


/*
 * Structure of an input source.
 */

struct input {
	int	     (*	in_read)  	PROTO ((input_t * _input));
	void	     (*	in_unread)	PROTO ((input_t * _input));
	CONST unsigned char *
		     (*	in_readtok)	PROTO ((input_t * _input,
						lex_t * _lexp,
						size_t * _len));
	void	     (*	in_close) 	PROTO ((input_t * _input));
	void	     (*	in_error) 	PROTO ((input_t * _input));
};

#endif	/* ! defined (INPUT_H) */
