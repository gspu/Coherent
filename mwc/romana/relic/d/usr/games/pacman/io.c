#include <signal.h>
#include <sgtty.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
 * Conditionalized by steve 6/10/92 for _I386.
 * I didn't bother to clean up the way the i8086 version works;
 * it too would be vastly more efficient if it passed both the
 * character and attribute to a single call rather than calling sout() twice.
 */
#if	_I386
char	outbuf[2];	/* character and attribute */
#define	atrb(a)		{outbuf[1] = (a);}
#define	bufchr(c)	{outbuf[0] = (c); sout2();}
#else
int			a_n;
#define	atrb(a)		{a_n=a;}
#define	bufchr(c)	{sout(caddr++, (c)); sout(caddr++, (a_n));}
#endif

#define	bflush()
#define	bufstr(s)	{register char *pc; pc=s; while (*pc) bufchr(*pc++);}
#define	beep()		write(2, "\7", 1)

struct	sgttyb	sgttyb, oldsgttyb;
struct	stat	sbuf;
int	pv[3];

#if	_I386
/*
 * Write character and attribute to display memory directly.
 * The character and attribute are in outbuf[].
 * The display memory offset is in caddr, which gets bumped.
 * This presumes that the process can write /dev/mem.
 */
sout2()
{
	static unsigned int offset;
	static int mfd;
	extern unsigned int scrbase;

	if (mfd == 0) {
		/* First time, open /dev/mem. */
		if ((mfd = open("/dev/mem", 1)) ==  -1)
			error("cannot open /dev/mem");
		if (lseek(mfd, scrbase, 0) != scrbase)
			error("seek failed");
	}
	if (caddr != offset && lseek(mfd, scrbase + caddr, 0) == -1)
		error("seek failed");
	if (write(mfd, outbuf, 2) != 2)
		error("write failed");
	offset = caddr += 2;
}
#endif

cls()
{
#if	_I386
	outbuf[0] = ' ';
	outbuf[1] = 7;
	for (caddr = 0; caddr < 4000; )
		sout2();
#else
	register int c;

	for (c = 0; c < 4000; ) {
		sout (c++, ' ');
		sout (c++, 7);
	}
#endif
}

reset()
{

	ioctl(1, TIOCSETP, &oldsgttyb);
	kill (pv[2], SIGTERM);
	cls();
	exit (0);
}

setup()
{
	struct stat cstat, ostat;

	/* Make sure the user is running from the console keyboard. */
	if (stat("/dev/console", &cstat) == -1)
		error("cannot stat /dev/console");
	else if (fstat(1, &ostat) == -1)
		error("cannot stat standard output");
	else if (cstat.st_dev  != ostat.st_dev
	      || cstat.st_mode != ostat.st_mode
	      || cstat.st_rdev != ostat.st_rdev)
		error("Sorry, you can only play from the console keyboard.");
	if (pipe(pv) < 0)
		error("cannot create pipe");
	if ((pv[2]=fork()) < 0)
		error("cannot fork");
	if (pv[2] == 0) {
		dup2(pv[1], 1);
		close(pv[0]);
		close(pv[1]);
		chdir("/tmp");
		execl("/bin/cat", "", "-u", 0);
		exit (1);
	}
	signal(SIGINT, reset);
	dup2(pv[0], 0);
	close(pv[0]);
	close(pv[1]);
	ioctl(1, TIOCGETP, &oldsgttyb);
	sgttyb = oldsgttyb;
	sgttyb.sg_flags &= ~ECHO;
	sgttyb.sg_flags |= CBREAK | RAWOUT;
	ioctl(1, TIOCSETP, &sgttyb);
	cls();
}

static	char	crlf[2] = { '\r', '\n' };
error(s)
char *s;
{
	register int i;
	register char *t;

	for (i=0, t = s; *t++; )
		++i;
	write(2, s, i);
	write(2, crlf, 2);
	exit (1);
}

input(c)
register char *c;
{
	register n;

	fstat(0, &sbuf);
	n = (int)sbuf.st_size;
	if (n <= 0)
		return (0);
	while (n--)
		read(0, c, 1);
	return (1);
}

inputecho(c)
register char *c;
{
	while (input(c) == 0)
		;
	if (*c == '\n')
		return;
	bufchr(*c);
	bflush();
}

delay(t)
register t;
{
	extern int ndelay;
	register i;

	if (ndelay)
		t *= ndelay;
	while (t--)
		for (i=0; i<4096; ++i)
			;
}
