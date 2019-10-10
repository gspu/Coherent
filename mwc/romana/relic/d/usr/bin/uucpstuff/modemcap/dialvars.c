/*
 *  dialvars.c
 *
 *  Data declaration for global variables used by the dial routines
 *  within modemcap.
 *
 *  Note:  This is initialized data intentionally, rather than being
 *	   uninitialized (i.e. common) data which limits the ability
 *	   of the linker to bring this file in from the archive.
 */

#define	NULL	((char *)0)

char
AS = 0,		/* True if numbers dialed in ASCII, False for binary digits */
DI = 0,		/* True if modem can dial numbers, False otherwise          */
HC = 0,		/* True if modem hangs up when DTR drops, False otherwise   */
TT = 0;		/* True if modem uses touchtone by default, False for pulse */

char
*AT = NULL,	/* Enter command state when online                          */
*CS = NULL,	/* Command start string                                     */
*CE = NULL,	/* Command end string - must be present if CS is            */
*DS = NULL,	/* Dial command string                                      */
*DE = NULL,	/* End of dial command string - must be present if DS is    */
*CO = NULL,	/* Connection made at primary baud rate                     */
*CL = NULL,	/* Connection made at secondary (lower) baud rate           */
*IS = NULL,	/* Initialization string - reset modem to onhook and ready  */
*HU = NULL;	/* Hangup command                                           */

int
AD = 0,		/* Delay after AT string before next command                */
BD = 0,		/* Highest communications baud rate                         */
BL = 0,		/* Another, lower baud rate                                 */
ID = 0;		/* Delay time after initialization                          */
