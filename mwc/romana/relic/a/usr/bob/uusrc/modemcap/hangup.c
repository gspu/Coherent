/*
 *  hangup.c
 */

/* hangup is called by sysend() to hangup the modem. Hangup then calls
 * undial() to remove any remote device lock files and to re enable
 * a previously disabled port. Bob H. 11/22/91.
 */

#include "dcp.h"
#include "modemcap.h"

hangup (fd)
int	fd;
{
	if ( (HU == NULL) && (HC == 0) ) {
		printmsg(M_DEBUG,"hangup: (level1) about to drop line.");
		undial(fd);
		return(0);
	}

	if (AT != (char *) 0) {

	/* guard time for modem. Before sending the +++ to the modem to put
	 * it in command mode, there must be some delay between the real data
	 * and the AT string for it to work.
	 */
		sleep (3);
		write (fd, AT, strlen (AT));
		if (AD)
			sleep (AD);
	}
	if (HU) {
		if (CS) {
			write (fd, CS, strlen (CS));
		}
		write (fd, HU, strlen (HU));
		if (CE) {
			write (fd, CE, strlen (CE));
		}
		if (IS) {
			write (fd, IS, strlen (IS));
			if (ID)
				sleep (ID);
		}
		printmsg(M_DEBUG,"hangup: (level2) about to drop line.");
		undial (fd);
		return (1);
	}
	printmsg(M_DEBUG,"hangup: (level3) about to drop line.");
	undial(fd);
	ttyexit(fd);
	return(1);
}
