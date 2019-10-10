#ifndef	STUNE_H
#define	STUNE_H

#include "devadm.h"


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

void		read_stune_file	PROTO ((CONST char * _inname,
					CONST char * _outname,
					VOID * _extra));
void		read_stune_string PROTO ((CONST char * _string,
					  VOID * _extra));
void		write_stune	PROTO ((stune_t * _stunep, input_t * _input));

EXTERN_C_END

#endif	/* ! defined (STUNE_H) */
