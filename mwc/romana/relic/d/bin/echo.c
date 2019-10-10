/*
 * echo -- print arguments
 *
 * Usage:	echo [ -n ] [ argument ... ]
 *
 * Option:	-n	Do not print terminal newline
 *
 * Note:	The following C-like escape conventions are usable with
 *		ECHO; beware of conflicts with the shell's use of \:
 *			\b	backspace
 *			\c	print line without new-line
 *				(same as -n option)
 *			\f	form-feed
 *			\n	new-line
 *			\r	carriage returnm
 *			\t	tab
 *			\v	vertical tab
 *			\\	backslash
 *			\0x	where x represents the octal value of an
 *				ASCII character
 *
 */

static char	buf[512];			/* working buffer */
static char   * bufend	= &buf[sizeof buf];	/* pointer to end ogf buf */
static char   * bufp	= &buf[0];	/* pointer to current offset in buf */
static char	lastch	= '\n';		/* last output char (if non-zero) */
static int	asc;			/* octal value */

main( argc, argv )
int	argc;
register char	** argv;
{
	extern	echos();

	if ( strcmp( *++argv, "-n" ) == 0 ) {
		lastch = 0;
		++argv;
	}

	/*
	 * Read all argument strings into buffer. Separate each argument
	 * with a blank.
	 */
	while ( *argv ) {
		echos( *argv );
		if ( *++argv )
			echos( " " );
	}

	if ( *bufp = lastch )
		++bufp;

	if ( bufp > buf )
		write( 1, buf, bufp - buf );

	exit( 0 );
}

/*
 * static
 * echos( sp )
 * char * sp;
 *
 *	Input:	sp	- pointer to string to append to output buffer.
 *
 *	Action:	Transfer string to output buffer at offset specified by 'bufp'.
 *		If buffer becomes full during transfer, write buffer to stdout,
 *		and wrap 'bufp' back to beginning of buffer.
 */

static
echos( sp )
register char	* sp;
{
	register char	* dp = bufp;
	extern char *	getoct();

 	/*
	 * Copy argument to buffer.
	 */
	for ( ; *sp ; ++sp ) {
		if ( *sp == '\\' ) {
			if ( *++sp == '\0' )
				break;

			switch (*sp) {
			case 'b': *dp = '\b';	break;
			case 'f': *dp = '\f';	break;
			case 'n': *dp = '\n';	break;
			case 'r': *dp = '\r';	break;
			case 't': *dp = '\t';	break;
			case 'v': *dp = 013;	break;
			case 'c': lastch = 0; --dp; break;
			case '0': sp  = getoct( ++sp );
				  *dp = (char)asc;
				  break;
			default : *dp = *sp;	break;
			}
		} else
			*dp = *sp;

	 	/*
		 * Empty buffer to standard output if full.
		 */
		if ( ++dp == bufend ) {
			write( 1, buf, sizeof buf );
			dp = buf;
		}
	}

	bufp = dp;
}

/*
 * char *
 * getoct( s )
 * char * s;
 *
 *	Input:	s	- pointer to current offset in string.
 *
 *	Action:	Read octal value from string ( up to 3 characters ).
 *
 *	Return:	Pointer to current offset in string.
 */
char *
getoct( s )
char	* s;
{
	int	i;	/* working counter */

	i   = 0;
	asc = 0;	/* clear octal value buffer */

	while ( (*s) && (*s >= '0') && (*s <= '7') && ( i < 3 ) ) {
		asc = asc * 8 + *s - '0';	/* octal value accumulation */
		++s;
		++i;
	}

	return( --s );
}
