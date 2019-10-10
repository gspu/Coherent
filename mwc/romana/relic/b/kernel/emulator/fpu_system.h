/*---------------------------------------------------------------------------+
 |  fpu_system.h                                                             |
 |                                                                           |
 | Copyright (C) 1992    W. Metzenthen, 22 Parker St, Ormond, Vic 3163,      |
 |                       Australia.  E-mail apm233m@vaxc.cc.monash.edu.au    |
 |                                                                           |
 +---------------------------------------------------------------------------*/

#ifndef _FPU_SYSTEM_H
#define _FPU_SYSTEM_H

/* system dependent definitions */
#ifdef COHERENT
#include <sys/reg.h>
#include <ieeefp.h>

extern int printf(const char * fmt, ...);
#define NULL ((void *)0)

/* FPU_info will be a pointer to _fpemstate */
/* current will be a pointer to _fpstackframe */

#define FPU_CS			(*(unsigned short *) &(emCurrent->regs[CS]))
#define FPU_DS			(*(unsigned short *) &(emCurrent->regs[DS]))
#define FPU_EAX			(emCurrent->regs[EAX])
#define FPU_EFLAGS		(emCurrent->regs[EFL])
#define FPU_EIP			(emCurrent->regs[EIP])

#define FPU_lookahead           (FPU_info->lookahead)
#define FPU_entry_eip           (FPU_info->entry_eip)
#define fpregs			(FPU_info->regs)
#define top			(FPU_info->top)
#if 0
#define FPU_ORIG_EIP		(FPU_info->ipoff)
#else
#define FPU_ORIG_EIP		FPU_EIP
#endif
#define status_word		(FPU_info->sw)
#define control_word		(FPU_info->cw)

#define ip_offset		(FPU_info->ipoff)
#define cs_selector		(FPU_info->cssel)
#define data_operand_offset	(FPU_info->dataoff)
#define operand_selector	(FPU_info->datasel)

extern struct _fpemstate *FPU_info;
extern struct _fpstackframe *emCurrent;

#else

#include <linux/sched.h>
#include <linux/kernel.h>

#define I387			(emCurrent->tss.i387)
#define FPU_info		(I387.soft.info)

#define FPU_CS			(*(unsigned short *) &(FPU_info->___cs))
#define FPU_DS			(*(unsigned short *) &(FPU_info->___ds))
#define FPU_EAX			(FPU_info->___eax)
#define FPU_EFLAGS		(FPU_info->___eflags)
#define FPU_EIP			(FPU_info->___eip)
#define FPU_ORIG_EIP		(FPU_info->___orig_eip)

#define FPU_lookahead           (I387.soft.lookahead)
#define FPU_entry_eip           (I387.soft.entry_eip)

#define status_word		(I387.soft.swd)
#define control_word		(I387.soft.cwd)
#define fpregs			(I387.soft.regs)
#define top			(I387.soft.top)

#define ip_offset		(I387.soft.fip)
#define cs_selector		(I387.soft.fcs)
#define data_operand_offset	(I387.soft.foo)
#define operand_selector	(I387.soft.fos)

#endif
#endif
