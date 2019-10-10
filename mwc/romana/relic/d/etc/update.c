/*
 * /etc/update - sync every 30 seconds
 */

#include <signal.h>

main()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	for (;;) {
		sleep(30);
		sync();
	}
}
