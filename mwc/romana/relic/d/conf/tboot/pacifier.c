/*
 * pacifier.c - state machine for putting something interesting on the screen.
 */
#include "tboot.h"
#include "pac_states.h"

int slow_flag = FALSE;		/* Slow down pacifier.  */
int feet_flag = FALSE;		/* Print footprints?  */

void pac_init();		/* Initialise the state machine.  */
void pac_cleanup();		/* Clean up after the state machine.  */
void pacifier();		/* Run the next step of the state machine.  */
void subliminal();	/* Print a subliminal message.  */

static int pac_inited = FALSE;	/* Has pac_init() been called?  */
static int pac_dirty = FALSE;	/* Has pacifier() been called?  */
static int state;
static int substate;
static int count;

void
pacifier()
{
	pac_dirty = TRUE;

	if (!pac_inited) {
		pac_init();
	}

	switch (state) {
	case FORWARD:
		switch (substate) {
		case N_S:
			FOOTPRINT;
			puts("|\010");
			substate = NE_SW;
			break;
		case NE_SW:
			FOOTPRINT;
			puts("/\010");
			substate = E_W;
			break;
		case E_W:
			FOOTPRINT;
			puts("-\010");
			substate = NW_SE;
			break;
		case NW_SE:
			FOOTPRINT;
			puts("\\\010");
			substate = NE_SW;
			break;
		default:
			puts("pacifier(): Illegal substate.\r\n");
			pac_init();
			break;
		} /* switch (substate) */

		if (++count > THRESHOLD) {
#if 0
			subliminal("Buy COHware!");
#endif
			puts(" \010\010");
			state = REVERSE;
			count = 0;
		}
		if (slow_flag) {
			wait_for_keystroke(2, -1);	/* Brief pause.  */
		}
		break;	
	case REVERSE:
		switch (substate) {
		case N_S:
			UNFOOTPRINT;
			puts("|\010");
			substate = NW_SE;
			break;
		case NW_SE:
			UNFOOTPRINT;
			puts("\\\010");
			substate = E_W;
			break;
		case E_W:
			UNFOOTPRINT;
			puts("-\010");
			substate = NE_SW;
			break;
		case NE_SW:
			UNFOOTPRINT;
			puts("/\010");
			substate = N_S;
			break;
		default:
			puts("pacifier(): Illegal substate.\r\n");
			pac_init();
			break;
		} /* switch (substate) */
		if (++count > THRESHOLD) {
			puts("_\010");
			state = FORWARD;
			count = 0;
		}
		if (slow_flag) {
			wait_for_keystroke(2, -1);	/* Brief pause.  */
		}
		break;
	default:
		puts("pacifier(): Illegal state.\r\n");
		pac_init();
		break;
	} /* switch (state) */

} /* pacifier() */


/*
 * Print a subliminal message on the console.
 * It does this by printing the message, backspacing over it,
 * spacing over it, and then backspacing again.
 */
void
subliminal(msg)
char *msg;
{
	int i, len;

	len = strlen(msg);
	puts(msg);

	for (i = len; i > 0; --i) {
		puts("\010");
	}

	if (slow_flag) {
		wait_for_keystrok(4, -1);	/* Brief pause.  */
	}

	for (i = len; i > 0; --i) {
		puts(" ");
	}

	for (i = len; i > 0; --i) {
		puts("\010");
	}
} /* subliminal() */

/*
 * Initialize the pacifier state machine.
 */
void
pac_init()
{
	pac_inited = TRUE;
	state = FORWARD;
	substate = N_S;
	count = 0;
} /* pac_init() */


/*
 * Clean up the screen after the pacifier.
 */
void
pac_cleanup()
{
	int i;

	if (pac_dirty) {
		puts("\r");
		for (i = THRESHOLD+1; i > 0; --i) {
			puts(" ");
		}
		puts("\r");
	}
	pac_dirty = FALSE;
} /* pac_cleanup() */
