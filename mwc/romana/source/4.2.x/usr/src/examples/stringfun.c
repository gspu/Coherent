#include <stdio.h>

#define NNAMES 512
#define MAXLEN 60

char *array[NNAMES];
char first[MAXLEN], mid[MAXLEN], last[MAXLEN];
char *space = " ";

int compare();
extern char *strcat();

main()
{
	register int index, count, inflag;
	register char *name;

	count = 0;
	while (scanf("%s %s %s\n", first, mid, last) == 3) {
		strcat(first, space);
		strcat(mid, space);
		name = strcat(first, (strcat(mid, last)));
		inflag = 0;

		for (index=0; index < count; index++) 
			if (strcmp(array[index], name) == 0) 
				inflag = 1;

		if (!inflag) {
			if ((array[count] = 
				malloc(strlen(name) + 1)) == NULL) {
				fprintf(stderr, 
				"insufficient memory is available\n");
				exit(1);
			}
			strcpy(array[count], name);
			count++;
		}
	}

	shellsort(array, count, sizeof(char *), compare);
	for (index=0; index < count; index++)
		printf("%s\n", array[index]);
	exit(0);
}

compare(s1, s2)
register char **s1, **s2;
{
	extern int strcmp();
	return(strcmp(*s1, *s2));
}

