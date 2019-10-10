#include "stdio.h"
xread(file) char *file;
{
	FILE *fp;
	int c;

	fp = fopen(file, "r");
	while ((c = getc(fp)) != EOF)
		write(1, &c, 1);
	fclose(fp);
}
