/*
 * File:	sys4.c
 *
 * Purpose:
 * Non-Sytem V (compatibility) system calls introduced by the 386 port.
 *
 * $Log:	sys4.c,v $
 * Revision 2.2  93/07/26  14:29:12  nigel
 * Nigel's R80
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
