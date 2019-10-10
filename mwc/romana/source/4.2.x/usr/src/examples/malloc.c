#include <stdio.h>
#define NITEMS 512
#define MAXLEN 256

char *data[NITEMS];
char string[MAXLEN];

main()
{
	register char **cpp;
	register int count;
	extern int compare();
	extern char *malloc();
	extern char *gets();

	for (cpp = &data[0]; cpp < &data[NITEMS]; cpp++) {
		if (gets(string) == NULL)
			break;
		if ((*cpp = malloc(strlen(string) + 1)) == NULL)
			exit(1);
		strcpy(*cpp, string);
	}

	count = cpp - &data[0];
	qsort(data, count, sizeof(char *), compare);

	for (cpp = &data[0]; cpp < &data[count]; cpp++) {
		printf("%s\n", *cpp);
		free(*cpp);
	}
	exit(0);
}

compare(p1, p2)
register char **p1, **p2;
{
	extern int strcmp();
	return(strcmp(*p1, *p2));
}
