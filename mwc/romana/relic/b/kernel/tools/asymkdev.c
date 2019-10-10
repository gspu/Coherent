/*
 * File:	asymkdev.c
 *
 * Purpose:	read a spec from stdin, ask questions, and make nodes
 *
 * $Log:	asymkdev.c,v $
 * Revision 1.3  93/05/18  07:32:18  bin
 * *** empty log message ***
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
#include <sys/devices.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define BSZ		256
#define ASYNC_SPECFILE	"/etc/default/async"
#define ASYNC_MKNODS	"asy_mknod"

#define L_		0
#define R_		1
#define PL_		2
#define PR_		3
#define FL_		4
#define FR_		5
#define FPL_		6
#define FPR_		7

#define NO(a,b,c,d)	{use_opt[a]=0;use_opt[b]=0;use_opt[c]=0;use_opt[d]=0;}
#define MAX_OPT		8
#define prompt		if(!uflag)printf

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void fatal();
void g_rec();
void get_opts();
void m_rec();
void make_nodes();
void p_rec();
void usage();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
int	line;			/* line number in input file		*/
unsigned int	x1,x2,x3,x4,x5,x6,x7;
char	*cmd;
char	chan_found[MAX_ASY];	/* true if record seen for the channel	*/
int	port[MAX_ASY];		/* port address for the channel		*/
char	buf[BSZ];		/* input line buffer			*/
char	msg[BSZ];		/* error message line buffer			*/
char	expect_chan;		/* number of "M" records expected	*/
char	uflag;			/* unprompted mode				*/
char	*suffix[MAX_OPT] = {
	"l",			/* L_	*/
	"r",			/* R_	*/
	"pl",			/* PL_	*/
	"pr",			/* PR_	*/
	"fl",			/* FL_	*/
	"fr",			/* FR_	*/
	"fpl",			/* FPL_	*/
	"fpr"			/* FPR_	*/
};
char	*ofile, *sfile;
unsigned char	nmod, poll, flow;

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
	char	*cp;
	FILE	*fp;

	cmd = argv[0];

	if (!isatty(1))
		usage();

	/*
	 * Scan command arguments.
	 */
	while (cp = *++argv) {
		if (*cp == '-') {
			while(*++cp) {
				switch (*cp) {
				case 'u':
					uflag = 1;
					break;
				default:
					usage();
				}
			}
		} else {
			if (ofile) {
				usage();
			} else {
				if (sfile)
					ofile = cp;
				else
					sfile = cp;
			}
		}
	}
	if (!ofile)
		ofile = ASYNC_MKNODS;
	if (!sfile)
		sfile = ASYNC_SPECFILE;

	/*
	 * Find out which channels are in use.
	 */
	if ((fp = fopen(sfile, "r")) == 0) {
		sprintf(msg, "%s: can't open spec file %s", cmd, sfile);
		fatal(msg);
	}
	while (fgets(buf, BSZ, fp)) {
		line++;
		switch (buf[0]) {
			case 'P':  p_rec();  break;
			case 'G':  g_rec();  break;
			case 'M':  m_rec();  break;
		}
	}
	fclose(fp);

	if (expect_chan) {
		sprintf(msg, "expecting %d \"M\" entries", expect_chan);
		fatal(msg);
	}

	make_nodes();

	prompt("Done.\n");
	exit(0);
}

void
p_rec()
{
	if (expect_chan) {
		sprintf(msg, "expecting %d \"M\" entries", expect_chan);
		fatal(msg);
	}

	/*
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
	chan_found[x6] = 1;
	port[x6] = x1;
}

void
g_rec()
{
	if (expect_chan) {
		fprintf(stderr, "expecting %d \"M\" entries", expect_chan);
		fatal("record out of order");
	}

	/*
	 * "G" record is:
	 * G port irq outs type number-of-channels nms
	 * port and outs are in hex, others in decimal
	 * see asy.h for valid types
	 */
	if (sscanf(buf+1, "%x%d%x%d%d%d",&x1,&x2,&x3,&x4,&x5,&x6) != 6)
		fatal("need 6 parameters");
	if (x5 >= MAX_ASY || x5 == 0)
		fatal("number of channels must be in range 1..8");
	expect_chan = x5;
}

void
usage()
{
	fprintf(stderr, "Usage: %s [-u] specfile outfile\n", cmd);
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
	if (expect_chan == 0) {
		fatal("unexpected \"M\" record");
	}

	/*
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

	chan_found[x4] = 1;
	port[x4] = x1;
	expect_chan--;
}

void
make_nodes()
{
	char	chan;
	char	devname[20] = "";
	int	last;
	unsigned char	minor;
	char	use_opt[MAX_OPT], opt;
	FILE	*ofp;

	if ((ofp = fopen(ofile, "w")) == 0) {
		sprintf(msg, "%s: can't open outfile %s", cmd, ofile);
		fatal(msg);
	}
	fprintf(ofp,
	  "set -x\n"
	  "DEV=${DEV-/dev}\n"
	  "if [ -d $DEV ]\n"
	  "then\n"
	  "\t:\n"
	  "else\n"
	  "\techo \"Invalid device directory $DEV\"\n"
	  "\texit 1\n"
	  "fi\n");

	line = 0;
	for (chan = 0; chan < MAX_ASY; chan++) {
		if (chan_found[chan]) {
			/*
			 * Make default device name.
			 * Add 1 to last character of previous name, with carry.
			 * Start with "asy00".
			 */
			if (devname[0]) {
				last = strlen(devname) - 1;
				if (last > 0 && devname[last]++ == '9') {
					devname[last--] = '0';
					devname[last]++;
				}
			} else {
				strcpy(devname, "asy00");
			}
			prompt("Channel %2d is port %4x\n", chan, port[chan]);
			prompt("Device name [%s]: ", devname);
			line++;
			if (!fgets(buf, BSZ, stdin)) {
				sprintf(msg, "%s: input error at channel %d",
				  cmd, chan);
				fatal(msg);
			}
			sscanf(buf, "%s", devname);

			/*
			 * Get user preferences about which devices to make.
			 */
			get_opts(chan);

			/*
			 * Make up to 8 nodes per device.
			 */
			for (opt = 0; opt < MAX_OPT; opt++)
				use_opt[opt] = 1;
			if (nmod == 0)
				NO(L_, PL_, FL_, FPL_)
			else if (nmod == 1)
				NO(R_, PR_, FR_, FPR_)
			if (poll == 0)
				NO(PL_, PR_, FPL_, FPR_)
			else if (poll == 1)
				NO(L_, R_, FL_, FR_)
			if (flow == 0)
				NO(FL_, FR_, FPL_, FPR_)
			else if (flow == 1)
				NO(L_, R_, PL_, PR_)
			for (opt = 0; opt < MAX_OPT; opt++)
				if (use_opt[opt]) {
					minor = chan;
					if ((opt & 1) == 0)
						minor |= 0x80;
					if (opt & 2)
						minor |= 0x40;
					if (opt & 4)
						minor |= 0x20;
fprintf(ofp, "/etc/mknod -f $DEV/%s%s c %d %d		|| exit 1\n",
  devname, suffix[opt], ASY_MAJOR, minor);
fprintf(ofp, "/bin/chmog 666 sys sys $DEV/%s%s	|| exit 1\n",
  devname, suffix[opt]);
				}
		}
	}
	fclose(ofp);
}

/*
 * get_opts()
 *
 * set nmod, poll, flow
 */
void
get_opts(chan)
int chan;
{
	static char my_opts[20] = "lrin";
	char	*cp;

	prompt("options - (l|r) (i|p) (f|n) [%s]: ", my_opts);
	line++;
	if (!fgets(buf, BSZ, stdin)) {
		sprintf(msg, "input error at channel %d", chan);
		fatal(msg);
	}
	sscanf(buf, "%s", my_opts);

	/*
	 * Must have at least one of each pair of switches.
	 */
	nmod = 0;
	poll = 0;
	flow = 0;
	for (cp = my_opts; *cp; cp++) {
		switch (*cp) {
			case 'r': nmod |= 1; break;
			case 'l': nmod |= 2; break;
			case 'i': poll |= 1; break;
			case 'p': poll |= 2; break;
			case 'n': flow |= 1; break;
			case 'f': flow |= 2; break;
			default:
				sprintf(msg,
				  "invalid option [%s],  channel %d",
				  my_opts, chan);
				fatal(msg);
		}
	}
	if (nmod)
		nmod--;
	else {
		sprintf(msg, "missing (l|r) option [%s],  channel %d",
		  my_opts, chan);
		fatal(msg);
	}
	if (poll)
		poll--;
	else {
		sprintf(msg, "missing (i|p) option [%s],  channel %d",
		  my_opts, chan);
		fatal(msg);
	}
	if (flow)
		flow--;
	else {
		sprintf(msg, "missing (f|n) option [%s],  channel %d",
		  my_opts, chan);
		fatal(msg);
	}
}
