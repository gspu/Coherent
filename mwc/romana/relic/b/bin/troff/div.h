/*
 * div.h
 * Nroff/Troff.
 * Diversions and traps.
 */

#define nsm	cdivp->d_nspm		/* No space mode */

/*
 * Vertical trap list.
 */
typedef	struct	tpl {
	struct	tpl *t_next;		/* Pointer to next */
	int	t_rpos;			/* Relative trap position */
	unsigned t_apos;		/* Absolute position */
	char	t_name[2];		/* Name of macro to execute */
} TPL;

/*
 * Diversions.
 */
typedef	struct	div {
	struct	div *d_next;		/* Pointer to next */
	char	d_name[2];		/* Name of diversion */
	int	d_rpos;			/* Real vertical position */
	int	d_nspm;			/* No space mode */
	int	d_maxh;			/* Maximum height of diversion */
	int	d_maxw;			/* Maximum width of diversion */
	unsigned long d_seek;		/* Last seek position */
	union	MAC *d_macp;		/* Pointer to macro desc */
	struct	tpl *d_stpl;		/* Start of trap list */
	struct	tpl *d_trap;		/* Start of usable traps */
	struct	tpl *d_ctpp;		/* Current trap pointer */
	int	d_mk;			/* Marked vertical pos'n, see .mk */
} DIV;

/*
 * Global variables.
 */
extern	DIV	*cdivp;			/* Pointer to current diversion */
extern	DIV	*mdivp;			/* Pointer to main diversion */

/* end of div.h */
