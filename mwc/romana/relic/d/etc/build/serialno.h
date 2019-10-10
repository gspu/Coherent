/*
 * serialno.h
 * COHERENT serial number encryption.
 */

#define VERSIONS	3
#define	NSERIAL		(9+1)		/* Length of serial number + NUL */

/* numtab.c */
extern int 	magic1[];
extern int 	magic2[];
extern int 	magic3[];

/* serialno.c */
extern	char	serialno[NSERIAL];
extern	int	isserial();

/* end of serialno.h */
