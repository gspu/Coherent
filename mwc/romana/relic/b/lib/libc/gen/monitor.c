/*
 * libc/gen/monitor.c
 * Runtime profiling.
 */

/*
 * monitor() is an interface to the profil() system call.
 * It is called _profon() and _profoff() in crt/_prof.c.
 * If 'low' is non-NULL, it starts the profiling.
 * Otherwise, it dumps out statistics and turns off profiling.
 * The region of memory profiled is from 'low' to 'high',
 * not including 'high'.
 * 'buff' is an array of 'blen' shorts into which profiling counts are stored.
 * On the z8001, only stack monitoring and call counting are supported.
 * After the profil() system call enables profiling,
 * the system examines the user PC on each clock tick and bumps
 * the appropriate counter in short buff[].
 */

#include <mon.h>

#define MODE	0666			/* mon.out creation mode */

/*
 * The call counting routine scount() references _mhdr.m.low_sp and _fnclst.
 * The list pointed to by m_flst is never empty;
 * it always has a dummy entry on the end which has m_link field NULL.
 */
static	struct m_flst	dummy[];
	struct m_flst	*_fnclst	= dummy;
	struct m_hdr	_mhdr;
static	short		*baddr;		/* buffer address */

#ifndef	Z8001
monitor(low, high, buff, blen) caddr_t low, high; short buff[]; int blen;
/* monitor(low, high, buff, blen) vaddr_t low, high; short buff[]; int blen; */
#else
monitor(low)
vaddr_t	low;
#endif
{
/*	if (low == (vaddr_t)0) { */
	if (low == (caddr_t)0) {
		endmon();
		return;
	}

#ifndef	Z8001
	baddr = buff;			/* save buffer address for endmon() */
	if (blen >= (high-low)/2) {
		blen = (high-low)/2;
		_mhdr.m_scale = (unsigned short)-1;
	} else
		_mhdr.m_scale = ((long)blen << 17) / (high-low);
	_mhdr.m_nbins = blen;
	_mhdr.m_lowpc = low;

	/* Without getting incredibly involved, this will do */
/*	_mhdr.m_lowsp = ((vaddr_t)&blen) + sizeof(blen); */
	_mhdr.m_lowsp = ((caddr_t)&blen) + sizeof(blen);
#else
	_mhdr.m_lowsp = 0xFFFEL;
#endif
	_mhdr.m_hisp = _mhdr.m_lowsp;

#ifndef	Z8001
	/* Turn on profiling. */
	profil(buff, blen * sizeof *buff, low, _mhdr.m_scale);	/* turn it on */
#endif
}

/*
 * End profiling.
 * Shut off system profiling and write the mon.out file.
 * /bin/prof reads and interprets the mon.out.
 */
static
endmon()
{
	register int	fd,
			cnt;
	register struct m_flst	*flp;

#ifndef	Z8001
	/* Shut off system profiling. */
	profil(NULL, 0, 0, 0);
#endif

	for (flp = _fnclst, cnt = 0; flp != dummy; flp = flp->m_link)
		++cnt;			/* count function call entries */
	_mhdr.m_nfuncs = cnt;

	/* Create the mon.out file. */
	if ((fd = creat("mon.out", MODE)) < 0)
		return;
	write(fd, &_mhdr, sizeof _mhdr);			/* write header */
	for (flp = _fnclst; flp != dummy; flp = flp->m_link)
		write(fd, &flp->m_data, sizeof flp->m_data);	/* write call data */
#ifndef	Z8001
	write(fd, baddr, _mhdr.m_nbins * sizeof *baddr);	/* write bin data */
#endif
	close(fd);
}

/* end of libc/gen/monitor.c */
