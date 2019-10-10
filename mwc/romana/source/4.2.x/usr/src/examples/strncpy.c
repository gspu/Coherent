#include <stdio.h>
#define NNAMES 512
#define MAXLEN 60

char *array[NNAMES];
char gname[MAXLEN], lname[MAXLEN];
extern int strncmp(), strcomp();
extern char *strcpy(), *strncpy(), *strncat(), *index();

main(argc, argv)
int argc; char *argv[];
{
	FILE *fp;
	register int count, num;
	register char  *name, string[60], *cptr, *eptr;
	unsigned glength, length;

	if (argc != 2) {
		fprintf (stderr, "Usage: strnmcpy filename\n");
		exit(1);
	}

	if ((fp = fopen(argv[1], "r")) == NULL) {
		printf("Cannot open %s\n", argv[1]);
	}
	count = 0;

	while (fgets(string, 60, fp) != NULL) {
		if ((cptr = index(string, '.')) != NULL) {
			cptr++; 
			cptr++;
		} else 
			if ((cptr = index(string,' ')) != NULL)
				cptr++;

		strcpy(lname, cptr);
		eptr = index(lname, '\n');
		*eptr = ','; 

		strcat(lname," "); 
		glength = (unsigned)(strlen(string) - strlen(cptr));
		strncpy(gname, string, glength);

		name = strncat(lname, gname, glength);
		length = (unsigned)strlen(name);
		array[count] = malloc(length + 1);

		strcpy(array[count],name);
		count++;
	}

	for (num = 0; num < count; num++)
		printf("%s\n", array[num]);
	exit(0);
}
