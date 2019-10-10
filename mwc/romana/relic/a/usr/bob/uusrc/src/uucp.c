/*
 *	The uucp command.
 *	Copyright Mark Williams Company Lake Bluff IL 1989.
 *	All rights reserved.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <ctype.h>
#include <access.h>
#include <signal.h>
#include "dcp.h"

#define DEFGRADE	'a'

extern int getopt();
extern int optind;
extern char *optarg;
extern char *index(/* char *string, char c */);
extern char *rindex(/* char *string, char c */);
extern char *mktemp(/* char *template */);
extern FILE *fopen();
extern	char	*getwd();
char	*build_full_path();
char *uucpname(), *whoami();
char *filesite(/* char *filename */), *filepath(/* char *filename */);
char *myfilepath();
char *basename(/* char *filename */);
char	tempname[] =			"/usr/spool/uucp/TM.XXXXXX";
char	luser [32];			/* local user name */
char	notifyusr [32];			/* user to be notified */

char *rmtname = NULL;

static	char	spoolname [BUFSIZ];
static	char	fsname [BUFSIZ];	/* full spool pathname */
static	char	dirname [BUFSIZ];	/* full spool pathname */
static	char	fullfrompath [BUFSIZ];
static	char	fulltopath [BUFSIZ];
static	char	fromsite [32];
static	char	tosite [32];
static	char	options [32];
char	thissite[SITELEN];
char	cmd[64];			/* remote command name */
FILE	*commandfile;			/* the "C." spool command file */
char	commandsite [40];		/* site in command file name */
char	grade = DEFGRADE;		/* transfer grade */
char	spdir [40] = SPOOLDIR;		/* spool directory */

int	nocicoflg	= 0;		/* "don't run uucico" flag */
int	called_uux	= 0;
int	mkdirflg	= 1;
int	nocopyflg	= 1;
int	copyflg		= 0;
int	mailflg		= 0;
int	notifyflg	= 0;
int	debugflg	= 0;
int	debugdir	= 0;
char	*toptr, *fromptr;		/* pointers to from and to arguments */
char	*topath, *frompath;		/* file pathname part of source/dest */
char	topathbuf[BUFSIZ];
char	frompathbuf[BUFSIZ];
char	*tositeptr, *fromsiteptr;
char	argbuf[BUFSIZ];
char	argbuf1[BUFSIZ];
char	argbuf2[BUFSIZ];

int	sending, receiving;
struct	stat	statbuf;
int	seqno;

main(argc, argv)
int argc; 
char *argv[];
{
	int ch;
	int	argx;

	commandfile = NULL;
	strcpy(commandsite, "<?>");
	strcpy(thissite, uucpname());
	if (strlen(thissite) == 0) {
		fprintf(stderr, "uucp: can't get my own uucpname\n");
		exit(1);
	}
	strcpy(luser, whoami());
	strcpy(notifyusr, luser);
	if (debugflg > 0)
		fprintf(stderr, "I yam %s.\n", luser);
	while( (ch=getopt(argc, argv, "dfcCmrg:s:x:n:vV")) != EOF ) {
		switch(ch) {
		case 'r':
			nocicoflg = 1;
			break;
		case 's':
			debugdir = 1;
			fprintf(stderr, "-s option is %s.\n", optarg);
			strcpy(spdir, optarg);
			break;
		case 'x':
			debugflg = atoi(optarg);
			fprintf(stderr, "Debug level is %d\n", debugflg);
			fprintf(stderr, "uucp Version %s\n", VERSION);
			break;
		case 'g':
			if (isalnum(optarg[0]) && '\0' == optarg[1])
				grade = optarg[0];
			else {
				fprintf(stderr,
					"uucp: %s: illegal grade\n", optarg);
				usage();
			}
			break;
		case 'd':
			mkdirflg = 1;
			break;
		case 'f':
			mkdirflg = 0;
			break;
		case 'c':
			copyflg 	= 0;
			nocopyflg 	= 1;
			break;
		case 'C':
			copyflg 	= 1;
			nocopyflg 	= 0;
			break;
		case 'm':
			mailflg = 1;
			break;
		case 'n':
			notifyflg = 1;
			strcpy (notifyusr, optarg);
			if (debugflg > 5)
			fprintf(stderr, "notify user %s upon rcpt\n",notifyusr);
			break;
		case 'v':
		case 'V':
			fatal("uucp: Version %s", VERSION);
		default:
			fprintf(stderr, "uucp: Option character of %c\n", ch);
			usage();
		}
	}
	toptr = argv[argc - 1];
	if (debugflg > 4) {
		fprintf(stderr, "argc is now %d\n", argc);
		fprintf(stderr, "therefore, last argument is %s.\n", toptr);
	}
	if ((argc - optind) < 2)
		usage();
	if (optind >= argc)
		usage();
	for (argx = optind; argx < argc - 1; argx ++ )
		uucp(argv[argx], toptr);
	if (commandfile != NULL)
		finish_commandfile();
	if (nocicoflg || called_uux)
		return 0;
	else
		exec_cico(commandsite);
	exit(0);
}

uucp(fromptr, toptr)
char	*fromptr;
char	*toptr;
{
	if (debugflg > 1)
		fprintf(stderr, "uucp from %s to %s\n", fromptr, toptr);
	sending = 0;
	receiving = 0;
	topath = toptr;
	if ((tositeptr = filesite(toptr)) != NULL) {
		sending = 1;
		strcpy(topathbuf, filepath(toptr));
		topath = topathbuf;		
		if (!knowhost(tositeptr)) {
			fprintf(stderr,
			"uucp: destination site %s unknown\n", tositeptr);
			exit(1);
		}
	}
	strcpy(tosite, tositeptr);	tositeptr = tosite;
	frompath = fromptr;
	if ((fromsiteptr = filesite(fromptr)) != NULL) {
		receiving = 1;
		frompath = filepath(fromptr);
		if (!knowhost(fromsiteptr)) {
			fprintf(stderr,
			"uucp: from site %s unknown\n", fromsiteptr);
			exit(1);
		}
	}
	strcpy(fromsite, fromsiteptr);	fromsiteptr = fromsite;
	if (strlen(fromsite) == 0 && strlen(tosite) == 0) {
		if (debugflg > 1) {
			fprintf(stderr, "Need to issue a cp right here\n");
			fprintf(stderr, "cp %s %s\n", frompath, topath);
		}
		sprintf(argbuf, "cp %s %s", frompath, topath);
		system(argbuf);
		return;
	}
	strcpy(frompathbuf, frompath);
	if (debugflg > 1)
		fprintf(stderr,
		"We are copying from site %s file %s to site %s file %s.\n",
		fromsiteptr, frompathbuf, tositeptr, topath);
	if (sending && receiving) {
		if (strcmp (tosite, fromsite) == 0) {
			sprintf(topathbuf, "(%s)", filepath(toptr));
			strcpy(frompathbuf, fromptr);
			sprintf(frompathbuf, "(%s)", filepath(frompathbuf));
		} else {
			sprintf(topathbuf, "(%s!%s)", thissite, toptr);
			sprintf(frompathbuf, "(%s)", frompath);
		}
		return uuxit(fromsiteptr, frompathbuf, topathbuf);
	} else if (complex(toptr)) {
		sprintf(frompathbuf, "!%s", fromptr);
		sprintf(topathbuf, "(%s)", filepath(toptr)); /* strip off first */
		if (debugflg > 3) 
			fprintf(stderr, "complex to; %s, %s, %s\n",
				tositeptr, frompathbuf, topathbuf);
		return uuxit(tositeptr, frompathbuf, topathbuf);
	} else if (complex(fromptr)) {
		sprintf(topathbuf, "(%s!%s)", thissite, toptr);
		sprintf(frompathbuf, "(%s)", frompath);
		if (debugflg > 3)
			fprintf(stderr, "complex fromptr: %s, %s, %s\n",
			fromsiteptr, frompathbuf, topathbuf);
		return uuxit(fromsiteptr, frompathbuf, topathbuf);
	} else {
		if (sending)
			return send_file();
		if (receiving) {
			if (metacharacters(frompathbuf)) {
				sprintf(topathbuf, "(%s!%s)", thissite, toptr);
				return
				 uuxit(fromsiteptr, frompathbuf, topathbuf);
			} else
				return receive_file();
		}
	}
}

encode_options()
{
	strcpy(options, "-");
	if (mkdirflg)
		strcat (options, "d");
	if (nocopyflg)
		strcat(options, "c");
	if (copyflg)
		strcat(options, "C");
	if (notifyflg)
		strcat(options, "n");
	if (mailflg)
		strcat(options, "m");
}

/*
 *	send one file.
 *	Return 1 if queuing was successful,
 *	0 if any error.
 */
int
send_file()
{
	FILE	*sourcefp, *datafp;
	int	c;
	register int (*intfun)(), (*quitfun)();

	encode_options();
	strcpy(fullfrompath, build_full_path(frompath));
	if (stat(fullfrompath, &statbuf) == -1) {
		fprintf(stderr, "uucp: Cannot stat %s.\n", fullfrompath);
		return 0;
	}
	if (debugflg > 4)
		fprintf(stderr,
		"Send: frompath is %s, full is %s.\n", frompath, fullfrompath);
	if (getcommandfile(tositeptr))
		return 0;

	intfun = signal(SIGINT, SIG_IGN);
	quitfun = signal(SIGQUIT, SIG_IGN);
	seqno = getseq(tositeptr);
	if (nocopyflg)
		strcpy(spoolname, "D.0");
	else {
		sprintf(spoolname, "D.%s%c%04d", tositeptr, grade, seqno);
		if (debugflg > 3) {
			fprintf(stderr,	"data file built is %s\n", spoolname);
			fprintf(stderr,"mode: %o.\n",statbuf.st_mode & ~S_IFMT);
		}
	}
	fprintf(commandfile, "S %s %s %s %s %s %o %s\n",
		fullfrompath, topath, luser, options, spoolname,
		statbuf.st_mode & ~S_IFMT, notifyusr);
	if (nocopyflg == 0) {
		sprintf(dirname, "%s/%s", spdir, tositeptr);
		if (debugflg > 2) 
			fprintf(stderr, "Gotta copy\n");
		sprintf(fsname, "%s/%s/%s", spdir, tositeptr, spoolname);
		if ((!ckdir(dirname)) || (datafp = fopen(fsname, "w")) == NULL)
			return cantopen("datafile", fsname);
		if ((sourcefp = fopen(fullfrompath, "r")) == NULL) 
			return cantopen("fromfile", fullfrompath);
		chmod(fsname, 0600);
		while ((c = getc(sourcefp)) != EOF)
			putc(c, datafp);
		fclose(datafp);
		fclose(sourcefp);
	} else
		strcpy(fsname, fullfrompath);
	signal(SIGINT, intfun);
	signal(SIGQUIT, quitfun);
	return 1;
}

cantopen(kind, which)
char	*kind;
char	*which;
{
	fprintf(stderr, "uucp: Cannot open %s (%s)\n", which, kind);
	return 1;
}

/*
 *	receive_file
 *	Return 1 if queuing successful,
 *	0 if not.
 */
receive_file()
{
	if (debugflg > 2)
		fprintf(stderr, "Receive file\n");
	copyflg   = nocopyflg = notifyflg = 0;
	encode_options();
	strcpy(fulltopath, build_full_path(topath));
	if (getcommandfile(fromsiteptr))
		return 0;
	seqno = getseq(fromsiteptr);
	fprintf(commandfile, "R %s %s %s %s dummy\n",
		frompathbuf, fulltopath, luser, options);
	return 1;
}

/*
 *	Handle send and receive forms of command.
 *	Command of the form "uucp a!b c!d".
 *	Send it off via uux to do at another location.
 *	of the form "c!uucp b <mysite>!c!d".
 */
uuxit(cmdsite, from, to)
char	*cmdsite;
char	*from;
char	*to;
{
	int	argx;
	char	*argv[30];
	char	debugdirm[64];
	char	debugbuf[64];
	int	waitstat;

	if (debugflg > 2)
		fprintf(stderr, "uuxit(%s, %s, %s)\n",
		cmdsite, from, to);
	argx	= 0;
	argv[argx++] = "memeUUX";
	if (debugflg > 0) {
		sprintf(debugbuf, "-x%d", debugflg);
		argv[argx++] = debugbuf;
	}		
	/* if (nocicoflg) */	/* always turn off -r for intermediate site */
		argv [argx++] = "-r";
	if (debugdir) {
		sprintf(debugdirm, "-S%s", spdir);
		argv [argx++] = debugdirm;
	}
	sprintf(argbuf, "%s!uucp", cmdsite);
	argv[argx++] = argbuf;
	argv[argx++] = "-C";
	if (mkdirflg)
		argv[argx++] = "-d";
	if (notifyflg) {
		sprintf(argbuf1, "-n%s ", notifyusr);
		argv[argx++] = argbuf1;
	}
	argv[argx++] =  from;
	sprintf(argbuf2, "%s", to);
	argv[argx++] = argbuf2;
	argv[argx++] = NULL;
	if (debugflg > 3) {
		argx = 0;
		while (argv [argx] != NULL)
			fprintf(stderr, "%s ", argv[argx++]);
		fprintf(stderr, "\n");
	}
	if (fork() == 0 ) {
		execvp("uux", argv);
		exit(1);
	} else 
		wait (&waitstat);
	called_uux = 1;
	return waitstat;
}

getcommandfile(forwhich)
char	*forwhich;
{
	if (commandfile != NULL) {
		if (strcmp(forwhich, commandsite) != 0)
			finish_commandfile();
	}
	if (commandfile == NULL) {
		if ((commandfile = fopen(mktemp(tempname), "w")) == NULL) {
			fprintf(stderr, "uucp: can't open ");
			perror(tempname);
			return 1;
		}
		strcpy(commandsite, forwhich);
	}
	return 0;
}

finish_commandfile()
{
	static char buf[BUFSIZ];

	sprintf(buf, "%s/%s", spdir, commandsite);
	if (!ckdir(buf)) {
		fprintf(stderr, "Unable to make directory \"%s\"\n", buf);
		exit(1);
	}
	sprintf(buf, "%s/%s/C.%.6s%c%04d",
		 spdir, commandsite, commandsite, grade, seqno);
	if (debugflg > 1)
		fprintf(stderr, "command file name is %s.\n", buf);
	if (link(tempname, buf) == 0)
		unlink(tempname);
	else {
		fprintf(stderr, "uucp: couldn't rename commandfile\n");
		exit(1);
	}
	fclose (commandfile);
	commandfile = NULL;
}

char *filesite(name)
char *name;
{
	static char site[SITELEN];
	char *p;

	strcpy(site, name);
	if ((p = index(site, '!')) == NULL)
		return NULL;
	*p = '\0';
	return site;
}

char *filepath(name)
char *name;
{
	char *p;
	static char site[SITELEN];
	strcpy(site, name);
	p = index(site, '!');
	if (p == NULL) 
		return NULL;
	return p + 1;
}

char *basename(name)
char *name;
{
	char *p;
	if (NULL == (p = rindex(name, '/')))
		return name;
	else
		return p + 1;
}

char	*
build_full_path(localpath)
char	*localpath;
{
	static	char buf[BUFSIZ];
	char	*p;
	if ((*localpath != '/') && (*localpath != '~')) {
		strcpy(buf, p = getwd());
		if (p == NULL) {
			fprintf(stderr, "uucp: Unable to do getpwd\n");
			exit(1);
		}
		strcat(buf, "/");
		strcat(buf, localpath);
	} else
		strcpy(buf, localpath);
	return buf;
}

usage()
{
	fatal("\n\
Usage: uucp [-dcmr] [-g grade] [-s dir] [-xnum] <fn> <fn> ... \n\
");
}

/*
 *	Return 1 if there are shell type metacharacters in the argument.
 *	Zero otherwise.
 */
metacharacters(strin)
char	*strin;
{
	char	*first;

	if (index(strin, '*') != NULL)
		return 1;
	if (index(strin, '?') != NULL)
		return 1;
	if (((first = index(strin, '[')) != NULL) && (index(first, ']') != NULL))
		return 1;
	return 0;
}

complex(pathname)
char	*pathname;
{
	char	*cp;

	if ((cp = index(pathname, '!')) != NULL) {
		if (index (cp + 1, '!') != NULL)
			return 1;	/* more than one ! */
	}
	return 0;
}
