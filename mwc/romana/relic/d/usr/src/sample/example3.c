/*
 * Factor prints out the prime factorization of numbers.
 * If there are any arguments, then it factors these.  If
 * there are no arguments, then it reads stdin until
 * either EOF or the number zero or a non-numeric
 * non-white-space character.  Since factor does all of
 * its calculations in double format, the largest number
 * which can be handled is quite large.
 */
#include <stdio.h>
#include <math.h>
#include <ctype.h>


#define	NUL	'\0'
#define	ERROR	0x10	/* largest input base */
#define	MAXNUM	200	/* max number of chars in number */


main(argc, argv)
int		argc;
register char	*argv[];
{
	register char	*chp;
	double		n;
	double		atod();
	char		*getnum();

	if (argc != 1)
		while ((chp=*++argv) != NULL &&
			(n=atod(chp)) != 0)
			factor(n);
	else
		while ((chp=getnum()) != NULL &&
			(n=atod(chp)) != 0)
			factor(n);
	return (0);
}
die(str)
char	*str;
{
	fprintf(stderr, "%r\en", &str);
	exit(1);
}

usage()
{
	die("usage: factor [number number ...]");
}


char	*
getnum()
{
	register char	*chp,
			ch;
	static char	res[MAXNUM+1];

	do {
		ch = getchar();
	} while (isascii(ch) && isspace(ch));
	if (!isascii(ch) || todigit(ch) == ERROR)
		return (NULL);
	for (chp=res; isascii(ch) && !isspace(ch);
		ch=getchar())
		if (chp < &res[MAXNUM])
			*chp++ = ch;
	if (chp >= &res[MAXNUM])
		die("Number too big");
	*chp++ = NUL;
	return (res);
}
