/*
 * coh_intro.c
 * 6/5/91
 * Usage: coh_intro
 * Uses routines in build0.c: cc -o coh_intro coh_intro.c build0.c
 */

#include <stdio.h>
#include "build0.h"

#define	VERSION		"1.5"
#define	DEFPAGER	"exec more -d"

/* External. */
extern	char	*getenv();

/* Forward. */
int	lcdir();
void	mycls();
void	tour();

/* Global. */
char	*pager;
int	ttyflag;

main(argc, argv) int argc; char *argv[];
{
	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'V')
		fprintf(stderr, "%s: V%s\n", argv[0], VERSION);
	ttyflag = isatty(fileno(stdout));
	if ((pager = getenv("PAGER")) == NULL || *pager == '\0')
		pager = DEFPAGER;
	if (!ttyflag || yes_no("Would you like an introductory tour of COHERENT"))
		tour();
	exit(0);
}

/*
 * List the contents of a directory.
 * Flush stdout in case output is redirected.
 * If pflag is true (big directory), pipe output through PAGER.
 */
int
lcdir(dir, msg, pflag) char *dir, *msg; int pflag;
{
	mycls(1);
	printf(msg);
	fflush(stdout);
	if (ttyflag && pflag)
		sprintf(cmd, "lc %s | %s", dir, pager);
	else
		sprintf(cmd, "lc %s", dir);
	return sys(cmd, S_IGNORE);
}

/* Print newlines if stdout is redirected; otherwise, clear the screen. */
void
mycls(flag)
{
	if (ttyflag)
		cls(flag);
	else
		printf("\n\n");
}

/* Take a walk on the Boardwalk... */
void
tour()
{
	mycls(0);
	/* Startup and shutdown. */
	printf(
"When you boot your computer system (by turning on the power,\n"
"by hitting the reset switch, or by typing <Ctrl><Alt><Del>),\n"
"the bootstrap program by default runs the operating system on\n"
"the partition marked as active.  To boot an operating system\n"
"system on a different partition, type a partition number (0 to 7)\n"
"when the boot tries to read the floppy disk.\n"
"\n"
"When you boot COHERENT, it starts up in single-user mode.\n"
"It automatically executes the commands in the file /etc/brc,\n"
"which typically check COHERENT filesystems for consistency\n"
"using /etc/fsck.  If the filesystems check without errors,\n"
"the system goes into multi-user mode, automatically executing\n"
"the commands in the file /etc/rc.  You can interrupt file system\n"
"checking by typing <Ctrl-C>.\n"
"\n"
"To shut down COHERENT from multi-user mode, log in as root and\n"
"execute /etc/shutdown to return the system to single-user mode.\n"
"Then type sync before rebooting or turning off the power.\n"
"Your COHERENT filesystems may be damaged if you reboot or\n"
"power down without following this procedure.\n"
"\n"
		);

	/* Root. */
	lcdir("/", 
"Now we will take a quick tour of your COHERENT filesystem.\n"
"The root directory / contains:\n\n",
		0);
	printf(
"\n"
"COHERENT executes file /.profile when the superuser root logs in.\n"
"File /coherent (linked to /autoboot) is the COHERENT system image.\n"
"Empty directories /f0 and /f1 are used for mounting diskettes.\n"
"Empty directory /mnt is used for mounting other devices.\n"
"The filesystem checking program /etc/fsck uses empty directory\n"
"/lost+found if it finds a problem with the root filesystem.\n"
"COHERENT uses directory /tmp for temporary files; it removes files\n"
"in /tmp automatically when the system goes multiuser (via /etc/rc).\n"
"\n"
		);

	/* /bin. */
	mycls(1);
	printf(
"Directory /bin contains executable programs (commands).\n"
"Some of the most commonly used commands are:\n"
"\tbc\tinteractive calculator with arbitrary precision\n"
"\tcat\tconcatenate/print files\n"
"\tcc\tC compiler\n"
"\tcmp\tcompare files\n"
"\tcp\tcopy files\n"
"\tcpdir\tcopy directory hierarchy\n"
"\tdiff\tsummarize differences between files\n"
"\tdos\ttransfer files to or from MS-DOS\n"
"\ted\tinteractive line-oriented text editor\n"
"\tegrep\textended pattern search\n"
"\thelp\tprint concise command description\n"
"\tlc\tcategorize files in a directory\n"
"\tls\tlist directory contents\n"
"\tmail\tcomputer mail\n"
"\tps\tprint process status\n"
"\trm\tremove files\n"
"\trmdir\tremove directories\n"
"\tscat\tsegmented concatenation\n"
"\tsort\tsort lines of text\n"
"\twc\tcount words, lines, characters in a file\n"
"The next screen lists all the commands in /bin.\n"
		);
	lcdir("/bin", "", 1);
	putchar('\n');

	/* /conf. */
	lcdir("/conf", "Directory /conf contains:\n\n", 0);
	putchar('\n');

	/* /conf/kbd. */
	lcdir ("/conf/kbd",
		"Subdirectory /conf/kbd contains keyboard support:\n\n",
		0);
	printf(
"\n"
"You can change the operation of various keys on your keyboard by changing\n"
"the appropriate file in /conf/kbd and running /etc/kbdinstall.\n"
		);

	/* /dev. */
	lcdir("/dev", "Directory /dev contains COHERENT devices:\n\n", 0);
	putchar('\n');
	mycls(1);
	printf(
"Some important devices are:\n"
"\t/dev/at*\tCooked (block-by-block) hard disk partitions\n"
"\t/dev/com*\tSerial port devices (COM1, COM2, COM3, COM4)\n"
"\t/dev/console\tConsole terminal\n"
"\t/dev/f*\t\tFloppy disk devices\n"
"\t/dev/lpt*\tParallel port devices (LPT1, LPT2 and LPT3)\n"
"\t/dev/null\tBit bucket\n"
"\t/dev/rat*\tRaw (character-by-character) hard disk partitions\n"
"\t/dev/rf*\tRaw (character-by-character) floppy disk devices\n"
"\n"
		);

	/* /drv. */
	lcdir("/drv", "Directory /drv contains loadable device drivers:\n\n", 0);
	putchar('\n');

	/* /etc. */
	lcdir("/etc",
"Directory /etc contains files and programs used in system administration:\n\n",
		0);
	putchar('\n');
	mycls(1);
	printf(
"Files of particular interest in /etc include:\n"
"\t/etc/brc\tExecuted when the system comes up single-user\n"
"\t/etc/group\tDefines user group membership information\n"
"\t/etc/passwd\tDefines user login information\n"
"\t/etc/profile\tExecuted for each shell\n"
"\t/etc/rc\t\tExecuted when the system comes up multi-user\n"
"\t/etc/ttys\tDefines status of terminals attached to system\n"
"\n"
		);

	/* /lib. */
	lcdir("/lib",
		"Directory /lib contains libraries and C compiler phases:\n\n",
		0);
	putchar('\n');

	/* /usr. */
	lcdir("/usr",
		"Directory /usr contains a number of subdirectories:\n\n",
		0);
	printf(
"\n"
"\t/usr/adm\tSystem administration files\n"
"\t/usr/bin\tAdditional COHERENT commands\n"
"\t/usr/dict\tDictionary for spell command\n"
"\t/usr/games\tGames\n"
"\t/usr/include\tHeader files\n"
"\t/usr/lib\tAdditional libraries\n"
"\t/usr/man\tCOHERENT online manual pages\n"
"\t/usr/msgs\tSystem-wide messages\n"
"\t/usr/pub\tPublic information\n"
"\t/usr/spool\tWork area for daemon processes\n"
"\t/usr/src\tSome sources and sample programs\n"
"\t/usr/tmp\tTemporary file directory\n"
"\n"
		);

	/* File types. */
	mycls(1);
	printf(
"COHERENT filenames often contain an extension following '.' which\n"
"indicates the contents of the file.  Some common extensions are:\n"
"\n"
"\t.a\tarchive (or library) in ar format\n"
"\t.b\tbc source\n"
"\t.c\tC source\n"
"\t.h\tC header\n"
"\t.i\tC intermediate file (cpp output)\n"
"\t.l\tlex source\n"
"\t.m\tmacro assembler source\n"
"\t.o\tobject file (unlinked)\n"
"\t.r\tnroff or troff input\n"
"\t.s\tassembler source\n"
"\t.tar\tarchive in tar format\n"
"\t.tar.Z\tcompressed tar-format archive, cf. tar, compress\n"
"\t.tmp\ttemporary file\n"
"\t.y\tyacc source\n"
"\t.Z\tcompressed file, cf. compress/uncompress\n"
"\n"
	);

	/* Diskettes. */
	mycls(1);
	printf(
"Some commonly used diskette device names and formats are:\n"
"\tDevice name  Sectors/track  Heads  Sectors  Bytes   Format\n"
"\t/dev/f9a0          9          2      720    360 KB   5.25\"\n"
"\t/dev/fqa0          9          2     1440    720 KB   3.5\"\n"
"\t/dev/fha0         15          2     2400    1.2 MB   5.25\"\n"
"\t/dev/fva0         18          2     2880    1.44 MB  3.5\"\n"
"Device names ending in '0' and '1' indicate drives A: and B:.\n"
"\n"
		);
	mycls(1);
	printf(
"To use a floppy disk with COHERENT, you must:\n"
"\t(1) format it with /etc/fdformat,\n"
"\t(2) check it for bad blocks with /etc/badscan,\n"
"\t(3) build an empty filesystem on it with /etc/mkfs,\n"
"\t(4) mount it with /bin/mount or /etc/mount,\n"
"\t(5) copy files to or from it, e.g. with /bin/cp or /bin/cpdir,\n"
"and\t(6) unmount it with /bin/umount or /etc/umount.\n"
"\n"
"For example, to copy directory /dir to a 5.25\" high density diskette in A:\n"
"\t/etc/fdformat /dev/fha0\n"
"\t/etc/badscan -o /tmp/proto /dev/fha0 2400\n"
"\t/etc/mkfs /dev/fha0 /tmp/proto\n"
"\t/etc/mount /dev/fha0 /f0\n"
"\tcpdir -vd /dir /f0/dir\n"
"\t/etc/umount /dev/fha0\n"
"/bin/mount and /bin/umount provide handy abbreviations for mount commands.\n"
"\n"
		);

	/* Done. */
	mycls(1);
	printf(
"This concludes your brief introduction to COHERENT.\n"
"To see this introduction again, type \"/etc/coh_intro\".\n"
"To create a file with this information, type \"/etc/coh_intro >/tmp/tour\".\n"
"\n"
		);
}

/* end of coh_intro.c */
