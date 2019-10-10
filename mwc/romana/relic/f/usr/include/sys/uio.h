#ifndef	__SYS_UIO_H__
#define	__SYS_UIO_H__

/*
 * This file defines the uio_t data structure used to control a scatter/gather
 * I/O request under the System V DDI/DKI. Drivers should follow the DDI/DKI
 * guidelines with respect to this structure by not directly modifying any
 * members. The only permitted use of this structure is to initialise members
 * of a driver-created uio_t to pass to the DDI/DKI library functions.
 */
/*
 *-IMPORTS
 *	<common/ccompat.h>
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__PROTO ()
 *	<sys/types.h>
 *		caddr_t
 *		off_t
 */

#include <common/ccompat.h>
#include <sys/types.h>


typedef	struct uio	uio_t;
typedef struct iovec	iovec_t;

/*
 * Note that the definition of this structure in the DDI/DKI allows the
 * routines uiomove (), ureadc () and uwritec () license to maintain all kinds
 * of cached state about iovec's once I/O through a uio structure has been
 * initiated. For now, we don't define any such state, but note that it may
 * be added.
 */

struct uio {
	iovec_t	      *	uio_iov;	/* start of the iovec array */
	int		uio_iovcnt;	/* number of iovecs in the array */
	off_t		uio_offset;	/* offset into file/device */
	short		uio_segflg;	/* type of I/O transfer */
	short		uio_fmode;	/* file mode flags (see <sys/file.h>) */
	int		uio_resid;	/* residual count */
};

struct iovec {
	caddr_t		iov_base;	/* base address of storage area */
	int		iov_len;	/* size of the storage area in bytes */
};


/*
 * Values for "uio_segflg"
 */

enum {
	UIO_USERSPACE,
	UIO_SYSSPACE
};

typedef enum {
	UIO_READ,
	UIO_WRITE
} uio_rw_t;


__EXTERN_C_BEGIN__

int		uiomove		__PROTO ((caddr_t _addr, long _nbytes,
					  uio_rw_t _rwflag, uio_t * _uiop));
int		ureadc		__PROTO ((int _c, uio_t * _uiop));
int		uwritec		__PROTO ((uio_t * uiop));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_UIO_H__) */
