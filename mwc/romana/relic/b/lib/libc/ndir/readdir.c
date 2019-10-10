/*
	readdir -- read next entry from a directory stream

	last edit:	25-Apr-1987	D A Gwyn
*/

#ifdef COHERENT
#include	<errno.h>
#else
#include	<sys/errno.h>
#endif

#include	<sys/types.h>
#include	"dirent.h"

extern int	getdents();		/* SVR3 system call, or emulation */

extern int	errno;

#ifndef NULL
#define	NULL	0
#endif


struct dirent *
readdir( dirp )
	register DIR		*dirp;	/* stream from opendir() */
	{
	register struct dirent	*dp;	/* -> directory data */

#if DBG
	printf("In readdir(dirp = %x, buffer= %x\n",(int) dirp,dirp->dd_buf);
	printf("fd= %d; loc= %d; size=%d.",dirp->dd_fd,dirp->dd_loc,dirp->dd_size);

#endif
	if ( dirp == NULL || dirp->dd_buf == NULL )
		{
		errno = EFAULT;
		return NULL;		/* invalid pointer */
		}

	do	{
		if ( dirp->dd_loc >= dirp->dd_size )	/* empty or obsolete */
			dirp->dd_loc = dirp->dd_size = 0;

		if ( dirp->dd_size == 0	/* need to refill buffer */
		  && (dirp->dd_size =
			getdents( dirp->dd_fd, dirp->dd_buf, (unsigned)DIRBUF )

		     ) <= 0
		   )

			return NULL;	/* EOF or error */
#if DBG
	printf("dirp is good.\n");
#endif

		dp = (struct dirent *) (&(dirp->dd_buf[dirp->dd_loc]));
#if DBG
	printf("buffer = %x, ptr = %x\n",dirp->dd_buf, dp);
#endif
		dirp->dd_loc += dp->d_reclen;
#ifdef DBG
	printf("readdir: dir. buffer loc= %d\n",dirp->dd_loc);
	printf("\tinode = %D\n",dp->d_ino);
#endif
		}
	while ( dp->d_ino == 0L );	/* don't rely on getdents() */

#if DBG
printf ("readdir: name: %s; inode %D; offset %D; reclen %d\n",dp->d_name,
	dp->d_ino, dp->d_off, dp->d_reclen);
#endif
	return dp;
	}








