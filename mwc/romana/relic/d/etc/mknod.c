/*
 * mknod -- make a special file or named pipe
 * Usage: /etc/mknod filename type major minor
 *        /etc/mknod filename p
 * The 'type' is 'b' for block special or 'c' for character special;
 * 'major' and 'minor' are numbers.
 * The second form creates a pipe with the given filename.
 */

#include <sys/stat.h>

#define	NMAJOR	256
#define	NMINOR	256

int force;
struct stat sb;

main( argc, argv )

int argc;
register char **argv;

{
	register unsigned mode;
	unsigned maj, min;
	dev_t dev;

	if ( argc >= 2 && strcmp( argv[1], "-f" ) == 0 ) {
		++force;
		++argv;
		--argc;
	}

	if ( argc < 3 )
		usage();

	if ( strlen( argv[2] ) != 1 )
		usage();

	switch ( argc ) {

	case 3:
		if ( argv[2][0] != 'p' )
			usage();

		mode = S_IFPIP;
		dev = 0;
		break;

	case 5:	
		if ( getuid() != 0 )
			fatal( "not superuser\n", 0 );

		switch ( argv[2][0] ) {
		
		case 'c':			/* mknod name c major minor */
			mode = S_IFCHR;
			break;

		case 'b':			/* mknod name b major minor */
			mode = S_IFBLK;
			break;

		default:
			usage();
		}

		if ((maj = atoi(argv[3])) >= NMAJOR)
			usage();

		if ((min = atoi(argv[4])) >= NMINOR)
			usage();

		dev = makedev( maj, min );
		break;

	default:
		usage();
	}

	if ( (stat( argv[1], &sb )) >= 0 ) {

		if ((sb.st_mode & S_IFMT) == S_IFDIR)
			usage();

		if (force)
			unlink( argv[1] );
	}

	if ( mknod( argv[1], mode | 0777, dev ) < 0 )
		fatal( "can't mknod: ", argv[1] );

	exit( 0 );
}

usage()
{
	fatal( "\
Usage: mknod [-f] filename b major minor\n\
       mknod [-f] filename c major minor\n\
       mknod [-f] filename p", 0 );
}

fatal( mesg, file )
register char *mesg, *file;
{
	write( 2, mesg, strlen(mesg) );
	if (file)
		write( 2, file, strlen(file) );
	write( 2, "\n", 1 );
	exit( 1 );
}

/*
 * atoi ( s )
 *
 *	Input:	s = numeric string to evaluate.
 *
 *	Action:	Convert numeric string into a positive integer.
 *
 *	Return:	-1 = invalid character(s) encountered.
 *		*  = numeric equivalent.
 */

atoi( s )
register char *s;
{
	register int n = 0;

	while ( *s ) {

		if ( ('0' <= *s) && (*s <= '9') ) {
			n *= 10;
			n += *s++ - '0';
		}
		else
			return -1;
	}
	return n;
}
