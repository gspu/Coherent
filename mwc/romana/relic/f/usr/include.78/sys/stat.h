/* (-lgl
 * 	COHERENT Version 4.1.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/stat.h
 * Stat.
 */

#ifndef	__SYS_STAT_H__
#define	__SYS_STAT_H__

#include <sys/types.h>

/*
 * Structure returned by stat and fstat system calls.
 */
struct stat {
	dev_t	 st_dev;		/* Device	*/
	ino_t	 st_ino;		/* Inode number	*/
	unsigned short st_mode;		/* Mode		*/
	short	 st_nlink;		/* Link count	*/
	short	 st_uid;		/* User id	*/
	short	 st_gid;		/* Group id	*/
	dev_t	 st_rdev;		/* Real device	*/
	fsize_t	 st_size;		/* Size		*/
	time_t	 st_atime;		/* Access time	*/
	time_t	 st_mtime;		/* Modify time	*/
	time_t	 st_ctime;		/* Change time	*/
};

/*
 * Modes.
 */
#define	S_IFMT		0xF000		/* Type				*/
#define	S_IFDIR		0x4000		/* Directory			*/
#define	S_IFCHR		0x2000		/* Character special		*/
#define	S_IFPIP		0x1000		/* Pipe				*/
#define	S_IFIFO		S_IFPIP		/* Pipe				*/
#define	S_IFBLK		0x6000		/* Block special		*/
#define	S_IFREG		0x8000		/* Regular			*/
#define	S_IFSOCK	0xC000		/* Socket -- UNIMPLEMENTED	*/
#define	S_IFMPC		0x3000		/* Multiplexed character special */
#define	S_IFMPB		0x7000		/* Multiplexed block special	*/

#define	S_ISUID		0x0800		/* Set user id on execution	*/
#define	S_ISGID		0x0400		/* Set group id on execution	*/
#define	S_ISVTX		0x0200		/* Save swapped text even after use */

#define	S_IREAD		0400		/* Read permission, owner	*/
#define	S_IWRITE	0200		/* Write permission, owner	*/
#define	S_IEXEC		0100		/* Execute/search permission, owner */
#define	S_IRWXU		0700		/* RWX permission, owner	*/
#define	S_IRUSR		S_IREAD		/* Read permission, owner	*/
#define	S_IWUSR		S_IWRITE	/* Write permission, owner	*/
#define	S_IXUSR		S_IEXEC		/* Execute/search permission, owner */
#define	S_IRWXG		0070		/* RWX permission, group	*/
#define	S_IRGRP		0040		/* Read permission, group	*/
#define	S_IWGRP		0020		/* Write permission, group	*/
#define	S_IXGRP		0010		/* Execute/search permission, group */
#define	S_IRWXO		0007		/* RWX permission, other	*/
#define	S_IROTH		0004		/* Read permission, other	*/
#define	S_IWOTH		0002		/* Write permission, other	*/
#define	S_IXOTH		0001		/* Execute/search permission, other */

/* Mode test macros. */
#define	S_ISFIFO(mode)	(((mode)&S_IFMT) == S_IFIFO)
#define	S_ISCHR(mode)	(((mode)&S_IFMT) == S_IFCHR)
#define	S_ISDIR(mode)	(((mode)&S_IFMT) == S_IFDIR)
#define	S_ISBLK(mode)	(((mode)&S_IFMT) == S_IFBLK)
#define	S_ISREG(mode)	(((mode)&S_IFMT) == S_IFREG)
#define	S_ISSOCK(mode)	(((mode)&S_IFMT) == S_IFSOCK)

/*
 * Nonexistent device.
 * Must compare correctly with dev_t, which is an unsigned short.
 */
#define NODEV	((dev_t)-1)

/*
 * Functions.
 */
#define	major(dev)	((dev>>8) & 0xFF)
#define minor(dev)	(dev & 0xFF)
#define makedev(m1, m2)	((m1<<8)|m2)

#endif

/* end of sys/stat.h */
