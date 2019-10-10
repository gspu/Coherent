/*
 *  alarm.h
 *
 *  Header file for a modest support for SIGALRM catching.
 */

#include <signal.h>

extern	int timedout;		/* flag for receipt of SIGALRM signal	*/
extern	int alarmclk();		/* SIGALRM signal Service Function	*/

#define	INITALRM()	signal(SIGALRM, alarmclk)
#define	RESETALRM()	signal(SIGALRM, alarmclk)
#define	SETALRM(x)	{timedout = 0; alarm(x);}
#define	CLRALRM()	{timedout = 0; alarm(0);}
#define	STOPALRM()	signal(SIGALRM, SIG_IGN)
