/*
 * mkfs.c
 * 7/14/92
 * Make a filesystem.
 * Efficiently, rec 84.08.31
 */

#define	VERSION	"1.2"
#define NDEBUG		1	/* No assertions turned on */
#define ES_SUCCESS	0	/* No problems at all */
#define ES_IGNORED	1	/* Some problems ignored */
#define ES_FORMAT	2	/* Proto file format error */
#define ES_FATAL	4	/* Process aborted */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#define syserror	(sys_errlist[errno])
#include <sys/filsys.h>
#include <sys/ino.h>
#include <sys/fblk.h>
#include <sys/dir.h>
#include <l.out.h>
#include <sys/timeb.h>
#include <canon.h>
#include <sys/stat.h>
#include <access.h>
#include <assert.h>
#include <string.h>
#include <sys/mdata.h>

#define inodeb(i)	(INODEI + ((i)-1)/8)
#define inodei(i)	(((i)-1)%8)
#define	NDLEV	25			/* Maximum directory nesting */

char s755[20];
char sB[20];

struct protoargs {	/* Parameter management */
	char	*p_bname, *p_fname, *p_fpack;
	char	*p_fsize, *p_nino, *p_intn, *p_intm;
};

struct entre {		/* Directory management */
	ino_t	e_ino;
	char	*e_name;
};

struct xnode {		/* Minimal inode management */
	ino_t	x_ino;
	int	x_mode;
	int	x_uid;
	int	x_gid;
	dev_t	x_dev;
	char	*x_name;
	int	x_nlink;
	fsize_t	x_size;
	time_t	x_atime;
	time_t	x_mtime;
	time_t	x_ctime;
	daddr_t	x_start;
	struct ynode *x_y;
	struct entre	x_ents[];
};

struct ynode {		/* Medial inode management */
	ino_t	y_ino;
	fsize_t	y_seek;
	daddr_t	y_nb;		/* Number of blocks total */
	daddr_t	y_ni;		/* Number of indirect blocks total */
	int	y_niiib;	/* Number of triple indirect blocks */
	int	y_niib;		/* Number of double indirect blocks */
	daddr_t	y_nib;		/* Number of single indirect blocks */
	daddr_t	y_ndb;		/* Number of data blocks */
	daddr_t *y_db;		/* Data block base */
	daddr_t *y_ib;		/* Single indirect block base */
	daddr_t *y_iib;		/* Double indirect block base */
	daddr_t *y_iiib;	/* Triple indirect block base */
	daddr_t	y_b[];		/* Actual block addresses, indirect first */
};

struct pfp {		/* Input parsing */
	char	*p_fn;		/* File name if any */
	int	p_ln;		/* Line number if any */
	int	p_dln;		/* Line number increment if any */
	char	*p_ip;		/* Initial stream pointer */
	char	*p_cp;		/* Current stream pointer */
	struct pfp	*p_fp;	/* Previous stream record */
};

extern char	devnull[];
extern char	ascii0[];
extern char	ascii1[];
extern int		nino;
extern char		*special;
extern char		*proto;
extern struct filsys	S;
extern int		FS;
extern struct protoargs P;
extern struct protoargs N;
extern ino_t		E[NDLEV];
extern int		Elevel;
extern struct xnode **X;
extern char pnil[];
extern struct pfp pfp;
extern char	*argv0;
extern int	estatus;
extern char	miscbuf[BSIZE];

extern struct entre *getentre();
extern ino_t getxnode();
extern ino_t getlink();
extern char *gettoken();
extern char *getline();
extern int getopen();
extern int getclose();
#define getch()		((*pfp.p_cp != 0) ? *pfp.p_cp++ : -1)
#define ungetch(c)	((c>=0 && pfp.p_cp>pfp.p_ip) ? (*--pfp.p_cp = c) : -1)
extern daddr_t balloc();
extern ino_t ialloc();
extern char *bcache();
extern char *realloc();

time_t	time();
char	*xmalloc();
char	*xrealloc();
char	*strncpy();
char	*strcat();
fsize_t	ftell();
long	atol();
int	atoi();

/* FILE data.c */
char	devnull[] = "/dev/null";
char	ascii0[] = "0";
char	ascii1[] = "1";

int		nino;	/* Number of inodes on new file system */
char		*special;	/* Special file specified */
char		*proto;		/* Proto file specified */
struct filsys	S;	/* Super block for new file system */
int		FS;	/* File descriptor of file system */
struct protoargs P;	/* File system prototype parameters */
struct protoargs N = {	/* Some names */
	"bootstrap file", "file system name", "file pack name",
	"file system size", "number of inodes", "interleave n", "interleave m"
};
ino_t		E[NDLEV];	/* Current nesting of directory inodes */
int		Elevel = -1;	/* Current nesting level of directory inodes */
struct xnode **X;	/* I-list pointers for new file system */
char pnil[] = "";
struct pfp pfp = { pnil, 0, 0, pnil, pnil, NULL };	/* Initial stream */
char	*argv0 = "mkfs";	/* Command name */
int	estatus = ES_SUCCESS;	/* Exit status */
char	miscbuf[BSIZE];

int	dflag = 0;		/* Preserve dates if on; set w/ -d opt. */

/* FILE main.c */
/*
 * Magic number of i-nodes as a function of
 * filesystem size in blocks.
 */
magic()
{
	register char *p;
	daddr_t fsize;
	unsigned long nino;
	static char b[8];

	fsize = atol(P.p_fsize);
	if (fsize > 1000)
		nino = fsize / 7;
	else
		nino = fsize / 5;
	if (nino > 65000U)		/* must fit into a short! */
		nino = 65000U;
	p = b+8;
	while (nino != 0) {
		*--p = nino % 10 + '0';
		nino /= 10;
	}
	P.p_nino = p;
}

main(argc, argv)
char *argv[];
{
	int anyopts = 0;
	if (argc < 1)
		return eusage();

	strcpy(s755, "\nd--755 0 0\n$\n");
	strcpy(sB, "B----- 0 0\n");

	argv0 = argv[0];
	/*
	 * Collect options.
	 */
	while (argc > 1 && argv[1][0] == '-') {
		switch (argv[1][1]) {
		case 'b':	P.p_bname = argv[2];		break;
		case 'f':	P.p_fname = argv[2];		break;
		case 'i':	P.p_nino = argv[2];		break;
		case 'm':	P.p_intm = argv[2];		break;
		case 'n':	P.p_intn = argv[2];		break;
		case 'p':	P.p_fpack = argv[2];		break;
		case 'd':	dflag = 1; --argc; ++argv;	continue;
		case 'V':	fprintf(stderr, "%s: V%s\n", argv0, VERSION);
				--argc; ++argv;			continue;
		default: 	return eusage();
		}
		anyopts += 1;
		argc -= 2;
		argv += 2;
	}
	/*
	 * Parse primary arguments and read or construct
	 * prototype description.
	 */
	if (argc != 3)
		return eusage();
	special = argv[1];
	if ( ! unnatural(argv[2])) {
		P.p_fsize = argv[2];
		if (mkproto() < 0)
			return estatus;
	} else {
		proto = argv[2];
		if (rdproto() < 0)
			return estatus;
		else if (anyopts)
			eignore("superfluous command line options");
	}
	/*
	 * Scan real or imaginary proto file.
	 */
	getboot();
	if (getsuper() < 0
	 || getdir() < 0
	 || (estatus&ES_FORMAT))
		return estatus;
	/*
	 * Scan in core i list and allocate blocks.
	 */
	scanilist();
	if (estatus&ES_FATAL)
		return (estatus);
	/*
	 * Write the new file system.
	 */
	if (putboot() < 0
	 || putsuper() < 0
	 || putilist() < 0
	 || putdata() < 0
	 || putfree() < 0)
		return (estatus);
	return (estatus);
}

/*
 * Prepare proto file for input.
 *  rdproto copies file into memory,
 *  mkproto constructs proto file in memory,
 *  each sets up gettoken() to read tokens.
 */
rdproto()
{
	register char *p;
	register int n;
	register int fd;
	register int nb;

	if ((fd = open(proto, 0)) < 0)
		return badopen("descriptor file", proto);
	nb = 0;
	n = BSIZE;
	p = xmalloc(n);
	while ((n = read(fd, p+nb, BSIZE)) == BSIZE) {
		nb += BSIZE;
		p = xrealloc(p, nb+BSIZE);
	}
	p[nb+n] = 0;
	getopen(p, proto, 1);
	return (0);
}

mkproto()
{
	register char *p, *p1, *p0;

	p0 = p = xmalloc(BSIZE);

	if ((p1 = P.p_bname) == NULL && (P.p_fname!=NULL || P.p_fpack != NULL))
		P.p_bname = devnull;
	if (P.p_fpack != NULL && P.p_fname == NULL)
		P.p_fname = "noname";

	if ((p1 = P.p_bname) != NULL) {
		while (*p = *p1++) ++p;
		if ((p1 = P.p_fname) != NULL) {
			*p++ = ' ';
			while (*p = *p1++) ++p;
			if ((p1 = P.p_fpack) != NULL) {
				*p++ = ' ';
				while (*p = *p1++) ++p;
			}
		}
	}
	*p++ = '\n';
	if (unnatural((p1 = P.p_fsize)))
		return badvalue(N.p_fsize, p1), -1;
	while (*p = *p1++) ++p;
	if (P.p_nino == NULL)
		magic();
	if (unnatural((p1 = P.p_nino)))
		return badvalue(N.p_nino, p1), -1;
	*p++ = ' ';
	while (*p = *p1++) ++p;
	if ((p1 = P.p_intn) != NULL) {
		if (unnatural(p1))
			return badvalue(N.p_intn, p1), -1;
		*p++ = ' ';
		while (*p = *p1++) ++p;
		if ((p1 = P.p_intm) != NULL) {
			if (unnatural(p1))
				return badvalue(N.p_intm, p1), -1;
			*p++ = ' ';
			while (*p = *p1++) ++p;
		}
	}
	p1 = s755;
	while (*p = *p1++) ++p;
	getopen(p0, NULL, 0);
	return (0);
}

/* FILE get.c */
/*
 * First pass activity -- scan proto file
 *  getboot() reads the boot file name.
 *  getsuper() reads the super block parameters and initializes
 *	the in memory file system structure.
 *  getbad() reads and actuates the bad block specification.
 *  getdir() reads the remainder of the proto file and constructs
 *	a directory tree and xnode list for the file system.
 */
getboot()
{
	P.p_bname = gettoken();
	if (P.p_bname == NULL)
		P.p_bname = devnull;
}

getsuper()
{
	register char	*ch;
	register int	i;

	if ((FS = open(special, 2)) < 0)
		return badopen("special file", special);

	if ((P.p_fname = gettoken()) == NULL) {
		P.p_fname = "noname";
		strncpy(S.s_fname, P.p_fname, 6);
	} else 
		for (ch = P.p_fname, i = 0; *ch != ' ' && *ch != '\t' && 
				*ch != '\n' && *ch != '\r' && i < 6; i++, ch++)
			S.s_fname[i] = *ch;

	if ((P.p_fpack = gettoken()) == NULL) {
		P.p_fpack = "nopack";
		strncpy(S.s_fpack, P.p_fpack, 6);
	} else
		for (ch = P.p_fpack, i = 0; *ch != ' ' && *ch != '\t' && 
				*ch != '\n' && *ch != '\r' && i < 6; i++, ch++)
			S.s_fpack[i] = *ch;

	if (getline() == NULL)
		return earlyeof();
	if ((P.p_fsize = gettoken()) == NULL)
		return efatal("no value for %s", N.p_fsize);
	else if (unnatural(P.p_fsize))
		return badvalue(N.p_fsize, P.p_fsize), -1;
	S.s_fsize = atol(P.p_fsize);
	if ((P.p_nino = gettoken()) == NULL)
		magic();
	else if (unnatural(P.p_nino))
		return badvalue(N.p_nino, P.p_nino), -1;
	S.s_isize = (atoi(P.p_nino)+INOPB-1)/INOPB + INODEI;
	if ((P.p_intn = gettoken()) == NULL)
		P.p_intn = ascii1;
	else if (unnatural(P.p_intn))
		return badvalue(N.p_intn, P.p_intn), -1;
	S.s_n = atoi(P.p_intn);
	if ((P.p_intm = gettoken()) == NULL)
		P.p_intm = ascii1;
	else if (unnatural(P.p_intm))
		return badvalue(N.p_intm, P.p_intm), -1;
	S.s_m = atoi(P.p_intm);
	if (getline() == NULL)
		return earlyeof();

	/* Set the initial ifree list so that bad file can be initialized */
	S.s_ninode = 0;
	S.s_tinode = (S.s_isize - INODEI) * INOPB;
	S.s_inode[S.s_ninode++] = BADFIN+1;
	S.s_inode[S.s_ninode++] = BADFIN;
	nino = S.s_tinode;
	/* Allocate memory i list */
	X = xmalloc(S.s_tinode * sizeof(*X));
	clear(X, S.s_tinode * sizeof(*X));
	/* Read the bad block list */
	if (getbad() < 0)
		return (-1);
	/* Initialize the free block list */
	bbegin();
	/* Fixup the bad block file, ie allocate indirects */
	xfixup(X[BADFIN-1]);
	/* Set the time of construction */
	time(&S.s_time);
	return (0);
}

getbad()
{
	register int c;
	register daddr_t b;
	register char *tp;
	static char mymsg1[] = "illegal bad block number '%s'";
	static char mymsg2[] = "out of bounds bad block number '%s'";

	if (getxnode(sB) != BADFIN)
		return efatal("bad block file != inode %d", BADFIN);
	xexpand(X[BADFIN-1]);
	for (;;) {
		if ((c = getch()) != '%') {
			ungetch(c);
			break;
		}
		if ((c = getch()) != 'b') {
			ungetch(c);
			c = '%';
			ungetch(c);
			break;
		}
		while ((tp = gettoken()) != NULL) {
			if (unnatural(tp)) {
				eformat(mymsg1, tp);
				continue;
			}
			b = atol(tp);
			if (b >= S.s_fsize) {
				eformat(mymsg2, tp);
				continue;
			}
			xextend(X[BADFIN-1], b);
		}
		getline();
	}
	return (0);
}

getdir()
{
	char *cp1, *cp2;
	struct entre *ep;
	struct xnode *xp;
	int inum;

	for (;;) {
		if (Elevel < 0) {
			cp1 = getline();
		} else {
			cp1 = gettoken();
			cp2 = getline();
		}
		if (cp1 == NULL)
			return earlyeof();
		if (*cp1 == '$') {
			if (Elevel-- < 0) {
				assert(proto != NULL);
				efatal("misplaced '$' in '%s'", proto);
			}
			if (Elevel < 0)
				break;
			continue;
		}
		if (Elevel >= 0) {
			ep = getentre(cp1);
		} else
			cp2 = cp1;
		inum = getxnode(cp2);
		if (Elevel >= 0)
			ep->e_ino = inum;
		if (inum == 0)
			continue;
		assert(inum > BADFIN);
		assert(inum <= nino);
		xp = X[inum-1];
		xp->x_nlink += 1;
		assert(xp != NULL);
		if ((xp->x_mode&IFMT) == IFDIR) {
			if (++Elevel >= NDLEV)
				return efatal("directories too deep");
			E[Elevel] = inum;
			ep = getentre(".");
			ep->e_ino = inum;
			xp = X[inum-1];
			xp->x_nlink += 1;
			ep = getentre("..");
			if (Elevel != 0)
				inum = E[Elevel-1];
			ep->e_ino = inum;
			xp = X[inum-1];
			xp->x_nlink += 1;
		}
	}
	return (0);
}

struct entre *
getentre(cp)
char *cp;
{
	register struct xnode *xp;
	register struct entre *ep;
	register int inum;
	int nent;

	assert(Elevel >= 0);
	inum = E[Elevel];
	assert(inum > BADFIN);
	assert(inum <= nino);
	xp = X[inum-1];
	assert(xp != NULL);
	assert((xp->x_mode&IFMT) == IFDIR);
	xp->x_size += sizeof(struct direct);
	nent = xp->x_size / sizeof(struct direct);
	xp = xrealloc(xp, sizeof(*xp) + nent*sizeof(*ep));
	X[inum-1] = xp;
	ep = &xp->x_ents[nent-1];
	ep->e_ino = 0;
	ep->e_name = cp;
	return (ep);
}

ino_t
getxnode(cp)
char *cp;
{
	register struct xnode *xp;
	register ino_t inum;
	char *type, *uid, *gid, *name;
	int mode;
	dev_t dev;
	struct stat sbuf;

	if (cp == NULL)
missing_spec:
		return eformat("missing file specification");
	getopen(cp, NULL, 0);
	type = gettoken();
	uid = gettoken();
	gid = gettoken();
	name = gettoken();
	getclose();
	if (type == NULL)
		goto missing_spec;
	inum = 0;
	sbuf.st_size = 0;
	sbuf.st_mtime = sbuf.st_atime = sbuf.st_ctime = S.s_time;
	dev = 0;
	if (*type == 'l') {	/* Link to previously defined file */
		if (name == NULL)
			return eformat("missing link name '%s'", cp);
		inum = getlink((*name=='/' ? E[0] : E[Elevel]), name);
		if (inum == 0)
			return eformat("unknown link name '%s'", name);
		return (inum);
	}
	switch (type[0]) {
	case 'b':
	case 'c':
		if (name == NULL)
			return eformat("missing device specification");
		else
			dev = getdev(name);
		goto common;
	case 'd':
	case 'p':
		goto common;
	case 'B':
		goto common;
	case '-':
		if (name == NULL)
		   return eformat("missing source for regular file");
		else if (access(name, AREAD) < 0)
		   return eignore("access to '%s' failed: %s", name, syserror);
		else if (stat(name, &sbuf) < 0)
		   return eignore("stat of '%s' failed: %s", name, syserror);
		else if ((sbuf.st_mode&IFMT) != IFREG)
		   return eformat("'%s' is not a regular file", name);
	common:
		if ((mode = getmode(type)) == 0)
			return (0);
		if (uid == NULL || unnatural(uid))
			return eformat("bad user id '%s'", uid);
		if (gid == NULL || unnatural(gid))
			return eformat("bad or missing group id '%s'", gid);
		break;
	default:
		return eformat("bad file specification '%s'", cp);
	}
	xp = xmalloc(sizeof(*xp));
	xp->x_size = sbuf.st_size;
	xp->x_atime = sbuf.st_atime;
	xp->x_mtime = sbuf.st_mtime;
	xp->x_ctime = sbuf.st_ctime;
	xp->x_nlink = 0;
	xp->x_y = NULL;
	xp->x_start = 0;
	xp->x_ino = inum = ialloc();
	xp->x_mode = mode;
	xp->x_dev = dev;
	xp->x_uid = atoi(uid);
	xp->x_gid = atoi(gid);
	xp->x_name = name;
	X[inum-1] = xp;
	return (inum);
}

/* Translate proto mode specification into mode word */
getmode(cp)
char *cp;
{
	register char *p;
	register int mode;
	register int i;
	static char mymessage[] = "mode format: %s char should be one of %s";

	p = cp;
	assert(p != NULL);
	mode = 0;
	switch (*p++) {
	case 'b': mode |= IFBLK; break;
	case 'c': mode |= IFCHR; break;
	case 'd': mode |= IFDIR; break;
	case 'p': mode |= IFPIPE; break;
	case '-': mode |= IFREG; break;
	case 'B': mode |= IFREG; return (mode);
	case 'l': assert(p[-1] != 'l'); break;
	default: return eformat(mymessage, "1st", "[bcdlp-]");
	}
	switch (*p++) {
	case 'u': mode |= ISUID; break;
	case '-': break;
	default: return eformat(mymessage, "2nd", "[u-]");
	}
	switch (*p++) {
	case 'g': mode |= ISGID; break;
	case '-': break;
	default: return eformat(mymessage, "3rd", "[g-]");
	}
	for (i = 1<<6; i > 0; i >>= 3) switch (*p++) {
	case '7': mode += i;
	case '6': mode += i;
	case '5': mode += i;
	case '4': mode += i;
	case '3': mode += i;
	case '2': mode += i;
	case '1': mode += i;
	case '0':
	case '-': break;
	default: return eformat(mymessage, "4th, 5th, or 6th", "[01234567-]");
	}
	return (mode);
}

getdev(cp)
char *cp;
{
	char *majp, *minp;

	getopen(cp, NULL, 0);
	majp = gettoken();
	minp = gettoken();
	getclose();
	if (majp == NULL || unnatural(majp))
		return badvalue("major number", cp);
	if (minp == NULL || unnatural(minp))
		return badvalue("minor number", cp);
	return makedev(atoi(majp), atoi(minp));
}

/*
 * Lookup a name in our directory tree.
 */
ino_t
getlink(inum, name)
ino_t inum;
char *name;
{
	int i, nent;
	struct entre *ep;
	struct xnode *xp;

	assert(inum > BADFIN);
	assert(inum <= nino);
	xp = X[inum-1];
nextname:
	assert(xp != NULL);
	ep = xp->x_ents;
	nent = xp->x_size / sizeof(struct direct);
	while (*name == '/')
		name += 1;
	if (*name == 0)
		return (inum);
	while (--nent >= 0) {
		if (ep->e_ino == 0) {
			ep += 1;
			continue;
		}
		for (i = 0; ; i += 1) {
			if (name[i] != '\0' && name[i] != '/'
			  && name[i] != ep->e_name[i])
				break;
			if (i == DIRSIZ
			 || ep->e_name[i] == 0) {
				inum = ep->e_ino;
				assert(inum > BADFIN);
				assert(inum <= nino);
				if (name[i] == 0)
					return (inum);
				if (name[i] == '/') {
					name += i;
					xp = X[inum-1];
					assert(xp != NULL);
					if ((xp->x_mode&IFMT) != IFDIR)
						return (0);
					goto nextname;
				}
				return (0);
			}
		}
		ep += 1;
	}
	return (0);
}

/*
 * Intermediate pass, prepare for writing.
 *  scanilist() scans the xnode list and allocates data blocks
 *	and indirect mapping blocks.
 */
scanilist()
{
	register int i;
	register struct xnode *xp;

	for (i = BADFIN+1; i <= nino; i += 1) {
		if ((xp = X[i-1]) == NULL)
			continue;
		xexpand(xp);
		xcontract(xp);
	}
}

/* FILE put.c */
/*
 * Second pass --
 *  we write everything in the order it appears
 *  without intervening reads.
 *  putboot() writes the boot block.
 *  putsuper() writes the super block.
 *  putilist() writes the ilist.
 *  putdata() writes the files.
 *  putfree() fills in the free block list.
 */
putboot()
{
	register int fd;
	register struct ldheader *ldp;
	char *bp;
	fsize_t fsize;

	if ((fd = open(P.p_bname, 0)) < 0) {
		eignore("open %s '%s': %s\n", N.p_bname, P.p_bname, syserror);
		return (0);
	}
	bp = bcache((daddr_t)BOOTBI);
	if (read(fd, bp, BSIZE) == BSIZE) {
		ldp = (struct ldheader *)bp;
		canint(ldp->l_magic);
		if (ldp->l_magic == L_MAGIC) {
			cansize(ldp->l_ssize[L_PRVD]);
			cansize(ldp->l_ssize[L_SHRD]);
			cansize(ldp->l_ssize[L_PRVI]);
			cansize(ldp->l_ssize[L_SHRI]);
			fsize = ldp->l_ssize[L_PRVD] + ldp->l_ssize[L_SHRD]
			      + ldp->l_ssize[L_PRVI] + ldp->l_ssize[L_SHRI];
			if (fsize > BSIZE)
				eignore("%s '%s' truncated to %d",
					N.p_bname, P.p_bname, BSIZE);
			lseek(fd, 44L, 0);
			read(fd, bp, BSIZE);
		} else
			canint(ldp->l_magic);
	}
	close(fd);
	return (0);
}

putsuper()
{
	register char *bp;

	bp = bcache((daddr_t)SUPERI);
	copy(bp, &S, BSIZE);
	cansuper(bp);
	return (0);
}

/*
 * Canonicalize the super block.
 */
cansuper(sbp)
register struct filsys *sbp;
{
	canshort(sbp->s_isize);
	candaddr(sbp->s_fsize);
	canshort(sbp->s_nfree);
	{
		register daddr_t *dp;
		register int i;

		i = NICFREE;
		dp = &sbp->s_free[NICFREE];
		while (--i >= 0) {
			dp -= 1;	/* Watch for side effects */
			candaddr(*dp);
		}
	}
	canshort(sbp->s_ninode);
	{
		register ino_t *ip;
		register int i;

		i = NICINOD;
		ip = &sbp->s_inode[NICINOD];
		while (--i >= 0) {
			ip -= 1;
			canino(*ip);
		}
	}
	cantime(sbp->s_time);
	candaddr(sbp->s_tfree);
	canino(sbp->s_tinode);
	canshort(sbp->s_m);
	canshort(sbp->s_n);
	canlong(sbp->s_unique);
}

putilist()
{
	register struct xnode *xp;
	struct dinode din;
	ino_t inum;

	clear(&din, sizeof(din));
	for (inum = BADFIN; inum <= nino; inum += 1) {
		if (bad((daddr_t)inodeb(inum))) {
			inum += INOPB;
			continue;
		}
		xp = X[inum-1];
		if (xp == NULL) {
			iput(inum, &din);
			continue;
		}
		din.di_mode = xp->x_mode;
		if (dflag)  {
			din.di_mtime = xp->x_mtime;
			din.di_atime = xp->x_atime;
			din.di_ctime = xp->x_ctime;
		} else {
			din.di_mtime = S.s_time;
			din.di_ctime = S.s_time;
			din.di_atime = S.s_time;
		}
		din.di_size = xp->x_size;
		din.di_nlink = xp->x_nlink;
		din.di_uid = xp->x_uid;
		din.di_gid = xp->x_gid;
		switch (xp->x_mode&IFMT) {
		case IFBLK:
		case IFCHR:
			din.di_a.di_rdev = xp->x_dev;
			break;
		case IFDIR:
		case IFREG:
			if (inum != BADFIN)
				xexpand(xp);
			xymerge(xp, &din);
			if (inum != BADFIN)
				xcontract(xp);
			break;
		}
		iput(inum, &din);
		clear(&din, sizeof(din));
	}
	return (0);
}

putdata()
{
	register int i;
	register struct xnode *xp;

	putbad();
	for (i = BADFIN+1; i <= nino; i += 1) {
		if ((xp = X[i-1]) == NULL)
			continue;
		if ((xp->x_mode&IFMT) == IFDIR)
			putdir(xp);
		else if ((xp->x_mode&IFMT) == IFREG)
			putreg(xp);
	}
	return (0);
}

putbad()
{
	register struct ynode *yp;

	assert(X[BADFIN-1] != NULL);
	yp = X[BADFIN-1]->x_y;
	assert(yp != NULL);
	if (yp->y_ni)
		xindir(yp);
}

putdir(xp)
struct xnode *xp;
{
	struct direct dir;
	register struct entre *ep;
	int i, nent;

	xexpand(xp);
	nent = xp->x_size / sizeof(struct direct);
	for (i = 0; i < nent; i += 1) {
		ep = xp->x_ents + i;
		dir.d_ino = ep->e_ino;
		strncpy(dir.d_name, ep->e_name, DIRSIZ);
		if (strlen(ep->e_name) > DIRSIZ)
			eignore("directory name '%s' truncated to %d chars",
				ep->e_name, DIRSIZ);
		canino(dir.d_ino);
		xwrite(xp, &dir, sizeof(struct direct));
	}
	xcontract(xp);
}

putreg(xp)
struct xnode *xp;
{
	int fd, nb;
	fsize_t size;
	struct stat sbuf;

	if (stat(xp->x_name, &sbuf) < 0)
		return eignore("stat of %s failed: %s", xp->x_name, syserror);
	if (sbuf.st_size != xp->x_size)
		return eignore("size of %s has changed", xp->x_name);
	if ((fd = open(xp->x_name, 0)) < 0)
		return eignore("open %s failed: %s", xp->x_name, syserror);
	xexpand(xp);
	size = xp->x_size;
	while (size > 0) {
		nb = read(fd, miscbuf, BSIZE);
		if (nb < 0) {
			eignore("read %s failed: %s", xp->x_name, syserror);
			break;
		}
		xwrite(xp, miscbuf, BSIZE);
		size -= nb;
	}
	xcontract(xp);
	close(fd);
}

putfree()
{
	daddr_t b;
	register daddr_t *dp;
	register int n;

	while (S.s_nfree > 0) {
		while (S.s_nfree > 1)
			b = balloc();
		b = balloc();	/* Forces load of next free block */
		dp = bcache(b);
		((struct fblk *)dp)->df_nfree = n = S.s_nfree;
		canint(((struct flbk *)dp)->df_nfree);
		dp = &((struct fblk *)dp)->df_free[0];
		copy(dp, S.s_free, n * sizeof(daddr_t));
		while (--n >= 0) {
			candaddr(*dp);
			dp += 1;
		}
	}
	bcache((daddr_t)-1);	/* Flush */
	return (0);
}

/* FILE misc.c */
/*
 * Allocation with fatal errors.
 */
char *
xmalloc(nb)
{
	char *p;

	p = malloc(nb);
	if (p != NULL)
		return (p);
	efatal("memory allocation failed");
}

char *
xrealloc(p, nb)
char *p;
{
	p = realloc(p, nb);
	if (p != NULL)
		return (p);
	efatal("memory reallocation failed");
}

/*
 * Errors.
 */
char usage[] =	"Usage: %s [ option ... ] special proto\n"
		"Options:\n"
		"\t-b boot\n"
		"\t-d\n"
		"\t-f name\n"
		"\t-i inodes\n"
		"\t-m arg\n"
		"\t-n arg\n"
		"\t-p pack\n";

eusage()
{
	fprintf(stderr, usage, argv0, argv0);
	estatus |= ES_FATAL;
	exit(estatus);
}

eerror(es, fs, ap)
int es;
char *fs;
char **ap;
{
	fprintf(stderr, "%s: ", argv0);
	if (pfp.p_fn) fprintf(stderr, "in %s: ", pfp.p_fn);
	if (pfp.p_ln) fprintf(stderr, "at %d: ", pfp.p_ln);
	fprintf(stderr, fs, ap);
	estatus |= es;
	if (estatus & ES_FATAL)
		exit(estatus);
	return (0);
}

efatal(a1)
char *a1;
{
	return eerror(ES_FATAL, "%r\n", &a1);
}

eignore(a1)
char *a1;
{
	return eerror(ES_IGNORED, "%r (ignored)\n", &a1);
}

eformat(a1)
char *a1;
{
	return eerror(ES_FORMAT, "%r\n", &a1);
}

badvalue(np, vp)
char *np, *vp;
{
	return eformat("bad value for %s: %s", np, vp);
}

badopen(np, cp)
char *cp, *np;
{
	return efatal("open '%s' as %s failed: %s", cp, np, syserror);
}

earlyeof()
{
	assert(proto != NULL);
	return efatal("unexpected end of descriptor file: %s", proto);
}

/*
 * Token and line input.
 *  getopen() pushes the current input string,
 *  getclose() pops the previous input string,
 *  gettoken() returns a pointer to the beginning of the next token,
 *  getline() returns pointer to the same and advances to next line.
 *  both return NULL on unexpected end of file.
 */
getopen(cp, fn, dln)
char *cp;
char *fn;
int dln;
{
	struct pfp *tp;

	tp = xmalloc(sizeof(*tp));
	*tp = pfp;
	pfp.p_fn = fn;
	pfp.p_ln = 0;
	pfp.p_dln = dln;
	pfp.p_ip = pfp.p_cp = cp;
	pfp.p_fp = tp;
}

getclose()
{
	struct pfp *tp;

	assert(pfp.p_fp != NULL);
	tp = pfp.p_fp;
	pfp = *tp;
	free(tp);
}

char *
gettoken()
{
	register int c;
	register char *p, *tp;

	pfp.p_ln += pfp.p_dln;
	pfp.p_dln = 0;
	p = pfp.p_cp;

	/* DEBUG - this code is writing into string literals! */
#if _I386 && !NDEBUG
	if ((int)pfp.p_cp < 0x400000) {
		fprintf(stderr, "pfp.p_cp=%x:%s\n",
		  pfp.p_cp, pfp.p_cp);
		fflush(stderr);
		exit(1);
	}
#endif

	while ((c = *p) == ' ' || c == '\t')
		*p++ = 0;
	pfp.p_cp = tp = p;
	if (c == 0 || c == '\n')
		return (NULL);
	do
		c = *p++;
	while (c != 0 && c != ' ' && c != '\t' && c != '\n');
	pfp.p_cp = p - 1;
	return (tp);
}

char *
getline()
{
	register int c;
	register char *p, *tp;

	pfp.p_ln += pfp.p_dln;
	pfp.p_dln = 0;
	p = pfp.p_cp;
	while ((c = *p) == ' ' || c == '\t')
		*p++ = 0;
	pfp.p_cp = tp = p;
	if (c == 0)
		return earlyeof();
	if (c == '\n') {
		*p++ = 0;
		pfp.p_cp += 1;
		if (pfp.p_ln) pfp.p_dln = 1;
		return (tp);
	}
	do
		c = *p++;
	while (c != 0 && c != '\n');
	if (c == 0)
		return earlyeof();
	pfp.p_cp = p;
	p[-1] = 0;
	return (tp);
}

/*
 * Miscellaneous.
 */
unnatural(cp)
char *cp;
{
	register char *p;

	p = cp;
	while (isdigit(*p)) p += 1;
	switch (*p) {
	case 0:
	case ' ':
	case '\t':
	case '\n':
		return (0);
	default: 
		return (-1);
	}
}

clear(bp, n)
register char *bp;
register unsigned n;
{
	if (n) do {
		*bp++ = 0;
	} while (--n);
}

copy(bp1, bp2, n)
register char *bp1;
register char *bp2;
register unsigned n;
{
	if (n) do {
		*bp1++ = *bp2++;
	} while (--n);
}

iszero(bp, n)
register char *bp;
register unsigned n;
{
	if (n) do
		if (*bp++ != 0) return (0);
	while (--n);
	return (1);
}

/* FILE xmisc.c */
/*
 *  xexpand() - creates a block map for the inode.
 *	indirect blocks are allocated sequentially, followed by data blocks
 *	also sequentially.
 *  xcontract() - discards the block map.
 *  xextend() - appends a specified data block to the bad inode.
 *  xymerge() - merges block addresses into disk inode structure.
 *  xwrite() - writes data and necessary indirect blocks into filesystem.
 */
int xwatch = 0;

xexpand(xp)
struct xnode *xp;
{
	register struct ynode *yp;
	register daddr_t *dp;
	register daddr_t nb;
	daddr_t b;

	assert(xp != NULL);
	assert(xp->x_ino >= BADFIN);
	assert(xp->x_ino <= nino);
	yp = xp->x_y;
	assert(yp == NULL);
	xp->x_y = yp = xmalloc(sizeof(*yp) + NADDR * sizeof(daddr_t));
	clear(yp, sizeof(*yp) + NADDR * sizeof(daddr_t));
	yp->y_ino = xp->x_ino;
	yp->y_seek = 0;
	xblkuse(xp);
	nb = yp->y_nb;
	if (nb > NADDR) {
	    xp->x_y = yp = xrealloc(yp, sizeof(*yp)
		+ ((unsigned)nb) * sizeof(daddr_t));
	    clear(yp->y_b, ((unsigned)nb) * sizeof(daddr_t));
	}
	/* Set up pointers */
	yp->y_iiib = yp->y_b;
	yp->y_iib = yp->y_iiib + yp->y_niiib;
	yp->y_ib = yp->y_iib + yp->y_niib;
	yp->y_db = yp->y_ib + yp->y_nib;
	/* Resynchronize the allocator */
	if (b = xp->x_start)
		bstart(xp->x_start);
	/* Allocate blocks */
	dp = yp->y_b;
	nb = yp->y_nb;
	while (--nb >= 0)
		*dp++ = balloc();
	if (xp->x_start == 0)
		xp->x_start = yp->y_b[0];
}

xblkuse(xp)
struct xnode *xp;
{
	register struct ynode *yp;
	register daddr_t nb;

	yp = xp->x_y;
	nb = xp->x_size + BSIZE - 1;
	nb /= BSIZE;
	yp->y_niiib = 0;
	yp->y_niib = 0;
	yp->y_nib = 0;
	yp->y_ndb = 0;
	yp->y_ni = 0;
	if (nb > ND) {
		if (nb > ND+NBN) {
			if (nb > ND+NBN+NBN*NBN) {
				nb -= ND+NBN+NBN*NBN;
				assert(nb > 0 && nb < NBN*NBN*(long)NBN);
				yp->y_ndb += nb;
				nb += NBN-1;
				nb /= NBN;
				yp->y_nib += nb;
				nb += NBN-1;
				nb /= NBN;
				yp->y_niib += nb;
				assert(((nb+NBN-1)/NBN) == 1);
				yp->y_niiib += 1;
				nb = ND+NBN+NBN*NBN;
			}
			nb -= ND+NBN;
			assert(nb > 0 && nb < NBN*NBN);
			yp->y_ndb += nb;
			nb += NBN-1;
			nb /= NBN;
			yp->y_nib += nb;
			assert(((nb+NBN-1)/NBN) == 1);
			yp->y_niib += 1;
			nb = ND+NBN;
		}
		nb -= ND;
		assert(nb > 0 && nb < NBN);
		yp->y_ndb += nb;
		assert(((nb+NBN-1)/NBN) == 1);
		yp->y_nib += 1;
		nb = ND;
	}
	assert(nb >= 0 && nb <= ND);
	yp->y_ndb += nb;
	yp->y_ni = yp->y_nib + yp->y_niib + yp->y_niiib;
	yp->y_nb = yp->y_ndb + yp->y_ni;
}

xcontract(xp)
register struct xnode *xp;
{
	register struct ynode *yp;

	assert(xp != NULL);
	assert(xp->x_ino >= BADFIN);
	assert(xp->x_ino <= nino);
	yp = xp->x_y;
	assert(yp != NULL);
	assert(yp->y_ino == xp->x_ino);
	xp->x_y = NULL;
	free(yp);
}

xymerge(xp, dip)
struct xnode *xp;
struct dinode *dip;
{
	register struct ynode *yp;
	char *l3p;

	assert(xp != NULL);
	assert(xp->x_ino >= BADFIN);
	assert(xp->x_ino <= nino);
	yp = xp->x_y;
	assert(yp != NULL);
	assert(yp->y_ino == xp->x_ino);
	l3p = dip->di_addr;
	ltol3(l3p, yp->y_db, ND);
	l3p += 3*ND;
	if (yp->y_nib)
		ltol3(l3p, yp->y_ib, 1);
	l3p += 3;
	if (yp->y_niib)	
		ltol3(l3p, yp->y_iib, 1);
	l3p += 3;
	if (yp->y_niiib)
		ltol3(l3p, yp->y_iiib, 1);
}

xwrite(xp, cp, nb)
struct xnode *xp;
char *cp;
int nb;
{
	register struct ynode *yp;
	daddr_t bn;
	int bo;
	char *bp;

	assert(xp != NULL);
	assert(xp->x_ino > BADFIN);
	assert(xp->x_ino <= nino);
	yp = xp->x_y;
	assert(yp != NULL);
	assert(yp->y_ino == xp->x_ino);
	if (yp->y_seek == 0 && yp->y_ni != 0)
		xindir(yp);	/* Write indirect blocks */
	bn = yp->y_seek / BSIZE;
	assert(bn < yp->y_ndb);
	bo = yp->y_seek % BSIZE;
	assert(bo+nb <= BSIZE);
	bp = bcache(yp->y_db[(int)bn]);
	copy(bp+bo, cp, nb);
	yp->y_seek += nb;
}

xindir(yp)
register struct ynode *yp;
{
	/* Triple indirect block */
	if (yp->y_niiib)
		xindblks(yp->y_iiib, yp->y_iib+1, (daddr_t)yp->y_niib-1);
	/* Double indirect blocks */
	if (yp->y_niib)
		xindblks(yp->y_iib, yp->y_ib+1, (daddr_t)yp->y_nib-1);
	/* Single indirect blocks */
	if (yp->y_nib)
		xindblks(yp->y_ib, yp->y_db+ND, (daddr_t)yp->y_ndb-ND);
}

xindblks(dp, sp, nb)
register daddr_t *dp, *sp, nb;
{
	register int n;

	while (nb > 0) {
		if (nb >= NBN)
			n = NBN;
		else
			n = nb;
		xindblk(*dp, sp, n);
		dp += 1;
		sp += n;
		nb -= n;
	}
}


xindblk(b, bp, nd)
daddr_t b, *bp;
register int nd;
{
	register daddr_t *dp;
	dp = bcache(b);
	copy(dp, bp, nd * sizeof(daddr_t));
	while (--nd >= 0) {
		candaddr(*dp);
		dp += 1;
	}
}

/*
 * Extend the bad block file.
 */
xextend(xp, b)
struct xnode *xp;
daddr_t b;
{
	register struct ynode *yp;
	int nd;

	assert(xp != NULL);
	assert(xp->x_ino == BADFIN);
	yp = xp->x_y;
	assert(yp != NULL);
	assert(yp->y_ino == BADFIN);
	assert(b < S.s_fsize);
	if (bad(b))
		return eignore("duplicated bad block %ld", b);
	else if (b == BOOTBI)
		return eignore("boot block (%ld) is bad", b);
	else if (b == SUPERI)
		return efatal("super block (%ld) is bad", b);
	else if (b == inodeb(BADFIN))
		return efatal("first inode block (%ld) is bad", b);
	else if (b < S.s_isize)
		S.s_tinode -= INOPB;
	else
		S.s_tfree -= 1;
	xp->x_size += BSIZE;
	nd = yp->y_ndb += 1;
	if (nd > NADDR)
	    xp->x_y = yp = xrealloc(yp, sizeof(*yp) + nd * sizeof(daddr_t));
	yp->y_b[nd-1] = b;
}

bad(b)
daddr_t b;
{
	struct ynode *yp;
	register daddr_t *dp;
	register int i;

	assert(X[BADFIN-1] != NULL);
	yp = X[BADFIN-1]->x_y;
	assert(yp != NULL);
	assert(yp->y_ino == BADFIN);
	i = yp->y_ndb;
	dp = yp->y_db;
	while (--i >= 0)
		if (b == *dp)
			return (1);
		else
			dp += 1;
	return (0);
}

xfixup(xp)
struct xnode *xp;
{
	struct ynode *yp;
	register daddr_t *dp1, *dp2;
	register int i;


	assert(xp != NULL);
	assert(xp->x_ino == BADFIN);
	yp = xp->x_y;
	assert(yp != NULL);
	xblkuse(xp);
	if (yp->y_ni) {
		yp = xrealloc(yp, sizeof(*yp) + yp->y_nb * sizeof(daddr_t));
		xp->x_y = yp;
		yp->y_iiib = yp->y_b;
		yp->y_iib = yp->y_iiib + yp->y_niiib;
		yp->y_ib = yp->y_iib + yp->y_niib;
		yp->y_db = yp->y_ib + yp->y_nib;
		dp1 = yp->y_db + yp->y_ndb;
		dp2 = yp->y_b + yp->y_ndb;
		i = yp->y_ndb;
		while (--i >= 0)
			*--dp1 = *--dp2;
		dp1 = yp->y_b;
		i = yp->y_ni;
		while (--i >= 0)
			*dp1++ = balloc();
	}
}

xdump(xp)
register struct xnode *xp;
{
	register struct ynode *yp;

	yp = xp->x_y;
	fprintf(stderr, "%d %x %ld\n", xp->x_ino, xp->x_mode, xp->x_size);
	if (yp->y_niiib)
		xlist("indir^3", yp->y_iiib, (daddr_t)yp->y_niiib);
	if (yp->y_niib)
		xlist("indir^2", yp->y_iib, (daddr_t)yp->y_niib);
	if (yp->y_nib)
		xlist("indir^1", yp->y_ib, (daddr_t)yp->y_nib);
	if (yp->y_ndb)
		xlist("data", yp->y_db, (daddr_t)yp->y_ndb);
}
xlist(cp, dp, n)
char *cp;
daddr_t *dp;
daddr_t n;
{
	fprintf(stderr, "	%d %s: ", n, cp);
	for (;;) {
		fprintf(stderr, "%ld", dp[0]);
		dp += 1;
		n -= 1;
		if (n == 0)
			break;
		if (dp[0] == dp[-1]+1) {
			fprintf(stderr, "..");
			while (dp[0] == dp[-1]+1) {
				dp += 1;
				n -= 1;
				if (n == 0)
					break;
			}
			fprintf(stderr, "%ld", dp[-1]);
			if (n == 0)
				break;
		}
		fprintf(stderr, ", ");
	}
	fprintf(stderr, "\n");
}

/* FILE imisc.c */
/*
 * Inode cache management.
 *  iput() - writes an inode to disk.
 */
/*
 * Write the inode `ip' containing the inode `i' onto the
 * filesystem in canonical form.
 */
iput(i, dip1)
ino_t i;
register struct dinode *dip1;
{
	register struct dinode *dip2;

	dip2 = bcache((daddr_t)inodeb(i));
	dip2 += inodei(i);
	*dip2 = *dip1;
	canshort(dip2->di_mode);
	canshort(dip2->di_nlink);
	canshort(dip2->di_uid);
	canshort(dip2->di_gid);
	cansize(dip2->di_size);
	cantime(dip2->di_atime);
	cantime(dip2->di_mtime);
	cantime(dip2->di_ctime);
	switch (dip1->di_mode&IFMT) {
	case IFCHR:
	case IFBLK:
		candev(dip2->di_a.di_rdev);
		break;
	}
}

/*
 * Return a free inode number.
 */
ino_t
ialloc()
{
	ino_t inum;
	ino_t in;
	ino_t *ip;
	daddr_t b;

	if (S.s_ninode == 0)
		return efatal("out of inodes");
	inum = S.s_inode[--S.s_ninode];
	if (S.s_ninode == 0) {
		ip = &S.s_inode[NICINOD];
		in = inum + 1;
		while ((b = inodeb(in)) < S.s_isize && ip >S.s_inode) {
			if (bad(b)) {
				in += INOPB;
				continue;
			}
			*--ip = in++;
		}
		S.s_ninode = NICINOD;
		if (ip != S.s_inode) {
			S.s_ninode -= ip - S.s_inode;
			copy(S.s_inode, ip, S.s_ninode * sizeof(ino_t));
		}
	}
	--S.s_tinode;
	return (inum);
}

/* FILE bmisc.c */
/*
 * Block management.
 *  bbegin() - set the seed of the block allocator.
 *  balloc() - allocate a block.
 *  bstart() - resynchronize the allocation.
 *  bmap() - implement interleave mapping.
 *  bcache() - single block buffer cache.
 */
int bwatch = 0;

static daddr_t bseed;
static daddr_t bmap();

bbegin()
{
	/* Must be called after the bad block list is initialized */
	assert(X != NULL);
	assert(X[BADFIN-1] != NULL);
	assert(X[BADFIN-1]->x_y != NULL);
	bseed = S.s_isize;
	S.s_tfree += S.s_fsize - S.s_isize;
	clear(S.s_free, NICFREE*sizeof(daddr_t));
	S.s_nfree = 1;
	while (bad(S.s_free[0] = bmap(bseed)))
		++bseed;
}

daddr_t
balloc()
{
	register daddr_t *dp;
	register daddr_t b, b1;

	if (S.s_nfree == 0)
		return 0;
	b = S.s_free[--S.s_nfree];
	if (S.s_nfree == 0) {
		clear(S.s_free, sizeof S.s_free);
		dp = &S.s_free[NICFREE];
		while (dp > S.s_free && ++bseed < S.s_fsize) {
			b1 = bmap(bseed);
			if (bad(b1))
				continue;
			*--dp = b1;
		}
		S.s_nfree = NICFREE - (dp - S.s_free);
		if (dp != S.s_free)
			copy(S.s_free, dp, S.s_nfree * sizeof(daddr_t));
	}
	--S.s_tfree;
	return (b);
}

bstart(b)
daddr_t b;
{
	if (S.s_nfree && S.s_free[S.s_nfree-1] == b)
		return;
	bbegin();
	while (S.s_nfree && S.s_free[S.s_nfree-1] != b)
		balloc();
	assert(S.s_nfree != 0);
}

#define	MAXINTN	255			/* maptab must be int * if > 255 */
static unsigned	char	*maptab;	/* Interleave table */
static daddr_t	mapbot;
static daddr_t maptop;
/*
 * Return a mapped block number with interleaving.
 */
static daddr_t
bmap(b)
daddr_t b;
{
	register short i;
	register int ints;

	if (maptab == NULL) {
		if (S.s_n > MAXINTN
		 || S.s_m > S.s_n
		 || S.s_n%S.s_m != 0)
			efatal("%d/%d: bad interleave factor", S.s_m, S.s_n);
		maptab = xmalloc(S.s_n);
		mapbot = ((S.s_isize + S.s_n - 1) / S.s_n) * S.s_n;
		maptop = (S.s_fsize / S.s_n) * S.s_n;
		ints = S.s_n / S.s_m;
		for (i=0; i < S.s_n; i++)
			maptab[i] = (i / ints) + (i % ints) * S.s_m;
	}
	if (b >= mapbot && b < maptop) {
		i = b % S.s_n;
		b -= i;
		b += maptab[i];
	}
	return (b);
}

char *
bcache(b)
daddr_t b;
{
	static daddr_t bcacheb = -1;
	static char buffer[BSIZE];
	if (bcacheb != b) {
		if (bcacheb >= 0) {
			lseek(FS, (long)bcacheb*BSIZE, 0);
if (bwatch) bdump(b, bcacheb, buffer);
			if (write(FS, buffer, BSIZE) != BSIZE)
			    eignore("filesystem write error at block %ld", b);
		}
		clear(buffer, BSIZE);
	}
	bcacheb = b;
	return (buffer);
}

bdump(b1, b2, bp)
daddr_t b1, b2;
char *bp;
{
	int i, j;
	fprintf(stderr, "bcache(%ld) writes(%ld) = {", b1, b2);
	if (iszero(bp, BSIZE))
		fprintf(stderr, " 0 ");
	else {
		fprintf(stderr, "\n");
		for (i = 0; i < 32; i += 1) {
			for (j = 0; j < 16; j += 1)
				fprintf(stderr, " %02x", *bp++ & 0377);
			fprintf(stderr, "\n");
		}
	}
	fprintf(stderr, "}\n");
}

/* end of mkfs.c */
