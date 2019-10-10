/*
 * reboot.c - command to reboot the system
 *
 * "-p" option prompts and will sync before rebooting.
 */

#include <stdio.h>
#include <signal.h>
#include <sys/param.h>

sigquiet(sig)
int sig;
{
	signal(sig, sigquiet);
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
	signal(SIGALRM, sigquiet);

	/*
	 * Wait at least 4 seconds for drives to turn off, etc.
	 */
	alarm(5);
	pause();

	/*
	 * Reboot the processor.
	 */
	if (cohcall(COH_REBOOT) == -1)
		perror(argv[0]);

	/*
	 * Should never reach here.
	 */
	exit(1);
}
