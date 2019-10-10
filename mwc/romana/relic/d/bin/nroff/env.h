/*
 * env.h
 * Nroff/Troff.
 * Environment header file.
 */

/* Buffer sizes. */
#define	ENVSIZE		5		/* Total number of environments	*/
#define	ENVS		3		/* Number of .ev environments	*/
#define	ENVWIDTH	3		/* Special \w environment	*/
#define	ENVTITLE	4		/* Special .tl environment	*/
#define	ENVSTACK	20		/* Pushdown stack for environments */
#define	LINSIZE		300		/* Size of line buffer		*/
#define	NFONTS		50		/* Max number of fonts		*/
#define	TABSIZE		20		/* Maximum number of tab stops	*/
#define	WORSIZE		200		/* Size of word buffer		*/

/*
 * Tab stop entry.
 */
typedef	struct {
	int	t_pos;			/* Tab position			*/
	int	t_jus;			/* Type of justification	*/
} TAB;

/*
 * Environments.
 */
typedef	struct {
	int	e_adj;			/* Adjust justification type	*/
	int	e_adm;			/* Adjust mode			*/
	char	e_ccc;			/* Current control character	*/
	int	e_cec;			/* Count for center command	*/
	int	e_csz;			/* Constant character size	*/
	int	e_curfont;		/* Current font			*/
	int	e_enb;			/* Enbolden by this amount	*/
	int	e_fcsz[NFONTS];		/* Font character size		*/
	int	e_fill;			/* Fill mode			*/
	char	e_fon[2];		/* Font name			*/
	unsigned char	*e_fonwidt;	/* Current font width table	*/
	int	e_fpsz[NFONTS];		/* Font pointsizes		*/
	char	e_hic;			/* Hyphenation indicator character */
	int	e_hyp;			/* Hyphenation mode		*/
	int	e_hypf;			/* Hyphenation of last word flag */
	int	e_ind;			/* Indent			*/
	int	e_inpltrc;		/* Input line trap count	*/
	char	e_inptrap[2];		/* Input line trap		*/
	char	e_ldc;			/* Leader dot character		*/
	int	e_lgm;			/* Ligature mode		*/
	CODE	e_linebuf[LINSIZE];	/* Line buffer			*/
	int	e_llindir;		/* Last directive count		*/
	CODE	*e_llinptr;		/* Last line pointer		*/
	int	e_llinsiz;		/* Last line size		*/
	int	e_lln;			/* Line length			*/
	int	e_lmn;			/* Line number multiple		*/
	int	e_lni;			/* Line number indent		*/
	int	e_lnn;			/* Current line number		*/
	int	e_lns;			/* Line number separation	*/
	int	e_lsp;			/* Line spacing			*/
	char	e_ltabchr;		/* Last tab character		*/
	int	e_mar;			/* Margin separation		*/
	char	e_mrc;			/* Margin character		*/
	char	e_mrch;			/* Margin character here	*/
	char	e_nbc;			/* No break character		*/
	int	e_nlindir;		/* New directive count		*/
	CODE	*e_nlinptr;		/* New line pointer		*/
	int	e_nlinsiz;		/* New line size		*/
	int	e_nnc;			/* Count for no number command	*/
	char	e_oldfon[2];		/* Last value of fon		*/
	int	e_oldind;		/* Last value of ind		*/
	int	e_oldlln;		/* Last value of lln		*/
	int	e_oldlsp;		/* Last value of lsp		*/
	int	e_oldpsz;		/* Last value of psz		*/
	int	e_oldtln;		/* Last value of tln		*/
	int	e_oldvls;		/* Last value of vls		*/
	int	e_posexls;		/* Post extra line space	*/
	int	e_preexls;		/* Pre extra line space		*/
	int	e_psz;			/* Pointsize			*/
	int	e_spcnt;		/* Space padding size		*/
	int	e_ssz;			/* Space size			*/
	int	e_sszdiv;		/* Space size divisor		*/
	int	e_sszmul;		/* Space size multiplier	*/
	long	e_swddiv;		/* Width table divisor		*/
	long	e_swdmul;		/* Width table multiplier	*/
	TAB	e_tab[TABSIZE];		/* Tab table			*/
	TAB	*e_tabptr;		/* Tab pointer			*/
	char	e_tbc;			/* Tab character		*/
	int	e_tbs;			/* Tab spacing			*/
	int	e_tfn;			/* Tab font number		*/
	int	e_tif;			/* Temporary indent flag	*/
	int	e_tin;			/* Temporary indent		*/
	CODE	*e_tlinptr;		/* Tab line pointer		*/
	int	e_tlinsiz;		/* Tab line size		*/
	int	e_tln;			/* Title length			*/
	char	e_tpc;			/* Page character in title	*/
	int	e_ufn;			/* Underline font number	*/
	int	e_ufp;			/* Underline font previous	*/
	int	e_ulc;			/* Count for underline command	*/
	int	e_vls;			/* Vertical line spacing	*/
	CODE	e_wordbuf[WORSIZE];	/* Word buffer			*/
} ENV;

/*
 * Variables in current environment.
 */
#define	adj	env.e_adj		/* Adjust justification type	*/
#define	adm	env.e_adm		/* Adjust mode			*/
#define	ccc	env.e_ccc		/* Current control character	*/
#define	cec	env.e_cec		/* Count for center command	*/
#define	csz	env.e_csz		/* Constant character size	*/
#define	curfont	env.e_curfont		/* Current font			*/
#define	enb	env.e_enb		/* Enbolden by this amount	*/
#define	fcsz	env.e_fcsz		/* Font character size		*/
#define	fill	env.e_fill		/* Fill mode			*/
#define	fon	env.e_fon		/* Font name			*/
#define	fonwidt	env.e_fonwidt		/* Current font width table	*/
#define	fpsz	env.e_fpsz		/* Font pointsizes		*/
#define	hic	env.e_hic		/* Hyphenation indicator character */
#define	hyp	env.e_hyp		/* Hyphenation mode		*/
#define	hypf	env.e_hypf		/* Hyphenation of last word flag */
#define	ind	env.e_ind		/* Indent			*/
#define	inpltrc	env.e_inpltrc		/* Input line trap count	*/
#define	inptrap	env.e_inptrap		/* Input line trap		*/
#define	ldc	env.e_ldc		/* Leader dot character		*/
#define	lgm	env.e_lgm		/* Ligature mode		*/
#define	linebuf	env.e_linebuf		/* Line buffer			*/
#define	llindir	env.e_llindir		/* Last directive count		*/
#define	llinptr	env.e_llinptr		/* Last line pointer		*/
#define	llinsiz	env.e_llinsiz		/* Last line size		*/
#define	lln	env.e_lln		/* Line length			*/
#define	lmn	env.e_lmn		/* Line number multiple		*/
#define	lni	env.e_lni		/* Line number indent		*/
#define	lnn	env.e_lnn		/* Current line number		*/
#define	lns	env.e_lns		/* Line number separation	*/
#define	lsp	env.e_lsp		/* Line spacing			*/
#define	ltabchr	env.e_ltabchr		/* Last tab character		*/
#define	mar	env.e_mar		/* Margin separation		*/
#define	mrc	env.e_mrc		/* Margin character		*/
#define	mrch	env.e_mrch		/* Margin character here	*/
#define	nbc	env.e_nbc		/* No break character		*/
#define	nlindir	env.e_nlindir		/* Current directive count	*/
#define	nlinptr	env.e_nlinptr		/* New line pointer		*/
#define	nlinsiz	env.e_nlinsiz		/* New line size		*/
#define	nnc	env.e_nnc		/* Count for no number command	*/
#define	oldfon	env.e_oldfon		/* Last value of fon		*/
#define	oldind	env.e_oldind		/* Last value of ind		*/
#define	oldlln	env.e_oldlln		/* Last value of lln		*/
#define	oldlsp	env.e_oldlsp		/* Last value of lsp		*/
#define	oldpsz	env.e_oldpsz		/* Last value of psz		*/
#define	oldtln	env.e_oldtln		/* Last value of tln		*/
#define	oldvls	env.e_oldvls		/* Last value of vls		*/
#define	posexls	env.e_posexls		/* Post extra line space	*/
#define	preexls	env.e_preexls		/* Pre extra line space		*/
#define	psz	env.e_psz		/* Pointsize			*/
#define	spcnt	env.e_spcnt		/* Space padding size		*/
#define	ssz	env.e_ssz		/* Space size			*/
#define	sszdiv	env.e_sszdiv		/* Space size divisor		*/
#define	sszmul	env.e_sszmul		/* Space size multiplier	*/
#define	swddiv	env.e_swddiv		/* Width table divisor		*/
#define	swdmul	env.e_swdmul		/* Width table multiplier	*/
#define	tab	env.e_tab		/* Tab table			*/
#define	tabptr	env.e_tabptr		/* Tab pointer			*/
#define	tbc	env.e_tbc		/* Tab character		*/
#define	tbs	env.e_tbs		/* Tab spacing			*/
#define	tfn	env.e_tfn		/* Tab font number		*/
#define	tif	env.e_tif		/* Temporary indent flag	*/
#define	tin	env.e_tin		/* Temporary indent		*/
#define	tlinptr	env.e_tlinptr		/* Tab line pointer		*/
#define	tlinsiz	env.e_tlinsiz		/* Tab line size		*/
#define	tln	env.e_tln		/* Title length			*/
#define	tpc	env.e_tpc		/* Page character in title	*/
#define	ufn	env.e_ufn		/* Underline font number	*/
#define	ufp	env.e_ufp		/* Underline font previous	*/
#define	ulc	env.e_ulc		/* Count for underline command	*/
#define	vls	env.e_vls		/* Vertical line spacing	*/
#define	wordbuf	env.e_wordbuf		/* Word buffer			*/

/*
 * Global variables.
 */
extern	ENV	env;			/* Current environment		*/
extern	int	envinit[ENVSIZE];	/* If initialized		*/
extern	int	envs;			/* Environment stack index	*/
extern	int	envstak[ENVSTACK];	/* Environment stack		*/

/* end of env.h */
