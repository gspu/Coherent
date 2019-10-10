/*
 * Cpdir. Copy hierarchies in a file system, preserving structure.
 *	  Define SLOW for 'block at a time copying' - not recommended tho.
 */
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <ctype.h>
#include <access.h>
#include <canon.h>

#define	VERSION		"1.1"
#define	CPBUFSIZ	(50*BUFSIZ)			/* copy buffer */
#define	SDSIZ		(sizeof(struct direct))
#define	MAXINT		32767
#define	SOURCE		0
#define	TARGET		1
#define	DEV0		((dev_t)0)
#define	INODE0		((ino_t)0)
#define	ROOTUID		0
#define	HASHSIZE	37

#define	hash(ino)	((ino)%HASHSIZE)

#define	not		!
#define	and		&&
#define	or		||
#define	TRUE		(0==0)
#define	FALSE		(not TRUE)

typedef	char		bool;
typedef	unsigned char	uchar;
typedef	unsigned int	uint;
typedef	unsigned short	ushort;
typedef	unsigned long	ulong;

typedef union {
	struct	direct	dbuf;
	char		cbuf[SDSIZ + 1];
} DIRBUF;
#define	db_ino		dbuf.d_ino
#define	db_name		dbuf.d_name

typedef struct link {
	ino_t		l_ino;
	dev_t		l_dev;
	dev_t		l_tdev;
	struct link	*l_next;
	uint		l_nlink;
	char		l_name[];
} LINK;

typedef struct str {
	struct str	*s_next;
	char		s_str[];
} STR;



/*
 * Global variables.
 */
extern	int	errno;
dev_t		dir2_dev;
ino_t		dir2_ino;
dev_t		src_dev;
dev_t		tgt_dev;
struct stat	srcstat;
struct stat	tgtstat;
LINK		*srctab[HASHSIZE];
LINK		*tgttab[HASHSIZE];
STR		*strtab[HASHSIZE];

bool	aflag;
bool	dflag;
bool	eflag;
bool	sflag;
bool	tflag;
bool	uflag;
bool	vflag;
bool	wflag;
bool	root;
bool	splitmsg;
bool	errprefix;
bool	dir1slash;
bool	dir2slash;

int	rlimit = MAXINT;
int	rlevel = 1;
int	exitval;
int	numsuppress;
int	broken;
int	srclinks;
int	tgtlinks;
int	srcsize;
int	tgtsize;
int	dir1len;
int	dir2len;
int	interrupted;		/* For handling SIGINT and SIGQUIT */

short	uid;
short	gid;

char	*dir1;
char	*dir2;
char	*target;
char	*source;

/*
 * Error message and other strings.
 */
char	*dot =		".";
char	*dotdot =	"..";
char	*fmt1 =		"%s\n";
char	*fmt1a =	"%-72s\r";
char	*fmt2 =		"%s: %s\n";
char	*fmt3 =		"%s: %s%s\n";
char	*fmt4 =		"%s: %s%s%s\n";
char	*usage =	"usage: cpdir [options] dir1 dir2";
char	*devbound =	"crossing device boundary";
char	*writeerr =	"write error";
char	*readerr =	"read error";
char	*supressd =	"suppressed";
char	*linked =	"linked";
char	*notdir =	"not a directory";
char	*nomemory =	"out of memory";
char	*nopermit =	"permission denied";
char	*nomkdir =	"cannot make directory";
char	*nounlink =	"cannot unlink ";
char	*nolink =	"cannot link ";
char	*nocreate =	"cannot create";
char	*nofind =	"cannot find";
char	*noopen =	"cannot open";

/*
 * Functions returning non_int.
 */
long	lseek();
char	*tmalloc();
char	*realloc();
char	*rindex();
char	*concat();
char	*parent();
bool	suppress();
bool	isslash();
bool	dirchecks();
bool	linkattempt();
bool	tgtunlink();
LINK	*linklocate();
LINK	*linkinstall();

main(ac, av) int ac; char *av[];
{
	ac = 0;
	aarghh(av);
	init();
	cpdir();
	report();
	if (aflag)
		printf("%72s\r", "");
	return (exitval);
}

aarghh(av)
register char *av[];
{
	register char *cp;

	for (;;) {
		if ((cp = *++av) == NULL) {
			errprint(fmt1, usage);
			exit(1);
		}
		if (*cp++ != '-')
			break;
		while (*cp)
			switch (*cp++) {
			case 'a':
				aflag = TRUE;
				break;
			case 'd':
				dflag = TRUE;
				break;
			case 'e':
				eflag = TRUE;
				break;
			case 'r':
				if (not isdigit(*cp)) {
					rlimit = 1;
					break;
				}
				*av = cp;
				rlimit = new_atoi(av);
				cp = *av;
				break;
			case 's':
				sflag = TRUE;
				strinstall(cp);
				++numsuppress;
				goto NEXTARG;
				break;
			case 't':
				tflag = wflag = eflag = TRUE;
				break;
			case 'u':
				uflag = TRUE;
				break;
			case 'v':
				vflag = wflag = TRUE;
				break;
			case 'V':
				fprintf(stderr, "cpdir: V%s\n", VERSION);
				break;
			default:
				errprint(fmt1, usage);
				exit(1);
			}
		NEXTARG:;
	}

	if ((dir1=*av++) == NULL  or  (dir2=*av++) == NULL  or  *av != NULL) {
		errprint(fmt1, usage);
		exit(1);
	}
	return;
}

/*
 * Actions: Initialize certain flags and the name buffers source and target.
 * Verify dir1 and dir2 permissions. Dir2 is made if necessary.
 * Side Effects: On return srcstat contains the status of dir1. If dir2
 * exists tgtstat contains its status. In the case that tflag is set and dir2
 * does not exist, tgt_dev and tgtstat.st_dev are the device of target's
 * parent, the device that target would be created on except for tflag.
 */
init()
{
	struct stat outstat;
	struct stat errstat;

	uid = getuid();
	gid = getgid();
	root = (uid == ROOTUID);
	if (root)
		umask(0);

	errprefix = not isatty(fileno(stdin));
	fstat(fileno(stdout), &outstat);
	fstat(fileno(stderr), &errstat);
	splitmsg = (outstat.st_ino != errstat.st_ino)
		or (outstat.st_dev != errstat.st_dev);

	dir1len = strlen(dir1);
	srcsize = (dir1len + 1);
	source = tmalloc(srcsize);
	strcpy(source, dir1);
	dir1slash = isslash(dir1);

	dir2len = strlen(dir2);
	tgtsize = dir2len + 1;
	target = tmalloc(tgtsize);
	strcpy(target, dir2);
	dir2slash = isslash(dir2);

	/*
	 * We must check boundary conditions before we call dirchecks().
	 * These are: source exists and is a directory, and if target does not
	 * exist that we have write and search permission on its parent and
	 * tgt_dev contains the device number of the parent.
	 */
	if (stat(dir1, &srcstat) < 0) {
		errprint(fmt2, dir1, nofind);
		exit(1);
	}
	if ((srcstat.st_mode & S_IFMT) != S_IFDIR) {
		errprint(fmt2, dir1, notdir);
		exit(1);
	}
	if (access(dir2, 0) < 0) {
		register char *cp;
		cp = parent(dir2);
		if (stat(cp, &tgtstat) < 0) {
			errprint(fmt2, dir2, "cannot find parent");
			exit(1);
		}
		if (access(cp, AWRITE|ASRCH) < 0) {
			errprint(fmt2, dir2, nomkdir);
			exit(1);
		}
		tgt_dev = tgtstat.st_dev;
	}
	if (not dirchecks())
		exit(1);

	src_dev = srcstat.st_dev;
	dir2_dev = tgt_dev = tgtstat.st_dev;
	if (access(target, 0) == 0)
		dir2_ino = tgtstat.st_ino;
	return;
}


/*
 * Assume: Source is a directory we can read and search. Srcstat contains its
 * status. Target is a directory we can write and search. Tgtstat contains its
 * status. Src_dev and tgt_dev are the devices of the parents of source and
 * target. If tflag is on target may not exist. In this case tgtstat.st_dev
 * contains the device where target would have been created if tflag were off.
 */
cpdir()
{
	register int	n;
	register int	fd;
	register DIRBUF	*dbp;
	DIRBUF	dirbuf;
	long	address;
	dev_t	sdev;
	dev_t	tdev;
	struct	stat	locsrcstat;
	struct	stat	loctgtstat;

	/*
	 * Check for circular copy.
	 */
	if (srcstat.st_ino == dir2_ino
	and srcstat.st_dev == dir2_dev) {
		if (wflag)
			printf(fmt2, source,
			"not copied to avoid circular copy");
		return;
	}

	/*
	 * Open source, and prepare dirbuf.
	 */
	if ((fd = open(source, 0)) < 0) {
		errprint(fmt2, source, noopen);
		return;
	}
	dbp = &dirbuf;
	dbp->cbuf[SDSIZ] = '\0';

	/*
	 * Save srcstat, src_dev, tgtstat, tgt_dev to restore before exit.
	 */
	locsrcstat = srcstat;
	loctgtstat = tgtstat;
	tdev = tgt_dev;
	sdev = src_dev;

	/*
	 * Detect the crossing of device boundaries.
	 */
	if (tgt_dev != tgtstat.st_dev) {
		if (wflag)
			printf(fmt2, target, devbound);
		tgt_dev = tgtstat.st_dev;
	}
	if (src_dev != srcstat.st_dev) {
		if (wflag)
			printf(fmt2, source, devbound);
		src_dev = srcstat.st_dev;
	}

	/*
	 * Loop through the directory source.
	 */
	while ((n = read(fd, dbp->cbuf, SDSIZ)) > 0) {
		if (n != SDSIZ) {
			errprint(fmt2, source, readerr);
			close(fd);
			goto OUT;
		}

		canino(dbp->db_ino);
		if (dbp->db_ino == INODE0
		or  strcmp(dbp->db_name, dot) == 0
		or  strcmp(dbp->db_name, dotdot) == 0)
			continue;

		grow(dbp->db_name);
		if (sflag  and  suppress()) {
			if (vflag)
				printf(fmt2, source, supressd);
			shrink();
			continue;
		}
		if (stat(source, &srcstat) < 0) {
			errprint(fmt2, source, nofind);
			shrink();
			continue;
		}

		switch (srcstat.st_mode & S_IFMT) {
		default:
			errprint(fmt2, source, "unknown file type");
			shrink();
			continue;
		case S_IFCHR:
		case S_IFBLK:
			cpnode();
			shrink();
			continue;
		case S_IFPIP:
			srcstat.st_rdev = DEV0;
			cpnode();
			shrink();
			continue;
		case S_IFREG:
			cpfile();
			shrink();
			continue;
		case S_IFDIR:
			if (rlevel == rlimit) {
				if (vflag)
					printf(fmt2, source, supressd);
				shrink();
				continue;
			}
			if (not dirchecks()) {
				shrink();
				continue;
			}

			address = lseek(fd, 0L, 1);
			close(fd);
			++rlevel;
			cpdir();
			--rlevel;
			shrink();
			if ((fd = open(source, 0)) < 0) {
				errprint(fmt2, source,
				"cannot reopen, copy incomplete");
				goto OUT;
			}
			lseek(fd, address, 0);
			continue;
		}
	}
	close(fd);

	OUT:
	vprintf();
	srcstat = locsrcstat;
	tgtstat = loctgtstat;
	tgt_dev = tdev;
	src_dev = sdev;
	adjust();
	return;
}

/*
 * Assume source names the file to be copied, srcstat contains its status,
 * and target is the file to copy to. Target may or may not exist.
 */
cpfile()
{
#ifndef	SLOW
 	register char *ip;
#endif
	register int n;
	register int i;
	register int fd1;
	register int fd2;
	register fsize_t size;
#ifndef	SLOW
	register char *wp;				/* write pointer */
	register int wflag;				/* write flag */
	static char buf[CPBUFSIZ];			/* copy buffer */
#else
	static char buf[BUFSIZ];
#endif
	int intflag, hupflag;

	if (access(source, AREAD) < 0) {
		errprint(fmt2, source, noopen);
		return;
	}

	if (uflag  and  stat(target, &tgtstat) == 0)
		if (srcstat.st_mtime <= tgtstat.st_mtime) {
			if (vflag)
				printf(fmt2, target, "no update");
			return;
		}

	if (not tgtunlink())
		return;
	if (srcstat.st_nlink > 1)
		if (linkattempt())
			return;

	if (tflag) {
		vprintf();
		return;
	}

	intflag = catch(SIGINT);
	hupflag = catch(SIGHUP);
	if ((fd1 = open(source, 0)) < 0) {
		errprint(fmt2, source, noopen);
		return;
	}
	if ((fd2 = creat(target, 0)) < 0) {
		close(fd1);
		errprint(fmt2, target, nocreate);
		return;
	}

	size = srcstat.st_size;
#ifndef	SLOW
	while ((n = read(fd1, buf, sizeof(buf))) > 0) {
#else
	while ((n = read(fd1, buf, BUFSIZ)) > 0) {
#endif
		/*
		 * Check for blocks of zeroes (holes in a sparse file).
		 * However, a block of zeroes at the end of a file must be
		 * written so the file has correct length.
		 */
#ifndef	SLOW
		wp = ip = buf;
		size -= n;
		while ((n-BUFSIZ) > 0 || ((n-BUFSIZ) == 0 && size != 0)) {
			n -= BUFSIZ;
			wflag = FALSE;
			ip = wp;
			for (i = 0; i < BUFSIZ; ++i)
				if (*ip++ != '\0') {
					wflag = TRUE;
					break;
				}
			if (wflag) {
				if (write(fd2, wp, BUFSIZ) < BUFSIZ) {
					errprint(fmt2, target, writeerr);
					close(fd1);
					close(fd2);
					return (FALSE);
				}
			} else {
				lseek(fd2, (long)BUFSIZ, 1);
			}
			wp += BUFSIZ;
		}
		if (write(fd2, wp, n) < n) {
			errprint(fmt2, target, writeerr);
			close(fd1);
			close(fd2);
			return (FALSE);
		}
#else
		if ((size -= n) == (fsize_t)0)
			goto WRITE;
		for (i = 0; i < n; ++i)
			if (buf[i] != '\0')
				goto WRITE;
		lseek(fd2, (long)BUFSIZ, 1);
		continue;

		WRITE:
		if (write(fd2, buf, n) < n) {
			errprint(fmt2, target, writeerr);
			close(fd1);
			close(fd2);
			return (FALSE);
		}
#endif
	}
	close(fd1);
	close(fd2);
	adjust();
	if (intflag)
		signal(SIGINT, SIG_DFL);
	if (hupflag)
		signal(SIGHUP, SIG_DFL);
	if (interrupted)
		exit(1);

	if (n < 0)
		errprint(fmt2, source, readerr);
	else vprintf();
	return;
}

/*
 * Copy special nodes and named pipes.
 */
cpnode()
{
	if (not root) {
		if (vflag)
			printf(fmt2, source, "not the super-user");
		return;
	}
	if (not tgtunlink())
		return;
	if (srcstat.st_nlink > 1)
		if (linkattempt())
			return;
	if (not tflag) {
		if (mknod(target, srcstat.st_mode, srcstat.st_rdev) < 0) {
			errprint(fmt2, target, "cannot make node");
			return;
		}
		adjust();
	}
	if (vflag)
		printf(fmt2, target, "copied node");
	else if (aflag)
		printf(fmt1a, target);
	return;
}


/*
 * Grow each of target and source by appending '/' and cp.
 */
grow(cp)
register char *cp;
{
	register int a;
	register int b;

	a = strlen(cp) + 2;

	b = strlen(source);
	if (a+b > srcsize) {
		if ((source = realloc(source, a+b)) == NULL) {
			errprint(fmt1, nomemory);
			exit(1);
		}
		srcsize = a+b;
	}
	if (rlevel > 1  or  not dir1slash)
		source[b++] = '/';
	strcpy(source+b, cp);

	b = strlen(target);
	if (a+b > tgtsize) {
		if ((target = realloc(target, a+b)) == NULL) {
			errprint(fmt1, nomemory);
			exit(1);
		}
		tgtsize = a+b;
	}
	if (rlevel > 1  or  not dir2slash)
		target[b++] = '/';
	strcpy(target+b, cp);
}

/*
 * Shrink source and target down by the last pathname component.
 */
shrink()
{
	if (rlevel == 1  and  dir1slash)
		source[dir1len] = '\0';
	else
		*rindex(source, '/') = '\0';
	if (rlevel == 1  and  dir2slash)
		target[dir2len] = '\0';
	else
		*rindex(target, '/') = '\0';
	return;
}


/*
 * Returns a value equal to atoi(*cpp). Positive decimal integers only.
 * Leaves *cpp pointing to the character that terminated the digit string.
 */
new_atoi(cpp)
char **cpp;
{
	register int sum = 0;
	register int c;
	register char *cp;

	sum = 0;
	cp = *cpp;
	while (isdigit(c = *cp++)) {
		sum *= 10;
		sum -= '0';
		sum += c;
	}
	*cpp = cp-1;
	return (sum);
}

errprint(arg)
char *arg[];
{
	register char *format;

	format = (errprefix) ? "cpdir: %r" : "%r";
	fprintf(stderr, format, &arg);
	if (wflag  and  splitmsg)
		printf(format, &arg);
	if (not eflag)
		exit(1);
	exitval = 1;
	return;
}

/*
 * Interface to malloc to check for bad returns.
 */
char *
tmalloc(n)
int n;
{
	register char *cp;
	if ((cp = malloc(n)) == NULL) {
		errprint(fmt1, nomemory);
		exit(1);
	}
	return (cp);
}

/*
 * Assume: source is a dir, srcstat has its status, we have write and search
 * permission on target's parent directory, tgt_dev is the device of target's
 * parent.
 * Actions: Check permissions on source. Check permissions on target if it
 * exists. If non-extant and tflag is off, make it.
 * Side Effects: On return, if target exists, tgtstat contains its status. If
 * it does not exist, tgtstat.st_dev is the device it would have been created
 * on if tflag were off.
 * Return TRUE if everything Aok, FALSE otherwise.
 */
bool
dirchecks()
{
	register int n;
	static int status;

	if (access(source, AREAD | ASRCH) < 0) {
		errprint(fmt2, source, nopermit);
		return (FALSE);
	}

	if (stat(target, &tgtstat) == 0) {
		if ((tgtstat.st_mode & S_IFMT) != S_IFDIR) {
			errprint(fmt3, source, "target is ", notdir);
			return (FALSE);
		}
		if (access(target, AWRITE | ASRCH) < 0) {
			errprint(fmt2, target, nopermit);
			return (FALSE);
		}
		return (TRUE);
	}

	if (tflag) {
		tgtstat.st_dev = tgt_dev;
		return (TRUE);
	}

	if ((n = fork()) < 0) {
		errprint(fmt2, target, nomkdir);
		return (FALSE);
	}
	if (n == 0) {
		close(2);
		umask(077);
		execl("/bin/mkdir", "cpdir", target, NULL);
		exit(1);
	}
	while (wait(&status) != n)
		;
	if (status != 0) {
		errprint(fmt2, target, nomkdir);
		return (FALSE);
	}
	if (stat(target, &tgtstat) < 0) {
		errprint(fmt2, target, "made but cannot stat");
		return (FALSE);
	}
	return (TRUE);
}

strinstall(cp)
register char *cp;
{
	register STR *sp;
	register int n;

	n = strhash(cp);
	for (sp = strtab[n]; sp != NULL; sp = sp->s_next)
		if (strcmp(cp, sp->s_str) == 0)
			return;
	sp = (STR *) tmalloc(sizeof(STR) + strlen(cp) + 1);
	sp->s_next = strtab[n];
	strtab[n] = sp;
	strcpy(sp->s_str, cp);
	return;
}

bool
suppress()
{
	register char *cp;
	register STR *sp;
	register STR **spp;

	cp = source + dir1len;
	if (not dir1slash)
		++cp;
	spp = strtab + strhash(cp);
	if ((sp = *spp) == NULL)
		return (FALSE);
	while (sp != NULL) {
		if (strcmp(cp, sp->s_str) == 0) {
			*spp = sp->s_next;
			free(sp);
			if (--numsuppress == 0)
				sflag = FALSE;
			return (TRUE);
		}
		spp = &(sp->s_next);
		sp = *spp;
	}
	return (FALSE);
}

strhash(cp)
register uchar *cp;
{
	register uint sum = 0;
	for (sum = 0; *cp != '\0'; sum += *cp++)
		;
	return (sum % HASHSIZE);
}

LINK *
linklocate(flag)
int flag;
{
	register LINK *lp;
	register struct stat *stp;

	if (flag == SOURCE) {
		stp = &srcstat;
		lp = srctab[hash(stp->st_ino)];
	}
	else {
		stp = &tgtstat;
		lp = tgttab[hash(stp->st_ino)];
	}

	for ( ; lp != NULL; lp = lp->l_next) {
		if (lp->l_ino != stp->st_ino)
			continue;
		if (lp->l_dev != stp->st_dev)
			continue;
		--(lp->l_nlink);
		return (lp);
	}
	return (NULL);
}

LINK *
linkinstall(flag)
int flag;
{
	register LINK **lpp;
	register LINK *lp;
	register struct stat *stp;

	if (flag == SOURCE) {
		++srclinks;
		stp = &srcstat;
		lpp = hash(stp->st_ino) + srctab;
	}
	else {
		++tgtlinks;
		stp = &tgtstat;
		lpp = hash(stp->st_ino) + tgttab;
	}

	lp = (LINK *) tmalloc(sizeof(LINK) + strlen(target) - dir2len + 1);
	if (!dir2slash)
		strcpy(lp->l_name, target + dir2len + 1);
	else
		strcpy(lp->l_name, target + dir2len);
	lp->l_dev = stp->st_dev;
	lp->l_ino = stp->st_ino;
	lp->l_tdev = tgt_dev;
	lp->l_nlink = stp->st_nlink - 1;
	lp->l_next = *lpp;
	*lpp = lp;
	return (lp);
}

linkpurge(flag, lp)
int flag;
register LINK *lp;
{
	register LINK *lp1;
	register LINK **lpp;

	if (flag == SOURCE) {
		--srclinks;
		lpp = hash(srcstat.st_ino) + srctab;
	}
	else {
		--tgtlinks;
		lpp = hash(tgtstat.st_ino) + tgttab;
	}

	for (lp1 = *lpp; lp1 != NULL; *lpp = lp1, lp1 = lp1->l_next) {
		if (lp1 != lp)
			continue;
		*lpp = lp1->l_next;
		free((char *)lp1);
		return;
	}
	return;
}

bool
linkattempt()
{
	register LINK *lp;
	register bool ret;
	register char *cp;

	if ((lp = linklocate(SOURCE)) == NULL) {
		linkinstall(SOURCE);
		return (FALSE);
	}
	cp = concat(dir2, lp->l_name);

	if (tflag) {
		if (tgt_dev != lp->l_tdev) {
			errprint(fmt4, target, nolink, " to ", cp);
			++broken;
			ret = FALSE;
		}
		else {
			vprintf();
			ret = TRUE;
		}
	}
	else if (link(cp, target) == 0) {
		vprintf();
		ret = TRUE;
	}
	else {
		errprint(fmt4, target, nolink, " to ", cp);
		++broken;
		ret = FALSE;
	}

	if (lp->l_nlink == 0)
		linkpurge(SOURCE, lp);
	return (ret);
}

bool
tgtunlink()
{
	register LINK *lp;

	if (stat(target, &tgtstat) < 0)
		return (TRUE);
	if ((tgtstat.st_mode & S_IFMT) == S_IFDIR) {
		errprint(fmt3, target, nounlink, "directory");
		return (FALSE);
	}
	else if ((tgtstat.st_mode & S_IFMT) != (srcstat.st_mode & S_IFMT)) {
		errprint(fmt3, source, "file type mismatch with ", target);
		return (FALSE);
	}

	if (not tflag)
		if (unlink(target) < 0) {
			errprint(fmt2, target, nounlink);
			return (FALSE);
		}
	if (tflag)
		printf(fmt2, target, "unlinked");

	if ((lp = linklocate(TARGET)) != NULL) {
		if (lp->l_nlink == 0)
			linkpurge(TARGET, lp);
	}
	else if (tgtstat.st_nlink > 1)
		linkinstall(TARGET);
	return (TRUE);
}

report()
{
	register LINK *lp;
	register LINK **lpp;
	static char *fmtlinks = "%s external links into hierarchy %s:\n";

	if (not wflag)
		return;
	if (broken)
		printf("%d internal link%s broken\n", broken,
			(broken == 1) ? "" : "s");
	if (srclinks) {
		printf(fmtlinks, "missed", dir1);
		for (lpp = srctab; lpp < srctab + HASHSIZE; ++lpp)
			for (lp = *lpp; lp != NULL; lp = lp->l_next)
				printf("\t%d\t%s (inode %d)\n", lp->l_nlink,
					concat(dir1, lp->l_name), lp->l_ino);
	}
	if (tgtlinks) {
		printf(fmtlinks, "broken", target);
		for (lpp = tgttab; lpp < tgttab + HASHSIZE; ++lpp)
			for (lp = *lpp; lp != NULL; lp = lp->l_next)
				printf("\t%d\t%s (inode %d)\n", lp->l_nlink,
					concat(dir2, lp->l_name), lp->l_ino);
	}
	return;
}

/*
 * Chown, chmod, and chdate target. Assume srcstat has status of source,
 * and tgtstat has status of target if it exists.
 */
adjust()
{
	time_t date[2];

	if (tflag)
		return;
	if (root)
		chown(target, srcstat.st_uid, srcstat.st_gid);
	chmod(target, srcstat.st_mode & (root ? 07777 : 06777));
	if (dflag) {
		time(&date[0]);
		date[1] = srcstat.st_mtime;
		utime(target, date);
	}
	return;
}

/*
 * Concatenate pieces of pathnames, a and b. If a is not "/" a '/' char
 * is placed between the names. Previous return is freed.
 */
char *
concat(a, b)
register char *a;
char *b;
{
	static char *ret;
	register int a1;
	register char *rp;

	if (ret != NULL)
		free(ret);
	a1 = strlen(a);
	rp = ret = tmalloc(a1 + strlen(b) + 2);
	strcpy(rp, a);
	rp += a1;
	if (not isslash(ret))
		*rp++ = '/';
	strcpy(rp, b);
	return (ret);
}

char *
parent(cp)
register char *cp;
{
	static char *ret;
	register char *cp0;

	if (ret != NULL)
		free(ret);
	if ((cp0 = rindex(cp, '/')) == NULL)
		return (dot);
	ret = tmalloc(cp0 - cp + 1);
	strncpy(ret, cp, cp0 - cp);
	return (ret);
}

bool
isslash(cp)
register char *cp;
{
	register int c;
	while ((c = *cp++) != '\0')
		if (c != '/')
			return (FALSE);
	return (TRUE);
}

/*
 * Routine exectuted when SIGINT or SIGHUP caught.
 * Set the interrupted flag.
 */
onintr()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	++interrupted;
}

/*
 * If sig's handler is currently SIG_DFL,
 * catch it with onintr and return 1.
 * If not, leave its handler unchanged and return 0.
 */
int
catch(sig) int sig;
{
	int (*old)();

	if ((old = signal(sig, SIG_IGN)) == SIG_DFL) {
		signal(sig, onintr);
		return 1;
	}
	signal(sig, old);
	return 0;
}

vprintf()
{
	if (vflag)
		printf(fmt1, target);
	else if (aflag) {
		printf(fmt1a, target);
		fflush(stdout);
	}
}
