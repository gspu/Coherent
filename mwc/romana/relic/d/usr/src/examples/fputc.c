#include <stdio.h>

main()
{
	FILE *fp, *fout;
	int ch;
	int infile[20];
	int outfile[20];

	printf("Enter name to copy: ");
	gets(infile);
	printf("Enter name of new file: ");
	gets(outfile);

	if ((fp = fopen(infile,"r")) != NULL) {
		if ((fout = fopen(outfile,"w")) != NULL) 
			while ((ch = fgetc(fp)) != EOF) 
				fputc(ch, fout);
		else
			printf("Cannot write %s.\n", outfile);
	} else
		printf("Cannot read %s.\n", infile);
}

