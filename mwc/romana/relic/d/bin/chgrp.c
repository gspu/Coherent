/*
 * Change the group owner of specified files.
 */

/*
 * chgrp didn't check numerical group id; prints out strange
 * error messages, didn't check effective uid. Fixed 1-06-91. Vlad.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <grp.h>
#include <ctype.h>	/* Vlad */

main(argc, argv)
char *argv[];
{
	register struct group *grp;
	register int c;
	register short owner, group;
	struct stat sb;
	register short status = 0;

	/* Only superuser can invoke chgrp under COHERENT. Vlad */
	if (geteuid())
		cherr("not a super user\n");

	if (argc < 3)
		usage();
	
	/* You can specify group by numeric id or by name. Vlad */
	if (isdigit((int) *argv[1])) {
		if ((grp = getgrgid(atoi(argv[1]))) == NULL)
			cherr("bad numeric group id `%s'\n", argv[1]);
	} else
		if ((grp = getgrnam(argv[1])) == NULL)
			cherr("bad group name `%s'\n", argv[1]);

	group = grp->gr_gid;

	for (c = 2; c < argc; c++) {
		owner = 0;
		if (stat(argv[c], &sb) >= 0)
			owner = sb.st_uid;
		if (chown(argv[c], owner, group) < 0) {
			perror(argv[c]);
			status = 2;
		}
	}
	exit (status);
}

usage()
{
	fprintf(stderr, "Usage: chgrp group file ...\n");
	exit(1);
}

/* VARARGS */
cherr(x)
{
	fprintf(stderr, "chgrp: %r", &x);
	exit(2);
}
