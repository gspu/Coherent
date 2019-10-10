/*
 * File:	tty_mode.c
 *
 * Purpose:	transfer modes between tty devices
 *
 * $Log:	tty_mode.c,v $
 * Revision 1.2  92/06/15  10:04:58  bin
 * *** empty log message ***
 * 
 */

/*
 * Includes.
 */
#include <sgtty.h>

/*
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */
typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

/*
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
int tty_getmode();
int tty_setmode();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static struct sgttyb	tty_sgttyb;
static struct tchars	tty_tchars;

/*
 * tty_getmode()
 *
 * Save modes for the given file descriptor.
 */
int
tty_getmode(fd)
int fd;
{
	if (ioctl(fd, TIOCGETP, &tty_sgttyb) < 0)
		return -1;
	if (ioctl(fd, TIOCGETC, &tty_tchars) < 0)
		return -1;
	return 0;
}

/*
 * tty_setmode()
 *
 * Restore modes to the given file descriptor.
 */
int
tty_setmode(fd)
int fd;
{
	if (ioctl(fd, TIOCSETP, &tty_sgttyb) < 0)
		return -1;
	if (ioctl(fd, TIOCSETC, &tty_tchars) < 0)
		return -1;
	return 0;
}
