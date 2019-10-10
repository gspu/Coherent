#ifndef	MTUNE_H
#define	MTUNE_H

/*
 *-IMPORTS:
 *	<sys/compat.h>
 *		CONST
 *		EXTERN_C_BEGIN
 *		EXTERN_C_END
 *		PROTO ()
 */

#include <sys/compat.h>

#ifndef	MTUNE_T
#define	MTUNE_T
typedef struct mtune	mtune_t;
#endif

#ifndef	MTLIST_T
#define	MTLIST_T
typedef	struct mtlist	mtlist_t;
#endif

#ifndef	SYMBOL_T
#define	SYMBOL_T
typedef struct symbol	symbol_t;
#endif

#ifndef	STUNE_T
#define	STUNE_T
typedef	struct stune	stune_t;
#endif


/*
 * Structure of an entry read in from the 'mtune' file.
 */
struct mtune {
	mtune_t	      *	mt_next;	/* next entry on global list */

	symbol_t      *	mt_name;	/* tunable parameter name */
	long		mt_min;		/* minimum legal value */
	long		mt_default;	/* default value */
	long		mt_max;		/* maximum legal value */

	stune_t	      *	mt_stune;	/* if non-default value configured */
};


/*
 * Structure for keeping a global list of all the entries read in.
 */

struct mtlist {
	mtune_t	      *	mtl_first;
	mtune_t	      *	mtl_last;

	int		mtl_count;
};


enum {
	MAX_PARAMNAME = 31
};


EXTERN_C_BEGIN

mtune_t	      *	find_mtune	PROTO ((symbol_t * _sym));
mtune_t       *	mtunes		PROTO ((void));
void		read_mtune_file	PROTO ((CONST char * _name));

EXTERN_C_END

#endif	/* ! defined (MTUNE_H) */
