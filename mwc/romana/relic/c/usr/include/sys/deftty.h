/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Coherent - default tty settings.
 *	used by sys/drv/tty.c, src/cmd0/getty.c, and src/cmd0/login.c
 *	to initialize terminal characteristics.
 */
#ifndef DEFTTY_H
#define DEFTTY_H
#include <sys/chars.h>

#define DEF_SG_ISPEED	B9600
#define DEF_SG_OSPEED	B9600
#define	DEF_SG_ERASE	BS
#define	DEF_SG_KILL	CTRLU
#define	DEF_SG_FLAGS	EVENP|ODDP|CRMOD|ECHO|XTABS|CRT
#define	DEF_T_INTRC	CTRLC
#define	DEF_T_QUITC	FS
#define	DEF_T_STARTC	CTRLQ
#define	DEF_T_STOPC	CTRLS
#define	DEF_T_EOFC	CTRLD
#define	DEF_T_BRKC	-1
#endif
