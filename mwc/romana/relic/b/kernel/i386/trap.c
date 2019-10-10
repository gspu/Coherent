/* (lgl-
 *	COHERENT Version 4.0
 *	Copyright (c) 1982, 1992.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 -lgl) */
#include <sys/coherent.h>
#include <sys/reg.h>
#include <sys/systab.h>
#include <errno.h>
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
 * Macro RDUMP does register dump, followed by final message.
 *
 * Callable only from within trap() or one of its cousins.
 */
#define RDUMP() { \
  printf("\neax=%x  ebx=%x  ecx=%x  edx=%x\n", eax, ebx, ecx, edx); \
  printf("esi=%x  edi=%x  ebp=%x  esp=%x\n", esi, edi, ebp, esp); \
  printf("cs=%x  ds=%x  es=%x  ss=%x  fs=%x  gs=%x\n", \
    cs&0xffff, ds&0xffff, es&0xffff, ss&0xffff, fs&0xffff, gs&0xffff); \
  printf("err #%d eip=%x  uesp=%x  cmd=%s\n", err, eip, uesp, u.u_comm); \
  printf("efl=%x  ", efl); }
/* end RDUMP */

/*
 * Debug only - display 64 words of stack traceback.
 */
#define SDUMP(frame) { \
  int *ip = frame, i; \
  for (i=0;i < 32;i++) { \
    if ((i % 8)==0) \
      putchar('\n'); \
    printf("%x ", *ip++); \
  } \
  putchar('\n'); \
}
/* end SDUMP */

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
trap(gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax, trapno, err,
  eip, cs, efl, uesp, ss)
char *eip;
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
	unsigned char opcode;	/* Opcode we trapped on.	*/
	unsigned short e_arg;	/* Argument to 'enter' opcode.  */
	unsigned long newsp;	/* Anticipated value for stack pointer.  */
	unsigned int cr2 = 0;
	unsigned int cpl = cs & SEG_PL;

	/*
	 * Avoid sign extension confusion on 286 ds
	 */
	if (ds == (SEG_286_UD | R_USR))
		uesp = (unsigned short)uesp;

	if (err==SINMI) {
		printf("Parity error\n");
		RDUMP();
		panic("...");
	}

	/*
	 * Expect this to never happen!
	 */
	if (SELF->p_flags & PFKERN) {
		panic("pid%d: kernel process trap: err=%x, ip=%x ax=%d",
			SELF->p_pid, err, eip, eax);
	}

	T_HAL(0x4000, printf("T%d ", err));
	sigcode = 0;

	u.u_regl = &gs;	/* hook in register set for consave/conrest */

	switch (err) {
	case SIOSYS:
		/*
		 * 286 System call.
		 */
		sigcode = oldsys();
		break;
	case SISYS:
		/*
		 * 386 System call.
		 */
		u.u_error = 0;
		callnum = eax;

		T_PIGGY(4, printf("{%d}", callnum));

		if (callnum < NMICALL)
			stp = sysitab + callnum;
		else if ( callnum == COHCALL )
			stp = &cohcall;
		else if ( ((callnum&0xFF)==0x28) && ((callnum>>8)<=H28CALL) )
			stp = h28itab + ((callnum>>8) - 1);
		else {
			sigcode = SIGSYS;
			goto trapend;
		}

		ukcopy(uesp+sizeof(long),u.u_args, stp->s_nargs*sizeof(long));
		if (u.u_error) {
			sigcode = SIGSYS;
			goto trapend;
		}

		u.u_io.io_seg = IOUSR;
		if (envsave(&u.u_sigenv)) {
			u.u_error = EINTR;
		} else {
			eax = (*stp->s_func)(u.u_args[0],
			      u.u_args[1],
			      u.u_args[2],
			      u.u_args[3],
			      u.u_args[4],
			      u.u_args[5]);
			edx = u.u_rval2;
		}

		efl &= ~MFCBIT;		/* clear carry flag in return efl */
		if (u.u_error) {
			eax = u.u_error;
			efl |= MFCBIT;
		}
		break;
		/*
		 * Trap.
		 */
	case SIDIV:
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
			int *ip = (int *)eip;

			RDUMP();
			printf("(eip)=%x %x %x  ", ip[0], ip[1], ip[2]);
			panic("Invalid Opcode");
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
		panic("double exception: cs=%x ip=%x", cs, eip);
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
		panic("invalid tss: cs=%x ip=%x", cs, eip);
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
		RDUMP();
		panic("Fatal Trap");
	}

trapend:
	/*
	 * Send user a signal.
	 * If not a breakpoint, do console register dump.
	 */
	if (sigcode) {
		if (sigcode != SIGTRAP) {
			RDUMP();
			printf("sigcode=#%d  User Trap\n", sigcode);
		}
		sendsig(sigcode, SELF);
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

	switch (selkcopy(cs,eip)) {
	case 0x0F:
		switch (selkcopy(cs,eip+1)) {
		case 0x20:
			switch (selkcopy(cs,eip+2)) {
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
			switch (selkcopy(cs,eip+2)) {
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
			switch (selkcopy(cs,eip+2)) {
			case 0xC0:
				ret = WRITE_CR0;
				break;
			case 0xD8:
				ret = WRITE_CR3;
				break;
			}
			break;
		case 0x23:
			switch (selkcopy(cs,eip+2)) {
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
__debug_ker__(gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax, trapno,
  err, eip, cs, efl, uesp, ss)
{
	unsigned int	dr6 = read_dr6();
	unsigned	cpl = cs & SEG_PL;
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
				if (eip == &syc32 || eip == &sig32) {
					do_rdump = 0;
				} else {
printf("/nefl=%x  No single stepping the kernel.\n", efl);
					SDUMP(uesp);
				}
				efl &= ~TRAP_FLAG;
				break;
			case DPL_3:
				do_rdump = 0;
T_HAL(0x20000, printf("Kernel SSTEP eip=%x efl=%x  ", eip, efl));
				sendsig(SIGTRAP, SELF);
				break;
			}
		}
		if (dr6 & 0x2000) {
			printf("ICE in use\n");
			eip += 3;
		}
	}
	if (do_rdump) {
		RDUMP();
	}
	write_dr6(0);
	efl |= RESUME_FLAG;
	return;
}

/*
 * General protection fault handler.
 * Entered via a ring 0 gate.
 */
gpfault(gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax, trapno, err,
  eip, cs, efl, uesp, ss)
char *eip;
{
	unsigned cpl = cs & SEG_PL;

	/*
	 * Switch on CPL of code that trapped.
	 */
	switch(cpl) {
	case DPL_0:
		/*
		 * Ring 0 should not gp fault.
		 */
		RDUMP();
		T_HAL(0x1000, SDUMP(&uesp));
		T_HAL(0x1000, SDUMP(*((&uesp) + 2)));
		panic("System GP Fault from Ring 0");
		break;
	case DPL_1:
		/*
		 * If ring 1 faulted on a valid request, emulate the
		 * request while running in ring 0.
		 */
		switch(trap_op(cs,eip)) {
		case READ_CR0:
			eax = read_cr0();
			eip += 3;
			break;
#if 0
		case WRITE_CR0:
			if (eax & 4)
				setfpe(0);
			else
				setfpe(1);
			eip += 3;
			break;
#endif
		case READ_CR2:
			eax = read_cr2();
			eip += 3;
			break;
		case READ_CR3:
			eax = read_cr3();
			eip += 3;
			break;
#if 0
		case WRITE_CR3:
			mmuupdnR0();
			eip += 3;
			break;
#endif
		case IRET:
			/*
			 * Some CPU's wrongly generate GP faults on IRET
			 * from inner ring to ring 3.
			 * Fix is to retry the instruction a few times.
			 */
			if (!iret_flt) {
				iret_flt = 1;
				printf("CPU Bug:  "
				  "Spurious GP Fault on Iret to Ring 3.\n");
			}
			break;
		case READ_DR0:
			eax = read_dr0();
			eip += 3;
			break;
		case READ_DR1:
			eax = read_dr1();
			eip += 3;
			break;
		case READ_DR2:
			eax = read_dr2();
			eip += 3;
			break;
		case READ_DR3:
			eax = read_dr3();
			eip += 3;
			break;
		case READ_DR6:
			eax = read_dr6();
			eip += 3;
			break;
		case READ_DR7:
			eax = read_dr7();
			eip += 3;
			break;
		case WRITE_DR0:
			write_dr0(eax);
			eip += 3;
			break;
		case WRITE_DR1:
			write_dr1(eax);
			eip += 3;
			break;
		case WRITE_DR2:
			write_dr2(eax);
			eip += 3;
			break;
		case WRITE_DR3:
			write_dr3(eax);
			eip += 3;
			break;
		case WRITE_DR6:
			write_dr6(eax);
			eip += 3;
			break;
		case WRITE_DR7:
			write_dr7(eax);
			eip += 3;
			break;
#if 0
		case WRITE_DR7:
			/*
			 * Expect breakpoint info in globals DR0-3,DR7.
			 */
printf("Setting DR0=%x  DR1=%x  DR2=%x  DR3=%x  DR7=%x\n",
  DR0, DR1, DR2, DR3, DR7);
			write_dr0(DR0);
			write_dr1(DR1);
			write_dr2(DR2);
			write_dr3(DR3);
			write_dr7(DR7);
			eip += 3;
			break;
#endif
		default:
			if (eip >= &__xtrap_on__ && eip < &__xtrap_off__) {
				SET_U_ERROR(EFAULT, "copy gp");
				eip = &__xtrap_break__;
			} else {
				RDUMP();
				T_HAL(0x1000, SDUMP(uesp));
				panic("System GP Fault from Ring 1");
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
		RDUMP();
		printf("User GP Violation\n");
		sendsig(SIGSEGV, SELF);
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
__debug_usr__(gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax, trapno,
  err, eip, cs, efl, uesp, ss)
{
	unsigned int	dr6 = read_dr6();
	unsigned	cpl = cs & SEG_PL;
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
				if (eip == &syc32 || eip == &sig32) {
					do_rdump = 0;
				} else {
printf("/nefl=%x  No single stepping the kernel.\n", efl);
					SDUMP(uesp);
				}
				efl &= ~TRAP_FLAG;
				break;
			case DPL_3:
				do_rdump = 0;
T_HAL(0x20000, printf("User SSTEP eip=%x efl=%x  ", eip, efl));
				sendsig(SIGTRAP, SELF);
				break;
			}
		}
		if (dr6 & 0x2000) {
			printf("ICE in use\n");
			eip += 3;
		}
	}
	if (do_rdump) {
		RDUMP();
	}
	write_dr6(0);
	efl |= RESUME_FLAG;
	return;
}

irqblab(gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax, trapno,
  err, eip, cs, efl, uesp, ss)
{
	puts("*ip=");
	print32(eip);
	puts(" *err=");
	print32(err);
	if ((err & 0xff) == 0x40) {
		int irqno = (err >> 8) & 0xFF;
		print8(irqno);
	} else if (err == 2) {
		puts("NMI ");
	}
}

pagefault(gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax, trapno, err,
  eip, cs, efl, uesp, ss)
char *eip;
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
	unsigned char opcode;	/* Opcode we trapped on.	*/
	unsigned short e_arg;	/* Argument to 'enter' opcode.  */
	unsigned long newsp;	/* Anticipated value for stack pointer.  */
	unsigned int cr2 = 0;
	unsigned int cpl = cs & SEG_PL;

	/*
	 * Avoid sign extension confusion on 286 ds
	 */
	if (ds == (SEG_286_UD | R_USR))
		uesp = (unsigned short)uesp;

	/*
	 * Expect this to never happen!
	 */
	if (SELF->p_flags&PFKERN) {
		panic("pid%d: kernel process trap: err=%x, ip=%x ax=%d",
			SELF->p_pid, err, eip, eax);
	}

	T_HAL(0x4000, printf("T%d ", err));
	sigcode = 0;

	u.u_regl = &gs;	/* hook in register set for consave/conrest */

	{
		/*
		 * Page fault
		 */
		cr2 = read_cr2();
		if (cpl < 2) {
			/*
			 * If page fault during Ring 1 copy service routine,
			 * such as kucopy or ukcopy, set u_error and abort
			 * the copy, but don't send signal to the user.
			 */
			if (eip >= &__xtrap_on__ && eip < &__xtrap_off__) {
				T_HAL(0x1000, printf("copy trapped "));
				SET_U_ERROR(EFAULT, "copy service");
				eip = &__xtrap_break__;
				goto pf_end;
			} else {
				printf("&uesp=>");
				SDUMP(&uesp);
#if 0
				printf("*(&uesp + 2)=>");
				SDUMP(*((&uesp) + 2));
#endif
				printf("cr2=%x", cr2);
				RDUMP();
				panic("Kernel Page Fault");
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
		segp = u.u_segl[SISTACK].sr_segp;
#if 0
		splo = u.u_segl[SISTACK].sr_base - segp->s_size;
		datahi = u.u_segl[SIPDATA].sr_base + u.u_segl[SIPDATA].sr_size;
#else
		splo = (XMODE_286) ? ISP_286 : ISP_386;
		splo -= segp->s_size;
		datahi = u.u_segl[SIPDATA].sr_size;
#endif /* 0 */

		/*
		 * Catch bad function pointer here - don't want to restart
		 * the user instruction and get runaway segv's.
		 *
		 * For 286 executables, eip starts at 0, but cs points to
		 * descriptor SEG_286_UII which adds 0x400000 (UII_BASE).
		 */
		txtlo = u.u_segl[SISTEXT].sr_base;
		if (XMODE_286)
			txtlo -= UII_BASE;
		txthi = txtlo + u.u_segl[SISTEXT].sr_size;
		if (eip < txtlo || eip > txthi) {
			T_HAL(0x1000, printf("Bad eip, txtlo=%x txthi=%x\n",
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
		newsp = uesp;
		opcode = selkcopy(cs, eip);
		if (ENTER_OP == opcode) {
			e_arg = (selkcopy(cs, eip+2)<<8) + selkcopy(cs, eip+1);
			newsp -= e_arg;
		}

		if (cr2 <= splo
		  && newsp <= splo
		  && newsp > datahi
		  && btoc(datahi) < btocrd(splo)) {
			pp = c_extend(segp->s_vmem, btoc(segp->s_size));
			if (pp==0) {
				T_HAL(0x1000, printf("c_extend(%x,%x)=0 ",
				  segp->s_vmem, btoc(segp->s_size)));
				goto bad_pf;
			}

			segp->s_vmem = pp;
			segp->s_size += NBPC;
			if (sproto(0)==0) {
				T_HAL(0x1000, printf("sproto(0)=0 "));
				goto bad_pf;
			}

			segload();
			goto pf_end;
		}

		/*
		 * Catch bad data pointer here - don't want to restart
		 * the user instruction and get runaway segv's.
		 */
		{
			T_HAL(0x1000, printf("Bad data, splo=%x datahi=%x\n",
			  splo, datahi));
		}

	bad_pf:
		/*
		 * User generated unacceptable page fault.
		 */
		sigcode = SIGSEGV;
		printf("\ncr2=%x  ", cr2);
	}

pf_end:
	/*
	 * Send user a signal.
	 * If not a breakpoint, do console register dump.
	 */
	if (sigcode) {
		RDUMP();
		printf("sigcode=#%d  User Page Fault\n", sigcode);
		sendsig(sigcode, SELF);
	}
}
