/* dos1.h */
/* Common header for "dos" and "dosshrink" commands. */

#include <stdio.h>
#include "bpb.h"
#include "mdir.h"

/* Manifest constants. */
#define	BBSIZE		512		/* Boot block size		*/
#define	CLBAD		0xFFF7		/* FAT bad cluster		*/
#define	CLEOF		0xFFFF		/* FAT end of file marker	*/
#define	CLFREE		0x0000		/* FAT free cluster		*/
#define	CLMAX		0xFFEF		/* Maximum FAT normal cluster	*/
/*
 * The FAT cache must hold a complete 1.5-byte FAT expanded to 2 bytes/entry,
 * so it must contain at least 4096 * 2 = 8192 bytes or 16 sectors.
 */
#define	FATCSECS	(16)		/* Sectors in FAT cache		*/
#define	FATCNPSEC	(BBSIZE/sizeof(short))	/* 2-byte FAT clusters per sector */
#define	FATCCOUNT	(FATCSECS*FATCNPSEC)	/* 2-byte FAT cache entries */
#define	FATMASK		0x0FFF		/* Mask for 1.5-byte FAT entry	*/
#define	INFILE		"/dev/dos"	/* Default input file name	*/
#define	isdir(mdp)	((mdp)->m_attr&MSUBDIR) /* MS-DOS directory test */
#define	ishidden(mdp)	((mdp)->m_attr&MHIDDEN) /* MS-DOS directory test */
#define	is_media_id(n)	(((n)&0xF0)==0xF0) /* Media descriptor test	*/

#if	DEBUG
#define	dbprintf(arglist) printf arglist
#else
#define	dbprintf(arglist)
#endif

/* Externals. */
extern	void	exit();
extern	long	lseek();
extern	char	*malloc();

/* Globals in dos1.c. */
/* Diskette parameters. */
extern	BPB		d8floppy;
extern	BPB		d9floppy;
extern	BPB		d15floppy;
extern	BPB		d18floppy;
extern	BPB		q9floppy;
extern	BPB		s8floppy;
extern	BPB		s9floppy;

/* Other globals. */
extern	char		*argv0;
extern	unsigned char	bootb[BBSIZE];
extern	BPB		*bpb;
extern	short		cflag;
extern	unsigned char	*clbuf;
extern	unsigned short	clsize;
extern	unsigned short	dirbase;
extern	unsigned short	dirsize;
extern	unsigned short	fatbase;
extern	unsigned short	fatbytes;
extern	unsigned short	*fatcache;
extern	unsigned short	fatccount;
extern	unsigned short	fatcfirst;
extern	short		fatcflag;
extern	unsigned short	fatcmax;
extern	unsigned short	fatcmin;
extern	unsigned short	fatsize;
extern	unsigned short	filebase;
extern	short		fsfd;
extern	unsigned short	heads;
extern	unsigned short	maxcluster;
extern	unsigned short	mdirsize;
extern	unsigned short	nspt;
extern	unsigned short	sectors;
extern	unsigned short	ssize;
extern	char		*usagemsg;
extern	short		vflag;

/* Functions in dos1.c. */
extern	unsigned long	cltosec();
extern	void		decodefat();
extern	void		diskread();
extern	void		diskseek();
extern	void		diskwrite();
extern	void		fatal();
extern	void		fatcflush();
extern	void		fatcread();
extern	unsigned short	getcluster();
extern	char		*lcname();
extern	void		putcluster();
extern	void		readfat();
extern	void		setglobals();
extern	void		usage();
extern	void		writefat();
extern	void		xpartition();

/* end of dos1.h */

