/* builtin.c -- builtin routines for tboot.
 *
 * Add new ones by adding a check for them in interpret().
 *
 * La Monte H. Yarroll <piggy@mwc.com>, September 1991
 */

#include <string.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/dir.h>
#include <sys/typed.h>
#include "tboot.h"

extern int slow_flag;	/* Slow down pacifier.  */
extern int feet_flag;	/* Enable pacifier footprints.  */

/*
 * If possible, execute "command".
 * Return "true" if the command exists, "false" otherwise.
 */
int
interpret(command)
	char *command;
{
	char *argv[MAX_TOKENS+1];	/* Command vector.  */
	char *current_token;
	char *cmd_name;	/* The first token of the command line.  */
	int i;

	current_token = strtok(command, WS);
	cmd_name = current_token;		/* Extract the command.  */
	/* Build the command argv.  */
	for (i = 0; NULL != current_token && i < MAX_TOKENS; ++i) {
		argv[i] = current_token;
		current_token = strtok(NULL, WS);
	}

	argv[i] = NULL;	/* NULL terminate the list of pointers.  */

	/*
	 * If we ran out of token slots, print a warning.
	 */
	if (i >= MAX_TOKENS) {
		puts("\r\nWarning: truncating line to:\r\n");
		for (i = 0 ; i < MAX_TOKENS; ++i) {
			puts(argv[i]);
			puts(" ");
		}
		puts("\r\n");
	}

	/*
	 * Interpret the lexed command.
	 */
	if (0 == strcmp(cmd_name, "info")) {
		dpb();
		return(TRUE);
	} else if ((0 == strcmp(cmd_name, "dir")) ||
		   (0 == strcmp(cmd_name, "ls") ) ||
		   (0 == strcmp(cmd_name, "lc") )) {
		dir();
		return(TRUE);
	} else if ( (0 == strncmp(cmd_name, "sys_base", strlen("sys_base")) ) ||
		   (0 == strncmp(cmd_name, "scs", strlen("scs")) )
		) {
		char lbuf[5]; /* 5 == strlen("0000") + 1 */
		if (NULL != strchr(cmd_name, '=')){
			puts("Setting sys_base.\r\n");
			sys_base_set = (1==1);
			sys_base = (uint16)
				basetoi(1 + strchr(cmd_name, '='), 16);
		}
		if (!sys_base_set) {
			puts("sys_base will default to ");
			print16(DEF_SYS_BASE);
			puts(" for l.out executables,\r\n and to ");
			print16(COFF_SYS_BASE);
			puts(" for COFF executables.\r\n");
		} else {
			puts("sys_base is ");
			itobase((uint16) sys_base, lbuf, 16);
			puts(lbuf);
			puts("\r\n");
		}
		return(TRUE);
	} else if (0 == strcmp(cmd_name, "monitor")) {
		monitor();
		return(TRUE);
	} else if (0 == strcmp(cmd_name, "cpu_type")) {
		switch(get_cpu_type()) {
		case 0: puts("less than 286\r\n");	break;
		case 1: puts("286\r\n");		break;
		case 2: puts("386 or better\r\n");	break;
		}
		return(TRUE);
	} else if ((0 == strcmp(cmd_name, "verbose"))) {
		verbose_flag = TRUE;
		puts("Verbose mode.\r\n");
		return(TRUE); 
	} else if ((0 == strcmp(cmd_name, "terse"))) {
		verbose_flag = FALSE;
		puts("Terse mode.\r\n");
		return(TRUE); 
	} else if ((0 == strcmp(cmd_name, "slow"))) {
		slow_flag = TRUE;
		puts("Slow pacifier mode.\r\n");
		return(TRUE); 
	} else if ((0 == strcmp(cmd_name, "fast"))) {
		slow_flag = FALSE;
		puts("Normal pacifier mode.\r\n");
		return(TRUE); 
	} else if ((0 == strcmp(cmd_name, "feet"))) {
		feet_flag = TRUE;
		puts("Pacifier footprints enabled.\r\n");
		return(TRUE); 
	} else if ((0 == strcmp(cmd_name, "nofeet"))) {
		feet_flag = FALSE;
		puts("No pacifier footprints.\r\n");
		return(TRUE); 
	} else if ((0 == strcmp(cmd_name, "gift"))) {
		dump_gift();	/* Dump boot_gift.  */
		return(TRUE);
	} else if ((0 == strcmp(cmd_name, "test"))) {
		if (arg_exist("arg")) {
			puts("Yep.\r\n");
		} else {
			puts("Nope.\r\n");
		}
		return(TRUE);
	} else if ((0 == strcmp(cmd_name, "help")) ||
		   (0 == strcmp(cmd_name, "?")) ) {
		   puts("info          Print disk information.\r\n");
		   puts("dir|ls        List contents of /.\r\n");
		   puts("?|help        Print this list.\r\n");
		   return(TRUE);
	} else if (0 == strcmp(cmd_name, "mon_on")) {
		want_monitor = TRUE;
		puts("Mini-monitor will autoinvoke before running kernel.\r\n");
		return(TRUE);
	} else if (0 == strcmp(cmd_name, "??")) {
		   puts("UNSUPPORTED FEATURES:  (don't call :-)\r\n");
		   puts("sys_base|scs  Print current load segment.\r\n");
		   puts("sys_base=tttt Set current load segment to 0xtttt.\r\n");
		   puts("test arg      Test for the presence of arg.\r\n");
		   puts("monitor       Invoke the mini-monitor.\r\n");
		   puts("mon_on        Autoinvoke the mini-monitor before kernel.\r\n");
		   puts("cpu_type      print the cpu type.\r\n");
		   puts("verbose       Be verbose.\r\n");
		   puts("terse         Do not be verbose.\r\n");
		   puts("slow          Slow down pacifier.\r\n");
		   puts("fast          Normal speed pacifier.\r\n");
		   puts("feet          Enable pacifier footprints.\r\n");
		   puts("nofeet        Disable pacifier footprints.\r\n");
		   return(TRUE);
	} else {
		execute(argv);
		return(FALSE);
	}
	puts("\r\nUNREACHABLE CODE IN interpret() EXECUTED.\r\n");
	puts("There is probably a missing return() in interpret().\r\n");
	return(FALSE);	/* This should be an unreachable line.  */
} /* interpret() */

/* Display the BIOS parameters loaded up by the startup code.  */
void
dpb()
{
	char buffer[BLOCK];
	int i;
	int num_of_drives;
	struct reg r;
	
	
	num_of_drives = get_num_of_drives();

	puts("\r\nHere are the BIOS parameters for your hard drive");
	if (num_of_drives > 1) { putchar('s'); }
	puts(".\r\n");
	puts("Write them down for use during installation.\r\n");
	
	for (i = 0; i < num_of_drives; ++i) {
	 
		r.r_ax = DISK_PARAMS;
		r.r_dx = HARD_DRIVE + i;
	
		intcall(&r, &r, DISKINT);	/* Ask the BIOS.  */

		puts("Drive ");
		itobase((uint16) i, buffer, 10);
		puts(buffer);
	

		/* ch is the lower 8 bits of number of cylinders.
		 * The high two bits of cl are the top two bits of
		 * 10 bit number of cylinders.
		 *
		 * The cylinder count is actually 1 short.
		 */
		puts(":  Cylinders=");
		itobase((uint16) (
			  ((LOW(r.r_cx) >> 6) * 256) + /* Top two bits...  */
			  (HIGH(r.r_cx) + 1)
			),
			buffer, 10);
		puts(buffer);
	
		/* The head count is actually 1 short.  */
		puts("  Heads=");
		itobase((uint16) HIGH(r.r_dx) + 1, buffer, 10);
		puts(buffer);
	
		/* Only the lower 6 bits of cl are the sectors per track.  */
		puts("  Sectors per track=");
		itobase((uint16) SIXBITS & LOW(r.r_cx), buffer, 10);
		puts(buffer);
	
		puts("\r\n");
	} /* for i = 0 to num_of_drives - 1 */
} /* dpb() */


/*
 * Ask the BIOS how many drives are attached.
 *
 * If the BIOS says more than MAX_DRIVES, ask again.  Some BIOS's
 * seem to answer this question differently on alternate calls, or
 * on the first call.
 */
#define MAX_DRIVES 8
int get_num_of_drives()
{
	struct reg r;

	r.r_ax = DISK_PARAMS;	/* ah = 8 -- Return disk drive parameters */
	r.r_dx = HARD_DRIVE;	/* set bit 7 of dl for hard disk info.  */
	
	intcall(&r, &r, DISKINT);

	if ( verbose_flag ) {
		/*
		 * Print out the flags for debugging purposes.
		 */
		puts("flags: ");
		print16(r.r_flags);
		puts(" ");
	}

	if (LOW(r.r_dx) > MAX_DRIVES) {
		puts("I doubt that you have 0x");
		print8(LOW(r.r_dx));
		puts(" disk drives.  Let's try again.\r\n");

		r.r_ax = DISK_PARAMS;	/* ah = 8 -- Return disk drive parameters */
		r.r_dx = HARD_DRIVE;	/* set bit 7 of dl for hard disk info.  */
		
		intcall(&r, &r, DISKINT);

		if (LOW(r.r_dx) <= MAX_DRIVES) {
			puts(" That's better!");
		}
	}
	
	if (LOW(r.r_dx) > MAX_DRIVES) {
		puts("I simply don't believe that you have 0x");
		print8(LOW(r.r_dx));
		puts(" disk drives.\r\n");
		puts("I'm going to guess that you have 0x");
		print8(MAX_DRIVES);
		puts(" drives.\r\n");

		return(MAX_DRIVES);
	} else {
		return(LOW(r.r_dx));
	}
} /* get_num_of_drives() */

/* Create a listing of file names in /.  */
void
dir()
{
	int i, j;
	char outbuff[LINESIZE];	/* Buffer for outputing numbers.  */
	struct inode rootinode;
	struct direct dirent;

	/* Open "/" and print out the direct block file entries.  */
	iopen(&rootinode, (ino_t) 2);

	/* Read each directory entry one at a time.  */
	/* We are careful to print an even fraction of 80 for each
	 * directory entry, so that everything lines up nicely.
	 */
	for (i = 0; i < rootinode.i_size; i += sizeof(struct direct)) {

		iread(&rootinode, (char *) &dirent, (fsize_t) i,
		      (uint16) sizeof(struct direct));
		
		/* If the entry is not active, skip the rest.  */
		if (0 == dirent.d_ino) {
			continue;
		}
		
		/* NUL terminate the name.  */
		strncpy(outbuff,
			dirent.d_name,
			DIRSIZ);
		outbuff[DIRSIZ] = '\0';
		/* Print the file name.  */
		puts(outbuff);
		for (j = DIRSIZ - strlen(outbuff); j > 0; --j) {
			putchar(' ');
		}
		puts(" :"); /* Bring total to 16 (80/5) characters.  */
	}
	puts("\r\n");
	
} /* dir() */
