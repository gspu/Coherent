/*
 * execve(), tmpnam(), getenv(), path() example for the COHERENT.
 * find all lines with more than LIMIT characters and call 'me'
 * to edit them.
 */
#include <stdio.h>
#include <path.h>
#include <sys/stat.h>

#define LIMIT 70

extern char *getenv(), **environ, *tempnam();

main(argc, argv)
char *argv[];
{
	/*		  me     -e   tmp   file */
	char *cmda[5] = { NULL, "-e", NULL, NULL, NULL };
	FILE *ifp, *tmp;
	char line[256];
	int  ct, len;

	if ((NULL == (cmda[3] = argv[1])) ||
	    (NULL == (ifp = fopen(argv[1], "r")))) {
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		exit(1);
	}

	if (NULL == (cmda[0] = path(getenv("PATH"), "me", AEXEC))) {
		fprintf(stderr, "Cannot locate me\n");
		exit(1);
	}

	if (NULL == (tmp = fopen((cmda[2] = tempnam(NULL, "lng")), "w"))) {
		fprintf(stderr, "Cannot open tmpfile\n");
		exit(1);
	}

	for (ct = 1; NULL != fgets(line, sizeof(line), ifp); ct++)
		if (((len = strlen(line)) > LIMIT) ||
		    ('\n' != line[len -1]))
			fprintf(tmp, "%d: %d characters long\n", ct, len);

	fclose(tmp);
	fclose(ifp);

	if (execve(cmda[0], cmda, environ) < 0) {
		fprintf(stderr, "cannot execute me\n");
		exit(1);
	}
	/* We never reach here ! */
}
