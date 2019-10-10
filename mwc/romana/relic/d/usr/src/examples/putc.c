#include <stdio.h>
main()
{
	FILE *fp;
	int ch;
	int filename[20];
	printf("Enter file name: ");
	gets(filename);

	if ((fp = fopen(filename,"r")) != NULL) {
		while ((ch = fgetc(fp)) != EOF)
			putc(ch, stdout);
	} else
		printf("Cannot open %s.\n", filename);
	fclose(fp);
}
