/*
 * Dump a filesystem onto a
 * tape. Dump only those files that
 * have checked since the last
 * dump.
 */
#include <stdio.h>
#include <types.h>
#include <discbuf.h>
#include <dumptape.h>
#include <canon.h>
#include <filsys.h>

char	*dtn	= DTAPE;		/* Tape name */
FILE	*dtp;				/* Tape file pointer */
char	*fsn	= DFSYS;		/* File system name */
int	block	= 20;			/* Blocking factor */
int	density	= 1600;			/* Tape density, bpi */
long	length	= 2300L*12L*100L;	/* Length of tape, inches/100 */
long	used;				/* Tape used, inches/100 */
struct	dumpheader dh;			/* Dump header */
int	reel	= 1;			/* Reel # */
int	uflag	= 1;			/* Update dates file flag */
int	vflag;				/* Verbose flag */
int	Sflag;				/* Set when sizes are in blocks */
int	level	= 9;			/* Dump level */
time_t	bdate;				/* Date at beginning of dump */
time_t	ddate;				/* Dump since date */
char	*map;				/* Map pointer */
char	*ddbuf;				/* Dump data buffer */
char	*ddend;				/* Dump data buffer end pointer */
int	ddnbuf;				/* Buffer size */
union	dumpdata *ddptr;		/* Pointer to current item */
ino_t	isize;				/* # of inodes */
char	dev[DIRSIZ+1];			/* Name of device, squashed */
char	fname[6];			/* File system name */
char	fpack[6];			/* File system pack name */
long	ninodes;			/* # of inodes dumped */
long	nblocks;			/* # of blocks of data */
long	nsparse;			/* # of sparse blocks deleted */
char	helpmessage[]		= "\
Usage: dump [bdfsuvS0123456789 [args]] filesystem ...\n\
";

/*
 * Tables used by imap.
 * This effectively implements
 * the access polynomial for the indirect
 * blocks.
 */
daddr_t	ranges[]	= {
	ND,
	ND+(daddr_t)1*NBN,
	ND+(daddr_t)1*NBN + (daddr_t)1*NBN*NBN,
	ND+(daddr_t)1*NBN + (daddr_t)1*NBN*NBN + (daddr_t)1*NBN*NBN*NBN
};

char	offsets[]	= {
	0,
	ND,
	ND+1,
	ND+1+1
};

daddr_t	coeff[]		= {
	1,
	(daddr_t) NBN,
	(daddr_t) NBN*NBN,
	(daddr_t) NBN*NBN*NBN
};

/*
 * One pass compilers are
 * such fun.
 */
long	time();
long	ddlen();
union	dumpdata *ddnextbuf();
long	getddate();
char	*ctime();
daddr_t	imap();
char	*calloc();

main(argc, argv)
char *argv[];
{
	register DISCBUF *dbp;
	register struct filsys *fsp;
	unsigned fi;

	init(argc, argv);
	if (*fsn == 0)
		fatal("no filesystem specified");
	if ((dbfp=fopen(fsn, "r")) == NULL)
		fatal("%s: cannot open filesystem", fsn);
	if ((dtp=fopen(dtn, "w")) == NULL)
		fatal("%s: cannot open dump file",  dtn);
	squash(dev, fsn);
	bdate = time((long *) 0);
	ddate = getddate();
	if (vflag) {
		if (ddate == 0)
			fprintf(stderr, "dump: since epoch\n");
		else
			fprintf(stderr, "dump: since %s", ctime(&ddate));
	}
	dbclaim(5);
	sync();
	dbp = dbread((long) SUPERI);
	fsp = (struct filsys *) (dbp->db_data);
	fi = fsp->s_isize;
	canshort(fi);
	isize = (fi-INODEI)*INOPB;
	strncpy(fname, fsp->s_fname, sizeof(fname));
	strncpy(fpack, fsp->s_fpack, sizeof(fpack));
	dbfree(dbp, 0);
	if ((map=calloc(isize, sizeof(char))) == NULL)
		fatal("out of memory (map)");
	walk((ino_t) ROOTIN);
	nextvol();
	dumpinomap();
	dump(DD_DIR);		/* Directories */
	dump(0);		/* Files */
	dumpfinish();
	if (uflag)
		putddate();
	if (vflag) {
		if (Sflag)
			used /= 512; else
			used /= 100*12;
		fprintf(stderr, "dump: length is %D ", used);
		if (Sflag)
			fprintf(stderr, "blocks\n");
		else if (used == 1)
			fprintf(stderr, "foot\n");
		else
			fprintf(stderr, "feet\n");
		fprintf(stderr, "dump: %D inodes, ",  ninodes);
		fprintf(stderr, "%D real blocks, ",   nblocks);
		fprintf(stderr, "%D sparse blocks\n", nsparse);
	}
	exit(0);
}

/*
 * Scan the command line and
 * set options. Perhaps reset the name
 * of the filesystem, the dump tape
 * and the dump origin.
 */
init(argc, argv)
char *argv[];
{
	register char *p;
	register c, i;

	if (argc > 1) {
		uflag = 0;
		i = 1;
		p = argv[1];
		while ((c = *p++) != '\0') {
			switch (c) {

			case 'f':
				if (++i >= argc)
					usage();
				dtn = argv[i];
				break;

			case 'u':
				uflag = 1;
				break;

			case 'v':
				vflag = 1;
				break;

			case 's':
				if (++i >= argc)
					usage();
				length = (long)12*100*num(argv[i], "length",
					600, 4800);
				break;

			case 'S':
				if (++i >= argc)
					usage();
				Sflag++;
				length = (long)512 * num(argv[i], "block size",
					0, (unsigned)65535L);
				break;

			case 'b':
				if (++i >= argc)
					usage();
				block = num(argv[i], "blocking", 1, 20);
				break;

			case 'd':
				if (++i >= argc)
					usage();
				density = num(argv[i], "density", 800, 6250);
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				level = c-'0';
				break;

			default:
				usage();
			}
		}
		if (++i < argc)
			fsn = argv[i];
	}
}

/*
 * Collect and range check
 * numeric arguments.
 */
num(s, msg, ll, hl)
register char *s;
char *msg;
unsigned ll, hl;
{
	register c, n;

	n = 0;
	while ((c=*s++)>='0' && c<='9')
		n = 10*n + c - '0';
	if (c!='\0' || n<ll || n>hl)
		fatal("bad %s specification", msg);
	return (n);
}

/*
 * Walk down the file system,
 * looking for files that should be
 * dumped. Mark them in the map.
 * All directories that lead to changed
 * inodes are forced to be dumped.
 */
walk(ino)
ino_t ino;
{
	register DISCBUF *dbp;
	register struct direct *dirp;
	int flag, nent, ndir;
	long pb, dirb;
	struct dinode dinode;
	ino_t ent[BUFSIZ/sizeof(struct direct)];

	if ((map[ino-1]&DD_DEJA) != 0)
		return (0);
	ireadin(&dinode, ino);
	canshort(dinode.di_mode);
	cantime(dinode.di_ctime);
	/*
	 * Normally pipes stay in core,
	 * and are not in directories. Just to be
	 * safe we pretend any pipe is a clear
	 * inode.
	 */
	if ((dinode.di_mode&IFMT) == IFPIPE)
		return (0);
	map[ino-1] |= DD_BUSY;
	if ((dinode.di_mode&IFMT) != IFDIR) {
		if (level==0 || dinode.di_ctime>ddate) {
			map[ino-1] |= DD_DUMP;
			return (1);
		}
		return (0);
	}
	map[ino-1] |= DD_DEJA|DD_DIR;
	flag = dirb = 0;
	cansize(dinode.di_size);
	while (dinode.di_size != 0) {
		if ((pb = imap(&dinode, dirb++)) == 0) {
			dinode.di_size -= BUFSIZ;
			continue;
		}
		/*
		 * Writing the code is this
		 * strange fashon makes it require 1 cache
		 * buffer, not 1 per level.
		 */
		dbp = dbread(pb);
		nent = 0;
		ndir = BUFSIZ / sizeof(struct direct);
		if (dinode.di_size < BUFSIZ)
			ndir = dinode.di_size / sizeof(struct direct);
		dirp = (struct direct *) (dbp->db_data);
		while (ndir--) {
			if (dirp->d_ino != 0) {
				ent[nent] = dirp->d_ino;
				canino(ent[nent]);
				++nent;
			}
			++dirp;
			dinode.di_size -= sizeof(struct direct);
		}
		dbfree(dbp, 0);
		while (nent--) {
			if (walk(ent[nent]))
				flag = 1;
		}
	}
	if (flag!=0 || level==0 || dinode.di_ctime>ddate)
		map[ino-1] |= DD_DUMP;
	map[ino-1] &= ~DD_DEJA;
	return (flag);
}

/*
 * Read in an inode.
 * Leave it in the buffer in disc
 * (that is, cannonical) format.
 */
ireadin(diop, ino)
register struct dinode *diop;
ino_t ino;
{
	register struct dinode *diip;
	DISCBUF *dbp;

	dbp = dbread((long)((ino-1)/INOPB + INODEI));
	diip = (struct dinode *)(dbp->db_data) + (ino-1)%INOPB;
	*diop = *diip;		/* Structure assignment */
	dbfree(dbp, 0);
}

/*
 * Map logical block `lb' in the
 * file whose in core inode is pointed to
 * by `ip' to the associated physical
 * block. Return 0 if the block will not
 * map.
 */
daddr_t
imap(ip, lb)
register struct dinode *ip;
daddr_t lb;
{
	register il;
	daddr_t bpos, pb;
	DISCBUF *dbp;
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
				while (il-- > 0) {
					dbp = dbread((long) pb);
					bpos = lb/coeff[il];
					lb %= coeff[il];
					bp = (daddr_t *)(dbp->db_data) + bpos;
					dbfree(dbp, 0);
					if ((pb = *bp) == 0)
						break;
					candaddr(pb);
				}
			}
			return (pb);
		}
	return (0);
}


/*
 * Dump out the map.
 * The map is packed into ordinary
 * tape blocks.
 */
dumpinomap()
{
	register ino_t ibase;
	register char *imapp;
	register nmap;
	union dumpdata *ddp;

	ibase = 0;
	while ((nmap = isize-ibase) > 0) {
		ddp = ddnextbuf();
		ddp->dd_type = DD_MAP;
		ddp->dd_ino = ibase+1;
		if (nmap > BUFSIZ)
			nmap = BUFSIZ;
		ddp->dd_nmap = nmap;
		canint(ddp->dd_type);
		canino(ddp->dd_ino);
		canint(ddp->dd_nmap);
		imapp = &ddp->dd_map[0];
		do {
			*imapp++ = map[ibase++];
		} while (--nmap);
	}
}

/*
 * Fabricate a dump header and
 * write it to the dump file. All the
 * stuff in the header is in the
 * usual canonical format. This makes it
 * a little easier to move tapes from
 * one machine to another.
 */
dumpheader()
{
	register char *p;
	register checksum;

	dh.dh_magic = DH_MAG;
	dh.dh_nino = isize;
	dh.dh_bdate = bdate;
	dh.dh_ddate = ddate;
	dh.dh_level = level;
	dh.dh_reel = reel++;
	dh.dh_blocking = block;
	dh.dh_nbyte = Sflag ? length : 0;
	strncpy(dh.dh_dev, dev, DIRSIZ);
	strncpy(dh.dh_fname, fname, 6);
	strncpy(dh.dh_fpack, fpack, 6);
	p = (char *) &dh;
	checksum = 0;
	while (p < (char *) &dh.dh_checksum)
		checksum += (*p++) & 0377;
	dh.dh_checksum = checksum;
	canint(dh.dh_magic);
	canino(dh.dh_nino);
	cantime(dh.dh_bdate);
	cantime(dh.dh_ddate);
	canint(dh.dh_level);
	canint(dh.dh_reel);
	canint(dh.dh_blocking);
	cansize(dh.dh_nbyte);
	canint(dh.dh_checksum);
	if (write(fileno(dtp), &dh, sizeof dh) != sizeof dh)
		fatal("header write error");
}

/*
 * Put out the end of tape item
 * and the very end of the very last
 * dump tape.
 */
dumpfinish()
{
	register union dumpdata *ddp;

	ddp = ddnextbuf();
	ddp->dd_type = DD_EOT;
	canint(ddp->dd_type);
	ddflush();
}

/*
 * Dump out all inodes with
 * the appropriate flags set in the
 * map.
 */
dump(flag)
register flag;
{
	register ino_t i;
	register mapentry;

	for (i=0; i<isize; ++i) {
		mapentry = map[i];
		if ((mapentry&DD_DUMP)!=0 && (mapentry&DD_DIR)==flag) {
			dumpi(i+1);
			map[i] = 0;
		}
	}
}

/*
 * Dump an inode.
 */
dumpi(ino)
ino_t ino;
{
	struct dinode dinode;
	register union dumpdata *ddp;
	register DISCBUF *dbp;
	daddr_t lb, pb;
	int size;

	++ninodes;
	ireadin(&dinode, ino);
	ddp = ddnextbuf();
	ddp->dd_type = DD_INO;
	ddp->dd_ino  = ino;
	canint(ddp->dd_type);
	canino(ddp->dd_ino);
	copyb(&ddp->dd_dinode, &dinode, sizeof(struct dinode));
	lb = 0;
	cansize(dinode.di_size);
	while (dinode.di_size != 0) {
		pb = imap(&dinode, lb);
		size = dinode.di_size<BUFSIZ ? dinode.di_size : BUFSIZ;
		if (pb != 0) {
			dbp = dbread(pb);
			if (allzeros(dbp->db_data, size) == 0) {
				++nblocks;
				ddp = ddnextbuf();
				ddp->dd_type = DD_DATA;
				ddp->dd_ino  = ino;
				ddp->dd_block = lb;
				ddp->dd_size = size;
				canint(ddp->dd_type);
				canino(ddp->dd_ino);
				candaddr(ddp->dd_block);
				canint(ddp->dd_size);
				copyb(ddp->dd_data, dbp->db_data, size);
			} else
				++nsparse;
			dbfree(dbp, 0);
		}
		dinode.di_size -= size;
		++lb;
	}
}

/*
 * Check if a disc block is all
 * zero bytes. True return if it is.
 */
allzeros(cp, nb)
register char	*cp;
register int	nb;
{
	do {
		if (*cp++ != 0)
			return (0);
	} while (--nb);
	return (1);
}

/*
 * Copy a block of bytes.
 */
copyb(atp, afp, anb)
char *atp, *afp;
{
	register char *tp, *fp;
	register nb;

	if ((nb = anb) != 0) {
		tp = atp;
		fp = afp;
		do {
			*tp++ = *fp++;
		} while (--nb);
	}
}

/*
 * Return a pointer to the
 * next data buffer.
 */
union	dumpdata *
ddnextbuf()
{
	register char *p;
	register n;

	if (ddbuf == NULL) {
		ddnbuf = block * sizeof(union dumpdata);
		if ((ddbuf = malloc(ddnbuf)) == NULL)
			fatal("out of memory (buffer)");
		ddend = &ddbuf[ddnbuf];
		ddptr = (union dumpdata *) ddbuf;
	}
	if ((char *) ddptr == ddend) {
		ddflush();
		ddptr = (union dumpdata *) ddbuf;
	}
	if ((char *) ddptr == ddbuf) {
		p = ddbuf;
		n = ddnbuf;
		do {
			*p++ = 0;
		} while (--n);
	}
	return (ddptr++);
}

/*
 * Write out the big data buffer.
 */
ddflush()
{
	used += ddlen(ddnbuf);
	if (used >= length) {
		nextvol();
		used += ddlen(ddnbuf);
	}
	if (write(fileno(dtp), ddbuf, ddnbuf) != ddnbuf)
		fatal("dump write error");
}

/*
 * Request next dump volume.
 */
nextvol()
{
	char buf[40];

	if (Sflag != 0 || reel != 1) {
		fclose(dtp);
		fprintf(stderr, "dump: mount %s %d, type return key ...",
			Sflag ? "volume" : "reel", reel);
		if (gets(buf) == NULL)
			exit(1);
		if ((dtp = fopen(dtn, "w")) == NULL)
			fatal("%s: cannot open dump file", dtn);
	}
	dumpheader();
	used = ddlen(sizeof(dh));
}

/*
 * Given a record size in bytes,
 * return the number of inches/100 of tape
 * that will get used.
 * For blocked devices, the granularity is
 * actually in bytes.
 */
long
ddlen(nbytes)
{
	if (Sflag)
		return (nbytes);
	return (100L*((nbytes+density-1)/density) + 75);
}

/*
 * Update the dump date in the
 * dump date file. The date used is the
 * date at the beginning of the dump.
 */
putddate()
{
	register FILE *ddfp;
	struct idates id;

	if ((ddfp = fopen(DDATE, "r+w")) == NULL) {
		fprintf(stderr, "dump: cannot update dump date\n");
		return;
	}
	while (fread(&id, sizeof(id), 1, ddfp) == 1) {
		if (strncmp(id.id_name, dev, DIRSIZ) == 0
		&& id.id_incno==level) {
			fseek(ddfp, (long)-sizeof(id), 1);
			break;
		}
	}
	strncpy(id.id_name, dev, DIRSIZ);
	id.id_incno = level;
	id.id_ddate = bdate;
	if (fwrite(&id, sizeof(id), 1, ddfp) != 1)
		fprintf(stderr, "dump: error updating date file\n");
	fclose(ddfp);
}

/*
 * Get dump date.
 */
long
getddate()
{
	register FILE *ddfp;
	register long ddate;
	struct idates id;

	ddate = 0;
	if ((ddfp = fopen(DDATE, "r")) != NULL) {
		while (fread(&id, sizeof(id), 1, ddfp) == 1) {
			if (strncmp(id.id_name, dev, DIRSIZ)==0
			&& id.id_incno<level
			&& id.id_ddate>ddate)
				ddate = id.id_ddate;
		}
		fclose(ddfp);
	}
	return (ddate);
}

/*
 * Print out a fatal diagnostic
 * and die.
 */
fatal(a)
{
	fprintf(stderr, "dump: %r", &a);
	fprintf(stderr, "\n");
	exit(1);
}

/*
 * Print out a usage message and
 * exit. If you can figure out how to use
 * dump from the usage message you are
 * a better man than I.
 */
usage()
{
	fprintf(stderr, helpmessage);
	exit(1);
}

/*
 * Squash off any structure
 * before the last part of a file
 * name and put it in the supplied
 * buffer.
 */
squash(tp, fp)
register char *tp, *fp;
{
	register char *np;

	np = fp;
	while (*fp != '\0') {
		if (*fp == '/')
			np = fp+1;
		++fp;
	}
	while (*tp++ = *np++)
		;
}
