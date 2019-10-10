/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * assert.h
 * C diagnostics header.
 * Draft Proposed ANSI C Standard, Section 4.2, 12/7/88 draft.
 */

#ifndef	ASSERT_H
#define	ASSERT_H	ASSERT_H

#if	NDEBUG
#define	assert(p)
#else
#include <stdio.h>
#define	assert(p)	if (!(p)) {\
				fprintf(stderr, "%s: %d: assert(%s) failed.\n",\
					__FILE__, __LINE__, #p);\
				exit(1);\
			}
#endif
#endif

/* end of assert.h */
