#ifndef	MKCONF_H
#define	MKCONF_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		PROTO ()
 */

#include <sys/compat.h>

#ifndef	EXTINFO_T
#define	EXTINFO_T
typedef struct extinfo	extinfo_t;
#endif


EXTERN_C_BEGIN

int		write_conf_h	PROTO ((CONST char * name));
int		write_conf_c	PROTO ((CONST char * name,
					extinfo_t * extinfop));
int		write_link	PROTO ((CONST char * outname,
					CONST char * inname));

EXTERN_C_END

#endif	/* ! defined (MKCONF_H) */
