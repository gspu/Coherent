/* is_fs.c */

#define _KERNEL 1

#include <sys/buf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <canon.h>
#include <sys/filsys.h>

/*
 * Check if a special file is a well-formed filesystem.
 * This routine is derived from code in "mount.c".
 * Return values:
 *	-1	Not a device, cannot open, read or seek failed
 *	0	No filesystem
 *	1	Legal filesystem
 */
int
is_fs(special) char *special;
{
	static struct filsys f;
	struct stat sbuf;
	register int fd;
	register struct filsys *fp;
	register daddr_t *dp;
	register ino_t *ip, maxinode;

	if (stat(special, &sbuf) < 0)
		return -1;			/* cannot stat device */
	else if ((sbuf.st_mode&S_IFMT) != S_IFBLK
	      && (sbuf.st_mode&S_IFMT) != S_IFCHR)
		return -1;			/* not a special file */
	else if ((fd = open(special, 2)) < 0)
		return -1;			/* cannot open */
	else if (lseek(fd, (long)SUPERI*BSIZE, 0) == -1L
	      || read(fd, &f, sizeof(f)) != sizeof(f)) {
		close(fd);
		return -1;			/* seek or read failed */
	} else
		close(fd);			/* so far so good */

	/* Canonical stuff. */
	fp = &f;
	canshort(fp->s_isize);
	candaddr(fp->s_fsize);
	canshort(fp->s_nfree);
	for (dp = &fp->s_free[0]; dp < &fp->s_free[NICFREE]; dp += 1)
		candaddr(*dp);
	canshort(fp->s_ninode);
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[NICINOD]; ip += 1)
		canino(*ip);
	candaddr(fp->s_tfree);
	canino(fp->s_tinode);

	/* Test for rationality. */
	maxinode = (fp->s_isize - INODEI) * INOPB + 1;
	if (fp->s_isize >= fp->s_fsize)
		return 0;
	if ((fp->s_tfree < fp->s_nfree)
	||  (fp->s_tfree >= fp->s_fsize - fp->s_isize + 1))
		return 0;
	if ((fp->s_tinode < fp->s_ninode) || (fp->s_tinode >= maxinode-1 ))
		return 0;
	for (dp = &fp->s_free[0]; dp < &fp->s_free[fp->s_nfree]; dp += 1)
		if ((*dp < fp->s_isize) || (*dp >= fp->s_fsize))
			return 0;
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[fp->s_ninode]; ip += 1)
		if ((*ip < 1) || (*ip > maxinode))
			return 0;
	return 1;
}
