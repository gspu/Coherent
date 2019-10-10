/* fwtable.h */

/* Manifest constants. */
#define	FLAG_PCL	1		/* PCL font width table flag */
#define	FLAG_PS		2		/* PostScript font width table flag */
#define	NBUF		512
#define	NWIDTH		256		/* character buffer size */
#define	VERSION		"1.5"

/* Debugging output macro. */
#if	DEBUG
#define	dbprintf(args)	fprintf args
#else
#define	dbprintf(args)
#endif

/* Type definitions. */
typedef unsigned char uchar;

/* Functions in fwtable.c. */
char		*alloc();
void		base();
int		char_code();
void		dump_chartab();
void		dump_glyph();
void		escape();
void		escape_cparen();
void		escape_oparen();
void		escape_star();
void		fatal();
void		getextra();
int		getparm();
short		getshort();
unsigned int	getuchar();
void		nonfatal();
void		ofpwrite();
void		putshort();
void		putstring();
void		read_header();
void		usage();

/* Functions in fwt_PS.c. */
void		character();
void		charmetrics();
void		inputPS();
int		lookup();
char		*newstring();
void		outputPS();

/* Functions in fwt_TFM.c. */
long		gcd();
long		getlong();
void		getrational();
char		*getstring();
int		getword();
void		inputTFM();
void		outputTFM();
int		PCL_width();
int		*read_array();
void		read_symbol_set();
void		xseek();
long		xtell();

/* Externals in libc.a. */
extern	long	ftell();
extern	char	*malloc();

/* Globals in fwtable.c. */
extern	char	buf[NBUF];
extern	int	cflag;
extern	int	char_datasize[NWIDTH];
extern	int	char_movement[NWIDTH];
extern	FILE	*ifp;
extern	int	ipointsz;
extern	FILE	*ofp;
extern	int	pflag;
extern	char	*symset;
extern	int	tflag;
extern	int	this_char;
extern	int	vflag;

/* Globals in fwtableps.c. */
extern	int	lineno;

/* end of fwtable.h */
