/*
 * Clear inodes.
 */
#include <sys/filsys.h>
#include <sys/ino.h>
#include <canon.h>
#include <stdio.h>


union {
	struct filsys	filsys_;
	struct dinode	dinode_[INOPB];
} buff;
#define	fs	buff.filsys_
#define	di	buff.dinode_

unsigned	isize;


main(argc, argv)
register char **argv;
{
	register fd;

	if (argc < 3)
		fatal("usage: clri dev ino ...");
	fd = open(*++argv, 2);
	if (fd < 0)
		fatal("can't open %s", *argv);
	lseek(fd, (long)SUPERI*BSIZE, 0);
	if (read(fd, (char *)&fs, sizeof fs) != sizeof fs)
		fatal("superblock read error");
	isize = fs.s_isize;
	canint(isize);

	while (*++argv)
		clri(fd, atoi(*argv));

	return (0);
}


clri(fd, ino)
register fd;
ino_t 	ino;
{
	register bn;

	bn = iblockn(ino);
	if (bn >= isize)
		fatal("inode %u out of range", ino);
	lseek(fd, (long)bn*BSIZE, 0);
	if (read(fd, (char *)di, sizeof di) != sizeof di)
		fatal("read error on inode %u", ino);
	clear(&di[iblocko(ino)]);
	lseek(fd, (long)bn*BSIZE, 0);
	if (write(fd, (char *)di, sizeof di) != sizeof di)
		fatal("write error on inode %u", ino);
}


clear(dp)
struct dinode *dp;
{
	register char *p;
	register i;

	p = dp;
	i = sizeof *dp;
	do {
		*p++ = 0;
	} while (--i);
}


fatal(arg0)
char *arg0;
{

	fprintf(stderr, "clri: %r\n", &arg0);
	exit(1);
}
