#line 1 checkperms.lex
#include <stdio.h>
extern	char		yytext[];
extern	int		yyleng;
extern	int		yyscon;
extern	int		yyline;
#define	input()		getchar()
#define	output(c)	putchar(c)
#define	unput(c)	yyback(c)
#define	ECHO		{register n=0; while (n<yyleng) output(yytext[n++]);}
#define	REJECT		yyrjct()
#define	BEGIN		yyscon = 
#line 2
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
#line 80

#define	yyswitch(x)	yyctxt((x),(01))
_llic()
{
	return (input());
}
_lloc(c)
{
	output(c);
}
yylex()
{
#line 82
yyloop:
	switch (_lltk()) {
	case 0:
		if (yywrap())
			return (EOF);
		break;
	case 01:
#line 82
		{
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
		break;

	case 02:
#line 97
		{
			    if (!ignore_next_indent) {
				return(INDENT);
			    } else {
				ignore_next_indent = FALSE;
			    } /* if !ignore_next_indent */
			} /* INDENT */
		break;
	case 03:
#line 104
		{ /* Ignore everything after a "#" to end of line.  */ }
		break;
	case 04:
#line 105
		{ /* Internal blanks and commas are white space.  */ }
		break;
	case 05:
#line 106
		{ copy_str(&chmod_string, yytext);
			  return(CHMOD_PERMISSION);
			} /* Chmod pattern.  */
		break;
	case 06:
#line 109
		{ copy_str(&ret_string, yytext);
			  /* Strip off the leading and trailing quotes.  */
			  ret_string[strlen(ret_string) - 1] = (char) NULL;
			  ++ret_string;
			  return(STRING);
			} /* String pattern.  */
		break;
	case 07:
#line 115
		{ int retval;
			  if ((retval = lookup(yytext)) == 0) {
				fprintf(stderr,
					"Invalid token %s ",
					yytext);
				FATAL("in line %d.\n", lineno);
			  } /* if retval invalid */
			  return(retval); }
		break;
	case 010:
#line 123
		{ return(NL); }
		break;
	}
	goto yyloop;

}
#line 125
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
int yy_lxctab[] = {
01000,00,010001,00,00,00,00,00,
00,00,00,00,00,00,00,00,
00,00,00,00,00,00,0100200,040,
00,00,00,00,00,00,00,00,
00,00,024000,00,00,00,00,00,
00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,0634,
00,00,00,00,00,00,00,00,
00,00,00,00,0177776,03777,0177776,03777,
00,00,00,00,00,00,00,00,
00,00,00,01777,0177776,03777,0177776,03777,
00,00,00,00,00,00,00,00
};
int yy_lxbtab[] = {
01<<000,01<<001,01<<002,01<<003,01<<004,01<<005,01<<006,01<<007,
01<<010,01<<011,01<<012,01<<013,01<<014,01<<015,01<<016,01<<017
};
int yy_ctxtab[] = {
00
};
int yy_lextab[] = {
061,025,024,061,045,044,0161,01065,
061,01351,0177761,0245,064,0101,06,0177761,
0104,0161,026,0177761,046,066,0177761,0124,
0261,01045,0161,0101,02705,01045,042,01351,
0177661,01045,0144,0141,0106,061,0126,0177761,
0164,061,0245,0204,00
};
int *yy_clist[050];
int *yy_nlist[050];
