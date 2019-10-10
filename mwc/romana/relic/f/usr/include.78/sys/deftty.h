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

#include <sys/ascii.h>

#define DEF_SG_ISPEED	B9600
#define DEF_SG_OSPEED	B9600
#define	DEF_SG_ERASE	A_BS
#define	DEF_SG_KILL	A_NAK
#define	DEF_SG_FLAGS	CRMOD|ECHO|XTABS|CRT
#define	DEF_T_INTRC	A_ETX
#define	DEF_T_QUITC	A_FS
#define	DEF_T_STARTC	A_DC1
#define	DEF_T_STOPC	A_DC3
#define	DEF_T_EOFC	A_EOT
#define	DEF_T_BRKC	-1
#endif
