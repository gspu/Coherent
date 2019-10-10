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

/* undial()	
 * removes the lock on the remote device if it exists and reenables
 * the port. Undial() is called by hangup(), which was called by 
 * sysend(). Bob H. 11/22/91.
*/
undial (fd)
int	fd;
{
	close (fd); /* close the port */

	/* If lock removal fails, print message. */

	if ( (strcmp(rdevname,"-") != 0) && lockttyexist(rdevname) ){
		if(unlocktty(rdevname) == -1){
			printmsg(M_DEBUG,"Undial: tty lock file removal failed");
			plog(M_CALL,"Undial: tty lock file removal failed");
		}
	}
	/* If lock removal failed, then do not re enable the port because we
	 * no longer know who did what to the remote port. Re enabling the
	 * port could result in a race condition we don't want.
	*/

	if ((enableme[0] != '\0') && (lockttyexist(rdevname) == 0)){
		plog(M_CALL, "Enabling tty line %s", enableme);
		exec_stat("enable", enableme);
		strcpy(enableme, "");
	}else{
		if(enableme[0] != '\0'){
		printmsg(M_DEBUG,"Undial: Can not re-enable port due to tty lock file.");
		plog(M_CALL,"Undial: Could not re-enable port due to tty lock file.");
		}
	}
	rdevname = NULL;
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
	int	retval;			/* Place to stash return of exec_stat */

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


		/* If the Ldev remote line is not a '-', then see if a lock
		 * exists on the remote device. If a lock exists, then we don't
		 * want to disable the remote before calling out on the local
		 * local device for fear of booting off a logged in process.
		*/

		/* Check for a lock on the remote device */
		if ((strcmp(l_rline,"-")!=0) && (0 != lockttyexist(l_rline))) {
			plog(M_CALL,"Remote tty device %s locked, cannot disable.",
				l_rline);
			continue;
		} else {
			enableme[0] = '\0';
			if(strcmp(l_rline,"-") !=0){
	/* Disable the remote device and then create a lock on it.
	 * If the lock fails, abort.
	 * Note that we will then sleep for 5 seconds to make sure that
	 * the port gets closed after the disable.
	 */
				if (locktty(l_rline) < 0) {
					plog(M_CALL,"Remote tty device %s locked, cannot disable.",
						l_rline);
					continue;
				}

				/* Note that disable could be terminated by
				 * a SIGHUP when the port is disabled.
				 */
				if (0!=(retval=exec_stat("disable", l_rline)) &&
				    SIGHUP<<8 != retval) {
					plog(M_CALL,"Disable of tty line %s failed",
					     l_rline);
					continue;
				}else{
				plog(M_CALL,"Disabling tty line %s", l_rline);
					sleep(5);
					strcpy(enableme,l_rline);
				}
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
	 
	return waitstat;
}

