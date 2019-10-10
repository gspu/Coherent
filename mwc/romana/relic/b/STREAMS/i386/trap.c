/* (lgl-
 *	COHERENT Version 4.0
 *	Copyright (c) 1982, 1992.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */

#include <common/_gregset.h>
#include <kernel/systab.h>

#include <sys/coherent.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <signal.h>

/* opcodes recognized, and partially emulated, in gp fault handler */
#define READ_CR0	1
#define WRITE_CR0	2
#define READ_CR2	3
#define READ_CR3	4
#define WRITE_CR3	5
#define HALT		6
#define IRET		7
#define READ_DR0	8
#define READ_DR1	9
#define READ_DR2	10
#define READ_DR3	11
#define READ_DR6	12
#define READ_DR7	13
#define WRITE_DR0	14
#define WRITE_DR1	15
#define WRITE_DR2	16
#define WRITE_DR3	17
#define WRITE_DR6	18
#define WRITE_DR7	19

#define ENTER_OP	0xC8	/* Opcode for 'enter' instruction.  */
#define IRET_RETRY_LIM	10
#define RESUME_FLAG	0x10000	/* RF bit in PSW */
#define TRAP_FLAG	0x00100	/* TF bit in PSW */

extern unsigned char selkcopy();
extern unsigned int DR0,DR1,DR2,DR3,DR7;
static int trap_op();


/*
 * Global symbols from kernel text.
 */

extern unsigned int	_Idle;
extern unsigned int	__xtrap_break__;
extern unsigned int	__xtrap_off__;
extern unsigned int	__xtrap_on__;
extern unsigned int	sig32;
extern unsigned int	syc32;

/*
 * iret_flt is set when first bad iret is detected.
 */
static int iret_flt;

/*
 * Trap handler.
 * The arguments are the registers,
 * saved on the stack by machine code. This call
 * is different from most C calls in that the registers
 * get copied back; if you change a "trap" parameter then
 * the machine register will be altered when the trap is
 * dismissed.
 *
 * Argument "trapno" is the return eip for the code calling tsave().
 */

void
trap (regset)
gregset_t	regset;
{
	register struct	systab	*stp;
	register int	sigcode;
	extern int	trapcode;
	extern	*mmdata[], mminit;
	cseg_t *pp;
	register SEG *segp;
	int	splo, datahi;
	unsigned int	txtlo, txthi;
	unsigned int cr2 = 0;
	unsigned int cpl = regset._i386._cs & SEG_PL;

	if (regset._i386._err == SINMI) {
		printf ("Parity error\n");
		curr_register_dump (& regset);
#if	0
		panic("...");
#else
		return;
#endif
	}

	/*
	 * Expect this to never happen!
	 */
	if (SELF->p_flags & PFKERN) {
		panic ("pid%d: kernel process trap: err=%x, ip=%x ax=%d",
			SELF->p_pid, regset._i386._err, regset._i386._eip,
			regset._i386._eax);
	}

	T_HAL (0x4000, printf ("T%d ", regset._i386._err));
	sigcode = 0;

#if	0
	u.u_regl = & regset;	/* hook in register set for consave/conrest */
#endif

	switch (regset._i386._err) {
	case SIOSYS:
		/*
		 * 286 System call.
		 */
		sigcode = oldsys (& regset);
		break;

	case SISYS: {
		long		args [MSACOUNT];
		unsigned	callnum;

		/*
		 * 386 System call.
		 */
		u.u_error = 0;
		callnum = regset._i386._eax;

		T_PIGGY (4, printf ("{%d", callnum));

		if (callnum < NMICALL)
			stp = sysitab + callnum;
		else if (callnum == COHCALL)
			stp = & cohcall;
		else if ((callnum & 0xFF) == 0x28 &&
			 (callnum >> 8) <= H28CALL)
			stp = h28itab + (callnum >> 8) - 1;
		else {
			sigcode = SIGSYS;
			goto trapend;
		}

		ukcopy (regset._i386._uesp + sizeof (long), args,
			stp->s_nargs * sizeof (long));

		if (u.u_error) {
			sigcode = SIGSYS;
			goto trapend;
		}

		/*
		 * NIGEL: This is sleazy and stupid, and must go ASAP. We
		 * must learn to initialize structures properly.
		 */

#if	0
		u.u_io.io_seg = IOUSR;
#endif

		if (envsave (& u.u_sigenv)) {
			u.u_error = EINTR;
		} else {
			regset._i386._eax = __DOSYSCALL (stp->s_nargs,
							 stp->s_func, args,
							 & regset);
			regset._i386._edx = u.u_rval2;
		}

		if (u.u_error) {
			regset._i386._eax = u.u_error;
			regset._i386._eflags |= MFCBIT;
		} else		/* clear carry flag in return efl */
			regset._i386._eflags &= ~ MFCBIT;

		T_PIGGY (0x04, printf ("=%x}", regset._i386._eax));
		break;
	    } /* end block */

		/*
		 * Trap.
		 */
	case SIDIV:
#ifdef	TRACER
		printf ("Integer divide by zero");
		curr_register_dump (& regset);
#endif
		sigcode = SIGFPE;
		break;

	case SISST:
		sigcode = SIGTRAP;
		break;

	case SIBPT:
		sigcode = SIGTRAP;
		break;

	case SIOVF:
		sigcode = SIGEMT;
		break;

	case SIBND:
		/*
		 * Bound
		 */
		sigcode = SIGIOT;
		break;

	case SIOP:
		/*
		 * Invalid opcode
		 */
		if (cpl < 2) {
			int	      *	ip = (int *) regset._i386._eip;

			curr_register_dump (& regset);
			printf ("(eip)=%x %x %x  ", ip [0], ip [1], ip [2]);
			panic ("Invalid Opcode");
		}
		sigcode = SIGILL;
		break;

#if 0
	case SIXNP:
		/*
		 * Processor extension not available
		 */
		if (int11() & 2)	/* NDP present */
			ndpNewOwner();
		else
			sigcode = SIGFPE;
		break;
#endif

	case SIDBL:
		/*
		 * Double exception
		 */
		panic ("double exception: cs=%x ip=%x", regset._i386._cs,
		       regset._i386._eip);
		sigcode = SIGSEGV;
		break;

	case SIXS:
		/*
		 * Processor extension segment overrun
		 */
		sigcode = SIGSEGV;
		break;

	case SITS:
		/*
		 * Invalid task state segment
		 */
		panic ("invalid tss: cs=%x ip=%x", regset._i386._cs,
		       regset._i386._eip);
		sigcode = SIGSEGV;
		break;

	case SINP:
		/*
		 * Segment not present
		 */
		sigcode = SIGSEGV;
		break;

	case SISS:
		/*
		 * Stack segment overrun/not present
		 */
		sigcode = SIGKILL;
		break;

	default:
		curr_register_dump (& regset);
		panic("Fatal Trap");
	}

trapend:
	/*
	 * Send user a signal.
	 * If not a breakpoint, do console register dump.
	 */
	if (sigcode) {
		if (sigcode != SIGTRAP) {
			curr_register_dump (& regset);
			printf("sigcode=#%d  User Trap\n", sigcode);
		}
		sendsig (sigcode, SELF);
	}
}

/*
 * trap_op()
 *
 * Look at the trapped instruction.
 * If it's one of a select few, recognize and return the type.
 * Otherwise, return 0.
 *
 * This could be fancier, but all we want to look for is:
 * (for CRx and DRx, second operand is limited to %eax).
 *	0F 20 C0	READ_CR0
 *	0F 22 C0	WRITE_CR0
 *	0F 20 D0	READ_CR2
 *	0F 20 D8	READ_CR3
 *	0F 22 D8	WRITE_CR3
 *	CF		IRET
 *	F4		HALT
 *	0F 23 C0	WRITE_DR0
 *	0F 23 C8	WRITE_DR1
 *	0F 23 D0	WRITE_DR2
 *	0F 23 D8	WRITE_DR3
 *	0F 23 F0	WRITE_DR6
 *	0F 23 F8	WRITE_DR7
 *	0F 21 C0	READ_DR0
 *	0F 21 C8	READ_DR1
 *	0F 21 D0	READ_DR2
 *	0F 21 D8	READ_DR3
 *	0F 21 F0	READ_DR6
 *	0F 21 F8	READ_DR7
 */
static int
trap_op(cs,eip)
unsigned int cs, eip;
{
	int		ret = 0;

	switch (ffbyte (eip, cs)) {
	case 0x0F:
		switch (ffbyte (eip + 1, cs)) {
		case 0x20:
			switch (ffbyte (eip + 2, cs)) {
			case 0xC0:
				ret = READ_CR0;
				break;
			case 0xD0:
				ret = READ_CR2;
				break;
			case 0xD8:
				ret = READ_CR3;
				break;
			}
			break;
		case 0x21:
			switch (ffbyte (eip + 2, cs)) {
			case 0xC0:
				ret = READ_DR0;
				break;
			case 0xC8:
				ret = READ_DR1;
				break;
			case 0xD0:
				ret = READ_DR2;
				break;
			case 0xD8:
				ret = READ_DR3;
				break;
			case 0xF0:
				ret = READ_DR6;
				break;
			case 0xF8:
				ret = READ_DR7;
				break;
			}
			break;
		case 0x22:
			switch (ffbyte (eip + 2, cs)) {
			case 0xC0:
				ret = WRITE_CR0;
				break;
			case 0xD8:
				ret = WRITE_CR3;
				break;
			}
			break;
		case 0x23:
			switch (ffbyte (eip + 2, cs)) {
			case 0xC0:
				ret = WRITE_DR0;
				break;
			case 0xC8:
				ret = WRITE_DR1;
				break;
			case 0xD0:
				ret = WRITE_DR2;
				break;
			case 0xD8:
				ret = WRITE_DR3;
				break;
			case 0xF0:
				ret = WRITE_DR6;
				break;
			case 0xF8:
				ret = WRITE_DR7;
				break;
			}
			break;
		}
		break;
	case 0xF4:
		ret = HALT;
		break;
	case 0xCF:
		ret = IRET;
		break;
	}
	return ret;
}

/*
 * Kernel debugger.
 *
 * Runs in ring 0.
 */

void
__debug_ker__ (regset)
gregset_t	regset;
{
	unsigned int	dr6 = read_dr6 ();
	unsigned	cpl = regset._i386._cs & SEG_PL;
	int		do_rdump = 1;

	if (dr6 & 0xf) {	/* report breakpoint exception(s) */
		printf("Breakpoint  ");
		if (dr6 & 1)
			printf("DR0=%x  ", DR0);
		if (dr6 & 2)
			printf("DR1=%x  ", DR1);
		if (dr6 & 4)
			printf("DR2=%x  ", DR2);
		if (dr6 & 8)
			printf("DR3=%x  ", DR3);
		printf("DR7=%x\n", DR7);
	}

	if (dr6 &  0xf000) {	/* report other debug exception(s) */
		if (dr6 & 0x8000)
			printf("Switch to debugged task\n");

		if (dr6 & 0x4000) {
			/* Single Step */
			switch (cpl) {
			/*
			 * If user code trapped, send signal
			 * and suppress console register dump.
			 */
			case DPL_1:
				/*
				 * Turn off single-stepping when entering
				 * Ring 1.
				 */
				if (regset._i386._eip == & syc32 ||
				    regset._i386._eip == & sig32) {
					do_rdump = 0;
				} else {
printf("\nefl=%x  No single stepping the kernel.\n", regset._i386._eflags);
				}
				regset._i386._eflags &= ~TRAP_FLAG;
				break;
			case DPL_3:
				do_rdump = 0;
T_HAL(0x20000, printf ("Kernel SSTEP eip=%x efl=%x  ", regset._i386._eip,
			regset._i386._eflags));
				sendsig (SIGTRAP, SELF);
				break;
			}
		}
		if (dr6 & 0x2000) {
			printf("ICE in use\n");
			regset._i386._eip += 3;
		}
	}

	if (do_rdump)
		curr_register_dump (& regset);

	write_dr6 (0);
	regset._i386._eflags |= RESUME_FLAG;
	return;
}

/*
 * General protection fault handler.
 * Entered via a ring 0 gate.
 */

void
gpfault (regset)
gregset_t	regset;
{
	unsigned cpl = regset._i386._cs & SEG_PL;

	/*
	 * Switch on CPL of code that trapped.
	 */

	switch(cpl) {
	case DPL_0:
		/*
		 * Ring 0 should not gp fault.
		 */
		curr_register_dump (& regset);
		panic ("System GP Fault from Ring 0");
		break;

	case DPL_1:
		/*
		 * If ring 1 faulted on a valid request, emulate the
		 * request while running in ring 0.
		 */
		switch (trap_op (regset._i386._cs, regset._i386._eip)) {
		case READ_CR0:
			regset._i386._eax = read_cr0 ();
			regset._i386._eip += 3;
			break;
#if 0
		case WRITE_CR0:
			if (eax & 4)
				setfpe (0);
			else
				setfpe (1);
			eip += 3;
			break;
#endif
		case READ_CR2:
			regset._i386._eax = read_cr2 ();
			regset._i386._eip += 3;
			break;

		case READ_CR3:
			regset._i386._eax = read_cr3 ();
			regset._i386._eip += 3;
			break;
#if 0
		case WRITE_CR3:
			mmuupdnR0 ();
			eip += 3;
			break;
#endif
		case IRET:
			/*
			 * Some CPU's wrongly generate GP faults on IRET
			 * from inner ring to ring 3.
			 * Fix is to retry the instruction a few times.
			 */
			if (! iret_flt) {
				iret_flt = 1;
				printf ("CPU Bug:  "
				  "Spurious GP Fault on Iret to Ring 3.\n");
			}
			break;

		case READ_DR0:
			regset._i386._eax = read_dr0 ();
			regset._i386._eip += 3;
			break;

		case READ_DR1:
			regset._i386._eax = read_dr1 ();
			regset._i386._eip += 3;
			break;

		case READ_DR2:
			regset._i386._eax = read_dr2 ();
			regset._i386._eip += 3;
			break;

		case READ_DR3:
			regset._i386._eax = read_dr3 ();
			regset._i386._eip += 3;
			break;

		case READ_DR6:
			regset._i386._eax = read_dr6 ();
			regset._i386._eip += 3;
			break;

		case READ_DR7:
			regset._i386._eax = read_dr7 ();
			regset._i386._eip += 3;
			break;

		case WRITE_DR0:
			write_dr0 (regset._i386._eax);
			regset._i386._eip += 3;
			break;

		case WRITE_DR1:
			write_dr1 (regset._i386._eax);
			regset._i386._eip += 3;
			break;

		case WRITE_DR2:
			write_dr2 (regset._i386._eax);
			regset._i386._eip += 3;
			break;

		case WRITE_DR3:
			write_dr3 (regset._i386._eax);
			regset._i386._eip += 3;
			break;

		case WRITE_DR6:
			write_dr6 (regset._i386._eax);
			regset._i386._eip += 3;
			break;

		case WRITE_DR7:
			write_dr7 (regset._i386._eax);
			regset._i386._eip += 3;
			break;

		case HALT:
			halt ();
			break;
#if 0
		case WRITE_DR7:
			/*
			 * Expect breakpoint info in globals DR0-3,DR7.
			 */
printf("Setting DR0=%x  DR1=%x  DR2=%x  DR3=%x  DR7=%x\n",
  DR0, DR1, DR2, DR3, DR7);
			write_dr0 (DR0);
			write_dr1 (DR1);
			write_dr2 (DR2);
			write_dr3 (DR3);
			write_dr7 (DR7);
			regset._i386._eip += 3;
			break;
#endif
		default:
			if (regset._i386._eip >= & __xtrap_on__ &&
			    regset._i386._eip < & __xtrap_off__) {
#ifdef	TRACER
				curr_register_dump (& regset);
				printf ("copy fault called from %x\n",
					* (int *) regset._i386._edx);
#endif
				SET_U_ERROR (EFAULT, "copy gp");
				regset._i386._eip = & __xtrap_break__;
			} else {
				curr_register_dump (& regset);
				panic ("System GP Fault from Ring 1");
			}
		}
		goto gpdone;
		break;

	case DPL_2:
		/*
		 * Nothing should be running in Ring 2.
		 */

	case DPL_3:
		/*
		 * Ring 3 gp fault means errant user process.
		 */
		curr_register_dump (& regset);
		printf ("User GP Violation\n");
		sendsig (SIGSEGV, SELF);
		break;
	}
gpdone:
	return;
}


/*
 * User debugger.
 *
 * Runs in ring 1.
 */

void
__debug_usr__ (regset)
gregset_t	regset;
{
	unsigned int	dr6 = read_dr6();
	unsigned	cpl = regset._i386._cs & SEG_PL;
	int		do_rdump = 1;

	if (dr6 & 0xf) {	/* report breakpoint exception(s) */
		printf("Breakpoint  ");
		if (dr6 & 1)
			printf("DR0=%x  ", DR0);
		if (dr6 & 2)
			printf("DR1=%x  ", DR1);
		if (dr6 & 4)
			printf("DR2=%x  ", DR2);
		if (dr6 & 8)
			printf("DR3=%x  ", DR3);
		printf("DR7=%x\n", DR7);
	}

	if (dr6 &  0xf000) {	/* report other debug exception(s) */
		if (dr6 & 0x8000)
			printf("Switch to debugged task\n");

		if (dr6 & 0x4000) {
			/* Single Step */
			switch(cpl) {
			/*
			 * If user code trapped, send signal
			 * and suppress console register dump.
			 */
			case DPL_1:
				/*
				 * Turn off single-stepping when entering
				 * Ring 1.
				 */
				if (regset._i386._eip == & syc32 ||
				    regset._i386._eip == & sig32) {
					do_rdump = 0;
				} else {
printf("/nefl=%x  No single stepping the kernel.\n", regset._i386._eflags);
				}
				regset._i386._eflags &= ~TRAP_FLAG;
				break;

			case DPL_3:
				do_rdump = 0;
T_HAL(0x20000, printf ("User SSTEP eip=%x efl=%x  ", regset._i386._eip,
			regset._i386._eflags));
				sendsig (SIGTRAP, SELF);
				break;
			}
		}
		if (dr6 & 0x2000) {
			printf("ICE in use\n");
			regset._i386._eip += 3;
		}
	}

	if (do_rdump)
		curr_register_dump (& regset);

	write_dr6 (0);
	regset._i386._eflags |= RESUME_FLAG;
	return;
}

irqblab (regset)
gregset_t	regset;
{
	puts("*ip=");
	print32 (regset._i386._eip);
	puts(" *err=");
	print32 (regset._i386._err);

	if ((regset._i386._err & 0xff) == 0x40) {
		int irqno = (regset._i386._err >> 8) & 0xFF;
		print8 (irqno);
	} else if (regset._i386._err == 2)
		puts("NMI ");
}

void
pagefault (regset)
gregset_t	regset;
{
	register struct	systab	*stp;
	register int	callnum;
	register int	sigcode;
	extern int	trapcode;
	extern	*mmdata[], mminit;
	cseg_t *pp;
	register SEG *segp;
	int	splo, datahi;
	unsigned int	txtlo, txthi;
	unsigned long newsp;	/* Anticipated value for stack pointer.  */
	unsigned int cr2 = 0;
	unsigned int cpl = regset._i386._cs & SEG_PL;

	/*
	 * Expect this to never happen!
	 */

	if (SELF->p_flags & PFKERN) {
		panic ("pid%d: kernel process trap: err=%x, ip=%x ax=%d",
			SELF->p_pid, regset._i386._err, regset._i386._eip,
			regset._i386._eax);
	}

	T_HAL (0x4000, printf("T%d ", regset._i386._err));
	sigcode = 0;

#if	0
	u.u_regl = &gs;	/* hook in register set for consave/conrest */
#endif

	/*
	 * Page fault
	 */

	cr2 = read_cr2 ();
	if (cpl < 2) {
		/*
		 * If page fault during Ring 1 copy service routine,
		 * such as kucopy or ukcopy, set u_error and abort
		 * the copy, but don't send signal to the user.
		 */

		if (regset._i386._eip >= & __xtrap_on__ &&
		    regset._i386._eip < & __xtrap_off__) {

			T_HAL (0x1000, printf ("copy trapped "));
#ifdef	TRACER
			curr_register_dump (& regset);
			printf ("copy fault called from %x\n",
				* (int *) regset._i386._edx);
#endif
			SET_U_ERROR (EFAULT, "copy service");
			regset._i386._eip = & __xtrap_break__;
			goto pf_end;
		} else {
			printf ("cr2=%x", cr2);
			curr_register_dump (& regset);
			panic ("Kernel Page Fault");
		}
	}

	/* Check for stack underflow. */

	/*
	 * I think 'splo' is being calculated in a bass-ackwards way,
	 * and that 'datahi' is just wrong, but I'm not certain,
	 * so the fixes are #if 0'd out. -piggy
	 *
	 * I'll take out the 0 some day and test these changes.
	 */
	segp = u.u_segl [SISTACK].sr_segp;
#if 0
	splo = u.u_segl [SISTACK].sr_base - segp->s_size;
	datahi = u.u_segl [SIPDATA].sr_base + u.u_segl [SIPDATA].sr_size;
#else
	splo = __xmode_286 (& regset) ? ISP_286 : ISP_386;
	splo -= segp->s_size;
	datahi = u.u_segl [SIPDATA].sr_size;
#endif /* 0 */

	/*
	 * Catch bad function pointer here - don't want to restart
	 * the user instruction and get runaway segv's.
	 *
	 * For 286 executables, eip starts at 0, but cs points to
	 * descriptor SEG_286_UII which adds 0x400000 (UII_BASE).
	 */

	txtlo = u.u_segl [SISTEXT].sr_base;
	if (__xmode_286 (& regset))
		txtlo -= UII_BASE;

	txthi = txtlo + u.u_segl [SISTEXT].sr_size;
	if (regset._i386._eip < txtlo ||
	    regset._i386._eip > txthi) {
		T_HAL (0x1000, printf ("Bad eip, txtlo=%x txthi=%x\n",
				       txtlo, txthi));
		goto bad_pf;
	}

	/*
	 * If we trapped on an 'enter' instruction, the stack
	 * pointer (uesp) has not yet been decremented.  In
	 * order to correctly process such a stack overflow,
	 * we must look at the _expected_ value for uesp.
	 * NB: We COPY uesp, because that arg gets loaded back
	 * into the real esp--when we return from the trap the
	 * enter instruction will decrement the esp.
	 */

	newsp = __xmode_286 (& regset) ? regset._i286._usp :
					 regset._i386._uesp;

	if (ffbyte (regset._i386._eip, regset._i386._cs) == ENTER_OP) {
		/*
		 * Adjust the sp by the argument of the ENTER
		 * instruction.
		 */

		newsp -= ffword (regset._i386._eip + 1, regset._i386._cs);
	}

	if (cr2 <= splo && newsp <= splo && newsp > datahi &&
	    btoc (datahi) < btocrd (splo)) {

		pp = c_extend(segp->s_vmem, btoc(segp->s_size));
		if (pp==0) {
			T_HAL (0x1000, printf ("c_extend(%x,%x)=0 ",
					       segp->s_vmem,
					       btoc(segp->s_size)));
			goto bad_pf;
		}

		segp->s_vmem = pp;
		segp->s_size += NBPC;

		if (sproto (0) == 0) {
			T_HAL(0x1000, printf("sproto(0)=0 "));
			goto bad_pf;
		}

		segload ();
		goto pf_end;
	}

	/*
	 * Catch bad data pointer here - don't want to restart
	 * the user instruction and get runaway segv's.
	 */

	{
		T_HAL(0x1000, printf ("Bad data, splo=%x datahi=%x\n",
				      splo, datahi));
	}

bad_pf:
	/*
	 * User generated unacceptable page fault.
	 */

	sigcode = SIGSEGV;
	printf("\ncr2=%x  ", cr2);

pf_end:
	/*
	 * Send user a signal.
	 * If not segload ();
	 * If not a breakpoint, do console register dump.
	 */

	if (sigcode) {
		curr_register_dump (& regset);
		printf ("sigcode=#%d  User Page Fault\n", sigcode);
		sendsig (sigcode, SELF);
	}
}
