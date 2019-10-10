/*
 * Reimplementation of SysVr3 sh builtin command "getopts" for S5R2 shell.
 *
 * created by Arnold Robbins
 * modified by Doug Gwyn
 * modified for PD ksh by Eric Gisin
 */

static char *RCSid = "$Header: getopts.c,v 3.1 88/11/03 09:16:01 egisin Exp $";

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include "sh.h"
#include "table.h"

/*
 * The following is derived from getopt() source placed into the public
 * domain by AT&T (the only time they're known to have done that).
 *
 * It has been modified somewhat to fit into the context of the shell.
 *
 * -D"FASCIST" if you really want to strictly enforce ALL the
 * Command Syntax Standard rules (not recommended).
 */

#define GETOPTEOF	(-1)
#define ERR(S, C)	shellf("%s%c\n", (S), (C))

	int	optind;
static char    *optarg;
static int	sp;

static int
getopt(argc, argv, opts)
	int argc;
	register char **argv, *opts;
{
	register int c;
	register char *cp;

	if (sp == 1)
		if (optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(GETOPTEOF);
		else if (strcmp(argv[optind], "--") == 0) {
			optind++;
			return(GETOPTEOF);
		}
	c = argv[optind][sp];
	if (c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR("illegal option -- ", c);
		if (argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		optarg = NULL;
		return('?');
	}
	if (*++cp == ':') {
#if FASCIST
		if (sp != 1) {
			ERR("option must not be grouped -- ", c );
			optind++;
			sp = 1;
			optarg = NULL;
			return('?');
		} else
#endif
		if (argv[optind][sp+1] != '\0') {
#if FASCIST
			ERR("option must be followed by whitespace -- ", c );
			optind++;
			sp = 1;
			optarg = NULL;
			return('?');
#else
			optarg = &argv[optind++][sp+1];
#endif
		} else if (++optind >= argc) {
			ERR("option requires an argument -- ", c);
			sp = 1;
			optarg = NULL;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if (argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

/*
 * The following were created by Arnold Robbins.
 */

/* resetopts --- magic code for when OPTIND is reset to 1 */

void
resetopts ()
{
	optind = 1;
	sp = 1;
}

int
c_getopts(wp)
	char **wp;
{
	int ret;
	register int argc;
	char temp[2];
	char *optstr;			/* list of options */
	char *name;			/* variable to get flag val */

	if ((optstr = *++wp) == NULL || (name = *++wp) == NULL)
		errorf("missing arguments\n");

	for (argc = 1; wp[argc] != NULL; argc++)
		;

	if (argc > 1)
		ret = getopt(argc, wp, optstr);
	else
		ret = getopt(e.loc->argc+1, e.loc->argv, optstr);

	/*
	 * set the OPTIND variable in any case, to handle "--" skipping
	 */

	setint(global("OPTIND"), (long)optind);

	if (ret == GETOPTEOF)		/* end of args */
		return (1);

	/*
	 * else, got an arg, set the various shell variables
	 */

	if (optarg != NULL)
		setstr(global("OPTARG"), optarg);

	temp[0] = (char) ret;
	temp[1] = '\0';
	setstr(global(name), temp);

	return (0);
}

