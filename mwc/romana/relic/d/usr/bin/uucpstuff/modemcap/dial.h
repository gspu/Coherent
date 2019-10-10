/*
 *  dial.h
 *
 *  Header file for the dial(3) dialer package, using modemcap.
 */

#define	CONNECTTIME	60	/* Max seconds after dial to Connect	*/

extern	int merrno;		/* modem error return code		*/
extern	char *_merr_list[];	/* modem error code strings		*/
extern	char modembuf[];	/* characters received from modem	*/

typedef struct {
    int		baud;  /* transmission data rate */
    char	*line; /* device name for out-going line */
    char	*telno;/* pointer to tel-no digits string */
} CALL;

#define	M_INTRPT  -1	/* interrupt occurred */
#define	M_D_HUNG  -2	/* dialer hung (no return from write) */
#define	M_NO_ANS  -3	/* no answer within CONNECTTIME seconds */
#define	M_ILL_BD  -4	/* illegal baud-rate */
#define	M_A_PROB  -5	/* acu problem (open() failure) */
#define	M_L_PROB  -6	/* line problem (open() failure) */
#define	M_NO_LDV  -7	/* can't open LDEVS file */
#define	M_DV_NT_A -8	/* requested device not available */
#define	M_DV_NT_K -9	/* requested device not known */
#define	M_NO_BD_A -10	/* no device available at requested baud */
#define	M_NO_BD_K -11	/* no device known at requested baud */
#define M_DEV_TEL -12	/* Incompatible ACU and telephone number */

#define DVC_LEN	64
#define DEVDIR	"/dev/"

/* minix */


#ifndef _I386

/* #define O_RDWR	2 */
/*
#ifdef BITS8
#define ANYP	BITS8
#else
#define ANYP	(EVENP|ODDP)
#endif
*/
#endif /* _I386 */

