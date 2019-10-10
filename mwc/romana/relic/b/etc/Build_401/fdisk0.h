/* fdisk0.h */

#define	DOSSHRINK	0	/* punt dosshrink() for now	*/

#define	USAGE		"Usage: /etc/fdisk [ -crvx ] [ -b mboot ] [ device ... ]\n"
#define	VERSION		"2.8"
#define	COH		"/coherent"
#define	KMEM		"/dev/kmem"
#define	NBUF		512	/* buffer size			*/
#define	SSIZE		512	/* sector size			*/

#define PATCHFILE	"/tmp/patches" /* WARNING! duplicated in build0.h */

/*
 * Conversions.
 * (unsigned) c:h:s to (ulong) sectors,
 * (ulong) sectors to (unsigned) c:h:s.
 */
#define	chs_to_sec(c,h,s) ((((unsigned long)(c)*nheads) + (h)) * nspt + (s) - 1)
#define	sec_to_c(sec)	((unsigned)((sec) / cylsize))
#define	sec_to_h(sec)	((unsigned)(((sec) / nspt) % nheads))
#define	sec_to_s(sec)	((unsigned)(((sec) % nspt) + 1))
/* (ulong) Sectors to (double) megabytes. */
#define	meg(sec)	(((double)(sec)) * SSIZE / 1000000L)
/* (ulong) Sectors to (unsigned) cylinders and tracks, rounding up. */
#define	sec_upto_c(sec)	(sec_to_c((sec) + nspt * nheads - 1))
#define	sec_upto_t(sec)	((unsigned)(((sec) + nspt - 1) / nspt))

/* Externals. */
extern	long	atol();
extern	long	lseek();
extern	char	*malloc();
extern	void	qsort();

/* Functions. */
void		atpatch();
void		change_active();
void		change_part();
void		check_chs();
void		cls();
void		dos_shrink();
void		drive_info();
void		fatal();
void		fdisk();
void		fix_chs();
int		get_boot();
int		get_int();
char *		get_line();
long		get_long();
int		pcompare();
void		print_part();
int		quit();
void		sanity();
void		saveboot();
void		sys();
char *		sys_type();
void		unused();
void		usage();
int		yes_no();

/* Globals. */
extern	char		*argv0;
extern	int		badflag;
extern	char		buf[NBUF];
extern	int		cfd;
extern	int		cflag;
extern	int		cylflag;
extern	unsigned int	cylsize;
extern	unsigned char	*defargs[3];
extern	unsigned char	*device;
extern	unsigned char	*drivename;
extern	int		drivenum;
extern	int		freepart;
extern	unsigned long	freesize;
extern	unsigned long	freestart;
extern	HDISK_S		hd;
extern	hdparm_t	hdparms;
extern	int		isatflag;
extern	char		*mboot;
extern	int		megflag;
extern	unsigned int	nspt;
extern	unsigned int	ncyls;
extern	HDISK_S		newhd;
extern	unsigned int	nheads;
extern	int		nmods;
extern	unsigned long	nsectors;
extern	int		openmode;
extern	int		partbase;
extern	int		rflag;
extern	int		vflag;

/* end of fdisk0.h */
