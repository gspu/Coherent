/*
 * perm.c
 * 5/1/90
 * Usage: perm file
 * Print a line describing the arg file to stdout.
 * Called by mkperm.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

void fatal();

main(argc, argv) int argc; char *argv[];
{
	struct stat s;
	struct passwd *pp;
	struct group *gp;

	if (argc != 2)
		fatal("one arg required");
	if (argv[1][0] == '.' && argv[1][1] == '/')
		argv[1] = &argv[1][2];
	if (stat(argv[1], &s) == -1)
		fatal("cannot stat \"%s\"", argv[1]);
	if ((pp = getpwuid(s.st_uid)) == NULL)
		fatal("uid botch file=%s uid=%d", argv[1], s.st_uid);
	if ((gp = getgrgid(s.st_gid)) == NULL)
		fatal("gid botch file=%s gid=%d", argv[1], s.st_gid);
	printf("%-24s\t%07o\t%s\t%s\t%d\n",
		argv[1],
		s.st_mode,
		pp->pw_name,
		gp->gr_name,
		s.st_nlink);
	exit(0);
}

void
fatal(s) char *s;
{
	fprintf(stderr, "perm: %r\n", &s);
	exit(1);
}

/* end of perm.c */
