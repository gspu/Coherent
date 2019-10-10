#ifndef	__COH_I386LIB_H__
#define	__COH_I386LIB_H__

/* prototypes from kernel internal routines in i386/lib */

/* inb.s */
unsigned char	inb	__PROTO ((int port));

/* outb.s */
void		outb	__PROTO ((int port, unsigned char c));

#endif	/* ! defined (__COH_I386LIB_H__) */
