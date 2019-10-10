#include <sys/al.h>
/*
 * the following kernel resident parts are shared by loadable serial drivers
 *   (see al.h and poll_clk.h)
 */
int	com_usage[NUM_AL_PORTS];	/* COM_UNUSED/COM_IRQ/COM_POLLED */
TTY	*(tp_table[NUM_AL_PORTS]);	/* table of pointers for polling */
int	poll_rate;			/* poll_* variables are explained */
int	poll_owner;			/* in poll_clk.h */
