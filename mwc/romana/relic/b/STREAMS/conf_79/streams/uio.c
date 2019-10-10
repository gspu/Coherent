#define	_DDI_DKI	1
#define	_SYSV3		1

/*
 * Implementations of the functions which manipulate the uio(4) structure
 * under the DDI/DKI. This function runs in _SYSV3 mode to interface with
 * the iBCS2 Coherent kernel.
 */
/*
 *-IMPORTS:
 *	<common/ccompat.h>
 *		__CONST__
 *		__EXTERN_C_BEGIN__
 *		__EXTERN_C_END__
 *		__USE_PROTO__
 *		__ARGS ()
 *		__PROTO ()
 *	<sys/debug.h>
 *		ASSERT ()
 *	<sys/types.h>
 *		_VOID
 *		size_t
 *	<sys/errno.h>
 *		EFAULT
 *	<limits.h>
 *		CHAR_BIT
 *		UCHAR_MAX
 *	<string.h>
 *		memcpy ()
 */

#include <common/ccompat.h>
#include <sys/debug.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <limits.h>
#include <string.h>

#include <sys/uio.h>

#if	__COHERENT__

#include <sys/coherent.h>


/*
 * Routines to perform transfers between the user and kernel address spaces.
 */

__EXTERN_C_BEGIN__

int		kucopy		__PROTO ((__CONST__ caddr_t ker, caddr_t usr,
					  size_t n));
int		ukcopy		__PROTO ((__CONST__ caddr_t usr, caddr_t ker,
					  size_t n));
void		getubd		__PROTO ((__CONST__ caddr_t usr));
void		putubd		__PROTO ((caddr_t usr, int ch));

__EXTERN_C_END__

#define	TOUSER(usr,ker,n)	kucopy ((caddr_t) ker, (caddr_t) usr, n)
#define	FROMUSER(ker,usr,n)	ukcopy ((caddr_t) usr, (caddr_t) ker, n)

#define	PUTBYTE(c,usr)		(putubd (usr, c), u.u_error)
#define	GETBYTE(usr)		((unsigned char) getubd (usr) - \
					(u.u_error > 0 ? UCHAR_MAX : 0))

#else	/* if ! defined (COHERENT) */


#define	TOUSER(usr,ker,n)	(memcpy ((usr), (ker), (n)), (n))
#define	FROMUSER(ker,usr,n)	(memcpy ((ker), (usr), (n)), (n))

#define	PUTBYTE(c,usr)		(* (usr) = (c), 0)
#define	GETBYTE(usr)		(* (unsigned char *) (usr))


#endif


/*
 * For some reason the System V DDI/DKI uses "longs" and "ints" in many cases
 * where a size_t is far more appropriate. We will only work for cases where
 * the request does not exceed the half range of a size_t.
 */

#define	SIZE_T_MAX	((1UL << (sizeof (size_t) * CHAR_BIT - 1)) - 1)


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	bcopy		Copy data between address locations in the kernel.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *
 *	void bcopy (caddr_t from, caddr_t to, size_t bcount);
 *
 *-ARGUMENTS:
 *	from		Source address from which the copy is made.
 *
 *	to		Destination address to which the copy is made.
 *
 *	bcount		Number of bytes to be copied.
 *
 *-DESCRIPTION:
 *	bcopy () copies "bcount" bytes from one kernel address to another. It
 *	chooses the best algorithm based on address alignment and number of
 *	bytes to copy. If the input and output addresses over, the function
 *	executes, but the results are undefined.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	The source and destination address ranges must both be within the
 *	kernel address space and must be memory resident. No range checking is
 *	done. Since there is no mechanism by which drivers that conform to the
 *	rules of the DDI/DKI can obtain and use a kernel address which is not
 *	memory resident (an address which is paged out), DDI/DKI conforming
 *	drivers can assume that any address to which they have access is
 *	memory resident and therefore a valid argument to bcopy (). Addresses
 *	within user space are not valid arguments, and specifying such an
 *	address may cause the driver to corrupt the system in an unpredictable
 *	way. For copying between kernel and user space, drivers must use an
 *	appropriate function defined for that purpose (for example, copyin (),
 *	copyout (), uiomove (), ureadc (), or uwritec ()).
 *
 *-SEE ALSO:
 *	bzero (), copyin (), copyout (), uiomove (), ureadc (), uwritec ()
 */

#if	__USE_PROTO__
void (bcopy) (__CONST__ _VOID * from, _VOID * to, size_t bcount)
#else
void
bcopy __ARGS ((from, to, bcount))
__CONST__ _VOID
	      * from;
_VOID	      *	to;
size_t		bcount;
#endif
{
	ASSERT (to != NULL);

	(void) memcpy (to, from, bcount);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	bzero		Clear memory for a given number of bytes.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *
 *	void bzero (caddr_t addr, size_t bytes);
 *
 *-ARGUMENTS:
 *	addr		Starting virtual address of memory to be cleared.
 *
 *	bytes		Number of bytes to clear.
 *
 *-DESCRIPTION:
 *	The bzero () function clears a contiguous portion of memory by filling
 *	the memory with zeroes. It chooses the best algorithm based on address
 *	alignment and number of bytes to clear.
 *
 *-RETURN VALUE:
 *	None.
 *
 *-LEVEL:
 *	Base or interrupt.
 *
 *-NOTES:
 *	Does not sleep.
 *
 *	Driver-defined basic locks, read/write locks, and sleep locks may be
 *	held across calls to this function.
 *
 *	There are no alignment restrictions on "addr", and no length
 *	restrictions on "bytes", other than the address range specified must
 *	be within the kernel address space and must be memory resident. No
 *	range checking is done. Since there is no mechanism by which drivers
 *	that conform to the rules of the DDI/DKI can obtain and use a kernel
 *	address that is not memory resident (an address that is paged out),
 *	DDI/DKI conforming drivers can assume that any address to which they
 *	have access is memory resident and therefore a valid argument to
 *	bzero (). An address within user space is not a valid argument to
 *	bzero (), and specifying such an address may cause the driver to
 *	corrupt the system in an unpredictable way.
 *
 *-SEE ALSO:
 *	bcopy (), kmem_zalloc ()
 */

#if	__USE_PROTO__
void (bzero) (_VOID * addr, size_t bytes)
#else
void
bzero __ARGS ((addr, bytes))
_VOID	      *	addr;
size_t		bytes;
#endif
{
	ASSERT (addr != NULL);

	(void) memset (addr, 0, bytes);
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	copyin		Copy data from a user buffer to a driver buffer.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *
 *	int copyin (caddr_t userbuf, caddr_t driverbuf, size_t count);
 *
 *-ARGUMENTS:
 *	userbuf		User source address from which copy is made.
 *
 *	driverbuf	Driver destination address to which copy is made.
 *
 *	count		Number of bytes to copy.
 *
 *-DESCRIPTION:
 *	copyin () copies "count" bytes of data from the user virtual address
 *	specified by "userbuf" to the kernel virtual address specified by
 *	"driverbuf". The driver must ensure that adequate space is allocated
 *	for the destination address.
 *
 *	copyin () chooses the best algorithm based on address alignment and
 *	number of bytes to copy. Although the source and destination addresses
 *	are not required to be word aligned, word aligned addresses may result
 *	in a more efficient copy.
 *
 *-RETURN VALUE:
 *	If the copy is successful, 0 is returned. Otherwise, -1 is returned to
 *	indicate the specified user address range is invalid.
 *
 *-LEVEL:
 *	Base only.
 *
 *-NOTES:
 *	May sleep.
 *
 *	Drivers usually convert a return value of -1 to an EFAULT error.
 *
 *	Driver-defined basic locks and read/write locks may not be held across
 *	calls to this function.
 *
 *	Driver-defined sleep locks may be held across calls to this function.
 *
 *	When holding sleep locks across calls to this function, drivers must
 *	be careful to avoid creating a deadlock. During the data transfer,
 *	page fault resolution might result in another I/O to the same device.
 *	For example, this could occur if the driver controls the disk drive
 *	used as the swap device.
 *
 *	The driver destination buffer must be completely within the kernel
 *	address space, or the system can panic.
 *
 *-SEE ALSO:
 *	copyout (), uiomove (), ureadc (), uwritec ()
 */

#if	__USE_PROTO__
int (copyin) (_VOID * userbuf, _VOID * driverbuf, size_t count)
#else
int
copyin __ARGS ((userbuf, driverbuf, count))
_VOID	      *	userbuf;
_VOID	      *	driverbuf;
size_t		count;
#endif
{
	ASSERT (driverbuf != 0);

	return FROMUSER (driverbuf, userbuf, count) != count ? -1 : 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	copyout		Copy data from a driver buffer to a user buffer.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *
 *	int copyout (caddr_t driverbuf, caddr_t userbuf, size_t count);
 *
 *-ARGUMENTS:
 *	driverbuf	Driver source address from which copy is made.
 *
 *	userbuf		User destination address to which copy is made.
 *
 *	count		Number of bytes to copy.
 *
 *-DESCRIPTION:
 *	copyout () copies "count" bytes of data from the kernel virtual
 *	address specified by "driverbuf" to the user virtual address specified
 *	by "userbuf".
 *
 *	copyout () chooses the best algorithm based on address alignment and
 *	number of bytes to copy. Although the source and destination addresses
 *	are not required to be word aligned, word aligned addresses may result
 *	in a more efficient copy.
 *
 *-RETURN VALUE:
 *	If the copy is successful, 0 is returned. Otherwise, -1 is returned to
 *	indicate that the specified user address range is not valid.
 *
 *-LEVEL:
 *	Base only.
 *
 *-NOTES:
 *	May sleep.
 *
 *	Drivers usually convert a return value of -1 into an EFAULT error.
 *
 *	Driver-defined basic locks and read/write locks may not be held across
 *	calls to this function.
 *
 *	Driver-defined sleep locks may be held across calls to this function.
 *
 *	When holding sleep locks across calls to this function, drivers must
 *	be careful to avoid creating a deadlock. During the data transfer,
 *	page fault resolution may result in another I/O to the same device.
 *	For example, this could occur if the driver controls the disk drive
 *	used as the swap device.
 *
 *	The driver source buffer must be completely within the kernel address
 *	space, or the system can panic.
 *
 *-SEE ALSO:
 *	copyin (), uiomove (), ureadc (), uwritec ()
 */

#if	__USE_PROTO__
int (copyout) (_VOID * driverbuf, _VOID * userbuf, size_t count)
#else
int
copyout __ARGS ((driverbuf, userbuf, count))
_VOID	      *	driverbuf;
_VOID	      *	userbuf;
size_t		count;
#endif
{
	ASSERT (driverbuf != NULL);

	return TOUSER (userbuf, driverbuf, count) != count ? -1 : 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	uiomove		Copy data using uio structure.
 *
 *-SYNOPSIS:
 *	#include <sys/types.h>
 *	#include <sys/uio.h>
 *
 *	int uiomove (caddr_t addr, long nbytes, uio_rw_t rwflag, uio_t * uiop);
 *
 *-ARGUMENTS:
 *	addr		Source/destination kernel address of the copy.
 *
 *	nbytes		Number of bytes to copy.
 *
 *	rwflag		Flag indicating read or write operation. Possible
 *			values are UIO_READ and UIO_WRITE.
 *
 *	uiop		Pointer to the "uio" structure for the copy.
 *
 *-DESCRIPTION:
 *	The uiomove () function copies "nbytes" of data between the kernel
 *	address "addr" and the space defined by the "uio" structure pointed to
 *	by "uiop". If "rwflg" is "UIO_READ", the data is copied from "addr" to
 *	the space described by the "uio" structure. If "rwflag" is "UIO_WRITE",
 *	the data is copied from the space described by the "uio" structure to
 *	"addr".
 *
 *	The "uio_segflg" member of the "uio" structure specifies the type of
 *	space described by the "uio" structure. If "uio_segflg" is set to
 *	"UIO_SYSSPACE" the "uio" structure describes a portion of the kernel
 *	address space. If "uio_segflg" is set to "UIO_USERSPACE" the "uio"
 *	structure describes a portion of the user address space.
 *
 *	If the copy is successful, uiomove () updates the appropriate members
 *	of the "uio" and "iovec" structures to reflect the copy ("uio_offset"
 *	and "iov_base" and increased by "nbytes" and "uio_resid" and "iov_len"
 *	are decreased by "nbytes").
 *
 *-RETURN VALUE:
 *	uiomove () returns 0 on success or an error number on failure. If a
 *	partial transfer occurs, the "uio" structure is updated to indicate
 *	the amount not transferred and an error is returned.
 *
 *-LEVEL:
 *	Base only if "uio_segflg" is set to "UIO_USERSPACE". Base or interrupt
 *	if "uio_segflg" is set to "UIO_SYSSPACE".
 *
 *-NOTES:
 *	May sleep if "uio_segflg" is set to "UIO_USERSPACE".
 *
 *	Driver-defined basic locks and read/write locks may be held across
 *	calls to this function if "uio_segflg" is "UIO_SYSSPACE" but may not be
 *	held if "uio_segflg" is "UIO_USERSPACE".
 *
 *	Driver-defined sleep locks may be held across calls to this function
 *	regardless of the value of "uio_segflg".
 *
 *	When holding locks across calls to this function, drivers must be
 *	careful to avoid creating a deadlock. During the data transfer, page
 *	fault resolution might result in another I/O to the same device. For
 *	example, this could occur if the driver controls the disk drive used
 *	as the swap device.
 *
 *	If "addr" specifies an address in user space, or if the value of
 *	"uio_segflg" is not consistent with the type of address space
 *	described by the "uio" structure, the system can panic.
 *
 *-SEE ALSO:
 *	bcopy (), copyin (), copyout (), ureadc (), uwritec (), iovec, uio
 */

#if	__USE_PROTO__
int (uiomove) (caddr_t addr, long nbytes, uio_rw_t rwflag, uio_t * uiop)
#else
int
uiomove __ARGS ((addr, nbytes, rwflag, uiop))
caddr_t		addr;
long		nbytes;
uio_rw_t	rwflag;
uio_t	      *	uiop;
#endif
{
	iovec_t	      *	scan;
	iovec_t	      *	end;

	ASSERT (addr != NULL);
	ASSERT (uiop != NULL);
	ASSERT (nbytes > 0);
	ASSERT (nbytes <= SIZE_T_MAX);
	ASSERT (uiop->uio_resid <= SIZE_T_MAX);
	ASSERT (rwflag == UIO_READ || rwflag == UIO_WRITE);
	ASSERT (uiop->uio_segflg == UIO_SYSSPACE ||
		uiop->uio_segflg == UIO_USERSPACE);
	ASSERT (uiop->uio_resid > 0);

	for (scan = uiop->uio_iov, end = scan + uiop->uio_iovcnt ;
	     nbytes > 0 ; scan ++) {
		size_t		size;


		/*
		 * Now we are going to move a single block of the scatter/
		 * gather request. We assume that scan->iov_len is
		 * less than or equal to uiop->uio_resid!
		 */

		ASSERT (scan < end);
		ASSERT (scan->iov_len > 0);
		ASSERT (scan->iov_len <= uiop->uio_resid);

		if (nbytes < (size = (size_t) uiop->uio_iov->iov_len))
			size = (size_t) nbytes;

		if (size == 0)
			continue;


		/*
		 * Transfer a segment as indicated.
		 */

		if (rwflag == UIO_READ) {

			if (uiop->uio_segflg == UIO_USERSPACE) {
				/*
				 * Transfers involving user memory may sleep
				 * due to paging!
				 */

				if (TOUSER (scan->iov_base, addr,
					    size) != size)
					break;
			} else
				memcpy (scan->iov_base, addr, size);
		} else {

			if (uiop->uio_segflg == UIO_USERSPACE) {
				/*
				 * Transfers involving user memory may sleep
				 * due to paging!
				 */


				if (FROMUSER (addr, scan->iov_base,
					      size) != size)
					break;
			} else
				memcpy (addr, scan->iov_base, size);
		}

		addr += size;
		nbytes -= size;

		uiop->uio_resid -= size;
		uiop->uio_offset += size;

		scan->iov_len -= size;
		scan->iov_base += size;
	}

	return nbytes == 0 ? 0 : EFAULT;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	ureadc		Copy a character to space described by a "uio"
 *			structure.
 *
 *-SYNOPSIS:
 *	#include <sys/uio.h>
 *
 *	int ureadc (int c, uio_t * uiop);
 *
 *-ARGUMENTS:
 *	c		The character to be copied.
 *
 *	uiop		Pointer to the "uio" structure.
 *
 *-DESCRIPTION:
 *	ureadc () copies the character "c" into the space described by the
 *	"uio" structure pointed to by "uiop".
 *
 *	The "uio_segflg" member of the "uio" structure specifies the type of
 *	space to which the copy is made. If "uio_segflg" is set to
 *	"UIO_SYSSPACE" the character is copied to a kernel address. If
 *	"uio_segflg" is set to "UIO_USERSPACE" the character is copied to a
 *	user address.
 *
 *	If the character is successfully copied, ureadc () updates the
 *	appropriate members of the "uio" and "iovec" structures to reflect
 *	the copy ("uio_offset" and "iov_base" are incremented and "uio_resid"
 *	and "iov_len" are decremented).
 *
 *-RETURN VALUE:
 *	ureadc () returns 0 on success or an error number on failure.
 *
 *-LEVEL:
 *	Base only if "uio_segflg" is set to "UIO_USERSPACE". Base or interrupt
 *	if "uio_segflg" is set to "UIO_SYSSPACE".
 *
 *-NOTES:
 *	May sleep if "uio_segflg" is set to "UIO_USERSPACE".
 *
 *	Driver-defined basic locks and read/write locks may be held across
 *	calls to this function if "uio_segflg" is set to "UIO_SYSSPACE" but
 *	may not be held if "uio_segflg" is set to "UIO_USERSPACE".
 *
 *	Driver-defined sleep locks may be held across calls to this function
 *	regardless of the value of "uio_segflg".
 *
 *	When holding locks across calls to this function, drivers must be
 *	careful to avoid creating a deadlock. During the data transfer, page
 *	fault resolution might result in another I/O to the same device. For
 *	example, this could occur if the driver controls the disk drive used
 *	as the swap device.
 *
 *-SEE ALSO:
 *	uiomove (), uwritec (), iovec, uio
 */

#if	__USE_PROTO__
int (ureadc) (int c, uio_t * uiop)
#else
int
ureadc __ARGS ((c, uiop))
int		c;
uio_t	      *	uiop;
#endif
{
	iovec_t	      *	scan;


	ASSERT (uiop != NULL);
	ASSERT (uiop->uio_segflg == UIO_SYSSPACE ||
		uiop->uio_segflg == UIO_USERSPACE);
	ASSERT (uiop->uio_segflg == UIO_SYSSPACE ||
		uiop->uio_segflg == UIO_USERSPACE);
	ASSERT (uiop->uio_resid > 0);

	/*
	 * What to do if the structure is empty? As indicated above, I
	 * consider that an error worthy of a kernel panic. At a minimum we
	 * should make it a recoverable error as the code below does.
	 */

	if (uiop->uio_resid <= 0)
		return EFAULT;


	/*
	 * For now, we loop over any initial empty segments. It is not clear
	 * to me whether this code can alter "uio_iov" or not, although we
	 * definitely are permitted to maintain a separate entry in the
	 * structure to the same effect.
	 */

	for (scan = uiop->uio_iov ; scan->iov_len == 0 ; scan ++)
		;

	ASSERT (scan < uiop->uio_iov + uiop->uio_iovcnt);

	/*
	 * Now, write a single character to the space indicated.
	 */

	if (uiop->uio_segflg == UIO_USERSPACE) {

		if (PUTBYTE (c, scan->iov_base) != 0)
			return EFAULT;

		scan->iov_base ++;
	} else
		* scan->iov_base ++ = c;

	scan->iov_len ++;

	uiop->uio_resid --;
	uiop->uio_offset ++;

	return 0;
}


/*
 *-STATUS:
 *	DDI/DKI
 *
 *-NAME:
 *	uwritec		Return a character from space described by a "uio"
 *			structure.
 *
 *-SYNOPSIS:
 *	#include <sys/uio.h>
 *
 *	int uwritec (uio_t * uiop);
 *
 *-ARGUMENTS:
 *	uiop		Pointer to the "uio" structure.
 *
 *-DESCRIPTION:
 *	uwritec () copies a character from the space described by the "uio"
 *	structure pointed to by "uiop" and returns the character to the
 *	caller.
 *
 *	The "uio_segflg" member of the "uio" structure specifies the type of
 *	space from which the copy is made. If "uio_segflg" is set to
 *	"UIO_SYSSPACE" the character is copied from a kernel address. If
 *	"uio_segflg" is set to "UIO_USERSPACE" the character is copied from a
 *	user address.
 *
 *	If the character is successfully copied, uwritec () updates the
 *	appropriate members of the "uio" and "iovec" structures to reflect the
 *	copy ("uio_offset" and "iov_base" are incremented and "uio_resid" and
 *	"iov_len" are decremented).
 *
 *-RETURN VALUE:
 *	If successful, uwritec () returns the character. -1 is returned if the
 *	space described by the "uio" structure is empty or if there is an
 *	error.
 *
 *-LEVEL:
 *	Base only if "uio_segflg" is set to "UIO_USERSPACE". Base or interrupt
 *	if "uio_segflg" is set to "UIO_SYSSPACE".
 *
 *-NOTES:
 *	May sleep if "uio_segflg" is set to "UIO_USERSPACE".
 *	Driver-defined basic locks and read/write locks may be held across
 *	calls to this function if "uio_segflg" is "UIO_SYSSPACE" but may not
 *	be held if "uio_segflg" is "UIO_USERSPACE".
 *
 *	Driver-defined sleep locks may be held across calls to this function
 *	regardless of the value of "uio_segflg".
 *
 *	When holding locks across calls to this function, drivers must be
 *	careful to avoid creating a deadlock. During the data transfer, page
 *	fault resolution might result in another I/O to the same device. For
 *	example, this could occur if the driver controls the disk drive used
 *	as the swap device.
 *
 *-SEE ALSO:
 *	uiomove (), ureadc (), iovec, uio
 */

#if	__USE_PROTO__
int (uwritec) (uio_t * uiop)
#else
int
uwritec __ARGS ((uiop))
uio_t	      *	uiop;
#endif
{
	iovec_t	      *	scan;
	int		c;


	ASSERT (uiop != NULL);
	ASSERT (uiop->uio_segflg == UIO_SYSSPACE ||
		uiop->uio_segflg == UIO_USERSPACE);
	ASSERT (uiop->uio_segflg == UIO_SYSSPACE ||
		uiop->uio_segflg == UIO_USERSPACE);

	/*
	 * What to do if the structure is empty? Unlike ureadc (), we are
	 * allowed to return -1.
	 */

	if (uiop->uio_resid <= 0)
		return -1;


	/*
	 * For now, we loop over any initial empty segments. It is not clear
	 * to me whether this code can alter "uio_iov" or not, although we
	 * definitely are permitted to maintain a separate entry in the
	 * structure to the same effect.
	 */

	for (scan = uiop->uio_iov ; scan->iov_len == 0 ; scan ++)
		;

	ASSERT (scan < uiop->uio_iov + uiop->uio_iovcnt);

	/*
	 * Now, write a single character to the space indicated.
	 */

	if (uiop->uio_segflg == UIO_USERSPACE) {

		if ((c = GETBYTE (scan->iov_base)) < 0)
			return -1;

		scan->iov_base ++;
	} else
		* scan->iov_base ++ = c;

	scan->iov_len --;

	uiop->uio_resid --;
	uiop->uio_offset ++;

	return 0;
}
