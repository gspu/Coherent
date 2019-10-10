/* dos1.c */
/* Globals and functions common to the "dos" and "dosshrink" commands. */

#include "dos0.h"

/* Diskette parameters. */
BPB	d8floppy  = { 512, 2, 1, 2, 112,  640, 0xFF, 1,  8, 2, 0 };
BPB	d9floppy  = { 512, 2, 1, 2, 112,  720, 0xFD, 2,  9, 2, 0 };
BPB	d15floppy = { 512, 1, 1, 2, 224, 2400, 0xF9, 7, 15, 2, 0 };
BPB	d18floppy = { 512, 1, 1, 2, 224, 2880, 0xF0, 9, 18, 2, 0 };
BPB	q9floppy  = { 512, 2, 1, 2, 112, 1440, 0xF9, 3,  9, 2, 0 };
BPB	s8floppy  = { 512, 1, 1, 2,  64,  320, 0xFE, 1,  8, 1, 0 };
BPB	s9floppy  = { 512, 1, 1, 2,  64,  360, 0xFC, 2,  9, 1, 0 };

/* Globals. */
char		*argv0;			/* Command name			*/
unsigned char	bootb[BBSIZE];		/* Boot block			*/
BPB		*bpb;			/* Disk parameters		*/
short		cflag;			/* Readonly			*/
unsigned char	*clbuf;			/* One cluster buffer		*/
unsigned short	clsize;			/* Sectors per cluster		*/
unsigned short	dirbase;		/* First root directory sector	*/
unsigned short	dirsize;		/* Root dir size in sectors	*/
unsigned short	fatbase;		/* First FAT sector		*/
unsigned short	fatbytes;		/* Bytes per FAT entry (1 means 1.5)*/
unsigned short	*fatcache;		/* File allocation table cache	*/
unsigned short	fatccount;		/* Sectors in FAT cache		*/
unsigned short	fatcfirst;		/* First sector in FAT cache	*/
short		fatcflag;		/* FAT must be written		*/
unsigned short	fatcmax;		/* Max cluster in FAT cache	*/
unsigned short	fatcmin;		/* Min cluster in FAT cache	*/
unsigned short	fatsize;		/* FAT size in sectors		*/
unsigned short	filebase;		/* First disk file data sector	*/
short		fsfd;			/* File system file descriptor	*/
unsigned short	heads;			/* Heads			*/
unsigned short	maxcluster;		/* Max cluster number		*/
unsigned short	mdirsize;		/* MDIRs per cluster		*/
unsigned short	nspt;			/* Sectors per track		*/
unsigned short	sectors;		/* Sectors			*/
unsigned short	ssize;			/* Sector size			*/
char		*usagemsg;		/* Usage message		*/
short		vflag;			/* Verbose			*/

/*
 * Convert cluster number n to a block number.
 * Complain if the cluster number is out of range.
 */
unsigned long
cltosec(n) register unsigned short n;
{
	if (n < 2 || n > maxcluster)
		fatal("cluster number %u out of range", n);
	return ((long)(n - 2) * (long)clsize + filebase);
}

/*
 * Decode or encode a FAT with 1.5-byte entries.
 * The FAT is already in the FAT cache and the cache is big enough
 * for the expanded version with 2-byte entries.
 * The flag is 0 to decode after read, 1 to encode before write.
 * This lazy code uses short pointers on char boundaries.
 */
void
decodefat(flag) short flag;
{
	register unsigned short i;
	register unsigned short u, *ip;
	register unsigned char *cp;

	cp = fatcache;
	if (flag == 0) {
		/* Expand 1.5-byte entries to unsigned shorts. */
		for (i = maxcluster; ; i--) {
			ip = &cp[i*3/2];
			fatcache[i] = *ip;
			if (i & 1)
				fatcache[i] >>= 4;
			else
				fatcache[i] &= FATMASK;
			if (fatcache[i] > (CLMAX & FATMASK))
				fatcache[i] |= 0xF000;
			if (i == 0)
				break;
		}
	} else {
		/* Compress the FAT table to 1.5-byte entries. */
		for (i = 0; i <= maxcluster; i++) {
			u = fatcache[i] & FATMASK;
			ip = &cp[i*3/2];
			if (i & 1) {
				*ip &= ~(FATMASK << 4);
				*ip |= u << 4;
			} else {
				*ip &= ~FATMASK;
				*ip |= u;
			}
		}
	}
}

/*
 * Read size sectors starting at block n into buf.
 * Failure is fatal.
 */
void
diskread(buf, n, size, msg) char *buf; unsigned long n; short size; char *msg;
{
	diskseek(n);
	while (size-- > 0) {
		if (read(fsfd, buf, ssize) != ssize)
			fatal("%s read error", msg);
		buf += ssize;
	}
}

/*
 * Seek to the specified sector on the disk.
 */
void
diskseek(n) unsigned long n;
{
	if (lseek(fsfd, partseek + n * ssize, 0) == -1L)
		fatal("seek failed 0x%lx", n * ssize);
}

/*
 * Write size sectors starting at block n from buf.
 * Failure is fatal.
 */
void
diskwrite(buf, n, size, msg) char *buf; unsigned long n; short size; char *msg;
{
	if (cflag)
		return;
	diskseek(n);
	while (size--) {
		if (write(fsfd, buf, ssize) != ssize)
			fatal("%s write error", msg);
		buf += ssize;
	}
}

/*
 * Cry and die.
 * Uses the nonportable "%r" format.
 */
void
fatal(x) char *x;
{
	fflush(stdout);
	fprintf(stderr, "%s: %r\n", argv0, &x);
	rm_lock();
	exit(1);
}

void
fatal1(x) char *x;
{
	fflush(stdout);
	fprintf(stderr, "%s: %r\n", argv0, &x);
	exit(1);
}

/*
 * Flush the FAT cache.
 */
void
fatcflush()
{
	register unsigned short i, n;

	if (fatcflag)
		for (n = fatbase + fatcfirst, i = 1;
		     i <= bpb->b_fats;
		     n += fatsize, i++)
			diskwrite(fatcache, (long)n, fatccount, "FAT cache");
	fatcflag = 0;
}

/*
 * Read segment of FAT including cluster n into the FAT cache.
 */
void
fatcread(n) register unsigned short n;
{
	fatcflush();				/* flush previous contents */
	fatcflag = 0;				/* clear dirty flag */
	fatcfirst = (n/FATCCOUNT) * FATCSECS;	/* first FAT sector to read */
	fatccount = FATCSECS;			/* sectors to read */
	if (fatcfirst + fatccount > fatsize)
		fatccount = fatsize - fatcfirst;	/* read less at end */
	fatcmin = fatcfirst * FATCNPSEC;	/* min cluster in cache */
	fatcmax = (fatcfirst + fatccount) * FATCNPSEC - 1;	/* max cl */
/*
printf("n=%d,fatbase=%d,fatcfirst=%d,(long)(fatbase+fatcfirst)=%d,fatccount=%d\n",
n,fatbase,fatcfirst,(long)(fatbase + fatcfirst),fatccount);
*/
	diskread(fatcache, (long)(fatbase + fatcfirst),fatccount,"FAT cache");
}

/*
 * Get the FAT entry for cluster n.
 */
unsigned short
getcluster(n) register unsigned short n;
{
	if (n < 2 || n > maxcluster)
		fatal("getcluster: bad cluster number %u", n);
	if (n < fatcmin || n > fatcmax)
		fatcread(n);
	return fatcache[n - fatcmin];
}

/*
 * Convert up to n characters from src (an MS-DOS filename or extension)
 * to dst (a COHERENT filename buffer),
 * mapping to lower case and stopping at ' '.
 * Return a pointer to the NUL terminator.
 */
char *
lcname(dst, src, n) unsigned char *dst, *src; register short n;
{
	register short c;

	while (n--) {
		if ((c = *src++) == ' ')
			break;
		else if (isupper(c))
			c = tolower(c);
		*dst++ = c;
	}
	*dst = '\0';
	return dst;
}

/*
 * Put val in the FAT entry for cluster n.
 */
void
putcluster(n, val) unsigned short n; register short val;
{
	if (n < 2 || n > maxcluster)
		fatal("putcluster: bad cluster number %u", n);
	if (n < fatcmin || n > fatcmax)
		fatcread(n);
	fatcache[n - fatcmin] = val;
	fatcflag = 1;
}

/*
 * Read the file allocation table.
 * Try the rational approach:
 * read the boot block and use the boot block BPB info.
 * This being MS-DOS, the rational approach sometimes loses,
 * because the BPB information is not always there;
 * try e.g. DOS 3.2 "format a: /8".
 * Hence, the somewhat backhanded approach below.
 */
void
readfat()
{
	register unsigned short i;
	unsigned char id[3];
	char *s;

	if (xpart != 0)
		xpartition();

	/* Read the boot block and hope its BPB is rational. */
	lseek(fsfd, partseek, 0);
	if (read(fsfd, bootb, BBSIZE) != BBSIZE)
		fatal("boot block read error");
	bpb = &bootb[BPBOFF];
	if (is_media_id(bpb->b_media))
		s = "Found";
	else {
		/*
		 * The boot block BPB is missing,
		 * check for media id at start of block 1.
		 */
		lseek(fsfd, (long)BBSIZE, 0);
		if (read(fsfd, id, 3) != 3)
			fatal("media id read error");
		i = id[0];
		if ((i!=0xFC && i!=0xFD && i!=0xFE && i!=0xFF)
		  || id[1]!=0xFF || id[2]==0xFF)
			fatal("Probably not a DOS disk (media descriptor 0x%02x)", id[0]);
		/* Known diskette format. */
		s = "Known";
		switch (i) {
		case 0xFC:	bpb = &s9floppy;	break;
		case 0xFD:	bpb = &d9floppy;	break;
		case 0xFE:	bpb = &s8floppy;	break;
		case 0xFF:	bpb = &d8floppy;	break;
		}
	}
	setglobals();
#if	DEBUG
	printf("%s BPB: %u %u %u %u %u %u 0x%x %u %u %u %u\n",
		s,
		ssize,
		clsize,
		fatbase,
		bpb->b_fats,
		bpb->b_files,
		sectors,
		bpb->b_media,
		fatsize,
		nspt,
		heads,
		bpb->b_hidden);
#endif
	/*
	 * Allocate space for a FAT cache.
	 * If the FAT contains 1.5-byte entries, the cache must be big
	 * enough for the entire FAT table.
	 * Read start of FAT for sanity check below.
	 */
	if ((fatcache = malloc(FATCSECS * ssize)) == NULL)
		fatal("FAT allocation failed");
	if (fatbytes == 1) {		/* 1.5-byte FAT entries */
		if (fatsize > FATCSECS)
			fatal("FAT cache botch fatsize=%d", fatsize);
		fatcread(0);
		fatcmax = (fatsize * ssize * 2 / 3) - 1;
		if (fatcmax < maxcluster)
			fatal("FAT cache botch max=%d", fatcmax);
		decodefat(0);		/* expand entries to two bytes each */
	} else if (fatbytes == 2)	/* 2-byte FAT entries */
		fatcread(0);
	else				/* 4-byte FAT entries, presumably */
		fatal("fatbytes=%u", fatbytes);

	/* Sanity check. */
	if (fatcache[0] != (0xFF00 | bpb->b_media) || fatcache[1] != 0xFFFF)
		fatal("bad MS-DOS diskette format 0x%x 0x%x", fatcache[0], fatcache[1]);
}

/*
 * Set globals based on values in BPB.
 * This is mostly for efficiency.
 */
void
setglobals()
{
	ssize = bpb->b_ssize;
	clsize = bpb->b_clsize;
	fatbase = bpb->b_reserved;
	sectors = bpb->b_sectors;
	fatsize = bpb->b_fatsize;
	nspt = bpb->b_tracks;
	heads = bpb->b_heads;

	/* This program is untested for sector size != 512. */
	/* It might work, it might not; let's be prudent. */
	if (ssize != BBSIZE)
		fatal("ssize=%u BBSIZE=%u", ssize, BBSIZE);

	/* Compute base file sector, number of clusters, etc. */
	dirbase = fatbase + fatsize * bpb->b_fats;
	dirsize = (bpb->b_files * sizeof(MDIR) + ssize - 1) / ssize;
	filebase = dirbase + dirsize;
	mdirsize = clsize * ssize / sizeof(MDIR);

	if (sectors == 0) {
		fatbytes = 2;
		maxcluster=1+(unsigned short)((bpb->b_bigsectors-filebase)/clsize);
	}
	else {
		fatbytes = (sectors / clsize > (FATMASK & CLMAX)) ? 2 : 1;
		maxcluster = 1 + (sectors - filebase) / clsize;
	}

	dbprintf(("dirbase=%u dirsize=%u fatbytes=%u filebase=%u maxcluster=%u mdirsize=%u\n", dirbase, dirsize, fatbytes, filebase, maxcluster, mdirsize));
}

/*
 * Print usage message and die.
 */
void
usage(msg) char *msg;
{
	fprintf(stderr, usagemsg);
	if (msg)
		fatal(msg);
	else
		exit(1);
}

/*
 * Write the file allocation table.
 * If fatbytes==1, the FAT is compressed into 1.5-byte entries;
 * it is not directly usable after compression.
 */
void
writefat()
{
	if (fatbytes == 1)
		decodefat(1);
	fatcflush();
}

/*
 * If the desired MS-DOS filesystem is an extended MS-DOS partition,
 * find the seek into the COHERENT partition
 * which specifies the base of the actual MS-DOS filessytem.
 */
void
xpartition()
{
	register short part;
	HDISK_S hd;
	FDISK_S *p1, *p2;

	partseek = 0L;
	for (part = 1; part <= xpart; ++part) {
		if (lseek(fsfd, partseek, 0) == -1L)
			fatal("extended partition lseek failed part=%d seek=%lx",
				part, partseek);
		if (read(fsfd, &hd, sizeof hd) != sizeof hd)
			fatal("extended partition table %d read failed", part);
		if (hd.hd_sig != HDSIG)
			fatal("no signature found on extended partition table %d",
				part);
		p1 = &(hd.hd_partn[0]);
		p2 = &(hd.hd_partn[1]);
		if (part == xpart) {
			partseek += p1->p_base * BBSIZE;
			break;
		} else if (p2->p_sys == SYS_DOS_XP)
			partseek = p2->p_base * BBSIZE;
		else
			fatal("extended MS-DOS partition does not contain %d drives",
				xpart);
	}
}

/* end of dos1.c */

