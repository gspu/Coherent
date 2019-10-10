/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Coherent - default tty settings.
 *	used by sys/drv/tty.c, src/cmd0/getty.c, and src/cmd0/login.c
 *	to initialize terminal characteristics.
 */

#ifndef __SYS_DEFTTY_H__
#define __SYS_DEFTTY_H__

#if	'A' != 65 || '@' != 64
# error	Your native character set should be ASCII
#else
# define	__CTRL(c)	((c) - '@')
#endif

#define DEF_SG_ISPEED	B9600
#define DEF_SG_OSPEED	B9600
#define	DEF_SG_ERASE	'\b'
#define	DEF_SG_KILL	__CTRL ('U')
#define	DEF_SG_FLAGS	(CRMOD | ECHO | XTABS | CRT)
#define	DEF_T_INTRC	__CTRL ('C')
#define	DEF_T_QUITC	__CTRL ('\\')
#define	DEF_T_STARTC	__CTRL ('Q')
#define	DEF_T_STOPC	__CTRL ('S')
#define	DEF_T_EOFC	__CTRL ('D')
#define	DEF_T_BRKC	-1

#endif	/* ! defined (__SYS_DEFTTY_H__) */

