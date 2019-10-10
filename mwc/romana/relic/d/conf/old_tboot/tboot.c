/* tboot.c -- tertiary boot
 * This is invoked by the secondary boot to do all the things we can't
 * do in just 512 bytes.
 *
 * Includes an interpreter for builtin commands.  Just type "info" or "dir"
 * to get disk information, or a directory listing of "/".
 *
 * Can load an image up to 1 gigabyte in length.  Segments can be as
 * big as the whole file.
 *
 * La Monte H. Yarroll <piggy@mwc.com>, September 1991
 */

#include <canon.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/ino.h>
#include <sys/inode.h>
#include <l.out.h>
#include <filehdr.h>
#include <string.h>
#include <sys/typed.h>

#define MAIN
#include "tboot.h"
#undef MAIN


/* Potentially communicated information from an earlier tboot.  */
TYPED_SPACE(boot_gift, 8192, T_FIFO_SIC);	/* Static In-Core FIFO.  */

main()
{
	extern uint16 myds;	/* My data segment, from Setup.s.  */
	char cmd_line[BLOCK];		/* Command typed at prompt.  */
	char cmd_name[BLOCK] = "autoboot"; /* File to boot.  */

	ino_t imageinum;		/* inode number of the boot image.  */
	struct inode imageinode;  /* Inode structure for the boot image.  */
	int imageok;		  /* Flag to identify usable executables.  */

	uint16 filemagic;		/* Magic number from file.  */
	struct load_segment imagetable[MAX_SEGS]; /* How to load a file.  */
	struct load_segment *cur_segment; /* Pointer for walking imagetable.  */ 

	uint16 data_seg;	/* Data segment register for image.  */

	uint16 boot_value;      /* Offset of boot_gift into
				 * load image data segment.
				 */

	
	/* Load the kernel here.  */
	sys_base = DEF_SYS_BASE;
	sys_base_set = FALSE;

	want_monitor = FALSE;	/* Don't invoke mini-monitor before execution.  */

	puts("\r\nCOHERENT Tertiary boot Version 1.0.7\r\n");
	/* Look for a valid executable.  */
	do {
		/* Find the file in the file system.  */
		while  ((ino_t) 0 == (imageinum = namei(cmd_name))){

			/* Ask for another name.
			 * Don't generate a message for name "".
			 */
			if (cmd_name[0] != '\0' &&
			    (0 == strcmp(cmd_name, "autoboot"))) {
				puts("\r\nCan't find \"");
				puts(cmd_name);
				puts("\".  Type \"dir\" for a list of files.\r\n");
				puts("If installing COHERENT, please type \"begin\".\r\n");
			}

			/* Fetch new file names, executing them
			 * if they are builtins.  Terminate loop
			 * when we want to try another file name.
			 */
			do {
				puts("? ");
				gets(cmd_line, DIRSIZ);
				puts("\r\n");
				sanity_check("Main command loop");

			} while (interpret(cmd_line));

			/* Extract the cmd_name from the command line.
			 * Do this because strtok is destructive and we
			 * want cmd_line for later use.
			 */
			strcpy(cmd_name, cmd_line);
			strtok(cmd_name, WS);
		}

		/* We've found the image we want to boot--let's open it.  */
		if (0 == iopen(&imageinode, imageinum)) {
			puts("Can't open ");
			puts(cmd_name);
			puts(".  Type \"dir\" for a list of files.\r\n");
			continue;
		}

		/* Read the magic number.  */
		iread(&imageinode, &filemagic,
			(fsize_t) 0, (uint16) sizeof (int));
		canint(filemagic);	/* Harmless on 80386.  */

		/* If we haven't explicitly set sys_base, default it based
		 * on the type of file we are loading.
		 */
		if (!sys_base_set) {
#ifdef VERBOSE
			puts("Assuming default sys_base.\r\n");
#endif /* VERBOSE */
			sys_base = object_sys_base(filemagic);
		}

		imageok = object2load(filemagic, &imageinode, imagetable, &data_seg);

		if (!imageok) {
			puts("File ");
			puts(cmd_name);
			puts(" is not an executable.\r\n");
			puts(".\r\n");
			cmd_name[0] = '\0';
		}

		if (imageok && (0 == strcmp(cmd_name, "autoboot"))) {
			puts("Press <SPACE> to abort boot.\r\n");
			imageok = !wait_for_keystroke(WAIT_DELAY, (int) ' ');
			if (!imageok) {
				cmd_name[0] = '\0';
			}
		}
	} while (!imageok);

	/* ASSERTION: imageinode and imagetable describe a valid executable.  */

#ifdef VERBOSE
	puts("OK!  Loading ");
	puts(cmd_name);
	puts("...\r\n");
#endif /* VERBOSE */

	/* Now actually load everything into memory.  */
	for (cur_segment = &imagetable[0]; cur_segment->valid; ++cur_segment) {
		puts(cur_segment->message);

		ifread(&imageinode,
			cur_segment->load_toseg,
			cur_segment->load_tooffset,
			cur_segment->load_offset,
			cur_segment->load_length);
	}

	/* Does the program we just loaded want more info?  */

	/* Look up the variable "boot_gift" in the image.  */	
	puts("\r\nPlease wait...\r\n");
	boot_value = object_nlist(filemagic, cmd_name, "boot_gift");


	if (0 != boot_value) {
#ifdef VERBOSE
		puts("Preparing arguments.\r\n");
#endif /* VERBOSE */
		/* Yes, this program can accept more information.  */
		prepare_gift(data_seg, boot_value, cmd_line);
	}

#ifdef VERBOSE
	puts("\r\nRunning ");
	puts(cmd_name);
	puts("...\r\n");
#endif /* VERBOSE */

	if (want_monitor) {
		puts("The kernel is ready to run.\r\n");
		puts("SYS_START: ");
		print16(SYS_START);
		puts(" sys_base: ");
		print16(sys_base);
		puts(" data_seg: ");
		print16(data_seg);
		puts("\r\n");
		monitor();
	}

	/* Run the image (the kernel).  */
	gotoker(SYS_START, sys_base, data_seg);
}
