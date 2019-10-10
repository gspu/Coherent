/*
 * n2/i386/emit1.c
 * Prologs, etc.
 * i386.
 */

#ifdef   vax
#include "INC$LIB:cc2.h"
#else
#include "cc2.h"
#endif

/* Globals. */
static	SYM	*countp;
ival_t		framesize;
PREGSET		usedregs;
int		hasfloat;

/*
 * Generate code for the ALIGN operation.
 * In BSS, just bump the location counter.
 * Otherwise, write 0 bytes; note that this does not write nops,
 * it would not work right if the alignment bytes were executed.
 */
genalign(align) register int align;
{
	register int n;

	if (align == 0)
		return;			/* ignore ALIGN 0 */
	if (isvariant(VASM)) {
		bput(ALIGN); bput(align);
		return;
	}
	n = dot & (align-1);		/* excess past alignment boundary */
	if (n == 0)
		return;
	n = align - n;			/* bump to next boundary */
	if (dotseg == SBSS)
		dot += n;
	else
		outnzb((sizeof_t)n, (dotseg == SCODE));
}

/*
 * Generate code for a function prolog.
 * The prolog is generally of the form
 *	mov	ebp, esp
 *	sub	ebp, $framesize
 *	push	esi
 *	push	edi
 *	push	ebx
 * with variations for profiling and framesize.
 * Saving the registers after the local adjust allows suppressing the
 * register save/restore if the function does not touch the register.
 */
genprolog()
{
	register int	lab;
	register SYM	*labp;

	hasfloat = 0;
	if (isvariant(VASM)) {
		/* Write assembler. */
		if ((framesize == 0) || (framesize >= 65536L) || isvariant(VPROF)) {
			genone(ZPUSH, A_REBP);
			gentwo(ZMOV, A_REBP, A_RESP);
		}
		if (isvariant(VPROF)) {
			genseg(SBSS);
			bput(LLABEL); iput((ival_t)(lab=newlab()));
			genblock((sizeof_t)12);
			genseg(SCODE);
			gentwo(ZMOV, A_RECX, A_IMM|A_LID, lab);
			genone(ZCALL, A_GID|A_DIR, "_scount");
		}
		if (framesize != 0) {
			if (notvariant(VPROF) && framesize < 65536L)
				gentwo(ZENTER, A_IMM|A_OFFS, framesize, A_IMM|A_OFFS, (ival_t)0);
			else {
				if (framesize <= 4)
					genone(ZPUSH, A_REAX);
				else
					gentwo(ZSUB, A_RESP, A_OFFS|A_IMM, framesize);
			}
		}
		if ((usedregs & BESI) != 0)
			genone(ZPUSH, A_RESI);
		if ((usedregs & BEDI) != 0)
			genone(ZPUSH, A_REDI);
		if ((usedregs & BEBX) != 0)
			genone(ZPUSH, A_REBX);
	} else {
		/* Write bits. */
		if ((framesize == 0) || (framesize >= 65536L) || isvariant(VPROF)) {
			outab(0x55);			/* push ebp */
			outab(0x8B); outab(0xEC);	/* mov ebp, esp */
		}
		if (isvariant(VPROF)) {
			genseg(SBSS);
			labp = llookup(newlab(), 1);
			labp->s_seg = dotseg;
			labp->s_value  = dot;
			genblock((sizeof_t)12);
			genseg(SCODE);
			outab(0xB9);			/* mov ecx, offset lab */
			outxl(labp, 0, 0);
			if (countp == NULL)
				countp = glookup("_scount", 0);
			outab(0xE8);			/* call _scount */
			outxl(countp, 0, 1);
		}
		if (framesize != 0) {
			if (notvariant(VPROF) && framesize < 65536L) {
				outab(0xC8);		/* enter framesize,0 */
				outaw(framesize);
				outab(0);
			} else {
				if (framesize == 4)
					outab(0x50);		/* push eax */
				else {
					if (framesize >= 128)
						outab(0x81);	/* sub, sw=01 */
					else
						outab(0x83);	/* sub, sw=11 */
					outab(0xEC);		/* esp, */
					if (framesize < 128)
						outab(framesize); /* value */
					else
						outal(framesize);
				}
			}
		}
		if ((usedregs & BESI) != 0)
			outab(0x56);			/* push esi */
		if ((usedregs & BEDI) != 0)
			outab(0x57);			/* push edi */
		if ((usedregs & BEBX) != 0)
			outab(0x53);			/* push ebx */
	}
}

/*
 * Generate code for a function epilog.
 * If NDP, the variable 'hasfloat' keeps track of whether the function performs
 * floating point operations.
 * If it does, the epilogue includes an FWAIT to assure completion;
 * otherwise, a store into a local variable could complete after the exit,
 * when the allocated stack space has been reused for something else.
 * The peephole optimizer knows the details of epilogue code generation,
 * because it suppresses a stack adjust which immediately precedes LEAVE;
 * if the generated code here changes, change i386/peep.c/noeffect() too.
 */
genepilog()
{
	if (isvariant(VASM)) {
		if (isvariant(VNDP) && hasfloat) {
#if	0
			if (isvariant(VEMU87))
				genone(ZCALL, A_GID|A_DIR, "emu87");
			else {
#endif
				bput(CODE); bput(ZFWAIT);
#if	0
			}
#endif
		}
		if ((usedregs & BEBX) != 0)
			genone(ZPOP, A_REBX);
		if ((usedregs & BEDI) != 0)
			genone(ZPOP, A_REDI);
		if ((usedregs & BESI) != 0)
			genone(ZPOP, A_RESI);
		bput(CODE); bput(ZLEAVE);
		bput(CODE); bput(ZRET);
	} else {
		if (isvariant(VNDP) && hasfloat) {
#if	0
			if (isvariant(VEMU87))
				outemucall();		/* call emu87 */
			else
#endif
				outfb(0x9B);		/* fwait */
		}
		if ((usedregs & BEBX) != 0)
			outab(0x5B);			/* pop ebx */
		if ((usedregs & BEDI) != 0)
			outab(0x5F);			/* pop edi */
		if ((usedregs & BESI) != 0)
			outab(0x5E);			/* pop esi */
		outab(0xC9);				/* leave */
		outab(0xC3);				/* ret */
	}
	genalign(4);
}

/*
 * Generate the code for a BLOCK item.
 * If in -S mode, output an assembly directive to get the space.
 * If generating binary, bump 'dot' if compiling into BSS;
 * otherwise, output a block of 0 bytes.
 */
genblock(n) register sizeof_t n;
{
	if (isvariant(VASM)) {
		bput(BLOCK); zput(n);
	} else if (dotseg == SBSS)
		dot += n;
	else
		outnzb(n, 0);
}

/* end of n2/i386/emit1.c */
