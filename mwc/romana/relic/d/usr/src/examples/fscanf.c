#include <stdio.h>

main ()
{
	FILE *fp;
	char let[4];

	/* open file into write/read mode */
	if ((fp = fopen("tmpfile", "wr")) == NULL) {
		printf("Cannot open 'tmpfile'\n");
		exit(1);
	}

	/* write a string of chars into file */
	fprintf(fp, "1234");

	/* move file pointer back to beginning of file */
	rewind(fp);

	/* read and print data from file */
	fscanf(fp, "%c %c %c %c",
		&let[0], &let[1], &let[2], &let[3]);
	printf("%c %c %c %c\n",
		let[3], let[2], let[1], let[0]);
	exit(0);
}
