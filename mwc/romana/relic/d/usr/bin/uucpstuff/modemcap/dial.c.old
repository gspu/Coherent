/*
 *  dial.c
 *
 *  Implement the dial(3) function calls.
 *
 *  Copyright 1987 (c) John F. Haugh II
 *  Changes Copyright 1989-91 (c) Mark Williams Company
 */

#include <stdio.h>
#include <signal.h>
#include <sys/fcntl.h>
#include "modemcap.h"
#include "dial.h"
#include "dcp.h"
#include "ldev.h"

char	*devname = NULL;	/* Communications Device Name Connected	*/
char	*rdevname = NULL;	/* Remote device name */

static	char	login_lock[15];
static	char	enableme[16];
static	int	modemfd = -1;
extern	char	*strtok();
extern	int	hupcatch();

/*
 *  dial(cp)  CALL cp;
 *
 *  Dial and initiate the call specified via the given CALL data structure.
 *  Returns the opened file descriptor to be used for reads and writes to
 *  the determined device line.  If there is an error, then the return value
 *  is less than zero, and the variable "merror" is set appropriately.
 */

dial(cp)
CALL *cp;
{
	char	modemline[64];		/* device name			*/
	char	*modemname = "\0";
	char	*strcpy (),
	*strcat ();
	int	fd, err;

	fd = -1;		/* channel illegal until line is opened	*/
	if ( (err=findline(cp, &modemname)) <= 0 )
		goto error;
	strcat(strcpy(modemline, DEVDIR), cp->line);
	if ((fd = open (modemline, O_RDWR)) < 0) { /* can't open modem line */
		err = M_L_PROB;
		goto error;
	}
	if ( (err=ttyinit(fd, cp->baud)) != 0 )
		goto error;

	initmodem(modemname, fd);	/* setup modemcap variables */
	if (cp->telno == NULL)     /* no phone number, connection complete */
		goto okay;
	if (! DI) {			/* modem has no ACU!!! */
		/* plog("Says no acu to attatch to???"); */
		err = M_A_PROB;		/* no ACU to attach to */
		goto error;
	}
	if (BD != cp->baud) {	/* is connection desired at high speed? */
		if (BL != cp->baud) {/* is connection desired at low speed? */
			err = M_ILL_BD;	/* modem can't handle this speed */
			goto error;
		}
		BD = BL;		/* set baud to low baud rate */
		CO = CL;	/* set connect reply to low baud reply */
	}
	if (err = mdial (cp->telno, fd)) 	/* some error trying to dial */
		goto error;

okay:
	return (modemfd = fd);
error:
	hangup(fd);
	return (merrno = err);
}

undial (fd)
int	fd;
{
	if (fd > 2)
		close (fd);
	if (enableme[0] != '\0') {
		plog(M_CALL, "Enabling line %s", enableme);
		exec_stat("enable", enableme);
	}
	if ( (rdevname != NULL) && lockexist(rdevname) )
		lockrm(rdevname);
	devname = NULL;
}

static
findline(callp, brand)
CALL *callp;
char **brand;
{
	int	exists = 0;		/* device exists at some baud rate */
	int	tried = 0;		/* found a device but it was locked */
	int	devflag, telflag;
	char	*l_lline;		/* tty device local name */
	char	*l_rline;		/* tty device remote name */
	char	*l_type;		/* ACU, DIR, etc. */
	char	*l_brand;		/* modemcap brand name */
	int	l_baud;			/* tty baud rate */

	ldev_open();
	if ( ((devflag=(callp->line != NULL)) &&
	      (telflag=(callp->telno != NULL))) || (!devflag && !telflag) )
		return(M_DEV_TEL);

	while ( ldev_next() ) {
		l_type  = ldev_value(type_e);
		l_lline = ldev_value(lline_e);
		l_rline = ldev_value(rline_e);
		l_baud  = atoi(ldev_value(baud_e));
		l_brand = ldev_value(brand_e);

		if ( strcmp(l_type, "ACU") == 0 ) {
			if ( devflag )
				continue;
			exists++;
			if (l_baud != callp->baud)
				continue;
		} else if ( strcmp(l_type, "DIR") == 0 ) {
			if ( telflag )
				continue;
			if ( strcmp(l_lline, callp->line) )
				continue;
			callp->baud = l_baud;
		} else {
			continue;
		}
		++tried;		/* found device at desired baud rate */
		strcpy(login_lock, l_lline);
		strcat(login_lock, "+");
		if (lockexist(login_lock))
			continue;	/* somebody is logged in there */

		/* If the Ldev remote line is not a '-', then see if a lock
		 * exists on the remote device. If a lock exists, then we don't
		 * want to disable the remote before calling out on the local
		 * local device for fear of booting off a logged in process.
		 * Bob H. 11/4/91.
		*/

		if((strcmp(l_rline,"-")!=0) && (0 != lockexist(l_rline))){
			plog(M_CALL,"Remote device %s enabled, cannot disable.",
				l_rline);
			continue;
		}

		else if ((strcmp(l_rline,"-") != 0) && (lockit(l_rline) < 0) ){
				continue;
		}

		enableme[0] = '\0';
		if (strcmp(l_rline, "-") != 0) {
			if (exec_stat("disable", l_rline) != 0) {
				plog(M_CALL, "Disabling line %s", l_rline);
				/* tty was enabled */
				strcpy(enableme, l_rline);
			}
		}
		devname = l_lline;
		rdevname = l_rline;
		*brand = l_brand;
		ldev_close();
		callp->line = l_lline;
		return (1);
	}
	if (tried)
		return (merrno = M_DV_NT_A);
	else if (exists) {
		return (merrno = M_ILL_BD);
	} else {
		/* plog("Device not known: %s", brand); */
		return (merrno = M_DV_NT_K);
	}
}

exec_stat(command, line)
char	*command;
char	*line;
{
	int	pid;
	int	waitstat;
	static	char	etccommand[32];

	strcpy(etccommand, "/etc/");
	strcat(etccommand, command);
	/* plog("%s (%s) on line %s", command, etccommand, line);
	 */
	pid = fork();
	if (pid == 0) {
		execl(etccommand, command, line, NULL);
		exit(1);
	} else 
		wait(&waitstat);
	/* plog("Command returned value of %d", waitstat);
	 */
	return waitstat;
}
