head	1.1;
access
	hal
	udo
	ljg
	jrd
	nigel
	fred;
symbols
	r12:1.1;
locks; strict;
comment	@ *@;


1.1
date	93.11.09.19.27.04;	author rcsadmin;	state Exp;
branches;
next	;


desc
@@


1.1
log
@COHERENT r12
@
text
@/*
 * install.c
 *
 * Last hacked 07/01/93, COH 4.2-Alpha
 *
 * Install COHERENT disks on a system.
 * This is the back end of the initial COHERENT installation procedure;
 * the first part is in build.c.
 * Without the -b option, it installs an update to an existing COH system.
 * If -b or -u selected, suppress copying of files from /compressed directory
 * on floppies and actually uncompress and unarchive them into place.
 * Uses common routines in build0.o: cc install.c build0.c
 * Usage: install [ -bdv ] id device ndisks
 * Options:
 *	-b	Build: special processing for build, part 2.
 *	-d	Debug, echo commands without executing
 *	-u	Update: special processing for update, part 2.
 *	-v	Verbose
 *	-c	Uncompress files in "compressed" dir on source diskette.
 */

#include <stdio.h>
#include "build0.h"

#define	VERSION		"4.2"
#define	USAGE		"Usage: /etc/install [ -bduv ] id device ndisks\n"

/* Forward. */
void	config();
void	install();
int	newdisk();
void	newusr();
void	setbaud();

/* Globals. */
int	bflag;				/* build flag		*/
int	cflag;				/* uncompress flag	*/
char	*device;			/* special device name	*/
char	*id;				/* disk id		*/
int	ndisks;				/* number of disks	*/
int	update;				/* perform update install */

main(argc, argv) int argc; char *argv[];
{
	register char *s;
	register int i;

	argv0 = argv[0];
	abortmsg = 1;
	usagemsg = USAGE;
	if (argc > 1 && argv[1][0] == '-') {
		for (s = &argv[1][1]; *s; ++s) {
			switch(*s) {
			case 'c':	++cflag;	break;
			case 'b':	++bflag;	++cflag; break;
			case 'd':	++dflag;	break;
			case 'u':	++update;	++cflag; break;
			case 'v':	++vflag;	break;
			case 'V':
				fprintf(stderr, "%s: V%s\n", argv0, VERSION);
				break;
			default:	usage();	break;
			}
		}
		--argc;
		++argv;
	}
	if (argc != 4)
		usage();
	id = argv[1];
	device = argv[2];
	ndisks = atoi(argv[3]);

	/* Add line to /etc/install.log. */
	sprintf(cmd, "/bin/echo /etc/install: %s %s %s >>/etc/install.log",
		argv[1], argv[2], argv[3]);
	sys(cmd, S_NONFATAL);
	sys("/bin/date >>/etc/install.log", S_NONFATAL);

	if (bflag || update)
		sys("/etc/mount.all", S_NONFATAL);

#if	0
	/* Execute prefile.  Not required for the moment. */
	sprintf(cmd, "/conf/%s.pre", id);
	if (exists(cmd)) {
		cls(0);
		sys(cmd, S_NONFATAL);
	}
#endif

	/*
	 * Install disks.
	 * Disk numbers are 2 to ndisks for build, 1 to ndisks otherwise.
	 */
	for (i = ((bflag || update) ? 2 : 1); i <= ndisks; ++i)
		install(i);
#if 0
	if (bflag)
		newusr();
#endif
	if (bflag || update) {
		config();
	}

	/* Delete ids and execute postfile if present. */
	sprintf(cmd, "/bin/rm -f /%s.*", id);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/conf/%s.post", id);
	if (exists(cmd)) {
		if (update)
			strcat(cmd, " -u");
		sys(cmd, S_NONFATAL);
	}
	sys("/bin/echo /etc/install: success >>/etc/install.log", S_NONFATAL);
	sys("/bin/date >>/etc/install.log", S_NONFATAL);
	sys("/bin/echo >>/etc/install.log", S_NONFATAL);
	if (bflag || update)
		sys("/etc/umount.all", S_NONFATAL);

	printf("\nYou have completed the %s procedure successfully.\n",
		(update) ? "update" : "installation");
	printf("Don't forget to remove the last diskette from the disk drive.\n");
	sync();
	if (bflag || update) {
		printf(
"\nYou must reboot your system in order to allow the preceding\n"
"configuration steps to take effect.\n");		
		sys("/etc/reboot -p", S_IGNORE);
	}
	exit(0);
}

/*
 * System-specific configuration.  A lot of grunge.
 */
void
config()
{
	register int port, i, polled, parallel;
	char device[6+1];		/* e.g. "com1pr" */
	char rdevice[7+1];

	cls(1);

	if (yes_no("Does your computer system have a modem")) {
		printf(
"You must specify which asychronous serial line your modem will use.\n"
"See the article \"asy\" in the COHERENT documentation for details.\n");
		port = get_int(1, 4, "Enter 1 to 4 for COM1 through COM4:");
		i = (port > 2) ? port - 2 : port;	/* 1 or 2 */
		printf(
"If your computer system uses both ports COM%d and COM%d,\n"
"one must be run in polled mode rather than interrupt-driven.\n",
			i, i+2);
		polled = yes_no("Do you want to run COM%d in polled mode", port);
		sprintf(cmd, "/bin/ln -f /dev/com%d%sl /dev/modem",
			port, (polled) ? "p" : "");
		if (sys(cmd, S_NONFATAL) == 0)
			printf("/dev/modem is now linked to /dev/com%d%sl.\n",
				port, (polled) ? "p" : "");
		printf("\n");
	}
}

/*
 * Install disk n.
 */
void
install(n) int n;
{
	register int i;

	putchar('\n');
again:
	get_line("Insert a disk from the installation kit into the drive and hit <Enter>.",
		n);
	sprintf(cmd, "/etc/mount %s /mnt -r", device);
	if (sys(cmd, S_NONFATAL))
		goto again;
	if ((i = newdisk()) == 0) {
		sprintf(cmd, "/etc/umount %s", device);
		sys(cmd, S_NONFATAL);
		goto again;
	}

	printf("Copying disk %d.  This will take a few minutes...\n", i);

	sprintf(cmd, "/bin/cpdir -ad%s -smnt %s %s /mnt /",
	(vflag) ? "v" : "",
	(cflag) ? "-scompressed" : "",
	(update) ? "`/bin/sed -e \"s/^/-s/\" < /conf/upd_suppress`" : "");

	sys(cmd, S_FATAL);

	/*
	 * If called from /etc/build as part of the COHERENT install/update,
	 * suppress any files in directory "compressed" and then go install
	 * those files individually while uncompressing and unarchiving.
	 */
	if (cflag) {
		if (update) {

			/* uncompress files in /mnt/compressed, if any */
			sys("/etc/install.u -u", S_NONFATAL);

		} else {

			/* uncompress files in /mnt/compressed, if any */
			sys("/etc/install.u", S_NONFATAL);
		}
	}
	sprintf(cmd, "/etc/umount %s", device);
	sys(cmd, S_NONFATAL);
	sprintf(cmd, "/bin/echo /etc/install: disk %d installed >>/etc/install.log",
		i);
	sys(cmd, S_NONFATAL);
	sync();
}

/*
 * Check for an appropriate id on the disk on /mnt.
 * Return 0 if not found, disk number otherwise.
 */
int
newdisk()
{
	register int i;
	static int n;

	if (dflag)
		return (n >= ndisks) ? 0 : ++n;
	for (i = 1; i <= ndisks; i++) {
		sprintf(buf, "/mnt/%s.%d", id, i);
		if (!exists(buf))
			continue;			/* not disk i */
		sprintf(buf, "/%s.%d", id, i);
		if (exists(buf)) {			/* exists on root */
			printf(
				"The disk you inserted is disk %d of the kit;\n"
				"it has already been copied to the hard disk.\n"
				"Please try again.\n",
				i);
			return 0;			/* wrong disk */
		}
		return i;				/* ok */
	}
	printf(
		"The disk you inserted is not part of the kit.\n"
		"Please try again.\n"
		);
	return 0;					/* no id found */
}

#if 0
/*
 * Install new users.
 */
void
newusr()
{
	static int flag = 0;
	register int n, status, passwd;
	register char *s;
	char homeparent[80], user[80];

	cls(0);
	printf(
"Your COHERENT system initially allows logins by users \"root\" (superuser)\n"
"and \"bin\" (system administrator).  In addition, the password file contains\n"
"special entries for \"remacc\" (to control remote access, e.g. via modem),\n"
"\"daemon\" (the spooler), \"sys\" (to access system information), and\n"
"\"uucp\" (for communication with other COHERENT systems).\n"
"\n"
"If your system has multiple users or allows remote logins, you should assign\n"
"a password to each user.\n"
"\n"
	);
	passwd = yes_no("Do you want to assign passwords to users");
	if (passwd) {
		printf("You must enter each password twice.\n");
		if (yes_no("Do you want to assign a password for user \"root\""))
			sys("passwd root", S_NONFATAL);
		if (yes_no("Do you want to assign a remote access password"))
			sys("passwd remacc", S_NONFATAL);
		if (yes_no("Do you want to assign a password for user \"bin\""))
			sys("passwd bin", S_NONFATAL);
		if (yes_no("Do you want to assign a password for user \"uucp\""))
			sys("passwd uucp", S_NONFATAL);
	}
	printf(
"\nYou should create a login for each additional user of your system.\n"
		);
	for (n = 0; ;) {
		if (!yes_no("Do you want to create another login"))
			break;
		if (++n == 1) {
			printf(
"You must specify a login name, a full name and a shell for each user.\n"
"Joe Smith might have login name \"joe\" and full name \"Joseph H. Smith.\"\n"
"His home directory would be in \"/usr\" by default, namely \"/usr/joe\".\n"
"Do not type quotation marks around the names you enter.\n"
				);
			if (yes_no("Do you want home directories in \"/usr\"")) 
				strcpy(homeparent, "/usr");
			else {
again:
				s = get_line("Where do you want home directories?");
				if (*s != '/') {
					printf(
"Please enter a name beginning with '/', such as \"/u\".\n"
						);
					goto again;
				} else
					strcpy(homeparent, s);
			}
			if ((status = is_dir(homeparent)) == -1) {
				printf("%s is not a directory, try again.\n",
					homeparent);
				goto again;
			} else if (status == 0) {
				sprintf(cmd, "/bin/mkdir -r %s", homeparent);
				if (sys(cmd, S_NONFATAL) != 0)
					goto again;
			}
		}
		s = get_line("Login name:");
		strcpy(user, s);
		sprintf(cmd, "/etc/newusr %s ", s);
		s = get_line("Full name:");
		sprintf(&cmd[strlen(cmd)], "\"%s\" %s", s, homeparent);
		if (flag == 0) {
			++flag;		/* print only first time through */
			printf(
"COHERENT includes three different command line interpreters, or shells.\n"
"A command line interpreter is a program which reads and executes each\n"
"command which the user types.  The available command line interpreters\n"
"are the Bourne shell (/bin/sh), the Korn shell (/usr/bin/ksh) and the\n"
"visual (full screen) shell (/usr/bin/vsh).\n\n"
"Use the visual shell if you prefer a menu-driven interface.  Use the\n"
"Bourne shell if you are not sure which shell to use.\n\n"
"After you have finished installing COHERENT, you can change the shell\n"
"for any user by editing the password file /etc/passwd.\n"
				);
		}
		for (s = NULL; s == NULL; ) {
			if (yes_no("Do you want user %s to use the Bourne shell (/bin/sh)",
					user))
				s = "/bin/sh";
			else if (yes_no("Do you want user %s to use the visual shell (/usr/bin/vsh)",
					user))
				s = "/usr/bin/vsh";
			else if (yes_no("Do you want user %s to use the Korn shell (/usr/bin/ksh)",
					user))
				s = "/usr/bin/ksh";
			else
				printf("You must specify either the Bourne, visual or Korn shell.\n");
		}
		sprintf(&cmd[strlen(cmd)], " %s", s);
		sys(cmd, S_NONFATAL);
		if (passwd && yes_no("Do you want to assign a password for user \"%s\"", user)) {
			sprintf(cmd, "passwd %s", user);
			sys(cmd, S_NONFATAL);
		}
	}
	printf("\n");
}
#endif

/* end of install.c */
@
