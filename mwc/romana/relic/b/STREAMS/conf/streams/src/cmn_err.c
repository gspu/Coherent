#define	_DDI_DKI	1
#define	_SYSV4		1

/*
 * Error-reporting code....
 */

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__PROTO ()
 *		__USE_PROTO__
 *		__ARGS ()
 *	<common/xdebug.h>
 *		__LOCAL__
 *	<sys/inline.h>
 *		splhi ()
 *		splx ()
 *	<sys/ksynch.h>
 *		lock_t
 *		LOCK ()
 *		LOCK_ALLOC ()
 *		UNLOCK ()
 *	<common/_null.h>
 *		NULL
 *	<limits.h>
 *		CHAR_BIT
 *	<stdarg.h>
 *		va_list
 *		va_start ()
 *		va_arg ()
 *		va_end ()
 *
 *-PRIVATE DEPENDENCIES:
 *	_put_console ()
 *	_put_putbuf ()
 */

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <sys/inline.h>
#include <sys/ksynch.h>
#include <common/_null.h>
#include <sys/ddi.h>

#include <limits.h>
#include <stdarg.h>

#include <sys/cmn_err.h>

#ifdef	__MSDOS__

#include <process.h>

#endif


/*
 *-GLOBAL VARIABLES:
 *      conlock		lock to single-thread console output
 *	putlock		lock to single-thread putbuf output
 */

/*
 * Note that these locks are always acquired in the order conlock -> putlock;
 * this is required to ensure that the system remains deadlock-free. The
 * hierarchy mechanism is used to enforce this constraint.
 *
 * We use the user namespace for these since if they are actually made public
 * by definitions in <common/xdebug.h> then they will still not conflict with
 * user symbols (ie, we assume that this module is an "implementation"
 * module...
 */

__LOCAL__ lock_t       	      *	_conlock;
__LOCAL__ lock_t	      *	_putlock;


/*
 * The following are not global variables, they're static constants.
 */

static char * message_level [] = {
	"",
	"NOTICE : ",
	"WARNING : ",
	"PANIC : ",
	"<INVALID ERROR LEVEL> : "
};


/*
 * The following external function declarations exist for internal layering
 * purposes and do not appear in any headers. The names begin with a single
 * underscore (reserved for the user with internal linkage, reserved for the
 * system with external linkage) so as not to conflict with any user-defined
 * symbol names.
 *
 * If you find this use of the implementation namespace distasteful, then
 * you'll have to move the function definitions in here.
 *
 * Note also that these functions will have the default language linkage
 * for the translator, ie we allow these functions to be mangled C++. This
 * constrains them to be produced by the same translator used for this
 * module.
 */

#if	__COHERENT__
void		putchar		__PROTO ((int _c));
#define	_put_console(c)		putchar (c)
#else
void		_put_console	__PROTO ((unsigned char _outch));
#endif

void		_put_putbuf	__PROTO ((unsigned char _outch));


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	cmn_err ()	display an error message or panic the system
 *
 *-SYNOPSIS:
 *	#include <sys/cmn_err.h>
 *
 *	void cmn_err (int level, char * format, ...);
 *
 *-ARGUMENTS:
 *	level		"level" indicates the severity of the error condition.
 *			Valid levels are:
 *			CE_CONT	used to continue a previous message or to
 *				display an informative message not connected
 *				with an error.
 *			CE_NOTE	used to display a message preceded with
 *				NOTICE:. This message is used to report system
 *				events that do not necessarily require action,
 *				by may interest the system administrator. For
 *				example, a message saying that a sector on a
 *				disk needs to be accessed repeatedly before it
 *				can be accessed correctly might be noteworthy.
 *			CE_WARN	use to display a message preceded with
 *				WARNING:. This message is used to report
 *				system events that require immediate
 *				attention, such as those where if an action is
 *				not taken, the system may panic. For example,
 *				where a peripheral device does not initialize
 *				correctly, this level should be used.
 *			CE_PANIC used to display a message preceded by PANIC:,
 *				and panic the system. Drivers should use this
 *				level only for debugging or in the case of
 *				severe errors that indicate that the system
 *				cannot continue to function. This level halts
 *				processing.
 *
 *	format		The message to be displayed. By default, the message
 *			is sent both to the system console and to the circular
 *			kernel buffer, "putbuf". If the first character in
 *			"format" is an exclamation point ("!"), the message
 *			goes only to "putbuf". If the first character in
 *			"format" is a circumflex ("^"), the message goes only
 *			to the console. The size of the circular buffer
 *			"putbuf" is defined by the kernel variable "putbufsz".
 *			Driver developers or administrators can read the
 *			"putbuf" buffer using appropriate debugging or
 *			administrative tools.
 *
 *			cmn_err () appends '\n' to each "format" string, even
 *			when a message is sent to "putbuf", except when level
 *			is CE_CONT.
 *
 *			Valid conversion specifications are %s, %u, %d, %o,
 *			and %x. The cmn_err () function is otherwise similar
 *			to the printf () library subroutine is its
 *			interpretation of the "format" string, except that
 *			cmn_err () does not accept length specifications in
 *			conversion specifications. For example, %3d is invalid
 *			and will be treated as a literal string, resulting in
 *			a mismatch of arguments.
 *
 *	...		the set of arguments passed with the message being
 *			displayed. Any argument within the range of supported
 *			conversion specifications can be passed.
 *
 *-DESCRIPTION:
 *	cmn_err () displays a specified message on the console and/or stores
 *	it in the kernel buffer "putbuf". cmn_err () can also panic the
 *	system.
 *
 *	At times a driver may encounter error conditions requiring the
 *	attention of a system console monitor. These conditions may mean
 *	halting the system; however, this must be done with caution. Except
 *	during the debugging stage, or in the case of a serious, unrecoverable
 *	error, a driver should never stop the system.
 *
 *	The cmn_err () function with the CE_CONT argument can be used by
 *	driver developers as a driver code debugging tool. However, using
 *	cmn_err () in this capacity can change system timing characteristics.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	If "level" is CE_PANIC, then driver defined basic locks, read/write
 *	locks, and sleep locks may not be held across calls to this function.
 *	For other levels, locks may be held.
 */

enum {	OUT_CONSOLE = 1,
	OUT_PUTBUF = 2,
	OUT_LONG = 4,
	OUT_SHORT = 8,
	OUT_SIGNED = 16
};

typedef	char 	      *	_string_t;	/* for va_arg () */
typedef unsigned short 	_ushort_t;
typedef unsigned int	_uint_t;
typedef unsigned long	_ulong_t;


#if	__USE_PROTO__
void (cmn_err) (int level, char * format, ...)
#else
void
cmn_err __ARGS ((level, format))
int		level;
char	      *	format;
#endif
{
	/*
	 * The following local data is used for converting numeric data to
	 * strings. Numbuf is defined such that it is large enough to hold
	 * the maximum number of digits that could be generated from any
	 * conversion specification (currently, the longest possible spec is
	 * %lo).
	 */

	char		numbuf [(sizeof (long) * CHAR_BIT  + 2) / 3 + 1];
	char	      *	numptr;
	int		radix;
	long		svalue;
	unsigned long	value;

	va_list		args;

	unsigned	output;		/* who to output to */

	pl_t		con_pl;		/* for locking */
	pl_t		put_pl;


	if (format == NULL)
		format = "<cmn_err: no error message was supplied>";

	if (* format == '!') {
		format ++;
		output = OUT_PUTBUF;
	} else if (* format == '^') {
		format ++;
		output = OUT_CONSOLE;
	} else
		output = OUT_PUTBUF | OUT_CONSOLE;


	/*
	 * Before we generate any output, we should lock the console/putbuf
	 * so that our messages won't get interspersed with diagnostic output
	 * from interrupt handlers or other CPUs.
	 *
	 * If we get called before the locks have been allocated, proceed
	 * at the highest interrupt priority as a fallback.
	 */

	if (_conlock != NULL) {
		if ((output & OUT_CONSOLE) != 0)
			con_pl = LOCK (_conlock, plhi);

		if ((output & OUT_PUTBUF) != 0)
			put_pl = LOCK (_putlock, plhi);
	} else
		con_pl = splhi ();


	/*
	 * Check that the level argument is reasonable.
	 */

	if (level < CE_CONT || level > CE_PANIC)
		level = CE_INVALID;


	/*
	 * We should think about generating the date and as accurate a time
	 * reading as possible along with the fixed-text header.
	 *
	 * For now, we'll just generate some fixed text.
	 */

	numptr = message_level [level];

	while (* numptr != 0) {
		if ((output & OUT_CONSOLE) != 0)
			_put_console (* numptr);

		if ((output & OUT_PUTBUF) != 0)
			_put_putbuf (* numptr);

		numptr ++;
	}


	/*
	 * Now start generating formatted output.
	 *
	 * The manual page quoted in the function header doesn't say whether
	 * the 'l' and 'h' modifiers are available for conversion
	 * specifications. We'll implement them anyway.
	 */

	va_start (args, format);

	while (* format != 0) {
		if (format [0] == '%') {
			/*
			 * Process the conversion-specifications.
			 *
			 * We temporarily borrow 'radix' as a temporary for
			 * working out how long the conversion specification
			 * is.
			 */


			output &= ~ (OUT_LONG | OUT_SHORT | OUT_SIGNED);

			radix = 2;

			if (format [radix - 1] == 'h') {
				output |= OUT_SHORT;
				radix ++;
			} else if (format [radix - 1] == 'l') {
				output |= OUT_LONG;
				radix ++;
			}

			switch ((format += radix) [-1]) {

			case 's':
				numptr = va_arg (args, _string_t);
				goto out_string;

			case 'd':
				/*
				 * This is the only signed numeric conversion
				 * specification, so we process the sign here.
				 */

				radix = 10;

				if ((output & OUT_SHORT) != 0)
					svalue = (short) va_arg (args, int);
				else if ((output & OUT_LONG) != 0)
					svalue = va_arg (args, long);
				else
					svalue = va_arg (args, int);

				if (svalue < 0) {
					if ((output & OUT_CONSOLE) != 0)
						_put_console ('-');

					if ((output & OUT_PUTBUF) != 0)
						_put_putbuf ('-');

					svalue = - svalue;
				}

				output |= OUT_SIGNED;
				break;

			case 'o':
				radix = 8;
				break;

			case 'u':
				radix = 10;
				break;

			case 'x':
				radix = 16;
				break;

			case '%':
				/*
				 * Special case: %% -> %
				 */

				format -= 1;
				goto bad_format;

			default:
				/*
				 * It's not a valid conversion, so we back
				 * up "format".
				 */
				format -= radix;
				goto bad_format;
			}


			/*
			 * All numeric conversions come here.
			 */

			if ((output & OUT_SIGNED) != 0)
				value = svalue;
			else if ((output & OUT_SHORT) != 0)
				value = (_ushort_t) va_arg (args, _uint_t);
			else if ((output & OUT_LONG) != 0)
				value = va_arg (args, _ulong_t);
			else
				value = va_arg (args, _uint_t);


			/*
			 * Now we generate the output digits one-at-a-time in
			 * reverse order. We step back from the end of the
			 * conversion buffer so that at the end of the
			 * conversion process we have a C string with the
			 * characters in a 'normal' order.
			 *
			 * (This allows the final output loop to be folded in
			 *  with the string-output code).
			 */

			numptr = & numbuf [sizeof (numbuf) - 1];

			* numptr = 0;	/* terminate the output string */

			do {
				int	rem = (int) (value % radix);
				value /= radix;

				* -- numptr = (rem < 10) ? rem + '0' :
							   rem + 'a' - 10;
			} while (value > 0);


			/*
			 * Copy temporary numeric-conversion string or
			 * user-supplied string argument to the output.
			 */

out_string:
			while (* numptr != 0) {
				if ((output & OUT_CONSOLE) != 0)
					_put_console (* numptr);

				if ((output & OUT_PUTBUF) != 0)
					_put_putbuf (* numptr);

				numptr ++;
			}
			continue;
		}
bad_format:
		if ((output & OUT_CONSOLE) != 0)
			_put_console (* format);

		if ((output & OUT_PUTBUF) != 0)
			_put_putbuf (* format);

		format ++;
	}


	/*
	 * Output a trailing '\n' unless the level is CE_CONT
	 */

	if (level != CE_CONT) {
		if ((output & OUT_CONSOLE) != 0)
			_put_console ('\n');

		if ((output & OUT_PUTBUF) != 0)
			_put_putbuf ('\n');
	}


	/*
	 * Now release the locks we acquired, and consider returning to the
	 * caller (unless this is a panic).
	 *
	 * A minor point: if we panic, we should release our locks but
	 * without causing interrupts to be enabled on this CPU; in order to
	 * do this we kludge "con_pl" and "put_pl" to be "plhi";
	 */

	if (level >= CE_PANIC)		/* panic on an invalid level */
		con_pl = put_pl = plhi;


	if (_conlock != NULL) {
		if ((output & OUT_PUTBUF) != 0)
			UNLOCK (_putlock, put_pl);

		if ((output & OUT_CONSOLE) != 0)
			LOCK (_conlock, con_pl);
	} else
		(void) splx (con_pl);


	if (level >= CE_PANIC) {
		/*
		 * This CPU is shutting down. The actions to be taken here
		 * are so highly system-specific that we'll just throw them
		 * into another file.
		 */

#ifdef	__MSDOS__
		splx (plbase);

		exit (3);	/* call atexit () functions */
#endif
	}
}
