/**
 *	strings -- prints out all the character strings in an executable file.
 *
 *	Usage: strings [-xaop] [-nnn] [file ...]
 *
 *	Options:
 *		 -x	print offset of each string in the file in hex
 *		 -a	print offset of each string in the file in ASCII
 *		 -o	print offset of each string in the file in octal
 *		 -p	mask out the parity bit
 */

#include	<sys/stat.h>
#include	<l.out.h>
#include	<stdio.h>
#include	<canon.h>
#include	<ctype.h>

/* The lamentable <rico.h>... */
#define	bool	char
#define	TRUE	(0 == 0)
#define	FALSE	(!TRUE)

int	nchars	= 4;	/* minimum character string length */

bool	flag;		/* input condition: FALSE for redirection, else TRUE */
bool	aflag;		/* option flags:	ascii line numbering */
bool	xflag;		/*			hex   line numbering */
bool	oflag;		/*			octal line numbering */

int	pbit;		/* parity bit mask */
long	addr;		/* current seek position in data space segment */
long	count;		/* size of data space segment */
char  * buff;		/* string to store beginning NCHARS
					characters in character string */
bool	printstr();
bool	lookfor();

extern	FILE	* freopen();
extern	char	* malloc();


main( argc, argv )

int	argc;
char  * argv[];

{
	char	* malloc();
	char	* s;

	while( (--argc > 0) && ((*++argv)[0] == '-') ) {

		/*
		 * Read options declared.
		 */
		for( s = argv[0] + 1 ; *s != '\0'; s++ ) {

			/*
			 * Set minimum string length for recognizing
			 * character strings.
			 */
			if( isdigit( *s ) ) {
				if( (nchars = atoi( s )) <= 0 )
					fatal( "string length must be greater than 0" );

				break;
			}

			/*
			 * Set appropriate flags or bit mask according to
			 * option declared.
			 */
			switch( *s ) {

			case 'x':
				if( aflag | oflag | xflag )
					usage();

				xflag = TRUE;
				break;

			case 'a':
				if( aflag | oflag | xflag )
					usage();

				aflag = TRUE;
				break;

			case 'o':
				if( aflag | oflag | xflag )
					usage();

				oflag = TRUE;
				break;

			case 'p':
				pbit = 0200;
				break;

			default:
				usage();
			}
		}
	}

	/*
	 * Get memory for string buffer.
	 */
	if( (buff = malloc( nchars )) == NULL )
		fatal( "not enough memory" );

	/*
	 * Set input condition:
	 *	FALSE if input is through indirection, else TRUE.
	 */
	if( argc > 0 )
		flag = TRUE;
	else {
		argc++;
		flag = FALSE;
	}

	/*
	 * Examine each file for strings of the required length.
	 */
	while( argc-- ) {

		/*
		 * Test for read permission in file.
		 */
		if( flag  &&  freopen( *argv, "r", stdin ) == NULL )
			fatal( "can't open %s", argv[0] );

		/*
		 * Test if file is an executable file.
		 */
		if( checklout( stdin ) < 0 ) {
			if( flag ) {
				fprintf( stderr, "strings: '%s' not executable\n", *argv);
				++argv;
				continue;
			}
			else {
				fprintf( stderr, "strings: not executable\n" );
				exit( 1 );
			}
		}

		/*
		 * Display file name if available.
		 */
		if( flag )
			fprintf( stderr, "%s:\n", *argv );

		/*
		 * Display all character strings in file.
		 */
		while( printstr() )
			;

		++argv;
	}

	free( buff );
	exit( 0 );
}

/**
 * bool
 * printstr()
 *
 *	Input:	None.
 *
 *	Action:	Display character strings in file.
 *
 *	Return:	FALSE on error or no character strings available,
 *		otherwise TRUE.
 *
 *	Note:	None.
 *
 */

bool
printstr()

{
	char	c;
	char  * p;
	char  * q;

	p = buff;
	q = p + nchars;

 	/*
	 * Check if character strings are available in file.
	 */
	if( ! lookfor() )
		return( FALSE );

	/*
	 * Display line number according to the format option specified.
	 * Ignore if no format option is set.
	 */
	if( aflag )
		printf( "%-6D: ", addr-nchars );

	else if( oflag )
		printf( "0%-6O: ", addr-nchars );

	else if( xflag )
		printf( "0x%-6X: ", addr-nchars );

	/*
	 * Display printable characters found.
	 */
	do {
		putchar( *p++ );
	} while( p < q );

	/*
	 * Display remaining printable characters in file.
	 */
	for ( ;; ) {
		if( --count <= 0L )
			return( FALSE );

		++addr;
		c = getchar();

		if( c == EOF )
			return( FALSE );

		if( ! (isascii( (c) & ~pbit ) &&  isprint( (c) & ~pbit )) )
			break;

		putchar( c );
	}

	putchar( '\n' );
	return( TRUE );
}

/**
 * bool
 * lookfor()
 *
 *	Input:	None.
 *
 *	Action:	Look for the first character string in file.
 *
 *	Return: TRUE if a character string is found, else FALSE.
 *
 *	Note:	None.
 *
 */

bool
lookfor()

{
	char	c;
	char  * p;
	char  * q;

	q = buff + nchars;

	for( ;; ) {

		/*
		 * Seek in data space segment of file until a printable
		 * character is encountered.
		 */
		do {
			if( --count <= 0L )
				return( FALSE );

			++addr;
			c = getchar();

			if( c == EOF )
				return( FALSE );

		} while( ! (isascii( (c) & ~pbit ) && isprint( (c) & ~pbit )) );

		p = buff;

		/*
		 * Check for valid character string.
		 */
		do {
			*p++ = c;

			if( p >= q )
				return( TRUE );

			if( --count <= 0L )
				return( FALSE );

			++addr;
			c = getchar();

			if( c == EOF )
				return( FALSE );

		} while( isascii( (c) & ~pbit ) && isprint( (c) & ~pbit ) );
	}
}


/**
 * int
 * checklout( f )
 *
 * register FILE * f;
 *
 *	Input:	f	- file pointer to stdin.
 *
 *	Action:	Check if executable (l.out) file.
 *
 *	Return: Returns -1 on error or file is not executable, otherwise 0.
 *
 *	Note:	None.
 *
 */

int
checklout( f )

register FILE	* f;

{
	register int	  i;		/* counter variable */
	int		  ret = -1;	/* returning error value */
	struct	 stat	  s;		/* file attributes */
	struct	 ldheader l;		/* file header information */

	if( fstat( fileno( f ), &s ) == -1 )
		return( ret );

	/*
	 * File type check. Return unless regular file.
	 */
	if( (s.st_mode & S_IFMT) != S_IFREG )
		return( ret );

	/*
	 * Get file header information.
	 */
	if( fread( &l, sizeof( l ), 1, f ) != 1 ) {
		rewind( f );
		return( ret );
	}

	rewind( f );

	if( l.l_magic != L_MAGIC )
		return( ret );

	/*
	 * Correct canonical effect on file format information.
	 */
	for( i = 0 ; i < NLSEG ; ++i )
		canlong(l.l_ssize[i]);

	/*
	 * Get seek position to the beginning memory location of the
	 * data space segment in the executable file.
	 */
	fseek( f, (long)sizeof(l) + l.l_ssize[L_SHRI] + l.l_ssize[L_PRVI], 0 );
	addr = ftell( f );

	/*
	 * Get size of data space segment of file.
	 */
	count = l.l_ssize[L_SHRD] + l.l_ssize[L_PRVD];

	if( count == 0L ) {
		fatal( "zero-length data segments" );
		/* NOTREACHED */
	}

	++count;
	return( 0 );
}

/**
 * void
 * fatal( arg0 )
 *
 * char	* arg0;
 *
 *	Input:	arg0	- pointer to string containing the error message.
 *
 *	Action:	Display fatal error message and exit.
 *
 *	Return:	Never return, always exit.
 *
 *	Note:	None.
 *
 */

fatal( arg0 )

char	* arg0;

{
	fflush( stdout );
	fprintf( stderr, "strings: %r\n", &arg0 );
	exit( 1 );
}

/**
 * void
 * usage()
 *
 *	Input:	None.
 *
 *	Action:	Display command usage format.
 *
 *	Return:	Never return, always exit.
 *
 *	Note:	None.
 *
 */

usage()

{
	fprintf( stderr, "Usage: strings [-xaop] [-nnn] [file ...]\n" );
	exit( 1 );
}
