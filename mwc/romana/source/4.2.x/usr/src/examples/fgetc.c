#include <stdio.h>

main()
{
	FILE *fp;
	int ch;
	int filename[20];
	int nlines = 0;
	int nsents = 0;

	printf("Enter file to test: ");
	gets(filename);

	if ((fp = fopen(filename,"r")) == NULL) {
		printf("Cannot open %s.\n", filename);
		exit(1);
	}
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == '\n')
			++nlines;
		else
			if (ch == '.' || ch == '!' 
					|| ch == '?') {
				if ((ch = fgetc(fp)) != '.') 
					++nsents;
				else 
					while 
					((ch=fgetc(fp)) == '.')
						;                              
				ungetc(ch, fp);
			}
	} 				
	printf("%d line(s), %d sentence(s).\n", nlines, nsents);
}

