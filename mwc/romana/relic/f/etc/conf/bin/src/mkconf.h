#ifndef	MKCONF_H
#define	MKCONF_H

#include "devadm.h"


EXTERN_C_BEGIN

int		write_conf_h	PROTO ((CONST char * name));
int		write_conf_c	PROTO ((CONST char * name,
					extinfo_t * extinfop));
int		write_link	PROTO ((CONST char * outname,
					CONST char * inname));

EXTERN_C_END

#endif	/* ! defined (MKCONF_H) */
