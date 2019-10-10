/*
 * build0.c
 * 10/22/90
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
	putchar(0x1B);		/* ESC */
	putchar('[');
	putchar('2');
	putchar('J');
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
		if (*s == 'y')
			return 1;
		else if (*s == 'n')
			return 0;
	}
}

/* end of build0.c */
