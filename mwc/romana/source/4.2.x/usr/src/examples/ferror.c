#include <stdio.h>

main()
{
	FILE *fpin, *fpout;
	int inerr = 0;
	int outerr = 0;
	int word;
	char infile[20], outfile[20];

	printf("Name data file you wish to copy:\n");
	gets(infile);
	printf("Name new file:\n");
	gets(outfile);

	if ((fpin = fopen(infile, "r")) != NULL) {
		if ((fpout = fopen(outfile, "w")) != NULL) {
			for (;;) {
				word = fgetw(fpin);
				if (ferror(fpin)) {
					clearerr(fpin);
					inerr++;
				}	
				if (feof(fpin))
					break;
				fputw(word, fpout);
				if (ferror(fpout)) {
					clearerr(fpout);
					outerr++;
				}
			}
		} else {	
			printf
				("Cannot open output file %s\n", 
					outfile);
			exit(1);
		}
	} else {
		printf("Cannot open input file %s\n", infile);
		exit(1);
	}	
	printf("%d - read error(s)  %d - write error(s)\n", 
		inerr, outerr);
	exit(0);
}
