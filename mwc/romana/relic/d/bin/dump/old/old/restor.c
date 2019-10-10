/*
 * Restore.
 * restor key [args]
 * f [dump]	Use this dump, not the default.
 * v		Verbose.
 * t		Print dates of the dump.
 * x,X		Extract. X asks for reel numbers and does
 *		a rewind between reels.
 * r,R		Mass restore. R asks for reel numbers and does
 * 		a rewind between reels.
 *
 * Multiple dumps can be put on a single spool of
 * tape by dumping to the no rewind device. The restore is
 * done by positioning the tape using the `skip' command and
 * restoring using the `x' option.
 */
#include <stdio.h>
#include <dumptape.h>
#include <canon.h>
#include <filsys.h>
#include <fblk.h>
#include <discbuf.h>
#include <signal.h>

#define	NXF	20			/* # of `x' files */
#define	NRBUF	10			/* # of restore cache buffers */

/*
 * This structure is used
 * to remember the names and i numbers
 * of the files being extracted.
 */
struct	xf
{
	char	*xf_path;		/* Path name */
	ino_t	xf_ino;			/* The associated inumber */
};

/*
 * Structure used to remember
 * things about the directories that
 * were on the tape.
 */
struct	dlist
{
	struct	dlist	*dl_dlp;	/* Link */
	ino_t	dl_ino;			/* Inumber of the directory */
	long	dl_seek;		/* Temp file seek address */
	long	dl_size;		/* Size in bytes */
};

int	key;				/* Operation */
int	vflag;				/* A verbose flag */
char	*dtn	= DTAPE;		/* Dump file name */
FILE	*dtp;				/* Its file pointer */
struct	dumpheader dh;			/* Header buffer */
int	reel	= 1;			/* Reel # */
char	tfn[30]	= "/tmp/ddxxxxxx";	/* Temp file name */
FILE	*tfp;				/* Its file pointer */
struct	dlist	*dlist;			/* List of directory nodes */
struct	dlist	*droot;			/* Root (first) directory node */
char	*ddbuf;				/* Big buffer */
char	*ddend;				/* End of the big buffer */
int	ddnbuf;				/* Size of the big buffer */
union	dumpdata *ddptr;		/* Current buffer pointer */
char	*map;				/* Directory map */
int	nxf;				/* # of `x' files */
ino_t	nindisc;			/* # of inodes on the disc */
ino_t	ningrab;			/* # of inodes to grab */
struct	xf xf[NXF];			/* Save for x names */

/*
 * Block mapping tables.
 */
#define	LNBN	((daddr_t) NBN)

char	offs[]		= {
	0,
	ND,
	ND+1,
	ND+1+1,
	ND+1+1+1
};

daddr_t	ranges[]	= {
	ND,
	ND + 1*LNBN,
	ND + 1*LNBN + 1*LNBN*LNBN,
	ND + 1*LNBN + 1*LNBN*LNBN + 1*LNBN*LNBN*LNBN
};

char	shifts[]	= {
	0,
	L2NBN,
	2*L2NBN,
	3*L2NBN
};

daddr_t	masks[]		= {
	0,
	LNBN-1,
	LNBN*LNBN-1,
	LNBN*LNBN*LNBN-1
};

/*
 * Forward references for the
 * one pass compiler.
 */
ino_t	lookup();
ino_t	numfile();
char	*ctime();
int	cleanup();
struct	dlist *findnode();
union	dumpdata *readdump();

main(argc, argv)
char *argv[];
{
	register char *p;
	register c, i;
	char *name, *path;
	ino_t ino;

	if (argc < 2)
		usage();
	p = argv[1];
	i = 1;
	while ((c = *p++) != '\0') {
		switch (c) {

		case 'f':
			if (++i >= argc)
				usage();
			dtn = argv[i];
			break;

		case 'r':
		case 'R':
		case 'x':
		case 'X':
		case 't':
			if (key != 0)
				usage();
			key = c;
			break;

		case 'v':
			vflag = 1;
			break;

		default:
			usage();
		}
	}
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, cleanup);
	switch (key) {

	case 'r':
	case 'R':
		if (++i >= argc)
			usage();
		if ((dbfp = fopen(argv[i], "r+w")) == NULL)
			message(1, "%s: cannot open filesystem", argv[1]);
		dbclaim(NRBUF);
		if (key == 'r') {
			opendump();
			readhead(1);
		} else {
			for (;;) {
				reel = getreel();
				opendump();
				if (readhead(0) != 0)
					break;
				fclose(dtp);
			}
		}
		restore();
		dbflush(1);
		break;

	case 'x':
	case 'X':
		opendump();
		readhead(1);
		mktemp(tfn);
		if ((tfp = fopen(tfn, "w")) == NULL
		||  (tfp = freopen(tfn, "r+w", tfp)) == NULL)
			message(1, "cannot create temporary file");
		readdirs();
		while (++i < argc) {
			name = argv[i];
			if ((ino = numfile(name)) != 0) {
				if (ino<ROOTIN || ino>dh.dh_nino) {
					message(0, "%s: bad inumber", name);
					continue;
				}
				path = "(by ino)";
			} else  if ((ino = lookup(name)) != 0)
				path = name;
			else {
				message(0, "%s: not found", name);
				continue;
			}
			if (getmap(ino) == 0) {
				message(0, "%s: not dumped", name);
				continue;
			}
			if (nxf >= NXF) {
				message(0, "%s: too many names", name);
				continue;
			}
			xf[nxf].xf_path = path;
			xf[nxf].xf_ino  = ino;
			++nxf;
		}
		if (nxf == 0)
			break;
		for (i=0; i<nxf; ++i)
			printf("%u\t%s\n", xf[i].xf_ino, xf[i].xf_path);
		if (key == 'x')
			readfile(0);
		else {
			for (;;) {
				fclose(dtp);
				reel = getreel();
				opendump();
				readfile(1);
				for (i=0; i<nxf && xf[i].xf_ino==0; ++i)
					;
				if (i == nxf)
					break;
			}
		}
		fclose(dtp);
		for (i=0; i<nxf; ++i)
			if (xf[i].xf_ino != 0)
				message(0, "%s: not restored", xf[i].xf_path);
		break;

	case 't':
		opendump();
		readhead(1);
		printf("Dump since %s", ctime(&dh.dh_ddate));
		printf("Dumped  on %s", ctime(&dh.dh_bdate));
		break;

	default:
		usage();
	}
	delexit(0);
}

/*
 * If the supplied character string
 * is all number convert it to binary and
 * return it. Otherwise return 0. A
 * file name that is all numeric is taken
 * to be an inumber.
 */
ino_t
numfile(s)
register char *s;
{
	register ino_t ino;
	register c;

	ino = 0;
	while ((c = *s++)>='0' && c<='9')
		ino = 10*ino + c - '0';
	if (c != '\0')
		return (0);
	return (ino);
}

/*
 * Open the dump tape.
 * Die if the tape cannot be opened
 * for any reason.
 */
opendump()
{
	if ((dtp = fopen(dtn, "r")) == NULL)
		message(1, "%s: cannot open dump file", dtn);
}

/*
 * Read reel number.
 */
getreel()
{
	register c, flag, reel;

	for (;;) {
		printf("restor: desired reel? ");
		reel = 0;
		flag = 0;
		while ((c = getchar())>='0' && c<='9') {
			flag = 1;
			reel = 10*reel + c - '0';
		}
		if (c == EOF)
			delexit(1);
		if (c=='\n' && flag!=0)
			return (reel);
		message(0, "bad reel number");
		while (c!=EOF && c!='\n')
			c = getchar();
		if (c == EOF)
			delexit(1);
	}
}

/*
 * Do the hard work of a
 * restore.
 */
readfile(flag)
{
	register union dumpdata *ddp;
	register ino_t ino;
	register FILE *rfp;
	register i;
	int bwerror, outsync;
	char rfn[20];

	if (flag) {
		if (readhead(0) == 0)
			return;
		while ((ddp = readdump()) != NULL) {
			canint(ddp->dd_type);
			if (ddp->dd_type != DD_MAP)
				break;
			canino(ddp->dd_ino);
			canint(ddp->dd_nmap);
			setmap(ddp);
		}
	} else if ((ddp = readdump()) != NULL)
		canint(ddp->dd_type);
	if (ddp==NULL || anyfiles()==0)
		return;
	outsync = 0;
	do {
		switch (ddp->dd_type) {

		case DD_EOT:
			return;

		case DD_INO:
			canino(ddp->dd_ino);
			ino = ddp->dd_ino;
			rfp = NULL;
			bwerror = 0;
			if (outsync == 1)
				message(0, "skipped 1 item");
			else if (outsync != 0)
				message(0, "skipped %d items", outsync);
			outsync = 0;
			for (i=0; i<nxf && xf[i].xf_ino!=ino; ++i)
				;
			if (i != nxf) {
				sprintf(rfn, "%u", ino);
				if ((rfp = fopen(rfn, "w")) == NULL)
					message(0, "%s: cannot create", rfn);
				else
					xf[i].xf_ino = 0;
			}
			while ((ddp = readdump()) != NULL) {
				canint(ddp->dd_type);
				if (ddp->dd_type != DD_DATA)
					break;
				canino(ddp->dd_ino);
				if (ddp->dd_ino != ino) {
					if (outsync == 0)
						message(0, "data sync");
					++outsync;
					continue;
				}
				if (rfp != NULL) {
					candaddr(ddp->dd_block);
					canint(ddp->dd_size);
					if (bwrite(rfp, ddp) == 0)
						bwerror = 1;
				}
			}
			if (bwerror)
				message("%s: write error", rfn);
			if (rfp != NULL)
				fclose(rfp);
			if (anyfiles() == 0)
				return;
			break;

		default:
			if (outsync == 0)
				message(0, "inode sync");
			++outsync;
		}
	} while (ddp != NULL);
}

/*
 * Do a restore.
 * The tape is open and the header
 * has been checked.
 */
restore()
{
	register union dumpdata *ddp;
	register DISCBUF *dbp;
	struct filsys *fsp;
	register ino_t ino;
	struct dinode *dip;
	struct dinode dinode;
	int me, outsync;
	int ifmt, ndeleted;

	/*
	 * If the target file system is not
	 * large enough, complain about it and pretend
	 * that all of the inodes that are beyond the
	 * end are not there. This may, of course, leave
	 * the file system addled.
	 */
	dbp = dbread((long) SUPERI);
	fsp = (struct filsys *) dbp->db_data;
	nindisc = INOPB * (fsp->s_isize-INODEI);
	ningrab = dh.dh_nino;
	if (ningrab > nindisc) {
		message(0, "I-list too small, some I-nodes may be deleted");
		ningrab = nindisc;
	}
	/*
	 * Read in the map.
	 */
	while ((ddp=readdump()) != NULL) {
		canint(ddp->dd_type);
		if (ddp->dd_type != DD_MAP)
			break;
		canino(ddp->dd_ino);
		canint(ddp->dd_nmap);
		setmap(ddp);
	}
	/*
	 * Do required inode clears.
	 * Never clear the badblock inode.
	 * Clear only as far as we are restoring;
	 * that is, ningrab inodes.
	 */
	for (ino=ROOTIN; ino<=ningrab; ++ino) {
		me = getmap(ino);
		if ((me&DD_BUSY)==0 || (me&DD_HERE)==DD_HERE) {
			if (vflag)
				message(0, "clri I#%u", ino);
			clri(ino);
		}
	}
	/*
	 * Read through the tape.
	 * Look for files that we are going to
	 * restore and do so.
	 */
	outsync  = 0;
	ndeleted = 0;
	while (ddp!=NULL && ddp->dd_type!=DD_EOT) {
		if (ddp->dd_type != DD_INO) {
			if (outsync++ == 0)
				message(0, "inode sync");
			if ((ddp=readdump()) != NULL)
				canint(ddp->dd_type);
			continue;
		}
		/*
		 * Restore the file.
		 * Block by block.
		 * Don't move anything.
		 */
		canino(ddp->dd_ino);
		if ((ino=ddp->dd_ino) > ningrab) {
			if (vflag != 0)
				message(0, "deleted I#%u", (unsigned)ino);
			++ndeleted;
			printskip(outsync);
			outsync = 0;
			while ((ddp=readdump()) != NULL) {
				canint(ddp->dd_type);
				if (ddp->dd_type != DD_DATA)
					break;
				canino(ddp->dd_ino);
				if (ddp->dd_ino!=ino && outsync++==0)
					message(0, "skip sync");
			}
			continue;
		}
		if (vflag != 0)
			message(0, "restoring I#%u", (unsigned)ino);
		copyb(&dinode, &ddp->dd_dinode, sizeof(dinode));
		canshort(ddp->dd_dinode.di_mode);
		ifmt = ddp->dd_dinode.di_mode&IFMT;
		if (ifmt==IFREG || ifmt==IFDIR)
			zerob(dinode.di_addr, sizeof(dinode.di_addr));
		printskip(outsync);
		outsync = 0;
		while ((ddp=readdump()) != NULL) {
			canint(ddp->dd_type);
			if (ddp->dd_type != DD_DATA)
				break;
			canino(ddp->dd_ino);
			if (ddp->dd_ino != ino) {
				if (outsync++ == 0)
					message(0, "data sync");
				continue;
			}
			candaddr(ddp->dd_block);
			canint(ddp->dd_size);
			dbp = dbimap(&dinode, ddp->dd_block);
			copyb(dbp->db_data,ddp->dd_data,ddp->dd_size);
			dbfree(dbp, DB_DIRT);
		}
		dbp = dbread((long) (--ino/INOPB + INODEI));
		dip = (struct dinode *)(dbp->db_data) + ino%INOPB;
		if (dip->di_mode != 0) {
			message(0, "I#%u busy", (unsigned)ino+1);
			clri(ino+1);
		}
		copyb((char *)dip, (char *)&dinode, sizeof(dinode));
		dbfree(dbp, DB_DIRT);
	}
	if (ndeleted != 0)
		message(0, "%d I-nodes deleted", ndeleted);
	fixilist();
}

/*
 * Print out a message that
 * tells the number of data items that were
 * skipped on the tape. Special stuff for
 * handling 0 and 1 items.
 */
printskip(n)
{
	if (n == 1)
		message(0, "skipped 1 item");
	else if (n != 0)
		message(0, "skipped %d items", n);
}

/*
 * Fix the ifree list in
 * the super block. It is safer to
 * always reconstruct it.
 */
fixilist()
{
	DISCBUF *ibp, *sbp;
	register struct filsys *fsp;
	register struct dinode *dip;
	register ino_t  ino;
	short ninode, tinode, minode;
	ino_t tmpino;

	if (vflag)
		message(0, "building I-free list");
	sbp = dbread((long) SUPERI);
	fsp = (struct filsys *)(sbp->db_data);
	tinode = 0;
	ninode = 0;
	minode = fsp->s_isize;
	canshort(minode);
	minode = INOPB * (minode-INODEI);
	ibp = NULL;
	for (ino=1; ino<=minode; ++ino) {
		if ((ino-1)%INOPB == 0) {
			if (ibp != NULL)
				dbfree(ibp, 0);
			ibp = dbread((long) ((ino-1)/INOPB + INODEI));
			dip = (struct dinode *)(ibp->db_data);
		}
		if (dip->di_mode == 0) {
			++tinode;
			if (ninode < NICINOD) {
				tmpino = ino;
				canino(tmpino);
				fsp->s_inode[ninode++] = tmpino;
			}
		}
		++dip;
	}
	dbfree(ibp, 0);
	canshort(tinode);
	fsp->s_tinode = tinode;
	canshort(ninode);
	fsp->s_ninode = ninode;
	strncpy(fsp->s_fname, dh.dh_fname, sizeof(dh.dh_fname));
	strncpy(fsp->s_fpack, dh.dh_fpack, sizeof(dh.dh_fpack));
	dbfree(sbp, DB_DIRT);
}

/*
 * Clear an inode.
 * Free all of its blocks and
 * zero the on disc inode. Don't worry about
 * the super block as it is always rebuilt
 * at the end.
 */
clri(ino)
ino_t ino;
{
	register DISCBUF *dbp;
	register struct dinode *dip;
	register i;
	short dimode;
	daddr_t addr[NADDR];

	dbp = dbread((long) ((ino-1)/INOPB + INODEI));
	dip = (struct dinode *)(dbp->db_data) + (ino-1)%INOPB;
	dimode = dip->di_mode;
	canshort(dimode);
	if ((dimode&IFMT)==IFDIR || (dimode&IFMT)==IFREG) {
		l3tol(addr, dip->di_addr, NADDR);
		for (i=0; i<NADDR-3; ++i)
			bfree(addr[i], 0);
		bfree(addr[NADDR-3], 1);
		bfree(addr[NADDR-2], 2);
		bfree(addr[NADDR-1], 3);
	}
	zerob((char *) dip, sizeof(struct dinode));
	dbfree(dbp, DB_DIRT);
}

/*
 * Free a block.
 * The first argument is the block
 * number. 0 here means no block is allocated
 * and the call is a nop. The second argument
 * is the number of levels of indirect blocks
 * to read through.
 */
bfree(bn, nil)
daddr_t bn;
{
	register DISCBUF *dbp, *dbp1;
	struct filsys *fsp;
	struct fblk *fbp;
	int dbp1flag;
	int i;
	daddr_t ibn;

	if (bn == 0)
		return;
	dbp1 = NULL;
	if (nil != 0) {
		dbp1flag = 0;
		dbp1 = dbread((long) bn);
		for (i=0; i<NBN; ++i) {
			ibn = ((daddr_t *) dbp->db_data)[i];
			if (ibn != 0)
				bfree(ibn, nil-1);
		}
	}
	dbp = dbread((long) SUPERI);
	fsp = (struct filsys *) dbp->db_data;
	canshort(fsp->s_nfree);
	if (fsp->s_nfree == NICFREE) {
		if (dbp1 == NULL)
			dbp1 = dbread((long) bn);
		dbp1flag = DB_DIRT;
		fbp = (struct fblk *) dbp->db_data;
		fbp->df_nfree = fsp->s_nfree;
		canshort(fbp->df_nfree);
		copyb(fbp->df_nfree, fsp->s_free, sizeof(fsp->s_free));
		fsp->s_nfree = 0;
	}
	candaddr(bn);
	fsp->s_free[fsp->s_nfree++] = bn;
	canshort(fsp->s_nfree);
	candaddr(fsp->s_tfree);
	++fsp->s_tfree;
	candaddr(fsp->s_tfree);
	if (dbp1 != NULL)
		dbfree(dbp1, dbp1flag);
	dbfree(dbp, DB_DIRT);
}

/*
 * Allocate a block.
 * Return 0 if there are no blocks
 * remaining.
 */
daddr_t
balloc()
{
	register struct filsys *fsp;
	register DISCBUF *dbp;
	register DISCBUF *dbp1;
	register struct fblk *fbp;
	short nfree;
	daddr_t tfree, bn;

	dbp = dbread((long) SUPERI);
	fsp = (struct filsys *) dbp->db_data;
	if ((tfree = fsp->s_tfree) == 0)
		message(1, "out of space");
	candaddr(tfree);
	nfree = fsp->s_nfree;
	canshort(nfree);
	if ((bn = fsp->s_free[--nfree]) == 0)
		message(1, "out of space and tfree lied");
	candaddr(bn);
	if (nfree == 0) {
		dbp1 = dbread((long) bn);
		fbp = (struct fblk *) dbp1->db_data;
		nfree = fbp->df_nfree;
		canshort(nfree);
		copyb(fsp->s_free, fbp->df_free, sizeof(fsp->s_free));
		dbfree(dbp1, 0);
	}
	--tfree;
	canshort(nfree);
	fsp->s_nfree = nfree;
	candaddr(tfree);
	fsp->s_tfree = tfree;
	dbfree(dbp, DB_DIRT);
	return (bn);
}

/*
 * Quickly zero out a block of
 * memory. Used to clear out disc inodes
 * and other similar things.
 */
zerob(ap, an)
char *ap;
{
	register char *p;
	register n;

	if ((n = an) != 0) {
		p = ap;
		do {
			*p++ = 0;
		} while (--n);
	}
}

/*
 * Quickly move a block of
 * memory from one place to another
 * place.
 */
copyb(atp, afp, an)
char *atp;
char *afp;
{
	register char *tp, *fp;
	register n;

	if ((n = an) != 0) {
		tp = atp;
		fp = afp;
		do {
			*tp++ = *fp++;
		} while (--n);
	}
}

/*
 * Yet another version of
 * the inode mapping code. This version
 * allocates blocks if they are not present
 * in the file. It makes good use of the
 * buffer cache.
 * A pointer to a DISCBUF holding the
 * block is returned. Usually this will be
 * a buffer created by `dbzero'.
 */
dbimap(dip, lb)
struct dinode *dip;
daddr_t lb;
{
	register DISCBUF *dbp;
	register il, newblock;
	daddr_t addr[NADDR];
	daddr_t pb, bpos, *bkp;

	l3tol(addr, dip->di_addr, NADDR);
	for (il=0; il<4; ++il) {
		if (lb < ranges[il]) {
			if (il > 0)
				lb -= ranges[il-1];
			bpos = lb >> shifts[il];
			lb &= masks[il];
			bkp = &addr[(int)bpos + offs[il]];
			newblock = 0;
			if ((pb = *bkp) == 0) {
				newblock = 1;
				*bkp = pb = balloc();
				ltol3(dip->di_addr, addr, NADDR);
			}
			if (pb != 0) {
				while (il-- > 0) {
					if (newblock)
						dbp = dbzero(pb); else
						dbp = dbread(pb);
					bpos = lb >> shifts[il];
					lb &= masks[il];
					bkp = (long *)dbp->db_data + bpos;
					pb = *bkp;
					candaddr(pb);
					if (pb == 0) {
						newblock = 1;
						pb = balloc();
						*bkp = pb;
						candaddr(*bkp);
						dbfree(dbp, DB_DIRT);
					} else {
						newblock = 0;
						dbfree(dbp, 0);
					}
					if (pb == 0)
						break;
				}
			}
			if (pb != 0) {
				if (newblock)
					dbp = dbzero(pb); else
					dbp = dbread(pb);
				return (dbp);
			}
			return (NULL);
		}
	}
	message(0, "fill too large to map");
	return (NULL);
}

/*
 * Write a block.
 */
bwrite(fp, ddp)
register FILE *fp;
register union dumpdata *ddp;
{
	lseek(fileno(fp), BUFSIZ*ddp->dd_block, 0);
	if (write(fileno(fp), ddp->dd_data, ddp->dd_size) != ddp->dd_size)
		return (0);
	return (1);
}

/*
 * Check if any files in the
 * `x' file list remain on this
 * tape (as indicated by the
 * map).
 */
anyfiles()
{
	register ino_t ino;
	register i;

	for (i=0; i<nxf; ++i) {
		if ((ino = xf[i].xf_ino)!=0 && getmap(ino)!=0)
			return (1);
	}
	return (0);
}

/*
 * Read and validate tape header.
 * The `quit' flag is true if errors
 * are fatal.
 */
readhead(quit)
{
	register char *p;
	register checksum;

	if (read(fileno(dtp), &dh, sizeof(dh)) != sizeof(dh)) {
		message(quit, "header read error");
		return (0);
	}
	canint(dh.dh_magic);
	canino(dh.dh_nino);
	cantime(dh.dh_bdate);
	cantime(dh.dh_ddate);
	canint(dh.dh_level);
	canint(dh.dh_reel);
	canint(dh.dh_blocking);
	canint(dh.dh_checksum);
	if (dh.dh_magic != DH_MAG) {
		message(quit, "not a dump");
		return (0);
	}
	p = (char *) &dh;
	checksum = 0;
	while (p < (char *) &dh.dh_checksum)
		checksum += (*p++) & 0377;
	if (checksum != dh.dh_checksum) {
		message(quit, "checksum error");
		return (0);
	}
	if (dh.dh_reel != reel) {
		message(quit, "wrong reel (is %d, not %d)", dh.dh_reel, reel);
		return (0);
	}
	++reel;
	if (map != NULL)
		free(map);
	if (ddbuf != NULL)
		free(ddbuf);
	if ((map = calloc(sizeof(char), dh.dh_nino)) == NULL)
		message(1, "out of memory (map)");
	ddnbuf = dh.dh_blocking * sizeof(union dumpdata);
	if ((ddbuf = malloc(ddnbuf)) == NULL)
		message(1, "out of memory (big buffer)");
	ddend = &ddbuf[ddnbuf];
	ddptr = (union dumpdata *) ddend;
}

/*
 * Read in directories and set up
 * the map. The last tape record is ungotten
 * so that the extract code can be made a
 * little simpler.
 */
readdirs()
{
	register union dumpdata *ddp;
	register struct dlist *dlp;
	int type;
	unsigned short mode;

	while ((ddp = readdump()) != NULL) {
		type = ddp->dd_type;
		canint(type);
		switch (type) {

		case DD_EOT:
			--ddptr;
			return;

		case DD_MAP:
			canino(ddp->dd_ino);
			canint(ddp->dd_nmap);
			setmap(ddp);
			break;

		case DD_INO:
			mode = ddp->dd_dinode.di_mode;
			canshort(mode);
			if ((mode&IFMT) != IFDIR) {
				--ddptr;
				return;
			}
			canino(ddp->dd_ino);
			cansize(ddp->dd_dinode.di_size);
			dlp = (struct dlist *) malloc(sizeof(struct dlist));
			if (dlp == NULL)
				message(1, "out of memory (dlist)");
			dlp->dl_dlp = dlist;
			dlist = dlp;
			if (droot == NULL)
				droot = dlp;
			dlp->dl_ino = ddp->dd_ino;
			dlp->dl_seek = ftell(tfp);
			dlp->dl_size = ddp->dd_dinode.di_size;
			break;

		case DD_DATA:
			canino(ddp->dd_ino);
			candaddr(ddp->dd_block);
			canint(ddp->dd_size);
			if (dlist==NULL || dlist->dl_ino!=ddp->dd_ino)
				message(1, "directory out of sync");
			fseek(tfp, dlp->dl_seek+(BUFSIZ*ddp->dd_block), 0);
			fwrite(ddp->dd_data, sizeof(char), ddp->dd_size, tfp);
			if (ferror(tfp))
				message(1, "directory write error");
			break;

		default:
			message(1, "bad type %d", ddp->dd_type);
		}
	}
}

/*
 * Fill in map.
 */
setmap(ddp)
union dumpdata *ddp;
{
	register char *p1, *p2;
	register nb;

	if ((nb = ddp->dd_nmap) != 0) {
		p1 = &map[ddp->dd_ino-1];
		p2 = ddp->dd_map;
		do {
			*p1++ = *p2++;
		} while (--nb);
	}
}

/*
 * Get map item.
 */
getmap(ino)
ino_t ino;
{
	return (map[ino-1]);
}

/*
 * Read dump file.
 */
union	dumpdata *
readdump()
{
	register nb;
	register c;

	while ((char *) ddptr == ddend) {
		if ((nb = read(fileno(dtp), ddbuf, ddnbuf)) != ddnbuf) {
			if (nb != 0)
				message(1, "dump read error");
			for (;;) {
				fclose(dtp);
				printf("restor: mount reel %d ...", reel);
				while ((c = getchar())!=EOF && c!='\n')
					;
				if (c == EOF)
					delexit(1);
				opendump();
				if (readhead(0) != 0)
					break;
			}
			continue;
		}
		ddptr = (union dumpdata *) ddbuf;
		break;
	}
	return (ddptr++);
}

/*
 * Lookup a file, by name.
 * Return the inumber.
 * This routine only looks in
 * directories. The file may
 * not actually be on the tape.
 */
ino_t
lookup(cp)
register char *cp;
{
	char db[DIRSIZ];
	struct direct dirbuf;
	ino_t ino;
	long seek;
	struct dlist *dlp;

	ino = ROOTIN;
	for (;;) {
		{
			register char *dp;
			register c;

			while ((c = *cp++) == '/')
				;
			if (c == '\0')
				return (ino);
			dp = db;
			for (;;) {
				if (dp < &db[DIRSIZ])
					*dp++ = c;
				if ((c = *cp)=='\0' || c=='/')
					break;
				++cp;
			}
			while (dp < &db[DIRSIZ])
				*dp++ = 0;
		}
		if ((dlp = findnode(ino)) == NULL)
			return (0);
		seek = dlp->dl_seek;
		for (;;) {
			if (seek-dlp->dl_seek >= dlp->dl_size)
				return (0);
			fseek(tfp, seek, 0);
			fread(&dirbuf, sizeof(dirbuf), 1, tfp);
			if (ferror(tfp))
				message(1, "temporary file read error");
			canino(dirbuf.d_ino);
			ino = dirbuf.d_ino;
			if (ino!=0 && strncmp(db, dirbuf.d_name, DIRSIZ)==0)
				break;
			seek += sizeof(struct direct);
		}
	}
}

/*
 * Look for a directory inode
 * in the dlist.
 */
struct	dlist *
findnode(ino)
register ino_t ino;
{
	register struct dlist *dlp;

	dlp = dlist;
	while (dlp != NULL) {
		if (dlp->dl_ino == ino)
			break;
		dlp = dlp->dl_dlp;
	}
	return (dlp);
}

/*
 * Usage message.
 */
usage()
{
	fprintf(stderr, "Usage: restor key [args]\n");
	delexit(1);
}

/*
 * Message output.
 */
message(quit, a)
{
	fprintf(stderr, "restor: %r\n", &a);
	if (quit)
		delexit(1);
}

/*
 * Cleanup function.
 * Called from the interrupt signal.
 */
cleanup()
{
	delexit(1);
}

/*
 * Exit.
 * Delete the temp file, if there.
 */
delexit(s)
{
	if (tfp != NULL)
		unlink(tfn);
	exit(s);
}
