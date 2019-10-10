/*
 * Directory entry on an MS-DOS diskette. Used by file extraction program "dos"
 * and by the MS-DOS emulator.
 */

#ifdef _I386
#pragma align 1
#endif
typedef	struct	mdir	{
	unsigned char	m_name[8];	/* File name		*/
	unsigned char	m_ext[3];	/* File extension	*/
	unsigned char	m_attr;		/* File attribute	*/
	unsigned char	m_junk[10];	/* Reserved		*/
	unsigned short  m_time;		/* Sec/Min/Hour Creation*/
	unsigned short  m_date;		/* Day/Month/(Year-1980)*/
	short		m_cluster;	/* Starting cluster	*/
	long		m_size;		/* File size in bytes	*/
} MDIR;
#ifdef _I386
#pragma align
#endif

#define m_sec(z)	(unsigned short)((unsigned short)(z->m_time    ) & 0x1f)
#define m_min(z)	(unsigned short)((unsigned short)(z->m_time>>5)  & 0x3f)
#define m_hour(z)	(unsigned short)((unsigned short)(z->m_time>>11) & 0x1f)

#define m_day(z)	(unsigned short)((unsigned short)(z->m_date   ) &0x1f)
#define m_month(z)	(unsigned short)((unsigned short)(z->m_date>>5) &0xf)
#define m_year(z)	(unsigned short)((unsigned short)(z->m_date>>9) &0x7f)

#define c_sec(y)	(unsigned short)(y)
#define c_min(y)	((unsigned short)(y)<<5)
#define c_hour(y)	((unsigned short)(y)<<11)

#define c_day(y)	(unsigned short)(y)
#define c_month(y)	((unsigned short)(y)<<5)
#define c_year(y)	((unsigned short)(y)<<9)


/* Special values for m_name[0]. */
#define MFREE	(unsigned char)0x00		/* Never used		*/
#define MMDIR	(unsigned char)0x2E		/* Directory file	*/
#define	MEMPTY	(unsigned char)0xE5		/* Empty name		*/

/* Attributes in m_attr. */
#define MRONLY	0x01			/* Read only		*/
#define	MHIDDEN	0x02			/* Hidden file		*/
#define	MSYSTEM	0x04			/* System file		*/
#define MVOLUME	0x08			/* Volume identifier	*/
#define MSUBDIR	0x10			/* Sub directory	*/
#define MARCHIV	0x20			/* Archive bit		*/
