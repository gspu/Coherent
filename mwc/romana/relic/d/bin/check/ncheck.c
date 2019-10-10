/*
 * ncheck -- map an I-number into a pathname.
 * Also, look for special and setuid files.
 */

#include <stdio.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <sys/dir.h>
#include <sys/ino.h>
#include <canon.h>

#define	NHASH	101		/* Prime provides a reasonable distribution */
#define	NSBRK	512		/* Bytes to add each time out */
#define	NFNAME	400		/* Longest filename generated */
#define	NBPC	8		/* Bits per char (for bitmap) */
#define	IBLK	12		/* I-node read blocking factor */
#define	NINUM	20		/* Maximum number of i-numbers to look for */
#define	ESEEN	0200		/* Seen bit for ENTRY */
#define	unpack()	l3tol(addrs, ip->di_addr, NADDR)

/* Functions to test for directory or setuid/special i-numbers */
#define	test(bm,i)	(bm[(i)/NBPC] & 1<<((i)%NBPC))
#define	mark(bm,i)	(bm[(i)/NBPC] |= 1<<((i)%NBPC))

/*
 * Tables used by imap.
 * This effectively implements
 * the access polynomial for the indirect
 * blocks.
 */
#undef	NI
#define	NI	1
#define	NII	1
#define	NIII	1
static	daddr_t	ranges[] = {
	ND,
	ND + (daddr_t)NI*NBN,
	ND + (daddr_t)NI*NBN + (daddr_t)NII*NBN*NBN,
	ND + (daddr_t)NI*NBN + (daddr_t)NII*NBN*NBN + (daddr_t)NIII*NBN*NBN*NBN,
};

static	char	offsets[] = {
	0,
	ND,
	ND+NI,
	ND+NI+NII,
};

static	daddr_t	coeff[] = {
	1, (daddr_t)NBN, (daddr_t)NBN*NBN, (daddr_t)NBN*NBN*NBN
};

/*
 * Default filesystem names
 * to check.
 */
char	*defnames[] = {
	"/dev/rrm00",
	NULL
};

char	tmi[] = "ncheck: too many i-numbers given\n";
char	irderr[] = "ncheck: inode read error -- pass %d\n";

/*
 * An entry for each directory
 * name in the system containing
 * the current i-number and the
 * parent and the name.
 */
typedef	struct	ENTRY {
	struct ENTRY	*e_next;
	ino_t	e_pino;			/* Parent i-number */
	ino_t	e_cino;			/* Current i-number */
	char	e_name[];		/* Name */
}	ENTRY;

ENTRY	*entries[NHASH];		/* Hashed entries */

char	*dbmap;				/* Directory I-node bit-map */
char	*sbmap;				/* Special + setuid i-node bitmap */

int	ninumber;
ino_t	inums[NINUM];
char	superb[BSIZE];
char	ibuf[BSIZE*IBLK];
char	dbuf[BSIZE];
char	namebuf[NFNAME];

int	aflag;			/* All (print "." and ".." names) flag */
int	sflag;			/* Special and setuid files */
int	uflag;			/* Print unreferenced structure */
int	exstat;			/* Exit status */
int	fsfd;			/* File system file descriptor */
daddr_t	fsize	= SUPERI+1;		/* Allow read of super-block */
ino_t	isize;
ino_t	maxino;

daddr_t	imap();
char	*malloc();

main(argc, argv)
char *argv[];
{

	while (argc>1 && *argv[1]=='-') {
		switch (argv[1][1]) {
		case 'a':
			aflag = 1;
			break;

		case 'i':
			for (;;) {
				if (ninumber >= NINUM) {
					fprintf(stderr, tmi);
					exstat = 1;
					break;
				}
				if ((inums[ninumber] = atoi(argv[2])) == 0)
					break;
				argv++;
				argc--;
				ninumber++;
			}
			break;

		case 's':
			sflag = 1;
			break;

		case 'u':	/* Unimplemented search for orphan structure */
			uflag = 1;
			break;

		default:
			usage();
		}
		argc--;
		argv++;
	}
	if (argc > 1)
		allcheck(argv+1); else
		allcheck(defnames);
	exit(exstat);
}

/*
 * Check the given list of filesystems
 */
allcheck(fsl)
register char **fsl;
{
	while (*fsl != NULL)
		ncheck(*fsl++);
}

/*
 * Do `ncheck' for each filesystem.
 */
ncheck(fsname)
char *fsname;
{
	register struct filsys *sbp;
	register unsigned nb;

	if ((fsfd = open(fsname, 0)) < 0) {
		fprintf(stderr, "%s: cannot open\n", fsname);
		exstat = 1;
		return;
	}
	printf( "%s:\n", fsname);
	sync();
	bread((daddr_t)SUPERI, superb);
	sbp = superb;

	canshort(sbp->s_isize);
	candaddr(sbp->s_fsize);

	fsize = sbp->s_fsize;
	isize = sbp->s_isize;
	if (isize<INODEI+1 || isize>=fsize)
		cerr("Ridiculous fsize/isize");
	maxino = (isize-INODEI) * INOPB;
	nb = (maxino+NBPC)/NBPC;
	if ((dbmap = malloc(nb)) == NULL)
		cerr("Out of memory for directory bit map");
	bclear(dbmap, nb);
	if (sflag) {
		if ((sbmap = malloc(nb)) == NULL)
			cerr("Out of memory for special/setuid bit map");
		bclear(sbmap, nb);
	}
	pass1();
	pass2();
	pass3();
	mfree();
	close(fsfd);
}

/*
 * Pass one consists of running down each
 * I-node and marking those that are directories.
 * This is used to tell which directory entries are
 * to be saved in the hash chains in pass 2.
 */
pass1()
{
	register struct dinode *ip;
	register ino_t inum;
	register int i;
	register unsigned imax;
	fsize_t seek;

	inum = 1;
	seek = INODEI*BSIZE;
	for (i=maxino; i>0; i -= IBLK*INOPB) {
		lseek(fsfd, seek, 0);
		imax = i>IBLK*INOPB ? IBLK*INOPB : i;
		imax *= sizeof (struct dinode);
		seek += imax;
		if (read(fsfd, ibuf, imax) != imax) {
			fprintf(stderr, irderr, 1);
			exstat = 1;
			return;
		}
		for (ip = ibuf; ip < &ibuf[imax]; ip++) {
			canshort(ip->di_mode);
			canshort(ip->di_nlink);
			if ((ip->di_mode & IFMT) == IFDIR)
				mark(dbmap, inum);
			if (sflag && ip->di_mode&(ISUID|ISGID|IFBLK|IFCHR))
				mark(sbmap, inum);
			inum++;
		}
	}
}

/*
 * Pass two scans the i-list looking for
 * all directories.  For each directory
 * entry in each of these found, it
 * saves each name and i-number pair which
 * is itself a directory (as determined
 * by the bitmap computed in pass1).
 */
pass2()
{
	register struct dinode *ip;
	register ino_t inum;
	register int i;
	register unsigned imax;
	fsize_t seek;

	inum = 1;
	seek = INODEI*BSIZE;
	for (i=maxino; i>0; i -= IBLK*INOPB) {
		lseek(fsfd, seek, 0);
		imax = i>IBLK*INOPB ? IBLK*INOPB : i;
		imax *= sizeof (struct dinode);
		seek += imax;
		if (read(fsfd, ibuf, imax) != imax) {
			fprintf(stderr, irderr, 2);
			exstat = 1;
			return;
		}
		for (ip = ibuf; ip < &ibuf[imax]; ip++) {
			canshort(ip->di_mode);
			canshort(ip->di_nlink);
			cansize(ip->di_size);
			if ((ip->di_mode & IFMT) == IFDIR)
				finddirs(ip, inum);
			inum++;
		}
	}
}

/*
 * Pass 3 uses the hashed table prodeuced
 * during pass1 and pass2 to generate
 * the output information that was
 * requested by the command line.
 */
pass3()
{
	register struct dinode *ip;
	register ino_t inum;
	register int i;
	register unsigned imax;
	fsize_t seek;

	inum = 1;
	seek = INODEI*BSIZE;
	if (!sflag && (ninumber==0 || iarg(ROOTIN)))
		printf( "%u\t/.\n", ROOTIN);
	for (i=maxino; i>0; i -= IBLK*INOPB) {
		lseek(fsfd, seek, 0);
		imax = i>IBLK*INOPB ? IBLK*INOPB : i;
		imax *= sizeof (struct dinode);
		seek += imax;
		if (read(fsfd, ibuf, imax) != imax) {
			fprintf(stderr, irderr, 3);
			exstat = 1;
			return;
		}
		for (ip = ibuf; ip < &ibuf[imax]; ip++) {
			canshort(ip->di_mode);
			canshort(ip->di_nlink);
			cansize(ip->di_size);
			if ((ip->di_mode & IFMT) == IFDIR)
				printdir(ip, inum);
			inum++;
		}
	}
}

/*
 * Find all entries in this directory i-node
 * that are themselves directories.
 */
finddirs(ip, inum)
register struct dinode *ip;
register ino_t inum;
{
	fsize_t size;
	daddr_t pb, bn;

	size = ip->di_size;
	bn = 0;
	while (size >= sizeof(struct direct)) {
		register struct direct *dp;

		if ((pb = imap(ip, bn++)) == 0)
			break;
		bread(pb, dbuf);
		for (dp=dbuf; dp < &dbuf[BSIZE]; dp++) {
			canino( dp->d_ino);
			if (dp->d_ino) {
				if (dp->d_ino > maxino)
					dirline(inum, dp, "bad");
				else if (test(dbmap, dp->d_ino))
					direnter(dp, inum);
			}
			size -= sizeof( struct direct);
			if (size == 0)
				break;
		}
	}
}

/*
 * Print all of the names found
 * in this directory.
 */
printdir(ip, ino)
register struct dinode *ip;
register ino_t ino;
{
	fsize_t size;
	daddr_t pb, bn;

	size = ip->di_size;
	bn = 0;
	while (size >= sizeof(struct direct)) {
		register struct direct *dp;

		if ((pb = imap(ip, bn++)) == 0)
			break;
		bread(pb, dbuf);
		for (dp=dbuf; dp < &dbuf[BSIZE]; dp++) {
			canino( dp->d_ino);
			if (dp->d_ino) {
				if (dp->d_ino > maxino)
					continue;
				if (sflag && !test(sbmap, dp->d_ino))
					continue;
				if (ninumber!=0 && !iarg(dp->d_ino))
					continue;
				outname(dp, ino);
			}
			size -= sizeof( struct direct);
			if (size == 0)
				break;
		}
	}
}

/*
 * Print out the actual name by
 * traversing the structures
 * for a directory entry.
 */
outname(dp, ino)
register struct direct *dp;
ino_t ino;
{
	register char *np;

	np = dp->d_name;
	if (!aflag && *np++=='.')
		if ((*np=='.' && np[1]=='\0') || *np=='\0')
			return;
	np = &namebuf[NFNAME];
	*--np = '\0';
	if (!aflag && test(dbmap, dp->d_ino)) {
		*--np = '.';
		*--np = '/';
	}
	{
		register char *cp;

		for (cp = dp->d_name; cp < &dp->d_name[DIRSIZ]; cp++)
			if (*cp == '\0')
				break;
		while (cp > dp->d_name)
			*--np = *--cp;
		*--np = '/';
	}
	outpart(np, ino, dp->d_ino);
}

/*
 * Put out each name part.
 * Either get to the root
 * or find no parent.
 * `ep' is the pointer running
 * backwards in the namebuf.
 */
outpart(np, ino, oino)
register char *np;
ino_t ino;
ino_t oino;
{
	register ENTRY *ep;
	register char *cp;
	register int found = 0;
	register char *snp;

	if (ino != ROOTIN) {
		for (ep = entries[ino%NHASH]; ep != NULL; ep = ep->e_next)
			if (ep->e_cino == ino) {
				if (ep->e_name[0] & ESEEN) {
					*--np = '.';
					*--np = '.';
					*--np = '.';
					return;
				}
				snp = np;
				cp = &ep->e_name[strlen(ep->e_name)];
				while (cp > ep->e_name)
					*--np = *--cp;
				*--np = '/';
				ep->e_name[0] |= ESEEN;
				found = 1;
				if (np > namebuf+DIRSIZ)
					outpart(np, ep->e_pino, oino);
				ep->e_name[0] &= ~ESEEN;
				np = snp;
			}
		if (!found) {
			*--np = '?';
			*--np = '?';
		}
	}
	if (!found || ino==ROOTIN)
		printf( "%u\t%s\n", oino, np);
}

/*
 * Enter a directory entry and the current
 * i-number into the chained hash table
 * for use by pass 3.
 */
direnter(dp, ino)
register struct direct *dp;
ino_t ino;
{
	register ENTRY *ep;
	register char *cp;
	register int n;

	cp = dp->d_name;
	if (*cp++ == '.')
		if ((*cp=='.' && cp[1]=='\0') || *cp=='\0')
			return;
	for (cp = dp->d_name; *cp != '\0'; cp++)
		if (cp >= &dp->d_name[DIRSIZ])
			break;
	n = cp - dp->d_name;
	if ((ep = (ENTRY *)malloc(n+sizeof(char)+sizeof(ENTRY))) == NULL)
		cerr("Out of memory for directory entries");
	ep->e_pino = ino;
	ep->e_cino = dp->d_ino;
	strncpy(ep->e_name, dp->d_name, n);
	ep->e_name[n] = '\0';
	/*
	 * Compute hash and store in entries table.
	 */
	n = ep->e_cino%NHASH;
	ep->e_next = entries[n];
	entries[n] = ep;
}

/*
 * Return true if the argument
 * i-node is the one of the `-i'
 * arguments.
 */
iarg(ino)
register ino_t ino;
{
	register int i;

	for (i=0; i<ninumber; i++)
		if (inums[i] == ino)
			return (1);
	return (0);
}

/*
 * Print out a line for a directory
 * that is found in the search (e.g.
 * bad or argument directories).
 */
dirline(ino, dp, str)
ino_t ino;
register struct direct *dp;
char *str;
{
	printf("%u %s: %u/%-*.*s\n", dp->d_ino, str, ino,
	    DIRSIZ, DIRSIZ, dp->d_name);
}

/*
 * For a given inode (`ip'),
 * map a logical block number (`bn')
 * onto a physical disc block number.
 */
daddr_t
imap(ip, lb)
register struct dinode *ip;
daddr_t lb;
{
	register il;
	daddr_t bpos, pb;
	register daddr_t *bp;
	register daddr_t addrs[NADDR];

	unpack();
	for (il=0; il<4; il++)
		if (lb < ranges[il]) {
			if (il != 0)
				lb -= ranges[il-1];
			bpos = lb/coeff[il];
			lb %= coeff[il];
			bp = &addrs[(int)bpos + offsets[il]];
			if ((pb = *bp) != 0) {
				/*
				 * Map through indirect
				 * blocks here.
				 */
				while (il-- > 0) {
					bread(pb, dbuf);
					bpos = lb/coeff[il];
					lb %= coeff[il];
					bp = (daddr_t *)dbuf + bpos;
					if ((pb = *bp) == 0)
						break;
					pb = *bp;
					candaddr( pb);
				}
			}
			return (pb);
		}
	return (0);
}

/*
 * Read the specified block number
 * into `buf'.
 */
bread(bn, buf)
daddr_t bn;
char *buf;
{
	if (bn >= fsize) {
		badblock(bn);
		bclear(buf, BSIZE);
		return;
	}
	lseek(fsfd, (fsize_t)BSIZE * bn, 0);
	if (read(fsfd, buf, BSIZE) != BSIZE) {
		fprintf(stderr, "ncheck: Read error %ld\n", (long)bn);
		exstat = 1;
		bclear(buf, BSIZE);
	}
}

/*
 * Clear a block of memory
 * pointed to by `bp' for size
 * `nb' bytes.
 */
bclear(bp, nb)
register char *bp;
register unsigned nb;
{
	if (nb)
		do {
			*bp++ = 0;
		} while (--nb);
}

/*
 * free scratch memory
 * Space used to check a filesystem is freed.  This includes bitmaps
 * and directory entries.
 */
mfree( )
{
	register ENTRY	*ep,
			**epp;

	for (epp=entries; epp<&entries[NHASH]; ) {
		for (ep= *epp; ep; ep=ep->e_next)
			free( (char *)ep);
		*epp++ = NULL;
	}
	free( dbmap);
	if (sbmap)
		free( sbmap);
}

/*
 * Error routines
 */
badblock(bn)
daddr_t bn;
{
	fprintf(stderr, "ncheck: bad block #%ld\n", (long)bn);
	exstat = 1;
}

/*
 * Unrecoverable errors
 */
cerr(x)
{
	fprintf(stderr, "ncheck: %r\n", &x);
	exit(1);
}

usage()
{
	fprintf(stderr, "Usage: ncheck [-a] [-s] [-i ino ...] [filesystem ...]");
}
