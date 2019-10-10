/*
 * Monitor is an interface to the profil system call and to the
 * program prof.  It starts the profil if `low' is non-NULL and
 * otherwise dumps out the statistics information and turns off
 * profileing.  The region of memory profiled is from `low' to
 * `high', not includeing `high'.  `buff' is an array of `blen'
 * shorts into which the profileing will take place.
 * note that on the z8001, only stack monitoring and call counting
 * are supported.
 */
#include "mon.h"


#define MODE	0666			/* mon.out creation mode */


/*
 * Note that the list pointed to by m_flst is never empty.  It always has
 * a dummy entry on the end which has m_link field NULL.
 */
static struct m_flst	dummy[];
struct m_flst		*_fnclst	= dummy;
struct m_hdr	_mhdr;
static short		*baddr;		/* buffer address */

#ifndef	Z8001
monitor(low, high, buff, blen)
vaddr_t	low,
	high;
short	buff[];
int	blen;
#else
monitor(low)
vaddr_t	low;
#endif
{
	if (low == (vaddr_t)0) {
		endmon();
		return;
	}
#ifndef	Z8001
	baddr = buff;
	if (blen >= (high-low)/2) {
		blen = (high-low)/2;
		_mhdr.m_scale = (unsigned)-1;
	} else
		_mhdr.m_scale = ((long)blen << 17) / (high-low);
	_mhdr.m_nbins = blen;
	_mhdr.m_lowpc = low;
	/* Without getting incredibly involved, this will do */
	_mhdr.m_lowsp = ((vaddr_t)&blen) + sizeof(blen);
#else
	_mhdr.m_lowsp = 0xFFFEL;
#endif
	_mhdr.m_hisp = _mhdr.m_lowsp;
#ifndef	Z8001
	profil(buff, blen * sizeof *buff, low, _mhdr.m_scale);
#endif
}


static endmon()
{
	register int	fd,
			cnt;
	register struct m_flst	*flp;

#ifndef	Z8001
	profil(NULL, 0, 0, 0);
#endif
	for (flp = _fnclst, cnt = 0; flp != dummy; flp = flp->m_link)
		++cnt;
	_mhdr.m_nfuncs = cnt;
	if ((fd=creat("mon.out", MODE)) < 0)
		return;
	write(fd, &_mhdr, sizeof _mhdr);
	for (flp = _fnclst; flp != dummy; flp = flp->m_link)
		write(fd, &flp->m_data, sizeof flp->m_data);
#ifndef	Z8001
	write(fd, baddr, _mhdr.m_nbins * sizeof *baddr);
#endif
	close(fd);
}
