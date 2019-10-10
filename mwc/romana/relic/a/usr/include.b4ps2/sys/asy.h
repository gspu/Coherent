/*
 * asy.h - support for 8250-family serial devices
 */
#ifndef ASY_H
#define ASY_H

#include <sys/silo.h>

extern int ASY_NUM;		/* patched to number of ports		*/
extern int ASYGP_NUM;		/* patched to number of port groups	*/

#ifdef _I386
#define MAX_ASY		32	/* maximum number of ports per driver	*/
#define MAX_ASYGP	4	/* maximum number of port groups	*/
#define MAX_SLOTS	16	/* maximum number of ports per group	*/
#else
#define MAX_ASY		8	/* maximum number of ports per driver	*/
#define MAX_ASYGP	2	/* maximum number of port groups	*/
#define MAX_SLOTS	8	/* maximum number of ports per group	*/
#endif
#define ASY_VERSION	4	/* driver and asypatch check this	*/

#define NO_ASYGP	99
#define NO_CHANNEL	99

#define PT_SIMPLE	0
#define PT_COMTROL	1
#define PT_ARNET	2
#define PT_GTEK		3
#define PT_DIGI		4
#define PT_MAX		5	/* one more than highest PT value used	*/

/*
 * Fields that need to be patched during installation.
 * These structs must be static.
 */
typedef struct asy0 {
	short	a_port;		/* i/o address of uart			*/
	char	a_irqno;	/* irq number, if any			*/
	char	a_speed;	/* default baud, from sgtty.h/termio.h	*/
	char	a_outs;		/* settings for MC_OUT1/2 when open	*/
	char	a_asy_gp;	/* group number, or NO_ASYGP if none	*/
	char	a_ixc;		/* if 1, needs excl use of irq (COM1-4)	*/
	char	a_nms;		/* if 1, no modem status interrupts	*/
} asy0_t;

typedef struct asy_gp {
	short	stat_port;
	char	gp_type;
	char	irq;
	char	chan_list[MAX_SLOTS];
} asy_gp_t;

/*
 * Fields that do not require patched initial values.
 * This struct can be dynamically allocated.
 */
typedef struct asy1 {
	silo_t	a_in;		/* raw input fifo			*/
	silo_t	a_out;		/* raw output fifo			*/
	TTY	a_tty;		/* stuff for line discipline		*/
	TIM	a_tim;		/* for irq timeout kluge		*/
	short	a_in_use;	/* increment with each open attempt	*/
	char	a_ut;		/* uart type				*/
	char	a_lcr;		/* lcr readback				*/
	int	a_irq:1;	/* true when open or hanging with irq's */
	int	a_has_irq:1;	/* irq vector is usable			*/
	int	a_hopn:1;	/* doing first open			*/
	int	a_hcls:1;	/* doing last close			*/
	int	a_ohlt:1;	/* CTS flow control has halted output	*/
	int	a_modc:1;	/* open with modem control		*/
	int	a_poll:1;	/* open in polled mode			*/
	int	a_flc:1;	/* open with RTS/CTS flow control	*/
} asy1_t;

/*
 * Each irq number has a linked list.
 * There is one node for each i/o port triggering the given irq number.
 */
struct irqnode {
	struct irqnode	*next;
	struct irqnode	*next_actv;
	int		(*func)();
	int		arg;
};

#endif
