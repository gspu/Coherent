/*
 * The line printer spooler daemon.
 * This should be invoked from the `/etc/rc'
 * file and by the `lpr' command.
 * (NOTE: this command should be setuid to daemon)
 */

#include <stdio.h>
#include <sys/dir.h>
#include <pwd.h>
#include <sgtty.h>
#include <signal.h>
#include <errno.h>

#define DAEMON	1		/* Daemon's magic number */
#define	BANWID	10		/* Longest banner */
#define	MAXCOM	40		/* Longest comment line sent through mail */
#define	FF	014		/* Form feed */
#define	SMALL	037		/* Set small characters -- paper tiger */
#define	NORMAL	036		/* Set normal characters -- paper tiger */

#ifdef LASER
char	spooldir[] = "/usr/spool/hpd";
char	*printer = "/dev/hp";
#else
char	spooldir[] = "/usr/spool/lpd";
char	*printer = "/dev/lp";
#endif
char	lockfile[] = "dpid";

char	*argv0;
char	obuf[BUFSIZ];
char	cfline[300];		/* Control file line */
char	comment[300];		/* Comment line */
FILE	*lp;
int	printing;		/* On while printing */

char	*lgets();
int	cancel();
int	restart();

main(argc, argv)
char *argv[];
{
	int fd;
	int pid;

	argv0 = argv[0];
	setuid(DAEMON);
	if (chdir(spooldir) < 0)
		lperr("spool directory botch");
	if ((fd = creat(lockfile, 0400)) < 0)
		exit(0);
	if (fork())
		exit(0);
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGTRAP, cancel);
	signal(SIGREST, restart);
	pid = getpid();
	write(fd, &pid, sizeof pid);
	close(fd);
	if (argc > 1)
		printer = argv[1];
	if ((lp = fopen(printer, "w")) == NULL)
		lperr( "%s: %s", printer, sys_errlist[errno]);
	ioctl(fileno(lp), TIOCEXCL, NULL);
	setbuf(lp, obuf);
	process();
	rmexit(0);
}

/*
 * Find work to do.
 * The basic algorithm is to
 * run down the current directory
 * looking for files whose names
 * begin with `cf' and do the work
 * associated with each of them.
 */
process()
{
	FILE *dirfile;
	struct direct dir;
	char *r = "r";

	if ((dirfile = fopen(".", r)) != NULL) {
		while (fread(&dir, sizeof dir, 1, dirfile) == 1) {
			if (dir.d_ino == 0
			 || dir.d_name[0] != 'c'
			 || dir.d_name[1] != 'f')
				continue;
			control(dir.d_name);
			rewind(dirfile);
		}
		fclose(dirfile);
	}
}

/*
 * Do the action specified by the control
 * file.  Essentially, this must interpret
 * the control lines for such things as
 * mail notification, banners, etc.
 */
control(cfname)
register char *cfname;
{
	FILE *cfp;
	char mbuf[MAXCOM+40];
	char *message = "%s: Listing complete: %.*s\n";
	int state;	/* 0 to suppress header, 1 before first banner, 2 after */

	if ((cfp = fopen(cfname, "r")) != NULL) {
again:
		printing = state = 1;
		while (lgets(cfline, sizeof cfline, cfp) != NULL) {
			if (!printing)
				message = "%s: Listing killed by order: %.*s\n";
			else if (printing < 0) {
				rewind(cfp);
				putc(FF, lp);
				goto again;
			}
			switch (cfline[0]) {
			case 'A':
				if (state)
					putc(FF, lp);	/* FF after banner */
				if (print(cfline+1)) {
					message = "%s: Printer file error: %.*s\n";
					strcpy(comment, cfline+1);
				}
				putc(FF, lp);		/* FF after file */
				break;

			case 'B':
				state = 0;		/* suppress header */
				break;

			case 'D':
				strcpy(comment, cfline+1);
				break;

			case 'L':
				if (state == 0)
					break;
				if (state == 1) {
					state = 2;
					fprintf(lp, "%s\n\n", cfname);
				}
				if (printing > 0) {
					cfline[BANWID+1] = '\0';
/* Paper tiger controls
					putc(SMALL, lp);
 */
					banner(cfline+1, lp);
/* Paper tiger controls
					putc(NORMAL, lp);
 */
				}
				break;

			case 'M':
				sprintf(mbuf, message, argv0, MAXCOM, comment);
				notify(cfline+1, mbuf);
				break;

			case 'R':
				/* Charge ID -- not used */
				break;

			case 'U':
				unlink(cfline+1);
				break;

			default:
				lperr("Bad control line `%s'", cfline);
			}
		}
		fclose(cfp);
		printing = 0;
	}
	unlink(cfname);
}

/*
 * Routine to notify a user about
 * the completion of a transaction
 * Usually called by some daemon (e.g.
 * line printer daemon).
 * Return non-zero on failure.
 */
notify(name, msg)
char *name;
char *msg;
{
	register struct passwd *pwp;
	int pfd[2];
	register int pid, fd;
	int status;

	if (*name>='0' && *name<='9')
		if ((pwp = getpwuid(atoi(name))) == NULL)
			name = NULL; else
			name = pwp->pw_name;
	if (name==NULL || pipe(pfd)<0 || (pid = fork())<0)
		return (1);
	if (pid) {
		close(pfd[0]);
		write(pfd[1], msg, strlen(msg));
		close(pfd[1]);
		while (wait(&status) >= 0)
			;
	} else {
		close(pfd[1]);
		dup2(pfd[0], 0);
		close(pfd[0]);
		for (fd=3; fd<_NFILE; fd++)
			close(fd);
		execlp("/bin/mail", "mail", name, NULL);
		return (1);
	}
	return (0);
}

/*
 * Cancel the listing that is printing
 * just now.
 */
cancel()
{
	signal(SIGTRAP, SIG_IGN);
	if (printing) {
		printing = 0;
		fprintf(lp, "\n\n\nListing cancelled by order\n");
	}
	signal(SIGTRAP, cancel);
}

/*
 * Restart the listing that
 * is running just now.
 */
restart()
{
	signal(SIGREST, SIG_IGN);
	if (printing) {
		printing = -1;
		fprintf(lp, "\n\n\nListing restarted by order\n");
	}
	signal(SIGREST, restart);
}

/*
  * Like fgets but no newline is put at the end
 * of the string.  Also the rest of an input
 * line is flushed on truncated strings.
 */
char *
lgets(as, lim, iop)
char *as;
register lim;
FILE *iop;
{
	register c;
	register char *s;

	s = as;
	while (--lim > 0 && (c = getc(iop)) != EOF)
		if ((*s++ = c) == '\n') {
			s--;
			break;
		}
	*s = 0;
	if (lim==0 && c!='\n')
		while ((c = getc(iop))!='\n' && c!=EOF)
			;
	return (c==EOF && s==as ? NULL : as);
}

/* VARARGS */
lperr(x)
{
	fprintf(stderr, "%s: %r\n", argv0, &x);
	rmexit(1);
}

/*
 * Remove lock file and exit.
 */
rmexit(s)
{
	unlink(lockfile);
	if (lp != NULL)
		fclose(lp);
	exit(s);
}
