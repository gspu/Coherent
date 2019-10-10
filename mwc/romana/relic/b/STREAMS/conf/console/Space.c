/*
 * Configurable information for the console driver(s).
 */

#define __KERNEL__	 1

#include <sys/kb.h>
#include "conf.h"

int		kb_lang = kb_lang_us;

/* Number of virtual console sessions on monochrome display. */
int		mono_count = MONO_COUNT;	/* Tunable */

/* Number of virtual console sessions on color display. */
int		vga_count = VGA_COUNT;		/* Tunable */
