/*
 * libc/gen/_addargs.c
 * Add environmental args to argc/argv.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define	NBUF	128

/* External. */
extern char *getenv();
extern char *malloc();

/* Forward. */
static	char	**copyargs();
static	int	getargs();
static	char	*scanarg();

/*
 * Add "nameHEAD" and "nameTAIL" to the supplied argc/argv.
 * Return -1 on failure, 0 if argc/argv unchanged, 1 otherwise.
 */
int
_addargs(name, argcp, argvp) char *name; int *argcp; char ***argvp;
{
	register char **cpp, **oargv;
	register int oargc, hargc, targc;
	char *hp, *tp;
	
	if ((hargc = getargs(name, "HEAD", &hp)) == -1)
		return -1;
	if ((targc = getargs(name, "TAIL", &tp)) == -1)
		return -1;
	if (hargc == 0 && targc == 0)
		return 0;
	oargc = *argcp;
	oargv = *argvp;
	if ((cpp = (char **)malloc((hargc + oargc + targc + 1) * sizeof(char *))) == NULL)
		return -1;
	*argcp = hargc + oargc + targc;
	*argvp = cpp;
	*cpp++ = *oargv++;			/* old argv[0] */
	if (hargc != 0)
		cpp = copyargs(cpp, hp);	/* HEAD args */
	while (*oargv != NULL)
		*cpp++ = *oargv++;		/* remainder of old argv[] */
	if (targc != 0)
		cpp = copyargs(cpp, tp);	/* TAIL args */
	*cpp = NULL;				/* NULL-terminate */
	return 1;
}

/*
 * NUL-terminate each whitespace-separated arg in p
 * and store a pointer to each arg in cpp.
 * Return the updated cpp.
 */
static
char **
copyargs(cpp, p) register char **cpp; register char *p;
{
	register char c;

	while ((c = *p) != '\0') {
		if (isspace(c))
			*p++ = '\0';
		else if (c == '\'' || c == '"') {
			*cpp++ = p + 1;
			p = scanarg(p);
			if (*(p-1) == c)
				*(p-1) = '\0';
		} else {
			*cpp++ = p++;
			p = scanarg(p);
		}
	}
	return cpp;
}

/*
 * Look for environmental variable "<name><ext>".
 * If found, malloc a copy and store its address through cpp.
 * Return -1 if malloc fails,
 * else return count of whitespace-separated args in result.
 */
static
int
getargs(name, ext, cpp) char *name, *ext; char **cpp;
{
	register char *cp, *p;
	register int count;
	char buf[NBUF];

	if ((cp = getenv(strcat(strcpy(buf, name), ext))) == NULL)
		return 0;
	if ((p = malloc(strlen(cp) + 1)) == NULL)
		return -1;
	*cpp = strcpy(p, cp);
	count = 0;
	while (*p != '\0') {
		if (!isspace(*p)) {
			++count;
			p = scanarg(p);
		} else
			++p;
	}
	return count;
}

/*
 * Scan arg p to next whitespace or NUL.
 * Ignore whitespace within args quoted by ' ' or " ".
 * Return pointer to following character.
 */
static
char *
scanarg(p) register char *p;
{
	register char c;

	if ((c = *p) == '\'' || c == '"') {
		++p;
		while (*p != '\0' && *p++ != c)
			;
	} else
		while (!isspace(*p) && *p != '\0')
			++p;
	return p;
}

/* end of _addargs.c */
