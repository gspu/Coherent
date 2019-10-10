#include "asm.h"

/*
 * Note an error.
 * If the error is a `q' just
 * give up.
 */
err(c, msg)
char *msg;
{
	if (pass == 2) {
		++nerr;
		if (ifn != NULL)
			fprintf(efp, "%d: %s: %s\n", line, ifn, msg);
		else
			fprintf(efp, "%d: %s\n", line, msg);
	}
	if (c == 'q')
		longjmp(env, 1);
}

/*
 * Note a `u' error.
 * Tag it with the name if not making
 * a listing file.
 */
uerr(id)
char *id;
{
	char work[40];

	sprintf(work, "undefined symbol '%s'", id);
	err('u', work);
}

/*
 * Note an 'r' error.
 */
rerr(msg)
char *msg;
{
	err('r', msg);
}

/*
 * Note an 'a' error.
 */
aerr(msg)
char *msg;
{
	err('a', msg);
}

/*
 * Note a 'q' error.
 */
qerr(msg)
char *msg;
{
	err('q', msg);
}

static int lastline = 0;
/*
 * Note Data in bssd
 */
serr()
{
	if(line != lastline) {
		err('s', "data in bssd");
		lastline = line;
	}
}
