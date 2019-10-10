/* (-lgl
 * 	COHERENT Version 4.1.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/stat.h
 * Revised Sun Jun 20 1993 for POSIX.1, prototypes	-- NB
 */

#ifndef	__SYS_STAT_H__
#define	__SYS_STAT_H__

#include <common/ccompat.h>

#if	_POSIX_SOURCE || _DDI_DKI

#include <common/__time.h>
#include <common/__off.h>
#include <common/_fsize.h>
#include <common/_uid.h>

#else

#include <common/_time.h>
#include <sys/types.h>

#endif

#include <common/_imode.h>

/*
 * Structure returned by stat () and fstat () system calls.
 */

struct stat {
	__dev_t		st_dev;		/* Device	*/
	__ino_t		st_ino;		/* Inode number	*/
	__mode_t	st_mode;	/* Mode		*/
	__nlink_t	st_nlink;	/* Link count	*/
	__uid_t		st_uid;		/* User id	*/
	__gid_t		st_gid;		/* Group id	*/
	__dev_t	 	__NON_POSIX (st_rdev);
					/* Real device	*/
	__off_t		st_size;	/* Size		*/
	__time_t	st_atime;	/* Access time	*/
	__time_t	st_mtime;	/* Modify time	*/
	__time_t	st_ctime;	/* Change time	*/
};

/*
 * Mode macros. Note that the following definitions are required to be
 * macros, but are /not/ constrained to be integral constant expressions
 * suitable for use in #if-expressions.
 */

#define	S_IFMT		__IFMT		/* Type				*/
#define	S_IFDIR		__IFDIR		/* Directory			*/
#define	S_IFCHR		__IFCHR		/* Character special		*/
#define	S_IFPIP		__IFPIP		/* Pipe				*/
#define	S_IFIFO		__IFFIFO	/* Pipe				*/
#define	S_IFBLK		__IFBLK		/* Block special		*/
#define	S_IFREG		__IFREG		/* Regular			*/

#define	S_IFSOCK	0xC000		/* Socket -- UNIMPLEMENTED	*/
#define	S_IFMPC		0x3000		/* Multiplexed character special */
#define	S_IFMPB		0x7000		/* Multiplexed block special	*/

#define	S_ISUID		__ISUID		/* Set user id on execution	*/
#define	S_ISGID		__ISGID		/* Set group id on execution	*/
#define	S_ISVTX		__ISVTX		/* Save swapped text even after use */

#define	S_IREAD		__IREAD		/* Read permission, owner	*/
#define	S_IWRITE	__IWRITE	/* Write permission, owner	*/
#define	S_IEXEC		__IEXEC		/* Execute/search permission, owner */
#define	S_IRWXU		__IRWXU		/* RWX permission, owner	*/
#define	S_IRUSR		__IRUSR		/* Read permission, owner	*/
#define	S_IWUSR		__IWUSR		/* Write permission, owner	*/
#define	S_IXUSR		__IXUSR		/* Execute/search permission, owner */
#define	S_IRWXG		__IRWXG		/* RWX permission, group	*/
#define	S_IRGRP		__IRGRP		/* Read permission, group	*/
#define	S_IWGRP		__IWGRP		/* Write permission, group	*/
#define	S_IXGRP		__IXGRP		/* Execute/search permission, group */
#define	S_IRWXO		__IRWXO		/* RWX permission, other	*/
#define	S_IROTH		__IROTH		/* Read permission, other	*/
#define	S_IWOTH		__IWOTH		/* Write permission, other	*/
#define	S_IXOTH		__IXOTH		/* Execute/search permission, other */

/* Mode test macros. */

#define	S_ISFIFO(mode)	(((mode) & S_IFMT) == S_IFIFO)
#define	S_ISCHR(mode)	(((mode) & S_IFMT) == S_IFCHR)
#define	S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)
#define	S_ISBLK(mode)	(((mode) & S_IFMT) == S_IFBLK)
#define	S_ISREG(mode)	(((mode) & S_IFMT) == S_IFREG)
#define	S_ISSOCK(mode)	(((mode) & S_IFMT) == S_IFSOCK)

__EXTERN_C_BEGIN__

int		chmod		__PROTO ((__CONST__ char * _path,
					  __mode_t _mode));
int		mkdir		__PROTO ((__CONST__ char * _path,
					  __mode_t _mode));
int		fstat		__PROTO ((int _fildes, struct stat * _buf));
int		stat		__PROTO ((__CONST__ char * _path,
					  struct stat * _buf));
__mode_t	umask		__PROTO ((__mode_t _mask));

#if 0		/* not supported */
int		mkfifo		__PROTO ((__CONST__ char * _path,
					  __mode_t _mode));
#endif

__EXTERN_C_END__


#if	! _DDI_DKI

/*
 * Nonexistent device.
 * Must compare correctly with dev_t, which is an unsigned short in the old
 * Coherent device-driver system.
 */

#ifndef	NODEV
# define NODEV		((__dev_t) -1)
#endif


/*
 * Functions.
 */
#define	major(dev)	(((dev) >> 8) & 0xFF)
#define minor(dev)	((dev) & 0xFF)
#define makedev(m1, m2)	((__dev_t) (((m1) << 8) | (m2)))

#endif	/* ! _DDI_DKI */

#endif	/* ! defined (__SYS_STAT_H__) */

/* end of sys/stat.h */
