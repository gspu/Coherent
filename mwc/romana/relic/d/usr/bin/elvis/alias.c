/* alias.c */

/* Author:
 *		Peter Reinig
 *		Universitaet Kaiserslautern
 *		Postfach 3049
 *		7650 Kaiserslautern
 *		W. Germany
 *		reinig@physik.uni-kl.de
 */

/* This tiny program executes elvis with the flags that are appropriate
 * for a given command name.  This program is used only on systems that
 * don't allow UNIX-style file links.
 *
 * The benefit of this program is: instead of having 5 copies of elvis
 * on your disk, you only need one copy of elvis and 4 copies of this
 * little program.
 */

#include <stdio.h>
#include "config.h"

#if OSK
#define ELVIS	"/dd/usr/cmds/elvis"
#else
#define ELVIS	"elvis"
#endif

extern char **environ;
extern int errno;

main(argc, argv)
	int argc;
	char *argv[];
{
	int pid, i;
	char **argblk;
#if OSK
	extern int chainc();
#endif

	if ((argblk = (char **) malloc((argc + 2) * sizeof(char *))) == 0)
	{
#if OSK
		_errmsg(errno, "Can't get enough memory\n");
#else
		fprintf(stderr, "%s: Can't get enough memory\n", argv[0]);
#endif
		exit(2);
	}
	i = 1;
	switch (argv[0][strlen(argv[0]) - 1])
	{
	  case 'w':			/* "view" */
	  case 'W':
		argblk[i++] = "-R";
		break;

#if !OSK
	  case 'x':			/* "ex" */
	  case 'X':
		argblk[i++] = "-x";
		break;
#endif

	  case 't':			/* "input" */
	  case 'T':
		argblk[i++] = "-i";
		break;
	}
	argv++;
	while(argc--)
		argblk[i++] = *argv++;
	argblk[0] = ELVIS;
#if OSK
	pid = os9exec(chainc, argblk[0], argblk, environ, 0, 0, 3);
#else
	(void)execvp(argblk[0], argblk);
#endif
	fprintf(stderr, "%s: cannot execute\n", argblk[0]);
}
