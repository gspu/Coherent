/* (-lgl
 * 	COHERENT Driver Kit Version 1.1.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * al.h - include file for com[1-4] driver modules al.c/alx.c
 *
 * $Log:	al.h,v $
 * Revision 1.1  92/07/31  16:07:06  root
 * Initial revision
 * 
 * Revision 1.6  91/12/10  07:58:13  hal
 * Delete msr_deltas and tim fields from com_ddp.
 * 
 * Revision 1.5  91/11/14  14:10:36  hal
 * Re-entrancy fix.  Remove h_close.
 * 
 * Revision 1.4  91/11/14  13:06:56  hal
 * Use sys in include paths.
 * 
 * Revision 1.3	91/04/03  18:54:45	root
 * Add h_close field to COM_DDP struct
 * 
 * Revision 1.2	91/02/21  14:50:52	root
 * Add "msr_deltas" field to COM_DDP struct.
 * 
 * Revision 1.1	91/02/21  14:04:24	root
 * Used for COH Release 3.1.0
 * 
 */

#ifndef	AL_H
#define	AL_H

#ifndef TIMEOUT_H
#include <sys/timeout.h>
#endif

#ifndef KTTY_H
#include <sys/ktty.h>
#endif

#ifndef POLL_CLK_H
#include <sys/poll_clk.h>
#endif

#define NUM_AL_PORTS	4

typedef struct {
	int	port;
	char 	com_num;	/* 0..3 for com1..4, respectively */
} COM_DDP;

extern TTY	*(tp_table[NUM_AL_PORTS]);  /* table of pointers for polling */
#endif
