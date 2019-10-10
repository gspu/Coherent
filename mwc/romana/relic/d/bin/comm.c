/*
 * comm -- see which lines two sorted files have in common
 * Tom Duff 80.08.08 (how symmetrical)
 */

#include <stdio.h>

#define	LLEN	512	/* maximum line length */

int col1 = 1;
int col2 = 1;
int col3 = 1;
char line1[LLEN];
char line2[LLEN];
FILE *fd1, *fd2;

main(argc, argv)
char *argv[];
{
	register int s;

	if (argc < 2)
		usage();
	if (argv[1][0]=='-' && argv[1][1]!='\0') {
		while (argv[1][1]) {
			switch (argv[1][1]) {
			case '1':
				col1 = 0;
				break;

			case '2':
				col2 = 0;
				break;

			case '3':
				col3 = 0;
				break;

			default:
				usage();
			}
			++argv[1];
		}
		--argc;
		argv++;
	}
	if (argc != 3)
		usage();
	if (strcmp(argv[1], "-") == 0)
		fd1 = stdin;
	else if ((fd1 = fopen(argv[1], "r")) == NULL){
		fprintf(stderr, "comm: can't open %s\n", argv[1]);
		exit(1);
	}
	if (strcmp(argv[2], "-") == 0)
		fd2 = stdin;
	else if ((fd2 = fopen(argv[2], "r")) == NULL){
		fprintf(stderr, "comm: can't open %s\n", argv[2]);
		exit(1);
	}
	fgets(line1, LLEN, fd1);
	fgets(line2, LLEN, fd2);
	while (!feof(fd1) && !feof(fd2))
		if ((s = strcmp(line1, line2)) < 0)
			print1();
		else if (s > 0)
			print2();
		else
			print3();
	while (!feof(fd1))
		print1();
	while (!feof(fd2))
		print2();
	exit(0);
}

print1()
{
	if (col1)
		printf("%s", line1);
	fgets(line1, LLEN, fd1);
}

print2()
{
	if (col2){
		if (col1)
			putchar('\t');
		printf("%s", line2);
	}
	fgets(line2, LLEN, fd2);
}

print3()
{
	if (col3){
		if (col1)
			putchar('\t');
		if (col2)
			putchar('\t');
		printf("%s", line1);
	}
	fgets(line1, LLEN, fd1);
	fgets(line2, LLEN, fd2);
}

usage()
{
	fprintf(stderr, "Usage: comm [-[123]] file1 file2\n");
	exit(1);
}
