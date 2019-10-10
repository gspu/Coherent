/*
 * Print a disc usage summary for
 * some directories.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/ino.h>
#include <errno.h>

#define	DIOSIZ	(BUFSIZ/8)	/* Size of directory i/o, until stacks grow */
#define	NLINK	500		/* Size of link table */
#define	NFNAME	1000		/* Size of filename */
#define	NRECUR	12		/* Depth of recursion < NFILE-3-2 */

int	aflag;			/* do files as well */
int	sflag;			/* only give total */
int	depth;			/* recursion depth */

struct	linktab	{
	ino_t	l_ino;
	dev_t	l_dev;
}	linktab[NLINK];
struct	linktab	*ltp;

char	fname[NFNAME];

char	toodeep[] = "directory structure too deep to traverse";

struct	stat	sb;

/*
 * indirect block overhead based upon size of file
 */
long	ranges[] = {
	ND,				/* direct blocks */
	ND + NBN,			/* single indirect */
	ND + NBN*NBN,			/* double indirect */
	ND + NBN*NBN*NBN		/* tripple indirect */
};
	
long	duentry();
long	dufork();
long	dusize();
char	*buildname();
void	duerr();
void	dumsg();
void	usage();
void	main();

void
main(argc, argv)
char *argv[];
{
	register int i;
	register char *ap;
	int estat;

	while (argc>1 && *argv[1]=='-') {
		for (ap = &argv[1][1]; *ap != '\0'; ap++)
			switch (*ap) {
			case 'a':
				aflag = 1;
				break;

			case 's':
				sflag = 1;
				break;

			default:
				usage();
			}
		argc--;
		argv++;
	}
	if (argc < 2)
		estat = du(".");
	else {
		estat = 0;
		for (i=1; i<argc; i++)
			estat |= du(argv[i]);
	}
	exit (estat);
}

/*
 * Print out disc usage summary
 */
du(name)
char *name;
{
	register char *ep, *cp;
	fsize_t size;
	int nondir;

	ltp = linktab;
	cp = name;
	ep = fname;
	while (*cp)
		*ep++ = *cp++;
	*ep = '\0';
	if (stat(name, &sb) < 0)
		duerr("%s: nonexistent", name);
	nondir = (sb.st_mode&S_IFMT) != S_IFDIR;
	size = duentry(ep, &sb);
	if (sflag || nondir)
		printf("%ld\t%s\n", size, fname);
	return (0);
}

/*
 * Do a du on a single entry
 * The pointer is the end pointer
 * into the fname buffer.
 */
long
duentry(ep, sbp)
char *ep;
struct stat *sbp;
{
	extern int errno;
	fsize_t total, fsize;
	char iobuf[DIOSIZ];
	register char *np;
	register struct direct *dp;
	register int nb;
	int fd;
	int dirf;

	switch (sbp->st_mode & S_IFMT) {
	case S_IFREG:
		return (dusize(sbp));

	case S_IFDIR:
		total = dusize(sbp);
		if (++depth >= NRECUR) {
			depth = 0;
			return (dufork(ep, sbp));
		}
		if ((fd = open(fname, 0)) < 0) {
			dumsg("cannot open `%s'", fname);
			return (0);
		}
		while ((nb = read(fd, iobuf, DIOSIZ)) > 0)
			for (dp = iobuf; dp < &iobuf[nb]; dp++) {
				np = dp->d_name;
				if (*np++=='.'
				  && (*np=='\0' || (*np++=='.' && *np=='\0')))
					continue;
				if (dp->d_ino == 0)
					continue;
				if ((np = buildname(dp, ep)) == NULL)
					continue;
				if (stat(fname, &sb) < 0) {
					dumsg("stat failed on `%s'", fname);
					continue;
				}
				dirf = (sb.st_mode&S_IFMT)==S_IFDIR;
				fsize = duentry(np, sbp);
				if (aflag && !sflag && !dirf)
					prsize(fsize);
				total += fsize;
			}
		if (nb < 0)
			dumsg("%s: directory read error", fname);
		close(fd);
		*ep = '\0';
		if (!sflag)
			prsize(total);
		--depth;
		return (total);

	default:
		return (0);
	}
}

/*
 * Fork to do a du on recursive directory
 * structure that is too deep to fit into
 * user's open files.
 */
long
dufork(ep, sbp)
char *ep;
struct stat *sbp;
{
	register int i;
	register int pid;
	int pfd[2] = {0, 0};
	int status;
	long sz = 0;

	fflush(stdout);
	if (pipe(pfd)<0 || (pid = fork())<0) {
		if (pfd[0]) {
			close(pfd[0]);
			close(pfd[1]);
		}
		dumsg(toodeep);
		return (0);
	}
	if (pid) {
		close(pfd[1]);
		while (wait(&status) >= 0)
			;
		if (status || read(pfd[0], &sz, sizeof(long)) != sizeof(long))
			sz = 0;
		close(pfd[0]);
		return (sz);
	}
	for (i=3; i<_NFILE; i++)
		if (i != pfd[1])
			close(i);
	sz = duentry(ep, sbp);
	write(pfd[1], &sz, sizeof(long));
	close(pfd[1]);
	exit(0);
}

/*
 * Do a du on a single file.
 * Now takes into account indirect blocks.
 */
long
dusize(sbp)
struct stat *sbp;
{
	register int i;
	register long blocks;

	if ((sbp->st_mode & S_IFMT) != S_IFDIR
	 && sbp->st_nlink > 1
	 && addlink(sbp->st_dev, sbp->st_ino))
		return (0);
	blocks = (sbp->st_size+BUFSIZ-1) / BUFSIZ;
	for (i = 0; i < sizeof(ranges)/sizeof(ranges[0]); ++i)
		if (blocks <= ranges[i])
			break;
	return (blocks + i);
}

/*
 * Print out a size line.
 */
prsize(blocks)
fsize_t blocks;
{
	printf("%ld\t%s\n", blocks, fname);
}

/*
 * Build up the next entry
 * in the name.
 */
char *
buildname(dp, ep)
struct direct *dp;
register char *ep;
{
	register char *cp = dp->d_name;
	register unsigned n = DIRSIZ;

	if (ep+DIRSIZ+2 >= &fname[NFNAME]) {
		dumsg(toodeep);
		return (NULL);
	}
	if (ep[-1] != '/')
		*ep++ = '/';
	do {
		if (*cp == '\0')
			break;
		*ep++ = *cp++;
	} while (--n);
	*ep = '\0';
	return (ep);
}

/*
 * Add an entry to the table
 * of i-numbers with multiple links.
 * If there are too many multi-link files,
 * they will get counted twice.
 * Return 1 if already there.
 */
addlink(dev, ino)
register dev_t dev;
register ino_t ino;
{
	register struct linktab *lp;

	for (lp = linktab; lp<ltp; lp++)
		if (lp->l_ino==ino && lp->l_dev==dev)
			return (1);
	if (lp-linktab >= NLINK)
		return (0);
	lp->l_ino = ino;
	lp->l_dev = dev;
	ltp++;
	return (0);
}

void
usage()
{
	fprintf(stderr, "Usage: du [-s] [name ...]\n");
	exit(1);
}

/* VARARGS */
void
duerr(x)
{
	fprintf(stderr, "du: %r\n", &x);
	exit (1);
}

/* VARARGS */
void
dumsg(x)
{
	fprintf(stderr, "du: %r\n", &x);
}
