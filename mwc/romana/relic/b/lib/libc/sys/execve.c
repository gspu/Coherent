/*
 * libc/sys/i386/execve.c
 * This version of execve() understands "#!interpreter [ arg ...]\n" lines.
 * The original version of this source was "iexec.c" Version 1.6,
 * written by David MacKenzie <djm@eng.umd.edu> and placed in the public domain.
 * Extensively modified by steve 8/12/92 for COHERENT.
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAXLINELEN	512	/* bytes to read from first line of a script */
#define	iswhite(c)	((c) == ' ' || (c) == '\t')	/* whitespace */

/* Try _execve(), i.e. sys exec; then try running as a script if it fails. */
int
execve(cmd, argv, envp) char *cmd; char **argv; char **envp;
{

	if (_execve(cmd, argv, envp) == -1 && errno == ENOEXEC) {
		script_execve(cmd, argv, envp);
		errno = ENOEXEC;
	}
	return -1;
}

/*
 * If 'file' is a script which begins "#!interp [ arg ...]\n",
 * set 'interp' to the name of the program that should run it
 * and 'int_args' to the given args (or NULL if none).
 * Create a new argument list containing 'interp',
 * expanded 'int_args' (if non-NULL), 'file', and the original argv.
 * Then try to run 'interp' with the new argument list and 'envp'.
 * Return -1 if it fails.
 */
static
int
script_execve(file, argv, envp) char *file, **argv, **envp;
{
	char line[MAXLINELEN + 1];
	register char *cp, **ap, **nargv;
	char *interp, *int_args;
	register int argc, fd, n;
	char c, inquote;

	/* Open the file, read a chunk, see how it starts. */
	if ((fd = open(file, O_RDONLY, 0)) == -1)
		return -1;
	n = read(fd, line, MAXLINELEN);
	if (close(fd) == -1 || n < 4 || line[0] != '#' || line[1] != '!')
		return -1;

	/* The first line looks appropriate. */
	line[n] = '\0';				/* NUL-terminate */
	if ((cp = strchr(line, '\n')) == NULL)
		return -1;
	*cp = '\0';				/* zap newline */

	/* Find interpreter name and args. */
	for (cp = line + 2; iswhite(*cp); ++cp)
		;				/* skip whitespace after #! */
	if (*cp == '\0')
		return -1;
	interp = cp;				/* interpreter name */
	for (; *cp != '\0' && !iswhite(*cp); ++cp)
		;				/* scan to whitespace or NUL */
	if (*cp == '\0')
		int_args = NULL;
	else {
		*cp++ = '\0';			/* NUL-terminate name */
		int_args = cp;			/* interpreter args */
	}

	/* Find new arg count max and allocate space for new argv. */
	for (argc = 2, ap = argv; *ap != NULL; ++ap)
		++argc;				/* count old argv args + 1 */
	if (int_args != NULL)
		argc += strlen(int_args)/2;	/* add max int_args args */
	if ((nargv = (char **)malloc(sizeof(char *) * argc)) == NULL)
		return -1;

	/* Initialize the new argv. */
	argc = 0;
	nargv[argc++] = interp;			/* interpreter name */
	if (int_args != NULL) {

		/* Add whitespace-separated args from int_args. */
		for (cp = int_args; (c = *cp) != '\0'; ) {
			while (iswhite(c))
				c = *++cp;	/* skip whitespace */
			if (c == '\0')
				break;
			else if (c == '\'' || c == '"') {
				inquote = c;	/* quoted arg */
				++cp;
			} else
				inquote = 0;
			nargv[argc++] = cp;	/* start new arg */
			while ((c = *cp++) != '\0') {
				if (!inquote && iswhite(c))
					break;
				else if (inquote && c == inquote) {
					inquote = cp[-1] = '\0';	/* zap close quote */
					c = *cp++;	/* next char */
					break;
				}
			}
			if (inquote) {
				free(nargv);
				return -1;	/* fail on missing close quote */
			} else if (c == '\0')
				break;
			else
				cp[-1] = '\0';	/* NUL-terminate arg */
		}
	}
	nargv[argc++] = file;			/* script name */

	/* Copy the old arguments. */
	for (ap = argv + 1; *ap != NULL; ++ap)
		nargv[argc++] = *ap;
	nargv[argc] = NULL;

	/* Go for smoke. */
	_execve(interp, nargv, envp);
	free(nargv);
	return -1;
}

/* end of libc/sys/i386/execve.c */
