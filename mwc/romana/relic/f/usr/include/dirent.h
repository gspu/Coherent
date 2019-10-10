/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef __DIRENT_H__
#define __DIRENT_H__

#include <common/feature.h>
#include <common/ccompat.h>

#include <common/_uid.h>
#include <common/_daddr.h>
#include <common/__off.h>

#if	_I386

struct dirent {
	n_ino_t		__NON_POSIX (d_ino);	/* i-node number */
	__daddr_t	__NON_POSIX (d_off);	/* offset in actual directory*/
	unsigned short	__NON_POSIX (d_reclen);	/* record length */
	char		d_name [1];
};

typedef struct {
	int		__NON_POSIX (dd_fd);	/* file descriptor */
	int		__NON_POSIX (dd_loc);	/* offset in block */
	int		__NON_POSIX (dd_size);	/* amount of valid data */
	char	      *	__NON_POSIX (dd_buf);	/* -> directory block */
} DIR;			/* stream data from opendir() */


#if	! _POSIX_SOURCE

#define	DIRBUF		2048		/* buffer size for fs-indep. dirs */
	/* must in general be larger than the filesystem buffer size */

#define	MAXNAMLEN	14		/* maximum filename length */

#endif	/* ! _POSIX_SOURCE */

#else	/* if ! _I386 */

#if	! DIRSIZ
# define	DIRSIZ		14	/* Size of directory name */
#endif

struct dirent {
	ino_t	 d_ino;			/* Inode number */
	char	 d_name[DIRSIZ];	/* Name */
};

#endif	/* ! _I386 */


__EXTERN_C_BEGIN__

DIR	      *	opendir		__PROTO ((__CONST__ char * _dirname));
struct dirent *	readdir		__PROTO ((DIR * _dirp));
void		rewinddir	__PROTO ((DIR * _dirp));
int		closedir	__PROTO ((DIR * _dirp));

#if	! _POSIX_SOURCE

__off_t		telldir		__PROTO ((DIR * _dirp));
void		seekdir		__PROTO ((DIR * _dirp, __off_t _loc));

#endif

__EXTERN_C_END__


#endif	/* ! defined (__DIRENT_H__) */
