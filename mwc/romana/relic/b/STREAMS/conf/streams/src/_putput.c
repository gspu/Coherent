#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * Simple im-memory traceback buffer for kernel debugging.
 *
 * This module exists to service the DDI/DKI cmn_err () function, which takes
 * care of details like locking. This function is a simple debugging output
 * which logs characters to a wrap-around memory buffer that might be useful
 * for post-mortem kernel debugging.
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<common/xdebug.h>
 *		__LOCAL__
 */

#include <common/ccompat.h>
#include <common/xdebug.h>


#ifndef	DEBUG_BUF_SIZE
#define	DEBUG_BUF_SIZE	1024
#endif


#define	SIGNATURE	"$PUTBUF:"


/*
 * Local state for the simple debug output.
 */

__LOCAL__ struct {
	unsigned char	signature [sizeof (SIGNATURE)];
	unsigned	offset;
	unsigned char	buffer [DEBUG_BUF_SIZE];
} _putbuf = {
	SIGNATURE
};


/*
 *-STATUS:
 *	Private to cmn_err ()
 *
 *-NAME:
 *	_put_putbuf ()	simple post-mortem debugging output
 *
 *-ARGUMENTS:
 *	outch		The character to record in the post-mortem traceback
 *			buffer.
 *
 *-DESCRIPTION:
 *	This function provides system-specific debugging output support for
 *	the DDI/DKI cnm_err () function, interfacing to a simple wraparound
 *	traceback buffer.
 *
 *-NOTES:
 *	This function does not sleep.
 *
 *	Driver-defined basic locks, read/write locks and sleep locks may
 *	be held across calls to this function.
 *
 *	In systems where the contents of main memory are not usually
 *	available after a crash and restart, the traceback memory may be
 *	stored off-line (recorded through a serial port), or checkpointed
 *	into memory not touched by the system boot process (such as network
 *	adapter buffer memory). In such systems, the traceback output may
 *	be "buffered" and only flushed when requested to record a '\n'
 */

#if	__USE_PROTO__
void (_put_putbuf) (unsigned char outch)
#else
void
_put_putbuf __ARGS ((outch))
unsigned char	outch;
#endif
{
	_putbuf.buffer [_putbuf.offset] = outch;
	_putbuf.offset = (_putbuf.offset + 1) % sizeof (_putbuf.buffer);
}
