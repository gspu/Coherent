/*
 * Produce various information about
 * filesystem space usage and owners.
 * The options of this command seem a
 * little too disjoint in their functionality.
 */

#include <stdio.h>
#include <sys/ino.h>
#include <sys/filsys.h>
#include <canon.h>
#include <pwd.h>
#include <ctype.h>

#define	NHASH	64		/* Hash names table (power of 2) */
#define	NIREAD	(20*BUFSIZ)	/* Inode bytes per read */
#define	NCUM	2047		/* Number of different size entries */
#define	BADSIZE	(-1)		/* not valid - for hash synonym checking */
#define	BADUID	(-1)		/* not valid - for hash synonyms */

/*
 * Hash table of names
 */
typedef	struct	NAME {
	struct	NAME	*n_next;
	unsigned short	n_uid;
	char	n_name[];
}	NAME;

/*
 * Entry of hash table of
 * -f and ordinary options.
 */
typedef	struct	FENTRY	{
	unsigned short	f_uid;
	long	f_nfiles;
	long	f_nblocks;
}	FENTRY;

/*
 * Entry of hash table for
 * -c (cumulative sizes)
 */
typedef	struct	CENTRY	{
	unsigned long	c_nblocks;	/* Size of file in blocks */
	long	c_nfiles;		/* #files of that size */
}	CENTRY;

NAME	*names[NHASH];
FENTRY	*fentries;		/* File count and size entries */
FENTRY	*efp;			/* Pointer to end of `fentries' */
CENTRY	*centries;		/* Cumulative size entries */
CENTRY	*ecp;			/* Pointer to end of `centries' */
CENTRY	*lastcep;		/* Last for sorting */
char	ibuf[NIREAD];		/* I-node read buffer */
char	incopt[] = "incompatible options given";

int	cflag;			/* cumulative sizes list */
int	fflag;			/* #files as well as space */
int	nflag;			/* List of files and owners (via ncheck) */
int	tflag;			/* Print a total */
int	nuids;			/* Number of uids found in /etc/passwd */
ino_t	readino;		/* Last i-number read from input (`-n') */
char	*readfile;		/* Last filename read as above */
char	*line;			/* Line that is read in */

char	*username();
char	*alloc();
FENTRY	*getfentry();
CENTRY	*getcentry();
int	fcomp();
int	ccomp();

main(argc, argv)
int argc;
register char *argv[];
{
	register char *ap;
	register int es = 0;
	register int mflag;
	register int fd;

	while (argc>1 && *argv[1]=='-') {
		for (ap = &argv[1][1]; *ap!='\0'; ap++)
			switch (*ap) {
			case 'n':
				nflag++;
				if (cflag || fflag)
					cerr(incopt);
				break;

			case 'c':
				cflag++;
				if (nflag || fflag)
					cerr(incopt);
				break;

			case 'f':
				fflag++;
				if (nflag || cflag)
					cerr(incopt);
				break;

			case 't':
				tflag++;
				if (nflag)
					cerr(incopt);
				break;

			default:
				usage();
			}
		argv++;
		argc--;
	}
	argv++;
	if (argc < 2)
		usage();
	mflag = 0;
	if (argv[0]!=NULL && argv[1]!=NULL)
		mflag++;
	for ( ; *argv!=NULL; argv++) {
		if ((fd = open(*argv, 0)) < 0) {
			cwarn("cannot open `%s'", *argv);
			continue;
		}
		if (mflag)
			fprintf(stderr, "%s:\n", *argv);
		quotinit();
		es |= quot(fd, *argv);
		quotterm();
		close(fd);
	}
	exit(es);
}

/*
 * Initialise things for `quot' for
 * each filesystem pass.
 */
quotinit()
{
	if (!cflag && nuids==0) {
		readnames();
		nuids *= 2;
	}
	if (!nflag && !cflag) {
		register FENTRY *fep;

		fentries = (FENTRY *)alloc(nuids*sizeof (FENTRY));
		efp = &fentries[nuids];
		for (fep = fentries; fep < efp; fep++)
			fep->f_uid = BADUID;
	} else if (cflag) {
		register CENTRY *cep;

		centries = (CENTRY *)alloc(NCUM*sizeof(CENTRY));
		lastcep = centries;
		ecp = &centries[NCUM];
		for (cep = centries; cep < ecp; cep++)
			cep->c_nblocks = BADSIZE;
	} else {
		line = alloc(1000);
		readino = 0;
	}
}

/*
 * Routine run between filesystems
 * to clean up allocated storage.
 * Also may print out tables.
 */
quotterm()
{
	if (fentries != NULL) {
		register FENTRY *fep;
		long totblocks = 0;
		long totfiles = 0;

		qsort((char *)fentries, nuids, sizeof(FENTRY), fcomp);
		for (fep=fentries; fep < efp; fep++) {
			if (fep->f_uid == BADUID)
				break;
			if (tflag) {
				totblocks += fep->f_nblocks;
				totfiles += fep->f_nfiles;
			}
			if (fflag)
				printf("%8ld ", fep->f_nfiles);
			printf("%8ld    %s\n", fep->f_nblocks,
			    username(fep->f_uid));
		}
		if (tflag) {
			if (fflag)
				printf("%8ld ", totfiles);
			printf("%8ld      Total\n", totblocks);
		}
		free((char *)fentries);
		fentries = NULL;
	} else if (centries != NULL) {
		register CENTRY *cep;
		long cumblocks = 0;

		qsort((char *)centries, lastcep-centries, sizeof(CENTRY), ccomp);
		for (cep = centries; cep < lastcep; cep++) {
			if (cep->c_nblocks == BADSIZE)
				break;
			cumblocks += cep->c_nblocks*cep->c_nfiles;
			printf("%8ld %8ld %10ld\n", cep->c_nblocks,
			    cep->c_nfiles, cumblocks);
		}
		free((char *)centries);
		centries = NULL;
	}
}

/*
 * Called for each filesystem that is read.
 */
quot(fsfd, fs)
int fsfd;
char *fs;
{
	register ino_t maxino;
	register ino_t inum;

	{
		register struct filsys *sbp;

		lseek(fsfd, (fsize_t)(SUPERI*BSIZE), 0);
		if (read(fsfd, ibuf, BSIZE) != BSIZE)
			cerr("%s: bad filesystem format", fs);
		sbp = ibuf;
		canshort(sbp->s_isize);
		candaddr(sbp->s_fsize);
		if (sbp->s_isize > sbp->s_fsize) {
			cwarn("%s: ridiculous fsize/isize", fs);
			return (1);
		}
		maxino = (sbp->s_isize-INODEI) * INOPB;
	}
	lseek(fsfd, (fsize_t)(INODEI*BSIZE), 0);
	inum = 0;
	for (;;) {
		register struct dinode *ip;

		if (read(fsfd, ibuf, sizeof ibuf) <= 0) {
			cwarn("%s: i-node read error", fs);
			return (1);
		}
		for (ip=ibuf; ip < &ibuf[NIREAD]; ip++) {
			if (inum++ >= maxino)
				break;
			canshort(ip->di_uid);
			canshort(ip->di_mode);
			cansize(ip->di_size);
			if (ilook(ip, inum))
				return (0);
		}
		if (inum > maxino)
			break;
	}
	return (0);
}

/*
 * Routine that looks at every I-node
 * that we find.
 * Return 1 for premature end.
 */
ilook(ip, ino)
register struct dinode *ip;
register ino_t ino;
{

	if (ip->di_mode == 0)
		return (0);
	if (nflag) {
		register char *lp;

		while (ino > readino) {
			if (gets(line) == NULL)
				return (1);
			for (lp=line; *lp==' ' || *lp=='\t'; lp++)
				;
			if (!isdigit(*lp))
				continue;
			readino = atoi(lp);
			while (isdigit(*lp))
				lp++;
			while (*lp==' ' || *lp=='\t')
				lp++;
			readfile = lp;
		}
		if (readino == ino)
			printf("%-16s %s\n", username(ip->di_uid), readfile);
	} else if (cflag) {
		register CENTRY *cep;
		long size;

		size = (ip->di_size+BSIZE-1)/BSIZE;
		cep = getcentry(size);
		cep->c_nfiles++;
	} else {
		register FENTRY *fep;

		fep = getfentry(ip->di_uid);
		fep->f_nfiles++;
		switch (ip->di_mode & IFMT) {
		case IFREG:
		case IFDIR:
			fep->f_nblocks += (ip->di_size+BSIZE-1)/BSIZE;
			break;
		}
	}
	return (0);
}

/*
 * Using hashing from a large pool,
 * return a pointer to where the FENTRY
 * for that uid was put or created.
 * The table has to be contiguous for sorting.
 */
FENTRY *
getfentry(uid)
register short unsigned uid;
{
	register FENTRY *fep;

	fep = &fentries[(uid<<1)%nuids];
	for (;;) {
		if (fep >= efp)
			fep = fentries;
		if (fep->f_uid == uid)
			return (fep);
		if (fep->f_uid == BADUID) {
			fep->f_uid = uid;
			return (fep);
		}
		fep++;
	}
}

/*
 * Return a CENTRY for a particular file size.
 * This is hashed much like the FENTRY code
 * above.
 */
CENTRY *
getcentry(nb)
long nb;
{
	register CENTRY *cep;

	cep = &centries[((unsigned)nb<<1)%NCUM];
	for (;;) {
		if (cep >= ecp)
			cep = centries;
		if (cep->c_nblocks == nb)
			return (cep);
		if (cep->c_nblocks == BADSIZE) {
			cep->c_nblocks = nb;
			if (cep > lastcep)
				lastcep = cep+1;
			return (cep);
		}
		cep++;
	}
}

/*
 * Read all of the names from `/etc/passwd'
 * and build up a hash table.
 * Duplicates are discarded.
 */
readnames()
{
	register NAME *np;
	register unsigned short hash;
	register struct passwd *pwp;

	while ((pwp = getpwent()) != NULL) {
		hash = pwp->pw_uid%NHASH;
		for (np = names[hash]; np != NULL; np = np->n_next)
			if (np->n_uid == pwp->pw_uid)
				break;
		if (np == NULL) {
			nuids++;
			np = (NAME*)alloc(sizeof(NAME)+strlen(pwp->pw_name)+1);
			strcpy(np->n_name, pwp->pw_name);
			np->n_uid = pwp->pw_uid;
			np->n_next = names[hash];
			names[hash] = np;
		}
	}
	endpwent();
}

/*
 * Return user name for a particular number.
 * Names that are not located are given the string
 * representation of the number.
 */
char *
username(uid)
unsigned short uid;
{
	register NAME *np;
	static char buf[12];

	for (np = names[uid%NHASH]; np != NULL; np = np->n_next)
		if (np->n_uid == uid)
			return (np->n_name);
	return (sprintf(buf, "%u", uid));
}

/*
 * Comparison routine for no options
 * and `-f'.  Sort by # blocks, then
 * # files.
 */
fcomp(fep1, fep2)
register FENTRY *fep1, *fep2;
{
	if (fep1->f_nblocks < fep2->f_nblocks)
		return (1);
	if (fep1->f_nblocks > fep2->f_nblocks)
		return (-1);
	if (fep1->f_nfiles < fep2->f_nfiles)
		return (1);
	if (fep1->f_nfiles > fep2->f_nfiles)
		return (-1);
	return (0);
}

/*
 * Sort comparison routine for the
 * `-c' (cumulative size table)
 * option.
 */
ccomp(cep1, cep2)
register CENTRY *cep1, *cep2;
{
	if (cep1->c_nblocks < cep2->c_nblocks)
		return (-1);
	if (cep1->c_nblocks > cep2->c_nblocks)
		return (1);
	return (0);
}

/*
 * Call allocator and check for errors.
 */
char *
alloc(nb)
register unsigned nb;
{
	register char *ap;

	if ((ap = calloc(nb, 1)) == NULL)
		cerr("out of memory");
	return (ap);
}

/*
 * Errors and usage message.
 */
/* VARARGS */
cerr(x)
{
	fprintf(stderr, "quot: %r\n", &x);
	exit(1);
}

cwarn(x)
{
	fprintf(stderr, "quot: %r\n", &x);
	exit(1);
}

usage()
{
	fprintf(stderr, "Usage: quot [-n] [-c] [-ft] filesystem [ ... ]\n");
	exit(1);
}
