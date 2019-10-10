/*
 * Reboot the processor by transferring to the reset vector of the 8088.
 *
 * $Log:	/usr/src.inetco/etc/reboot.c,v $
 * Revision 1.3	90/04/17  14:56:54 	root
 * steve 4/17/90
 * Added sync() after prompt with -p option.
 * 
 * Revision 1.2	90/04/17  14:48:29 	root
 * steve 4/17/90
 * Added "-p" option to prompt user to hit <Enter> before rebooting,
 * for use during installation.
 * 
 * Revision 1.1	90/04/17  14:34:11 	root
 * Initial revision
 * 
 * 86/12/19	Allan Cornish		/usr/src/cmd/etc/reboot.c
 * reboot.s converted into reboot.c and rebootas.s to provide time for
 * disk drives to turn off before initiating reboot.
 */

#include <stdio.h>
#include <signal.h>

sigquiet( sig )
int sig;
{
	signal( sig, sigquiet );
}

main (argc, argv) int argc; char *argv[];
{
	char buf[80];

	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'p') {
		/* Prompt before reboot. */
		printf("Hit <Enter> and your system will reboot automatically... ");
		fflush(stdout);
		sync();
		fgets(buf, sizeof buf, stdin);
	}
		
	/*
	 * Trap alarm signals.
	 */
	signal( SIGALRM, sigquiet );

	/*
	 * Wait at least 4 seconds for drives to turn off, etc.
	 */
	alarm( 5 );
	pause();

	/*
	 * Reboot the processor.
	 */
	reboot();

	/*
	 * Should never reach here.
	 */
	exit(1);
}
