/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1990 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure for a setjmp environment.
 * Saves SP, BP and return PC.
 * An extra word is saved for LARGE model return PC.
 */

#ifndef	SETJMP_H
#define	SETJMP_H	SETJMP_H

typedef	int	jmp_buf[4];

#endif
