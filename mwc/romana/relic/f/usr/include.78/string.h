/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * string.h
 * C string handling library header.
 * Draft Proposed ANSI C Standard, Section 4.11, 5/13/88 draft.
 */

#ifndef	__STRING_H__
#define	__STRING_H__

#ifndef	_SIZE_T
#define	_SIZE_T
typedef	unsigned int	size_t;		/* sizeof result type		*/
#endif

#define	Void	char		/* Use "Void *" as generic pointer type	*/
#define	const			/* Ignore type modifier "const"		*/

/* External declarations. */
extern	Void	*memchr();		/* 4.11.5.1 */
extern	int	memcmp();		/* 4.11.4.1 */
extern	Void	*memcpy();		/* 4.11.2.1 */
extern	Void	*memmove();		/* 4.11.2.2 */
extern	Void	*memset();		/* 4.11.6.1 */
extern	char	*strcat();		/* 4.11.3.1 */
extern	char	*strchr();		/* 4.11.5.2 */
extern	int	strcmp();		/* 4.11.4.2 */
extern	int	strcoll();		/* 4.11.4.3 */
extern	char	*strcpy();		/* 4.11.2.3 */
extern	size_t	strcspn();		/* 4.11.5.3 */
extern	char	*strerror();		/* 4.11.6.2 */
extern	size_t	strlen();		/* 4.11.6.3 */
extern	char	*strncat();		/* 4.11.3.2 */
extern	int	strncmp();		/* 4.11.4.4 */
extern	char	*strncpy();		/* 4.11.2.4 */
extern	char	*strpbrk();		/* 4.11.5.4 */
extern	char	*strrchr();		/* 4.11.5.5 */
extern	size_t	strspn();		/* 4.11.5.6 */
extern	char	*strstr();		/* 4.11.5.7 */
extern	char	*strtok();		/* 4.11.5.8 */
extern	size_t	strxfrm();		/* 4.11.4.5 */

/* Non-ANSI function. */
extern	char	*memccpy();

/* Internal function. */
extern	void	_memxchg();

#endif
