#ifndef	SDEV_H
#define	SDEV_H

#include "devadm.h"


typedef	unsigned int	unit_t;
typedef	unsigned int	ipl_t;
typedef	unsigned int	itype_t;
typedef	unsigned int	vec_t;
typedef	unsigned long	cma_t;
typedef	unsigned long	ioa_t;


struct sdevice {
	mdev_t	      *	sd_mdevp;	/* master device entry */
	sdev_t	      *	sd_mnext;	/* next unit on master device */
	sdev_t	      *	sd_next;	/* next unit on global thread */
	sdev_t	      *	sd_link;	/* for sorting */

	symbol_t      *	sd_devname;	/* device name */
	int		sd_config;	/* configuration flag */
	unit_t		sd_unit;	/* unit number */
	ipl_t		sd_ipl;		/* ipl to run interrupt routine */
	itype_t		sd_itype;	/* type of interrupt scheme */
	vec_t		sd_vector;	/* interrupt vector number */

	ioa_t		sd_ioa [2];	/* I/O address range */
	cma_t		sd_cma [2];	/* controller address space range */
};


/*
 * Structure for holding a list of "sdevice" structures.
 */

struct sdlist {
	sdev_t	      *	sdl_first;
	sdev_t	      *	sdl_last;
};


/*
 * Types for predicate suitable for passing to sdev_sort ()
 */

typedef	int  (*	ssel_t)		PROTO ((sdev_t * _sdevp));
typedef	int  (*	scmp_t)		PROTO ((sdev_t * _left, sdev_t * _right));


enum {	MAX_VECTOR = 15,
	MAX_IPL = 8
};

enum {
	INT_NONE,
	INT_PER_CHANNEL,
	INT_PER_DEVICE,
	INT_SHAREABLE,
	MAX_ITYPE = INT_SHAREABLE
};


EXTERN_C_BEGIN

void		read_sdev_file	PROTO ((CONST char * _inname,
					CONST char * _outname,
					VOID * _extra));
void		read_sdev_string PROTO ((CONST char * _string,
					 VOID * _extra));
int		sdev_sort	PROTO ((sdev_t ** _sdlistp, sdev_t ** _sdendp,
					ssel_t _selpred, scmp_t _cmppred,
					size_t _ptroff));
void		write_sdevice	PROTO ((sdev_t * _sdevp, input_t * _input));

EXTERN_C_END

#endif	/* ! defined (SDEV_H) */
