#ifndef	__COMMON__WCHAR_H__
#define	__COMMON__WCHAR_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the ISO C data type 'wchar_t'. This type is defined as being exported from
 * several standard headers, and for maintainability the definition has been
 * centralized here.
 *
 * The 'wchar_t' data type should be the size of the compiler's wide-character
 * constants. Especially likely candidates for encodings are UCS-2 (aka
 * Unicode) and UCS-4.
 */

#include <common/feature.h>

#if	__COHERENT__

typedef	unsigned short	wchar_t;

#else

#error	Need a definition for wchar_t

#endif

#endif	/* ! defined (__COMMON__WCHAR_H__) */
