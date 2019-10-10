/*
 * setperm.c
 * 4/12/90
 * Usage: setperm [ -cv ] [ file ... ]
 * Set file ownership and permissions based upon /usr/adm/distrib/permlist.
 * Options:
 *	-c	Check only, do not change anything
 *	-v	Verbose
 */

#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

#define	VERSION	"1.3"
#define	INFILE	"/usr/adm/distrib/permlist"
#define	NSIZE	512
#define	SEPS	" \t\n"
#define	USAGE	"Usage: setperm [ -cv ] [ file ... ]\n"
extern	char *	strtok();

int	atoo();
void	fatal();
void	nonfatal();
void	process();

/* Globals. */
int		cflag;				/* check permissions only */
unsigned char	buf[NSIZE];			/* buffer */
int		found;				/* files found */
FILE *		ifp;				/* input FILE */
int		status;				/* exit status */
int		vflag;				/* verbose */

main(argc, argv) int argc; char *argv[];
{
	register int i;
	register char *name, *s;

	if (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; ++s) {
			switch(*s) {
			case 'c':	++cflag;	break;
			case 'v':	++vflag;	break;
			case 'V':	fprintf(stderr, "setperm: V%s\n", VERSION);
					break;
			default:	fprintf(stderr, USAGE);
					exit(1);
			}
		}
		--argc;
		++argv;
	}
	if ((ifp = fopen(INFILE, "r")) == NULL)
		fatal("cannot open \"%s\"", INFILE);
	while (fgets(buf, sizeof buf, ifp)) {
		if ((name = strtok(buf, SEPS)) == NULL)
			fatal("strtok failed, line \"%s\"", buf);
		if (argc == 1)
			process(name);
		else for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], name) != 0)
				continue;
			process(name);
			*argv[i] = '\0';
			if (++found == argc - 1)
				exit(0);
			break;
		}
	}
	if (argc > 1 && found != argc - 1) {
		for (i = 1; i < argc; i++)
			if (argv[i][0] != '\0')
				nonfatal("file \"%s\" not found in \"%s\"",
					argv[i], INFILE);
	}
	exit(status);
}

/*
 * Convert octal ASCII to int.
 */
int
atoo(cp) register char *cp;
{
	register val;
	register c;

	while ((c = *cp) == ' ' || c == '\t')
		cp++;
	for (val = 0; ;	c = *cp++) {
		if (c < '0' || c >= '8')
			break;
		val = val*8 + c - '0';
	}
	return val;
}

void
fatal(s) char *s;
{
	fprintf(stderr, "setperm: %r\n", &s);
	exit(1);
}

void
nonfatal(s) char *s;
{
	fprintf(stderr, "setperm: %r\n", &s);
	status = 1;
}

void
process(name) register char *name;
{
	char *mode, *uid, *gid, *links;
	struct passwd *pp;
	struct group *gp;
	struct stat s;

	if (stat(name, &s) == -1)
		fatal("cannot stat \"%s\"", name);
	else if (vflag)
		printf("%s:\n", name);
	mode = strtok(NULL, SEPS);
	uid = strtok(NULL, SEPS);
	gid = strtok(NULL, SEPS);
	links = strtok(NULL, SEPS);
	if (mode == NULL || uid == NULL || gid == NULL || links == NULL)
		fatal("strtok failed, line \"%s\"", buf);
	if ((pp = getpwnam(uid)) == NULL)
		fatal("no such uid: %s", uid);
	if ((gp = getgrnam(gid)) == NULL)
		fatal("no such gid: %s", gid);
	if (s.st_uid != pp->pw_uid || s.st_gid != gp->gr_gid) {
		if (cflag)
			nonfatal("%s: uid/gid (%d, %d) should be (%d, %d)",
				name, s.st_uid, s.st_gid, pp->pw_uid, gp->gr_gid);
		else if (chown(name, pp->pw_uid, gp->gr_gid) == -1)
			fatal("chown \"%s\" failed", name);
	}
	if (s.st_mode != atoo(mode)) {
		if (cflag)
			nonfatal("%s: mode %o should be %o",
				name, s.st_mode, atoo(mode));
		else if (chmod(name, atoo(mode)) == -1)
			fatal("chmod \"%s\" failed", name);
	}
	if (s.st_nlink != atoi(links))
		nonfatal("%s: link count %d should be %d",
			name, s.st_nlink, atoi(links));
}

/* end of setperm.c */
