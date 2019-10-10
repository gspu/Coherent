#define	_DDI_DKI	1
#define	_SYSV3		1

/*
 * This file contains supplementary definitions used to deal with or aid the
 * implementation of the automatically generated device configuration code
 * in the "conf.c" file.
 *
 * Note that we are in the _SYSV3 world because we touch <sys/uproc.h>
 */
/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__USE_PROTO__
 *		__ARGS ()
 *	<common/xdebug.h>
 *		__LOCAL__
 *	<sys/debug.h>
 *		ASSERT ()
 *	<sys/types.h>
 *		cred_t
 *		n_dev_t
 *		o_dev_t
 *		makedevice ()
 *	<sys/cmn_err.h>
 *		CE_PANIC
 *		cmn_err ()
 *	<sys/file.h>
 *		FREAD
 *		FWRITE
 *		FNDELAY
 *		FNONBLOCK
 *		FEXCL
 *	<sys/open.h>
 *		OTYP_BLK
 *		OTYP_CHR
 *		OTYP_LYR
 *	<sys/uio.h>
 *		iovec_t
 *		uio_t
 *	<stddef.h>
 *		NULL
 *	<sys/errno.h>
 *		ENXIO
 */

#include <common/ccompat.h>
#include <common/xdebug.h>
#include <kernel/strmlib.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <sys/cmn_err.h>
#include <sys/file.h>
#include <sys/open.h>
#include <sys/uio.h>
#include <sys/errno.h>
#include <stddef.h>

#include <kernel/confinfo.h>


/*
 * These are Coherent header files! Treat with all the caution you would
 * normally use for handling toxic waste!
 *
 *	<sys/io.h>
 *		IO
 *	<sys/stat.h>
 *		major ()
 *		minor ()
 *	<sys/uproc.h>
 *		u.u_uid
 *		u.u_gid
 *	<sys/con.h>
 *		DFBLK
 *
 * The following values which used to be in Coherent <sys/inode.h> have been
 * moved here so we can use them. <sys/inode.h> is too much of a mess.
 *
 *	<sys/file.h>
 *		IPR
 *		IPW
 *		IPNDLY
 *		IPEXCL
 *
 * Note that the actual magic u area variable 'u' is extern'ed in
 * <sys/coherent.h>.
 */

#include <sys/io.h>
#include <sys/stat.h>

#if	__COHERENT__

#define	__KERNEL__	2
#include <sys/uproc.h>
#undef	__KERNEL__

extern	UPROC	u;

#define	SETUERROR(e)	u.u_error = (e)

#else

#define	SETUERROR(e)	ASSERT (e == 0 || "Hit error" == NULL)

#endif


/*
 * This function initialises a cred_t structure from Coherent's internal data
 * structures. This function would normally be local, but it is also called
 * upon by code in "drv_ddi.c" to implement the drv_getparm () and
 * drv_setparm () utilities.
 */

#if	__USE_PROTO__
cred_t * (MAKE_CRED) (cred_t * credp)
#else
cred_t *
MAKE_CRED __ARGS ((credp))
cred_t	      *	credp;
#endif
{
	credp->cr_ref = 1;
	credp->cr_ngroups = 0;	/* no supplementary groups */

#ifdef	__MSDOS__
	credp->cr_uid = credp->cr_ruid = credp->cr_suid = 0;
	credp->cr_gid = credp->cr_rgid = credp->cr_sgid = 0;
#else
	credp->cr_uid = u.u_uid;
	credp->cr_gid = u.u_gid;
	credp->cr_ruid = u.u_ruid;
	credp->cr_rgid = u.u_rgid;
	credp->cr_suid = u.u_euid;	/* u.u_euid is saved effective uid */
	credp->cr_sgid = u.u_egid;	/* u.u_egid is saved effective gid */
#endif
	return credp;
}


/*
 * This local function is called to build a "uio_t" structure from Coherent's
 * UIO structure.
 */

#if	__USE_PROTO__
__LOCAL__ uio_t * (MAKE_UIO) (uio_t * uiop, iovec_t * iovp, int mode, IO * iop)
#else
__LOCAL__ uio_t *
MAKE_UIO __ARGS ((uiop, iovp, mode, iop))
uio_t	      *	uiop;
iovec_t	      *	iovp;
int		mode;
IO	      *	iop;
#endif
{
	uiop->uio_iov = iovp;
	uiop->uio_iovcnt = 1;
	uiop->uio_offset = iop->io_seek;
	uiop->uio_segflg = iop->io_seg == IOSYS ? UIO_SYSSPACE : UIO_USERSPACE;
	uiop->uio_fmode = mode | ((iop->io_flag & IONDLY) != 0 ? FNDELAY : 0);
	uiop->uio_resid = iop->io_ioc;

#ifdef	_I386
	iovp->iov_base = iop->io.vbase;
#else
	iovp->iov_base = iop->io_base;
#endif
	iovp->iov_len = iop->io_ioc;

	return uiop;
}


/*
 * This function is used to copy back the information from a "uio_t" structure
 * into a Coherent IO structure.
 */

#if	__USE_PROTO__
__LOCAL__ void (DESTROY_UIO) (uio_t * uiop, IO * iop)
#else
__LOCAL__ void
DESTROY_UIO __ARGS ((uiop, iop))
uio_t	      *	uiop;
IO	      *	iop;
#endif
{
	ASSERT (uiop->uio_iov != NULL);
	ASSERT (uiop->uio_iov->iov_len == uiop->uio_resid);

	iop->io_ioc = uiop->uio_resid;
	iop->io_seek = uiop->uio_offset;
#ifdef	_I386
	iop->io.vbase = uiop->uio_iov->iov_base;
#else
	iop->io_base = uiop->uio_iov->iov_base;
#endif
}


/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device open request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_open) (o_dev_t dev, int mode, int flags, ddi_open_t funcp)
#else
int
_forward_open __ARGS ((dev, mode, flags, funcp))
o_dev_t		dev;
int		mode;
int		flags;
ddi_open_t	funcp;
#endif
{
	n_dev_t		newdev = makedevice (major (dev), minor (dev));
	cred_t		cred;
	int		newmode;
	int		ret;

	newmode = 0;

	/*
	 * These are the flags documented by the Coherent driver kit. In
	 * actual fact, there are more (corresponding to the DDI/DKI flags)
	 * that are passed down from the system file table's flag entry. The
	 * fact that the others are not documented probably reflects the fact
	 * that calls to dread () either pass only IPR or IPW directly, while
	 * the existence of the other flags can only be discovered by tracing
	 * many layers of calls.
	 */

	if ((mode & IPR) != 0)
		newmode |= FREAD;
	if ((mode & IPW) != 0)
		newmode |= FWRITE;

	/*
	 * And the other flags. There is no equivalent for FNONBLOCK.
	 */

	if ((mode & IPEXCL) != 0)
		newmode |= FEXCL;
	if ((mode & IPNDLY) != 0)
		newmode |= FNDELAY;

	flags = (flags & DFBLK) != 0 ? OTYP_BLK : OTYP_CHR;

	ret = (* funcp) (& newdev, newmode, flags, MAKE_CRED (& cred));

	SETUERROR (ret);
	return ret;
}


/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device close request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_close) (o_dev_t dev, int mode, int flags, ddi_close_t funcp)
#else
int
_forward_close __ARGS ((dev, mode, flags, funcp))
o_dev_t		dev;
int		mode;
int		flags;
ddi_close_t	funcp;
#endif
{
	n_dev_t		newdev = makedevice (major (dev), minor (dev));
	cred_t		cred;
	int		newmode;
	int		ret;

	newmode = 0;

	/*
	 * These are the flags documented by the Coherent driver kit. In
	 * actual fact, there are more (corresponding to the DDI/DKI flags)
	 * that are passed down from the system file table's flag entry. The
	 * fact that the others are not documented probably reflects the fact
	 * that calls to dread () either pass only IPR or IPW directly, while
	 * the existence of the other flags can only be discovered by tracing
	 * many layers of calls.
	 */

	if ((mode & IPR) != 0)
		newmode |= FREAD;
	if ((mode & IPW) != 0)
		newmode |= FWRITE;

	/*
	 * And the other flags. There is no equivalent for FNONBLOCK.
	 */

	if ((mode & IPEXCL) != 0)
		newmode |= FEXCL;
	if ((mode & IPNDLY) != 0)
		newmode |= FNDELAY;

	flags = (flags & DFBLK) != 0 ? OTYP_BLK : OTYP_CHR;

	ret = (* funcp) (newdev, newmode, flags, MAKE_CRED (& cred));

	SETUERROR (ret);
	return ret;
}


/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device read request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_read) (o_dev_t dev, IO * iop, ddi_read_t funcp)
#else
int
_forward_read __ARGS ((dev, iop, funcp))
o_dev_t		dev;
IO	      *	iop;
ddi_read_t	funcp;
#endif
{
	n_dev_t		newdev = makedevice (major (dev), minor (dev));
	cred_t		cred;
	iovec_t		iov;
	uio_t		uio;
	int		ret;

	ret = (* funcp) (newdev, MAKE_UIO (& uio, & iov, FREAD, iop),
				 MAKE_CRED (& cred));
	DESTROY_UIO (& uio, iop);
	SETUERROR (ret);
	return ret;
}


/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device write request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_write) (o_dev_t dev, IO * iop, ddi_write_t funcp)
#else
int
_forward_write __ARGS ((dev, iop, funcp))
o_dev_t		dev;
IO	      *	iop;
ddi_write_t	funcp;
#endif
{
	n_dev_t		newdev = makedevice (major (dev), minor (dev));
	cred_t		cred;
	iovec_t		iov;
	uio_t		uio;
	int		ret;

	ret = (* funcp) (newdev, MAKE_UIO (& uio, & iov, FWRITE, iop),
				 MAKE_CRED (& cred));
	DESTROY_UIO (& uio, iop);
	SETUERROR (ret);
	return ret;
}


/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device ioctl request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_ioctl) (o_dev_t dev, int cmd, _VOID * arg, int mode,
		      ddi_ioctl_t funcp)
#else
int
_forward_ioctl __ARGS ((dev, cmd, arg, mode, funcp))
o_dev_t		dev;
int		cmd;
_VOID	      *	arg;
int		mode;
ddi_ioctl_t	funcp;
#endif
{
	n_dev_t		newdev = makedevice (major (dev), minor (dev));
	cred_t		cred;
	int		rval;
	int		newmode;
	int		ret;

	newmode = 0;

	/*
	 * These are the flags documented by the Coherent driver kit. In
	 * actual fact, there are more (corresponding to the DDI/DKI flags)
	 * that are passed down from the system file table's flag entry. The
	 * fact that the others are not documented probably reflects the fact
	 * that calls to dread () either pass only IPR or IPW directly, while
	 * the existence of the other flags can only be discovered by tracing
	 * many layers of calls.
	 */

	if ((mode & IPR) != 0)
		newmode |= FREAD;
	if ((mode & IPW) != 0)
		newmode |= FWRITE;

	/*
	 * And the other flags. There is no equivalent for FNONBLOCK.
	 */

	if ((mode & IPEXCL) != 0)
		newmode |= FEXCL;
	if ((mode & IPNDLY) != 0)
		newmode |= FNDELAY;

	ret = (* funcp) (newdev, cmd, arg, mode, MAKE_CRED (& cred), & rval);

	SETUERROR (ret);
	return rval;
}



#if 0
/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device block request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_strategy) (BUF * buf, ddi_strategy_t funcp)
#else
int
_forward_strategy __ARGS ((buf, funcp))
BUF	      *	buf;
ddi_strategy_t	funcp;
#endif
{
	cmn_err (CE_PANIC, "DDI/DKI block drivers not yet supported");
}
#endif


/*
 * This function is called upon by the machine-generated code in "conf.c" to
 * forward a Coherent device poll request to a System V device.
 */

#if	__USE_PROTO__
int (_forward_poll) (o_dev_t dev, int events, int msec, ddi_chpoll_t funcp)
#else
int
_forward_poll __ARGS ((dev, events, msec, funcp))
o_dev_t		dev;
int		events;
int		msec;
ddi_chpoll_t	funcp;
#endif
{
	int		ret;

	cmn_err (CE_PANIC, "DDI/DKI polling not yet supported");

/*	ret = (* funcp) (newdev, events, msec == 0, & revents, pollhead) */

	return 0;
}


/*
 * Forward Coh open request to actual STREAMS driver-request-processing code.
 */

#if	__USE_PROTO__
int (_streams_open) (o_dev_t dev, int mode, int __NOTUSED (flags),
		     struct streamtab * stabp)
#else
int
_streams_open __ARGS ((dev, mode, flags, stabp))
o_dev_t		dev;
int		mode;
int		flags;
struct streamtab
	      *	stabp;
#endif
{
	n_dev_t		newdev = makedevice (major (dev), minor (dev));
	n_dev_t		olddev = newdev;
	cred_t		cred;
	int		newmode;
	int		ret;

	newmode = 0;

	if ((mode & IPR) != 0)
		newmode |= FREAD;
	if ((mode & IPW) != 0)
		newmode |= FWRITE;
	if ((mode & IPEXCL) != 0)
		newmode |= FEXCL;
	if ((mode & IPNDLY) != 0)
		newmode |= FNDELAY;

	ret = STREAMS_OPEN (& newdev, stabp, newmode, MAKE_CRED (& cred));

	if (olddev != newdev)
		cmn_err (CE_PANIC, "No cloning permitted through the Coherent driver system");

	SETUERROR (ret);
	return ret;
}


/*
 * Forward Coh close request to actual STREAMS driver-request-processing code.
 */

#if	__USE_PROTO__
int (_streams_close) (o_dev_t dev, int mode, int __NOTUSED (flags),
		      struct streamtab * __NOTUSED (stabp))
#else
int
_streams_close __ARGS ((dev, mode, flags, stabp))
o_dev_t		dev;
int		mode;
int		flags;
struct streamtab
	      *	stabp;
#endif
{
	shead_t	      *	sheadp;
	cred_t		cred;
	int		newmode;
	int		ret;

	if ((sheadp = SHEAD_FIND (makedevice (major (dev), minor (dev)),
				  DEV_SLIST)) == NULL) {
		SETUERROR (ENXIO);
		return -1;
	}

	newmode = 0;

	if ((mode & IPR) != 0)
		newmode |= FREAD;
	if ((mode & IPW) != 0)
		newmode |= FWRITE;
	if ((mode & IPEXCL) != 0)
		newmode |= FEXCL;
	if ((mode & IPNDLY) != 0)
		newmode |= FNDELAY;

	ret = STREAMS_CLOSE (sheadp, newmode, MAKE_CRED (& cred));

	SETUERROR (ret);
	return ret;
}


/*
 * Forward Coh read request to actual STREAMS driver-request-processing code.
 */

#if	__USE_PROTO__
int (_streams_read) (o_dev_t dev, IO * iop,
		     struct streamtab * __NOTUSED (stabp))
#else
int
_streams_read __ARGS ((dev, iop, stabp))
o_dev_t		dev;
IO	      *	iop;
struct streamtab
	      *	stabp;
#endif
{
	iovec_t		iov;
	uio_t		uio;
	int		ret;
	shead_t	      *	sheadp;

	if ((sheadp = SHEAD_FIND (makedevice (major (dev), minor (dev)),
				  DEV_SLIST)) == NULL) {
		SETUERROR (ENXIO);
		return -1;
	}

	ret = STREAMS_READ (sheadp, MAKE_UIO (& uio, & iov, FREAD, iop));

	DESTROY_UIO (& uio, iop);
	SETUERROR (ret);
	return ret;
}


/*
 * Forward Coh write request to actual STREAMS driver-request-processing code.
 */

#if	__USE_PROTO__
int (_streams_write) (o_dev_t dev, IO * iop,
		      struct streamtab * __NOTUSED (stabp))
#else
int
_streams_write __ARGS ((dev, iop, stabp))
o_dev_t		dev;
IO	      *	iop;
struct streamtab
	      *	stabp;
#endif
{
	iovec_t		iov;
	uio_t		uio;
	int		ret;
	shead_t	      *	sheadp;

	if ((sheadp = SHEAD_FIND (makedevice (major (dev), minor (dev)),
				  DEV_SLIST)) == NULL) {
		SETUERROR (ENXIO);
		return -1;
	}

	ret = STREAMS_WRITE (sheadp, MAKE_UIO (& uio, & iov, FWRITE, iop));

	DESTROY_UIO (& uio, iop);
	SETUERROR (ret);
	return ret;
}


/*
 * Forward Coh ioctl request to actual STREAMS driver-request-processing code.
 */

#if	__USE_PROTO__
int (_streams_ioctl) (o_dev_t dev, int cmd, _VOID * arg, int mode,
		      struct streamtab * __NOTUSED (stabp))
#else
int
_streams_ioctl __ARGS ((dev, cmd, arg, mode, stabp))
o_dev_t		dev;
int		cmd;
_VOID	      *	arg;
int		mode;
struct streamtab
	      *	stabp;
#endif
{
	cred_t		cred;
	int		rval;
	int		newmode;
	int		ret;
	shead_t	      *	sheadp;

	if ((sheadp = SHEAD_FIND (makedevice (major (dev), minor (dev)),
				  DEV_SLIST)) == NULL) {
		SETUERROR (ENXIO);
		return -1;
	}

	newmode = 0;

	if ((mode & IPR) != 0)
		newmode |= FREAD;
	if ((mode & IPW) != 0)
		newmode |= FWRITE;
	if ((mode & IPEXCL) != 0)
		newmode |= FEXCL;
	if ((mode & IPNDLY) != 0)
		newmode |= FNDELAY;

	ret = STREAMS_IOCTL (sheadp, cmd, arg, newmode, MAKE_CRED (& cred),
			     & rval);

	SETUERROR (ret);
	return rval;

}


/*
 * Forward Coh poll request to actual STRAAMS driver-request-processing code.
 */

#if	__USE_PROTO__
int (_streams_chpoll) (o_dev_t dev, int events, int msec,
		       struct streamtab * stabp)
#else
int
_streams_chpoll __ARGS ((dev, events, msec, stabp))
o_dev_t		dev;
int		events;
int		msec;
struct streamtab
	      *	stabp;
#endif
{
	int		ret;

	cmn_err (CE_PANIC, "DDI/DKI polling not yet supported");

/*	ret = (* funcp) (newdev, events, msec == 0, & revents, pollhead) */
}
