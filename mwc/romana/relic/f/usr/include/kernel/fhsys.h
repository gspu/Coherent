#ifndef	__KERNEL_FHSYS_H__
#define	__KERNEL_FHSYS_H__

/*
 * This header defines data structures and prototypes for objects that
 * represent open files in an abstract manner.
 */
/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__VOID__
 *		__PROTO ()
 *	<sys/types.h>
 *		off_t
 *	<common/_time.h>
 *		time_t
 *	<common/_cred.h>
 *		cred_t
 *	<sys/uio.h>
 *		uio_t
 */

#include <common/ccompat.h>
#include <common/_time.h>
#include <common/_size.h>
#include <common/_cred.h>
#include <sys/types.h>
#include <sys/uio.h>


/*
 * Begin by setting up all the typedefs we need.
 */

typedef	struct sftab	sftab_t;
typedef	struct fhandle	fhandle_t;
typedef struct fprocs	fprocs_t;
typedef	struct fstat	fstat_t;


/*
 * Structure definitions imported by the function prototypes below need to be
 * declared with file scope.
 */

struct pollhead;
struct strbuf;
struct statvfs;


/*
 * We declare typedefs for the function pointers that the file handle system
 * use use for documentation purposes.
 */

typedef	int  (*	fh_open_t)	__PROTO ((fhandle_t * _handlep,
					  __VOID__ * _name, size_t _namelen,
					  int _oflag, cred_t * _credp));
typedef	int  (*	fh_close_t)	__PROTO ((fhandle_t * _handlep, int _oflag,
					  int _otyp, cred_t * _credp));
typedef	int  (*	fh_read_t)	__PROTO ((fhandle_t * _handlep, uio_t * _uiop,
					  cred_t * _credp));
typedef	int  (*	fh_write_t)	__PROTO ((fhandle_t * _handlep, uio_t * _uiop,
					  cred_t * _credp));
typedef	int  (*	fh_ioctl_t)	__PROTO ((fhandle_t * _handlep, int _cmd,
					  __VOID__ * _arg, int _mode,
					  cred_t * _credp, int * _rvalp));
typedef	int  (*	fh_poll_t)	__PROTO ((fhandle_t * _handlep, short _events,
					  int _anyyet, short * _reventsp,
					  struct pollhead ** _phpp));
typedef	int  (*	fh_lseek_t)	__PROTO ((fhandle_t * _handlep, off_t _pos,
					  int _whence, off_t * _rpos));
typedef	int  (*	fh_sync_t)	__PROTO ((fhandle_t * _handlep, int _flag));
typedef	int  (*	fh_getpmsg_t)	__PROTO ((fhandle_t * _handlep,
					  struct strbuf * _ctlbufp,
					  struct strbuf * _databufp,
					  int * _bandp, int * _flagsp));
typedef	int  (*	fh_putpmsg_t)	__PROTO ((fhandle_t * _handlep,
					  struct strbuf * _ctlbufp,
					  struct strbuf * _databufp,
					  int band, int flags));
typedef	int  (*	fh_fstat_t)	__PROTO ((fhandle_t * _handlep,
					  fstat_t * _statp));
typedef	int  (*	fh_statvfs_t)	__PROTO ((fhandle_t * _handlep,
					  struct statvfs * _bufp));
typedef	int  (*	fh_mmap_t)	__PROTO ((fhandle_t * _handlep, off_t _off,
					  off_t len, int _prot));
typedef int  (*	fh_attach_t)	__PROTO ((fhandle_t * _handlep,
					  __VOID__ * _detach));
typedef	int  (*	fh_detach_t)	__PROTO ((fhandle_t * _handlep));


__EXTERN_C_BEGIN__

/*
 * For dealing with file descriptors and system file table entries.
 */

fhandle_t     *	fd_get_handle	__PROTO ((int _fd));
sftab_t	      *	fd_get_sftab	__PROTO ((int _fd));
int		fd_add_sftab	__PROTO ((sftab_t * _sftabp, int * _fdp));
int		fd_can_add	__PROTO ((void));

/*
 * For dealing with file handles.
 */

fhandle_t     * fh_opening	__PROTO ((void));
fprocs_t      *	fh_procs	__PROTO ((fhandle_t * _handle));
__VOID__      *	fh_get_cookie	__PROTO ((fhandle_t * _handle));
int		fh_set_cookie	__PROTO ((fhandle_t * _handle,
					  __VOID__ * _cookie, size_t _size));

__EXTERN_C_END__


/*
 * Status information pertaining to an open file object. We define this rather
 * than passing around full "stat" structures because many of the fields have
 * no relevance to this subsystem, and that introduces unnecessary
 * complications about who fills in what field when.
 */

struct fstat {
	time_t		fh_atime;
	time_t		fh_mtime;
	time_t		fh_ctime;

	off_t		fh_size;
};


/*
 * The system described in part by this header file is not yet fully
 * operational. In particular, the configuration mechanisms are not yet
 * specified, so the following feature-test can be used to dike out code that
 * cannot yet be properly tested.
 */

#define	_NO_INSTALLABLE_FILESYSTEMS	1


#endif	/* ! defined (__KERNEL_FHSYS_H__) */
