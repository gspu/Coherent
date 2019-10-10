#ifndef	STUNE_H
#define	STUNE_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		PROTO ()
 */

#include <sys/compat.h>

#ifndef	MTUNE_T
#define	MTUNE_T
typedef	struct mtune	mtune_t;
#endif

#ifndef	STUNE_T
#define	STUNE_T
typedef	struct stune	stune_t;
#endif

#ifndef	SYMBOL_T
#define	SYMBOL_T
typedef	struct symbol	symbol_t;
#endif


/*
 * Structure of an entry read from the 'stune' file.
 */

struct stune {
	stune_t	      *	st_next;	/* global list of tuned parameters */
	symbol_t      *	st_name;	/* parameter name */
	mtune_t	      *	st_mtune;	/* master parameter entry */
	long		st_value;	/* configured value */
};


EXTERN_C_BEGIN

void	read_stune_file		PROTO ((CONST char * _name));

EXTERN_C_END

#endif	/* ! defined (STUNE_H) */
