/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
#ifndef __SYS_SILO_H__
#define __SYS_SILO_H__
/*
 * Raw Character Silo.
 */

#define	SI_BUFSIZ	256	/* Silo buffer size */

typedef struct silo {
	unsigned char	si_ix;
	unsigned char	si_ox;
	unsigned char	si_buf[SI_BUFSIZ];
} silo_t;
#endif
