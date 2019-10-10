/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * al.h - include file for com[1-4] driver modules al.c/alx.c
 */

#ifndef	__SYS_AL_H__
#define	__SYS_AL_H__

#include <kernel/timeout.h>
#include <sys/ktty.h>
#include <sys/poll_clk.h>

#define NUM_AL_PORTS	4

typedef struct {
	int	port;
	char 	com_num;	/* 0..3 for com1..4, respectively */
} COM_DDP;

extern TTY	*(tp_table[NUM_AL_PORTS]);  /* table of pointers for polling */
#endif
