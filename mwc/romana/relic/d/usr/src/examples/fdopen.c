#include <ctype.h>
#include <stdio.h>

void adios();

main(argc, argv)
int argc; char *argv[];
{
	extern FILE *fdopen();
	FILE *fd;
	int fo;
	int holder;

	if (--argc != 1)
		adios("Usage: fdopen example_filename");
	if ((fo = open(argv[1], 0)) == -1)
		adios("open failed.");
	if ((fd = fdopen(fo, "r")) == NULL)
		adios("fdopen failed.");

	while ((holder = fgetc(fd)) != EOF) {
		if ((holder > '\177') && (holder < ' '))
			switch(holder) {
				case '\t':
				case '\n':
					break;
				default:
					fprintf(stderr, 
					"Seeing char %d\n", holder);
					exit(1);
			}
		fputc(holder, stdout);
	}
	exit(0);
}

void adios(message)
char *message;
{
	fprintf(stderr, "%s\n", message);
	exit(1);
}
