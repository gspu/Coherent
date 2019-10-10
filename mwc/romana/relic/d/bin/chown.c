/*
 * Change the owner of specified files.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>

main(argc, argv)
char *argv[];
{
	register struct passwd *pwp;
	register int c;
	register short owner, group;
	struct stat sb;
	register short status = 0;

	if (argc < 3)
		usage();
	if ((c = *argv[1])>='0' && c<='9')
		owner = atoi(argv[1]);
	else {
		if ((pwp = getpwnam(argv[1])) == NULL)
			cherr("Bad username `%s'\n", argv[1]);
		owner = pwp->pw_uid;
	}
	for (c = 2; c < argc; c++) {
		group = 0;
		if (stat(argv[c], &sb) >= 0)
			group = sb.st_gid;
		if (chown(argv[c], owner, group) < 0) {
			perror(argv[c]);
			status = 2;
		}
	}
	exit (status);
}

usage()
{
	fprintf(stderr, "Usage: chown owner file ...\n");
	exit(1);
}

/* VARARGS */
cherr(x)
{
	fprintf(stderr, "chown: %r", &x);
	exit(2);
}
