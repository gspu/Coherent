/* $Header: /src386/bin/db/RCS/i8086d.c,v 1.4 93/03/11 07:45:48 bin Exp Locker: bin $
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
#include <sys/param.h>
#include <l.out.h>
#include "trace.h"
#include "i8086.h"


/* 
 * Legend to characters following "%" tokens in strings - Taken from intel
 * 386 "Programmer's Reference Manual".
 */

/*	First char after '%'; see also 2nd char, general case below:
		A - direct address
		C - reg of r/m selects control register
		D - reg of r/m selects debug register
		E - modR/M selects operand
		F - flags register
		G - reg field of modR/M selects general register
		I - immediate data
		J - relative offset added to IP
		K - as J, except find symbol, if possible
		M - modR/M selects memory only
		O - no modR/M byte; offset only
		R - mod field of modR/M selects register only
		S - reg field of modR/M selects segment register
		T - reg feild of modR/M selects test register
		X - memory addressed by DS:ESI
		Y - memory addressed by ES:EDI
		Z - assembler instruction mneumonic size qualifier
		2 - prefix of two-byte opcode

	For the following first characters after the %, the second after the %
	has a special meaning associated with it:
		e - put in 'e' if address size attribute = 32 
			- second char is part of reg name
		f - floating point 
			- second char is esc value
		g - REG/OPCODE field of the modR/M byte is interpreted as
		    additional opcode; it is used as an index to a
		    column in the 2-d group array (grp[][])
			- second character yields the group number, i.e., it 
			  is used as an index to a row in grp[][]  
		r - read next n bytes, but don't do anything with them; n is 
		    given by 2nd char and meaning conforms with general case
		s - size override:
			- second char has following meanings:
				"a" - address size override
				"o" - operand size override

	Second char after '%', the general case:
		a - two words or dwords in memory (BOUND)
		b - byte
		d - dword
		f - 32 bit pointer
		l - 48 bit pointer
		p - 32 or 48 bit pointer
		s - six byte pseudo-descriptor
		v - word or dword
		w - word
		z - r/m16: append 'w' to assem. nmeumonic
		  - r/m32: append 'w' to assem. nmeumonic if operand size = 16,
			   append 'l' to assem. nmeumonic if operand size = 32
!!!!!!!!!!!
		F - use floating regs in mod/rm
		1-8 - group number
*/

char *opStrMap1[] = {
	/* 0 */
	"add%Zb %Eb,%Gb", "add%Zv %Ev,%Gv", "add%Zb %Gb,%Eb", "add%Zv %Gv,%Ev",
	"add%Zb al,%Ib", "add%Zv %eax,%Iv", "pushw es", "popw es",
	"or%Zb %Eb,%Gb", "or%Zv %Ev,%Gv", "or%Zb %Gb,%Eb", "or%Zv %Gv,%Ev",
	"or%Zb al,%Ib", "or%Zv %eax,%Iv", "pushw cs", "%2 ",

	/* 1 */
	"adc%Zb %Eb,%Gb", "adc%Zv %Ev,%Gv", "adc%Zb %Gb,%Eb", "adc%Zv %Gv,%Ev",
	"adc%Zb al,%Ib", "adc%Zv %eax,%Iv", "pushw ss", "popw ss",
	"sbb%Zb %Eb,%Gb", "sbb%Zv %Ev,%Gv", "sbb%Zb %Gb,%Eb", "sbb%Zv %Gv,%Ev",
	"sbb%Zb al,%Ib", "sbb%Zv %eax,%Iv", "pushw ds", "popw ds",

	/* 2 */
	"and%Zb %Eb,%Gb", "and%Zv %Ev,%Gv", "and%Zb %Gb,%Eb", "and%Zv %Gv,%Ev",
	"and%Zb al,%Ib", "and%Zv %eax,%Iv", "es:", "daa",
	"sub%Zb %Eb,%Gb", "sub%Zv %Ev,%Gv", "sub%Zb %Gb,%Eb", "sub%Zv %Gv,%Ev",
	"sub%Zb al,%Ib", "sub%Zv %eax,%Iv", "cs:", "das",

	/* 3 */
	"xor%Zb %Eb,%Gb", "xor%Zv %Ev,%Gv", "xor%Zb %Gb,%Eb", "xor%Zv %Gv,%Ev",
	"xor%Zb al,%Ib", "xor%Zv %eax,%Iv", "ss:", "aaa",
	"cmp%Zb %Eb,%Gb", "cmp%Zv %Ev,%Gv", "cmp%Zb %Gb,%Eb", "cmp%Zv %Gv,%Ev",
	"cmp%Zb al,%Ib", "cmp%Zv %eax,%Iv", "ds:", "aas",

	/* 4 */
	"inc%Zv %eax", "inc%Zv %ecx", "inc%Zv %edx", "inc%Zv %ebx",
	"inc%Zv %esp", "inc%Zv %ebp", "inc%Zv %esi", "inc%Zv %edi",
	"dec%Zv %eax", "dec%Zv %ecx", "dec%Zv %edx", "dec%Zv %ebx",
	"dec%Zv %esp", "dec%Zv %ebp", "dec%Zv %esi", "dec%Zv %edi",

	/* 5 */
	"push%Zv %eax", "push%Zv %ecx", "push%Zv %edx", "push%Zv %ebx",
	"push%Zv %esp", "push%Zv %ebp", "push%Zv %esi", "push%Zv %edi",
	"pop%Zv %eax", "pop%Zv %ecx", "pop%Zv %edx", "pop%Zv %ebx",
	"pop%Zv %esp", "pop%Zv %ebp", "pop%Zv %esi", "pop%Zv %edi",

	/* 6 */
	"pusha%Zv", "popa%Zv", "bound%Zv %Gv,%Ma", "arpl %Ew,%Gw",
	"fs:", "gs:", "%so", "%sa",
	"push%Zv %Iv", "imul%Zv %Gv=%Ev*%Iv", "push%Zb %Ib", "imul%Zv %Gv=%Ev*%Ib",
	"ins%Zb %Yb", "ins%Zv %Yv", "outs%Zb %Xb", "outs%Zv %Xv",

	/* 7 */
	"jo %Jb", "jno %Jb", "jb %Jb", "jae %Jb",
	"je %Jb", "jne %Jb", "jbe %Jb", "ja %Jb",
	"js %Jb", "jns %Jb", "jp %Jb", "jnp %Jb",
	"jl %Jb", "jge %Jb", "jle %Jb", "jg %Jb",

	/* 8 */
	"%g1%Zb %Eb,%Ib", "%g1%Zv %Ev,%Iv", "mov%Zb al,%Ib", "%g1%Zv %Ev,%Ib",
	"test%Zb %Eb,%Gb", "test%Zv %Ev,%Gv", "xchg%Zb %Eb,%Gb", "xchg%Zv %Ev,%Gv",
	"mov%Zb %Eb,%Gb", "mov%Zv %Ev,%Gv", "mov%Zb %Gb,%Eb", "mov%Zv %Gv,%Ev",
	"mov%Zw %Ew,%Sw", "lea%Zv %Gv,%M ", "mov%Zw %Sw,%Ew", "pop%Zv %Ev",

	/* 9 */
	"nop", "xchg%Zv %eax,%ecx", "xchg%Zv %eax,%edx", "xchg%Zv %eax,%ebx",
	"xchg%Zv %eax,%esp", "xchg%Zv %eax,%ebp", 
					"xchg%Zv %eax,%esi", "xchg%Zv %eax,%edi",
	"cbw", "cwd", "lcall %Ap", "fwait",
	"pushf%Zv", "popf%Zv", "sahf", "lahf",

	/* A */
	"mov%Zb al,%Ob", "mov%Zv %eax,%Ov", "mov%Zb %Ob,al", "mov%Zv %Ov,%eax",
	"movs%Zb %Xb%Yb", "movs%Zz %Xv%Yv", "cmps%Zb %Xb%Yb", "cmps%Zz %Xv%Yv",
	"test%Zb al,%Ib", "test%Zv %eax,%Iv", "stos%Zb %Yb", "stos%Zz %Yv",
	"lods%Zb %Xb", "lods%Zz %Xv", "scas%Zb %Xb", "scas%Zv %Xv",

	/* B */
	"mov%Zb al,%Ib", "mov%Zb cl,%Ib", "mov%Zb dl,%Ib", "mov%Zb bl,%Ib",
	"mov%Zb ah,%Ib", "mov%Zb ch,%Ib", "mov%Zb dh,%Ib", "mov%Zb bh,%Ib",
	"mov%Zv %eax,%Iv", "mov%Zv %ecx,%Iv", "mov%Zv %edx,%Iv", "mov%Zv %ebx,%Iv",
	"mov%Zv %esp,%Iv", "mov%Zv %ebp,%Iv", "mov%Zv %esi,%Iv", "mov%Zv %edi,%Iv",

	/* C */
	"%g2%Zb %Eb,%Ib", "%g2%Zv %Ev,%Ib", "ret %Iw", "ret",
	"les%Zv %Gv,%Mp", "lds%Zv %Gv,%Mp", "mov%Zb %Eb,%Ib", "mov%Zv %Ev,%Iv",
	"enter %Iw,%Ib", "leave", "lret %Iw", "lret",
	"int 3", "int %Ib", "into", "iret",

	/* D */
	"%g2%Zb %Eb,$1", "%g2%Zv %Ev,$1", "%g2%Zb %Eb,cl", "%g2%Zv %Ev,cl",
	"aam %rb", "aad %rb", NULL, "xlat",
#if 0
	"esc 0,%Ib", "esc 1,%Ib", "esc 2,%Ib", "esc 3,%Ib",
	"esc 4,%Ib", "esc 5,%Ib", "esc 6,%Ib", "esc 7,%Ib",
#else
	"%f0", "%f1", "%f2", "%f3",
	"%f4", "%f5", "%f6", "%f7",
#endif

	/* E */
	"loopne %Jb", "loope %Jb", "loop %Jb", "jcxz %Jb",
	"in%Zb %Ib", "in%Zv %Ib", "out%Zb %Ib,al", "out%Zv %Ib,%eax",
	"call %Kv", "jmp %Jv", "ljmp %Ap", "jmp %Jb",
	"in%Zb ", "in%Zv ", "out%Zb dx,al", "out%Zv dx,%eax",

	/* F */
	"lock", NULL, "repne", "rep",
	"hlt", "cmc", "%g3", "%g0",
	"clc", "stc", "cli", "sti",
	"cld", "std", "%g4", "%g5"
};

/******************************************************************************/
char *opStrMap2[] = {
	/* 0 */
	"%g6", "%g7", "lar%Zv %Gv,%Ew", "lsl%Zv %Gv,%Ew", 
	NULL, NULL, "clts", NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 1 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 2 */
	"mov%Zd %Rd,%Cd", "mov%Zd %Rd,%Dd", "mov%Zd %Cd,%Rd", "mov%Zd %Dd,%Rd",
	"mov%Zd %Rd,%Td", NULL, "mov%Zd %Td,%Rd", NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 3 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 4 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 5 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 6 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 7 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* 8 */
	"jo %Jv", "jno %Jv", "jb %Jv", "jae %Jv",
	"je %Jv", "jne %Jv", "jbe %Jv", "ja %Jv",
	"js %Jv", "jns %Jv", "jp %Jv", "jnp %Jv",
	"jl %Jv", "jge %Jv", "jle %Jv", "jg %Jv",

	/* 9 */
	"seto %Eb", "setno %Eb", "setb %Eb", "setae %Eb",
	"sete %Eb", "setne %Eb", "setbe %Eb", "seta %Eb",
	"sets %Eb", "setns %Eb", "setp %Eb", "setnp %Eb",
	"setl %Eb", "setge %Eb", "setle %Eb", "setg %Eb",

	/* A */
	"pushw fs", "popw fs", NULL, "bt%Zv %Ev,%Gv",
	"shld%Zv %Ev,%Gv,%Ib", "shld%Zv %Ev,%Gv,cl", NULL, NULL,
	"pushw gs", "popw gs", NULL, "bts%Zv %Ev,%Gv",
	"shrd%Zv %Ev,%Gv,%Ib", "shrd%Zv %Ev,%Gv,cl", NULL, "imul%Zv %Gv,%Ev",

	/* B */
	NULL, NULL, "lss%Zv %Mp", "btr%Zv %Ev,%Gv",
	"lfs%Zv %Mp", "lgs%Zv %Mp", "movzx%Zb %Gv,%Eb", "movzx%Zw %Gv,%Ew",
	NULL, NULL, "%g8%Zv %Ev,%Ib", "btc%Zv %Ev,%Gv",
	"bsf%Zv %Gv,%Ev", "bsr%Zv %Gv,%Ev", "movsx%Zb %Gv,%Eb", "movsx%Zw %Gv,%Ew",

	/* C */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* D */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* E */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};


/******************************************************************************/
/*
 *	Intel groups 1 to 8: grpStrMap0[] is the %Ev case for grpStrMap3[]  
 */

char	*grpStrMap0[] = 
	{ "test%Zv %Ev,%Iv", "test%Zv %Ev,%Iv,", "not%Zv %Ev", "neg%Zv %Ev",
	"mul%Zv %eax,%Ev", "imul%Zv %eax,%Ev", "div%Zv %eax,%Ev", "idiv%Zv %eax,%Ev" };

/* grp[1][] */
char	*grpStrMap1[] = 
	{ "add", "or", "adc", "sbb", "and", "sub", "xor", "cmp" };

/* grp[2][] */
char	*grpStrMap2[] = 
	{ "rol", "ror", "rcl", "rcr", "shl", "shr", "shl", "sar" };

/* grp[3][] */
char	*grpStrMap3[] = 
	{ "test%Zb %Eb,%Ib", "test%Zb %Eb,%Ib,", "not%Zb %Eb", "neg%Zb %Eb",
	  "mul%Zb al,%Eb", "imul%Zb al,%Eb", "div%Zb al,%Eb", "idiv%Zb al,%Eb" };

/* grp[4][] */
char	*grpStrMap4[] = 
	{ "inc%Zb %Eb", "dec%Zb %Eb", NULL, NULL, NULL, NULL, NULL, NULL };

/* grp[5][] */
char	*grpStrMap5[] = 
	{ "inc%Zv %Ev", "dec%Zv %Ev", "icall %Ev", "ilcall %Ep",
	  "ijmp %Ev", "iljmp %Ep", "push%Zv %Ev", NULL };

/* grp[6][] */
char	*grpStrMap6[] = 
	{ "sldt %Ew", "str %Ew", "lldt %Ew", "ltr %Ew",
	  "verr %Ew", "verw %Ew", NULL, NULL };

/* grp[7][] */
char	*grpStrMap7[] = 
	{ "sgdt %Ms", "sidt %Ms", "lgdt%Zv %Ms", "lidt%Zv %Ms",
	  "smsw %Ew", NULL, "lmsw %Ew", NULL };

/* grp[8][] */
char	*grpStrMap8[] = 
	{ NULL, NULL, NULL, NULL, "bt", "bts", "btr", "btc" };

char	**grpStrMap[] =	{ grpStrMap0, grpStrMap1, grpStrMap2, grpStrMap3,
			  grpStrMap4, grpStrMap5, grpStrMap6, grpStrMap7,
			  grpStrMap8 };


/******************************************************************************/
/* 
 * Define arrays of the various 80386 registers
 */

/* segment registers */
char	*segReg[NSEGREGS] = {"es", "cs", "ss", "ds", "fs", "gs"};

/* general registers */
char	*genReg8[] = {"%al", "%cl", "%dl", "%bl"
					, "%ah", "%ch", "%dh", "%bh"};
char	*genReg16[] = {"%ax", "%cx", "%dx", "%bx"
					, "%sp", "%bp", "%si", "%di"};
char	*genReg32[] = {"%eax", "%ecx", "%edx", "%ebx"
					, "%esp", "%ebp", "%esi", "%edi"};
char	**genReg[] = {genReg8, genReg16, genReg32};

/* control registers */
char *ctrlReg[NCTRLREGS] = {
	"%cr0", "%cr1", "%cr2", "%cr3"	
};

/* debug registers */
char *dbgReg[NDBGREGS] = {
	"%dr0", "%dr1", "%dr2", "%dr3", "%dr4", "%dr5", "%dr6", "%dr7"	
};

/* test registers */
char *tstReg[NTSTREGS] = {
	NULL, NULL, NULL, NULL, NULL, NULL, "%tr6", "%tr7"	
};


/******************************************************************************/
/*
 * Indirect mode tables (including indexing for the 16 bit addressing form)
 */

/* 16 bit indirect addressing forms with the ModR/M byte */
char	*modRMtab16[] =	{
		"(%bx)(%si)", "(%bx)(%di)", "(%bp)(%si)", "(%bp)(%di)",
		"(%si)", "(%di)", "(%bp)", "(%bx)"			
};

/* 32 bit indirect addressing forms with the ModR/M byte
   or indirect base addressing with the SIB byte */
char	*modRMtab32[] =	{
		"(%eax)", "(%ecx)", "(%edx)", "(%ebx)", 
		"(%esp)", "(%ebp)", "(%esi)", "(%edi)"		
};

char **modRMtab[] = { modRMtab16, modRMtab32 };

/******************************************************************************/
/*
 * 32 bit indirect indexed addressing forms with the SIB byte 
 */

/* index multiplied by 1 */
char	*sibtab0[] = {
		"(%eax)", "(%ecx)", "(%edx)", "(%ebx)", 
		 NULL    , "(%ebp)", "(%esi)", "(%edi)" 
};

/* index multiplied by 2 */
char	*sibtab1[] = {
		"(%eax*2)", "(%ecx*2)", "(%edx*2)", "(%ebx*2)",
		 NULL      , "(%ebp*2)", "(%esi*2)", "(%edi*2)"
};

/* index multiplied by 4 */
char	*sibtab2[] = {
		"(%eax*4)", "(%ecx*4)", "(%edx*4)", "(%ebx*4)", 
		 NULL      , "(%ebp*4)", "(%esi*4)", "(%edi*4)"
};

/* index multiplied by 8 */
char	*sibtab3[] = {
		"(%eax*8)", "(%ecx*8)", "(%edx*8)", "(%ebx*8)", 
		 NULL      , "(%ebp*8)", "(%esi*8)", "(%edi*8)"
};

char	**sibtab[] = { sibtab0, sibtab1, sibtab2, sibtab3 };

/*
 * 8087 Instruction table.
 * The bits considered are the last 3 bits of the ESC
 * instruction and the next instruction byte.
 */
INS	fpinstab[] = {
	0x000, 0x138,	"fadd?",
	0x0C0, 0x1F8,	"fadd\tST(i)?",
	0x020, 0x130,	"fsubb?",
	0x0E0, 0x1F0,	"fsub\tST(i)?",
	0x008, 0x138,	"fmul?",
	0x0C8, 0x1F8,	"fmul\tST(i)?",
	0x030, 0x130,	"fdiv?",
	0x0F0, 0x1F0,	"fdiv\tST(i)?",
	0x1FA, 0x7FF,	"fsqrt",
	0x1FD, 0x7FF,	"fscale",
	0x1F8, 0x7FF,	"fprem",
	0x1FC, 0x7FF,	"frndint",
	0x1F4, 0x7FF,	"fxtract",
	0x1E1, 0x7FF,	"fabs",
	0x1E0, 0x7FF,	"fchs",
	0x1F2, 0x7FF,	"fptan",
	0x1F3, 0x7FF,	"fpatan",
	0x1F0, 0x7FF,	"f2xm1",
	0x1F1, 0x7FF,	"fyl2x",
	0x1F9, 0x7FF,	"fyl2p1",
	0x1EE, 0x7FF,	"fldz",
	0x1E8, 0x7FF,	"fld1",
	0x1EB, 0x7FF,	"fldpi",
	0x1E9, 0x7FF,	"fldl2t",
	0x1EA, 0x7FF,	"fldl2e",
	0x1EC, 0x7FF,	"fldlg2",
	0x1ED, 0x7FF,	"fldln2",
	0x3E3, 0x7FF,	"finit",
	0x3E0, 0x77FF,	"feni",
	0x3E1, 0x7FF,	"fdisi",
	0x128, 0x738,	"fldcw?",
	0x138, 0x738,	"fstcw?",
	0x538, 0x738,	"fstsw?",
	0x3E2, 0x7FF,	"fclex",
	0x130, 0x738,	"fstenv?",
	0x120, 0x738,	"fldenv?",
	0x530, 0x738,	"fsave?",
	0x520, 0x738,	"frstor?",
	0x1F7, 0x7FF,	"fincstp",
	0x1F6, 0x7FF,	"fdecstp",
	0x5C0, 0x7F8,	"ffree\tST(i)?",
	0x1D0, 0x7FF,	"fnop",
	0x00,	0x00,	NULL
};

/* end of i8086d1.c */
