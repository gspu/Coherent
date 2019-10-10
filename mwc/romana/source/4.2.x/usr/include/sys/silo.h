/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef __SYS_SILO_H__
#define __SYS_SILO_H__

/*
 * Raw Character Silo.
 */

#define	SI_BUFSIZ	512	/* Silo buffer size */

typedef struct silo {
	int	si_ix;
	int	si_ox;
	int	si_cnt;
	unsigned char	si_buf[SI_BUFSIZ];
} silo_t;

#endif	/* ! defined (__SYS_SILO_H__) */
