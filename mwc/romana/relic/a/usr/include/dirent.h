/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * POSIX (IEEE 1003.1) compatible dirent.h header file.
 */

#ifndef DIRENT_H
#define DIRENT_H 1

#ifdef _I386

#include <sys/dirent.h>

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

#else
/*
 *-------------------- 286 version ----------------
 */
/*
 * Rely on COHERENT's sys/dir.h to define type DIR and structure direct with
 * fields d_ino of type ino_t and d_name of type char [].
 */
#include <sys/dir.h>

/*
 * Implement dirent as a macro, making struct dirent equivalent to COHERENT's 
 * struct direct.
 */
#define dirent direct

#endif
#endif
