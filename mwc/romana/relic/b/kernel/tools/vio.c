/*
 * vio.c
 *
 * Usage:  vio a1 a2 a3
 *
 */
#include <stdio.h>
#include <sys/param.h>

char * cmd;

main(argc, argv)
int argc;
char ** argv;
{
	int a1, a2, a3, res;
	extern int errno;

	cmd = argv[0];
	if (argc != 4)
		usage();

	a1 = htoi(argv[1]);
	a2 = htoi(argv[2]);
	a3 = htoi(argv[3]);

	res = cohcall(COH_VIO, a1, a2, a3);
	if (errno)
		perror("vio");
	else
		printf("%s(%x, %x, %x) = %x\n", cmd, a1, a2, a3, res);
}

usage()
{
	fprintf(stderr, "Usage: %s a1 a2 a3\n", cmd);
	exit(1);
}

int
htoi(s)
char *s;
{
	int ret;

	sscanf(s, "%x", &ret);
	return ret;
}
