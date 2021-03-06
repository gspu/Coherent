/*
 * Mount a filesystem
 */

char helpmessage[] = "\
\
mount -- mount file system\n\
Usage:	/etc/mount [ special directory [-fru] ]\n\
Options:\n\
	-r	Mount is read only\n\
	-u	No mount, just update '/etc/mnttab' and/or '/etc/mtab'\n\
The file system residing on the block special file 'special' becomes\n\
accessible through the pathname 'directory'.\n\
With no arguments, lists the currently mount file systems.\n\
If /etc/mtab appears out of date, due to reboot, the fact is noted.\n\
If /etc/mnttab is present, then the dates of mounting will be reported.\n\
\
";

#include <stdio.h>
#include <mtab.h>
#include <mnttab.h>
#include <sys/mount.h>
#include <errno.h>
#include <sys/filsys.h>
#include <canon.h>
#include <sys/stat.h>
#include <sys/timeb.h>

int	uflag;			/* Update mount table only */
int	rflag;			/* Read-only mount */
int	qmtab;			/* Mtab is out of date */
char	mtabf[] = "/etc/mtab";
char	mnttabf[] = "/etc/mnttab";	/* System 5 style */
struct	mtab	mtab;
struct	mnttab	mnttab;
struct	stat	sbuf;
char	mspec[MNAMSIZ];
extern	FILE *getmtab();
extern	FILE *getmnttab();
char	*ctime();

main(argc, argv)
char *argv[];
{
	if (argc == 1)
		mlist();
	else {
		if (argc==4 && *argv[3]=='-') {
			getflags(argv[3]+1);
			argc -= 1;
		}
		if (argc==3) {
			domount(argv[1], argv[2], rflag);
		} else
			usage();
	}
	return (0);
}

getflags(ap)
register char *ap;
{
	register int c;

	while ((c = *ap++) != '\0')
		switch (c) {
		case 'r':
			rflag = MFRON;
			continue;

		case 'u':
			uflag++;
			continue;

		default:
			usage();
		}
}

usage()
{
	fprintf(stderr, helpmessage);
	exit(1);
}

merror(f)
char *f;
{
	register int err;

	err = errno;
	fprintf(stderr, "mount: %r", &f);
	if (err == EBUSY)
		fprintf(stderr, ": mount device or directory busy\n");
	else if (err > 0 && err < sys_nerr)
		fprintf(stderr, ": %s\n", sys_errlist[err]);
	else
		fprintf(stderr, ": unrecognized error\n");
	exit(1);
}

domount(special, name, flag)
char *special;
char *name;
int flag;
{
	register FILE *fp;
	register int openerr;
	extern time_t time();

	chkname(name);
	if (!uflag) {
		chkspec(special);
		if (mount(special, name, flag) != 0)
			merror("%s on %s", special, name);
	}
	if ((fp = getmnttab()) == NULL) {
		openerr |= 1;
	} else {
		mcopy(special, mspec);
		while (fread(&mnttab, sizeof(mnttab), 1, fp) == 1) {
			if (mnttab.mt_dev[0] == '\0'
			||  strncmp(mnttab.mt_filsys, mspec, MNTNSIZ) == 0) {
				fseek(fp, (long)(-sizeof(mnttab)), 1);
				break;
			}
		}
		strncpy(mnttab.mt_dev, name, MNTNSIZ);
		strncpy(mnttab.mt_filsys, mspec, MNTNSIZ);
		mnttab.mt_ro_flg = flag;
		mnttab.mt_time = time(NULL);
		if (fwrite(&mnttab, sizeof(mnttab), 1, fp) != 1)
			merror("writing %s", mnttabf);
		fclose(fp);
	}
	if ((fp = getmtab()) == NULL) {
		openerr |= 2;
	} else {
		mcopy(special, mspec);
		while (fread(&mtab, sizeof(mtab), 1, fp) == 1) {
			if (mtab.mt_name[0] == '\0'
			||  strncmp(mtab.mt_special, mspec, MNAMSIZ) == 0) {
				fseek(fp, (long)(-sizeof(mtab)), 1);
				break;
			}
		}
		strncpy(mtab.mt_name, name, MNAMSIZ);
		strncpy(mtab.mt_special, mspec, MNAMSIZ);
		mtab.mt_flags = flag;
		if (fwrite(&mtab, sizeof(mtab), 1, fp) != 1)
			merror("writing %s", mtabf);
		fclose(fp);
	}
	if (openerr == 3 && uflag)
		merror("could not open %s or %s", mnttabf, mtabf);
}

mlist()
{
	register FILE *fp;

	if ((fp = getmnttab()) != NULL) {
		while (fread(&mnttab, sizeof(mnttab), 1, fp) == 1)
			if (*mnttab.mt_dev != '\0') {
				printf("/dev/%.*s on %.*s", MNTNSIZ,
				    mnttab.mt_filsys, MNTNSIZ, mnttab.mt_dev);
				if (mnttab.mt_ro_flg & MFRON)
					printf(" (read only)");
				else
					printf(" (writeable)");
				printf(" since %s", ctime(&mnttab.mt_time));
			}
		return;
	}
	if ((fp = getmtab()) != NULL) {
		if (qmtab)
			fprintf(stderr,
				"mount: /etc/mtab older than /etc/boottime\n");
		while (fread(&mtab, sizeof(mtab), 1, fp) == 1)
			if (*mtab.mt_name != '\0') {
				printf("/dev/%.*s on %.*s", MNAMSIZ,
				    mtab.mt_special, MNAMSIZ, mtab.mt_name);
				if (mtab.mt_flags & MFRON)
					printf(" (read only)");
				printf("\n");
			}
	}
}

FILE *
getmnttab()
{
	return (fopen(mnttabf, "r+w"));
}

FILE *
getmtab()
{
	time_t boottime;

	if (stat("/etc/boottime", &sbuf) != 0)
		boottime = (time_t)0;
	else
		boottime = sbuf.st_mtime;
	if (stat(mtabf, &sbuf) != 0)
		return (NULL);
	if (sbuf.st_mtime < boottime)
		qmtab = 1;
	return (fopen(mtabf, "r+w"));
}

chkname(name)
char *name;
{
	if (stat(name, &sbuf) < 0)
		merror("%s", name);
	if ((sbuf.st_mode&S_IFMT) != S_IFDIR) {
		errno = ENOTDIR;
		merror("%s", name);
	}
}

chkspec(special)
char *special;
{
	register FILE *fp;
	struct filsys f;

	if (stat(special, &sbuf) < 0)
		merror("%s", special);
	if ((sbuf.st_mode&S_IFMT) != S_IFBLK) {
		errno = ENOTBLK;
		merror("%s", special);
	}
	if ((fp = fopen(special, "r")) == NULL)
		merror("opening %s", special);
	fseek(fp, (long)SUPERI*BSIZE, 0);
	if (fread(&f, sizeof(f), 1, fp) != 1)
		merror("%s", special);
	canf(&f);
	if ( ! tstf(&f)) {
		fprintf(stderr,
			"mount: %s: badly formed file system\n", special);
		exit(1);
	}
}

canf(fp)
register struct filsys *fp;
{
	register daddr_t *dp;
	register ino_t *ip;

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
}

tstf(fp)
register struct filsys *fp;
{
	register daddr_t *dp;
	register ino_t *ip;
	register ino_t maxinode;

	maxinode = (fp->s_isize - INODEI) * INOPB + 1;
	if (fp->s_isize >= fp->s_fsize)
		return (0);
	if (fp->s_tfree < fp->s_nfree
	 || fp->s_tfree >= fp->s_fsize - fp->s_isize + 1)
		return (0);
	if (fp->s_tinode < fp->s_ninode
	 || fp->s_tinode >= maxinode-1)
		return (0);
	for (dp = &fp->s_free[0]; dp < &fp->s_free[fp->s_nfree]; dp += 1)
		if (*dp < fp->s_isize || *dp >= fp->s_fsize)
			return (0);
	for (ip = &fp->s_inode[0]; ip < &fp->s_inode[fp->s_ninode]; ip += 1)
		if (*ip < 1 || *ip > maxinode)
			return (0);
	return (1);
}

/*
 * Copy special pathname (stripped of
 * leading directories) into a fixed
 * size buffer.
 */
mcopy(ms, buf)
char *ms, *buf;
{
	register char *p1, *p2;

	for (p1=p2=ms; *p1 != '\0'; )
		if (*p1++ == '/')
			p2 = p1;
	p1 = buf;
	while (*p1++ = *p2++)
		;
}
