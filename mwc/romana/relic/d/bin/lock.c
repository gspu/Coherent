/*
 *  lock.c
 *
 *  Provide a locking mechanism for UUCP
 */

#include <stdio.h>
#include <access.h>
#include <sys/stat.h>

char *gen_res_name();

#ifdef UUCP
#include "dcp.h"
#else
#define SPOOLDIR	"/usr/spool/uucp"
#define LOCKSIG		9	/* Significant Chars of Lockable Resources.  */
#define LOKFLEN		64	/* Max Length of UUCP Lock File Name.	     */
#endif /* UUCP */

#define LOCKDIR	SPOOLDIR
#define	LOCKPRE	"LCK.."
#define PIDLEN	6	/* Maximum length of string representing a PID.  */

/* There is a special version of DEVMASK for the PE multiport driver
 * because of the peculiar way it uses the minor device number.  For
 * all other drivers, the lower 5 bits describe the physical port--
 * the upper 3 bits give attributes for the port.
 */

#define PE_DRIVER 21	/* Major device number for the PE driver.  */
#define PE_DEVMASK 0x3f	/* PE driver minor device mask.  */
#define DEVMASK 0x1f	/* Minor device mask.  */


/*
 *  lockit(resource)  char *resource;
 *
 *  Lock the given resource.
 *  Returns (-1) if already locked or error in locking.
 *          ( 0) if all ok, resource locked.
 */

lockit(resource)
char *resource;
{

	int lockfd;
	char lockfn[LOKFLEN];
	char pidstring[PIDLEN];

	sprintf(lockfn, "%s/%s%.*s", LOCKDIR, LOCKPRE, LOCKSIG, resource);
	if ( (access(lockfn, AEXISTS) == 0) ||
	     ((lockfd=creat(lockfn, 0644)) == -1) ) {
#ifdef UUCP
		printmsg(M_DEBUG, "Can't lock: %s", lockfn);
#endif /* UUCP */
		return( -1 );
	}
#ifdef UUCP
	printmsg(M_DEBUG, "Just created lock: %s", lockfn);
#endif /* UUCP */
	sprintf(pidstring, "%d", getpid());
	if(-1 == write(lockfd, pidstring, strlen(pidstring))){
		close(lockfd);
		unlink(lockfn);
		return(-1);
	}

	close(lockfd);
	return( 0 );
}

/*
 *  lockrm(resource)  char *resource;
 *
 *  Simply remove the lock on the given resource.
 *  Returns (-1) if not locked or error in unlocking.
 *          ( 0) if all ok, resource lock removed.
 *
 *
 * Open the lock file for read operations to try to read the PID
 * stored in the file. If the open fails, abort. If the read fails, 
 * abort. If the read PID does not match our PID, abort. We will only
 * remove the lock if our PID matches the PID written to the file.
*/

lockrm(resource)
char *resource;
{
	return(locknrm(resource, getpid()));
}


/*
 *  locknrm(resource, pid)  char *resource;
 *
 *  Remove the lock on the given resource, using pid as the process id to
 *  look for.
 *
 *  Returns (-1) if not locked or error in unlocking.
 *          ( 0) if all ok, resource lock removed.
 *
 *  Open the lock file for read operations to try to read the PID
 *  stored in the file. If the open fails, abort. If the read fails, 
 *  abort. If the read PID does not match our PID, abort. We will only
 *  remove the lock if the passed matches the PID written to the file.
 */

locknrm(resource, pid)
	char *resource;
	int pid;
{
	int lockfd;	/* pointer to file to read */
	int chars_read;	/* Number of characters read().  */

	char gotpid[PIDLEN + 1];	/* String value of the PID that should be stored
			 	 * in the lock file pointed to by *lockfp.
				 */
	char lockfn[LOKFLEN];

	if ( resource == NULL ) {
#ifdef UUCP
		plog(M_CALL, "Unlocking NULL resource.");
#endif /* UUCP */
		return( 0 );
	}

	sprintf(lockfn, "%s/%s%.*s", LOCKDIR, LOCKPRE, LOCKSIG, resource);

	/* open the lock file for read, abort on failure */
	if(-1 == (lockfd = (open(lockfn, 0)))){
#ifdef UUCP
		printmsg(M_DEBUG, "Error opening lock file for PID verify");
		plog(M_CALL, "Error opening lock file for PID verify");
#endif /* UUCP */
		close(lockfd);
		return(-1);
	}

	/* read the contents of the file. Abort if empty */
	if ( -1 == (chars_read = read(lockfd, gotpid, PIDLEN))) {
#ifdef UUCP
		printmsg(M_DEBUG, "Lockrm: Error reading lock file for PID verify");
		plog(M_CALL, "Lockrm: Error reading lock file for PID verify");
#endif /* UUCP */
		close(lockfd);
		return(-1);
	}

	gotpid[chars_read] = '\0';	/* NUL terminate the string.  */
	if (atoi(gotpid) != pid){
#ifdef UUCP
		printmsg(M_DEBUG, "Lockrm: PID verify failed. PID read was %s.", 
			gotpid);
		plog(M_CALL, "Lockrm: PID verify failed. PID read was %s", gotpid);
#endif /* UUCP */
		close(lockfd);
		return(-1);
	}else{
#ifdef UUCP
		printmsg(M_DEBUG, "Lockrm: PID verify successful, removing lock.");
		plog(M_CALL, "Lockrm: PID verify successful, removing lock.");
#endif /* UUCP */

		if ( unlink(lockfn) < 0 ) {
#ifdef UUCP
			printmsg(M_DEBUG, "Lockrm: Error unlocking: %s", lockfn);
			plog(M_CALL, "Lockrm: Error unlocking: %s", lockfn);
#endif /* UUCP */
			close(lockfd);
			return( -1 );
		}
#ifdef UUCP
		printmsg(M_DEBUG, "Just unlocked: %s", lockfn);
#endif /* UUCP */
		close(lockfd);
		return( 0 );
	}
#ifdef UUCP
	plog(M_CALL, "Unreachable code in locknrm().");
#endif /* UUCP */
}

/*
 *  lockexist(resource)  char *resource;
 *
 *  Test for existance of a lock on the given resource.
 *
 *  Returns:  (1)  Resource is locked.
 *	      (0)  Resource is not locked.
 */

lockexist(resource)
char	*resource;
{
	char lockfn[LOKFLEN];

	if ( resource == NULL )
		return(0);
	sprintf(lockfn, "%s/%s%.*s", LOCKDIR, LOCKPRE, LOCKSIG, resource);

	return (!access(lockfn, AEXISTS));
}


/*
 * Attempt to lock a tty device.  Takes the name of the tty itself,
 * otherwise behaves like lockit().
 */
locktty(ttyname)
{
	char resource[LOKFLEN];
	char filename[LOKFLEN];

	sprintf(filename, "/dev/%s", ttyname);
	if (NULL == gen_res_name(filename, resource)){
		return(-1);
	}
	return(lockit(resource));

} /* locktty() */

/*
 * Unlock a tty device.  Takes the name of the tty itself,
 * otherwise behaves like lockrm().
 */
unlocktty(ttyname)
	char *ttyname;
{
	return(unlockntty(ttyname, getpid()));
} /* unlocktty() */

/*
 * Unlock a tty device.  Takes the name of the tty itself,
 * otherwise behaves like locknrm().
 */
unlockntty(ttyname, pid)
	char *ttyname;
	int pid;
{
	char resource[LOKFLEN];
	char filename[LOKFLEN];

	sprintf(filename, "/dev/%s", ttyname);
	if (NULL == gen_res_name(filename, resource)){
#ifdef UUCP
		plog(M_CALL, "Can't generate resource for %s.", ttyname);
#endif /* UUCP */
		return(-1);
	}
	return(locknrm(resource, pid));
} /* unlockntty() */

/*
 * Attempt to lock a tty device.  Takes the name of the tty itself,
 * otherwise behaves like lockit().
 */
lockttyexist(ttyname)
{
	char resource[LOKFLEN];
	char filename[LOKFLEN];

	sprintf(filename, "/dev/%s", ttyname);
	if (NULL == gen_res_name(filename, resource)){
		return(0);	/* Non-existent tty can not be locked :-) */
	}

	return(lockexist(resource));
} /* lockttyexist() */

/*
 * Generates a resource name for locking, based on the major number
 * and the lower 4 bits of the minor number of the tty device.
 *
 * Builds the name in buff as two "." separated decimal numbers.
 * Returns NULL on failure, buff on success.
 */
char *
gen_res_name(path, buff)
	char *path;
	char *buff;
{
	struct stat sbuf;
	int status;
	
	if (0 != (status = stat(path, &sbuf))) {
		/* Can't stat the file.  */
		return (NULL);
	}

	if (PE_DRIVER == major(sbuf.st_rdev)) {
		sprintf(buff, "%d.%d", major(sbuf.st_rdev),
				       PE_DEVMASK & minor(sbuf.st_rdev));
	} else {
		sprintf(buff, "%d.%d", major(sbuf.st_rdev),
				       DEVMASK & minor(sbuf.st_rdev));
	}

	return(buff);
} /* gen_res_name */

#ifdef TEST
#include <stdio.h>

#define LOCKSIG 9	/* Significant Chars of Lockable Resources.  */

main(argc, argv)
	int argc;
	char *argv[];
{
	char buffer[LOCKSIG + 1];
	char path[LOKFLEN];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s ttyname\n", argv[0]);
		exit(1);
	}

	sprintf(path, "/dev/%s", argv[1]);

	if (NULL == gen_res_name(path, buffer)) {
		fprintf(stderr, "%s: Can't stat %s.\n", argv[0], argv[1]);
		exit(1);
	}

	printf("Resource to lock: %s\n", buffer);

	if (-1 == locktty(argv[1])) {
		fprintf(stderr, "%s: Can't lock %s.\n", argv[0], argv[1]);
		exit(1);
	}

	printf("I think I've locked %s.\n", argv[1]);

	if (lockttyexist(argv[1])) {
		printf("Yep, %s is locked.\n", argv[1]);
	} else {
		fprintf(stderr, "%s: Failed to lock %s.\n", argv[0], argv[1]);
		exit(1);
	}


	sprintf(path, "cat /usr/spool/uucp/LCK..%s", buffer);
	printf("Contents of lock file: ");
	fflush(stdout);
	system(path);
	printf("\n");

	if (-1 == unlocktty(argv[1])) {
		fprintf(stderr, "%s: Problem removing lock on %s.\n", argv[0], argv[1]);
		exit(1);
	}

	printf("I think I've unlocked %s.\n", argv[1]);

	if (lockttyexist(argv[1])) {
		fprintf(stderr, "%s: Failed to unlock %s.\n", argv[0], argv[1]);
		exit(1);
	} else {
		printf("Successfully unlocked %s.\n", argv[1]);
	}

	
	exit(0);
}
#endif /* TEST */
