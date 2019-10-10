/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__WCHAR_H__
#define	__COMMON__WCHAR_H__

/*
 * This internal header file defines the ISO C data type 'wchar_t'. This type
 * is defined as being exported from several standard headers, and for
 * maintainability the definition has been centralized here.
 *
 * The 'wchar_t' data type should be the size of the compiler's wide-character
 * constants.  Especially likely candidates for encodings are UCS-2 (aka
 * Unicode) and UCS-4.
 */

#include <common/feature.h>

#if	__COHERENT__

typedef	unsigned short	wchar_t;

#else

#error	Need a definition for wchar_t

#endif

#endif	/* ! defined (__COMMON__WCHAR_H__) */
