/*
 * libc/gen/mktemp.c
 * Produce a unique filename by replacing
 * the trailing 6 X's on the user's input string
 * with process id and a unique letter.
 * (It is assumed, but not checked, that the X's are present.)
 * Revised by Michael Griffin, 7-5-84; by steve, 1/5/93.
 * Avoids unsuitable characters in generator.
 * Repeats after 26+26+10+1=63 invocations by the same process.
 */

char *
mktemp(template) char *template;
{
	static char generator = 'a';
	register char *p;
	register int i, pid;


	for (p = template; *p; p++)
		;				/* scan to end of template */
	*--p = generator;			/* store generator */
	switch (generator)  {			/* bump generator */
	case 'z':	generator = 'A';		break;
	case 'Z':	generator = '0';		break;
	case '9':	generator = '_';		break;
	case '_':	generator = 'a';		break;
	default:	++generator;			break;
	}
	pid = getpid();
	for (i = 5; i-- > 0; ) {		/* store 5-digit ASCII pid */
		*--p = pid%10 + '0';
		pid /= 10;
	}
	return template;
}

/* end of libc/gen/mktemp.c */
