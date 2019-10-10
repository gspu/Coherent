/*
 *	trace2.c
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <l.out.h>
#include <sys/ptrace.h>
#include <sys/timeout.h>
#include <sys/uproc.h>
#include <signal.h>
#include "trace.h"
#include "i8086.h"

/*
 * Talk to the user and try to solve his problems.
 */
process()
{
	register BPT *bp;
	/* register  */ int n;
	register int nibflag;
	register int bptflag;
	register caddr_t pc;
	register caddr_t fp;
	register int f;

	int		pcmd;
	char		ans[10];

	/*
	 * Reverse-engineered pseudocode, minus global variable garbage.
	 *
	 * forever:
	 *   execute(":x\n") <= Preload interpreter stack.
	 *   (Will come back after traced process traps, e.g. after ":e".)
	 *   forever:
	 *     See if at call instruction.
	 *     Install breakpoints, except at current instruction.
	 *     See if current instruction is a breakpoint opcode.
	 *     Start up traced process, full speed or single step.
	 *     Report any error from ptrace call.
	 *     Wait for traced process.
	 *     Fetch registers and signal received from traced process.
	 *     Replace breakpoints with instructions.
	 *     If signal to child was not SIGTRAP
	 *       display message
	 *       execute(":f\n:x\n");
	 *       continue inner loop
	 *     Back up instruction pointer to start of the breakpoint.
	 *     If single stepping
	 *       Execute single step command string (sinp).
	 *       If last single step in count
	 *         execute(":x\n")
	 *   end forever
	 * end forever
	 *
	 */
top:
	bptflag = 0;
	execute(":x\n");
	for (;;) {
		/*
		 * Initialize flags.
		 */
		bitflag = 0;
		nibflag = 0;
		bp = 0;
	
		/*
		 * If in single step mode, set up for single step.
		 * If in SCONT mode, change to SCSET mode at call instruction.
		 */
		if (sinmode!=SNULL && sinmode!=SWAIT) {
			bitflag = 1;
			if (sinmode == SCONT)
				setcont();
		}
	
		/*
		 * Place all breakpoints.
		 */
		if (reg.valid)
			pc = getpc();

		for (n=NBPT, bp=&bpt[0]; n--; bp++) {
			/* skip unused breakpoints */
			if (bp->b_flag == 0)
				continue;

			/* fetch instruction at breakpoint address */
			add = bp->b_badd;
			if (getb(ISEG, bp->b_bins, sizeof(BIN)) == 0) {
				printb(bp->b_badd);
				goto err;
			}

			/* don't install breakpoint at current instruction */
			if (reg.valid && bptflag && bp->b_badd==pc) {
				bitflag = 1;
				continue;
			}

			/* put in a breakpoint */
			add = bp->b_badd;
fprintf(stderr, "replacing breakpoint at eip=%x\n", add);
			if (putb(ISEG, bin, sizeof(BIN)) == 0) {
				printb(bp->b_badd);
				goto err;
			}
		}
		bptflag = 0;

		/*
		 * Set flags and call machine dependent restore routine.
		 * Set "nibflag" if instruction at EIP is BRK.
		 */
		if (reg.valid && testbpt(pc))
			nibflag = 1;

		/*
		 * restret() looks at bitflag and returns 1 if bitflag set.
		 * This is bogus.
		 * Mainly, restret() undoes the current breakpoint
		 * if it is done at a system call.
		 * This is probably unnecessary.
		 */
#if 0
		if (restret() == 0)
			goto err;
#endif
	
		/*
		 * Start up the child.
		 */
		errno = 0;
		switch (sinmode) {
		case SNULL:
		case SWAIT:
			pcmd = PTRACE_RESUME;
			break;
		case SCSET:
		case SCONT:
		case SSTEP:
			pcmd = PTRACE_SSTEP;
			break;
		}
		ptrace(pcmd, pid, 1, 0);
		if (errno) {
			perror("ptrace");
			goto err;
		}
		if (waitc() == 0)
			goto err;
	
#if 0
/* whatever this is, it looks unnecessary. */
		if (trapint() == 0)
			goto err;
#endif

		/*
		 * Copy registers into ureg, and thence to r.
		 * Why are there two register sets?  I don't know - hws
		 */
		if (setregs() == 0)
			goto err;

		/*
		 * Set f to the signal number sent to the traced process
		 * and global "trapstr" to its name.
		 */
		if ((f=settrap()) == 0)
			goto err;
	
		/*
		 * Replace breakpoints with instructions.
		 */
fprintf(stderr, "Replace breakpoints with instructions\n");
		for (n=NBPT, bp=bpt; n--; bp++) {
			if (bp->b_flag == 0)
				continue;
			add = bp->b_badd;
			if (putb(ISEG, bp->b_bins, sizeof(BIN)) == 0) {
				printb(bp->b_badd);
				goto err;
			}
		}
	
		/*
		 * If latest signal to traced process was not SIGTRAP,
		 * tell the user, then accept input.
		 */
		if (f != SIGTRAP) {
			fprintf(stderr,
			  "Traced process did not stop at a breakpoint\n");
			execute(":f\n:x\n");
			continue;
		}

		/*
		 * Find the breakpoint we are at.
		 * Back up pc to start of the breakpoint.
		 */
		bp = 0;
		pc = getpc() - sizeof(BIN);
		if (bitflag==0 || nibflag) {
			for (n=NBPT, bp=&bpt[0]; n--; bp++) {
				if (bp->b_flag == 0)
					continue;
				if (bp->b_badd != pc)
					continue;
fprintf(stderr, "found bp at %x\n", pc);
				setpc((caddr_t)pc);
				bptflag = 1;
				break;
			}
			if (bp==&bpt[NBPT]) {
				bp = 0;
			}
		}

		/*
		 * If in single step mode, execute command.
		 */
		switch (sinmode) {
		case SCSET:
fprintf(stderr, "sinmode=SCSET\n");
			break;
		case SNULL:
fprintf(stderr, "sinmode=SNULL\n");
			break;
		case SWAIT:
fprintf(stderr, "sinmode=SWAIT\n");
			break;
		case SCONT:
fprintf(stderr, "sinmode=SCONT\n");
			break;
		case SSTEP:
fprintf(stderr, "sinmode=SSTEP\n");
			break;
		}
		switch (sinmode) {
		case SSTEP:
		case SCONT:
			execute(sinp);
			if (--sindecr == 0)
				execute(":x\n");
			continue;
		case SCSET:
			for (n=0; n<NBPT; n++)
				bpt[n].b_flag &= ~BSIN;
			sinmode = SWAIT;
			/* put a BSIN breakpoint at the return address */
			intcont();
			break;
		}

		/*
		 * If we got an unexpected trace trap or unknown
		 * breakpoint, we handle it here.
		 */
		if (bp == NULL) {
			if (bitflag==0 || nibflag) {
				fprintf(stderr,
				  "Unexpected trace trap or breakpoint\n");
				execute(":f\n:x\n");
			}
			continue;
		}

		/*
		 * Single step breakpoints have highest priority.
		 */
		fp = getfp();
		if (bp->b_flag & BSIN) {
			if (bp->b_sfpt==0 || bp->b_sfpt==fp) {
				bp->b_flag &= ~BSIN;
				if (sinmode == SWAIT) {
fprintf(stderr, "SCWAIT->SCONT??\n");
					sinmode = SCONT;
					execute(sinp);
					if (--sindecr == 0)
					       execute(":x\n");
					continue;
				}
			}
		}

		/*
		 * Return breakpoints are next.
		 */
		if (bp->b_flag & BRET) {
			if (fp == bp->b_rfpt) {
				bp->b_flag &= ~BRET;
				execute(bp->b_rcom);
				continue;
			}
		}

		/*
		 * Your conventional everyday ordinary breakpoint.
		 */
		if (bp->b_flag & BBPT) {
			execute(bp->b_bcom);
			continue;
		}
	}

	/*
	 * Something is terribly wrong.  Kill off our child,
	 * and generally reset everything to the start.
	 */
err:
	killc();
	reslout();
	bptinit();
	goto top;
}

/*
 * Given a command line in `miscbuf', parse the command line, kill the
 * current child and start up a new one.  0 is returned on success, 1
 * on failure.
 */
runfile()
{
	register char *bp, *cp;
	register int c;
	char *ifn, *ofn, *argl[ARGSIZE];
	int qflag, aflag, n;

	killc();
	if (objflag == 0) {
		printe("No executable");
		return 1;
	}
	ifn = NULL;
	ofn = NULL;
	qflag = 0;
	aflag = 0;
	n = 0;
	bp = miscbuf;
	cp = miscbuf;
	c = *bp++;
	while (c != '\n') {
		switch (c) {
		case '<':
			ifn = cp;
			c = *bp++;
			break;
		case '>':
			ofn = cp;
			if ((c=*bp++) == '>') {
				aflag = 1;
				c = *bp++;
			}
			break;
		default:
			if (n >= ARGSIZE-1) {
				printe("Too many arguments");
				return 1;
			}
			argl[n++] = cp;
		}
		while (qflag || !isascii(c) || !isspace(c)) {
			if (c == '\n')
				break;
			if (c == '"') {
				qflag ^= 1;
				c = *bp++;
				continue;
			}
			if (c == '\\') {
				if ((c=*bp++) == '\n') {
					printe("Syntax error");
					return 1;
				}
			}
			*cp++ = c;
			c = *bp++;
		}
		if (qflag) {
			printe("Missing \"");
			return 1;
		}
		*cp++ = '\0';
		if (c == '\n')
			break;
		while (isascii(c) && isspace(c))
			c = *bp++;
	}
	if (n == 0)
		argl[n++] = lfn;
	argl[n] = NULL;
	if (startup(argl, ifn, ofn, aflag) == 0)
		return 1;
	return 0;
}

/*
 * Start execution of the child.  `argv' is the argument list, `ifnp' is
 * the name of the input file, `ofnp' is the name of the output file and
 * `appf' tells us whether the output file is opened for append or write.
 */
startup(argv, ifnp, ofnp, appf)
char **argv;
char *ifnp;
char *ofnp;
{
	register int n;

	if ((pid=fork()) < 0) {
		printr("Cannot fork");
		return 0;
	}
	if (pid == 0) {
		if (ifnp != NULL) {
			if ((n=open(ifnp, 0)) < 0)
				panic("Cannot open %s", ifnp);
			dup2(n, 0);
			close(n);
		}
		if (ofnp != NULL) {
			n = -1;
			if (appf) {
				if ((n=open(ofnp, 1)) >= 0)
					lseek(n, 0L, 2);
			}
			if (n < 0) {
				if ((n=creat(ofnp, 0644)) < 0)
					panic("%s: cannot create", ofnp);
			}
			dup2(n, 1);
			close(n);
		}
		ptrace(PTRACE_SETUP, 0, NULL, 0);
		execv(lfn, argv);
		exit (1);
	}
	if (waitc() == 0)
		return 0;
	clramap();
	DSPACE = setsmap(NULL, (off_t)0, (off_t)LI, (off_t)0,
			getp, putp, DSEG);
	ISPACE = setsmap(NULL, (off_t)0, (off_t)LI, (off_t)0,
			getp, putp, ISEG);
	USPACE = setsmap(NULL, (off_t)0, (off_t)UPASIZE, (off_t)0,
			getp, putp, USEG);
	excflag = 1;
	regflag = 1;
	return 1;
}

/*
 * Given a newly started child, find out necessary information.
 */
shiftup()
{
	if (trapint() == 0)
		return 0;
	if (setregs() == 0)
		return 0;
	return settrap();
}

/*
 * Kill off our child.
 */
killc()
{
	if (excflag) {
		ptrace(PTRACE_TERM, pid, 0, 0);
		waitc();
	}
	excflag = 0;
	regflag = 0;
	trapstr = NULL;
	reg.valid = 0;
}

/*
 * Wait for the traced process to stop.
 */
waitc()
{
	register int p;
	int s;
	extern errno;

	while ((p=wait(&s)) != pid) {
		if (p >= 0) {
			printr("Adopted a child %d", p);
			continue;
		}
		if (intflag == 0) {
			excflag = 0;
			printr("Nonexistent child");
			return 0;
		}
		intflag = 0;
	}
	if ((s&0xff) != 0x7f) {
		excflag = 0;
		printr("Child process terminated (%x)", (s>>8)&0xff );
		return 0;
	}
	return 1;
}


/******************************************************************************
 *
 * Note some #defines in these include files interferes with
 * items in preceeding include files. Hence the strange
 * program order.
 *
 ******************************************************************************/

#include <coff.h>

/*
 * Reset segmentation for an coff or l.out format.
 */
reslout()
{
	FILEHDR		coffh;
	struct ldheader ldh;

	clramap();
	objflag = 0;

	fseek(lfp, (long)0, 0);
	if (fread(&coffh, sizeof(coffh), 1, lfp) != 1) {
		printr("Can't read object file");
		return 0;
	}

	if (coffh.f_magic == C_386_MAGIC) {
#if 0
		/* get entry point from optional coff header */
		struct aouthdr	opth;

		reg.r_ip = 0xA8;
		if (coffh.f_opthdr >= sizeof(struct aouthdr)) {
			if (fread(&opth, sizeof(struct aouthdr), 1, lfp) == 1) 
				reg.r_ip = opth.entry;
		}
#endif
		objflag = 1;
		setcoffseg(lfp, &coffh);
		return 1;
	} else {
		fseek(lfp, (long)0, 0);
		if (fread(&ldh, sizeof(ldh), 1, lfp) != 1) {
			printr("Can't read object file");
			return 0;
		}
		canlout(&ldh);
		if (ldh.l_magic != L_MAGIC) {
			printr("not an object file");
			return 0;
		}
		objflag = 1;
		setaseg(&ldh);
		return 1;
	}
}

/* end of trace2.c */
