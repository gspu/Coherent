/* (-lgl
 * 	COHERENT 386 Device Driver Kit release 2.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */

/*
 * mon.h
 * Program profiling (monitor) header.
 *
 * A call to monitor() creates a mon.out file structured as follows:
 *	header			struct m_hdr
 *	function count array	struct m_func[m_nfuncs]
 *	profil buffer		short[m_nbins]
 * The profil data scales the pc as follows:
 *	bin number = (pc - low pc) * (scale/2) / (1<<16)
 */

#ifndef	__MON_H__
#define	__MON_H__

#include <sys/types.h>
#include <sys/_null.h>

struct	m_hdr	{
	unsigned short	m_nbins;	/* number of bins		*/
	unsigned short	m_scale;	/* scale factor			*/
	unsigned short	m_nfuncs;	/* number of function counts	*/
	caddr_t		m_lowpc;	/* lowest pc to monitor		*/
	caddr_t		m_lowsp;	/* lowest sp in scount		*/
	caddr_t		m_hisp;		/* highest sp in scount		*/
};

struct	m_func	{
	caddr_t		m_addr;		/* address in function		*/
	long		m_ncalls;	/* number of times function called */
};

/*
 * The C compiler allocates an m_flst structure for each function
 * compiled with the -VPROF option.
 */
struct	m_flst	{
	struct	m_func	m_data;		/* data on this function	*/
	struct	m_flst	*m_link;	/* link to next function in list */
};

#endif

/* end of mon.h */
