#ifndef	__SYS_STRLOG_H__
#define	__SYS_STRLOG_H__

/*
 * Structures and constants for streams log driver log (7).
 */
/*
 *-IMPORTS:
 *	<common/_clock.h>
 *		clock_t
 *	<common/_time.h>
 *		time_t
 */

#include <common/_clock.h>
#include <common/_time.h>


/*
 * It is not possible to achieve cross-system binary compatibility with any
 * System V program which uses device-dependent ioctl ()'s with the current
 * level of documentation published by USL. The contents and names of fields
 * in the structures defined below are not defined in any published document,
 * so in order to emulate the System V facilities we would have to have used
 * knowledge gained from examination of the System V header files, which are
 * described as unpublished proprietary works.
 *
 * Indeed, since it is not possible to use such facilities without recourse to
 * this proprietary information, we are presented with an interesting
 * conundrum. Not only can we not provide binary compatibility, we cannot even
 * provide source-level compatibility.
 *
 * Field names whose names are known from publically available information are
 * given in that forms. Field names which we known exist, but for which we do
 * not know the names have been assigned names with a special prefix. The
 * data types of these values has been inferred from the published
 * descriptions, and thus may not match System V types, and also may be given
 * in a different order.
 *
 * Caveat utilitor!
 */

struct log_ctl {
	short		mwc_mid;
	short		mwc_sid;
	char		level;
	unsigned short	flags;
	clock_t		mwc_ticks;	/* time logged in ticks */
	time_t		mwc_time;	/* time logged in POSIX format */
	unsigned long	mwc_seqno;
	unsigned short	mwc_pri;
};


#define	SL_ERROR		0x0001
#define	SL_TRACE		0x0002
#define	SL_CONSOLE		0x0004
#define	SL_NOTIFY		0x0008
#define	SL_FATAL		0x0010
#define	SL_WARN			0x0020
#define	SL_NOTE			0x0040


/*
 * Maximum number of arguments that may be passed with a log message. It is
 * not absolutely clear how much space this specifies, but we assume that
 * this many longwords can be passed. Floating-point arguments are not valid,
 * nor are pointers.
 */

enum {	NLOGARGS = 3 };


/*
 * Priority and facility code information derived from the flags.
 */

enum {
	LOG_NOTICE = 0,
	LOG_INFO,
	LOG_DEBUG,
	LOG_WARNING,
	LOG_ERR,
	LOG_CRIT,

	LOG_KERN = 0,
	LOG_USER = 256
};

#endif	/* ! defined (__SYS_STRLOG_H__) */
