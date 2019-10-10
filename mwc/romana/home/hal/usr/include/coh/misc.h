#ifndef	__COH_MISC_H__
#define	__COH_MISC_H__

#include <common/_uid.h>	/* o_dev_t */

/* prototypes from misc.c */

int		busyWait	__PROTO ((int (*f)(void), int));
int		busyWait2	__PROTO ((int (*f)(void), int));
void		devmsg		__PROTO ((o_dev_t dev, char * _format, ...))
				__PRINTF_LIKE (2, 3);
void		panic		__PROTO ((char * _format, ...))
				__PRINTF_LIKE (1, 2);

#endif	/* ! defined (__COH_MISC_H__) */
