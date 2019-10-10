/*
 * from - generate a list of numbers
 * usage - from number to number [ by number ]
 */
#include <stdio.h>

#define sign(x)	(x<0?-1:1)	/* returns sign of number */

char usage[] = "Usage: from number to number [by number]\n";
char signmsg[] = "Increment has wrong sign\n";

main(argc, argv)
char *argv[];
{
	int i;
	int start, end, incr;

	switch ( argc ) {

	case 4:
		incr = 1;
		break;

	case 6:
		if ( strcmp(argv[4], "by") )
			error(usage);
		else
			if ( (incr = numeric(argv[5])) == 0 )
				error("Increment must be non-zero\n");
		break;

	default:
		error(usage);
		break;
	}
	if ( strcmp(argv[2], "to") )
		error(usage);
	start = numeric(argv[1]);
	end = numeric(argv[3]);
	if ( sign(end-start) * sign(incr) < 0 )
		error(signmsg);

	if ( incr > 0 )
		for ( i = start; i <= end; i += incr )
			printf("%d\n", i);
	else
		for ( i = start; i >= end; i += incr )
			printf("%d\n", i);
	exit(0);
}

/*
 * Return the value of a numeric arg;
 * otherwise, call usage().
 */
numeric(s)
register char *s;
{
	register int n;

	n = atoi(s);
	if ( *s == '-' )
		s++;
	for ( ; *s; s++)
		if (*s<'0' || *s>'9')
			error(usage);
	return(n);
}

error(x)
{
	fprintf(stderr, "%r", &x);
	exit(1);
}
