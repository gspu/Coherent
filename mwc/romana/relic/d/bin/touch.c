/*
 * Touch -- update the last modified
 * time of a file, as for make.
 */

#include <stdio.h>

#define	DEFMODE	0666

int	cflag;

char	nocreate[] = "touch: cannot create %s\n";

main(argc, argv)
char *argv[];
{
	register int i;
	register int estat = 0;

	if (argc>1 && *argv[1]=='-') {
		if (argv[1][1]=='c' && argv[1][2]=='\0')
			cflag = 1;
		else
			usage();
		argv++;
		argc--;
	}
	if (argc < 2)
		usage();
	for (i=1; i<argc; i++)
		estat |= touch(argv[i]);
	exit(estat);
}

/*
 * Touch each file in the file-list
 * (The `cflag' indicates that
 * non-existent files should not be
 * created.)
 */
touch(f)
char *f;
{
	char c;
	register int fd;
	register int ret = 0;

	if ((fd = open(f, 2)) < 0) {
		if (!cflag && (fd = creat(f, DEFMODE))<0)
			fprintf(stderr, nocreate, f);
		if (fd >= 0)
			close(fd);
		else
			ret++;
		return (ret);
	}
	if (read(fd, &c, sizeof c) == sizeof c) {
		lseek(fd, 0L, 0);
		if (write(fd, &c, sizeof c) != sizeof c) {
			fprintf(stderr, "touch: write error on %s\n", f);
			ret++;
		}
		close(fd);
	} else {		/* Zero-byte file */
		close(fd);
		if (cflag)
			ret++;
		else if ((fd = creat(f, DEFMODE)) < 0)
			fprintf(stderr, nocreate, f);
		else
			close(fd);
	}
	return (ret);
}

usage()
{
	fprintf(stderr, "Usage: touch [-c] file ...\n");
	exit(1);
}
