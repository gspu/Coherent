#include <stdio.h>
#define MAXLINE 128

char buffer[MAXLINE];

main(argc,argv)
int argc; 
char *argv[];
{
	FILE *fpin;
	FILE *fpout;

	if (argc != 3 && argc != 4)
		fatal("Usage: freopen pattern infile [outfile]");
	if ((fpin = fopen(argv[2], "r")) == NULL)
		fatal("Cannot open input file");

	fpout = stdout;
	if (argc == 4)
		if ((fpout = freopen(argv[3], "w", stdout))
						== NULL)	
			fatal("Cannot open output file");

	while (fgets(buffer, MAXLINE, fpin) != NULL) {
		if (pnmatch(buffer, argv[1], 1)) 
			fputs(buffer, stdout);  
	}
	exit(0);
}

fatal(message)
char *message;
{
	fprintf(stderr, "match: %s\n", message);
	exit(1);
}
