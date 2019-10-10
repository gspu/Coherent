#ifndef	__COMMON_WHENCE_H__
#define	__COMMON_WHENCE_H__

/*
 * This internal header file is intended as the sole point of definition for
 * the ISO C and POSIX.1 manifest constants SEEK_SET, SEEK_END, and SEEK_CUR,
 * used as the "whence" parameters in functions such as fseek () and
 * lseek ().
 */

#define	SEEK_CUR	1		/* from current position */
#define	SEEK_END	2		/* from end		*/
#define	SEEK_SET	0		/* from beginning	*/

#endif	/* ! defined (__COMMON_WHENCE_H__) */

