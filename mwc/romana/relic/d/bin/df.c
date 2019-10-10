/*
 * df.c
 * 7/28/93
 * Usage: df [-fitv] [directory ...] [filesystem ...]
 *    or: df [-ait] [directory ...] [filesystem ...]     (if DF_OLD is set)
 * Print information regarding the
 * remaining space available on a file system.
 * This command also considers a directory
 * to represent the filesystem.
 *
 * 4-24-92 Fixed minor bug for looking /etc/mtab table. Vlad
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/filsys.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <mnttab.h>
#include <canon.h>
#if 1
#include <mtab.h>
#endif

#define	NMOUNT	64			/* Maximum mounted file systems */

/* New format options */
int	fflag;				/* supress information on i-nodes */
int	vflag;				/* Print % blocks used */
int	tflag;				/* Print total device size */
int	iflag;				/* Print information on i-nodes */

/* Old format options  - also -t and -i are valid */
int	aflag;				/* Print for each mounted fs */
int	oflag;				/* switch to old style */

char	buf[BSIZE];			/* Basic file system reading buffer */
struct	mnttab	m_tab[NMOUNT];
struct	mnttab	*emtabp;
int	noarg;				/* Was df called without any options */

char	*devname();

main(argc, argv)
int argc;
char *argv[];
{
	register char *ap;
	register int i;
	register int estat = 0;
	char *s;

	if ((s = getenv("DF_OLD")) != NULL && strcmp(s, "0") != 0)
		oflag++;

	while (argc > 1 && *argv[1] == '-') {
		for (ap = &argv[1][1]; *ap != '\0'; ap++)
			switch (*ap) {
			case 'a':
				if (oflag)
					aflag++;
				else
					usage("illegal option -a");
				break;

			case 'f':
				if (!oflag)
					fflag++;
				else
					usage("illegal option -f");
				break;

			case 'i':
				iflag++;
				break;

			case 't':
				tflag++;
				break;

			case 'v':
				vflag++;
				break;

			default:
				usage("illegal option");
			}
		argc--;
		argv++;
	}

	if (!oflag)
	{
		if (vflag && iflag)
			usage("Cannot use -v with -i");
		else if (vflag)
                	printf("Mount Dir      Filesystem                blocks    "
						 " used      free     %%used\n");
		else if (iflag)
			printf("Mount Dir      Filesystem                 iused     i"
	 					   "free    itotal   %%iused\n");
	}
	minit();
	sync();
	if (argc < 2) {
		if (!oflag || aflag)
			estat = dfmtab();
		else {
			noarg = 1;
			estat = df(".", NULL);
                }
	} else {
		for (i = 1; i < argc; i++)
			estat |= df(argv[i], NULL);
		if (aflag)
			estat |= dfmtab();
	}
	exit(estat);
}

/*
 * Read the mount table, looking for file systems
 * to run df on.
 */
dfmtab()
{
	register struct mnttab *mp;
	int estat;
	int name[MNAMSIZ + 10];

	estat = 0;
	for (mp = m_tab; mp < emtabp; mp++) {
		if (mp->mt_dev[0]=='\0' || mp->mt_filsys[0]=='\0')
			continue;
		sprintf(name, "/dev/%s", mp->mt_filsys);
		estat |= df(name, mp->mt_dev);
	}
	return (estat);
}

/*
 * Look at the file system
 * and find out free space.
 */
df(fs, name)
register char *fs, *name;
{
	register struct filsys *sbp;
	struct stat sb;
	int fd;
	long	btotal;
	long	bfree;
	long	itotal;
	long	ifree;
	long 	percent;
	char	*nfs = fs;


	if (stat(fs, &sb) < 0) {
		cmsg("cannot stat '%s'", fs);
		return (1);
	}
	switch (sb.st_mode & S_IFMT) {
	case S_IFDIR:
	{
		if ((nfs = devname(sb.st_dev)) == NULL) {
			cmsg("no file system device found for directory '%s'",
			    fs);
			return (1);
		}
		if (noarg)
			fs = nfs;
		break;
	}

	case S_IFBLK:
	case S_IFCHR:
		break;

	default:
		cmsg("unknown file type '%s'", fs);
		return (1);
	}
	if ((fd = open(nfs, 0)) < 0) {
		cmsg("cannot open '%s'", nfs);
		return (1);
	}
	lseek(fd, (long)BSIZE * SUPERI, 0);
	if (read(fd, buf, BSIZE) != BSIZE) {
		cmsg("read error on '%s'", nfs);
		close(fd);
		return (1);
	}
	close(fd);
	sbp = &buf[0];
	canf(sbp);
	if (tstf(sbp) == 0) {
		cmsg("badly formed super block on '%s'", nfs);
		return (1);
	}

	bfree = sbp->s_tfree;
	btotal = sbp->s_fsize - sbp->s_isize;
	itotal = (sbp->s_isize-INODEI)*INOPB;
	ifree = sbp->s_tinode;


        if (!oflag)
	{
		printf("%-12s   (%-20s): ", name == NULL ? fs : name, nfs);
		if (vflag)
		{
                	percent = ((btotal - bfree) * 1000L) / btotal;
			printf(" %7lu   %7lu   %7lu    %2ld.%1ld%%", btotal,
			      btotal - bfree, bfree, percent/10L, percent%10L);
        	}
		else if (iflag)
		{
                	percent = ((itotal - ifree) * 1000L) / itotal;
			printf(" %7lu   %7lu   %7lu    %2ld.%1ld%%",
  		         itotal-ifree, ifree, itotal, percent/10L, percent%10L);
		}
		else
                {
			printf(" %7lu blocks", bfree);
			if (!fflag)
				printf("   %7lu inodes", ifree);
			if (tflag)
			{
				printf("\n\t\t\t\tTotal:\t%7lu blocks", btotal);
				if (!fflag)
					printf("   %7lu inodes", itotal);
			}
		}
	}
	else
	{
		printf("%-11s", nfs);
		report(bfree, btotal);
		if (iflag)
		{
			printf(", ");
			report(ifree, itotal);
		}
		if (tflag)
			printf(", %lu", sbp->s_fsize);
	}	
	printf("\n");
	return (0);
}

report(free, total)
long free;
long total;
{
	long percent;

	printf(" %6lu", free);
	percent = (free * 1000L) / total;
	printf("/%6lu = %2ld.%1ld%%", total , percent/10L, percent%10L);
}

/*
 * Initialise mount table
 * in memory.
 */
minit()
{
	register int fd;
	register int n;

	emtabp = &m_tab[0];
	if ((fd = open("/etc/mnttab", 0)) >= 0) {
		if ((n = read(fd, (char *)&m_tab[0], sizeof m_tab)) > 0)
			emtabp = (char *)(&m_tab[0]) + n;
		close(fd);
		return;
	}
#if 1
	if ((fd = open("/etc/mtab", 0)) >= 0) {
		while (read(fd, (char *)emtabp, sizeof(struct mtab))
						== sizeof(struct mtab))
			emtabp++;
		close(fd);
	}
#endif
}

/*
 * Return the name of the block special file
 * (in directory '/dev') which corresponds to
 * the device given in argument.
 */
char *
devname(dev)
dev_t dev;
{
	register struct direct *dp;
	register int n;
	int fd;
	static char name[25];
	struct stat sb;

	if ((fd = open("/dev", 0)) < 0)
		return (NULL);
	while ((n = read(fd, buf, sizeof buf)) > 0) {
		for (dp = &buf[0]; dp < &buf[n]; dp++) {
			canino(dp->d_ino);
			if (dp->d_ino == 0)
				continue;
			strcpy(name, "/dev/");
			strncat(name, dp->d_name, DIRSIZ);
			if (stat(name, &sb) < 0)
				continue;
			if ((sb.st_mode & S_IFMT) != S_IFBLK)
				continue;
			if (sb.st_rdev != dev)
				continue;
			close(fd);
			return (name);
		}
	}
	close(fd);
	return (NULL);
}

/*
 * Canonicalize and check superblock for consistency.
 */
canf(fp)
register struct filsys *fp;
{
	register daddr_t *dp;
	register ino_t *ip;

	canshort(fp->s_isize);
	candaddr(fp->s_fsize);
	canshort(fp->s_nfree);
	for (dp = &fp->s_free[0]; dp < &fp->s_free[NICFREE]; dp += 1)
		candaddr(*dp);
	canshort(fp->s_ninode);
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[NICINOD]; ip += 1)
		canino(*ip);
	candaddr(fp->s_tfree);
	canino(fp->s_tinode);
}

tstf(fp)
register struct filsys *fp;
{
	register daddr_t *dp;
	register ino_t *ip;
	register ino_t maxinode;

	maxinode = (fp->s_isize - INODEI) * INOPB + 1;
	if (fp->s_isize >= fp->s_fsize)
		return (0);
	if (fp->s_tfree < fp->s_nfree
	 || fp->s_tfree >= fp->s_fsize - fp->s_isize + 1)
		return (0);
	if (fp->s_tinode < fp->s_ninode
	 || fp->s_tinode >= maxinode-1)
		return (0);
	for (dp = &fp->s_free[0]; dp < &fp->s_free[fp->s_nfree]; dp += 1)
		if (*dp < fp->s_isize || *dp >= fp->s_fsize)
			return (0);
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[fp->s_ninode]; ip += 1)
		if (*ip < 1 || *ip > maxinode)
			return (0);
	return (1);
}


/*
 * Errors and warnings.
 */
/* VARARGS */
cerr(arg)
char *arg;
{
	fprintf(stderr, "df: %r\n", &arg);
	exit(1);
}

/* VARARGS */
cmsg(arg)
char *arg;
{
	fprintf(stderr, "df: %r\n", &arg);
}

usage(str)
char *str;
{

       	fprintf(stderr, "df: %s\n", str);
	fprintf(stderr, "Usage: df [-%s] [directory ...] [filesystem ...]\n",
		oflag ? "ait" : "fitv");
	exit(1);
}

/* end of df.c */
