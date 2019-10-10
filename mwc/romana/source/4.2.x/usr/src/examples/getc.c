#include <stdio.h>

void error();

main(argc, argv)
int argc; char *argv[];
{
	int foo;
	FILE *source, *dest;

	if (argc != 3)
		error("Usage: getc source destination");

	if ((source = fopen(argv[1], "r")) == NULL)
		error("Cannot open source file");
	if ((dest = fopen(argv[2], "w")) == NULL)
		error("Cannot open destination file");

	while ((foo = getc(source)) != EOF)
		putc(foo, dest);
	exit(0);
}

void error(string)
char *string;
{
	printf("%s\n", string);
	exit (1);
}

