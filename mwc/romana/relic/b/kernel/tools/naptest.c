/* naptest.c */

#include <signal.h>

int bleep;

void
intHandler()
{
	bleep = 1;
}

main(argc, argv)
int argc;
char * argv[];
{
	int naptime, result;

	if (argc <= 1)
		printf("Usage: %s <nap-interval-in-milliseconds>\n", argv[0]);
	else {
		sigset(SIGINT, intHandler);
		naptime = atoi(argv[1]);
		printf("Napping for %d msec.\n", naptime);
		result = nap(naptime);
		if (bleep)
			printf("SIGINT received\n");
		if (result == -1)
			perror("Nap test error");
		else
			printf("Nap over.  Slept for %d msec.\n", result);
	}
}
