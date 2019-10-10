#ifndef	MTUNE_H
#define	MTUNE_H

#include "devadm.h"


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
void		read_mtune_file	PROTO ((CONST char * _inname,
					CONST char * _outname,
					VOID * _extra));
void		read_mtune_string PROTO ((CONST char * _string,
					  VOID * _extra));
void		write_mtune	PROTO ((mtune_t * _mtunep, input_t * _input));

EXTERN_C_END

#endif	/* ! defined (MTUNE_H) */
