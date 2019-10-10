#ifndef	__SYS_CMN_ERR_H__
#define	__SYS_CMN_ERR_H__

/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__PROTO ()
 */

#include <common/ccompat.h>


/*
 * This header defins prototypes and constants for the DDI/DKI error-reporting
 * function cmn_err ().
 */

/*
 * Error severity
 */

enum {	CE_CONT	= 0,		/* used to continue a previous message */
	CE_NOTE,		/* used to display a NOTICE: message */
	CE_WARN,		/* used to display a WARNING: message */
	CE_PANIC,		/* used to display a PANIC: message */
	CE_INVALID		/* used to warn about invalid severity */
};


__EXTERN_C_BEGIN__

void		cmn_err		__PROTO ((int _level, char * _format, ...));

__EXTERN_C_END__



#endif	/* ! defined (__SYS_CMN_ERR_H__) */
