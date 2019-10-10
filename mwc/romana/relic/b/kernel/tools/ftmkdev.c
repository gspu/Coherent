/*
 * File:	ftmkdev.c
 *
 * Purpose:	create nodes for floppy tape
 *
 * $Log:	ftmkdev.c,v $
 * Revision 1.1  93/06/14  13:29:32  bin
 * Initial revision
 * 
 */
/*
 * ----------------------------------------------------------------------
 * Includes.
 */
#include <sys/xlft.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
#define LO_MN		0x40		/* low minor number to generate  */
#define NUM_DEVS	64		/* number of devices to generate */

#define DNAME_LEN	20

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
static void genName();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */

/*
 * ----------------------------------------------------------------------
 * Code.
 */

main(argc, argv)
int argc;
char * argv[];
{
	int	m;	/* minor number */
	char	dname[DNAME_LEN];

	printf("/bin/mkdir /dev/ft\n");
	printf("/bin/chmog 777 sys sys /dev/ft\n");

	for (m = LO_MN; m < LO_MN + NUM_DEVS; m++) {

		/* Generate device name. */
		genName(m, dname);

		printf("/etc/mknod -f /dev/ft/%s c 4 %d\n", dname, m);
	}
	printf("/bin/chmog 666 sys sys /dev/ft/*\n");
}

static void
genName(m, str)
int m;
char * str;
{
	int	unit, format, control, noRewind, retension;

	unit = m & M_UNT;
	format = m & M_FMT;
	control = m & M_CTL;
	noRewind = m & M_REW;
	retension = m & M_RET;

	sprintf(str, "f%dq%d0%s%s%s",
	  unit,
	  (format) ? 8 : 4,
	  (control) ? "c" : "",
	  (noRewind) ? "n" : "",
	  (retension) ? "r" : "");
}
