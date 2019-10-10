/* build0.h */

/* Manifest constants. */
#define	NBUF		256		/* buffer size			*/
#define PATCHFILE	"/tmp/patches" /* WARNING! duplicated in fdisk0.h */
#define LDKERFILE	"/tmp/ldker"

/* Flags for sys(). */
#define	S_IGNORE	1
#define	S_NONFATAL	2
#define	S_FATAL		3

/* Functions. */
void	cls();
int	exists();
void	fatal();
int	get_int();
char	*get_line();
int	is_dir();
void	nonfatal();
int	sys();
void	usage();
int	yes_no();

/* Globals. */
extern	int	abortmsg;
extern	char	*argv0;
extern	char	buf[NBUF];
extern	char	cmd[NBUF];
extern	int	dflag;
extern	char	*usagemsg;
extern	int	vflag;

/* end of build0.h */
