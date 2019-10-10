/*
 * A debugger.
 * Tables for the Intel 8086.
 */
#include <stdio.h>
#include <sys/const.h>
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
	"Alarm clock",
	"Termination signal",
	"Restart",
	"Bad argument to system call",
	"Write on open pipe",
	"Kill",
	"Breakpoint",
	"Segmentation violation",
	"Divide error",
	"Overflow",
	"Signal 14",
	"Signal 15",
	"Signal 16",
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
 * Byte registers.
 */
char *regbtab[] ={
	"al",	"cl",	"dl",	"bl",	"ah",	"ch",	"dh",	"bh"
};

/*
 * Word registers.
 */
char *regwtab[] ={
	"ax",	"cx",	"dx",	"bx",	"sp",	"bp",	"si",	"di"
};

/*
 * Indirect mode table.
 */
char *indmtab[] ={
	"(bx)(si)",
	"(bx)(di)",
	"(bp)(si)",
	"(bp)(di)",
	"(si)",
	"(di)",
	"(bp)",
	"(bx)"
};

/*
 * Jump table.
 */
char *jumptab[] ={
	"jo",
	"jno",
	"jb",
	"jae",
	"je",
	"jne",
	"jbe",
	"ja",
	"js",
	"jns",
	"jp",
	"jnp",
	"jl",
	"jge",
	"jle",
	"jg"
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
	NULL,
	NULL,
	"sload",
	"suload",
};
