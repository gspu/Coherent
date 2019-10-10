/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

#ifndef __DIRENT_H__
#define __DIRENT_H__

#ifdef _I386

struct dirent {
	long	d_ino;	/* i-node number */
	daddr_t	d_off;	/* offset in actual directory*/
	unsigned short	d_reclen;  /*record length*/
	char	d_name[1];
};

#define	MAXNAMLEN	14		/* maximum filename length */

#define	DIRBUF		2048		/* buffer size for fs-indep. dirs */
	/* must in general be larger than the filesystem buffer size */

typedef struct
	{
	int	dd_fd;			/* file descriptor */
	int	dd_loc;			/* offset in block */
	int	dd_size;		/* amount of valid data */
	char	*dd_buf;		/* -> directory block */
	}	DIR;			/* stream data from opendir() */

extern DIR		*opendir();
extern struct dirent	*readdir();
extern void		rewinddir();
extern int		closedir();
extern daddr_t		telldir();
extern void		seekdir();

#define DIRSIZ	14		/* Size of directory name */
struct direct {
	ino_t	 d_ino;			/* Inode number */
	char	 d_name[DIRSIZ];	/* Name */
};

#else
/*
 *-------------------- 286 version ----------------
 */
/*
 * Rely on COHERENT's sys/dir.h to define type DIR and structure direct with
 * fields d_ino of type ino_t and d_name of type char [].
 */
#include <sys/types.h>

#define DIRSIZ	14		/* Size of directory name */
#define	DIR	char *		/* Directory type */

/*
 * Directory entry structure.
 */
struct direct {
	ino_t	 d_ino;			/* Inode number */
	char	 d_name[DIRSIZ];	/* Name */
};

/*
 * Implement dirent as a macro, making struct dirent equivalent to COHERENT's 
 * struct direct.
 */
#define dirent direct

#endif
#endif
