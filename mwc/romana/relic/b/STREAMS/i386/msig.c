/*
 * i386/msig.c
 *
 * signal handler start and return
 *
 * Revised: Tue May 11 14:24:00 1993 CDT
 */

/*
 * ----------------------------------------------------------------------
 * Includes.
 */

#include <common/_gregset.h>
#include <kernel/sigproc.h>
#include <sys/debug.h>

#include <sys/coherent.h>

/*
 * ----------------------------------------------------------------------
 * Definitions.
 *	Constants.
 *	Macros with argument lists.
 *	Typedefs.
 *	Enums.
 */

/*
 * ----------------------------------------------------------------------
 * Functions.
 *	Import Functions.
 *	Export Functions.
 *	Local Functions.
 */
void	msigend();
void	msigstart();

/*
 * ----------------------------------------------------------------------
 * Global Data.
 *	Import Variables.
 *	Export Variables.
 *	Local Variables.
 */
extern void	(*ndpKfsave)();
extern void	(*ndpKfrstor)();

/*
 * ----------------------------------------------------------------------
 * Code.
 */

/*
 * msigstart(signum, func)
 *
 * signum is 1-based signal number
 * func points to signal handler in user text, which is the address of a
 *	real handler and not a cookie such as SIG_DFL.
 *
 * This routine will set up the stack as shown before entering the user
 * signal handler:
 *
 *	ndp/emulator context (struct _fpstate or struct _fpemstate or absent)
 *	ndp/emulator flags
 *	previous signal mask
 *	fpstackframe:
 *		wsp (Weitek context pointer - always null, but part of BCS)
 *		fpsp (floating point context pointer, possibly null)
 *		CPU register set (SS+1 long registers)
 *		1-based signal number (user can overwrite this one)
 *	u.u_sigreturn (in place of user return address)
 */

/*
 * A special define for signal stack arithmetic:
 * Will copy at least u_sigreturn, _fpstackframe, previous mask, and ndpFlags.
 * Note that _fpstackframe is an iBCS2 anomaly, not present in the ABI; below
 * we define a structure with similar data and identical structure up to a
 * point.
 */

struct basic_signal_frame {
	ulong_t		sf_sigreturn;
	struct _fpstackframe _thank_you_intel;

	/*
	 * From this point on is not subject to iBCS2.
	 */

	__sigset_t	sf_signal_mask;
	ulong_t		sf_ndpflags;
};
#define	sf_signo	_thank_you_intel.signo
#define	sf_regset	_thank_you_intel.regs [0]
#define	sf_fpsp		_thank_you_intel.fpsp
#define	sf_weitekp	_thank_you_intel.wsp

/*
 * Be careful! sf_regset yields something not quite the right size!
 */
#define	SF_REGSET(rs)	(* (gregset_t *) & (rs).sf_regset)

void
msigstart(signum, func, regsetp)
__sigfunc_t	func;
gregset_t     *	regsetp;
{
	register int uesp;
	int sphi, splo;
	SEG * segp;
	cseg_t * pp;
	int sigArea;	/* number of bytes written to user's stack */
	struct _fpstate * fpsp;
	struct basic_signal_frame
			signal_frame;

	/*
	 * The following is all highly specific to i386 tasks. Also, check
	 * that our notion of general-register set matches the iBCS2 cookies.
	 */

	ASSERT (__xmode_286 (regsetp) == 0);
	ASSERT ((SS + 1) * sizeof (long) == sizeof (gregset_t));

	/*
	 * Will copy at least u_sigreturn, _fpstackframe, and ndpFlags.
	 * If using ndp, need room for an _fpstate.
	 * If emulating, need room for an _fpemstate.
	 * Fp context is immediately above regular signal context.
	 */

	sigArea = sizeof (signal_frame);
	uesp = regsetp->_i386._uesp;

	if (rdNdpUser () || rdEmTrapped ()) {
		fpsp = (struct _fpstate *) uesp - 1;
		sigArea += sizeof (struct _fpstate);
	} else
		fpsp = 0;

	uesp -= sigArea;

	/* Add to user stack if necessary. */
	segp = u.u_segl[SISTACK].sr_segp;
	sphi = ISP_386;
	splo = sphi - segp->s_size;

	if (splo > uesp) {
		pp = c_extend(segp->s_vmem, btoc(segp->s_size));
		if (pp==0) {
			printf("Signal failed.  cmd=%s  c_extend(%x,%x)=0 ",
			  u.u_comm, segp->s_vmem, btoc(segp->s_size));
			return;
		}

		segp->s_vmem = pp;
		segp->s_size += NBPC;
		if (sproto(0)==0) {
			printf("Signal failed.  cmd=%s  sproto(0)=0 ",
			  u.u_comm);
			return;
		}

		segload();
	}

	/*
	 * Build signal stack frame locally and copy as a group.
	 */

	curr_signal_mask (NULL, & signal_frame.sf_signal_mask);

	signal_frame.sf_weitekp = 0;
	signal_frame.sf_fpsp = fpsp;

	SF_REGSET (signal_frame) = * regsetp;
	signal_frame.sf_signo = signum;
	signal_frame.sf_sigreturn = u.u_sigreturn;

	/*
	 * Turn off single-stepping, and set up user registers.
	 */

	regsetp->_i386._eflags &= ~ MFTTB;
	regsetp->_i386._eip = func;
	regsetp->_i386._uesp = uesp;

	/*
	 * We are about to enter a signal handling function for the process.
	 * If current process is using ndp
	 *   copy ndp state and related flags into signal handler stack
	 *   mark the process as not using ndp
	 *   arm EM traps in case signal handler uses ndp
	 * Else if process is using emulator
	 *   copy emulator state and flags into signal handler stack
	 *   mark the process as not using emulator
	 * Else
	 *   put ndp/emulator flags on stack
	 */

	if (rdNdpUser ()) {
		/* if ndp state not saved yet for this process, save it now */
		if (! rdNdpSaved ()) {
			ndpSave (& u.u_ndpCon);
			wrNdpSaved (1);
		}

		signal_frame.sf_ndpflags = u.u_ndpFlags;

		kucopy (& u.u_ndpCon, fpsp, sizeof (struct _fpstate));
		ndpDetach ();

		wrNdpUser (0);
		wrNdpSaved (0);
		ndpEmTraps (1);
	} else {
		signal_frame.sf_ndpflags = u.u_ndpFlags;

		if (rdEmTrapped()) {
			if (ndpKfsave) {
				unsigned long sw_old;

				(* ndpKfsave) (& u.u_ndpCon, fpsp);
				sw_old = getuwd (& fpsp->sw);
				putuwd (& fpsp->status, sw_old);
				putuwd (& fpsp->sw, sw_old & 0x7f00);
			}
			wrEmTrapped(0);
		}
	}

	if (kucopy (& signal_frame, uesp, sizeof (signal_frame)) !=
			sizeof (signal_frame)) {
		printf ("Could not build signal frame!\n");
		return;
	}
}

void
msigend (regset)
gregset_t	regset;
{
	int savedNdpFlags;
	int sigNdpUser;
	int		temp;
	__sigset_t	signal_mask;
	struct basic_signal_frame
		      *	signal_framep;

	/*
	 * Retrieved saved signal mask and ndp flags; note that the sigreturn
	 * and signo members of the signal frame have vanished.
	 */

	signal_framep = (struct basic_signal_frame *) (regset._i386._uesp -
						       2 * sizeof (ulong_t));
	ASSERT (getuwd (& signal_framep->sf_weitekp) == 0);

	temp = ukcopy (& signal_framep->sf_signal_mask, & signal_mask,
		       sizeof (signal_mask));
	ASSERT (temp == sizeof (signal_mask));

	curr_signal_mask (& signal_mask, NULL);

	savedNdpFlags = getuwd (& signal_framep->sf_ndpflags);

	sigNdpUser = rdNdpUser ();
	u.u_ndpFlags = savedNdpFlags;

	/*
	 * We are about to leave a signal handling function for this process.
	 * If signal function for this process was using ndp
	 * And main process was *not* using ndp
	 *   Detach signal function from ndp
	 *   Restore current EM to its pre-signal value.
	 * If main process *was* using ndp
	 *   restore its ndp state and make it ndp owner again.
	 * If main process was using emulator
	 *   restore emulator state.
	 */
	if (sigNdpUser && !rdNdpUser()) {
		ndpDetach();
		ndpEmTraps(1);
	}

	if (rdNdpUser()) {
		ndpEmTraps(0);

		ASSERT (signal_framep + 1 == getuwd (& signal_framep->sf_fpsp));
		temp = ukcopy (signal_framep + 1, & u.u_ndpCon,
			       sizeof (struct _fpstate));
		ASSERT (temp == sizeof (struct _fpstate));

		ndpRestore (& u.u_ndpCon);
		wrNdpSaved (0);
		ndpMine();
	} else if (rdEmTrapped () && ndpKfrstor)
		(* ndpKfrstor) (signal_framep + 1, & u.u_ndpCon);

	/* Restore user process state to pre-signal values */

	temp = ukcopy (& signal_framep->sf_regset, & regset, sizeof (regset));
	ASSERT (temp == (SS + 1) * sizeof (long));
}
