/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Print directry name.
 */
#include <stdio.h>

main(argc, argv)
int	argc;
char	*argv[];
{
	extern char	*rindex();
	char		*pch;

	if (argc > 2) {
		fprintf(stderr, "Usage: dirname [path]\n");
		exit(1);
	}
	
	if ((pch = rindex(argv[1], '/')) == NULL)
		puts(".");
	else 
		if (pch == argv[1])
			puts("/");
		else {
			*pch ='\0';
			puts(argv[1]);
		}
}
