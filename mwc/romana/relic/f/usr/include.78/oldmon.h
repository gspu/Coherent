/* (-lgl
 * 	COHERENT Version 4.0
 * 	Copyright (c) 1982, 1992 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * oldmon.h
 * Program profiling (monitor) header.
 * COH286.
 *
 * A call to monitor() creates a mon.out file structured as follows:
 *	header			struct m_hdr
 *	function count array	struct m_func[m_nfuncs]
 *	profil buffer		short[m_nbins]
 * The profil data scales the pc as follows:
 *	bin number = (pc - low pc) * (scale/2) / (1<<16)
 */

#ifndef	_OLDMON_H
#define	_OLDMON_H

typedef	unsigned short	old_vaddr_t;	/* COH286 vaddr_t */

struct	old_m_hdr	{
	unsigned short	m_nbins;	/* number of bins		*/
	unsigned short	m_scale;	/* scale factor			*/
	unsigned short	m_nfuncs;	/* number of function counts	*/
	old_vaddr_t	m_lowpc;	/* lowest pc to monitor		*/
	old_vaddr_t	m_lowsp;	/* lowest sp in scount		*/
	old_vaddr_t	m_hisp;		/* highest sp in scount		*/
};

#pragma align 2
struct	old_m_func	{
	old_vaddr_t	m_addr;		/* address in function		*/
	long		m_ncalls;	/* number of times function called */
};
#pragma align

#endif

/* end of oldmon.h */
