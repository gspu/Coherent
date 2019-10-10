/*
 * db/db3.c
 * A debugger.
 * Command execution and display routines.
 */

#include "db.h"
#include <signal.h>

/*
 * Execute a command.
 * Return 1 if more commands may be read,
 * 0 to stop reading commands (for :e, :c, :s).
 */
int
command(vp) register VAL *vp;
{
	register int c, sig;

	switch (c = getn()) {

	/* [addr]:a	Print address (default: .) */
	case 'a':
		if (getn() != '\n')
			break;
		putaddr(val_segn(vp), lvalue(vp, dot));
		putx('\n');
		return 1;

	/* [addr]:b[r][cmd]	Set [return] breakpoint */
	case 'b':
		if (!IS_OBJ)
			break;
		if ((c = getn()) != 'r') {
			ungetn(c);
			c = 'b';
		}
		getline("i+.:a\ni+.?i\n:x");
		/* FIX_ME :br only works at current pc, not at specified addr. */
		if ((c == 'r' && bpt_setr(miscbuf) == 0)
		 || (c != 'r' && bpt_set(BBPT, lvalue(vp, dot), 0L, miscbuf) == 0))
			printe("Cannot set breakpoint");
		return 1;

	/* [addr]:c	Continue	 */
	case 'c':
		if (getn() != '\n')
			break;
		else
			c = '\n';
		if (execflag == 0)
			break;
		if (nvalue(vp) == 0)
			set_pc(lvalue(vp, (ADDR_T)0));
		step_mode = SNULL;
		return 0;

	/* [addr]:d[r][s]	Delete breakpoint */
	case 'd':
		if (!IS_OBJ)
			break;
		if ((c = getn()) != 'r' && c != 's') {
			ungetn(c);
			c = 'b';
		}
		if (getn() != '\n')
			break;
		if (bpt_del(c, lvalue(vp, dot)) == 0)
			printe("Cannot delete breakpoint");
		return 1;

	/* [addr]:eargs		Execute */
	case 'e':
		if (nvalue(vp) == 0)
			set_pc(lvalue(vp, (ADDR_T)0));
		getline("");
		return runfile();

	/* :f		Print fault type */
	case 'f':
		if (getn() != '\n')
			break;
		sig = get_sig();
		printx("%s\n", (sig == 0)   ? "no fault"
			     : (sig > NSIG) ? "bad signal number"
			     : signame[sig]);
		return 1;

	/* :h[f]		Print help info */
	case 'h':
		if ((c = getn()) != 'f')
			ungetn(c);
		if (getn() != '\n')
			break;
		helpinfo(c == 'f');
		return 1;

	/* [n]:k	??? send a signal ??? */
	case 'k':
		if (execflag == 0)
			break;
		if (getn() != '\n')
			break;
		if (nvalue(vp) != 0) {
			printe("No signal specified");
			return 1;
		}
		printr("Cannot send %d", (int)rvalue(vp, 0L));
		return 1;

	/* :m		Print segmentation map */
	case 'm':
		if (getn() != '\n')
			break;
		map_print();
		return 1;

	/* case 'n':	Documented but not implemented? */

#ifndef	NOCANON
	/* [expr]:o	Change canon type, UNDOCUMENTED... */
	case 'o':
		if (getn() != '\n')
			break;
		cantype = rvalue(vp, 0L);
		return 1;
#endif

	/* :p		Print breakpoints */
	case 'p':
		if (getn() != '\n')
			break;
		bpt_display();
		return 1;

	/* [expr]:q	Quit	 */
	case 'q':
		if (getn() != '\n')
			break;
		if (rvalue(vp, 1L))
			leave();
		return 1;

	/* :r[N]	Print registers [including NDP] */
	case 'r':
		if ((c = getn()) == 'N')
			c = R_ALL;
		else {
			ungetn(c);
			c = R_GEN;
		}
		if (getn() != '\n')
			break;
		if (reg_flag == R_INVALID)
			break;
		display_reg(c);
		return 1;

	/* [addr][,n]:s[c]	Single step [n times] [over calls] */
	case 's':
		if (execflag == 0)
			break;
		if (nvalue(vp) == 0)
			set_pc(lvalue(vp, (ADDR_T)0));
		step_count = rvalue(&vp[1], 1L);
		if ((c = getn()) != 'c') {
			step_mode = SSTEP;
			ungetn(c);
		} else
			step_mode = SCONT;
		getline("i+.?i");
		step_cmd = save_cmd(step_cmd, miscbuf);
		return 0;

	/* [expr]:t	Call traceback [expr levels] */
	case 't':
		if (getn() != '\n')
			break;
		else
			c = '\n';
		if (reg_flag == R_INVALID)
			break;
		traceback((int)rvalue(vp, 0L));
		return 1;

	/* [expr]:x	 Read and execute stdin */
	case 'x':
		if (getn() != '\n')
			break;
		if (rvalue(vp, 1L)) {
			step_prev = step_mode;
			step_mode = SNULL;
			add_input(IFILE, (char *)stdin);
		}
		return 1;
	}
	printe("Illegal command");
	while (c != '\n')
		c = getn();
	return 1;
}

/*
 * Print out data from segment 's' according to the given format "t1 t2".
 */
char *
conform(sp, s, t1, t2) register char *sp; int s, t1, t2;
{
	register char *lp;
	char c;
	unsigned char uc;
	short sh;
	unsigned short us;
	int  i;
	unsigned int ui;
	long l;
	unsigned char v[3];
	ADDR_T p;
#ifndef	NOFP
	float f;
	double d;
	struct _fpreg fpreg;
#endif

	dbprintf(("conform(... s=%d t1=%c t2=%c)\n", s, t1, t2));
	switch (t1) {

	case 'b':				/* byte */
		if (getb(s, &uc, sizeof(uc)) == 0)
			return NULL;
		modsize = sizeof(uc);

		/* Sign-extend or 0-extend depending on display format. */
		if (t2 != 'd')
			i = ui = (unsigned int)uc;
		else
			i = c = uc;
		sprintf(sp, get_format(t1, t2), i);
		break;
	case 'c':				/* char with escapes */
		if (getb(s, &uc, sizeof(uc)) == 0)
			return NULL;
		modsize = sizeof(uc);
		return printable(sp, uc);
	case 'C':				/* char */
		if (getb(s, &uc, sizeof(uc)) == 0)
			return NULL;
		modsize = sizeof(uc);
		*sp++ = (uc >= 0x20 && uc < 0x7F) ? uc : '.';
		return sp;
#ifndef	NOFP
	case 'f':				/* float */
		if (getb(s, (char *)&f, sizeof(f)) == 0)
			return NULL;
		modsize = sizeof(f);
		sprintf(sp, get_format(t1, t2), f);
		break;
	case 'F':				/* double */
		if (getb(s, (char *)&d, sizeof(d)) == 0)
			return NULL;
		modsize = sizeof(d);
		sprintf(sp, get_format(t1, t2), d);
		break;
	case 'N':				/* NDP fp register */
		if (getb(s, (char *)&fpreg, sizeof(fpreg)) == 0)
			return NULL;
		if (get_fp_reg(&fpreg, &d) == 0)
			return NULL;
		modsize = sizeof(fpreg);
		sprintf(sp, get_format('F', t2), d);
		break;
#endif
	case 'h':
		if (getb(s, (char *)&us, sizeof(us)) == 0)
			return NULL;
		modsize = sizeof(us);

		/* Sign-extend or 0-extend depending on display format. */
		if (t2 != 'd')
			i = ui = us;
		else
			i = sh = us;
		sprintf(sp, get_format(t1, t2), i);
		break;
	case 'i':				/* machine instruction */
		modsize = INLEN;
		return disassemble(sp, s);
	case 'l':				/* long */
		if (getb(s, (char *)&l, sizeof(l)) == 0)
			return NULL;
		modsize = sizeof(l);
		sprintf(sp, get_format(t1, t2), l);
		break;
	case 'p':				/* pointer */
		if (getb(s, (char *)&p, sizeof(p)) == 0)
			return NULL;
		modsize = sizeof(p);
		return cvt_addr(sp, find_seg(p), p);
	case 's':				/* string with escapes */
	case 'S':				/* string */
		lp = &sp[DISSIZE];
		for (;;) {
			if (getb(s, &uc, sizeof(uc)) == 0)
				return NULL;
			if (uc == '\0')
				break;
			if (t1 == 's')
				sp = printable(sp, uc);
			else
				*sp++ = uc;
			if (sp > lp)
				return NULL;
		}
		modsize = sizeof(i);
		return sp;
	case 'v':				/* l3 */
		if (getb(s, (char *)v, sizeof(v)) == 0)
			return NULL;
		modsize = sizeof(v);
		l3tol(&l, v, 1);
		sprintf(sp, get_format(t1, t2), l);
		break;
	case 'w':				/* word */
		if (getb(s, (char *)&us, sizeof(us)) == 0)
			return NULL;
		modsize = sizeof(us);
		sprintf(sp, get_format(t1, t2), (int)us);
		break;
	case 'Y':				/* time */
		modsize = sizeof(i);
		return gettime(sp, s);
	default:
		return NULL;
	}
	return strchr(sp, '\0');
}

/*
 * Print out 'n' data items from segment 's'.
 */
int
display(s, n) int s, n;
{
	register char *sp1, *sp2, *fs;
	register int c, r, t1, t2;
	register char *sp3, *fp;
	register ADDR_T nad, pad, uad;

	dbprintf(("display(%d, %d) dot=%x\n", s, n, dot));
	r = 1;
	t2 = '\0';
	add = nad = pad = uad = dot;
	sp1 = miscbuf;
	sp3 = &sp1[DISSIZE];
	fs = seg_format[s];
	while (n--) {
		fp = fs;
		for (;;) {
			c = *fp++;
			if (isascii(c) && isdigit(c)) {
				r = 0;
				do {
					r = r*10 + c-'0';
					c = *fp++;
				} while (isascii(c) && isdigit(c));
				--fp;
				continue;
			}
			switch (c) {
			case '^':
				add = uad;
				continue;
			case '+':
				add += r;
				r = 1;
				continue;
			case '-':
				add -= r;
				r = 1;
				continue;
			case 'n':
				*sp1 = '\0';
				flushb(nad);
				sp1 = miscbuf;
				nad = add;
				continue;
			case 'd':
			case 'o':
			case 'u':
			case 'x':
				t2 = c;
				continue;
			default:
				t1 = c;
				if (t1=='\0' && t2=='\0')
					break;
				if (t1 == '\0')
					t1 = 'w';
				if (t2 == '\0')
					t2 = DDCHR;
				uad = add;
				while (r--) {
					if (testint())
						return 1;
					*sp1++ = ' ';
					pad = add;
					sp2 = conform(sp1, s, t1, t2);
					if (sp2 == NULL) {
						*--sp1 = '\0';
						if (sp1 != miscbuf)
							flushb(nad);
						dbprintf(("add=0x%lX cseg=%d\n", add, cseg));
						return printe("Addressing error");
					}
					if (sp2 <= sp3)
						sp1 = sp2;
					else {
						*--sp1 = '\0';
						flushb(nad);
						*sp2 = '\0';
						*sp1 = ' ';
						sp2 = sp1;
						sp1 = miscbuf;
						while (*sp2)
							*sp1++ = *sp2++;
						nad = pad;
					}
					if (c == 'i') {
						*sp1++ = '\0';
						flushb(nad);
						sp1 = miscbuf;
						nad = add;
					}
				}
				r = 1;
				t2 = '\0';
				if (c != '\0')
					continue;
			}
			break;
		}
		old_add = add;
	}
	if (sp1 != miscbuf) {
		*sp1 = '\0';
		flushb(nad);
	}
	return 1;
}

/*
 * Execute a command string.
 */
void
execute(cmd) char *cmd;
{
	register INP *ip;

	dbprintf(("execute(%s)\n", cmd));

	/* Delete input list starting at "inpp". */
	while ((ip = inpp) != (INP *)NULL) {
		inpp = ip->i_next;
		nfree(ip);
	}
	ungotc = '\0';			/* clear ungot character */

	/* Muck with some globals. */
	modsize = sizeof(int);
	step_prev = SNULL;
	dot = get_pc();

	/* Add string "cmd" to the input list and process it. */
	if (cmd != NULL) {
		add_input(ICORE, cmd);
		request();
	}

	/* Check for interrupt. */
	testint();
}

/*
 * Flush the output buffer.
 * Print the given address 'addr' before the contents of the buffer
 * and set dot to it.
 */
void
flushb(addr) ADDR_T addr;
{
	dot = addr;
	printx(addr_fmt, dot);
	printx("\t%s\n", miscbuf);
}

/*
 * Display help info.
 * The flag is false for :h, true for :hf.
 */
void
helpinfo(flag) int flag;
{
	printf(flag
	 ?	"[addr][,n]?[ft]\tDisplay formatted information.\n"
		"Display formats:\n"
		"\tb\tbyte\n"
		"\tc\tchar, control and non-chars escaped\n"
		"\tC\tchar, control and non-chars print as '.'\n"
		"\td\tdecimal\n"
		"\tf\tfloat\n"
		"\tF\tdouble\n"
		"\ti\tdisassembled machine instruction\n"
		"\tl\tlong\n"
		"\tn\toutput '\\n'\n"
		"\tN\tNDP (80387) floating point register (10 bytes)\n"
		"\to\toctal\n"
		"\tp\tsymbolic address\n"
		"\ts\tstring (NUL-terminated) with escapes\n"
		"\tS\tstring (NUL-terminated)\n"
		"\tu\tunsigned\n"
		"\tv\tfilesystem l3 block address (3 bytes)\n"
		"\tw\tword\n"
		"\tx\thexadecimal\n"
		"\tY\ttime\n"
		"[doux] specify numeric bases (decimal, octal, unsigned decimal, hexadecimal).\n"
		"Each may be followed by [bwl] to indicate a datum size [byte, word, long].\n"
	 :	"Requests:\n"
		"\t[addr][,n]?[ft]\tDisplay formatted information.\n"
		"\t\t\tFormats: bcCdfFilnNopsSuvwxY [see :hf for details]\n"
		"\taddr?\t\tPrint address\n"
		"\t[addr]=\t\tPrint address\n"
		"\t[addr]=val...\tPatch address with val...\n"
		"\t?\t\tPrint last error message\n"
		"\t!cmd\t\tSend cmd to system\n"
		"\t[addr]:a\tPrint address\n"
		"\t[addr]:b[cmd]\tSet breakpoint [to execute cmd]\n"
		"\t:br[cmd]\tSet return breakpoint [to execute cmd]\n"
		"\t[addr]:c\tContinue [from addr]\n"
		"\t[addr]:d[r][s]\tDelete breakpoint\n"
		"\t[addr]:e args\tExecute\n"
		"\t:f\t\tPrint fault type\n"
		"\t:h[f]\t\tPrint help information [about display formats]\n"
/*		"\t:k\t\t???\n"	*/
		"\t:m\t\tPrint segmentation map\n"
#ifndef	NOCANON
		"\t[expr]:o\t\tSet canonization type\n"
#endif
		"\t:p\t\tPrint breakpoints\n"
		"\t[expr]:q\tQuit\n"
		"\t:r[N]\t\tPrint registers [including NDP]\n"
		"\t[addr][,n]:s[c]\tSingle step [over calls]\n"
		"\t[expr]:t\tPrint traceback [to expr levels]\n"
		"\t[expr]:x\tRead commands from stdin\n"
	);
}

/*
 * Get a long representing the current time and convert it.
 */
char *
gettime(sp, s) register char *sp; int s;
{
	long l;

	if (getb(s, (char *)&l, sizeof(l)) == 0)
		return NULL;
	memcpy(sp, ctime(&l), 24);
	return sp + 24;
}

/*
 * If the given character is unprintable,
 * convert it into an escape sequence.
 */
char *
printable(sp, c) register char *sp; register int c;
{
	*sp++ = '\\';
	switch (c) {
	case '\0':	*sp++ = '0';		break;
	case '\b':	*sp++ = 'b';		break;
	case '\n':	*sp++ = 'n';		break;
	case '\f':	*sp++ = 'f';		break;
	case '\r':	*sp++ = 'r';		break;
	case '\\':	*sp++ = '\\';		break;
	default:
		if (c<040 || c>=0177) {
			sprintf(sp, "%03o", c);
			while (*sp)
				sp++;
			break;
		}
		sp[-1] = c;
		break;
	}
	return sp;
}

/*
 * Talk to the user and try to solve his problems.
 */
void
process()
{
	static char *xcmd = ":x\n";
	static char *fxcmd = ":f\n:x\n";
	register BPT *bp;
	register ADDR_T pc, fp;
	register int n;
	register int bpt_next, bpt_skipped;
	int step_flag;			/* 0==not step, 1==step, 2==syscall */

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
	 *       Execute single step command string (step_cmd).
	 *       If last single step in count
	 *         execute(":x\n")
	 *   end forever
	 * end forever
	 *
	 */
top:
	execute(xcmd);
	for (;;) {

		/*
		 * If in single step mode, set up for single step.
		 * If in SCONT mode (for :sc),
		 * change to SCALL mode at call instruction.
		 */
		if (step_mode != SNULL && step_mode != SWAIT) {
			/* Single stepping. */
			step_flag = 1;
			if (step_mode == SCONT && is_call())
				step_mode = SCALL;
		} else
			step_flag = 0;

		/* Place all breakpoints. */
		if (reg_flag != R_INVALID)
			pc = get_pc();
		for (bpt_skipped = 0, bp = &bpt[0]; bp < &bpt[NBPT]; bp++) {

			/* Skip unused table entries. */
			if (bp->b_flag == 0)
				continue;

			/*
			 * If there is a breakpoint at the current PC,
			 * we do not want to install it, or we'll get nowhere.
			 * But if we are not single-stepping, we really want
			 * the breakpoint reinstalled after doing a single step.
			 * Kludge the mode to SSTEP and set 'bpt_skipped' in
			 * this case so SNULL mode gets restored after a step.
			 */
			if (reg_flag!=R_INVALID && bp->b_badd == pc) {
				if (step_mode == SNULL) {
					step_mode = SSTEP;
					bpt_skipped = step_flag = 1;
				}
				continue;
			}

			/* Install a breakpoint. */
			add = bp->b_badd;
			dbprintf(("setting breakpoint @ %lX\n", add));
			if (putb(ISEG, bin, sizeof(BIN)) == 0) {
				printb(bp->b_badd);
				goto err;
			}
		}

		/* Set 'bpt_next' if next instruction is a breakpoint. */
		bpt_next = (reg_flag!=R_INVALID && bpt_test(pc));

		/* Watch out for system calls if single stepping. */
		if (is_syscall(&step_flag) == 0)
			goto err;

		/* Run the child. */
		if (runc() == 0)
			goto err;

		/* Restore if we have stepped past system call. */
		if (step_flag == 2 && rest_syscall(&step_flag) == 0)
			goto err;

		/* Replace breakpoints with instructions. */
		dbprintf(("Replace breakpoints with instructions\n"));
		for (bp = &bpt[0]; bp < &bpt[NBPT]; bp++) {
			if (bp->b_flag == 0)
				continue;		/* unused */
			dbprintf(("Restore instruction %X in bpt[%d]\n", bp->b_bins[0] & 0xFF, bp - bpt));
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
		if (get_sig() != SIGTRAP) {
			if (get_sig() != SIGINT)
				printr("Traced process did not stop at a breakpoint");
			printf("Traced process: ");
			execute(fxcmd);
			continue;
		}

		/* Resume running following skipped breakpoint. */
		if (bpt_skipped) {
			step_mode = SNULL;
			continue;
		}

		/*
		 * Find the breakpoint we are at.
		 * Back up pc to start of the breakpoint.
		 */
		bp = (BPT *)NULL;
		pc = get_pc() - sizeof(BIN);
		dbprintf(("find bpt: --pc=%x step_flag=%d bpt_next=%d\n", pc, step_flag, bpt_next));
		if (step_flag == 0 || bpt_next) {
			for (bp = &bpt[0]; bp < &bpt[NBPT]; bp++) {
				if (bp->b_flag == 0)
					continue;
				if (bp->b_badd != pc)
					continue;
				dbprintf(("found bpt[%d] at %x\n", bp-bpt, pc));
				set_pc(pc);
				break;
			}
			if (bp == &bpt[NBPT])
				bp = (BPT *)NULL;	/* unknown */
		}

		/* If in single step mode, execute command. */
		dbprintf(("step_mode=%d\n", step_mode));
		switch (step_mode) {
		case SSTEP:
		case SCONT:
			execute(step_cmd);
			if (--step_count == 0)
				execute(xcmd);
			continue;
		case SCALL:
			/*
			 * :sc has stepped into call with step_mode SCALL.
			 * Clear other BSIN breakpoints, place a BSIN breakpoint
			 * at the return address, and set step_mode SWAIT
			 * to run until the return is executed.
			 */
			for (n = 0; n < NBPT; n++)
				bpt[n].b_flag &= ~BSIN;
			step_mode = SWAIT;
			set_ra_bpt();		/* set BSIN bpt at return address */
			break;
		}

		/*
		 * Handle an unexpected trace trap or unknown breakpoint.
		 * One way this can happen: if the child does not handle
		 * SIGINT, type "100:s" followed by "<Ctrl-C>" and ":c".
		 * The child is interrupted, the parent prints "Interrupted"
		 * and prompts, ":c" resumes the child with SNULL (step_flag 0),
		 * and the child then hits the actual breakpoint.
		 * t
		 */
		if (bp == (BPT *)NULL) {
			dbprintf(("step_mode=%d step_flag=%d bpt_next=%d\n", step_mode, step_flag, bpt_next));
			if (step_flag == 0 || bpt_next) {
				printf("Unexpected ");
				execute(fxcmd);
			}
			continue;
		}

		/* Single step breakpoints have highest priority. */
		fp = get_fp();
		if (bp->b_flag & BSIN) {
			if (bp->b_sfpt==0 || bp->b_sfpt == fp) {
				bp->b_flag &= ~BSIN;
				if (step_mode == SWAIT) {
					/*
					 * We hit the return from a called
					 * function with :sc, restore SCONT mode.
					 */
					step_mode = SCONT;
					execute(step_cmd);
					if (--step_count == 0)
					       execute(xcmd);
					continue;
				}
			}
		}

		/* Return breakpoints are next. */
		if (bp->b_flag & BRET) {
			if (bp->b_rfpt == fp) {
				bp->b_flag &= ~BRET;
				execute(bp->b_rcom);
				continue;
			}
		}

		/* Your conventional everyday ordinary breakpoint. */
		if (bp->b_flag & BBPT) {
			execute(bp->b_bcom);
			continue;
		}
	}

	/*
	 * Something is terribly wrong.
	 * Kill off our child and generally reset everything to the start.
	 */
err:
	killc();
	initialize();
	map_init();
	if (IS_LOUT)
		setloutseg();
	else if (IS_COFF)
		setcoffseg();
	goto top;
}

/*
 * Parse requests and execute them.
 * This is a loop which eats input and process it,
 * using command() to execute commands.
 * It returns when command() returns 0 or on '\n' for single step.
 */
void
request()
{
	static int ttyflag = -1;
	register int c;
	register char *cp;
	register unsigned segn;
	register ADDR_T l, d;
	VAL val[VALSIZE];

	for(;;) {
		/* Prompt if appropriate. */
		if (ttyflag == -1)
			ttyflag = isatty(fileno(stdin));
		if (ttyflag && inpp->i_type==IFILE && inpp->i_u.i_filp==stdin) {
			printf(prompt);
			fflush(stdout);
		}
		if ((c = getn()) == EOF)
			break;

		/* !cmd		Execute cmd */
		if (c == '!') {
			syscall();
			continue;
		}

		/* ?		Print fault type */
		if (c == '?') {
			if ((c = getn()) != '\n') {
				printe("Syntax error");
				goto next;
			}
			printr("%s", (lasterr == NULL) ? "No error" : lasterr);
			continue;
		}
		ungetn(c);
		if (expr_list(val) == 0)		/* get expression list */
			goto next;

		switch (c = getn()) {

		/* : indicates commands, handled by command(). */
		case ':':
			step_prev = SNULL;
			if (command(val) == 0)
				return;
			continue;

		/* \n indicates more of the same, single step or display */
		case '\n':
			if (nvalue(&val[0]) && step_prev != SNULL) {
				if (execflag == 0) {
					step_prev = SNULL;
					printe("Cannot single step");
					continue;
				}
				step_mode = step_prev;
				step_count = rvalue(&val[1], 1L);
				return;
			}
			/* else fall through to print value */

		/* addr\n	Print value */
		/* [addr]=val	Assign value */
		/* [addr]?	Print value */
		case '=':
			ungetn(c);
			/* fall through... */
		case '?':
			step_prev = SNULL;
			segn = val_segn(&val[0]);
			if ((c = getn()) == '=') {
				l = lvalue(&val[0], dot);
				if ((c = getn()) == '\n') {
					dbprintf(("print seg=%d loc=%lx fmt=%s\n", segn, l, addr_fmt));
					printx(addr_fmt, l);
					printx("\n");
					continue;
				}
				ungetn(c);
				dbprintf(("assign to seg=%d loc=%lx\n", segn, l));
				if (setdata(segn, l) == 0)
					break;
				continue;
			}
			if (c != '\n') {
				cp = &seg_format[segn][0];
				while (c != '\n') {
					*cp++ = c;
					c = getn();
				}
				*cp++ = '\0';
				dbprintf(("new seg_format[%d]=%s\n", segn, seg_format[segn]));
			}
			d = dot;
			l = old_add;
			dot = lvalue(&val[0], old_add);
			display(segn, (int)rvalue(&val[1], 1L));
			if (segn != USEG)
				cseg = segn;	/* set cseg unless USEG */
			else {
				dot = d;	/* restore dot, old_add if USEG */
				old_add = l;
			}
			continue;

		default:
			step_prev = SNULL;
			printe("Syntax error");
			break;
		}
	next:
		while ((c = getn()) != '\n')
			;
	}
}

/*
 * Parse the command line in 'miscbuf',
 * kill the current child and start up a new one.
 * Return 0 on success, 1 on failure.
 */
int
runfile()
{
	register char *bp, *cp;
	register int c;
	char *ifn, *ofn, *argl[ARGSIZE];
	int qflag, aflag, n;

	killc();
	if (!IS_OBJ) {
		printe("No executable");
		return 1;
	}
	ifn = ofn = NULL;
	aflag = qflag = n = 0;
	for (bp = cp = miscbuf, c = *bp++; c != '\n'; ) {
		switch (c) {
		case '<':
			ifn = cp;
			c = *bp++;
			break;
		case '>':
			ofn = cp;
			if ((c = *bp++) == '>') {
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
			printe("Missing '\"'");
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
	return startc(argl, ifn, ofn, aflag) == 0;
}

/*
 * Given a pointer to an allocated buffer and a pointer to a command string,
 * copy the command string to the allocated buffer.
 * If the buffer hasn't been allocated, allocate it.
 */
char *
save_cmd(buf, s) register char *buf, *s;
{
	if (buf == NULL)
		buf = nalloc(COMSIZE, "command buffer");
	buf[COMSIZE-1] = '\0';
	return strncpy(buf, s, COMSIZE-1);
}

/*
 * Change the value of a location.
 */
int
setdata(segn, a) int segn; ADDR_T a;
{
	register char *cp;
	register int n;
	register int c;
	char b[1];
	char l3[3];
	int i[1];
	long l[1];
	VAL val[VALSIZE];

	if (expr_list(val) == 0)
		return 0;
	if ((c = getn()) != '\n')
		return 0;
	for (n = 0; n < VALSIZE; n++, a += modsize) {
		if (nvalue(&val[n]))
			continue;
		l[0] = rvalue(&val[n], 0L);
		switch (modsize) {
		case (sizeof(char)):
			b[0] = l[0];
			cp = b;
			break;
		case (sizeof(short)):
			i[0] = l[0];
			cp = (char *)i;
			break;
		case (sizeof(l3)):
			ltol3(l3, l, 1);
			cp = l3;
			break;
		case (sizeof(long)):
			cp = (char *)l;
			break;
		default:
			printe("Bad change type");
			return 1;
		}
		add = a;
		if (putb(segn, cp, modsize) == 0) {
			printe("Cannot change value");
			return 1;
		}
	}
	if (segn == USEG) {
		/* Reread required registers after writing register data. */
		reg_flag = R_INVALID;
		get_regs(R_SOME);
	}
	return 1;
}

/*
 * Send a command to the shell.
 * Return its exit status.
 */
int
syscall()
{
	register int c, status;
	register char *cp;

	for (cp = miscbuf; (c = getn()) != '\n'; )
		*cp++ = c;
	*cp = '\0';
	status = system(miscbuf);
	testint();
	printf("!\n");
	return status;
}

/* end of db/db3.c */
