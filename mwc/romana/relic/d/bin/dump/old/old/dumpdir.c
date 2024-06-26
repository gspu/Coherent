/*
 * dumpdir [af [dumpfile]]
 * a	Show all entries (include `.' and `..').
 * f	Use `dumpfile', not default.
 */
#include <stdio.h>
#include <dumptape.h>
#include <canon.h>
#include <mdata.h>
#include <signal.h>

/*
 * Structure used to remember
 * things about all the directories
 * on the tape.
 */
struct	dlist
{
	struct	dlist	*dl_dlp;	/* Link to next */
	ino_t	dl_ino;			/* Inumber */
	int	dl_dejavu;		/* Already seen flag */
	long	dl_seek;		/* Tempfile base */
	long	dl_size;		/* Size */
};

char	*dtn	= DTAPE;		/* Tape name */
FILE	*dtp;				/* Tape file pointer */
char	tfn[30]	= "/tmp/ddXXXXXX";	/* Temp file name */
FILE	*tfp;				/* Temp file pointer */
int	aflag;				/* All flag */
struct	dlist	*dlist;			/* List of remembered directories */
struct	dlist	*droot;			/* Pointer to first directory */
char	dstr[1000];			/* Name string */
char	*dstrp	= &dstr[0];		/* Pointer into the above */
int	reel	= 1;			/* Reel # */
char	*ddbuf;				/* Dump data buffer */
char	*ddend;				/* Ptr to end of dump data buffer */
int	ddnbuf;				/* Size of buffer (bytes) */
union	dumpdata *ddptr;		/* Ptr to current record in buffer */
char	*map;				/* Inode map */

union	dumpdata *readdump();
struct	dlist	 *findnode();
int	cleanup();

main(argc, argv)
char *argv[];
{
	register char *p;
	register c, i;

	if (argc > 1) {
		i = 1;
		p = argv[1];
		while ((c = *p++) != '\0') {
			switch (c) {

			case 'a':
				aflag = 1;
				break;

			case 'f':
				if (++i >= argc)
					usage();
				dtn = argv[i];
				break;

			default:
				usage();
			}
		}
	}
	if ((dtp=fopen(dtn, "r")) == NULL)
		fatal("%s: cannot open dump file", dtn);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, cleanup);
	mktemp(tfn);
	if ((tfp=fopen(tfn, "w")) == NULL
	||  (tfp=freopen(tfn, "r+w", tfp)) == NULL)
		fatal("%s: cannot create temporary file", tfn);
	readhead();
	readdirs();
	if (droot == NULL)
		fatal("no directories");
	dumpdirs(droot);
	delexit(0);
}

/*
 * This routine has two jobs.
 * It reads in the dump file header and
 * checks it out, looking for strangenesses
 * in format. It also is responsible for
 * using information in the header to
 * allocate the necessary map and dump I/O
 * buffers.
 */
readhead()
{
	static struct dumpheader dh;
	register char *p;
	register checksum;

	if (read(fileno(dtp), &dh, sizeof(dh)) != sizeof(dh))
		fatal("header read error");
	canint(dh.dh_magic);
	canino(dh.dh_nino);
	cantime(dh.dh_bdate);
	cantime(dh.dh_ddate);
	canint(dh.dh_level);
	canint(dh.dh_reel);
	canint(dh.dh_blocking);
	canint(dh.dh_checksum);
	if (dh.dh_magic != DH_MAG)
		fatal("not a dump");
	p = (char *) &dh;
	checksum = 0;
	while (p < (char *) &dh.dh_checksum)
		checksum += (*p++) & 0377;
	if (checksum != dh.dh_checksum)
		fatal("header checksum error");
	if (dh.dh_reel != reel)
		fatal("wrong reel (is %d, not %d)", dh.dh_reel, reel);
	++reel;
	if (map != NULL)
		free(map);
	if (ddbuf != NULL)
		free(ddbuf);
	if ((map = calloc(sizeof(char), (dh.dh_nino+NBCHAR-1)/NBCHAR)) == NULL)
		fatal("out of memory (map)");
	ddnbuf = dh.dh_blocking * sizeof(union dumpdata);
	if ((ddbuf = malloc(ddnbuf)) == NULL)
		fatal("out of memory (buffer)");
	ddend = &ddbuf[ddnbuf];
	ddptr = (union dumpdata *) ddend;
}

/*
 * Read in all the directories.
 * This routine assumes that the map entries
 * have been put out before the inodes and
 * that all the directories have been put out
 * before the files.
 */
readdirs()
{
	register union dumpdata *ddp;
	register struct dlist *dlp;

	while ((ddp=readdump()) != NULL) {
		canint(ddp->dd_type);
		switch (ddp->dd_type) {

		case DD_EOT:
			return;

		case DD_DATA:
			canino(ddp->dd_ino);
			candaddr(ddp->dd_block);
			canint(ddp->dd_size);
			if (dlist==NULL || dlist->dl_ino!=ddp->dd_ino)
				fatal("out of sync");
			fseek(tfp, dlist->dl_seek+(BUFSIZ*ddp->dd_block), 0);
			fwrite(ddp->dd_data, sizeof(char), BUFSIZ, tfp);
			if (ferror(tfp))
				fatal("temporary file write error");
			break;

		case DD_INO:
			canino(ddp->dd_ino);
			canshort(ddp->dd_dinode.di_mode);
			cansize(ddp->dd_dinode.di_size);
			if ((ddp->dd_dinode.di_mode&IFMT) != IFDIR)
				return;
			dlp = (struct dlist *) malloc(sizeof(struct dlist));
			if (dlp == NULL)
				fatal("out of memory (directory)");
			dlp->dl_dlp = dlist;
			dlist = dlp;
			if (droot == NULL)
				droot = dlp;
			dlp->dl_ino = ddp->dd_ino;
			dlp->dl_dejavu = 0;
			dlp->dl_seek = ftell(tfp);
			dlp->dl_size = ddp->dd_dinode.di_size;
			break;

		case DD_MAP:
			canino(ddp->dd_ino);
			canint(ddp->dd_nmap);
			setmap(ddp);
			break;

		default:
			fatal("bad type %d", ddp->dd_type);
		}
	}
}

/*
 * Process a map entry.
 * Set a bit in the map for every inode
 * whose dump map entry has the DD_DUMP flag
 * set.
 */
setmap(ddp)
union dumpdata *ddp;
{
	register char *mmapp, *dmapp;
	register mmask;
	register nmapb;

	mmapp = &map[--ddp->dd_ino/NBCHAR];
	mmask = 01 << (ddp->dd_ino%NBCHAR);
	dmapp = &ddp->dd_map[0];
	nmapb = ddp->dd_nmap;
	while (nmapb--) {
		if (((*dmapp++)&DD_DUMP) != 0)
			*mmapp |= mmask;
		if ((mmask <<= 1) == (01<<NBCHAR)) {
			++mmapp;
			mmask = 01;
		}
	}
}

/*
 * Return a pointer to the 
 * next dump file item. This routine
 * knows all about reading the
 * next tape in a multitape dump.
 */
union dumpdata *
readdump()
{
	register nb;

	while ((char *) ddptr == ddend) {
		if ((nb = read(fileno(dtp), ddbuf, ddnbuf)) != ddnbuf) {
			if (nb != 0)
				fatal("dump read error");
			fclose(dtp);
			printf("dumpdir: mount reel %d ...", reel);
			if (fgets(dstr, sizeof(dstr), stdin) == NULL)
				delexit(1);
			if ((dtp = fopen(dtn, "r")) == NULL)
				fatal("%s: cannot open dump tape", dtn);
			readhead();
			continue;
		}
		ddptr = (union dumpdata *) ddbuf;
		break;
	}
	return (ddptr++);
}

/*
 * Start at the root and
 * print all of the directories.
 * Don't print the names of things that
 * the map says are not on the tape.
 */
dumpdirs(dlp)
register struct dlist *dlp;
{
	char *sdstrp;
	struct direct dirbuf;
	register char *p1, *p2;
	register c;
	register struct dlist *sdlp;
	ino_t ino;

	dlp->dl_dejavu = 1;
	while (dlp->dl_size != 0) {
		fseek(tfp, dlp->dl_seek, 0);
		if (fread(&dirbuf, sizeof(dirbuf), 1, tfp) != 1)
			fatal("temporary file read error");
		dlp->dl_seek += sizeof(struct direct);
		dlp->dl_size -= sizeof(struct direct);
		if (aflag == 0) {
			if (strncmp(dirbuf.d_name, ".",  DIRSIZ) == 0)
				continue;
			if (strncmp(dirbuf.d_name, "..", DIRSIZ) == 0)
				continue;
		}
		canino(dirbuf.d_ino);
		if ((ino = dirbuf.d_ino)!=0 && getbit(ino)!=0) {
			p1 = sdstrp = dstrp;
			if (p1 != dstr)
				*p1++ = '/';
			p2 = dirbuf.d_name;
			while (p2<&dirbuf.d_name[DIRSIZ] && (c=*p2++))
				*p1++ = c;
			*p1 = '\0';
			dstrp = p1;
			printf("%u\t%s\n", ino, dstr);
			if ((sdlp=findnode(ino))!=NULL && sdlp->dl_dejavu==0)
				dumpdirs(sdlp);
			dstrp = sdstrp;
			*dstrp = '\0';
		}
	}
	dlp->dl_dejavu = 0;
}

/*
 * Get the dumped bit for
 * inode `ino' from the big bit map.
 */
getbit(ino)
ino_t ino;
{
	register mapent;

	mapent = map[--ino/NBCHAR];
	return (mapent & (01<<ino%NBCHAR));
}

/*
 * Return the dlist pointer
 * for directory inode `ino', or NULL
 * if it is not there. Not there means
 * not a directory.
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
 * Nasty nasty nasty.
 */
fatal(a)
{
	fprintf(stderr, "dumpdir: %r", &a);
	fprintf(stderr, "\n");
	delexit(1);
}

/*
 * This routine is called from the
 * interupt signal. It points off to dumpdir
 * who deletes the temp file and returns
 * the error status.
 */
cleanup()
{
	delexit(1);
}

/*
 * A special version of `exit' that
 * cleans up by unlinking the temporary
 * file.
 */
delexit(s)
{
	if (tfp != NULL)
		unlink(tfn);
	exit(s);
}

/*
 * Print the usage message and
 * exit.
 */
usage()
{
	fprintf(stderr, "Usage: dumpdir [af [dumpfile]]\n");
	exit(1);
}
