/*
 * ld/message.c
 * Print messages of various origins.
 */

#include "data.h"

void
message( args )
char * args;
{
	printf( "ld: %r\n", &args );
}

/*
 * Fatal error; print message and exit
 */
void
fatal( args )
char * args;
{
	fprintf( stderr, "ld: %r\n", &args );
	exit( 1 );
}

/*
 * message plus command prototype
 */
void
usage( args )
char * args;
{
	fatal( "%r\n\
Usage: ld [-d] [-e entry] [-k[sysnam]] [-i] [-l<name>] [-m] [-n] [-o file]\\\n\
\t[-C] [-r] [-s] [-u symbol] [-X|x] file ...",
		&args );
}

/*
 * message with filename
 */
void
filemsg( fname, args )
char	*fname, *args;
{
	message( "file %s: %r", fname, &args );
}

/*
 * Message with module and file name
 */
void
modmsg( fname, mname, args )
char * fname;
char   mname[DIRSIZ];
char * args;
{
	if ( mname[0] == 0 )
		filemsg( fname, "%r", &args );

	else
		filemsg( fname, "module %.*s: %r",
			DIRSIZ, mname, &args );
}

/*
 * Message for module passed by pointer
 */
void
mpmsg( mp, args )
mod_t * mp;
char  * args;
{
	modmsg( mp->fname, mp->mname, "%r", &args );
}

/*
 * Message for symbol passed by pointer
 */
void
spmsg( sp, args )
sym_t	*sp;
char	*args;
{
	if ( sp->mod == NULL )
		message( "symbol %.*s: %r", NCPLN, sp->s.ls_id, &args );

	else
		mpmsg( sp->mod, "symbol %.*s: %r", NCPLN, sp->s.ls_id, &args );
}
