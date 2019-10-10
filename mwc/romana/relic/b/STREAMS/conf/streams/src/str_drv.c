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
 *	<kernel/ddi_base.h>
 *		DESTROY_UIO ()
 *		MAKE_CRED ()
 *		MAKE_UIO ()
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
#include <kernel/ddi_base.h>
#include <kernel/strmlib.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <sys/cmn_err.h>
#include <sys/file.h>
#include <sys/open.h>
#include <sys/uio.h>
#include <sys/errno.h>
#include <stddef.h>

#include <sys/confinfo.h>

/*
 * These are Coherent header files! Treat with all the caution you would
 * normally use for handling toxic waste!
 *
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

#include <sys/stat.h>

#if	__COHERENT__

#define	SETUERROR(e)	u.u_error = (e)

#else

#define	SETUERROR(e)	ASSERT (e == 0 || "Hit error" == NULL)

#endif


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

#if	0
	ret = (* funcp) (newdev, events, msec == 0, & revents, pollhead)
#else
	ret = EIO;
#endif
	SETUERROR (ret);
	return ret;
}

