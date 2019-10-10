/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * /usr/include/sys/fd.h
 * Open file descriptors.
 */ 

#ifndef	 __SYS_FD_H__
#define	 __SYS_FD_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <sys/types.h>
#include <sys/inode.h>

#if	! __KERNEL__
# error	You must be compiling the kernel to use this header
#endif

/*
 * The FD_CLOEXEC is not actually stored in the structure above, because it
 * is an attribute specific to a single file descriptor rather than global to
 * an open file object (which may have multiple references to it).
 *
 * To capture this information, we note that pointers as returned by an
 * allocator such as kalloc () are all aligned at integer granularity at a
 * minimum, leaving us one or possibly two least-significant bits for pointer-
 * tagging.
 *
 * We create a non-arithmetic pointer type for the purpose of holding tagged
 * pointers. The operations on this type are hidden, however.
 */

typedef	struct __tagged_fd    *	tagfd_t;

/*
 * We complete this type to work around a Coherent 'cc' bug.
 */
struct __tagged_fd {
	char	_bogus;
};

/*
 * File descriptor structure.
 */

typedef struct fd {
	short	 f_flag;		/* Flags */
	short	 f_refc;		/* Reference count */
	fsize_t	 f_seek;		/* Seek pointer */
	struct	 inode *f_ip;		/* Pointer to inode */
} FD;

typedef	unsigned	fd_t;

#define	ERROR_FD	((fd_t) -1)

__EXTERN_C_BEGIN__

FD	      *	fdget		__PROTO ((fd_t _fd));
int		fdgetflags	__PROTO ((fd_t _fd));
int		fdsetflags	__PROTO ((fd_t _fd, int _flags));

fd_t		fddup		__PROTO ((fd_t _old, fd_t _base));
fd_t		fdalloc		__PROTO ((void));
int		fdinit		__PROTO ((fd_t _fd, INODE * _ip, int _mode));
fd_t		fdfinish	__PROTO ((fd_t _fd));
fd_t		fdopen		__PROTO ((INODE * _ip, int _mode));
void		fdclose		__PROTO ((fd_t _fd));

void		fdadupl		__PROTO ((void));
void		fdaclose	__PROTO ((void));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_FD_H__) */
