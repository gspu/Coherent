#include <stdio.h>

/* ipcs usage. Print message and die */
usage(c) 
int	c;
{
	fprintf(stderr, "ipcs:  illegal option - %c\n", c);
	fprintf(stderr, "usage: "
		 "ipcs [-abcmopqst] [-C corefile] [-N namelist]\n");
	exit(1);
}
