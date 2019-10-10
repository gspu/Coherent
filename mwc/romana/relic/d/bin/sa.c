/*
 * System accounting
 * of command execution (in
 * conjuction with acct system
 * call.)
 */

#include <stdio.h>
#include <ctype.h>
#include <pwd.h>
#include <acct.h>
#include <sys/times.h>
#include <sys/dir.h>
#include <sys/machine.h>

#define	MIN	60		/* Seconds in a minute */
#define	MINHZ	(MIN*HZ)	/* HZ in a minute */
#define	CPUTIME	0		/* Sort by CPU time */
#define	PERCALL	1		/* Sort by CPU time per call */
#define	CALLS	2		/* Sort by numbers of calls */

#define	NUSER	100		/* Maximum number of users */
#define	NCOMM	500		/* Maximum number of commands */
#define	NSORT	NUSER
#define	NCNAME	(sizeof(ac.ac_comm))	/* Size of a command name) */

struct	acct	ac;

/*
 * structure of /usr/adm/usracct
 * for per-user type of `sa' information.
 */
struct	svu	{
	unsigned svu_count;		/* Number of processes measured */
	short	svu_uid;		/* User number */
	time_t	svu_stime;		/* Total system time (HZ) */
	time_t	svu_utime;		/* Total user time */
	time_t	svu_etime;		/* Total elapsed time  (sec.)*/
}	svu[NUSER];

/*
 * For per command information as
 * is stored in /usr/adm/savacct.
 */
struct	svc	{
	unsigned svc_count;		/* Number of calls */
	char	svc_comm[NCNAME];	/* Command name */
	time_t	svc_stime;
	time_t	svc_utime;
	time_t	svc_etime;
}	svc[NCOMM];
struct	svc	*junkp, *otherp;

/*
 * For the final sort.  This is
 * the combined information of
 * the two structures above.
 * I.e. either one or the other.
 */
struct	sort {
	unsigned s_count;		/* Number of calls */
	char	s_comm[NCNAME];		/* Command name */
	short	s_uid;			/* User number */
	time_t	s_key;			/* Sort key */
	time_t	s_stime;		/* System time */
	time_t	s_utime;		/* User time */
	time_t	s_etime;		/* Elapsed time */
}	sort[NSORT];

char	*acctf = "/usr/adm/acct";		/* Raw accounting file */
char	*sacctf = "/usr/adm/savacct";		/* Summary file */
char	*uacctf = "/usr/adm/usracct";		/* Per-user summary */
char	junk[NCNAME] = "**junk**";
char	other[NCNAME] = "***other";

int	aflag;			/* Move unlikely commands to "**other" */
int	cflag;			/* Give percentage of total time */
int	jflag;			/* Give seconds instead of minutes/call */
int	lflag;			/* Separate system & user times */
int	mflag;			/* Number of processes and CPU min. per user */
int	rflag;			/* Reverse sort order */
int	sflag;			/* Merge accounting file when done */
int	tflag;			/* Print ratio of real to CPU time */
int	uflag;			/* Print user-id and command name */
int	vflag;			/* Prompt if used fewer than `n' times */
int	sortflag = CPUTIME;	/* Sort mode */

long	ctol();
time_t	units();
struct	svc	*make();
struct	svc	*move();
char	*uname();
int	compar();

main(argc, argv)
char *argv[];
{
	register char *ap;

	while (argc>1 && *argv[1]=='-') {
		for (ap = &argv[1][1]; *ap != '\0'; ap++)
			switch (*ap) {
			case 'a':
				aflag = 1;
				break;

			case 'b':
				sortflag = PERCALL;
				break;

			case 'c':
				cflag = 1;
				break;

			case 'j':
				jflag = 1;
				break;

			case 'l':
				lflag = 1;
				break;

			case 'm':
				mflag = 1;
				break;

			case 'n':
				sortflag = CALLS;
				break;

			case 'r':
				rflag = 1;
				break;

			case 's':
				sflag = 1;
				break;

			case 't':
				tflag = 1;
				break;

			case 'u':
				uflag = 1;
				break;

			case 'v':
				if (ap[1]>='0' && ap[1]<='9')
					vflag = *++ap - '0';
				break;

			default:
				usage();
			}
		argc--;
		argv++;
	}
	if (argc == 2)
		acctf = argv[1];
	else if (argc > 2)
		usage();
	if (!uflag)
		rsummary();
	rraw(acctf);
	if (sflag && !uflag)
		samerge();
	if (!uflag)
		saprint();
	exit(0);
}

/*
 * Read the summary file of old accounting information
 * from both user and command saved accounting files.
 */
rsummary()
{
	register FILE *fp;

	if ((fp = fopen(sacctf, "r")) != NULL) {
		register struct svc *svcp;

		for (svcp = svc; svcp < &svc[NCOMM]; svcp++)
			if (fread(svcp, sizeof *svcp, 1, fp) != 1)
				break;
		if (svcp >= &svc[NCOMM])
			fprintf(stderr, "%s is too large\n", sacctf);
		fclose(fp);
	}
	if ((fp = fopen(uacctf, "r")) != NULL) {
		register struct svu *svup;

		for (svup = svu; svup < &svu[NUSER]; svup++)
			if (fread(svup, sizeof *svup, 1, fp) != 1)
				break;
		if (svup >= &svu[NUSER])
			fprintf(stderr, "%s is too large\n", uacctf);
		fclose(fp);
	}
}

/*
 * Read the raw accounting.
 */
rraw(af)
char *af;
{
	FILE *afp;
	register struct svc *svcp;

	if ((afp = fopen(af, "r")) == NULL) {
		fprintf(stderr, "Cannot open raw accounting file `%s'\n", af);
		exit(1);
	}
	while (fread(&ac, sizeof ac, 1, afp) == 1) {
		if (uflag)
			printf("%-*s  %.*s\n", DIRSIZ, uname(ac.ac_uid),
			    NCNAME, ac.ac_comm);
		else {
			senter(&ac);
			uenter(&ac);
		}
	}
	fclose(afp);
	/*
	 * Create (if not there) junk and other classes.
	 */
	if (aflag) {
		otherp = make(other);
		for (svcp = svc; svcp<&svc[NCOMM] && svcp->svc_count; svcp++) {
			if (svcp == otherp)
				continue;
			if (svcp->svc_count==1 || unprintable(svcp->svc_comm))
				otherp = move(svcp--, otherp);
		}
	}
	if (vflag) {
		junkp = make(junk);
		for (svcp = svc; svcp<&svc[NCOMM] && svcp->svc_count; svcp++) {
			if (svcp == junkp)
				continue;
			if (svcp->svc_count<=vflag && yes(svcp->svc_comm))
				junkp = move(svcp--, junkp);
		}
	}
}

/*
 * Merge system accounting info back
 * into the two merged files and
 * truncate the raw accounting file.
 * Accounting probably should be
 * turned off when `sa' is called
 * if this is to be done.
 */
samerge()
{
	register FILE *fp;

	if ((fp = fopen(sacctf, "w")) != NULL) {
		register struct svc *svcp;

		for (svcp = svc; svcp<&svc[NCOMM] && svcp->svc_count; svcp++)
			if (fwrite(svcp, sizeof *svcp, 1, fp) != 1)
				saerr("%s: write error", sacctf);
		fclose(fp);
	} else
		saerr("cannot rewrite %s", sacctf);
	if ((fp = fopen(uacctf, "w")) != NULL) {
		register struct svu *svup;

		for (svup = svu; svup<&svu[NUSER] && svup->svu_count; svup++)
			if (fwrite(svup, sizeof *svup, 1, fp) != 1)
				saerr("%s: write error", uacctf);
		fclose(fp);
	} else
		saerr("cannot rewrite %s", uacctf);
	if ((fp = fopen(acctf, "w")) == NULL)
		saerr("cannot truncate %s", acctf);
	fclose(fp);
}

/*
 * Output the accounting
 * information according to
 * sorting and printing options.
 */
saprint()
{
	register struct sort *sp;
	time_t tottime;

	if (mflag)
		userenter(); else
		commenter();
	for (sp = sort; sp<&sort[NSORT] && sp->s_count; sp++)
		if (sortflag == CPUTIME)
			sp->s_key = sp->s_stime+sp->s_utime;
		else if (sortflag == PERCALL)
			sp->s_key = (sp->s_stime+sp->s_utime)/sp->s_count;
		else		/* # calls */
			sp->s_key = sp->s_count;
	qsort(sort, sp-sort, sizeof *sp, compar);
	if (mflag)
		printf("%-*s #PROC", DIRSIZ, ""); else
		printf("%-*s #CALL", NCNAME, "");
	if (lflag)
		printf("  USER  SYS"); else
		printf("  CPU");
	printf("  REAL");
	if (cflag)
		printf(" CPU %% ");
	if (tflag)
		printf(" CPU/REAL %%");
	putchar('\n');
	if (cflag) {
		tottime = 0;
		for (sp = sort; sp<&sort[NSORT] && sp->s_count; sp++) {
			tottime += sp->s_stime;
			tottime += sp->s_utime;
		}
	}
	for (sp = sort; sp<&sort[NSORT] && sp->s_count; sp++) {
		if (mflag)
			printf("%-*s", DIRSIZ, uname(sp->s_uid)); else
			printf("%-*s", NCNAME, sp->s_comm);
		printf(" %5d", sp->s_count);
		if (lflag)
			printf(" %5ld %5ld", units(sp->s_utime, sp),
			    units(sp->s_stime, sp));
		else
			printf(" %5ld", units(sp->s_utime+sp->s_stime, sp));
		printf(" %5ld", units(sp->s_etime*HZ, sp));
		if (cflag)
			percent(sp->s_utime+sp->s_stime, tottime);
		if (tflag) {
			printf("  ");
			percent(sp->s_utime+sp->s_stime, sp->s_etime*HZ);
		}
		putchar('\n');
	}
}

/*
 * Enter user information for the sort.
 */
userenter()
{
	register struct sort *sp;
	register struct svu *svup;

	sp = sort;
	svup = svu;
	while (svup < &svu[NCOMM] && svup->svu_count) {
		sp->s_count = svup->svu_count;
		sp->s_uid = svup->svu_uid;
		sp->s_stime = svup->svu_stime;
		sp->s_utime = svup->svu_utime;
		sp->s_etime = svup->svu_etime;
		sp++;
		svup++;
	}
/*
	free(svc);
 */
}

/*
 * Enter the commands into the list for
 * sorting.
 */
commenter()
{
	register struct sort *sp;
	register struct svc *svcp;

	sp = sort;
	svcp = svc;
	while (svcp < &svc[NCOMM] && svcp->svc_count) {
		sp->s_count = svcp->svc_count;
		strncpy(sp->s_comm, svcp->svc_comm, NCNAME);
		sp->s_stime = svcp->svc_stime;
		sp->s_utime = svcp->svc_utime;
		sp->s_etime = svcp->svc_etime;
		sp++;
		svcp++;
	}
/*
	free(svu);
 */
}

/*
 * Enter this accounting entry into
 * the command table for savacct.
 */
senter(ap)
register struct acct *ap;
{
	register struct svc *svcp;

	for (svcp = svc; svcp<&svc[NCOMM] && svcp->svc_count; svcp++)
		if (strncmp(svcp->svc_comm, ap->ac_comm, NCNAME) == 0)
			break;
	if (svcp >= &svc[NCOMM])
		saerr("Command table overflow");
	if (svcp->svc_count++ == 0)
		strncpy(svcp->svc_comm, ap->ac_comm, NCNAME);
	svcp->svc_stime += ctol(ap->ac_stime);
	svcp->svc_utime += ctol(ap->ac_utime);
	svcp->svc_etime += ctol(ap->ac_etime);
}

/*
 * Enter this accounting entry into
 * the user table for usracct.
 */
uenter(ap)
register struct acct *ap;
{
	register struct svu *svup;

	for (svup = svu; svup<&svu[NUSER] && svup->svu_count; svup++)
		if (svup->svu_uid == ap->ac_uid)
			break;
	if (svup >= &svu[NUSER])
		saerr("User table overflow");
	svup->svu_count++;
	svup->svu_uid = ap->ac_uid;
	svup->svu_stime += ctol(ap->ac_stime);
	svup->svu_utime += ctol(ap->ac_utime);
	svup->svu_etime += ctol(ap->ac_etime);
}

/*
 * Make a new zero entry with
 * the indicated name.
 */
struct svc *
make(name)
register char *name;
{
	register struct svc *svcp;

	for (svcp = svc; svcp<&svc[NCOMM] && svcp->svc_count; svcp++)
		if (strncmp(name, svcp->svc_comm, NCNAME) == 0)
			return (svcp);
	if (svcp >= &svc[NCOMM])
		saerr("out of room for %s", name);
	strncpy(svcp->svc_comm, name, NCNAME);
	return (svcp);
}

/*
 * Move an entry to one of the deprecated
 * places.
 */
struct svc *
move(svcp, depp)
register struct svc *svcp;
struct svc *depp;
{

	depp->svc_count += svcp->svc_count;
	depp->svc_stime += svcp->svc_stime;
	depp->svc_utime += svcp->svc_utime;
	depp->svc_etime += svcp->svc_etime;
	while (svcp->svc_count) {
		if (svcp == depp)
			depp--;
		bcopy(svcp+1, svcp, sizeof *svcp);
		svcp++;
	}
	(svcp-1)->svc_count = 0;
	return (depp);
}

/*
 * Return the user-name for
 * a user-ID.
 */
char *
uname(uid)
short uid;
{
	register struct passwd *pwp;
	static char ubuf[15];

	if ((pwp = getpwuid(uid)) != NULL)
		return (pwp->pw_name);
	return (sprintf(ubuf, "%d", uid));
}

/*
 * Block copy of `n' bytes.
 */
bcopy(f, t, n)
register char *f, *t;
register unsigned n;
{
	if (n)
		do {
			*t++ = *f++;
		} while (--n);
}

/*
 * Return non-zero if an NCNAME-length string
 * is unprintable.
 */
unprintable(s)
register char *s;
{
	register int n = NCNAME;
	register int c;

	do {
		if ((c = *s++) == '\0')
			break;
		if (!isascii(c) || !isprint(c))
			return (1);
	} while (--n);
	return (0);
}

/*
 * Ask whether or not to delete and
 * entry.  Return 1 if yes.
 */
yes(s)
register char *s;
{
	register int c, ans = 0;

	printf("%.*s ? ", NCNAME, s);
	if ((c = getchar()) == 'y')
		ans = 1;
	while (c!='\n' && c!=EOF)
		c = getchar();
	return (ans);
}

/*
 * Qsort compare routine.
 */
compar(sp1, sp2)
register struct sort *sp1, *sp2;
{
	register int rval;

	if (sp1->s_key == sp2->s_key)
		return (0);
	rval = 1;
	if (sp1->s_key > sp2->s_key)
		rval = -1;
	if (rflag)
		return (-rval);
	return (rval);
}

/*
 * Return the number of CPU minutes from CPU HZ.
 * or if `-j', return seconds/call.
 */
time_t
units(hz, sp)
time_t hz;
register struct sort *sp;
{
	if (jflag)
		return ((hz + HZ/2) / (HZ*sp->s_count)); else
		return ((hz + MINHZ/2) / MINHZ);
}

/*
 * Print the percentage of
 * `t' out of `total'.
 */
percent(t, total)
time_t t, total;
{
	t *= 100;
	printf("%3ld.", t/total);
	t %= total;
	if (t < 0)
		t = -t;
	printf("%1ld ", t*10/total);
}

usage()
{
	fprintf(stderr, "Usage: sa [-abcjlmnrstu] [-v[n]] [file]\n");
	exit(1);
}

/* VARARGS */
saerr(x)
{
	fprintf(stderr, "sa: %r", &x);
	putc('\n', stderr);
	exit(1);
}
