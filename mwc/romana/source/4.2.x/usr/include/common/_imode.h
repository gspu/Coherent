/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__IMODE_H__
#define	__COMMON__IMODE_H__

/*
 * If sockets are real, then remove this definition; otherwise, permit use of
 * FIFO-based emulations.
 */

#define	_SOCKET_EMULATION	1


/*
 * This header defines constants used within COHERENT as the basis for
 * determining file modes. Mode information is used within the file system
 * and at user level; this set of internal definitions is used as the
 * basis for both.
 */

enum {
	_CRED_OTHER,		/* credentials match neither user nor group */
	_CRED_GROUP,		/* credentials match group */
	_CRED_OWNER		/* credentials match OWNER */
};

#define	__PERM_SHIFT_LEVEL	3U
#define	__PERM_MASK		((1 << __PERM_SHIFT_LEVEL) - 1)

#define	__PERM_EXTRACT(mode, credlvl) \
		((mode) >> (credlvl) * __PERM_SHIFT_LEVEL)

#define	__PERM_MAKE_CONST(mode, credlvl) \
		((mode) << (credlvl) * __PERM_SHIFT_LEVEL)

#define	__PERM_ALL_CONST(mode) \
		(__PERM_MAKE_CONST (mode, _CRED_OTHER) | \
		 __PERM_MAKE_CONST (mode, _CRED_GROUP) | \
		 __PERM_MAKE_CONST (mode, _CRED_OWNER))

enum {
	_PERM_EXIST =	0,
	_PERM_EXEC =	1,
	_PERM_WRITE =	2,
	_PERM_READ =	4,

	_PERM_ALL = _PERM_EXEC | _PERM_WRITE | _PERM_READ,
	__PERM_MAX = _PERM_ALL
};


enum {
	__IFPIP =	0x1000,
	__IFFIFO =	__IFPIP,
	__IFCHR =	0x2000,
	__IFDIR =	0x4000,
	__IFBLK =	0x6000,

/*
 * The required code for a socket is 0xC000; however, in a socket emulation
 * sockets are actually FIFO's.
 */

#if	-1U < 0x10000UL

/*
 * Integers are only 16 bits, so these cannot be enumeration constants.
 */
#define	__IFREG		0x8000
#define	__IFMT		0xF000
#if	_SOCKET_EMULATION
#define __IFSOCK	__IFFIFO
#else
#define	__IFSOCK	0xC000
#endif

#else	/* we have >16-bit integers */

	__IFREG =	0x8000,
	__IFMT =	0xF000,
#if	_SOCKET_EMULATION
	__IFSOCK =	__IFFIFO,
#else
	__IFSOCK =	0xC000,
#endif

#endif	/* > 16-bit integers */

	__ISUID	=	0x0800,
	__ISGID =	0x0400,
	__ISVTX =	0x0200,

	__IRUSR = __PERM_MAKE_CONST (_PERM_READ, _CRED_OWNER),
	__IWUSR = __PERM_MAKE_CONST (_PERM_WRITE, _CRED_OWNER),
	__IXUSR = __PERM_MAKE_CONST (_PERM_EXEC, _CRED_OWNER),
	__IRGRP = __PERM_MAKE_CONST (_PERM_READ, _CRED_GROUP),
	__IWGRP = __PERM_MAKE_CONST (_PERM_WRITE, _CRED_GROUP),
	__IXGRP = __PERM_MAKE_CONST (_PERM_EXEC, _CRED_GROUP),
	__IROTH = __PERM_MAKE_CONST (_PERM_READ, _CRED_OTHER),
	__IWOTH = __PERM_MAKE_CONST (_PERM_WRITE, _CRED_OTHER),
	__IXOTH = __PERM_MAKE_CONST (_PERM_EXEC, _CRED_OTHER),

	__IRWXU = __PERM_MAKE_CONST (_PERM_ALL, _CRED_OWNER),
	__IRWXG = __PERM_MAKE_CONST (_PERM_ALL, _CRED_GROUP),
	__IRWXO = __PERM_MAKE_CONST (_PERM_ALL, _CRED_OTHER),

	__IRUGO	= __PERM_ALL_CONST (_PERM_READ),
	__IWUGO = __PERM_ALL_CONST (_PERM_WRITE),
	__IXUGO = __PERM_ALL_CONST (_PERM_EXEC),

	__IRWXUGO = __PERM_ALL_CONST (_PERM_ALL),

	/*
	 * Common synonyms.
	 */

	__IREAD =	__IRUSR,
	__IWRITE =	__IWUSR,
	__IEXEC =	__IXUSR
};

#endif	/* ! defined (__COMMON__IMODE_H__) */

	
