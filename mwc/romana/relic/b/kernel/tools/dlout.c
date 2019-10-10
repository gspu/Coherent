/*
 * File:	dlout.c
 *
 * Purpose:	Display contents of l.out file intelligibly.
 *
 * $Log:	dlout.c,v $
 * Revision 1.1  93/04/16  07:01:40  bin
 * Initial revision
 * 
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <canon.h>
#include <l.out.h>
#include <stdio.h>
#include <sys/reg.h>
#include <sys/uproc.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define DSIZE			16

struct seg_info {
	int si_base;
	int si_size;
	long si_offset;
};

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static char * cmd;
static struct ldheader ldHead;
static char * loutName = NULL;
static FILE * fp;
static char *segName[NUSEG] = {
	"USER", "TEXT", "DATA", "STACK"
};
static struct seg_info segInfo[NUSEG];

/*
 * ----------------------------------------------------------------------
 * Code.
 */

void
usage()
{
	fprintf(stderr, "Usage: %s l.out-file\n", cmd);
	exit(1);
}

/*
 * Given text name and vital stats for a segment, do a hex dump.
 */
void
dumpseg(name, infop)
char *name;
struct seg_info *infop;
{
	unsigned char coredata[DSIZE];
	unsigned char savedata[DSIZE];
	int segOffset;
	int i;
	/*
	 * sameState:
	 * -1	just starting; don't look for matching display lines
	 *  0	current line does not match previous line
	 *  1	current line matches previous line
	 *  2	current line matches previous 2 or more lines
	 */
	int sameState = -1;
	int doPrint, match;

	/* make sure we can seek to start of segment */
	if (fseek(fp, infop->si_offset, 0) < 0) {
		fprintf(stderr, "Can't seek to %s segment!\n", name);
		exit(1);
	}

	printf("\nContents of %s segment:\n", name);
	for (segOffset = 0; segOffset < infop->si_size; segOffset += DSIZE) {
		int readLen = DSIZE;
		int remains = infop->si_size - segOffset;

		if (readLen > remains)
			readLen = remains;

		if (fread(coredata, readLen, 1, fp) !=1) {
			fprintf(stderr, "Can't read core data.\n");
			exit(1);
		}

		/* always display last line of segment */
		if (remains <= DSIZE)
			match = 1;
		else
			match = memcmp(coredata, savedata, DSIZE);

		switch(sameState) {
		case -1:
			sameState = 0;
			doPrint = 1;
			break;
		case 0:
			if (match) {
				doPrint = 1;
			} else {
				sameState = 1;
				doPrint = 0;
			}
			break;
		case 1:
			if (match) {
				doPrint = 1;
				sameState = 0;
			} else {
				sameState = 2;
				doPrint = 0;
				printf("   *\n");
			}
			break;
		case 2:
			if (match) {
				doPrint = 1;
				sameState = 0;
			} else {
				doPrint = 0;
			}
			break;
		}
		if (doPrint) {
			printf("%06X", segOffset);
			for (i = 0; i < readLen; i++)
				printf(" %02X", coredata[i]);
			printf("\n");
		}
		memcpy(savedata, coredata, DSIZE);
	}
}

/*
 * Once the file has been opened and magic number checked, this function
 * does the work.
 *
 * Globals used: fp, chInfo.
 */
void
dlout()
{
	int i;
	unsigned int shrds;	/* size of shared data */

	/* canonicalize where necessary */
	canint(ldHead.l_machine);
	if (ldHead.l_machine!=M_8086) {
		fprintf(stderr, "Wrong machine.\n");
		exit(1);
	}

	for (i=0; i<NXSEG; i++) {
		cansize(ldHead.l_ssize[i]);
	}	
	canint(ldHead.l_flag);
	canvaddr(ldHead.l_entry);

	/* display shared/unshared */
	if (ldHead.l_flag & LF_SHR)
		printf("shared executable\n");
	else
		printf("nonshared executable\n");


	/* display entry point */
	printf("entry point = %04X\n", ldHead.l_entry);

	/* fetch segment data */
	segInfo[SISTEXT].si_offset = sizeof(struct ldheader);
	segInfo[SISTEXT].si_base = NBPS;
	segInfo[SISTEXT].si_size = ldHead.l_ssize[L_SHRI];

	segInfo[SIPDATA].si_offset = sizeof(struct ldheader) +
		segInfo[SISTEXT].si_size;
	segInfo[SIPDATA].si_base = 0;
	segInfo[SIPDATA].si_size = ldHead.l_ssize[L_SHRD] +
		ldHead.l_ssize[L_PRVD];
	if (ldHead.l_flag & LF_SHR) {
		shrds = ldHead.l_ssize[L_SHRD];
		printf("shrds =  %04X\n", shrds);
	}

	segInfo[SIBSS].si_offset = 0;
	segInfo[SIBSS].si_base = segInfo[SIPDATA].si_size;
	segInfo[SIBSS].si_size = ldHead.l_ssize[L_BSSD];

	/* display segment sizes */
	printf("\nSegment\tBase\t\tSize\n");
	for (i = 0; i < NUSEG; i++) {
		if (i != SISTEXT && i != SIPDATA)
			continue;
		printf("%s\t%08x\t%08x\n",
		  segName[i], segInfo[i].si_base, segInfo[i].si_size);
	}

	/* dump segments */
	for (i = 0; i < NUSEG; i++) {
		if (i != SISTEXT && i != SIPDATA)
			continue;
		dumpseg(segName[i], segInfo + i);
	}
#if 0
	long offset;
	static struct uproc uProc;
	static int regs[SS+1];
	int i;
	int base, size;

	/* display uproc version number */
	offset = chInfo.ch_info_len + chInfo.ch_uproc_offset;
	if (fseek(fp, offset, 0) < 0) {
		fprintf(stderr, "Can't seek to uproc.\n");
		exit(1);
	}
	if (fread(&uProc, sizeof(uProc), 1, fp) !=1) {
		fprintf(stderr, "Can't read uproc.\n");
		exit(1);
	}
	printf("uproc version = 0x%04x\n", uProc.u_version);

#endif
}

main(argc, argv)
int argc;
char *argv[];
{

	/* command line housekeeping */
	cmd = argv[0];
	if (argc != 2)
		usage();
	loutName = argv[1];

	/* try to open the l.out file for reading */
	if ((fp = fopen(loutName, "r")) == NULL) {
		fprintf(stderr, "Can't open %s for reading.\n", loutName);
		exit(1);
	}

	/* simple test for l.out file format */
	if (fread(&ldHead, sizeof(ldHead), 1, fp) !=1) {
		fprintf(stderr, "Can't read l.out header\n");
		exit(1);
	}

	if (ldHead.l_magic != L_MAGIC) {
		fprintf(stderr, "Not a l.out file.\n");
		exit(1);
	}
	printf("L.out file: %s\n", loutName);

	/* call a function to do the work */
	dlout();

	/* cleanup */
	fclose(fp);
	exit(0);
}

long
_canl(n)
int n;
{
	return (n<<16)|((n>>16)&0xffff);
}
