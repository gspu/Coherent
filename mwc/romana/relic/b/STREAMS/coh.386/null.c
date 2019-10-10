/* $Header: /src386/STREAMS/coh.386/RCS/null.c,v 2.3 93/08/09 13:35:51 bin Exp Locker: bin $ */
/* (lgl-
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.37
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
/*
 * Null and memory driver.
 *  Minor device 0 is /dev/null
 *  Minor device 1 is /dev/mem, physical memory
 *  Minor device 2 is /dev/kmem, kernel data
 *  Minor device 3 is /dev/cmos
 *  Minor device 4 is /dev/boot_gift
 *  Minor device 5 is /dev/clock
 *  Minor device 6 is /dev/ps
 *  Minor device 7 is /dev/kmemhi, virtual memory 0x8000_0000-0xFFFF_FFFF
 *  Minor device 11 is /dev/idle
 *
 * $Log:	null.c,v $
 * Revision 2.3  93/08/09  13:35:51  bin
 * Kernel 82 changes
 * 
 * Revision 2.2  93/07/26  14:28:57  nigel
 * Nigel's R80
 * 
 * Revision 1.7  93/04/14  10:06:37  root
 * r75
 * 
 * Revision 1.10  93/03/02  08:16:25  bin
 * kernel 73 update
 * 
 * Revision 1.6  92/11/09  17:10:54  root
 * Just before adding vio segs.
 * 
 * Revision 1.2  92/01/06  11:59:49  hal
 * Compile with cc.mwc.
 * 
 * Revision 1.1	88/03/24  16:14:04	src
 * Initial revision
 * 
 */

/*
 * The symbol "DANGEROUS" should be undefined for a production system.
 */
#ifdef TRACER
#define NULL_IOCTL	/* Allow ioctl()s for /dev/kmem.  */
#define DANGEROUS	/* Allow dangerous ioctl()s for /dev/null.  */
#endif
#define IDLE_DEV

#include <kernel/typed.h>

#include <sys/coherent.h>
#include <sys/con.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/inode.h>
#include <sys/seg.h>
#include <sys/coh_ps.h>
#include <sys/file.h>
#if defined NULL_IOCTL || defined IDLE_DEV
   #include <sys/null.h>
#endif /* NULL_IOCTL || IDLE_DEV */


#if	TRACER
#include <sys/buf.h>
#endif

/* These are minor numbers.  */
#define DEV_NULL	0	/* /dev/null	*/
#define DEV_MEM		1	/* /dev/mem	*/
#define DEV_KMEM	2	/* /dev/kmem	*/
#define DEV_CMOS	3	/* /dev/cmos	*/
#define DEV_BOOTGIFT	4	/* /dev/bootgift  */
#define DEV_CLOCK	5	/* /dev/clock  */
#define DEV_PS		6	/* /dev/ps  */
#define DEV_KMEMHI	7	/* /dev/kmemhi  */
#define DEV_IDLE	11	/* /dev/idle    */

#define KMEMHI_BASE	0x80000000
#define PXCOPY_LIM	4096

/*
 * CMOS devices are limited by an 8 bit address.
 */
#define MAX_CMOS	255
#define CMOS_LEN	256

/*
 * The first 14 bytes of the CMOS are the clock.
 */
#define MAX_CLOCK	13
#define CLOCK_LEN	14

/*
 * These are definitions for mucking with the CMOS clock.
 */
#define SRA	10	/* Status Register A */
#define SRB	11	/* Status Register B */
#define SRC	12	/* Status Register C */
#define SRD	13	/* Status Register D */

#define UIP	0x80	/* Update In Progress bit of SRA.	*/
#define NO_UPD	0x80	/* No Update bit of SRB.		*/

/*
 * Functions for configuration.
 */
void	nlopen();
void	nlclose();
void	nlread();
void	nlwrite();
int	nlioctl();
int	nulldev();
int	nonedev();

/*
 * Configuration table.
 */
CON nlcon ={
	DFCHR,				/* Flags */
	0,				/* Major index */
	nlopen,				/* Open */
	nlclose,			/* Close */
	nulldev,			/* Block */
	nlread,				/* Read */
	nlwrite,			/* Write */
#if defined NULL_IOCTL || defined IDLE_DEV
	nlioctl,			/* Ioctl */
#else /* NULL_IOCTL || IDLE_DEV */
	nonedev,			/* Ioctl */
#endif /* NULL_IOCTL || IDLE_DEV */
	nulldev,			/* Powerfail */
	nulldev,			/* Timeout */
	nulldev,			/* Load */
	nulldev				/* Unload */
};

int lock_clock();
void unlock_clock();

/*
 * Null/memory open routine.
 */
void
nlopen(dev, mode)
dev_t dev;
int mode;
{
	switch (minor(dev)) {
#ifdef IDLE_DEV
	case DEV_IDLE:
#endif
	case DEV_PS:
		/* /dev/ps is read only */
		if (IPR != (IPR & mode)) 
			SET_U_ERROR( EACCES, "/dev/ps is read only" );
		break;
	default:
		/*
		 * For minor devices on NULL there is
		 * usually no action for open().
		 */
		break;
	}
	return;
} /* nlopen() */

/*
 * Null/memory close routine.
 */
void
nlclose(dev, mode)
dev_t dev;
int mode;
{
	/*
	 * For minor devices on NULL there is
	 * Usually no action for close().
	 */
	return;
} /* nlclose() */

/*
 * Null/memory read routine.
 */
void
nlread(dev, iop)
dev_t dev;
register IO *iop;
{
	register unsigned 	bytesRead;
	register PROC 		*pp1;		/* */
	char			psBuf[ARGSZ];	/* buffer for command line
						 * arguments for ps. */
	stMonitor		psData;		/* All process data for */
	UPROC	      		*uprc;		/* pointer to u area */
	int			ndpUseg;	/* System global address 
						 * of U segment */
	unsigned int 		seek;
	unsigned char 		read_cmos();
	extern typed_space 	boot_gift;

	switch (minor (dev)) {
	case DEV_NULL:
		/*
		 * Read nothing.
		 * Do NOT update iop->io_ioc.
		 * This way, caller knows 0 bytes were read.
		 */
		break;

	case DEV_MEM: {
		int src = iop->io_seek;
		int dest = iop->io.pbase;

		while (iop->io_ioc) {
			int numBytes = PXCOPY_LIM;
			if (numBytes > iop->io_ioc)
				numBytes = iop->io_ioc;

			bytesRead = pxcopy (src, dest, numBytes,
					    SEG_386_UD | R_USR);
			src += bytesRead;
			dest += bytesRead;
			iop->io_ioc -= bytesRead;
			if (u.u_error == EFAULT) {
				u.u_error = 0;
				break;
			}
		}
		break;
	}

	case DEV_KMEM:
		iowrite (iop, iop->io_seek, iop->io_ioc);
		if (u.u_error == EFAULT)
			u.u_error = 0;
		break;

	case DEV_CLOCK:
		/*
		 * Don't go past the end of the CLOCK.
		 */
		if (iop->io_seek >= CLOCK_LEN)
			break;

		/*
		 * Lock the clock before any reading.
		 */

		if (lock_clock () == 0) {
			SET_U_ERROR (EIO, "RT clock will not settle.");
			break;
		}

		/*
		 * Read the requested data out of the CMOS.
		 */
		for (seek = iop->io_seek; seek < CLOCK_LEN; seek++) {
			if(ioputc(read_cmos(seek), iop) == -1)
				break;
		}

		/*
		 * Now that we are done reading the CMOS, let
		 * the clock loose.
		 */
		unlock_clock();
		break;

	case DEV_CMOS:
		/*
		 * Don't go past the end of the CMOS.
		 */
		if (iop->io_seek >= CMOS_LEN)
			break;

		/*
		 * Read the requested data out of the CMOS.
		 */
		for (seek = iop->io_seek; seek < CMOS_LEN; seek++) {
			if(ioputc(read_cmos(seek), iop) == -1)
				break;
		}
		break;

	case DEV_BOOTGIFT:
		/*
		 * Reads all from the data structure boot_gift.
		 */
		if (iop->io_seek < BG_LEN) {
			bytesRead = iop->io_ioc;

			/*
			 * Copy no more than to the end of boot_gift.
			 */
			if (iop->io_seek + bytesRead > BG_LEN)
				bytesRead = BG_LEN - iop->io_seek;

			iowrite (iop, (char *) (& boot_gift) + iop->io_seek,
				 bytesRead);
		}
		break;

	case DEV_PS:
		/* Lock the process table. It allows to have an atomic ps. */
		lock (pnxgate);
		/* Main driver loop. Go through all processes. Fill struct PS
		 * and send put to user buffer.
		 */
		for (pp1 = & procq; (pp1 = pp1->p_nforw) != & procq; ) {
			register int		i;	/* loop index */
			register unsigned	uLen, 	/* Process size */
						uLenR;	/* Real process size */
			register SEG	*sp;	/* u area segment */
			int work;	/* virtual click number */

			/* Check if driver can send next proc data */ 
			if (iop->io_ioc < sizeof (stMonitor)) 
				break;
				
			/* Calculate the size of process. */
			uLen = uLenR = 0;
			for (i = 0 ; i < NUSEG ; i++) {
				if ((sp = pp1->p_segp [i]) == NULL)
					continue;
				uLenR += sp->s_size;
				if (i == SIUSERP || i == SIAUXIL)
					continue;
				uLen += sp->s_size;
		
			} 

			/* Find u area for process pp1 */
			sp = pp1->p_segp [SIUSERP];
			ndpUseg = MAPIO (sp->s_vmem, U_OFFSET);
			work = workAlloc ();
			ptable1_v [work] = 
				   sysmem.u.pbase [btocrd (ndpUseg)] | SEG_RW;
			uprc = (UPROC *) (ctob (work) + U_OFFSET);
			memcpy (psData.u_comm, uprc->u_comm, ARGSZ);
			memcpy (psData.u_sleep, uprc->u_sleep, U_SLEEP_LEN);
			workFree (work);

#ifdef	TRACER
			if (strncmp (psData.u_sleep, "lock",
				     U_SLEEP_LEN) == 0) {
				GATE	      *	g = pp1->p_event;
				printf ("[%d] locked at %x lock %s (%d) = %x\n",
					pp1->p_pid, g, g->_name, g->_count,
					g->_lock [0]);
			}
			if (strncmp (psData.u_sleep, "bpwait",
				     U_SLEEP_LEN) == 0) {
				BUF	      *	bp = pp1->p_event;
				printf ("[%d] blocked on %x flags = %x\n",
					pp1->p_pid, bp, bp->b_flag);
			}
#endif

			/* fill up stMonitor */
			psData.p_pid = pp1->p_pid;
			psData.p_ppid = pp1->p_ppid;
			psData.p_uid = pp1->p_uid;
			psData.p_ruid = pp1->p_ruid;
			psData.p_rgid = pp1->p_rgid;
			psData.p_state = pp1->p_state;
			psData.p_flags = pp1->p_flags;
			psData.rrun = (char *) pp1 != pp1->p_event;
			psData.p_event = pp1->p_event;
			psData.p_ttdev = pp1->p_ttdev;
			psData.p_nice = pp1->p_nice;
			psData.size = (short) (uLen >> 10);
			psData.rsize = (short) (uLenR >> 10);
			psData.p_schedPri = pp1->p_schedPri;
			psData.p_utime = pp1->p_utime;
			psData.p_stime = pp1->p_stime;

			memcpy (psData.pr_argv, psBuf, ARGSZ);

			/* send data to user */
			iowrite (iop, (char *) & psData, sizeof (stMonitor));
		}
		unlock (pnxgate);
		break;

	case DEV_KMEMHI:
		iowrite (iop, iop->io_seek - KMEMHI_BASE, iop->io_ioc);
		if (u.u_error == EFAULT)
			u.u_error = 0;
		break;

	default:
		SET_U_ERROR (ENXIO, "nlread(): illegal minor device for null");
	}
	return;
}

/*
 * Null/memory write routine.
 */
void
nlwrite(dev, iop)
dev_t dev;
register IO *iop;
{
	register unsigned bytesWrit;
	unsigned write_cmos();
	unsigned seek;
	int	ch;

	switch (minor (dev)) {
	case DEV_NULL:
		/*
		 * Tell caller all bytes were written.
		 */
		iop->io_ioc = 0;
		break;

	case DEV_MEM:
		while (iop->io_ioc) {
			int src = iop->io.pbase;
			int dest = iop->io_seek;
			int numBytes = PXCOPY_LIM;
			if (numBytes > iop->io_ioc)
				numBytes = iop->io_ioc;

			bytesWrit = xpcopy (src, dest, numBytes,
					    SEG_386_UD | R_USR);
			src += bytesWrit;
			dest += bytesWrit;
			iop->io_ioc -= bytesWrit;
			if (u.u_error == EFAULT) {
				u.u_error = 0;
				break;
			}
		}
		break;

	case DEV_KMEM:
		ioread (iop, iop->io_seek, iop->io_ioc);
		break;

	case DEV_CLOCK:
		/*
		 * Don't go past the end of the CLOCK.
		 */
		if (iop->io_seek >= CLOCK_LEN)
			break;

		/*
		 * Lock the clock before any writing.
		 */
		if (lock_clock () == 0) {
			SET_U_ERROR (EIO, "RT clock will not settle.");
			break;
		}

		/*
		 * Write the requested data into the CMOS.
		 */

		for (seek = iop->io_seek ; seek < CLOCK_LEN ; seek ++) {
			if ((ch = iogetc (iop)) == -1)
				break;
			write_cmos (seek, ch);
		}

		/*
		 * Now that we are done writing the CMOS, let
		 * the clock loose.
		 */
		unlock_clock ();
		break;

	case DEV_CMOS:
		/*
		 * Don't go past the end of the CMOS.
		 */
		if (iop->io_seek >= CMOS_LEN)
			break;

		/*
		 * Write the requested data into the CMOS.
		 */
		for (seek = iop->io_seek ; seek < CMOS_LEN ; seek ++) {
			if ((ch = iogetc (iop)) == -1)
				break;
			write_cmos (seek, ch);
		}
		break;

	case DEV_BOOTGIFT:
		/*
		 * /dev/bootgift is not writable.
		 */
		break;

	case DEV_PS:
		/*
		 * We should not be able to open /dev/ps to write.
		 * Just paranoya.
		 */
		break;

	case DEV_KMEMHI:
		ioread (iop, iop->io_seek - KMEMHI_BASE, iop->io_ioc);
		break;

	default:
		SET_U_ERROR (ENXIO,
			     "nlwrite(): illegal minor device for null");
	}
	return;
}

#if defined NULL_IOCTL || defined IDLE_DEV /* Includes all of nlioctl().  */

/*
 * Do an ioctl call for /dev/null.
 */
int
nlioctl(dev, cmd, vec)
	dev_t dev;
	int cmd;
	char * vec;
{
	/* Only /dev/kmem and /dev/idle have an ioctl.  */
	switch (minor (dev)) {
#ifdef NULL_IOCTL
	case DEV_KMEM:
		switch (cmd) {
#ifdef DANGEROUS
		case NLCALL:	/* Call a function.  */
		return docall (vec);
#endif /* DANGEROUS */
		default:
			SET_U_ERROR (EINVAL,
				     "nioctl(): illegal command for kmem");
			return -1;
		}
#endif /* NULL_IOCTL */
#ifdef IDLE_DEV
	case DEV_IDLE:
		if (cmd != NLIDLE) { 
			SET_U_ERROR (EINVAL,
				     "nioctl(): illegal command for idle");
			return -1;
		} else {
			register PROC *pp;
			register int *mem = vec;


			pp = & procq;	/* point to process queue */

			if (pp->p_pid != 0) {
				while ((pp = pp->p_nforw) != &procq)
					if (pp->p_pid == 0)       /* idle process ? */
						break;
			}

			/*
			 * At this point, pp->p_utime and pp->p_stime contain
			 * the idle time of the system process
			 */

			if (pp->p_pid != 0)
				putuwd (mem ++, 0);
			else
				putuwd (mem ++, pp->p_utime + pp->p_stime);
			putuwd (mem, lbolt);
			return 1; 
		}

#endif /* IDLE_DEV */
	default:
		SET_U_ERROR(EINVAL, "illegal minor device for null ioctl");
		return -1;
	} /* switch on minor device */

} /* nlioctl() */

#endif /* NULL_IOCTL || IDLE_DEV */

#ifdef DANGEROUS /* Includes all of docall().  */
/*
 * MASSIVE SECURITY HOLE!  This should NOT be included in a distribution
 * system.  Among other problems, it becomes possible to do "setuid(0)".
 *
 * Call a function with arguments.
 *
 * Takes an array of unsigned ints.  The first element is the length of
 * the whole array, the second element is a pointer to the function to
 * call, all other elements are arguments.  At most 5 arguments may be
 * passed.
 *
 * Returns the return value of the called fuction in uvec[0].
 */
int
docall(uvec)
	unsigned uvec[];
{
	int (* func)();
	unsigned kvec[7];
	int retval;

	printf("NLCALL security hole.\n");

	/* Fetch the first element of vec.  */
	ukcopy (uvec, kvec, sizeof (unsigned));

	if (kvec [0] < 2 || kvec[0] > 7) {
		/* Invalid number of elements in uvec.  */
		SET_U_ERROR (EINVAL, "Invalid number of elements in uvec");
		return -1;
	}
	
	/* Fetch the whole vector.  */
	ukcopy (uvec, kvec, kvec [0] * sizeof (unsigned));

	/* Extract the function.  */
	func = (int (*)()) kvec [1];

	/* Call the function with all arguments.  */
	retval = (* func) (kvec [2], kvec [3], kvec [4], kvec [5], kvec [6]);

	kucopy (& retval, uvec, sizeof (unsigned));
} /* docall() */

#endif /* DANGEROUS */

/*
 * int lock_clock() -- Stop the update cycle on the CMOS RT clock and
 * wait for it to settle.  Returns 0 if the clock would not settle
 * in time.
 */
int
lock_clock()
{
	register int i;

	/*
	 * Wait for the clock to settle.  If it does not settle in
	 * a reasonable amount of time, give up.
	 */

	i = 65536;	/* Loop for a longish time.  */
	while (-- i > 0) {
		if (0 == (UIP & read_cmos (SRA))) {
			break;	/* Break if there is no update in progress.  */
		}
	}
	
	if (0 == i) {
		/* The clock would not settle.  */
		return 0;
	}

	/*
	 * There is a tiny race here--an interrupt could conceivably
	 * come here, thus allowing enough delay for another update to
	 * begin.  But if we take interrupts that take a full second
	 * to process, other things are going to break horribly.
	 */
	
	/*
	 * Lock out updates.
	 * We set the No Updates bit in Clock Status Register B.
	 */
	write_cmos (SRB, read_cmos (SRB) | NO_UPD);
	return 1;
} /* lock_clock() */

/*
 * void unlock_clock() -- Restart the update cycle on the CMOS RT clock.
 */ 
void
unlock_clock()
{
	/*
	 * We clear the No Updates bit in Clock Status Register B.
	 */
	write_cmos (SRB, read_cmos(SRB) & ~ NO_UPD);
} /* unlock_clock() */
