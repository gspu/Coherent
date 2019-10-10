%{
#include "y.tab.h"
#include "checkperms.h"
#include "monitor.h"

#undef input

/*
 * input routine takes care of tab counting.
 */
input()
{
	int c;
	static int here = 0, atstart = 1, atpos = 0, level = 0, stops[20];

	while (atstart) {
		switch (c = my_getchar()) {
		case '\t':
			atpos |= 7;
		case ' ':
			atpos++;
			break;
		case '\n':
			atpos = 0;
			lineno++;
#if 0
/* DEBUG */		fprintf(stderr, "lineno: %d\n", lineno);
#endif /* 0 */
			break;
		default:
			my_ungetchar(c);
			atstart = 0;		
			if (atpos > here) {
				here = stops[++level] = atpos;
				return(2);
			}
		} /* switch (c = my_getchar()) */

	}

	if (atpos < here) {
		here = stops[--level];
		return(1);
	}

	switch (c = my_getchar()) {	
	case '\n':
		atstart = 1;
		atpos = 0;
		lineno++;
#if 0
/* DEBUG */		fprintf(stderr, "lineno: %d\n", lineno);
#endif /* 0 */
	}

	return(c);
}

lookup(s)
	char *s;
{
	int i;
	int retval;

	retval = 0;
	for (i = 0; i < NUM_TOKENS; ++i){
		if(strcmp(s, token_table[i].string) == 0) {
			retval = token_table[i].value;
#ifdef DEBUG
			fprintf(stderr,
				"Token: %s, Line: %d\n",
				token_table[i].string, lineno);
#endif DEBUG
		} /* if s matches token_table[i].string */
	} /* for i = 0 to NUM_TOKENS - walk through the table.  */

	return(retval);
} /* lookup() */
%}

%%
"\001"			{
			    /* If we are at the bottom of a foreach */
			    if (host_looping && (current_path == NULL)) {
				/* We probably want to rewind to the foreach.  */
				if (handle_foreach_unindent() == UNINDENT) {
					/* No, we really are done with the
					 * foreach loop.
					 */
					return(UNINDENT);
				}
			    } else { /* Otherwise, just an ordinary unindent.  */
				return(UNINDENT);
			    } /* if we are at the bottom of a foreach */
			} /* UNINDENT */

"\002"			{
			    if (!ignore_next_indent) {
				return(INDENT);
			    } else {
				ignore_next_indent = FALSE;
			    } /* if !ignore_next_indent */
			} /* INDENT */
#.*\n			{ /* Ignore everything after a "#" to end of line.  */ }
[ \t,]+			{ /* Internal blanks and commas are white space.  */ }
[ugo]+[+-][strwx]+	{ copy_str(&chmod_string, yytext);
			  return(CHMOD_PERMISSION);
			} /* Chmod pattern.  */
\"((\\\")|.)*\"		{ copy_str(&ret_string, yytext);
			  /* Strip off the leading and trailing quotes.  */
			  ret_string[strlen(ret_string) - 1] = (char) NULL;
			  ++ret_string;
			  return(STRING);
			} /* String pattern.  */
[a-zA-Z][a-zA-Z0-9]*	{ int retval;
			  if ((retval = lookup(yytext)) == 0) {
				fprintf(stderr,
					"Invalid token %s ",
					yytext);
				FATAL("in line %d.\n", lineno);
			  } /* if retval invalid */
			  return(retval); }
"\n"			{ return(NL); }
%%
int
handle_foreach_unindent()
{
#if 0
/* DEBUG */	printf("%d: finished a declarator block.\n", lineno);
#endif /* 0 */
		if((current_host = next_host()) != NULL){
#if 0
/* DEBUG */	    printf("%d: There is another host.\n", lineno);
#endif /* 0 */
		    saving_to_monitor_file = FALSE;
		    reading_from_monitor_file = TRUE;
		    rewind_monitor_file();
		    ignore_next_indent = TRUE;
		} else {
#if 0
/* DEBUG */	    printf("%d: No more hosts.\n", lineno);
#endif /* 0 */
		    close_monitor_file();	/* Throw away old input.  */
		    saving_to_monitor_file = FALSE;
		    reading_from_monitor_file = FALSE;
		    host_looping = FALSE;	/* We're done looping.  */
		    return(UNINDENT);
		}
	return(0);
} /* handle_foreach_unindent() */
