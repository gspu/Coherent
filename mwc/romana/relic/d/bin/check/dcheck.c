/*
 * Rec'd from Lauren Weinstein, 7-16-84.
 * Dcheck - check consistency of directory
 * graph structure for a filesystem.
 * and optionally repair faulty link counts.
 */
#include <stdio.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <sys/dir.h>
#include <sys/ino.h>
#include "check.h"
#include <canon.h>

#define	IBLK	12		/* I-node read blocking factor */
#define	NINUM	20		/* Maximum number of i-numbers to look for */
#undef	NI
#define	NI	1
#define	NII	1
#define	NIII	1
#define	INOORG	2

/*
 * A chain of these structures
 * holds all of the defective blocks found
 * in the bad block file. The list is sorted for
 * easy access by those parts of the program
 * that scan blocks.
 */
struct	defect
{
	struct defect	*d_next;	/* Link to next */
	daddr_t		d_start;	/* First bad block in cluster */
	int		d_length;	/* Size of cluster */
};

/*
 * Tables used by imap.
 * This effectively implements
 * the access polynomial for the indirect
 * blocks.
 */
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

char	tmi[] = "Too many i-numbers given\n";
char	irderr[] = "I-node read error -- pass %d\n";

int	ninumber;
ino_t	inums[NINUM];
struct	defect *deflist;
char	superb[BSIZE];
char	ibuf[BSIZE*IBLK];
char	dbuf[BSIZE];

int	sflag;			/* Repair filesystem */
int	exstat;			/* Exit status */
FILE	*fs;			/* File system i/o stream pointer */
daddr_t	fsize	= SUPERI+1;	/* Allow read of super-block */
ino_t	isize;
ino_t	maxino;
unsigned nhard;			/* Hard things requiring pass 3 to fix */
short	unsigned *entries;

daddr_t	imap();
int	imark();
int	icompare();

main(argc, argv)
char *argv[];
{

	while (argc>1 && *argv[1]=='-') {
		switch (argv[1][1]) {
		case 'i':
			ninumber = 0;
			while(inums[ninumber] = atoi(argv[2])) {
				if (ninumber++ >= NINUM) {
					fprintf(stderr, tmi);
					exstat |= DC_MISC;
					break;
				}
				argv++;
				argc--;
			}
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
	if (argc > 1)
		allcheck(argv+1);
	else
		usage();
	exit(exstat);
}

/*
 * Check the given list of filesystems
 */
allcheck(fsl)
register char **fsl;
{
	while (*fsl != NULL)
		dcheck(*fsl++);
}

/*
 * Check one filesystem
 */
dcheck(fsname)
char *fsname;
{
	register	i;
	struct filsys *sbp;
	char *mode;

	mode = sflag ? "r+w" : "r";
	if ((fs = fopen(fsname, mode)) == NULL) {
		fprintf(stderr, "%s: cannot open\n", fsname);
		exstat |= DC_MISC;
		return;
	}
	printf("%s:\n", fsname);
	if (!sflag)
		sync();
	bread((daddr_t)SUPERI, superb);
	sbp = superb;
	canshort( sbp->s_isize);
	candaddr( sbp->s_fsize);
	canshort( sbp->s_nfree);
	for (i=0; i<NICFREE; ++i)
		candaddr( sbp->s_free[i]);
	canshort( sbp->s_ninode);
	for (i=0; i<NICINOD; ++i)
		canino( sbp->s_inode[i]);
	cantime( sbp->s_time);
	candaddr( sbp->s_tfree);
	canino( sbp->s_tinode);
	canshort( sbp->s_m);
	canshort( sbp->s_n);
	fsize = sbp->s_fsize;
	isize = sbp->s_isize;
	if (isize<INODEI+1 || isize>=fsize)
		cerr("Ridiculous fsize/isize");
	if ((entries=calloc(isize*INOPB, sizeof(short unsigned))) == NULL)
		cerr("Not enough space");
	finddefective();
	maxino = (isize-INODEI) * INOPB;
	/*
	 * The first pass runs down the
	 * graph filling in the array
	 * `entries' which is the number
	 * of names found in directories for
	 * any i-node.
	 */
	entries[ROOTIN-1]++;
	pass(0, imark);
	/*
	 * In the next pass, link counts
	 * in the i-nodes are compared with
	 * those pre-computed for the graph.
	 */
	pass(1, icompare);
	/*
	 * This fixup pass is only
	 * required for some harder errors
	 * encountered in `-s' mode.
	 */
	if (nhard && sflag)
		pass(2, imark);
	free(entries);
	freedefective();
	nhard = 0;
	fclose(fs);
}

/*
 * A generalised pass over all i-nodes, calls
 * the routine `func' for every i-node encountered.
 * `n' is the pass number, used only in the diagnostics.
 */
pass(n, func)
int n;
int (*func)();
{
	register struct dinode *ip;
	register ino_t inum;
	daddr_t seek, limit;
	int thischunk;
	struct defect *cdsp;

	inum = 1;
	seek = INOORG;
	cdsp = deflist;
	while (seek < isize) {
		if (cdsp!=NULL && cdsp->d_start==seek) {
			seek  += cdsp->d_length;
			inum  += cdsp->d_length*INOPB;
			cdsp   = cdsp->d_next;
			continue;
		}
		limit = seek+IBLK;
		if (cdsp!=NULL && limit>cdsp->d_start)
			limit = cdsp->d_start;
		if (limit > isize)
			limit = isize;
		thischunk = limit-seek;
		lseek(fileno(fs), seek*BSIZE, 0);
		seek += thischunk;
		thischunk *= BSIZE;
		if (read(fileno(fs), ibuf, thischunk) != thischunk) {
			fprintf(stderr, irderr, n);
			exstat |= DC_HARD;
			break;
		}
		ip = (struct dinode *) &ibuf[0];
		while (ip < (struct dinode *) &ibuf[thischunk]) {
			if (inum != BADFIN) {
				canshort(ip->di_mode);
				canshort(ip->di_nlink);
				canshort(ip->di_uid);
				canshort(ip->di_gid);
				cansize(ip->di_size);
				cantime(ip->di_atime);
				cantime(ip->di_mtime);
				cantime(ip->di_ctime);
				if ((*func)(ip, inum, n))
					return;
			}
			++inum;
			++ip;
		}
	}
}

/*
 * Check an i-node link count (in
 * pass 2) against the entries already
 * found.
 */
icompare(ip, inum, pn)
register struct dinode *ip;
register ino_t inum;
int pn;
{
	register unsigned nent;

	nent = entries[inum-1];
	entries[inum-1] = 0;
	if (nent != ip->di_nlink
	|| (ip->di_mode!=0 && ip->di_nlink==0))
		badnlink(ip, inum, nent);
	return (0);
}

/*
 * Report or fix up bad link count
 * in filesystem.
 * `entries' is the number found.
 */
badnlink(ip, ino, nent)
register struct dinode *ip;
ino_t ino;
int nent;
{
	static int needtitle = 1;

	if (sflag == 0) {
		if (needtitle != 0) {
			printf(" Ino  Entries   Link\n");
			needtitle = 0;
		}
		printf("%4u  %7u %6u", ino, nent, ip->di_nlink);
		if (ip->di_mode!=0 && ip->di_nlink==0)
			printf(" (u)");
		putchar('\n');
	}
	if (nent == 0) {
		if (sflag) {
			bclear((char *) ip, sizeof (*ip));
			iwrite(ip, ino);
		} else
			exstat |= DC_CLRI;
	} else if (ip->di_mode != 0) {
		if (sflag) {
			ip->di_nlink = nent;
			iwrite(ip, ino);
		} else
			exstat |= DC_LCE;
	} else if (ip->di_mode==0 && ip->di_nlink==0) {
		nhard++;
		entries[ino-1] = nent;
	}
}

/*
 * Imark looks at all directory i-nodes
 * and marks all of the subordinate nodes
 * in the entries table.  It also checks for
 * argument i-numbers to list specially.
 * Returns non-zero if we should stop
 * i-list scanning in `pass'.
 */
imark(ip, inum, pn)
register struct dinode *ip;
register ino_t inum;
int pn;
{
	fsize_t size;
	daddr_t pb;
	register daddr_t bn;

	if (ip->di_mode == 0)
		return (0);
	if ((ip->di_mode&IFMT) != IFDIR)
		return (0);
	size = ip->di_size;
	bn = 0;
	if (pn==1 && (size % sizeof(struct direct))!=0) {
		printf("I#%u: Directory size not mod %d\n", inum,
		    sizeof(struct direct));
		size -= size % sizeof( struct direct);
	}
	while (size) {
		register struct direct *dp;

		if ((pb = imap(ip, bn++)) == 0)
			break;
		bread(pb, dbuf);
		for (dp=dbuf; dp < &dbuf[BSIZE]; dp++) {
			canino( dp->d_ino);
			if (dp->d_ino) {
				if (dp->d_ino > maxino)
					dirline(inum, dp, "bad");
				else if (pn == 0)
					entries[dp->d_ino-1]++;
				else {
					if (entries[dp->d_ino-1]) {
						if (--entries[dp->d_ino-1] == 0)
							nhard--;
						bclear(dp, sizeof(*dp));
						bwrite(pb, dbuf);
					}
				}
				if (ninumber)
					iarg(inum, dp);
			}
			size -= sizeof( struct direct);
			if (size == 0)
				break;
		}
	}
	if (pn==0)
		return (0);
	return (nhard == 0);
}

/*
 * Iarg checks if the directory i-number is in
 * the argument list of i-nodes, and if it is
 * prints this out.
 */
iarg(inum, dp)
register ino_t inum;
register struct direct *dp;
{
	register unsigned i;

	for (i=0; i<ninumber;)
		if (inums[i++] == dp->d_ino)
			dirline(inum, dp, "arg");
}

/*
 * Print out a line for a directory
 * that is found in the search (.e.g.
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
 * This routine finds all of the
 * defective space on the filsystem by reading
 * the bad block file and marking all the blocks.
 * The defective space list, used by the I-list
 * scanner and other guys, is constructued.
 */
finddefective()
{
	register struct dinode *ip;
	daddr_t  pb, lb;

	lseek(fileno(fs), (long)iblockn(BADFIN)*BSIZE, 0);
	if (read(fileno(fs), ibuf, BSIZE) != BSIZE) {
		printf("I/O error reading bad block inode\n");
		exstat |= IC_HARD;
		return;
	}
	ip = (struct dinode *) &ibuf[0] + iblocko(BADFIN);
	canshort(ip->di_mode);
	if (ip->di_mode == 0)
		return;
	if ((ip->di_mode&IFMT) != IFREG) {
		printf("Bad block file has bad mode\n");
		exstat |= IC_HARD;
		return;
	}
	lb = 0;
	while ((pb=imap(ip, lb++)) != 0)
		savedefective(pb);
}

/*
 * Free all of the nodes
 * in the defective space list.
 */
freedefective()
{
	register struct defect *cdsp1, *cdsp2;

	cdsp1 = deflist;
	deflist = NULL;
	while (cdsp1 != NULL) {
		cdsp2 = cdsp1->d_next;
		free((char *) cdsp1);
		cdsp1 = cdsp2;
	}
}

/*
 * Add a new, defective block
 * into the sorted defective block chain.
 * Merge this block with the ends of
 * any existing entries. No check is made
 * for entries fusing; bad blocks get scooped
 * (in general) up in order, and the bad blocks
 * are generally sparsely placed on the disc.
 */
savedefective(bn)
daddr_t bn;
{
	register struct defect *cdsp1, *cdsp2, *cdsp3;

	cdsp1 = NULL;
	cdsp2 = deflist;
	while (cdsp2!=NULL && bn>cdsp2->d_start) {
		cdsp1 = cdsp2;
		cdsp2 = cdsp2->d_next;
	}
	if (cdsp1!=NULL && bn==cdsp1->d_start+cdsp1->d_length) {
		++cdsp1->d_length;
		return;
	}
	if (cdsp2!=NULL && bn==cdsp2->d_start-1) {
		--cdsp2->d_start;
		++cdsp2->d_length;
		return;
	}
	if ((cdsp3=(struct defect *)malloc(sizeof(struct defect))) == NULL)
		cerr("Out of space for bad blocks");
	if (cdsp1 == NULL)
		deflist = cdsp3; else
		cdsp1->d_next = cdsp3;
	cdsp3->d_next = cdsp2;
	cdsp3->d_start = bn;
	cdsp3->d_length = 1;
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

	l3tol(addrs, ip->di_addr, NADDR);
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
		badblock(bn, "any");
		bclear(buf, BSIZE);
		return;
	}
	lseek(fileno(fs), (fsize_t)BSIZE * bn, 0);
	if (read(fileno(fs), buf, BSIZE) != BSIZE) {
		fprintf(stderr, "Read error %ld\n", (long)bn);
		exstat |= DC_HARD;
		bclear(buf, BSIZE);
	}
}

/*
 * Write block `bn' from `buf'.
 */
bwrite(bn, buf)
daddr_t bn;
char *buf;
{
	if (bn >= fsize) {
		badblock(bn, "any");
		return;
	}
	lseek(fileno(fs), (fsize_t)BSIZE * bn, 0);
	if (write(fileno(fs), buf, BSIZE) != BSIZE) {
		fprintf(stderr, "Write error %ld\n", (long)bn);
		exstat |= DC_HARD;
	}
}

/*
 * Put out an i-node to disc.
 * Used only for `-s' option.
 */
iwrite(ip, ino)
register struct dinode *ip;
register ino_t ino;
{
	register struct dinode	*ip2;
	daddr_t		bn;

	bn = iblockn(ino);
	bread(bn, dbuf);
	ip2 = &((struct dinode *)dbuf)[iblocko( ino)];
	*ip2 = *ip;
	canshort( ip2->di_mode);
	canshort( ip2->di_nlink);
	canshort( ip2->di_uid);
	canshort( ip2->di_gid);
	cansize( ip2->di_size);
	cantime( ip2->di_atime);
	cantime( ip2->di_mtime);
	cantime( ip2->di_ctime);
	bwrite(bn, dbuf);
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
 * Error routines
 */
badblock(bn)
daddr_t bn;
{
	printf("Bad block #%D\n", (long)bn);
	exstat |= DC_HARD;
}

/*
 * Unrecoverable errors
 */
cerr(x)
{
	printf("%r", &x);
	putchar('\n');
	exit(DC_MISC);
}

usage()
{
	cerr("Usage: dcheck [-s] [-i ino ...] filesystem ...");
}

/*
 * Block copy routine
 */
bcopy(in, out, nb)
register char *in, *out;
register unsigned nb;
{
	if (nb)
		do {
			*out++ = *in++;
		} while (--nb);
}
