

/*
 * sum -- generate checksum
 */
#include	<errno.h>
#include	<stdio.h>


#define	BUFSZ	(20*512)


int	err;
char	*filename;		/* used by error( ) */
extern	errno;


main( argc, argv)
register char	**argv;
{
	register int fd;

	if (*++argv == NULL) {
		filename = "[stdin]";
		sum( 0, (char *)0);
	}
	else
		while (filename = *argv++) {
			if ((fd = open( filename, 0)) < 0) {
				error( (char *)0);
				continue;
			}
			sum( fd, argc>2? filename: (char *)0);
			close(fd);
		}

	return (err);
}


sum( fd, file)
int	fd;
char	*file;
{
	register char	*p;
	register	a,
			n;
	long		size;
	static char	buf[BUFSZ];

	a = 0;
	size = 0;

	while ((n=read( fd, buf, sizeof buf)) > 0) {
		size += n;
		p = buf;
		do {
			a += *p++;
		} while (--n);
	}

	if (n < 0)
		error( errno==EIO? "read error": (char *)0);
	printf( "%5u %2ld", a, (size+BUFSIZ-1)/BUFSIZ);
	if (file)
		printf( " %s", file);
	printf( "\n");
}


error( mesg)
char	*mesg;
{
	extern char	*sys_errlist[];

	fprintf( stderr, "sum: %s: %s\n", filename,
		mesg? mesg: sys_errlist[errno]);
	err = 1;
}
