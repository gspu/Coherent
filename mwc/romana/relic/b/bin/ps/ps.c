#include <stdio.h>
#include <string.h>
#include <sys/coherent.h>
#include <sys/coh_ps.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include "ps.h"

#define	PROC_NUM	64	/* Number of processes */
#define CON_SIZE	8	/* max # of bytes in contrl terminal filed */

extern char	*malloc();
void		vGetData();	/* Read the data about all processes */
void		vCvtArgs();	/* Process command line */
void		vPrintPs();	/* Print data */
void		vPrintHeader();	/* Print ps header */
void		vPrintLine();	/* Print ps data */
dev_t		dvtGetTerminal();/* Get ps controlling terminal */
int		iState();	/* Is the process asleep, running, or zombie? */
void 		ReadDevDir();	 /* Read device directory */
unsigned	uOkToPrint();	/* Is the process one we want to display? */

stMonitor	*pMonData = NULL; 	/* Pointer to an array of data for ps */
int		iRet;			/* Number of bytes read */
dev_t		dvtPs;			/* ps device */

#define		MAXPROCESSES	40		/* # max number of precesses */

unsigned	auProcessP[MAXPROCESSES];	/* List of process to print */
int		iNumProcessP = 0;		/* Number of process to print */

static stDevices	*pstDevFirst= NULL;	/* List of character devices. */

/* Flags for input command options */
int	iaFlag,		/* Display information from all terminals */
	idFlag,		/* Status of loadable drivers */
	ifFlag,		/* Blank fields have '-' */
	igFlag,		/* Print group leader if l given */
	ilFlag,		/* Long format */
	imFlag,		/* Scheduling fields */
	inFlag,		/* Suppress header line */
	ipFlag,		/* Print only specified processes */
	irFlag,		/* Print real size of processes */
	itFlag,		/* Print elapsed CPU time */
	iwFlag,		/* Wide format (print 132 columns) */
	ixFlag;		/* Display processes without controlling terminals */

/*
 * Print out process status.
 */
main(argc, argv)
int	argc;
char	*argv[];
{
	/*
	 * To preserve compatibility with older versions of COHERENT ps
	 * we will fake minus here. Be aware that it adds '-' not only to
	 * the command line options but to their arguments as well ;-(
	 * See 'p' case.
	 */
	fakeMinus(argc, argv);
	/*
	 * Digest the command line.
	 *
	 * This routine will not return if there are errors in
	 * the command line.
	 */
	vCvtArgs(argc, argv);

	/* This function will not return if error occurs */
	vGetData();

	/* Get dev_t value for device from which ps was fired */
	dvtPs = dvtGetTerminal();

	/* We have our data and all flags are set. So, we can print it */
	vPrintPs();
}

/*
 * Fetch all data for ps from the kernel.
 */
void	vGetData()
{
	int	fd;	/* File descriptor to /dev/ps*/
	int	iCnt;	/* number of attempt to read */

	/* Open process device */
 	if ((fd = open("/dev/ps", O_RDONLY)) < 0) {
		fflush(stdout);
		perror("ps: cannot open /dev/ps");
		exit(1);
	}

	/* This is our main engine. At this point we do not know the
	 * total number of processes. So, we start from our gess PROC_NUM.
	 * If we get close enough, do realloc and try again.
	 */
	for (iCnt = 0, iRet = 0; iCnt - iRet <= sizeof(stMonitor); ) {
		iCnt += sizeof(stMonitor) * PROC_NUM;
		if (pMonData != NULL)
			free(pMonData);
		if ((pMonData = (stMonitor *) malloc(iCnt)) == NULL) {
			fflush(stdout);
			perror("ps");
			exit(1);
		}
		if ((iRet = read(fd, (char *) pMonData, iCnt)) < 0) {
			fflush(stdout);
			perror("ps");
			exit(1);
		}
	}

	close(fd);
	fflush(stdout);

} /* main() */

/*
 * Digest the command line.
 */
void
vCvtArgs(argc, argv)
int argc;
char *argv[];
{
	char		*opstring = "ac:defgk:lmnp:rtwx";
	extern char	*optarg;
	int		c;
	char 		*token;

	while ((c = getopt(argc, argv, opstring)) != EOF)
		switch (c) {
		case 'a':
		case 'e':
			iaFlag = 1;
			break;
		case 'c':	/* We will just ignore it */
			break;
		case 'd':
			idFlag = 1;
			break;
		case 'f':
			ifFlag = 1;
			break;
		case 'g':
			igFlag = 1;
			break;
		case 'k':	/* We will just ignore it */
			break;
		case 'l':
			ilFlag = 1;
			break;
		case 'm':
			imFlag = 1;
			break;
		case 'n':
			inFlag = 1;
			break;
		case 'p':
			ipFlag = iaFlag = 1;

			/* Take out the '-' that fakeMinus puts in */
			while (*optarg == '-')
				optarg++;

			token = strtok(optarg, ",");
			while((token != NULL) && (iNumProcessP < MAXPROCESSES)) 
			{
				auProcessP[iNumProcessP++] = atoi(token);
				token = strtok(NULL, ",");
			}
			if (iNumProcessP == 0)
				ipFlag = 0;
			break;
		case 'r':
			irFlag = 1;
			break;
		case 't':
			itFlag = 1;
			break;
		case 'w':
			iwFlag = 1;
			break;
		case 'x':
			ixFlag = 1;
			break;
		default:
			usage(c);
	}
	if (ilFlag && igFlag)
		igFlag = 1;
	else
		igFlag = 0;	
} /* cvt_args() */

void vPrintPs()
{
	register stMonitor	*pMonTmp;

	/* Read device directory only once */
	ReadDevDir();	/* We may not return from ReadDevDir */

	/* Print the header line */
	vPrintHeader();

	for (pMonTmp = pMonData + iRet / sizeof(stMonitor) - 1;
					pMonTmp >= pMonData; pMonTmp--) {
		if (uOkToPrint(pMonTmp->p_pid))
			vPrintLine(pMonTmp);
	}
}

void vPrintHeader()
{
	/* If header suppressed */
	if (inFlag)
		return;
	/* Now we can print */
	printf("%-8s%5s", "TTY", "PID");
	if (igFlag)
		printf(" %5s", "GROUP");
	if (ilFlag)
		printf(" %5s%9s%5s%5s%2s%9s",
			"PPID", "UID", "K", "F", "S", "EVENT");
#if 0
	if (imFlag)
		printf(" %8s %8s %8s %8s", "CVAL", "SVAL", "IVAL", "RVAL");
#endif
	if (itFlag)
		printf(" UTIME STIME");
	puts("  COMMAND");
}

/*
 * Print next line.
 */
void vPrintLine(pMonLine)
stMonitor	*pMonLine;
{
	char		*cpFindTerminal();
	char		*cpTty;

	/* Check controling terminal */
	if (major(pMonLine->p_ttdev) == 0) {
		if (minor(pMonLine->p_ttdev) == 0)
			cpTty = "null";
	} else {
		if (major(pMonLine->p_ttdev) == 0x0FF) {
			if (minor(pMonLine->p_ttdev) == 255) {
				cpTty = "-------";
			}
		} else {
			if ((cpTty=cpFindTerminal(pMonLine->p_ttdev)) == NULL)
				cpTty = "?";
		}
	}

	/* Do we have to skip this line? */
	if (!iaFlag && (dvtPs != pMonLine->p_ttdev)) {
		if (!ixFlag || strcmp("-------", cpTty))
			return;
	} else
		if (!ixFlag && !strcmp("-------", cpTty))
			return;
	printf("%-8s%5u", cpTty, pMonLine->p_pid);

	if (igFlag)
		printf(" %5d", pMonLine->p_rgid);

	if (ilFlag) {
		int	c;
		struct passwd	*pstPasswd;

		if ((pstPasswd = getpwuid(pMonLine->p_uid)) == NULL) {
			fflush(stdout);
			perror("\nps: cannot get user name");
			exit(1);
		}
		printf("%6d%9s", pMonLine->p_ppid, pstPasswd->pw_name);
		if (irFlag)
			printf("%5d", pMonLine->rsize);
		else
			printf("%5d", pMonLine->size);
		printf("%5o", pMonLine->p_flags);
		c = iState(pMonLine);
		printf(" %c", c);
		if (c == 'S') {
			if (pMonLine->u_sleep[0] != '\0')
				printf("%9s", pMonLine->u_sleep);
			else
				printf("%9x", pMonLine->p_event);
		} else
			if (ifFlag)
				printf("        -");
			else
				printf("         ");

	}

	/* Scheduling fields */
#if 0
	if (imFlag) 
		printf(" %8x %8x %8x %8x", pMonLine->p_cval,
				pMonLine->p_sval,
				pMonLine->p_ival,
				pMonLine->p_rval);
#endif
	if (itFlag) {
		ptime(pMonLine->p_utime);
		ptime(pMonLine->p_stime);
	}
				
	printf("  %.*s\n", U_COMM_LEN, pMonLine->u_comm);
}

/*
 * Given a time in HZ, print it out.
 */
ptime(l)
	long l;
{
	register unsigned m;

	if ((l=(l+HZ/2)/HZ) == 0) {
		if (ifFlag)
			printf("     -");
		else
			printf("      ");
		return;
	}
	if ((m=l/60) >= 100) {
		printf("%6d", m);
		return;
	}
	printf(" %2d:%02d", m, (unsigned)l%60);
}

/*
 * Find status of a proces. If sleep, reason why.
 */
int iState(pMon)
stMonitor	*pMon;
{

	if (ASLEEP(pMon)) {
		if (pMon->rrun)
			return 'S';
		if ((pMon->p_flags & PFSTOP) != 0)
			return 'T';
		if (pMon->p_state == PSSLEEP)
			return 'W';
		else
			return 'w';
	}
		
	if (pMon->p_state == PSRUN)
		return 'R';
	if (pMon->p_state == PSDEAD)
		return 'Z';
	return '?';
}

/*
 * Find a device. Return device name or NULL if device was not found.
 */
char *cpFindTerminal(dev)
dev_t	dev;
{
	register stDevices	*pstDev;

	/* Find a proper device */
	for (pstDev = pstDevFirst; pstDev != NULL; pstDev = pstDev->next) {
		if (dev == pstDev->dev) {
			return pstDev->pDevName;
		}
	}
	/* Device not found */
	return NULL;
}

/*
 * Read device directory and store data in memory.
 * This function will not return if it cannot open /dev or run
 * out of memory.
 */
void ReadDevDir()
{
	register DIR		*pDir;
	register stDevices	*pstDevNext;
	static stDevices	*pstDevPrev;
	register struct dirent	*pDrnt;
	struct stat		stStat;
	char			pFileName[32];	/* File name buffer */
	
	if ((pDir = opendir( "/dev" )) == NULL) {
		fflush(stdout);
		perror("ps");
		exit(1);
	}

	while ( (pDrnt = readdir( pDir )) != NULL ) {
		sprintf(pFileName, "/dev/%.*s", DIRSIZ, pDrnt->d_name);
		/* We may skip silent the file that we cannot stat.
		 */
		if (stat(pFileName, &stStat) < 0)
			continue;
		if ((stStat.st_mode & S_IFMT) != S_IFCHR)
			continue;

		if ((pstDevNext = malloc(sizeof(struct DEVICES))) == NULL) {
			fflush(stdout);
			perror("ps");
			exit(1);	
		}
		if (pstDevFirst == NULL)
			pstDevFirst = pstDevPrev = pstDevNext;
		else
			pstDevPrev->next = pstDevNext;
			
		pstDevNext->dev = stStat.st_rdev;
		strncpy(pstDevNext->pDevName, pDrnt->d_name, DIRSIZ);
		pstDevNext->next = NULL;
		pstDevPrev = pstDevNext;
        }
	closedir(pDir);
}

/*
 * Get dev_t value for ps controlling terminal.
 * Return 0 is a flag was set or controlling terminal was not found.
 */
dev_t dvtGetTerminal()
{
	register stMonitor	*pMonTmp;
	register int		iPid;	/* Our process id */

	if (iaFlag)
		return 0;
	
	iPid = getpid();
	
	/* Go through all data until ps id will be found */
	for (pMonTmp = pMonData + iRet / sizeof(stMonitor) - 1;
					pMonTmp >= pMonData; pMonTmp--)
		if (pMonTmp->p_pid == iPid)
			return pMonTmp->p_ttdev;

	return 0;
}

/* 
 * Return 1 if ipFlag is not set or ipFlag is set and process is specified, 
 * 0 otherwise.
 */
unsigned uOkToPrint(pnum)
unsigned pnum;
{
	int	i;

	if (!ipFlag)
		return 1;
	
	for (i = 0; i < iNumProcessP; i++)
		if (auProcessP[i] == pnum)
			return 1;
	return 0;
}

usage()
{
	printf("usage: ps [-][aefglmnrtwx][p#[,#]...]\n");
	exit(1);
}

fakeMinus(argc, argv)
int argc;
char * argv[];
{
	int argn;
	char * malloc();

	for (argn = 1; argn < argc; argn++) {
		if (argv[argn][0] && argv[argn][0] != '-') {
			char * temp = malloc(strlen(argv[argn])+2);
			temp[0] = '-';
			strcpy(temp+1, argv[argn]);
			argv[argn] = temp;
		}
	}
}
