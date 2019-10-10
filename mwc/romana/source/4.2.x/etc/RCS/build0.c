head	1.1;
access
	hal
	udo
	ljg
	jrd
	nigel
	fred;
symbols
	r12:1.1;
locks; strict;
comment	@ *@;


1.1
date	93.11.04.17.17.28;	author rcsadmin;	state Exp;
branches;
next	;


desc
@@


1.1
log
@COHERENT r12
@
text
@/*
 * build0.c
 * 07/11/92
 * Routines common to build and install.
 */

#include <stdio.h>
#include <sys/stat.h>
#include "build0.h"

/* Globals. */
int	abortmsg;			/* print abort message	*/
char	*argv0;				/* for error messages	*/
char	buf[NBUF];			/* input buffer		*/
char	cmd[NBUF];			/* command buffer	*/
int	dflag;				/* debug		*/
char	*usagemsg;			/* usage message	*/
int	vflag;				/* verbose		*/

/*
 * Clear the IBM AT console screen.
 * Prompt for <Enter> if the flag is true or if dflag.
 */
void
cls(flag) register int flag;
{
	if (flag || dflag)
		get_line("\Hit <Enter> to continue...");
#if 0
	putchar(0x1B);		/* ESC */
	putchar('[');
	putchar('2');
	putchar('J');
#else
	putchar('\n');
#endif
	fflush(stdout);
}

/*
 * Return 1 if file exists, 0 if not.
 */
int
exists(file) register char *file;
{
	register int fd;

	if ((fd = open(file, 0)) < 0)
		return 0;
	close(fd);
	return 1;
}

/*
 * Print a fatal error message.
 * Print "Installation aborted..." message if invoked from build or install.
 */
void
fatal(s) char *s;
{
	fprintf(stderr, "%s: %r\n", argv0, &s);
	if (abortmsg)
		fprintf(stderr, "Installation aborted before completion.\n");
	exit(1);
}

/*
 * Get an integer value in the given range.
 */
/* VARARGS */
int
get_int(min, max, prompt) int min, max; char *prompt;
{
	register char *s;
	register int i;

	s = get_line("%r", &prompt);
	for (;;) {
		if (*s >= '0' && *s <= '9') {
			i = atoi(s);
			if (i >= min && i <= max)
				return i;
		}
		get_line("Enter a value between %d and %d:", min, max);
	}
}

/*
 * Get an integer value from a given list.
 * Val_list is a an int array of acceptable int values.
 * Num_vals is the number of acceptable values in val_list, must be > 0.
 * If strict is 0, allow user to enter value not in list after a warning.
 * If default is not -1, display it in brackets and use it if just <enter>
 * is pressed.
 * If hex is nonzero, display numeric values in hexadecimal, else decimal.
 */
/* VARARGS */
int
get_allowed_int(val_list, num_vals, hex, default_val, strict_val, prompt)
int * val_list;
int num_vals;
int hex;
int default_val;
int strict_val;
char *prompt;
{
	register char *s;
	register int i;
	int user_val;
	char * fmt_str;

	/* Display the given prompt string. */
	printf("%r\n", &prompt);

	/* Get input from user. */
	for (;;) {
		/* Display second prompt line with acceptable values. */
		if (strict_val)
			printf("Valid choices are ");
		else
			printf("Standard choices are ");

		for (i = 0;  i < num_vals;  i++) {
			if (i > 0)
				putchar('/');
			fmt_str = hex ? "%x" : "%d" ;
			printf(fmt_str, val_list[i]);
		}

		if (default_val != -1) {
			fmt_str = hex ? " [%x]" : " [%d]" ;
			printf(fmt_str, default_val);
		}
		s = get_line(" : ");

		if (*s == '\0')
			return default_val;

		fmt_str = hex ? "%x" : "%d" ;

		if (sscanf(s, fmt_str, & user_val)) {

			/* Check the value entered. */
			for (i = 0;  i < num_vals;  i++) {
				if (user_val == val_list[i])
					return user_val;
			}
			if (!strict_val &&
			  yes_no("That is not one of the standard values.  "
			  "Use anyway? "))
				return user_val;
		}
	}
}

/*
 * Print the args and get a line from the user to buf[].
 * Strip the trailing newline and return a pointer to the first non-space.
 */
/* VARARGS */
char *
get_line(args) char *args;
{
	register char *s;

	printf("%r ", &args);
	fflush(stdout);
	fgets(buf, sizeof buf, stdin);
	buf[strlen(buf) - 1] = '\0';
	for (s = buf; ; ++s)
		if (*s != ' ' && *s != '\t')
			return s;
}

/*
 * Return:
 *	1	dir exists and is a directory,
 *	0	dir does not exist,
 * or	-1	dir exists but is not a directory.
 */
int
is_dir(dir) register char *dir;
{
	struct stat s;

	if (stat(dir, &s) == -1)
		return 0;			/* does not exist */
	return (s.st_mode & S_IFDIR) ? 1 : -1;	/* directory or nondirectory */
}

/*
 * Print a nonfatal error message.
 * Make sure the user sees it by waiting until he hits <Enter>.
 */
void
nonfatal(s) char *s;
{
	fprintf(stderr, "%s: %r\n", argv0, &s);
	get_line("\Hit <Enter> to continue...");
}

/*
 * Execute the given command and return its exit status.
 * The flag tells what to do if the command returns an error status:
 *	S_IGNORE	ignore it
 *	S_NONFATAL	report it
 *	S_FATAL		report it and die
 */
int
sys(command, flag) char *command; int flag;
{
	register int status;

	if (dflag || vflag)
		printf("%s\n", command);
	if (dflag)
		return 0;
	if ((status = system(command)) != 0) {
		if (flag == S_NONFATAL)
			nonfatal("command \"%s\" failed", command);
		else if (flag == S_FATAL)
			fatal("command \"%s\" failed", command);
	}
	sync();
	return status;
}

/*
 * Print a usage message and die.
 */
void
usage()
{
	fprintf(stderr, usagemsg);
	exit(1);
}

/*
 * Get the answer to a yes/no question.
 * Return 1 for yes, 0 for no.
 */
/* VARARGS */
int
yes_no(args) char *args;
{
	register char *s;

	for (;;) {
		printf("%r", &args);
		s = get_line(" [y or n]?");
		if (*s == 'y' || *s == 'Y')
			return 1;
		else if (*s == 'n' || *s == 'N')
			return 0;
	}
}

/* end of build0.c */
@
