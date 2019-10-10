/* (-lgl
 * 	COHERENT Version 4.0.2
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * assert.h
 * C diagnostics header.
 * Draft Proposed ANSI C Standard, Section 4.2, 12/7/88 draft.
 */

#ifndef	_ASSERT_H
#define	_ASSERT_H
extern	void	__assertfail();
#endif

#undef	assert

#if	NDEBUG
#define	assert(p)	((void)0)
#else
#define	assert(p)	((p) ? (void)0 : __assertfail(#p, __FILE__, __LINE__))
#endif

/* end of assert.h */
