/*
 * dosdump:	display extended DOS partition info
 */
#include <stdio.h>
#include <sys/fdisk.h>
extern	long lseek();
int	fd;
char	buf[512];
long	partseek;
HDISK_S	*hdp;
FDISK_S	*fdp1, *fdp2;

main(argc, argv)
char	*argv[];
{
	register i = 0;

	if (argc != 2)
		fatal("you fucked up");
	if ((fd = open(argv[1], 0)) < 0)
		fatal("open failed");
	for (;;) {
		if (lseek(fd, partseek, 0) == -1L)
			fatal("lseek failed");
		if (read(fd, buf, sizeof(buf)) != sizeof(buf))
			fatal("read failed");
		hdp = buf;
		if (hdp->hd_sig != HDSIG)
			fatal("no signature found");
		fdp1 = &(hdp->hd_partn[0]);
		fdp2 = &(hdp->hd_partn[1]);
		printf("Level %d:\n", ++i);
		printf("\tfdp1: sys=%d base=%8lx size=%8lx\n",
			fdp1->p_sys, fdp1->p_base, fdp1->p_size);
		printf("\tfdp2: sys=%d base=%8lx size=%8lx\n",
			fdp2->p_sys, fdp2->p_base, fdp2->p_size);
		if (fdp2->p_sys != SYS_DOS_XP)
			break;
		partseek = fdp2->p_base * 512;
	}
	exit(0);
}

fatal(cp)
char *cp;
{
	fprintf(stderr, "dosdump: %r\n", &cp);
	exit(1);
}

