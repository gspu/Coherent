/* checkperms.y
 * yacc grammar to read permissions description files.
 * part of uucheck.
 */

%token	FOREACH PATHNAME STRING INDENT UNINDENT NL ERROR_TOKEN WARNING_TOKEN
%token	EXIST OWNER GROUP CHMOD_PERMISSION NUMBER
%token	FILE_TOKEN DIRECTORY PIPE CHARACTER_SPECIAL BLOCK_SPECIAL
%start	statement_list
%%

statement_list	: statement
		| statement_list statement
		;

statement	: foreach_block
		| path_block
		| message_statement NL
		| NL	/* Blank line is nop.  */
		| error NL
			{yyerrok;}
		;

/* This is our looping construct--it runs through all possible hosts.  */
foreach_block	: foreach_declarator NL block {
			 /* Put lost character back into stdin.  */
			my_ungetchar(delayed_char);
#ifdef DEBUG
			printf("%d:  finished foreach_block.\n", lineno);
#endif /* DEBUG */
		} /* finish a foreach_block */
		;

foreach_declarator	: FOREACH string {
				if(host_looping) {
				  FATAL("%d: foreach can not nest.",
					  lineno);
				} else {
#ifdef DEBUG
/* DEBUG */		  printf("%d: Foreach identified\n", lineno);
#endif /* DEBUG */
				  host_looping = TRUE;
				  current_host = first_host($2);
				  saving_to_monitor_file = TRUE;
				  reading_from_monitor_file = FALSE;
				  open_monitor_file(); /* Start saving input.  */
				} /* if host_looping */
			} /* FOREACH */
			;

/* There's one of these for each file.  */
path_block	: path_declarator NL block {
			/* Once beyond the scope of a path_block, make sure
			 * that there is no current_path set.
			 */
			free(current_path);
			current_path = NULL;
		} /* path_block */
		;

path_declarator	: PATHNAME string {
			if(host_looping) {
				/* We need to build a whole pathname.  */
				current_path = malloc(strlen(current_host)+
						      strlen($2) + 1 );
				/* Insert the current host into the 
				 * partial path provided. */
				sprintf(current_path, $2, current_host);
			} else {
				if(current_path != NULL) {
				  FATAL("%d: pathnames can not be nested.\n",
					 lineno);
				} /* if there is a current path */
				copy_str(&current_path, $2);
			} /* if in a loop of host names */

			/* Now that we have a full pathname, we can build
 			 * a stat structure for it.
			 */
			if (stat(current_path, &current_stat) == -1){
#ifdef DEBUG
/* DEBUG */		printf("Can not stat %s.\n", current_path);
#endif /* DEBUG */
				it_exists = FALSE;
			} /* if stat failed */
			else {
#ifdef DEBUG
/* DEBUG */		printf("Just stat ed %s.\n", current_path);
#endif /* DEBUG */
				it_exists = TRUE;
			} /* else stat succeeded */
			sprintf(bigbuf, "Examining %s.\n", current_path);
			REALLYVERBOSE(bigbuf);
		}
		;

block		: INDENT statement_list UNINDENT
		| /* nothing--blocks are optional */
		;

message_statement	: message_type check_type string {
#ifdef DEBUG
/* DEBUG */		    printf("%d: Message %d, Check %d, String:  %s\n",
				lineno, $1, $2, $3);
#endif /* DEBUG */
			    do_check($1, $2, $3);
			} /* message_type check_type */
			| message_type check_type {
#ifdef DEBUG
/* DEBUG */		    printf("%d: Message %d, Check %d\n",
				lineno, $1, $2);
#endif
			    do_check($1, $2, NULL);
			} /* message_type check_type */
			;

/* Should this generate error, or warning messages?  */
message_type	: ERROR_TOKEN
			{$$ = ERROR_TOKEN;}
		| WARNING_TOKEN
			{$$ = WARNING_TOKEN;}
		;

/* What are we checking about the file?  */
check_type	:	existence
			{$$=$1;}
		|	file_type
			{$$=$1;}
		|	ownership
			{$$=$1;}
		|	groupship
			{$$=$1;}
		|	CHMOD_PERMISSION {
			  $$=CHMOD_PERMISSION;
			} /* CHMOD_PERMISSION */
		;
		
existence	:	EXIST
			{$$=EXIST;}
		;

file_type	:	FILE_TOKEN
			{$$=FILE_TOKEN;}
		|	DIRECTORY
			{$$=DIRECTORY;}
		|	PIPE
			{$$=PIPE;}
		|	CHARACTER_SPECIAL
			{$$=CHARACTER_SPECIAL;}
		|	BLOCK_SPECIAL
			{$$=BLOCK_SPECIAL;}
		;

ownership	:	OWNER string {
				$$=OWNER;
				copy_str(&id_string, $2);
			} /* ownership */
		;

groupship	:	GROUP string {
				$$=GROUP;
				copy_str(&id_string, $2);
			} /* groupship */
		;

string		:	STRING {
			  $$=ret_string; 
			  ret_string = NULL;
			} /* string */
		;

number		:	NUMBER {
			  $$ = ret_number;
			} /* number */
		;
%%

#include <stdio.h>
#include "checkperms.h"
#include "monitor.h"

yyerror (s)
	char *s;
{
	fprintf (stderr, "line %d: %s, Token: %d\n", lineno, s, yychar);
}
