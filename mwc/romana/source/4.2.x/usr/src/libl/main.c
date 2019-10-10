/* (-lgl
 * 	COHERENT Version 3.1
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#include <stdio.h>

int
main ()
{
	while (yylex () != EOF)
		/* DO NOTHING */ ;

	return 0;
}
