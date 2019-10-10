/*
 * File:	asypatch.c
 *
 * Purpose:	read a spec from stdin and patch the asy driver
 *
 * $Log:	asypatch.c,v $
 * Revision 1.1  92/07/17  15:28:54  bin
 * Initial revision
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <stdio.h>
#include <termio.h>
#include <sys/ktty.h>
#include <sys/asy.h>
#ifdef _I386
#include <coff.h>
#endif

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define BSZ		256
#ifdef _I386
#define MAKEPATCH(a1,a2,a3,a4,a5)	coffp0(a1,a2,a3,a4,a5)
#else
#define MAKEPATCH(a1,a2,a3,a4,a5)	loutpatch(a1,a2,a3,a4,a5)
#endif

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void p_rec();
void g_rec();
void m_rec();
void fatal();
void asydump();
int  asy_ver();
int get_speed();
void usage();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
int	line;			/* line number in input file		*/
int	num_chan;		/* highest channel number used		*/
int	group;			/* next port group to be assigned	*/
unsigned int	x1,x2,x3,x4,x5,x6,x7;
asy0_t	asy0[MAX_ASY];
asy_gp_t asy_gp[MAX_ASY];
char	chan_found[MAX_ASY];	/* true if record seen for the channel	*/
char	buf[BSZ];		/* input line buffer			*/
char	msg[BSZ];		/* error message line buffer			*/
char	expect_chan;		/* number of "M" records expected	*/
char	outs, irq, nms;
char	vflag;			/* verbose mode				*/
char	xflag;			/* debug mode				*/
char	*cmd;			/* command name				*/

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * main()
 *
 * while able to read a line from input
 *	call routine to process the record type
 */
main(argc, argv)
int argc;
char **argv;
{
	int	fd, len, ch, hi_chan, p_ver;
	char	*pfile = 0, *cp;

	cmd = argv[0];

	/*
	 * Scan command arguments.
	 */
	while (cp = *++argv) {
		if (*cp == '-') {
			while(*++cp) {
				switch (*cp) {
				case 'v':
					vflag = 1;
					break;
				case 'x':
					xflag = 1;
					break;
				default:
					usage();
				}
			}
		} else {
			if (pfile)
				usage();
			pfile = cp;
		}
	}

	if (!pfile)
		usage();

	if (vflag)
		printf("%s Version %d\n", cmd, ASY_VERSION);

	while (fgets(buf, BSZ, stdin)) {
		line++;
		switch (buf[0]) {
			case 'P':  p_rec();  break;
			case 'G':  g_rec();  break;
			case 'M':  m_rec();  break;
		}
	}

	if (expect_chan) {
		sprintf(msg, "expecting %d \"M\" entries", expect_chan);
		fatal(msg);
	}

	/*
	 * Find highest channel number in use.
	 */
	for (hi_chan = -1, ch = 0; ch < MAX_ASY; ch++)
		if (chan_found[ch])
			hi_chan = ch;

	num_chan = hi_chan + 1;

	if (num_chan == 0)
		fatal("No channels specified");

	/*
	 * Make sure the file to be patched is accessible.
	 */
	if ((fd = open(pfile, 2)) == -1) {
		sprintf(msg, "can't open file %s\n", pfile);
		fatal(msg);
	}
	close(fd);

	/*
	 * Version numbers in patch program and driver must match.
	 */
	p_ver = asy_ver(pfile);
	if (p_ver != ASY_VERSION) {
		sprintf(msg, "Wrong versions: asypatch=%d  %s=%d\n",
		  ASY_VERSION, pfile, p_ver);
		fatal(msg);
	}

	if (vflag)
		asydump();

	/*
	 * Make three or four patches.
	 */
	if (MAKEPATCH(pfile, "ASY_NUM", &num_chan, sizeof(int), 0) == 0)
		fatal("can't patch ASY_NUM");

	if (MAKEPATCH(pfile, "ASYGP_NUM", &group, sizeof(int), 0) == 0)
		fatal("can't patch ASYGP_NUM");

	len = num_chan * sizeof(asy0[0]);
	if (MAKEPATCH(pfile, "asy0", asy0, len, 0) == 0)
		fatal("can't patch asy0");

	if (group) {
		len = group * sizeof(asy_gp[0]);
		if (MAKEPATCH(pfile, "asy_gp", asy_gp, len, 0) == 0)
			fatal("can't patch asy_gp");
	}

	exit(0);
}

void
p_rec()
{
	asy0_t	*a0;
	int	speed;

	if (expect_chan) {
		sprintf(msg, "expecting %d \"M\" entries", expect_chan);
		fatal(msg);
	}

	/*
	 * Load a struct from input.
	 *
	 * "P" record is:
	 * P port irq outs excl speed channel nms
	 * port and outs are in hex, others in decimal
	 */
	if (sscanf(buf+1, "%x%d%x%d%d%d%d",&x1,&x2,&x3,&x4,&x5,&x6,&x7) != 7)
		fatal("need 7 parameters");

	if (x6 >= MAX_ASY) {
		sprintf(msg, "invalid channel #%d\n", x6);
		fatal(msg);
	}

	if (chan_found[x6]) {
		sprintf(msg, "duplicate channel #%d\n", x6);
		fatal(msg);
	}

	a0 = asy0 + x6;
#ifdef _I386
	if (x1 >= 0x10000)
		fatal("address must be ffff or less");
#endif
	a0->a_port = x1;
	if (x2 >= 16)
		fatal("irq must be 15 or less");
	a0->a_irqno = x2;
#ifdef _I386
	if (x3 & 0xfffffff3)
#else
	if (x3 & 0xfff3)
#endif
		fatal("only bits 2 & 3 of outs may be nonzero");
	a0->a_outs = x3;
	if (x4 > 1)
		fatal("excl must be 0 or 1");
	a0->a_ixc = x4;
	speed = get_speed(x5);
	if (speed < 0)
		fatal("invalid speed");
	else
		a0->a_speed = speed;
	a0->a_asy_gp = NO_ASYGP;
	chan_found[x6] = 1;
	if (x7 > 1)
		fatal("nms must be 0 or 1");
	a0->a_nms = x7;
}

void
g_rec()
{
	int slot;
	asy_gp_t *gp = asy_gp + group;

	if (expect_chan) {
		fprintf(stderr, "expecting %d \"M\" entries", expect_chan);
		fatal("record out of order");
	}

	if (group >= MAX_ASYGP) {
		fprintf(stderr, "Just found group #%d\n", group);
		fatal("too many groups");
	}

	/*
	 * Load a struct from input.
	 *
	 * "G" record is:
	 * G port irq outs type number-of-channels nms
	 * port and outs are in hex, others in decimal
	 * see asy.h for valid types
	 */
	if (sscanf(buf+1, "%x%d%x%d%d%d",&x1,&x2,&x3,&x4,&x5,&x6) != 6)
		fatal("need 6 parameters");
#ifdef _I386
	if (x1 >= 0x10000)
		fatal("address must be ffff or less");
#endif
	gp->stat_port = x1;
	if (x2 >= 16)
		fatal("irq must be 15 or less");
	irq = gp->irq = x2;
#ifdef _I386
	if (x3 & 0xfffffff3)
#else
	if (x3 & 0xfff3)
#endif
		fatal("only bits 2 & 3 of outs may be nonzero");
	outs = x3;
	if (x4 >= PT_MAX)
		fatal("invalid type");
	gp->gp_type = x4;
	if (x5 > MAX_SLOTS || x5 == 0)
		fatal("number of channels must be in range 1..16");
	expect_chan = x5;
	if (x6 > 1)
		fatal("nms must be 0 or 1");
	nms = x6;
	for (slot = 0; slot < MAX_SLOTS; slot++)
		gp->chan_list[slot] = NO_CHANNEL;
	group++;
}

void
usage()
{
	fprintf(stderr, "%s Version %d\n", cmd, ASY_VERSION);
	fprintf(stderr, "Usage: %s [-vx] patchfile < specfile\n", cmd);
	exit(1);
}

void
fatal(s)
char *s;
{
	fprintf(stderr, "%s: line %d  %s\n", cmd, line, s);
	exit(1);
}

void
m_rec()
{
	asy0_t *a0;
	int speed;

	if (expect_chan == 0) {
		fatal("unexpected \"M\" record");
	}

	/*
	 * Load a struct from input.
	 *
	 * "M" record is:
	 * M port speed slot channel
	 * port is in hex, speed in decimal
	 */
	if (sscanf(buf+1, "%x%d%d%d",&x1,&x2,&x3,&x4) != 4)
		fatal("need 4 parameters");

	if (x4 >= MAX_ASY) {
		sprintf(msg, "invalid channel #%d\n", x4);
		fatal(msg);
	}

	if (chan_found[x4]) {
		sprintf(msg, "duplicate channel #%d\n", x4);
		fatal(msg);
	}

	if (x3 >= MAX_SLOTS) {
		sprintf(msg, "slot number %d must be less than 16\n", x3);
		fatal(msg);
	}
	a0 = asy0 + x4;

#ifdef _I386
	if (x1 >= 0x10000)
		fatal("address must be ffff or less");
#endif
	a0->a_port = x1;
	speed = get_speed(x2);
	if (speed < 0)
		fatal("invalid speed");
	else
		a0->a_speed = speed;

	a0->a_outs = outs;
	a0->a_nms = nms;
	a0->a_ixc = 0;
	a0->a_irqno = irq;
	a0->a_asy_gp = group - 1;
	asy_gp[group - 1].chan_list[x3] = x4;
	chan_found[x4] = 1;
	expect_chan--;
}

int
get_speed(rate)
int rate;
{
	int ret;

	switch (rate) {
		case 0:		ret = B0;  	break;
		case 50:	ret = B50;	break;
		case 75:	ret = B75;	break;
		case 110:	ret = B110;	break;
		case 134:	ret = B134;	break;
		case 150:	ret = B150;	break;
		case 200:	ret = B200;	break;
		case 300:	ret = B300;	break;
		case 600:	ret = B600;	break;
		case 1200:	ret = B1200;	break;
		case 1800:	ret = B1800;	break;
		case 2400:	ret = B2400;	break;
		case 4800:	ret = B4800;	break;
		case 9600:	ret = B9600;	break;
		case 19200:	ret = B19200;	break;
#ifdef _I386
		case 38400:	ret = B38400;	break;
#endif
		default:	ret = -1;
	}
	return ret;
}

#ifdef _I386
/*
 * coffp0()
 *
 * Given a file name, a symbol name, a buffer, and number of bytes in
 * the buffer, read or write the buffer into the COFF file at the
 * symbol specified.
 *
 * Read the given file if argument "do_read" is 1, else write to the file.
 * Return nonzero if success; zero if failure.
 */
int
coffp0(fname, sym, data, len, do_read)
char *fname, *sym, *data;
int len, do_read;
{
	int	ret;
	SYMENT	se;

	/*
	 * put together SYMENT stuff for coffnlist
	 */
	se._n._n_n._n_zeroes = 0;
	se._n._n_n._n_offset = sizeof(long);
	se.n_type = -1;
	ret = coffnlist(fname, &se, sym, 1);

	if (ret)
		ret = coffpatch(fname, &se, sym, data, len, do_read);

	return ret;
}
#endif

void
asydump()
{
	char chan, g;
	asy0_t *a0;
	asy_gp_t *gp;

	/*
	 * Display totals.
	 */
	printf("Channels = %2d  Groups = %d\n", num_chan, group);

	/*
	 * Display channel table asy0.
	 */
	printf("Ch port Ir S O Gp X N\n");
	for (chan = 0; chan < num_chan; chan++) {
		a0 = asy0 + chan;
		printf("%2d %4x %2d %1x %1x %2d %1d %1d\n",
		  chan, a0->a_port, a0->a_irqno, a0->a_speed, a0->a_outs,
		  a0->a_asy_gp, a0->a_ixc, a0->a_nms);
	}

	/*
	 * Display group table asy_gp.
	 */
	if (group) {
		printf("Gp Port T Irq  Channels...............\n");
		for (g = 0; g < group; g++) {
			int s;

			gp = asy_gp + g;
			printf("%2d %4x %1d %2d ",
			  g,gp->stat_port,gp->gp_type,gp->irq);
			for (s = 0; s < MAX_SLOTS; s++)
				if (gp->chan_list[s] == NO_CHANNEL)
					printf("   ", gp->chan_list[s]);
				else
					printf(" %2d", gp->chan_list[s]);
			putchar('\n');
		}
	}
}

/*
 * asy_ver()
 *
 * Fetch the initial value at symbol ASY_VER (ASY_VER_ for 286)
 * in file "fname".
 *
 * Return -1 on failure;  otherwise return the value at the symbol.
 */
int
asy_ver(fname)
char * fname;
{
	int ret = -1;
	int version;

	if (MAKEPATCH(fname, "ASY_VER", &version, sizeof(int), 1))
		ret = version;
	return ret;
}
