/* $Header: /src386/bin/db/RCS/i8086c.c,v 1.2 93/03/11 07:45:47 bin Exp Locker: bin $
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 */
/*
 * A debugger.
 * Tables for the Intel 8086.
 *
 * $Log:	i8086c.c,v $
 * Revision 1.2  93/03/11  07:45:47  bin
 * Hal: post NDP update that at least
 * can st breakpoints.
 * 
 * Revision 1.1  92/06/10  14:36:44  bin
 * Initial revision
 * 
 * Revision 1.1	88/10/17  04:04:02 	src
 * Initial revision
 * 
 *
 * Revision: 386 version 92/05/01 
 * Bernard Wald, Wald Software Consulting, Germany
 *
 */

#include <stdio.h>
#include <sys/param.h>
#include <l.out.h>
#include "trace.h"
#include "i8086.h"

/*
 * Global variables.
 */
int	cacdata;
int	cacaddr;
int	cacsegn;
int	sysflag;
BIN	sin;
REG	reg;

/*
 * Breakpoint instruction
 */
BIN	bin ={
	0xCC
};

/*
 * Table containing format strings.
 */
char *formtab[4][4] ={
	"%4d",				/* 'b', 'd' */
	"%3u",				/* 'b', 'u' */
	"%04o",				/* 'b', 'o' */
	"%02x",				/* 'b', 'x' */
	"%6d",				/* 'w', 'd' */
	"%5u",				/* 'w', 'u' */
	"%07o",				/* 'w', 'o' */
	"%04x",				/* 'w', 'x' */
	"%10ld",			/* 'l', 'd' */
	"%11lu",			/* 'l', 'u' */
	"%012lo",			/* 'l', 'o' */
	"%08lx",			/* 'l', 'x' */
	"%8ld",				/* 'v', 'd' */
	"%8lu",				/* 'v', 'd' */
	"%09lo",			/* 'v', 'o' */
	"%06lx"				/* 'v', 'x' */
};

/*
 * Fault types.
 */
char *signame[] ={
	"Hangup",
	"Interrupt",
	"Quit",
	"Illegal Instruction",
	"Trace Trap",
	"Abort",
	"Emulator Trap",
	"Floating Point Exception",
	"Kill",
	"Bus Error",
	"Segmentation Violation",
	"Invalid System Call Number",
	"Write to Pipe with no Readers",
	"Alarm",
	"Termination",
	"USR1",
	"USR2",
	"Death of Child",
	"Restart",
	"Window Change",
	"Polled Event",
	"Signal 23",
	"Signal 24",
	"Signal 25",
	"Signal 26",
	"Signal 27",
	"Signal 28",
	"Signal 29",
	"Signal 30",
	"Signal 31",
};

/*
 * Names of registers.
 */
char regitab[] ={
	'b',	'p',
	'd',	'i',
	's',	'i',
	'?',	'\0',
	'e',	's',
	'c',	'x',
	'd',	'x',
	'a',	'x',
	'b',	'x',
	'd',	's',
	's',	'p',
	's',	's',
	'?',	'\0',
	'i',	'p',
	'c',	's',
	'f',	'w',
	'\0',	'\0'
};

/*
 * Table of system calls.
 */
char *sysitab[NMICALL] ={
	NULL,
	"exit",
	"fork",
	"read",
	"write",
	"open",
	"close",
	"wait",
	"creat",
	"link",
	"unlink",
	"exece",
	"chdir",
	NULL,
	"mknod",
	"chmod",
	"chown",
	"brk",
	"stat",
	"lseek",
	"getpid",
	"mount",
	"umount",
	"setuid",
	"getuid",
	"stime",
	"ptrace",
	"alarm",
	"fstat",
	"pause",
	"utime",
	NULL,
	NULL,
	"access",
	"nice",
	"ftime",
	"sync",
	"kill",
	NULL,
	NULL,
	NULL,
	"dup",
	"pipe",
	"times",
	"profil",
	"unique",
	"setgid",
	"getgid",
	"signal",
	NULL,
	NULL,
	"acct",
	NULL,
	NULL,
	"ioctl",
	NULL,
	"getegid",
	"geteuid",
	NULL,
	NULL,
	"umask",
	"chroot",
	"setpgrp",
	"getpgrp",
	"sload",
	"suload",
	"fcntl",
	"poll",
	"msgctl",
	"msgget",
	"msgrcv",
	"msgsnd",
	"alarm2",
	"tick",
};
