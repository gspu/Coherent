/*
 * Fix up sorted document from tabbld
 */
#include <stdio.h>

main()
{
	char *strchr();
	char buf[256];
	char opc[10];
	int lineno, gen, lgen;

	lgen = 2;	/* nogen */
	while (NULL != gets(buf)) {
		sscanf(buf, "%s %d %d!", opc, &lineno, &gen);
		if ((1 != gen) &&	/* not part of a general op */
		    ((1 == lgen) ||	/* previous was part of general */
		     (0 == gen)))	/* start of general */
			printf(".sp \\n(pDu\n");

		lgen = gen;
		puts(strchr(buf, '!') + 1);
	}
}
