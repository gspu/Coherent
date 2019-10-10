/* 
 * 	The information contained herein is a trade secret of Mark Williams
 * 	Company, and  is confidential information.  It is provided  under a
 * 	license agreement,  and may be  copied or disclosed  only under the
 * 	terms of  that agreement.  Any  reproduction or disclosure  of this
 * 	material without the express written authorization of Mark Williams
 * 	Company or persuant to the license agreement is unlawful.
 *
 * 	An unpublished work by Mark Williams Company, Chicago.
 * 	All rights reserved.
 */

/*
 * misc.c
 *
 * Miscelenious functions for crontab and cron.
 *
 * $ 12-9-1991 vlad (Vladimir Smelyansky)
 */
#include	<stdio.h>
#include	<string.h>
#include	<pwd.h>
#include	<errno.h>
#include	<time.h>
#include	<errno.h>
#include	<sys/ino.h>
#include	<sys/stat.h>
#include	<sys/times.h>
#include 	"cron.h"

FILE	*fpOpenTable();		/* Open crontable */
int	iAllow();		/* Check permissions to use crontab */
int	iInFile();		/* Check if the user is in a file */
int	lock();			/* Create a lock FIFO */
int	set_uid();		/* Set user and group ids */
void	vDelete();		/* Delete user crontable */
void	vGetName();		/* Get Real User name */
void	vList();		/* List user crontable */
void	vMailDisable();		/* Disable the mail messages */
void	vMailEnable();		/* Enable the mail messages */
void	vReplace();		/* Replace user crontable */

char		*pcFileName;	/* Source table for crontab */
extern char	acRealUser[MAX_UNAME];	/* Real user name */

/*
 * Get real user name and id.
 */
void vGetName()
{
	int		iRealUid;	/* Real user ID */
	struct passwd	*pstPasswd;	

	/* Find real user name. 
	 *
	 * NOTE: functions getpwuid and getpwnam will /etc/passwd file
	 * by calling setpwent.
	 */
	iRealUid = getuid();	/* Get real user id */
	if ((pstPasswd = getpwuid(iRealUid)) == NULL) {
		fprintf(stderr, "crontab: cannot find user id %d\n", iRealUid);
		exit(1);
	}
	strncpy(acRealUser, pstPasswd->pw_name, MAX_UNAME);
}

/* 
 * Check if user allowed to use cron.
 * Return TRUE if user autorized to use cron, FALSE otherwise.
 */
int iAllow()	
{
	FILE	*fpAllow,		/* /usr/lib/cron/cron.allow */
		*fpDeny;		/* /usr/lib/cron/cron.deny */
	int	iAllowExist = FALSE;	/* By default allow  doesn't exist*/
	int	iDenyExist = FALSE;	/* By default deny doesn't exist*/

	vGetName();	/* Get reqal user name */

	if (!strcmp(acRealUser, "root")) /* Superuser allowed by definition */
		return TRUE;
	
	/* If allow exist we do not want read anything more */
	if ((fpAllow = fopen(F_ALLOW, "r")) != NULL) 
		return iInFile(fpAllow, acRealUser);/* return TRUE if allow
						     * FALSE otherwise */
		
	if ((fpDeny = fopen(F_DENY, "r")) != NULL) 
		return !iInFile(fpDeny, acRealUser);/* return FALSE if deny
						     * TRUE otherwise */
	
	return FALSE;	/* If neither exist not autorized */
}

/* 
 * Return TRUE if one of the lines in file "fp" matches string "pString" 
 * exactly, FALSE otherwise.
 */
int iInFile(fp, pString)
FILE	*fp;		
char	*pString;
{
	char acLine[MAX_UNAME];

	rewind(fp);	/* Just to be safty */

	while (fgets(acLine, MAX_UNAME, fp)) {
		if (acLine[0] != '\0')
			acLine[strlen(acLine)-1] = '\0';
		if (0 == strcmp(acLine, pString)) 
			return TRUE;	/* Line was found */
	}
	/* We didn't find the line */
	return FALSE;
}

/*
 * Disable the mail messages.
 */
void vMailDisable()
{
	char	*buf;

	if ((buf = malloc(strlen(D_MAIN) + MAX_UNAME)) == 
				NULL) {
		fprintf(stderr, "crontab: not enough memory\n");
		exit(1);
	}
	sprintf(buf, "%s/%s", D_MAIN, acRealUser);
	Dprint("vMailDisable: create lock %s\n", buf);
	lock(buf);
}

/*
 * Enable the mail messages.
 */
void vMailEnable()
{
	char	*buf;

	if ((buf = malloc(strlen(D_MAIN) + MAX_UNAME)) == NULL) {
		fprintf(stderr, "crontab: not enough memory\n");
		exit(1);
	}
	sprintf(buf, "%s/%s", D_MAIN, acRealUser);
	Dprint("vMailDisable: remove lock %s\n", buf);
	unlink(buf);
}

/* 
 * Create a cron lock FIFO. Return TRUE on succes, FALSE if lock exist,
 * exit 1 if cannot lock.
 */
int lock(file_name)
char	*file_name;
{
	if (mknod(file_name, IFPIPE, 0) < 0)
		if (errno == EEXIST) 
			return FALSE;	/* Lock exist */
		else {
			/* Something bad with system at all */
			fprintf(stderr, "cron: cannot lock\n");
			exit(1);
		}
	else
		return TRUE;		/* Cron can run */
}

/*
 * Set user ID to user_name
 */
int set_uid(cpUserName)
char	*cpUserName;
{
	struct passwd	*stpPwd;

	Dprint("set_uid: Effective ID before setuid is %d\n", geteuid());
	
	setpwent();	/* Rewind /etc/passwd. Just to be safety */
	if ((stpPwd = getpwnam(cpUserName)) == NULL)
		return FALSE;
	else {
		/* You cannot setgid and setuid in the same if statement 
		 * because there is no garanty in what order they will be 
		 * evaluated.
		 */
		if (setgid(stpPwd->pw_gid)) {
			fprintf(stderr, 
				"cron: cannot set gid %d for user %s\n", 
						stpPwd->pw_gid, cpUserName);
			return FALSE;
		}			
		if (setuid(stpPwd->pw_uid)) {
			fprintf(stderr, 
					"cron: cannot set uid %d for user %s\n", 
						stpPwd->pw_uid, cpUserName);
			return FALSE;
		}
	}
	/* We have to cd to $HOME here */
	chdir(stpPwd->pw_dir);
	return TRUE;
}

/*
 * List user crontable.
 */
void vList()
{
	FILE	*fp;
	char	acBuf[MAX_STR_LEN];

	if ((fp = fpOpenTable("r")) == NULL) {
		printf("There is no table for user '%s'\n", acRealUser);
		exit(0);
	}

	while (fgets(acBuf, MAX_STR_LEN, fp))
		printf("%s", acBuf);
}

/* 
 * Delete user crontable.
 */
void	vDelete()
{
	char	*pcTableName;

	if ((pcTableName = malloc(strlen(D_SPOOL) + MAX_UNAME )) == NULL) {
		fprintf(stderr, "crontab: not enough memory\n");
		exit(1);
	}

	sprintf(pcTableName, "%s/%s", D_SPOOL, acRealUser);
	Dprint("vDelete: delete table %s\n", pcTableName);
	unlink(pcTableName);
}

void	vReplace()
{
	FILE	*fpInTable;	/* Source table */
	FILE	*fpOutTable;	/* Destination */
	char	acBuf[MAX_STR_LEN];
	time_t	tTime;
	
	Dprint("File name is %s\n", pcFileName);
	if (strcmp(pcFileName, "-") != 0) {
		if ((fpInTable = fopen(pcFileName, "r")) == NULL) {
			fprintf(stderr, "crontab: cannot open file '%s'\n", 
								pcFileName);
			exit(1);
		} 
	} else	/* If input file name is '-' read from stdin */
		fpInTable = stdin;

	Dprint("vReplace: source table is %s\n", pcFileName);

	if ((fpOutTable = fpOpenTable("w")) == NULL) {
		fprintf(stderr, "crontab: cannot open file '%s/%s'\n", 
						D_SPOOL, acRealUser);
		exit(1);
	}

	/* Print a crontab logo.
	*/
	time(&tTime);
	fprintf(fpOutTable, "# Cron Version %s. Installed on %s", VERSION,
						ctime(&tTime));
	fprintf(fpOutTable, "# Source was %s.\n", pcFileName);

	while (fgets(acBuf, MAX_STR_LEN, fpInTable))
		fprintf(fpOutTable, "%s", acBuf);
}	

/*
 * Open user table. Return file pointer if file is open, NULL othewrwise.
 */
FILE	*fpOpenTable(cmd)
char	*cmd;
{
	char	*pcTableName;
	FILE	*fp;

	if ((pcTableName = malloc(strlen(D_SPOOL) + MAX_UNAME + 1)) == NULL) {
		system("echo \"cron*: not enough memory\" > /dev/console");
	}

	strcpy(pcTableName, D_SPOOL);
	strcat(pcTableName, "/");
	strcat(pcTableName, acRealUser);

	Dprint("Destination file is %s\n", pcTableName);
	
	fp = fopen(pcTableName, cmd);
	free(pcTableName);
	return(fp);
}

/*
 * if_log - return TRUE if log file should be used.
 */
if_log()
{
	char	buf[64];
	FILE	*fp_log;

	if ((fp_log = fopen(F_DEF, "r")) == NULL)
		return FALSE;
	
	fgets(buf, 64, fp_log);
	if (!strncmp(buf, "CRONLOG=YES", 11))
		return TRUE;
	return FALSE;
}
	
