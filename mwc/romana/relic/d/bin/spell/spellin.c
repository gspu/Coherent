/*
 * Simply write words from standard
 * input (must be sorted) to standard
 * output in compressed form.  The
 * functionality is different than the
 * hashed version of spell. The assumption
 * is that you can sort -m each of the
 * dictionaries and always normally stored
 * word lists as well.
 */

#include <stdio.h>

char	word[100];

main(argc, argv)
char *argv[];
{
	if (argc > 1)
		usage();
	init();
	while (gets(word) != NULL)
		putword(word, stdout);
	fflush(stdout);
	if (ferror(stdout)) {
		fprintf(stderr, "spellin: I/O error on output\n");
		exit(1);
	}
	exit(0);
}

usage()
{
	fprintf(stderr, "Usage:  spellin\n");
	exit(1);
}
