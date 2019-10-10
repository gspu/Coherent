/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_FD_H__
#define	 __SYS_FD_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/__fsize.h>

#if	! (_KERNEL || _DDI_DKI_IMPL)
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


#if	__MWC__
struct __tagged_fd {
	char		_bogus;		/* bug in MWC 'cc' */
};
#endif


/*
 * File descriptor structure.
 */

typedef	struct fd	__fd_t;

struct fd {
	short		f_flag;		/* Flags */
	short		f_refc;		/* Reference count */
	__fsize_t	f_seek;		/* Seek pointer */
	struct inode  *	f_ip;		/* Pointer to inode */
};

typedef	unsigned	fd_t;

#define	ERROR_FD	((fd_t) -1)

__EXTERN_C_BEGIN__

__fd_t	      *	fd_get		__PROTO ((fd_t _fd));
int		fd_get_flags	__PROTO ((fd_t _fd));
int		fd_set_flags	__PROTO ((fd_t _fd, int _flags));

fd_t		fd_dup		__PROTO ((fd_t _old, fd_t _fbase));
int		fd_recv		__PROTO ((fd_t fd, __fd_t * _fdp));
fd_t		fd_get_free	__PROTO ((void));
fd_t		fd_alloc	__PROTO ((void));
int		fd_init		__PROTO ((fd_t _fd, struct inode * _ip,
					  int _mode));
fd_t		fd_finish	__PROTO ((fd_t _fd));
fd_t		fd_open		__PROTO ((struct inode * _ip, int _mode));
void		fd_close	__PROTO ((fd_t _fd));

void		fd_dup_all	__PROTO ((void));
void		fd_close_all	__PROTO ((void));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_FD_H__) */
