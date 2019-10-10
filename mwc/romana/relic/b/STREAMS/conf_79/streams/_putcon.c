#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * Simple console output for kernel debugging.
 *
 * This module exists to service the DDI/DKI cmn_err () function, which takes
 * care of details like locking. This function is a simple debugging output
 * which on PC's expects to write to the screen memory.
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


/*
 * For the IBM PC, simply define a way of making the console directly-
 * addressable memory. A plausible and useful alternative would be to define
 * a "console" virtual terminal that audiovisually signals the operator (via
 * the speaker and by palette "flash") when new information is written to it.
 *
 * That's a creature feature for later.
 */

#if	defined	(GNUDOS)

#include <pc.h>

#define	_putconsole(x,y,c)	ScreenPutChar (c, 0x07, x, y)	
#define	_setcursor(x,y)		ScreenSetCursor (y, x)

#elif	__BORLANDC__

#include <dos.h>

#define	_putconsole(x,y,c)	(* ((int __far *) MK_FP (0xB800, \
					((x) - 1 + ((y) - 1) * 80) * 2)) \
						 = (c) | 0x700)
__LOCAL__ void _setcursor (int x, int y) {
	union REGS	regs;
	regs.h.ah = 0x2;
	regs.h.dh = y;
	regs.h.dl = x;
	regs.h.bh = 0;
	int86 (0x10, & regs, & regs);
}

#else

#error	I do not know how to perform debugging output on this system

#endif


/*
 * Local state for the simple debug output.
 */

__LOCAL__ struct {
	unsigned	x;
	unsigned	y;
	unsigned	width;
	unsigned	height;
} _console = {
	1, 1, 80, 25
};


/*
 *-STATUS:
 *	Private to cmn_err ()
 *
 *-NAME:
 *	_put_console ()	simple console debugging output
 *
 *-ARGUMENTS:
 *	outch		The character to display on the system console. The
 *			'\n' and '\r' characters are interpreted as carriage-
 *			return+line-feed and carriage-return respectively.
 *			It is recommended that '\a' also be interpreted and
 *			audibly signal the console operator if the
 *			implementation can possibly do so.
 *
 *-DESCRIPTION:
 *	This function provides system-specific debugging output support for
 *	the DDI/DKI cnm_err () function, interfacing to the system console
 *	hardware.
 *
 *-NOTES:
 *	This function does not sleep.
 *
 *	Driver-defined basic locks, read/write locks and sleep locks may
 *	be held across calls to this function.
 */

#if	__USE_PROTO__
void (_put_console) (unsigned char outch)
#else
void
_put_console __ARGS ((outch))
unsigned char	outch;
#endif
{
	switch (outch) {

	case '\n':			/* Carriage return... clear to eol */

		while (_console.x < _console.width)
			_putconsole (_console.x ++, _console.y, ' ');
nextline:
		_console.x = 1;

		/*
		 * Don't bother scrolling, just wrap around will do.
		 */

		_console.y = _console.y % _console.height + 1;
		break;

	case '\r':
		_console.x = 0;
		break;

	case '\a':
		break;

	default:
		_putconsole (_console.x ++, _console.y, outch);

		if (_console.x > _console.width)
			goto nextline;

		break;
	}

	_setcursor (_console.x, _console.y);
}
