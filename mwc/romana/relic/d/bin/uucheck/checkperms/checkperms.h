/* checkperms.h - extra defines for checkperms parser
 * part of uucheck.
 */
#include "uucheck.h"

#ifndef EXTERN
#	ifdef MAIN
#		define EXTERN
#		define EQUALS(v)	= v
#	else
#		define EXTERN	extern
#		define EQUALS(v)
#	endif
#endif EXTERN

/* Expression to mask out all but the file mode bits.  */
#define FMODE(statstuff)	((statstuff.st_mode)&S_IFMT)

#define SETFAIL	if(msg_type == ERROR_TOKEN) {error=TRUE;} else {warning = TRUE;}

/* For token table used by lexer.  */
struct token_type {
	char *string;
	int value;
};

#define NUM_TOKENS	12	/* Size of the following table.  */

#ifdef MAIN
#include "y.tab.h"
struct token_type token_table[NUM_TOKENS] = {
	"foreach", FOREACH,
	"pathname", PATHNAME,
	"error", ERROR_TOKEN,
	"warning", WARNING_TOKEN,
	"exist", EXIST,
	"owner", OWNER,
	"group", GROUP,
	"file", FILE_TOKEN,
	"directory", DIRECTORY,
	"pipe", PIPE,
	"character", CHARACTER_SPECIAL,
	"block", BLOCK_SPECIAL
};
#else /* not MAIN */
extern struct token_type token_table[];
#endif /* MAIN */

EXTERN char *current_path EQUALS(NULL); /* Name of the file currently being investigated.  */
EXTERN char *current_host EQUALS(NULL); /* Loop variable for examining all hosts.  */
EXTERN char *ret_string EQUALS(NULL); /* String for optional return messages.  */
EXTERN char *chmod_string EQUALS(NULL);	/* String for chmod descriptors.  */
EXTERN char *id_string EQUALS(NULL);	/* String for user or group names.  */
EXTERN int major, minor;		/* Major and minor numbers for special files.  */
EXTERN int ret_number;			/* Place to put value of NUMBER tokens.  */
EXTERN struct stat current_stat; /* Stat structure from current file.  */
EXTERN boolean it_exists;	/* Set if the current file exists.  */
EXTERN int lineno EQUALS(1);	  /* Approximate current line number (input file).  */
EXTERN boolean host_looping EQUALS(FALSE);
EXTERN boolean last_host EQUALS(FALSE); /* Loop terminator boolean.  */
EXTERN boolean ignore_next_indent EQUALS(FALSE); /* Used by lexer in foreach handlineg.  */

char *str_assign();
boolean chmod_chk();
boolean owner_chk();
boolean group_chk();
