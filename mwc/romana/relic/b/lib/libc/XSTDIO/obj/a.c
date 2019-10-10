#include "stdio.h"
main()
{
	register int i;
	for (i = 0; i < 100; i++)
		printf("hello ANSI world %d via printf\n", i);
	fprintf(stdout, "hello ANSI world via stdout\n");
/*	fflush(stdout); */
	fprintf(stderr, "hello ANSI world via stderr\n");
}
