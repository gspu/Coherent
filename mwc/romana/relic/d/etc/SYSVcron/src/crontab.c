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
 * crontab.c
 *
 * Emulation of the SV crontab command.
 * 
 * The next table shows conditions when user is allowed to use crontab.
 *    
 *     \ deny 	 exist 		 exist		exist		doesn't
 *      \	is user    	no user 	but empty        exist
 * allow \
 * _______\________________________________________________________________
 *  exist  |	   
 * is user |        Y 		   Y		   Y               Y
 * ------------------------------------------------------------------------
 *  exist  |
 * no user |        N              N               N               N       
 * ------------------------------------------------------------------------
 *  doesn't|
 *   exist |        N              Y               Y               N
 *
 * Where deny means the file /usr/lib/cron/cron.deny, and allow is
 * the file /usr/lib/cron/cron.allow
 * This table is not valid for user root hwich can always use crontab.
 *
 * $ 12-06-1991 vlad (Vladimir Smelyansky)
 *
 */
#include	<pwd.h>
#include	<errno.h>
#include	<stdio.h>
#include	"cron.h"

extern int	iAllow();		/* Check permissions to use crontab */
extern void	vDelete();		/* Delete user crontable */
extern void	vList();		/* List user crontable */
extern void	vMailDisable();		/* Disable the mail messages */
extern void	vMailEnable();		/* Enable the mail messages */
extern void	vReplace();		/* Replace user crontable */

extern char	*pcFileName;		/* User crontab */

char	acRealUser[MAX_UNAME];		/* Real user name */
char	*pcUserName;			/* User name from a command line */
FILE	*pfNewTab;
int	lFlag = FALSE,			/* List crontab */
	rFlag = FALSE,			/* Delete crontab */
	fFlag = FALSE,			/* Replace crontab */
	uFlag = FALSE,			/* User name */
	mFlag = FALSE,			/* Mail enable/disable */
	ldrFlag = FALSE;

main(argc, argv)
int	argc;
char	*argv[];
{
	int		iChar;
	extern int 	optind;
	extern char	*optarg;

	/* Check if user allowed to use the crontab. 
	 * NOTICE: iAllow finds real user name (acRealUser).
	 */
	if (iAllow() != TRUE) {
		fprintf(stderr, "Sorry, you are not authorized to use cron\n");
		exit(1);
	}

	/* Ok, we have permissions to execute it.
	*/
	while((iChar = getopt(argc, argv, "u:lrf:m:")) != EOF) {
		switch(iChar) {
		case 'l':		/* List crontab */
			if (ldrFlag)
				usage();
			lFlag = ldrFlag = TRUE;
			break;
		case 'r':		/* Remove crontab */
			if (ldrFlag)
				usage();
			rFlag = ldrFlag = TRUE;
			break;
		case 'f':		/* Replace crontab */
			if (ldrFlag)
				usage();
			fFlag = ldrFlag = TRUE;
			pcFileName = optarg;
			Dprint("main: file is %s\n", pcFileName);
			break;
		case 'u':		/* User name */
			if (strcmp(acRealUser, optarg)) {
				if (strcmp(acRealUser, "root")) {
					fprintf(stderr, 
"Sorry, you are not autorized edit '%s' crontab\n", pcUserName);
					exit(1);
				}
				strncpy(acRealUser, optarg, MAX_UNAME);
			}
			break;
		case 'm':		/* Enable disable mail */
			mFlag = TRUE;
			switch(optarg[0]) {
			case 'e':
				Dprint("crontab: mail enable. %s\n", optarg);
				vMailEnable();
				break;
			case 'd':
				Dprint("crontab: mail disable. %s\n", optarg);
				vMailDisable();
				break;
			default:
				usage();
			}
			break;
		case '?':
		default:
			usage();
		}
	}

	/* At least one option should be given.
	*/
	if (ldrFlag == FALSE && mFlag == FALSE)	
		usage();

	if (lFlag) {	/* List user crontab */
		vList();
		exit(0);
	}
	if (fFlag) {	/* Replace user crontab */
		Dprint("main: Case replace. User %s\n", acRealUser);
		vReplace();
		exit(0);
	}
	if (rFlag) {	/* Remove user crontab */
		vDelete();
		exit(0);
	}
	exit(1);
}

usage()
{
	fprintf(stderr, "usage:  crontab [-u user_name] ...\n");
	fprintf(stderr, "\t\t-l                       list user's crontab\n");
	fprintf(stderr, "\t\t-r                       remove user's crontab\n");
	fprintf(stderr, "\t\t-f file_name             replace user's crontab\n");
	fprintf(stderr, "\t\t-m e[nable]/d[isable]    enable/disable mail\n");
	exit(1);
}

