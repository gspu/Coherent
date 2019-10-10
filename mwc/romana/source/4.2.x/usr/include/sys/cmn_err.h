/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_CMN_ERR_H__
#define	__SYS_CMN_ERR_H__

/*
 * This header defins prototypes and constants for the DDI/DKI error-reporting
 * function cmn_err ().
 */

#include <common/ccompat.h>

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

void		cmn_err		__PROTO ((int _level, char * _format, ...))
				__PRINTF_LIKE (2, 3);

__EXTERN_C_END__

#endif	/* ! defined (__SYS_CMN_ERR_H__) */
