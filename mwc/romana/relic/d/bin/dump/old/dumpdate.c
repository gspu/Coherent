#include <stdio.h>
#include <dumptape.h>

char	*ctime();

struct	idates	id;

main(argc, argv)
char *argv[];
{
	register FILE *ddfp;
	register char *cp;
	register i;

	if ((ddfp = fopen(DDATE, "r")) == NULL) {
		fprintf(stderr, "dumpdate: cannot open date file\n");
		exit(1);
	}
	while (fread(&id, sizeof(id), 1, ddfp) == 1) {
		if (argc > 1) {
			for (i=1; i<argc; ++i) {
				if (devcmp(id.id_name, argv[i]) == 0)
					break;
			}
			if (i == argc)
				continue;
		}
		cp = ctime(&id.id_ddate);
		cp[24] = '\0';
		printf("Level %d %s %.*s\n", id.id_incno, cp, DIRSIZ,
			id.id_name);
	}
	exit(0);
}

devcmp(p1, p2)
char *p1;
register char *p2;
{
	register char *p3;
	register c;

	p3 = p2;
	while ((c = *p2++) != '\0') {
		if (c == '/')
			p3 = p2;
	}
	return (strncmp(p1, p3, DIRSIZ));
}
