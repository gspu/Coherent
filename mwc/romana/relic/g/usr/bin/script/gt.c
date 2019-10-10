/* gt.c - test */

#include <sys/param.h>

char master[32], slave[32];

main()
{
	int i, fd;

	for (i = 0; i < NOFILE; i++) {
		if ((fd = getpseudotty(master, slave)) != -1)
			printf("fd=%d  m=%s  s=%s\n", fd, master, slave);
		else {
			printf("No more pty's\n");
			break;
		}
	}
}
