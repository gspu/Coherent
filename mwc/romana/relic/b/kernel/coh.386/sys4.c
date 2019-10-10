/*
 * File:	sys4.c
 *
 * Purpose:
 * Non-Sytem V (compatibility) system calls introduced by the 386 port.
 *
 * $Log:	sys4.c,v $
 * Revision 1.6  93/04/16  06:50:09  bin
 * Hal: kernel 76 update
 * 
 * Revision 1.2  92/01/06  12:00:58  hal
 * Compile with cc.mwc.
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
int ustty(); 
int ugtty(); 

/*
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
/*
 *
 */ 

int ustty(fd, sp)
int fd;
struct sgttyb * sp;
{
	return uioctl(fd, TIOCSETP, sp);
}

int ugtty(fd, sp)
int fd;
struct sgttyb * sp;
{
	return uioctl(fd, TIOCGETP, sp);
}
