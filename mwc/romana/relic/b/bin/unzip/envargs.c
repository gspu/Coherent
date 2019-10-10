/*****************************************************************
 | envargs - add default options from environment to command line
 |----------------------------------------------------------------
 | Author: Bill Davidsen, original 10/13/91, revised 23 Oct 1991.
 | This program is in the public domain.
 |----------------------------------------------------------------
 | Minor program notes:
 |  1. Yes, the indirection is a tad complex
 |  2. Parenthesis were added where not needed in some cases
 |     to make the action of the code less obscure.
 |  3. Set tabsize to four to make this pretty
 |----------------------------------------------------------------
 | UnZip notes 24 May 92 ("v1.4"):
 |  1. #include "unzip.h" for prototypes
 |  2. changed ch to type char
 |  3. added an ifdef to avoid Borland warnings
 *****************************************************************/

#include "unzip.h"
static int count_args __((char *));
static void mem_err __((void));

#if (defined(SCCS) && !defined(lint))  /* causes warnings:  annoying */
static char *SCCSid = "@(#)envargs.c	1.3 23 Oct 1991";
#endif

void
envargs(Pargc, Pargv, envstr)
int *Pargc;
char ***Pargv;
char *envstr;
{
	char *getenv();
	char *envptr;				/* value returned by getenv */
	char *bufptr;				/* copy of env info */
	int argc = 0;				/* internal arg count */
	char ch;					/* spare temp value */
	char **argv;				/* internal arg vector */
	char **argvect;				/* copy of vector address */

	/* see if anything in the environment */
	envptr = getenv(envstr);
	if (envptr == (char *)NULL || *envptr == 0) return;

	/* count the args so we can allocate room for them */
	argc = count_args(envptr);
	bufptr = (char *)malloc(1+strlen(envptr));
	if (bufptr == (char *)NULL) mem_err();
	strcpy(bufptr, envptr);

	/* allocate a vector large enough for all args */
	argv = (char **)malloc((argc+*Pargc+1)*sizeof(char *));
	if (argv == (char **)NULL) mem_err();
	argvect = argv;

	/* copy the program name first, that's always true */
	*(argv++) = *((*Pargv)++);

	/* copy the environment args next, may be changed */
	do {
		*(argv++) = bufptr;
		/* skip the arg and any trailing blanks */
		while (((ch = *bufptr) != '\0') && ch != ' ') ++bufptr;
		if (ch == ' ') *(bufptr++) = '\0';
		while (((ch = *bufptr) != '\0') && ch == ' ') ++bufptr;
	} while (ch);

	/* now save old argc and copy in the old args */
	argc += *Pargc;
	while (--(*Pargc)) *(argv++) = *((*Pargv)++);

	/* finally, add a NULL after the last arg, like UNIX */
	*argv = (char *)NULL;

	/* save the values and return */
	*Pargv = argvect;
	*Pargc = argc;
}

static int
count_args(s)
char *s;
{
	int count = 0;
	char ch;

	do {
		/* count and skip args */
		++count;
		while (((ch = *s) != '\0') && ch != ' ') ++s;
		while (((ch = *s) != '\0') && ch == ' ') ++s;
	} while (ch);

	return count;
}

static void
mem_err()
{
	perror("Can't get memory for arguments");
	exit(2);
}

#ifdef TEST
main(argc, argv)
int argc;
char **argv;
{
	int i;

	printf("Orig argv: %p\n", argv);
	dump_args(argc, argv);
	envargs(&argc, &argv, "ENVTEST");
	printf(" New argv: %p\n", argv);
	dump_args(argc, argv);
}

dump_args(argc, argv)
int argc;
char *argv[];
{
	int i;

	printf("\nDump %d args:\n", argc);
	for (i=0; i<argc; ++i) {
		printf("%3d %s\n", i, argv[i]);
	}
}
#endif
