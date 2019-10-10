#include <ctype.h>
#include <stdio.h>

main()
{
	FILE *fp;
	char fname[20];
	int ch;
	int alnum = 0;
	int alpha = 0;
	int allow = 0;
	int control = 0;
	int printable = 0;
	int punctuation = 0;
	int space = 0;

	printf("Enter name of text file to examine: ");
	fflush(stdout);
	gets(fname);

	if ((fp = fopen(fname, "r")) != NULL) {
		while ((ch = fgetc(fp)) != EOF) {

			if (isascii(ch)) {
				if (isalnum(ch)) 
					alnum++;
				if (isalpha(ch)) 
					alpha++;
				if (islower(ch)) 	
					allow++;
				if (iscntrl(ch)) 	
					control++;
				if (isprint(ch)) 
					printable++;
				if (ispunct(ch)) 
					punctuation++;
				if (isspace(ch)) 
						space++;
			} else {
				printf("%s is not ASCII.\n", 
						fname);
				exit(1);
			}
		}

		printf("%s has the following:\n", fname);
		printf("%d alphanumeric characters\n", alnum);
		printf("%d alphabetic characters\n", alpha);
		printf("%d alphabetic lower-case characters\n", 
			allow);
		printf("%d control characters\n", control);
		printf("%d printable characters\n", printable);
		printf("%d punctuation marks\n", punctuation);
		printf("%d white space characters\n", space);
		exit(0);
	} else  {
		printf("Cannot open \"%s\".\n", fname);
		exit(2);
	}
}


