/*
 * File:	tty_raw.c
 *
 * Purpose:	put a tty device into raw mode, and restore its old mode later
 *
 * $Log:	tty_raw.c,v $
 * Revision 1.2  92/06/15  10:04:59  bin
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
int tty_raw();
int tty_reset();

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
static struct sgttyb	save_mode;

/*
 * tty_raw()
 *
 * Given a file descriptor, put the device into raw mode.
 * Return 0 on success, -1 on failure.
 */
int
tty_raw(fd)
int fd;
{
	struct sgttyb temp_mode;

	if (ioctl(fd, TIOCGETP, &temp_mode) < 0)
		return -1;
	save_mode = temp_mode;
	temp_mode.sg_flags |= RAW;
	temp_mode.sg_flags &= ~ECHO;
	if (ioctl(fd, TIOCSETP, &temp_mode) < 0)
		return -1;
	return 0;
}

/*
 * tty_reset()
 *
 * Given a file descriptor, restore the old mode.
 * Return 0 on success, -1 on failure.
 */
int
tty_reset(fd)
int fd;
{
	if (ioctl(fd, TIOCSETP, &save_mode) < 0)
		return -1;
	return 0;
}
