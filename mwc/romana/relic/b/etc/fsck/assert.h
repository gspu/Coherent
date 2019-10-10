/*
 * assert.h
 * C diagnostics header.
 * Draft Proposed ANSI C Standard, Section 4.2, 12/7/88 draft.
 *
 * Special copy to replace fprintf() with printf()
 */
#ifndef	ASSERT_H
#define	ASSERT_H	ASSERT_H

#if	NDEBUG
#define	assert(p)
#else
#include <stdio.h>
#define	assert(p)	if (!(p)) {\
				printf("%s: %d: assert(%s) failed.\n",\
					__FILE__, __LINE__, #p);\
				exit(1);\
			}
#endif
#endif
/* end of assert.h */
