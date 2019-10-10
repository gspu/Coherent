#ifndef SILO_H
#define SILO_H
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
