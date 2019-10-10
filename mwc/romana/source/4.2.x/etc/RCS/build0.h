head	1.1;
access
	hal
	udo
	ljg
	jrd
	nigel
	fred;
symbols
	r12:1.1;
locks; strict;
comment	@ *@;


1.1
date	93.09.01.15.45.45;	author rcsadmin;	state Exp;
branches;
next	;


desc
@@


1.1
log
@COHERENT r12
@
text
@/* build0.h */
#ifndef BUILD0_H
#define BUILD0_H

#include <common/ccompat.h>

/* Manifest constants. */
#define	NBUF		256		/* buffer size			*/
#define PATCHFILE	"/tmp/patches"
#define LDKERFILE	"/tmp/ldker"
#define SMALLKER	"/tmp/smallkrnl"
#define IDCMDFILE	"/tmp/id-cmd"
#define IDVARFILE	"/tmp/id-var"

/* Switches for get_allowed_int. */
enum {
	ga_dec = 0,		/* Decimal radix in prompts. */
	ga_hex = 1,		/* Hexadecimal radix in prompts. */
	ga_nonstrict = 0,	/* May override standard choices. */
	ga_strict = 1		/* Must adhere to listed choices. */
};

/* Flags for sys(). */
#define	S_IGNORE	1
#define	S_NONFATAL	2
#define	S_FATAL		3

/* Functions. */
void	cls();
int	exists();
void	fatal();
int	get_int();
char	*get_line();
void	idenable_dev	__PROTO((char * devName));
void	idtune_var	__PROTO((char * varName, long varValue));
int	is_dir();
void	nonfatal();
int	sys();
void	usage();
int	yes_no();

/* Globals. */
extern	int	abortmsg;
extern	char	*argv0;
extern	char	buf[NBUF];
extern	char	cmd[NBUF];
extern	char	small_cmd[NBUF];
extern	int	dflag;
extern	char	*usagemsg;
extern	int	vflag;

#endif /* BUILD0_H */
/* end of build0.h */
@
