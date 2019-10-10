#ifndef	MKINPUT_H
#define	MKINPUT_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		CONST
 *		VOID
 *		PROTO ()
 *	<stdio.h>
 *		FILE
 */

#include <sys/compat.h>
#include <stdio.h>


/*
 * Structure of an input source.
 */

#ifndef	INPUT_T
#define	INPUT_T
typedef struct input	input_t;
#endif

/*
 * Pass this rather than a character to make_file_input () if you don't want
 * support for commentary in input text. Comments in file input simply throw
 * away all text from the comment character (which may appear anywhere) to the
 * end of the line. It's just a hack, but handy nonetheless, since sometimes
 * adding comment facilities to an ad-hoc input grammar properly would just
 * be over the top... now, I *really* want an LL(1) parser generator, but
 * without one I'd rather hand-code than use YACC, which bites when you have
 * a lot of little grammars.
 */

enum {
	NO_COMMENT	= -1
};


EXTERN_C_BEGIN

input_t	      *	make_file_input	PROTO ((FILE * _file, CONST char * _name,
					int _comment));
input_t	      *	make_string_input PROTO ((CONST unsigned char * _str,
					  int _copy));

EXTERN_C_END

#endif	/* ! defined (MKINPUT_H) */
