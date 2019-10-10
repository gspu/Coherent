/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * fontgen - generate 16x8 font assembler source on stdout.
 */

#define	BIT(n)	(1 << (n))
#define	BITS(n)	(((n) >= 4) ? (3 << (((n) - 4) * 2)) : (3 << (((n) + 4) * 2)))

unsigned char ibuf[1024];
unsigned char odef[] = "\tB________ = 0000000\n";
unsigned char obuf[] = "\t.word\tB________\n";

main()
{
	register unsigned s;
	register unsigned j;
	register unsigned i;
	extern long lseek();

	/*
	 * Read fonts from /dev/mem at offset 0xFFA6E.
	 */
	close(0);
	if (open("/dev/mem", 0) != 0)
		fatal("/dev/mem: can't open\n");
	if (lseek( 0, 0xFFA6EL, 0) != 0xFFA6EL)
		fatal("/dev/mem: can't seek to fonts");
	if (read( 0, ibuf, sizeof ibuf ) != sizeof ibuf)
		fatal("/dev/mem: can't read fonts");

	/*
	 * Define symbolic constants.
	 */
	for (i=0; i < 256; ++i) {
		/* Generate constant's name. */
		s = i;
		for (j=10; --j >= 2; s>>=1)
			odef[j] = (s&1) ? 'X' : '_';

		/* Convert 8 pixel width to 16 pixels */
		s = 0;
		for (j=0; j < 8; ++j)
			if (i & BIT(j))
				s |= BITS(j);

		/* Generate constant's value */
		for (j=19; j >= 14; --j) {
			odef[j] = (s & 7) + '0';
			s >>= 3;
		}

		/* Print constant's name and value */
		send( odef );
	}
	send( "\n\t.globl\tfontw_\nfontw_:" );

	/*
	 * Define fonts for 128 characters.
	 */
	for (i=0; i < sizeof ibuf; ++i) {

		/* Format and print one pixel line */
		j = 16;
		s = ibuf[i];
		while ( --j >= 8 ) {
			obuf[j] = (s&1) ? 'X' : '_' ;
			s >>= 1;
		}
		send( obuf );

		/* Insert blank line between characters for readability */
		if ( (i & 7) == 7 )
			send( "\n" );
	}
	exit( 0 );
}

/*
 * Fatal( msg ) - report error, and abort.
 */

fatal( msg )
register char *msg;
{
	write( 2, msg, strlen(msg) );
	exit( 1 );
}

/*
 * send( s ) - write null-terminated string to standard output.
 */

send( s )
register char *s;
{
	write( 1, s, strlen(s) );
}
