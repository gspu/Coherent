/*
 * tboot.c -- tertiary boot
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

#include <sys/types.h>
#include <sys/typed.h>

#define MAIN
#include "tboot.h"
#undef MAIN


/* Potentially communicated information from an earlier tboot.  */
TYPED_SPACE(boot_gift, 8192, T_FIFO_SIC);	/* Static In-Core FIFO.  */

main()
{
	char cmd_line[BLOCK];		/* Command typed at prompt.  */

	sys_base = DEF_SYS_BASE; 	/* Load the kernel here.  */
	sys_base_set = FALSE;
	want_monitor = FALSE;	/* Don't invoke mini-monitor before execution.  */
	verbose_flag = FALSE;	/* Don't be verbose.  */

	puts("\r\nCOHERENT Tertiary boot Version 1.2.6\r\n");

	/*
	 * If autoboot exists, give the user a chance to abort and then
	 * try to run it.
	 */
	if (0 != namei("autoboot")) {
		puts("Press <SPACE> to abort boot.\r\n");
		if( !wait_for_keystroke(WAIT_DELAY, (int) ' ') ) {
			interpret("autoboot");
		}
	}

	puts("If installing COHERENT, please type \"begin\".\r\n");

	for (;;) {
		puts("? ");
		gets(cmd_line, BLOCK);
		puts("\r\n");
		sanity_check("Main command loop");

		interpret(cmd_line);
	} /* forever */
	
	puts("Unreachable code in main().\r\n");
}
