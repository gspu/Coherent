/*
 * /usr/src/cmd/cp.c
 * Usage: cp [ -d ] file1 file2
 *	  cp [ -d ] file ... directory
 * Copy one file to another or
 * copy several files into a directory.
 * define SLOW for 'block at a time copying' .... (not recommended)
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#define	MAX(a, b)	((a) < (b) ? (b) : (a))
#define	or		||
#define	and		&&
#define	not		!
#define	TRUE		(0==0)
#define	FALSE		(not TRUE)

extern	int	errno;
struct stat	sb;

#ifndef		SLOW
char	buf[50*BUFSIZ];
#else
char	buf[BUFSIZ];
#endif
int	dflag;
char	*namebuf;
char	*suffix;
char	*target;

main(argc, argv)
int argc;
char *argv[];
{
	register int i;
	register int n;
	register int status = 0;

	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'd' && argv[1][2] == '\0') {
		++dflag;
		--argc;
		++argv;
	}
	n = init(argc, argv);
	if (namebuf == NULL)
		return (cp(argv[1], target) ? 0 : 1);
	for (i = 1; i <= n; ++i)
		status |= cpdir(argv[i]);
	return (status ? 0 : 1);
}

/*
 * Copy inf to outf.
 */
cp(inf, outf)
char *inf, *outf;
{
#ifndef SLOW
	register char	*ip;
#endif
	register int	n;
	register int	i;
	register short	infd;
	register short	outfd;
	register short	mode;
	register ino_t	ino;
	register dev_t	dev;
	register fsize_t size;
	time_t		times[2];
#ifndef SLOW
	register char	*wp;
	register int	wflag;
#endif

	if (stat(inf, &sb) < 0) {
		cperr("%s: cannot find", inf);
		return (FALSE);
	}
	if ((sb.st_mode & S_IFMT) == S_IFDIR) {
		cperr("%s: directory", inf);
		return (FALSE);
	}
	mode = sb.st_mode;
	size = sb.st_size;
	ino = sb.st_ino;
	dev = sb.st_dev;
	if (dflag)
		times[1] = sb.st_mtime;

	if (stat(outf, &sb) >= 0)
		if (sb.st_ino==ino && sb.st_dev==dev) {
			cperr("%s: cannot copy file to itself", inf);
			return (FALSE);
		}
	if ((infd = open(inf, 0)) < 0) {
		cperr("%s: cannot open", inf);
		return (FALSE);
	}
	if ((outfd = creat(outf, mode & 06777)) < 0) {
	   if (errno == ETXTBSY)
	      cperr("%s: cannot copy over busy shared text file", outf);
	   else
	      cperr("%s: cannot create", outf);
	   close(infd);
	   return (FALSE);
	}

#ifndef SLOW
	while ((n = read(infd, buf, sizeof(buf))) > 0) {
#else
	while ((n = read(infd, buf, BUFSIZ)) > 0) {
#endif
		/*
		 * Check for blocks of zeroes (holes in a sparse file).
		 * However, a block of zeroes at the end of a file must be
		 * written so the file has correct length.
		 */
#ifndef		SLOW
		wp = ip = buf;
		size -= n;
		while ((n-BUFSIZ) > 0 || ((n-BUFSIZ) == 0 && size != 0)) {
			n -= BUFSIZ;
			wflag = FALSE;
			ip = wp;
			for (i = 0; i < BUFSIZ; ++i)
				if (*ip++ != '\0') {
					wflag = TRUE;
					break;
				}
			if (wflag) {
				if (write(outfd, wp, BUFSIZ) < BUFSIZ) {
					cperr("%s: write error", outf);
					close(infd);
					close(outfd);
					return (FALSE);
				}
			} else {
				lseek(outfd, (long)BUFSIZ, 1);
			}
			wp += BUFSIZ;
		}
		if (write(outfd, wp, n) < n) {
			cperr("%s: write error", outf);
			close(infd);
			close(outfd);
			return (FALSE);
		}
#else
		if ((size -= n) == (fsize_t)0)
			goto WRITE;
		for (i = 0; i < n; ++i)
			if (buf[i] != '\0')
				goto WRITE;
		lseek(outfd, (long)BUFSIZ, 1);
		continue;

		WRITE:
		if (write(outfd, buf, n) < n) {
			cperr("%s: write error", outf);
			close(infd);
			close(outfd);
			return (FALSE);
		}
#endif
	}

	close(infd);
	if (dflag && fstat(outfd, &sb) != -1) {
		times[0] = sb.st_atime;
		if (utime(outf, times) == -1)
			cperr("%s: cannot preserve mtime", outf);
	}
	close(outfd);
	if (n < 0) {
		cperr("%s: read error", inf);
		return (FALSE);
	}
	return (TRUE);
}

/*
 * The second form of cp, copy a file into a directory. This builds the name
 * of the target file in namebuf and then just calls `cp'.
 */
cpdir(file)
char *file;
{
	register char *a, *b;

	for (a = b = file; *b != '\0'; )
		if (*b++ == '/')
			a = b;
	strcpy(suffix, a);
	return (cp(file, namebuf));
}

usage()
{
	fprintf(stderr,
		"Usage:\tcp [ -d ] file1 file2\n"
		      "\tcp [ -d ] file ... directory\n"
		);
	exit (2);
}

cperr(arg0)
{
	fprintf(stderr, "cp: %r\n", &arg0);
}

/*
 * Check whether target is a directory, regular file, non-extant or in error.
 * If a directory, malloc namebuf big enough for all names.
 * Also, set flags used by cperr.
 */
init(ac, av)
register int ac;
register char *av[];
{
	register int n;

	if (ac < 3)
		usage();
	target = av[ac-1];
	if (stat(target, &sb) == 0  and  (sb.st_mode & S_IFMT) == S_IFDIR) {
		av[ac-1] = NULL;
		n = 0;
		while (*++av != NULL)
			if (n < strlen(*av))
				n = strlen(*av);
		namebuf = malloc(n + strlen(target) + 2);
		if (namebuf == NULL) {
			cperr("out of memory");
			exit(1);
		} 
		strcpy(namebuf, target);
		suffix = namebuf + strlen(target);
		*suffix++ = '/';
		return (ac - 2);
	}
	if (ac != 3)
		usage();
	return (1);
}

/* end of cp.c */
