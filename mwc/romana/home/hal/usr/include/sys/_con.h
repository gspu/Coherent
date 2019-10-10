/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__SYS_CON_H__
#define	__SYS_CON_H__

#include <common/feature.h>
#include <common/ccompat.h>
#include <common/gregset.h>
#include <kernel/_buf.h>
#include <common/__cred.h>
#include <common/_io.h>
#include <common/_uid.h>
#include <sys/ksynch.h>

#if	! _KERNEL
# error	You must be compiling the kernel to use this header
#endif

/*
 * Device driver table.
 */

typedef struct drv {
	struct con    *	d_conp;		/* Pointer to configuration */
	int		d_time;		/* Timeout is active */
} DRV;


/*
 * Declare at file scope as a structure tag to avoid scope problems in
 * parameter lists.
 */

struct inode;

/*
 * Driver interface entry.
 *
 * The open routine below gets some parameters that drivers should not need
 * to spend time ignoring; so, we define several types for the open routine,
 * and internally cast where needed. This trick is silly and non-portable,
 * but we need to do it until the filesystem layering comes right.
 */

typedef	void (*	driver_open_t)	__PROTO ((o_dev_t _dev, int _mode,
					  int _flags, __cred_t * _credp));
typedef void (*	kernel_open_t)	__PROTO ((o_dev_t _dev, int _mode,
					  int _flags, __cred_t * _credp,
					  struct inode ** inodepp));
typedef	void (*	driver_close_t)	__PROTO ((o_dev_t _dev, int _mode,
					  int _flags, __cred_t * _credp));
typedef	void (*	kernel_close_t)	__PROTO ((o_dev_t _dev, int _mode,
					  int _flags, __cred_t * _credp,
					  __VOID__ * _private));
typedef	void (*	driver_read_t)	__PROTO ((o_dev_t _dev, IO * _iop,
					  __cred_t * _credp));
typedef	void (*	kernel_read_t)	__PROTO ((o_dev_t _dev, IO * _iop,
					  __cred_t * _credp,
					  __VOID__ * _private));
typedef	void (*	driver_write_t)	__PROTO ((o_dev_t _dev, IO * _iop,
					  __cred_t * _credp));
typedef	void (*	kernel_write_t)	__PROTO ((o_dev_t _dev, IO * _iop,
					  __cred_t * _credp,
					  __VOID__ * _private));
typedef	void (*	driver_ioctl_t)	__PROTO ((o_dev_t _dev, int _cmd,
					  __VOID__ * _arg, int _mode,
					  __cred_t * _credp, int * _rvalp));
typedef	void (*	kernel_ioctl_t)	__PROTO ((o_dev_t _dev, int _cmd,
					  __VOID__ * _arg, int _mode,
					  __cred_t * _credp, int * _rvalp,
					  __VOID__ * _private));
typedef	int  (*	driver_poll_t)	__PROTO ((o_dev_t _dev, int _events,
					  int _msec));
typedef	int  (*	kernel_poll_t)	__PROTO ((o_dev_t _dev, int _events,
					  int _msec, __VOID__ * _private));

typedef struct con {
	int	c_flag;			/* Flags */
	int	c_mind;			/* Major index */

	driver_open_t	c_open;
	driver_close_t	c_close;
	void	     (*	c_block)	__PROTO ((buf_t * _bp));
	driver_read_t	c_read;
	driver_write_t	c_write;
	driver_ioctl_t	c_ioctl;

	void	     (*	c_power)	__PROTO ((o_dev_t _dev));
	void	     (*	c_timer)	__PROTO ((o_dev_t _dev));
	void	     (*	c_load)		__PROTO ((void));
	void	     (*	c_uload)	__PROTO ((void));

	driver_poll_t	c_poll;
} CON;


/*
 * The historical way of indicating an unsupported entry point was to use a
 * symbol called "nulldev" that pointed at a function that simply did nothing.
 * Some drivers declared their own, some didn't.  An additional symbol called
 * "nonedev" was for driver entry points that gave errors.
 *
 * This style of operation does not conform to contemporary C usage.
 * Changing it would take more time than it is worth, but driver
 * developers should take heed: use NULL to fill in driver entry points or
 * explicitly do something else sensible or not as the mood takes you,
 * but you should not use nonedev or nulldev.  A NULL means "no nothing,
 * quietly" for open and close, and gives an error for everything else.
 */

/*
 * Flags.
 */

#define	DFBLK	0x01			/* Block device */
#define	DFCHR	0x02			/* Character device */
#define DFTAP	0x04			/* Tape */
#define	DFPOL	0x08			/* Pollable device */

/*
 * Global variables.
 */

extern	int	drvn;			/* Number of entries in table */
extern	DRV	drvl[];			/* Driver table */


/*
 * This seems like the easiest place to define the hooks into STREAMS,
 * the calls to which have been inserted into various key places.
 */

__EXTERN_C_BEGIN__

void		CHECK_TIMEOUT		__PROTO ((void));

struct inode  *	dopen		__PROTO ((o_dev_t _dev, int _mode, int _flags,
					  struct inode * inodep));
int		dclose		__PROTO ((o_dev_t _dev, int _mode,
					  int flags, __VOID__ * _private));
void		dblock		__PROTO ((o_dev_t _dev, buf_t * _bp));

int		dread		__PROTO ((o_dev_t _dev, IO * _iop,
					  __VOID__ * _private));
int		dwrite		__PROTO ((o_dev_t _dev, IO * _iop,
					  __VOID__ * _private));
int		dioctl		__PROTO ((o_dev_t _dev, int _cmd,
					  __VOID__ * _arg, int _mode,
					  __VOID__ * _private,
					  gregset_t * _regsetp));
int		dpoll		__PROTO ((o_dev_t _dev, int _events,
					  int _msec, __VOID__ * _private));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_CON_H__) */
