/* (-lgl
 * 	COHERENT Version 3.2
 * 	Copyright (c) 1982, 1991 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * mon.h
 * Program profiling (monitor) header.
 *
 * The layout of the mon.out file created by a call to monitor is
 * as follows:
 *	header (m_hdr)
 *	function count buffer (array of m_funcs)
 *	profil buffer (array of shorts)
 * The profil data scales the pc as follows:
 *	bin number = (pc - low pc) * (scale/2) / (1<<16)
 */

#ifndef	MON_H
#define	MON_H	MON_H

#include <sys/types.h>

#ifndef NULL
#define	NULL	((char *)0)
#endif

struct m_hdr {
	unsigned	m_nbins,	/* number of bins */
			m_scale,	/* scale factor */
			m_nfuncs;	/* number of function counts */
	vaddr_t		m_lowpc;	/* lowest pc to monitor */
	vaddr_t		m_lowsp;	/* lowest sp in scount */
	vaddr_t		m_hisp;		/* highest sp in scount */
};

struct m_func {
	vaddr_t		m_addr;		/* address in function */
	long		m_ncalls;	/* number of times function called */
};

/*
 * The m_flst structure is allocated (by the C compiler) for every routine
 * compiled with the -p option.
 */
struct m_flst {
	struct m_func	m_data;		/* data on this function */
	struct m_flst	*m_link;	/* link to next function in list */
};

#endif

/* end of mon.h */
