/*
 * Produce a unique filename by replacing
 * the trailing 6 X's on the user's input string
 * with process id and a unique letter.
 * (It is assumed, but not checked, that the X's are present.)
 * Revised by Michael Griffin, 7-5-84.  Avoids control characters in
 * generator.  Repeats after 91 invocations by the same process.
 */

static	char	generator = 'a';

char *
mktemp(template)
char *template;
{
	register char *p;
	register i, pid;

	for (p = template; *p; p++)
		;
	switch (generator)  {
		case '~':
			*--p = generator;
			generator = 'A';
			break;
		case '_':
			*--p = generator;
			generator = '!';
			break;
		case '*':
			*--p = ++generator;
			generator++;
			break;
		case '?':
			*--p = (generator = '@');
			break;
		case '@':
			generator = 'a';
			*--p = generator++;
			break;
		default:
			*--p = generator++;
	}
	pid = getpid();
	i = 5; do {
		*--p = pid%10 + '0';
		pid /= 10;
	} while (--i);
	return (template);
}
