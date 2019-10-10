/*
 * ls.c
 * 03/17/92
 * List structure
 */

#include <stdio.h>
#include <canon.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#define	BSIZE	BUFSIZ		/* Disc blocking factor for '-s' */
#define	NBN	128		/* Number of blocks in an indirect block */
#define	ND	10		/* Number of direct blocks */
#define	NI	1		/* Number of indirect blocks */
#define	NII	1		/* Number of double indirect blocks */
#define	NIII	1		/* Number of triple indirect blocks */

#define	MTIME	0		/* Use modify time */
#define	ATIME	1		/* Use access time */
#define	CTIME	2		/* Use create time */

#define	GAP	1		/* Space between Multi-column entries 	*/
#define	MAXLEN	78		/* Maximum line length			*/
#define MAXARG	4096		/* Maximum # of args stored for -C sort */
#define MAXDIR	128		/* Maximum # of subdirectories for -R	*/
#define MAXWID	12		/* Default max width for columns	*/
#define	MAXNBUF	80		/* Maximum namebuf length		*/

#define	OLD	(60L*60*24*365)	/* old form of dates (seconds) */

/*
 * The following flags are mutually exclusive.
 * The rightmost one on the command line wins.
 */
#define EXCLUDE_Clmnox	Cflag = lflag = mflag = nflag = oflag = xflag = 0

int	aflag;			/* List all entries (including "." & "..") */
int	bflag;			/* Print non-raphic chars in octal */
int	Cflag;			/* Multicolumn, sorted down the columns */
int	dflag;			/* Treat directories like files */
int	fflag;			/* Force something to look like a directory */
int	Fflag;			/* Print / after dirs, * after executables */
int	gflag;			/* Print gid vs. uid */
int	iflag;			/* Give i-number */
int	lflag;			/* Longer format */
int	mflag;			/* Stream format output */
int	nflag;			/* Same as -l except GID and UID are #s */
int	oflag;			/* Same as -l except group is not printed */
int	pflag;			/* Print a / after directories */
int	qflag;			/* Force non-graphic chars into ? mode */
int	Rflag;			/* List directories recursively */
int	rflag;			/* Reverse order of sort */
int	sflag;			/* Print size in bytes */
int	tflag = MTIME;		/* Which time to display and sort on */
int	xflag;			/* Multicolumn, sort across the columns */
int	sortflg;		/* On for sort by time, 0 for by name */
int	myuid;			/* User id for selecting .* suppression */

int	linesiz = 0;		/* Keep track of how much printed for -m */
int	first 	= 1;		/* First in stream format?		*/
int	maxwidth = MAXWID;	/* Longest string in multi-column format  */
int	ncols	= 1;		/* Number of columns */
int	nrows	= 1;		/* Number of rows */
int	offset	= 0;		/* How far to move pointer to print next entry*/
int	nlast	= 0;		/* Number of cols in last row */
int	ccol	= 0;		/* Current Column	*/
time_t	curtime;

char	obuf[BUFSIZ];

char	*deflist[] = {
	".",
	NULL
};

char	*namelist[MAXARG];	/* store file names for column output */

typedef	struct	dlist{			
	char *name;
	fsize_t	size;
} DLIST;

typedef	struct	ls {
	char	ls_dname[DIRSIZ+1];
	char	*ls_name;
	char	*ck_name;
	ino_t	ls_ino;
	short	ls_mode;
	short	ls_uid;
	short	ls_gid;
	short	ls_nlink;
	fsize_t	ls_size;
	dev_t	ls_rdev;		/* Real device */
	time_t	ls_time;		/* One of atime, mtime, ctime */
} LS;

int	(*qcomp)();
int	qtcomp();
int	qncomp();
int	qdncomp();

char	*alloc();
void	fatal();
char	*getuname();
char	*getgname();
char	*getname();
char	*getflag();

main(argc, argv)
char *argv[];
{
	register char *ap;
	register int es;

	setbuf(stdout, obuf);

	/*
	 * recognize variations on the name "ls" as implying which
	 * flags to set by default.
	 *   basename  flags
	 *	l	-l
	 *	lc	-C	recognized, but conflicts with COHERENT /bin/lc
	 *	lf	-CF
	 *	lr	-CR
	 *	lx	-x
	 */
	if ((ap = strrchr(argv[0], '/')) == NULL)
		ap = argv[0];
	else
		++ap;
	if (streq(ap, "l"))
		lflag = 1;
	else if (streq(ap, "lc"))
		Cflag = 1;
	else if (streq(ap, "lf"))
		Cflag = Fflag = 1;
	else if (streq(ap, "lr"))
		Cflag = Rflag = 1;
	else if (streq(ap, "lx"))
		xflag = 1;

	while (argc>1 && *argv[1]=='-') {
		for (ap=&argv[1][1]; *ap; ap++)
			switch (*ap) {
			case 'a':
				aflag = 1;
				break;

			case 'b':
				bflag = 1;
				break;

			case 'c':
				tflag = CTIME;
				break;

			case 'C':
				EXCLUDE_Clmnox;
				Cflag = 1;
				break;

			case 'd':
				dflag = 1;
				break;

			case 'F':
				Fflag = 1;
				break;

			case 'f':
				aflag = fflag = 1;
				break;

			case 'g':
				gflag = 1;
				break;

			case 'i':
				iflag = 1;
				break;

			case 'l':
				EXCLUDE_Clmnox;
				lflag = 1;
				break;

			case 'm':
				EXCLUDE_Clmnox;
				mflag = 1;
				break;

			case 'n':
				EXCLUDE_Clmnox;
				nflag = 1;
				break;

			case 'o':
				EXCLUDE_Clmnox;
				oflag = 1;
				break;

			case 'p':
				pflag = 1;
				break;

			case 'q':
				qflag = 1;
				break;

			case 'r':
				rflag = 1;
				break;

			case 'R':
				Rflag = 1;
				break;

			case 's':
				sflag = 1;
				break;

			case 't':
				sortflg = 1;
				break;

			case 'u':
				tflag = ATIME;
				break;

			case 'x':
				EXCLUDE_Clmnox;
				xflag = 1;
				break;

			default:
				usage();
			}
		argc--;
		argv++;
	}
	if (fflag)
		lflag = sflag = 0;
	time(&curtime);
	myuid = getuid();
	qcomp = (sortflg) ? qtcomp : qncomp;
	es = (argc > 1) ? ls(argv+1, argc-1) : ls(deflist, 1);
	exit(es);
}

/*
 * Do 'ls' on one file or directory.
 * 'narg' is the number of names in 'flist'
 * to determine special output format.
 */
ls(flist, narg)
register char **flist;
int narg;
{
	register int estat = 0;
	register LS *lsp;
	register LS *arena;
	LS *asavep;
	int Cargs = 0;
	struct stat sb;

	arena = (LS *)alloc(narg*sizeof(LS),"arena");
	asavep = arena;
	for ( ; *flist!=NULL; flist++) {
		if (stat(*flist, &sb) < 0) {
			perror(*flist);
			estat = 1;
			continue;
		}
		astore(*flist, &sb, asavep++);
	}
	qsort(arena, asavep-arena, sizeof(LS), qcomp);
	if (qcomp == qncomp)
		qcomp = qdncomp;
	if (xflag || Cflag)
		ncols = MAXLEN/(maxwidth+GAP);

	for (lsp = arena; lsp < asavep; lsp++) {
		if (fflag)
			continue;
		if ((lsp->ls_mode & S_IFMT) == S_IFDIR && !dflag)
			continue;
		if (Cflag)
			svname(lsp->ck_name,Cargs++);
		else if (xflag)
			prcolh(lsp->ck_name);
		else
			prstuff(lsp->ls_name, lsp);
	}
	if (Cflag && Cargs)
		prnames(Cargs);
	maxwidth = MAXWID;
	linesiz = 0;
	for (lsp = arena; lsp < asavep; lsp++) {
		if (dflag || (lsp->ls_mode&S_IFMT)!=S_IFDIR)
			continue;
		if (narg > 1)
			printf("\n%s:\n", lsp->ls_name);
		lsdir(lsp->ls_name,lsp->ls_size);
		freename(lsp);
	}
	free(arena);
	if ((mflag && linesiz) || ((Cflag || xflag) && ccol)){
		putchar('\n');
		ccol = 0;
		linesiz = 0;
	}
	return estat;
}

/*
 * Print for all options except -C and -x
 */
prstuff(file, lsp)
char *file;
register LS *lsp;
{
	register char *cp;
	register spcl = 0;

	if (iflag)
		printf("%5u ", lsp->ls_ino);
	if (sflag) {
		prsize(lsp);
		putchar(' ');
	}
	if (lflag || oflag || nflag) {
		switch (lsp->ls_mode & S_IFMT) {
		case S_IFREG:
			putchar('-');
			break;

		case S_IFDIR:
			putchar('d');
			break;

		case S_IFCHR:
			putchar('c');
			spcl++;
			break;

		case S_IFBLK:
			putchar('b');
			spcl++;
			break;

		case S_IFPIP:
			putchar('p');
			break;

		case S_IFMPB:
		case S_IFMPC:
			putchar('m');
			spcl++;
			break;

		default:
			putchar('x');
		}
		prmode((lsp->ls_mode>>6)&07, lsp->ls_mode&S_ISUID);
		prmode((lsp->ls_mode>>3)&07, lsp->ls_mode&S_ISGID);
		prmode(lsp->ls_mode&07, 0);
		if (lsp->ls_mode & S_ISVTX)
			putchar('t');
		else
			putchar(' ');
		printf("%2d ", lsp->ls_nlink);
		if (!gflag){
			if (!nflag){
				cp = getuname(lsp->ls_uid);
				if (cp == NULL)
					printf("%-10d ", lsp->ls_uid); 
				else
					printf("%-10s ", cp);
			} else
				printf("%-10d ", lsp->ls_uid); 
		}
		if (!oflag){
			if (!nflag){
				cp = getgname(lsp->ls_gid);
				if (cp == NULL)
					printf("%-10d ", lsp->ls_gid); 
				else
					printf("%-10s ", cp);
			} else
				printf("%-10d ", lsp->ls_gid); 
		}
		if (!spcl)
			printf("%7ld", lsp->ls_size);
		else
			printf("%3d %3d", major(lsp->ls_rdev),
			    minor(lsp->ls_rdev));
		prtime(&lsp->ls_time);
	}
	if (!mflag ){
		prname(file);
		prflag(lsp->ls_mode);
		putchar('\n');
	}
	else
		prstream(file,lsp->ls_mode);
}

/*
 * Print out a filesize from a ls store buffer.
 * This size (in BSIZE units or blocks) takes
 * into account indirect blocks.
 * However this should be done in a more general manner.
 */
prsize(lsp)
register LS *lsp;
{
	long blocks, size;
	register ftype;

	size = 0;
	ftype = lsp->ls_mode & S_IFMT;
	if (ftype==S_IFREG || ftype==S_IFDIR || ftype==S_IFPIP) {
		size = blocks = (lsp->ls_size+BSIZE-1)/BSIZE;
		if (blocks > ND) {
			size++;
			blocks -= ND;
			if (blocks > NBN*NI) {
				blocks -= NBN*NI;
				size += 2 + blocks/NBN;
			}
		}
	}
	printf("%4ld", size);
	return size;
}

/*
 * Print a time (if it is older than
 * one year) print the year instead
 * of the mm:ss part.
 */
prtime(tp)
register time_t *tp;
{
	register struct tm *tmp;
	register struct tm *now;
	register int thisyear;
	register char *cp;

	now = localtime(&curtime);
	thisyear = now->tm_year;
	cp = asctime(tmp = localtime(tp));
	if (thisyear > tmp->tm_year) {
		cp[10] = '\0';
		printf(" %s  %d ", cp, tmp->tm_year+1900);
	} else {
		cp[16] = '\0';
		printf(" %s ", cp);
	}
}

/*
 * Print 'rwx' type modes out.
 */
prmode(m, suid)
int m;
int suid;
{
	m <<= 6;
	putchar(m&S_IREAD ? 'r' : '-');
	putchar(m&S_IWRITE ? 'w' : '-');
	if (suid)
		putchar('s');
	else
		putchar(m&S_IEXEC ? 'x' : '-');
}

/*
 * Get a user name.  Either look
 * in password or group file depending
 * on 'gflag'.
 */
char *
getuname(uid)
short uid;
{
	register struct passwd	*pwp;
	static		id	= -1;
	static char	*name;

	if (uid == id)
		return name;
	id = uid;
	name = NULL;
	if ((pwp=getpwuid( uid)) != NULL)
		name = pwp->pw_name;
	return name;
}

/*
 * Get group name.
 * Look in group file .
 */
char *
getgname(uid)
short uid;
{
	register struct group	*grp;
	static		id	= -1;
	static char	*name;

	if (uid == id)
		return name;
	id = uid;
	name = NULL;
	if ((grp=getgrgid( uid)) != NULL)
		name = grp->gr_name;
	return name;
}

/*
 * List out the files in a directory
 * If 'fflag' is set, it may not be
 * but consider it one anyway.
 */
lsdir(dir,dirsize)
char *dir;
fsize_t dirsize;
{
	int fd, Cargs = 0, Rargs = 0;
	struct stat sb;
	LS *lsp;
	register char *np1, *np2;
	register int n;
	register struct direct *dp;
	register int nb;
	unsigned size;
	char 	*curname;
	char	*namebuf;		/* Buffer for constructing names */
	char	*dirbuf;		/* Buffer for reading directories */
	DLIST **dirlist;
	LS *dsaved;
	LS *dsavep;

	if ((fd = open(dir, 0)) < 0) {
		fprintf(stderr, "%s: cannot read\n", dir);
		return;
	}
	if (!fflag) {
		size = dirsize/sizeof (struct direct) * sizeof (LS);
		dsaved = alloc(size,"dsaved");
		dsavep = dsaved;
	}
	if (Rflag)
		dirlist = (char **)alloc(MAXDIR * sizeof(char *), "dirlist");
	dirbuf = alloc(BSIZE,"dirbuf");
	namebuf = alloc(MAXNBUF,"namebuf");
	curname = alloc(DIRSIZ+1,"curname");
	lsp = alloc(sizeof(LS),"lsp");
	if (mflag)
		first = 1;		/* For stream option */
	while ((nb = read(fd, dirbuf, BSIZE )) > 0)
	for (dp=dirbuf; dp<(dirbuf+nb); dp++) {
		if (dp->d_ino == 0)
			continue;
		np1 = dp->d_name;
		if (aflag == 0 && *np1++ == '.') {
			if (myuid != 0)
				continue;
			if (*np1=='\0' || (*np1++=='.' && *np1=='\0'))
				continue;
		}
		if (iflag) {
			sb.st_ino = dp->d_ino;
			canino(sb.st_ino);
		}
		np2 = curname;
		np1 = dp->d_name;
		n = DIRSIZ;
		do {
			*np2++ = *np1++;
		} while (--n);
		*np2 = '\0';
		if (lflag || xflag || Cflag || sflag || tflag || Rflag ||
		           sortflg || pflag || Fflag || oflag || nflag) {
			np2 = namebuf;
			np1 = dir;
			while (*np2++ = *np1++)
				;
			np2-=2;
			if (*np2++ != '/')
				*np2++ = '/';
			np1 = curname;
			while (*np2++ = *np1++)
				;
			if (stat(namebuf, &sb) < 0) {
				fprintf(stderr, "%s: cannot stat\n", curname);
				continue;
			}
		}
		convert(&sb, lsp);
		if (Rflag
		 && ((lsp->ls_mode & S_IFMT) == S_IFDIR)
		 && strcmp(curname, ".") != 0
		 && strcmp(curname, "..") != 0 ) {
			if (Rargs >= MAXDIR)
				fatal("too many subdirectories");
			dirlist[Rargs] = alloc(sizeof(DLIST),"dirlist");
			dirlist[Rargs]->name = alloc(strlen(namebuf) + 1,"name");
			strcpy(dirlist[Rargs]->name,namebuf);
			dirlist[Rargs++]->size = lsp->ls_size;
		} else if (fflag) {
			ncols = MAXLEN/(maxwidth+GAP);
			if (Cflag)
				svname(curname, Cargs++);
			else if (xflag)
				prcolh(curname);
			else
				prstuff(curname, lsp);
		} else {
			store(curname, &sb,dsavep++);
		}
	}
	free(lsp);
	free(curname);
	free(namebuf);
	free(dirbuf);
	if (!fflag) {
		ncols = MAXLEN/(maxwidth+GAP);
		output(dsaved,dsavep);
	} else if (Cflag && Cargs)
		prnames(Cargs);
	if (!fflag)
		for (lsp = dsaved; lsp < dsavep; lsp++)
			freename(lsp);
	if ((mflag && linesiz) || ((Cflag || xflag) && ccol)){
		putchar('\n');
		ccol = 0;
		linesiz = 0;
	}
	if (!fflag)
		free(dsaved);
	close(fd);
	maxwidth = MAXWID;
	if (Rflag && Rargs){
		for(n=0;n<Rargs;n++){
			putchar('\n');
			printf("%s:\n",dirlist[n]->name);
			lsdir(dirlist[n]->name,dirlist[n]->size);
			free(dirlist[n]->name);
		}
		for(n=0;n<Rargs;n++)
			free(dirlist[n]);
	}
	if (Rflag)
		free(dirlist);
}

/*
 * Store data away for intra-directory
 * sorting.
 */
store(name, sbp,lsp)
char *name;
register struct stat *sbp;
register LS *lsp;
{
	convert(sbp, lsp);
	if (xflag || Cflag)
		cstore(lsp,name);
	strncpy(lsp->ls_dname, name, DIRSIZ+1);
}

/*
 * Store each argument away for inter-directory sorting.
 * Sets lsp->ls_name to allocated name.
 */
astore(name, sbp,lsp)
char *name;
register struct stat *sbp;
register LS *lsp;
{
	convert(sbp, lsp);
	if (xflag || Cflag)
		cstore(lsp,name);
	lsp->ls_name = alloc(strlen(name)+1,"ls->name");
	strcpy(lsp->ls_name, name);
}

/*
 * Free allocated name fields.
 */
freename(lsp) register LS *lsp;
{
	if (lsp->ls_name != NULL)
		free(lsp->ls_name);
	if (lsp->ck_name != NULL)
		free(lsp->ck_name);
}

/*
 * Convert a stat buffer into an ls store
 * buffer.
 */
convert(sbp, lsp)
register struct stat *sbp;
register LS *lsp;
{
	lsp->ls_ino = sbp->st_ino;
	lsp->ls_mode = sbp->st_mode;
	lsp->ls_nlink = sbp->st_nlink;
	lsp->ls_uid = sbp->st_uid;
	lsp->ls_gid = sbp->st_gid;
	lsp->ls_size = sbp->st_size;
	lsp->ls_rdev = sbp->st_rdev;
	if (tflag == CTIME)
		lsp->ls_time = sbp->st_ctime;
	else if (tflag == MTIME)
		lsp->ls_time = sbp->st_mtime;
	else if (tflag == ATIME)
		lsp->ls_time = sbp->st_atime;
}

/*
 * Sort, output and free up space from
 * the current directory being considered.
 */
output(start,stop)
LS *start;
LS *stop;
{
	register LS *lsp, *lse;
	register unsigned nel;
	int Cargs = 0;
	nel = stop - start;
	qsort(start, nel, sizeof (LS), qcomp);
	for (lsp=start, lse=stop; lsp < lse; lsp++){
		if (Cflag)
			svname(lsp->ck_name,Cargs++);
		else if (xflag)
			prcolh(lsp->ck_name);
		else
			prstuff(lsp->ls_dname, lsp);
	}
	if (Cflag && Cargs)
		prnames(Cargs);
}

/*
 * The following are the three qsort comparison routines:
 * sort by time, sort by directory name, sort by full pathname.
 * They can be used both in the argument sort and in each directory sort.
 */

/*
 * Sort by time (either access, modify, or create setup elsewhere)
 * (forward or backward).
 */
qtcomp(lsp1, lsp2)
register LS *lsp1, *lsp2;
{
	register int rval = 0;

	if (lsp1->ls_time < lsp2->ls_time)
		rval++;
	else if (lsp1->ls_time > lsp2->ls_time)
		rval--;
	return (rflag) ? -rval : rval;
}

/*
 * Sort by directory name.
 * (forward or reverse).
 */
qdncomp(lsp1, lsp2)
LS *lsp1, *lsp2;
{
	register int rval;

	rval = strncmp(lsp1->ls_dname, lsp2->ls_dname, DIRSIZ);
	return (rflag) ? -rval : rval;
}

/*
 * Sort by full pathname.
 * (forward or reverse).
 */
qncomp(lsp1, lsp2)
LS *lsp1, *lsp2;
{
	register int rval;

	rval = strcmp(lsp1->ls_name, lsp2->ls_name);
	return (rflag) ? -rval : rval;
}

usage()
{
	fprintf(stderr, "Usage: ls [-abcCdfFgilmnopqrRstux] [files ...]\n");
	exit(1);
}

is_x(m)
int m;
{
	m <<= 6;
	return m&S_IEXEC;
}

/*
 * Print non-graphic chars in octal or '?' format 
 * or as they are depending on which flag is set.
 */
prname(s)
char *s;
{
	if (bflag) {
		while ( *s != '\0')
			if ( *s >= ' ' && *s <= '~')
				putchar(*s++);
			else
				printf("\\0%o",*s++);
	} else if (qflag) {
		while ( *s != '\0')
			if ( *s >= ' ' && *s <= '~')
				putchar(*s++);
			else {
				putchar('?');
				*s++;
			}
	} else
		printf(s);
}

/*
 *  Print in stream format for -m option
 */
prstream(s,m)
char *s;
short m;
{
	register int sl;

	sl = strlen(s);
	if ( (linesiz + sl) > MAXLEN ){
		putchar(',');
		putchar('\n');
		prname(s);
		linesiz = sl + prflag(m) + 1;
	}
	else if (!first) {
		printf(", ");
		prname(s);
		linesiz += sl + prflag(m) + 2;
	} else {
		first = 0;
		prname(s);
		linesiz = sl + prflag(m);
	}
}

/*
 * Prints flag if executable or directory and -F or -p flsg is set.
 */
prflag(m)
short m;
{
	if (pflag)
		if((m & S_IFMT) == S_IFDIR){
			putchar('/');
			return 1;
		}
	if (Fflag) {
		if((m & S_IFMT) == S_IFDIR) {
			putchar('/');
			return 1;
		} else if (is_x(m>>6 & 07) ) {
			putchar('*');
			return 1;
		} else if( is_x(m>>3 & 07) ) {
			putchar('*');
			return 1;
		} else if( is_x(m & 07) ){
			putchar('*');
			return 1;
		}
	}
	return 0;
}

/*
 * Returns flag if executable or directory and -F or -p flsg is set.
 */
char *
getflag(m)
short m;
{
	if (pflag) {
		if((m & S_IFMT) == S_IFDIR)
			return "/";
	}
	if (Fflag) {
		if((m & S_IFMT) == S_IFDIR)
			return "/";
		else if( is_x(m>>6 & 07) )
			return "*";
		else if( is_x(m>>3 & 07) )
			return "*";
		else if( is_x(m & 07) )
			return "*";
	}
	return "";
}

/*
 * Returns file name with non-graphic chars in octal or '?' format 
 * or as they are depending on which flag is set.
 */
char *
getname(s)
char *s;
{
	char tmp[80];
	char *t;
	int i = 0;
	char buf[5];

	t = tmp;
	if (bflag){
		while (*s) {
			if (*s >= ' ' && *s <= '~') {
				*t++ = *s++;
			} else {
				sprintf(buf,"\\0%o",*s++);
				while(buf[i] != '\0'){
					*t++ = buf[i];
					buf[i++] = '\0';
				}
				i=0;
			}
		}
		*t = '\0';
	} else if (qflag){
		while ( *s != '\0')
			if ( *s >= ' ' && *s <= '~')
				*t++ = *s++;
			else{
				*t++ = '?';
				*s++;
			}
		*t = '\0';
	} else
		strcpy(tmp,s);
	return tmp;
}

/*
 * Prints name in multi-column format across the screen (-x option).
 */
prcolh(name)
char *name;
{	
	if ( ccol < ncols)
		printf(name);
	if ( ++ccol <  ncols)
		printf("%*s",maxwidth - strlen(name) + GAP,"");
	else {
		ccol = 0;
		putchar('\n');
	}
}

/*
 * Sets chars in buffer to NULs.
 */
clear(s)
char *s;
{
	while (*s != '\0')
		*s++ = '\0';
}

/*
 * Adds whatever information to file name as indicated by flags
 * (i.e. iflag, sflag, bflag) and stores it in ck_name so its
 * entire length can be recorded.  Used for -x and -C.
 * Sets lsp->ls_name to allocated name.
*/
cstore(lsp, name)
LS *lsp;
char *name;
{
	register int sl;
	char buf[80],tmp[80],tmp2[80];

	clear(buf); clear(tmp); clear(tmp2);
	if ( sflag)
		sprintf(buf,"%3d",getsize(lsp));
	if (iflag){
		sprintf(tmp,"%5u ", lsp->ls_ino);
  		strcat(buf,tmp);
	}
	strcpy(tmp2,getname(name));
	strcat(buf,tmp2);
	if ( Fflag || pflag)
		strcat(buf,getflag(lsp->ls_mode));
	sl = strlen(buf);
	if (maxwidth < sl)
		maxwidth = sl;
	lsp->ck_name = alloc(sl+1, "ck_name");
	strcpy(lsp->ck_name,buf);
}

/*
 * Returns size of file.  see prsize.
 */
getsize(lsp)
register LS *lsp;
{
	long blocks, size;
	register ftype;

	size = 0;
	ftype = lsp->ls_mode & S_IFMT;
	if (ftype==S_IFREG || ftype==S_IFDIR || ftype==S_IFPIP) {
		size = blocks = (lsp->ls_size+BSIZE-1)/BSIZE;
		if (blocks > ND) {
			size++;
			blocks -= ND;
			if (blocks > NBN*NI) {
				blocks -= NBN*NI;
				size += 2 + blocks/NBN;
			}
		}
	}
	return size;
}

/*
 * Save file name for -C option.
 * This allocates namelist[i], prnames() eventually frees it.
 */
svname(name,i)
char *name;
int i;
{
	if (i >= MAXARG)
		fatal("more than %d files with -C option", MAXARG);
	namelist[i] = alloc(strlen(name)+1,"svname");
	strcpy(namelist[i],name);
}

/*
 * Print names in multi-column format going down the screen (-C option).
 * Free the namelist members allocated by svname.
 */
prnames(Cargs)
int Cargs;
{
	register int nc, i, j, nl;

	ncols = MAXLEN/(maxwidth+GAP);
	nc = ncols;
	nlast = Cargs % ncols;
	nrows = Cargs/ncols + (nlast ? 1 : 0);
	offset = nrows -1;

	if (nlast == 0)
		nlast = ncols;
	nl = nlast;	
	for(i=0; i < offset; i++){
		for(j = i; j < Cargs;j+=offset+ (nl-- >0) )
			prcolh(namelist[j]);
		nl = nlast;
	}
	while (nlast--){
		prcolh(namelist[i]);
		i += nrows;
	}
	if (ccol){
		putchar('\n');
		ccol = 0;
	}
	for (i = 0; i < Cargs; i++)
		free(namelist[i]);
}

/*
 * Put message and die.
 */
/* VARARGS */
void
fatal(s) char *s;
{
	fprintf(stderr, "\nls: fatal: %r\n", &s);
	exit(1);
}

/*
 * Get space or die.
 */
char *
alloc(n, msg) register int n; char *msg;
{
	extern char *calloc();
	register char *cp;

	if ((cp = calloc(n, 1)) == NULL)
		fatal("out of space: %s", msg);
	return cp;
}

/* end of ls.c */
