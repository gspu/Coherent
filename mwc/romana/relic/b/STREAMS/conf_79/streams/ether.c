/* STREAMS style driver for the DP8390 Ethernet interface */
/*
 * An interesting problem is how to manage the interface address binding,
 * especially for upper protocols (specifically, the Internet ARP protocol)
 * that need to advertise the physical host address, especially in the
 * presence of multiple hardware adapters (or network types).
 *
 * For now, I am happy with having a specific version of ARP for each network
 * type, and having ARP catch any M_PROTO SP_BIND messages going past.
 */
#include <sys/kernel.h>
#include <sys/dma.h>
#include <sys/stream.h>
#include <sys/strmlib.h>
#include <sys/strproto.h>
#include <sys/dp8390.h>
#include <sys/ether.h>
#include <sys/ints.h>
#include <sys/netstuff.h>
#include <string.h>

/*
 * Allow up to 8 minor devices connected to us - each one receives a copy of
 * all incoming traffic, and each can place datagrams.
 */
#define MAXMINOR	8
static queue_t * minortab[MAXMINOR];

#ifdef M68K
/*
 * The 68070 uses memory mapped I/O exclusively, so treat the 8390 as a
 * structure at a fixed location in memory. The header file also defines
 * the padding of the structure on the 16-bit 68000 bus.
 *
 * We usually assign this to a local to put the base into an address
 * register, which gets faster code than using the folded immediate address.
 */
#define DP8390		volatile dp8390 *
#define DP8390_BASE 	((DP8390)(0x1FFC00 + PHYSICAL_SEG))
#define	DEVICE		_device
#define DEVICE_REG      DP8390 DEVICE = DP8390_BASE;

#define input(dev, dp_reg)	(dev)->dp_pg0rd.dp_reg
#define input1(dev, dp_reg)	(dev)->dp_pg1rdwr.dp_reg
#define output(dev, dp_reg, val)	(dev)->dp_pg0wr.dp_reg = val
#define output1(dev, dp_reg, val) 	(dev)->dp_pg1rdwr.dp_reg = val
#endif

#ifdef IBMPC
/*
 * The 8086 has special instructions for I/O, but if we treat the base
 * address as a NEAR mode pointer (16-bit, current data segment) then cast
 * it to a short for the I/O instructions, we'll have the right kind of
 * effect.
 *
 * We DON'T assign the base to a local, since better code is produced by
 * folding the constants and loading a new one into DX every I/O.
 *
 * Note that we emit a JMP $+2 just before the I/O instruction to waste a
 * little time, as a NIC CS should not occur more often than 400ns.
 */
#define DP8390 		dp8390 near *
#define DP8390_BASE 	((DP8390)0x300)
#define DEVICE		DP8390_BASE
#define DEVICE_REG

#define _input(addr)		(_DX=(UWORD)addr, __emit__ (0xEB, 0, 0xEB, 0, 0xEB, 0, 0xEB, 0, 0xEB, 0, 0xEC), _AL)
#define _output(addr,val)	(_DX=(UWORD)addr, _AL = val, __emit__ (0xEB, 0, 0xEB, 0, 0xEB, 0, 0xEB, 0, 0xEB, 0, 0xEE))
#define input(dev, dp_reg)	_input (&(dev)->dp_pg0rd.dp_reg)
#define input1(dev, dp_reg)	_input (&(dev)->dp_pg1rdwr.dp_reg)
#define output(dev, dp_reg, val)	_output (&(dev)->dp_pg0wr.dp_reg,(val))
#define output1(dev, dp_reg, val) 	_output (&(dev)->dp_pg1rdwr.dp_reg,(val))


#define ETHER_IRQ	2

#endif


/*
 * For scheduling remote DMA operations on the NIC, since there is only one
 * channel on the NIC which is shared between remote read and remote write
 * operations. Complicated since the TX buffer area is held until the NIC has
 * actually completed the transmission, and because RX DMA may be halted in the
 * middle of a packet due to STREAMS buffer depletion.
 *
 * Made a structure in the anticipation of one day having multiple interfaces.
 */
static struct ethcb {		/* control block for a single interface */
	char	tx_state;	/* TX DMA in progress or TX DMA blocked */
				/* cleared when packet send completed */
	char	rx_state;	/* RX DMA in progress or waiting for buffers */
	char	rx_pending;	/* flag that RX packets for DMA are waiting */
	short	rx_length;	/* remaining length of RX packet */
	UWORD	rx_base;	/* NIC buffer address of RX DMA */
	mblk_t	* rx_start;	/* first mblk in RX packet */
	mblk_t	* rx_current;	/* current RX DMA buffer */
	short	tx_length;	/* length of packet being transmitted */

	short	tx_count;	/* count packets sent */
	short	rx_count;	/* RX activity */

	char	tx_error;	/* packet never ended transmission */
	short	tx_lastcount;	/* count at last timer tick */
	short	rx_lastcount;	/* count at last timre tick */

/*
 * About write packet queueing - since this device effectively functions as a
 * kind of multiplexor, flow control needs explicit attention. I use the
 * general methods developed for the other multiplexing drivers (like TCP),
 * namely the generic scheduler in my STREAMS implementation.
 */
	/* schedule of driver queues that have output packets queued */
	struct schedule tx_sched;

	struct dp8390info dp8390info;

	/* useful addresses that we may wish to keep copies of */
	Eth_addr myaddr;		/* our own address */

	/* This flag gets set when the NIC tests OK */
	int netok;

	/* error counter extensions */
	int errcnt0, errcnt1, errcnt2;
} eth;


#define ETH	LONG_TO_PTR (eth, struct ethcb *)
#define QETH	((struct ethcb *) q->q_ptr)

/*
 * Possible states for TX
 */
#define TX_IDLE		0
#define TX_DMA		1
#define TX_SEND		2
/*
 * Possible events for TX
 */
#define TXE_CRANK	0
#define TXE_DMADONE	1
#define TXE_SENDDONE	2

/*
 * Possible states for RX.
 *
 * The two states RX_HALT and RX_BLOCKED distinguish whether or not to
 * continue RX dma after a TX DMA or not.
 */
#define RX_IDLE		0
#define RX_DMA		1
#define RX_HALT		2
#define RX_BLOCKED	3
/*
 * possible events for RX
 */
#define RXE_CRANK	0
#define RXE_GOTMEM	1
#define RXE_NOMEM	2
#define RXE_DMADONE	3
#define RXE_ALLDONE	4

/*
 * To aid in detecting when the cable has been unplugged (which causes a
 * constant collision state, so the backoff count doesn't get incremented).
 */
static P_TIMER eth_timer = NIL_TIMER;

/*
 * program the address into the address registers.
 * Note : assumes that register set 1 has already been chosen!
 */
static void set_addr (int selpage)
{
	DEVICE_REG

	if (selpage)
		output (DEVICE, dp_cr, CR_PS_P1 | CR_DM_ABORT | CR_STA);
	output1 (DEVICE, dp_par0, eth.myaddr.e[0]);
	output1 (DEVICE, dp_par1, eth.myaddr.e[1]);
	output1 (DEVICE, dp_par2, eth.myaddr.e[2]);
	output1 (DEVICE, dp_par3, eth.myaddr.e[3]);
	output1 (DEVICE, dp_par4, eth.myaddr.e[4]);
	output1 (DEVICE, dp_par5, eth.myaddr.e[5]);
	if (input1 (DEVICE, dp_par0) != eth.myaddr.e[0] ||
	    input1 (DEVICE, dp_par1) != eth.myaddr.e[1] ||
	    input1 (DEVICE, dp_par2) != eth.myaddr.e[2] ||
	    input1 (DEVICE, dp_par3) != eth.myaddr.e[3] ||
	    input1 (DEVICE, dp_par4) != eth.myaddr.e[4] ||
	    input1 (DEVICE, dp_par5) != eth.myaddr.e[5] )
		STREAMS_DEBUG ("Device not accepting physical address!\n");
	if (selpage)
		output (DEVICE, dp_cr, CR_PS_P0 | CR_DM_ABORT | CR_STA);
}

/*
 * Since the DMA channel to the DP8390 is word-wide, I have taken special care
 * to deal with odd-length subunits of packets, or packets that are not word
 * aligned with pullupmsg(). It's not fast, but such ugly messages should not
 * occur often.
 */

extern void streams_levels (void);

static void eth_timer_func (ULONG eth)
{
	char buf [4];
	TIMER_SET (eth_timer, 1, eth_timer_func, eth);

	if (ETH->tx_lastcount != ETH->tx_count)
		ETH->tx_lastcount = ETH->tx_count;
	else if (ETH->tx_state != TX_IDLE &&
		 ETH->tx_error == 0) {/* packet send taking too long */
		mblk_t * temp;

		ETH->tx_error = 1;

		/* dispose of all the packets queued here */
		while (ETH->tx_sched.s_head != NULL) {
			temp = getq (ETH->tx_sched.s_head);
			freemsg (temp);
			muxrobin (& ETH->tx_sched);
		}
#if 1
		STREAMS_DEBUG ("Stuck in TX ");
#endif
	}
	if (ETH->rx_lastcount != ETH->rx_count)
		ETH->rx_lastcount = ETH->rx_count;
	else if (ETH->rx_state != RX_IDLE &&
		 ETH->tx_error == 0) {
		ETH->tx_error = 1;
		STREAMS_DEBUG ("Stuck in RX ");
	}
}


static UWORD dma_base;		/* DMA base address in NIC */
static UWORD dma_len;		/* remote DMA length */
static void *dma_buf;		/* local buffer address */
static int dma_dir;		/* 1 => remote write */

/* start a piece of remote DMA, rounding the transfer size up */
void dma_start (void)
{
	DEVICE_REG
	int xfer_len;

	if (dma_dir) {
		output(DEVICE, dp_rbcr0, 15);		/* dummy byte count */
		output(DEVICE, dp_rbcr1, 0);
		output(DEVICE, dp_cr, CR_PS_P0 | CR_DM_RR | CR_STA);
//		for (xfer_len = 0; xfer_len < 5; xfer_len ++)
//			;
	}

	output(DEVICE, dp_rsar0, dma_base & 0xff);	/* remote DMA base */
	output(DEVICE, dp_rsar1, dma_base >> 8 );
	if ((xfer_len = dma_len) & 1)
		xfer_len++;
	output(DEVICE, dp_rbcr0, xfer_len & 0xff);	/* remote DMA count */
	output(DEVICE, dp_rbcr1, xfer_len >> 8 );

	output(DEVICE, dp_cr, dma_dir ?
		CR_PS_P0 | CR_DM_RW | CR_STA :
		CR_PS_P0 | CR_DM_RR | CR_STA);

	dma_setup (DMA_0, (char *)dma_buf, xfer_len >> 1,
			dma_dir ? DMAF_TODEV|DMAF_WORDWIDE|DMAF_BURST :
				DMAF_TOMEM|DMAF_WORDWIDE|DMAF_BURST);
}

/*
 * During test, we want to poll for DMA completion. Also used when reading
 * NIC packet header, since it's so short.
 */
void wait_for_dma (void)
{
	DEVICE_REG

	for (;;) {
		while ((input (DEVICE, dp_cr) &	CR_DM_ABORT) == 0)
			;
		if (dma_done (DMA_0) == -1)
			break;
		dma_reset (DMA_0);
		dma_start ();		/* restart */
	}
}

/*
 * retrieve NIC memory from "base" to "buffer"
 */
static INLINE void get_mem (int base, int len, void * buffer)
{
	dma_base = base;
	dma_len = len;
	dma_buf = buffer;
	dma_dir = 0;
	dma_start ();
}
static INLINE void set_mem (int base, int len, unsigned char * buf)
{
	dma_base = base;
	dma_len = len;
	dma_buf = buf;
	dma_dir = 1;
	dma_start ();
}

/* send streams message block "mp" to NIC */
static void send_block (queue_t * q, mblk_t * mp)
{
	/*
	 * identify the losers, and fix them with pullupmsg(), which both
	 * concatenates (fixing length) and aligns at the same time.
	 */
	if ((mp->b_cont != NULL && ((mp->b_wptr - mp->b_rptr) & 1) != 0) ||
			((ULONG)mp->b_rptr & 1) != 0)
		pullupmsg (mp, -1);

	dma_base = (QETH->dp8390info.dpi_tbuf << 8) + QETH->tx_length;
	dma_len = mp->b_wptr - mp->b_rptr;
	dma_buf = mp->b_rptr;
	dma_dir = 1;

	dma_start ();
}

/* initiate transmission of the buffered packet, with length "len". */
static void send_pkt (struct ethcb * eth)
{
	DEVICE_REG

	if (eth->tx_length < 64)	/* Ethernet magic */
		eth->tx_length = 64;
	output (DEVICE, dp_tbcr0, eth->tx_length & 0xff);
	output (DEVICE, dp_tbcr1, eth->tx_length >> 8);
	output (DEVICE, dp_tpsr, eth->dp8390info.dpi_tbuf);

	output (DEVICE, dp_cr, CR_PS_P0|CR_DM_ABORT|CR_TXP|CR_STA);
}

static void rx_next (struct ethcb * eth, int event);
static void tx_next (struct ethcb * eth, int event)
{
again:
	SCREEN (26) = 'T';
	SCREEN (28) = eth->tx_state + '0';
	SCREEN (30) = event + '0';
	switch (eth->tx_state) {
	case TX_IDLE:
		if (event == TXE_CRANK) {
			if (eth->tx_sched.s_head != NULL &&
			    eth->rx_state != RX_DMA) {
				eth->tx_count ++;
				eth->tx_state = TX_DMA;
				eth->tx_length = 0;
				send_block (eth->tx_sched.s_head,
					    eth->tx_sched.s_head->q_first);
			}
			return;
		}
		break;
	case TX_DMA:
		if (event == TXE_DMADONE) {
			mblk_t * temp = getq (eth->tx_sched.s_head),
			       * msg = temp->b_cont;

			eth->tx_length += dma_len;
			freeb (temp);
			if (msg != NULL) {
				putbq (eth->tx_sched.s_head, msg);
				send_block (eth->tx_sched.s_head, msg);
			} else {
				send_pkt (eth);		/* spit it out */
				muxrobin (& eth->tx_sched);
				eth->tx_state = TX_SEND;

				rx_next (eth, RXE_CRANK);
			}
			return;
		}
		if (event == TXE_CRANK)
			return;
		break;
	case TX_SEND:
		if (event == TXE_SENDDONE) {
			eth->tx_error = 0;
			eth->tx_state = TX_IDLE;
			rx_next (eth, RXE_CRANK);
			event = TXE_CRANK;
			goto again;
		}
		if (event == TXE_CRANK)
			return;
		break;
	}
	STREAMS_DEBUG ("Bad TX state/event ");
}

static void rx_got_mem (long eth)
{
	short s = SPL7 ();
	rx_next (LONG_TO_PTR (eth, struct ethcb *), RXE_GOTMEM);
	SPLX (s);
}
static void rx_next (struct ethcb * eth, int event)
{
	mblk_t * mp;
	int len;

again:
	SCREEN (26) = 'R';
	SCREEN (28) = eth->rx_state + '0';
	SCREEN (30) = event + '0';
	switch (eth->rx_state) {
	case RX_IDLE:
		if (event == RXE_CRANK) {
			if (eth->rx_pending && eth->tx_state != TX_DMA) {
				/* busy wait on the header part */
				struct rcvdheader nic;

				get_mem (eth->dp8390info.dpi_next << 8, 4, & nic);
				wait_for_dma ();
				eth->dp8390info.dpi_next = nic.rp_next;
				/*
				 * count in packet header includes FCS, which we
				 * are not really interested in.
				 */
				eth->rx_length = (nic.rp_rbch << 8) + nic.rp_rbcl - 4;
				eth->rx_base = dma_base + 4;
				if ((input (DEVICE, dp_isr) & ISR_RDC) == 0)
					STREAMS_DEBUG ("IMPOSSIBLE 1");
				output (DEVICE, dp_isr, ISR_RDC);

				eth->rx_count ++;
				eth->rx_state = RX_DMA;
				event = RXE_DMADONE;

				if (input (DEVICE, dp_isr) & ISR_RDC)
					STREAMS_DEBUG ("IMPOSSIBLE 2");
				goto again;
			}
			return;
		}
		break;
	case RX_DMA:
		len = eth->rx_length;
		if (len > 768) {
			if (len > 1024)
				len = 1024;
		} else if (len > 192) {
			if (len > 256)
				len = 256;
		} else if (len > 64)
			len = 64;
		if (event == RXE_DMADONE) {
			if (len == 0) {
				event = RXE_ALLDONE;
				goto again;
			}
			if ((mp = allocb (len, BPRI_MED)) == NULL) {
				while (len < 1024) {
					if ((mp = allocb (len, BPRI_LO)) != NULL)
						break;
					len *= 2;
				}
				if (len >= 1024) {
					event = RXE_NOMEM;
					goto again;
				}
				if (len > eth->rx_length)
					len = eth->rx_length;
			}

			eth->rx_count ++;
			mp->b_wptr += len;
			if (eth->rx_start == NULL)
				eth->rx_start = mp;		/* first buffer */
			else
				eth->rx_current->b_cont = mp;	/* link buffers */
			eth->rx_current = mp;
			get_mem (eth->rx_base, len, mp->b_rptr);
			/* take care to wrap the rx_base! */
			if (((eth->rx_base += len) >> 8) >= eth->dp8390info.dpi_pstop)
				eth->rx_base -= (eth->dp8390info.dpi_pstop - eth->dp8390info.dpi_pstart) << 8;
			eth->rx_length -= len;
			return;
		}
		if (event == RXE_NOMEM) {
			eth->rx_state = RX_HALT;
			bufcall (len, BPRI_MED, rx_got_mem, (long) eth);
			tx_next (eth, TXE_CRANK);
			event = RXE_CRANK;
			goto again;
		}
		if (event == RXE_ALLDONE) {
			/*
			 * finished a piece of receive DMA - advance buffer
			 * queue endpoint over just received packet. In
			 * addition, we should check to see if there are any
			 * more buffered receive packets.
			 */
			output (DEVICE, dp_bnry, eth->dp8390info.dpi_next == eth->dp8390info.dpi_pstart ?
						eth->dp8390info.dpi_pstop - 1 :
						eth->dp8390info.dpi_next - 1);
			output (DEVICE, dp_cr, CR_PS_P1 | CR_DM_ABORT | CR_STA);
			if (input1 (DEVICE, dp_curr) == eth->dp8390info.dpi_next)
				eth->rx_pending = 0;
			output (DEVICE, dp_cr, CR_PS_P0 | CR_DM_ABORT | CR_STA);
			if (eth->rx_start != NULL) {
				/* send a copy of every message upwards */
				for (len = 0; len < MAXMINOR ; len ++)
					if (minortab [len] &&
					    (mp = dupmsg (eth->rx_start)) != NULL)
						putq (minortab [len], mp);
				SCREEN (22) ++;
				freemsg (eth->rx_start);
				eth->rx_start = eth->rx_current = NULL;
			}
			eth->rx_state = RX_IDLE;
			tx_next (eth, TXE_CRANK);
			event = RXE_CRANK;
			goto again;
		}
		return;
	case RX_HALT:
		if (event == RXE_GOTMEM) {
			if (eth->tx_state == TX_DMA) {
				eth->rx_state = RX_BLOCKED;
				return;
			}
			eth->rx_state = RX_DMA;
			event = RXE_DMADONE;
			goto again;
		}
		if (event == RXE_CRANK)
			return;
		break;
	case RX_BLOCKED:
		if (event == RXE_CRANK && eth->tx_state != TX_DMA) {
			eth->rx_state = RX_DMA;
			event = RXE_DMADONE;
			goto again;
		}
		break;
	default:
		STREAMS_DEBUG ("Impossible state");
		return;
	}
	STREAMS_DEBUG ("Bad RX state/event combo ");
}


/* Referenced in "system.asm", here is the interrupt service routine */
HW_INT_FUNC (void) etherint (void)
{
	DEVICE_REG
	UBYTE	status = input (DEVICE, dp_isr);
static	UBYTE	reentry;

	if (reentry ++ != 0) {
		STREAMS_DEBUG ("Reentry ");
		reentry --;
		return;
	}

	/*
	 * GCC - specific kernel hack, save all used registers on entry to
	 * this function. We don't use INTERRUPT for the PC version since
	 * hardware IRQ handlers need assembly-language wrappers to give
	 * us enough stack to use STREAMS library routines.
	 */
	GNU_INTERRUPT;

rescan:
	/* reset the flag bits that were indicated for us */
	output (DEVICE, dp_isr, status);

	if (eth.netok == 0) {			/* device not active! */
		reentry --;
		return;
	}
	if ((status & ISR_PRX) != 0) {
		eth.rx_pending = 1;
		rx_next (& eth, RXE_CRANK);
	}
	SCREEN (16) ++;
	if ((status & ISR_PTX) != 0) {
		if ((input (DEVICE, dp_tsr) & ~(TSR_COL | TSR_CRS | TSR_PTX | TSR_DFR))
		    != 0)
			STREAMS_DEBUG ("Packet TX error\n");
		SCREEN (18) ++;
		tx_next (& eth, TXE_SENDDONE);
	}
	/*
	 * The ISR_RDC bit is gated with CR_DM_ABORT since the fix for the
	 * NIC remote write problem causes spurious RDCs, which are not
	 * normally detected unless the TX DMA is delayed by arrive DMA to
	 * the NIC local memory.
	 */
	if ((status & ISR_RDC) != 0 &&
	    (input (DEVICE, dp_cr) & CR_DM_ABORT) != 0)
		if (dma_done (DMA_0) != -1) {
			dma_reset (DMA_0);
			dma_start ();
		} else if (dma_dir)
			tx_next (& eth, TXE_DMADONE);
		else
			rx_next (& eth, RXE_DMADONE);

	if ((status & (ISR_RXE | ISR_TXE | ISR_OVW | ISR_CNT)) != 0) {
		if (status & ISR_RXE)
			STREAMS_DEBUG ("Receive error\n");
		if (status & ISR_TXE) {
			/*
			 * This can happen due to excessive collisions if
			 * there is an open cable end or due to a FIFO
			 * underrun (which indicates a board problem).
			 */
			if ((input (DEVICE, dp_tsr) & (TSR_FU)) != 0)
				STREAMS_DEBUG ("TX FIFO underrun");
			tx_next (& eth, TXE_SENDDONE);
		}
		if (status & ISR_OVW)
			STREAMS_DEBUG ("Buffer overflow\n");
		if (status & ISR_CNT) {
			/* reset the error counters by reading them */
			eth.errcnt0 += input (DEVICE, dp_cntr0);
			eth.errcnt1 += input (DEVICE, dp_cntr1);
			eth.errcnt2 += input (DEVICE, dp_cntr2);
		}
	}
	if ((status = input (DEVICE, dp_isr)) != 0)
		goto rescan;

	reentry --;
}

/*
 * Routines for reading from/writing to the NIC memory.
 */

/* queue a packet for transmission */
static void queue_packet (queue_t *q, mblk_t *mp)
{
	if (QETH->tx_error != 0)
		return;

	short s = SPL7 ();
	/*
	 * We queue the message "normally" for STREAMS so that flow control
	 * operates as one would expect. Then we place the streams queue onto
	 * our own "scheduling" list for processing by the NIC interrupt.
	 */
	putq (q, mp);
	qschedule (q, & QETH->tx_sched);

	/*
	 * If there is no I/O presently being executed, we must start
	 * the ball rolling.
	 */
	tx_next (QETH, TXE_CRANK);
	SPLX (s);
}

/*
 * Set up the DP8390. This initialisation procedure comes fairly directly
 * from the NatSemi manual - much dark magic.
 */
static void chipinit (Eth_addr * addr)
{
	unsigned char * testbuf, * test2;
	mblk_t * testmem;
	int i, start = -1, len;
	DEVICE_REG

	if (input (DEVICE, dp_cr) == 0xFF) {
		eth.netok = -1;
		return;
	}

	/* reset dp8390 */
	output(DEVICE, dp_cr, CR_STP|CR_PS_P0|CR_DM_ABORT);

#ifdef M68K
	output(DEVICE, dp_dcr,
		DCR_LOOP | DCR_WORDWIDE | DCR_BIGENDIAN | DCR_8BYTES);
#endif
#ifdef IBMPC
	set_hw_int (ETHER_IRQ, etherint);
	output(DEVICE, dp_dcr, DCR_LOOP | DCR_WORDWIDE | DCR_8BYTES);
#endif

	output(DEVICE, dp_rbcr0, 0);
	output(DEVICE, dp_rbcr1, 0);
	output(DEVICE, dp_rcr, RCR_AB);
	output(DEVICE, dp_tcr, TCR_INTERNAL);
	output(DEVICE, dp_isr, 0xff);
	output(DEVICE, dp_imr, 0);

	/*
	 * We start the device here (and shut it down later) since the
	 * Remote DMA doesn't work until we do, and we can't find out how
	 * much memory we have until we can DMA. (Sigh)
	 */
	output (DEVICE, dp_cr, CR_PS_P0 | CR_DM_ABORT | CR_STA);

	/*
	 * Since the NIC is in internal loopback mode (TCR_INTERNAL), and
	 * interrupts are disabled, we can now test the NIC's memory to
	 * determine how much there is.
	 */
	testmem = getbuf (1024);	/* get temporary space */
	testbuf = testmem->b_rptr;
	test2 = testbuf + 256;

	for (i = 0 ; i < 256 ; i ++)
		testbuf [i] = i;
	for (i = 0 ; i < 256 ; i ++) {
		testbuf [0] = i;
		testbuf [1] = ~i;
		set_mem (i << 8, 256, testbuf);
		wait_for_dma ();
	}

	/* find start of memory */
	for (i = 0 ; i < 256 ; i++) {
		testbuf [0] = i;
		testbuf [1] = ~i;
		get_mem (i << 8, 256, test2);
		wait_for_dma ();
		if (memcmp (test2, testbuf, 256) == 0)
			/* got one - either start or extend a span */
			if (start < 0) {
				start = i;
				len = 1;
			} else
				len++;
		else
			if (start >= 0)
				break;
	}
	if (len < 16) {		/* needs at least 4k to work */
		eth.netok = -1;
		goto alldone;
	}

	eth.dp8390info.dpi_pstart = start + 8;
	eth.dp8390info.dpi_pstop = start + len;
	eth.dp8390info.dpi_tbuf = start;

	/*
	 * Since we're at it... if the board has an address PROM, it should
	 * be at address 0 (where the NIC has trouble locating RAM), so
	 * unless we're given an address, try finding our location.
	 */
	if (addr != NULL)
		eth.myaddr = * addr;
	else if (start > 0) {		/* may have PROM, there's no RAM */
		get_mem (0, 12, testbuf);
		wait_for_dma ();

		/* Fetch the PROM data from every second word */
		for (i = 0;i < 6;i++)
			eth.myaddr.e [i] = testbuf [i + i];
	}

	STREAMS_DEBUG_LONG (len << 8, 16);
	STREAMS_DEBUG ("h bytes of Ethernet memory, Address = $");
	for (i = 0 ; i < 6 ; i ++)
		STREAMS_DEBUG_LONG (eth.myaddr.e [i], 16);
	STREAMS_DEBUG (", IRQ ");
	STREAMS_DEBUG_LONG (ETHER_IRQ, 10);
	STREAMS_DEBUG ("\r\n");

	output (DEVICE, dp_isr, 0xFF);
	output (DEVICE, dp_tpsr, eth.dp8390info.dpi_tbuf);
	output (DEVICE, dp_pstart, eth.dp8390info.dpi_pstart);
	output (DEVICE, dp_bnry, eth.dp8390info.dpi_pstart);
	output (DEVICE, dp_pstop, eth.dp8390info.dpi_pstop);

	output (DEVICE, dp_cr, CR_PS_P1|CR_DM_ABORT|CR_STP);
	output1 (DEVICE, dp_curr, eth.dp8390info.dpi_pstart + 1);
	eth.dp8390info.dpi_next = eth.dp8390info.dpi_pstart + 1;

	set_addr (0);

	output (DEVICE, dp_cr, CR_PS_P0|CR_DM_ABORT|CR_STA);
	output (DEVICE, dp_tcr, 0);
	output (DEVICE, dp_imr, 0x7F);	/* all interrupts */

	if ((eth_timer = TIMER_ALLOC ()) != NIL_TIMER)
		TIMER_SET (eth_timer, 1, eth_timer_func, (long) & eth);
	eth.netok ++;

alldone:
	freemsg (testmem);
}

/*
 * Shut down the E'net chip. Whenever I get around to it, this will send
 * the ARP-style packet that indicates this station is dead'n'gone. This
 * will use the poll-type DMA access shown in the open.
 */
static void chipoff (void)
{
	DEVICE_REG

	if (eth.netok < 0)
		return;
#ifdef IBMPC
	set_hw_int (ETHER_IRQ, NULL);
#endif /* IBMPC */
	output (DEVICE, dp_imr, 0);
	output (DEVICE, dp_cr, CR_PS_P0 | CR_DM_ABORT | CR_STP);
}

/*
 * stream open routine for the driver
 */
static int ethopen (queue_t * q, dev_t dev, int /* flag */, int sflag)
{
	/* refuse to do anything unless everything checked out OK at boot */
	W (q)->q_ptr = q->q_ptr = & eth;
	if (QETH->netok < 0)
		return OPENFAIL;
	if (QETH->netok == 0) {
		chipinit (NULL);
		if (QETH->netok < 0)
			return OPENFAIL;

	} else
		QETH->netok ++;
	/* allow regular or clone device open */
	if (sflag == CLONEOPEN) {
		for (dev = 0;dev < MAXMINOR;dev++)
			if (minortab[dev] == NULL)
				break;
	} else
		dev = minor(dev);
	if (dev < 0 || dev >= MAXMINOR)
		return OPENFAIL;

	minortab[dev] = q;
	return dev;
}

/* ethernet write put procedure */
static void ethwput (queue_t * q, mblk_t * mp)
{
	Framehdr * frame;
	struct sp_bind * spbind;
	Eth_addr * addr;

	switch (mp->b_datap->db_type) {
	case M_PROTO:
		spbind = (struct sp_bind *)mp->b_rptr;
		addr = (Eth_addr *)(spbind + 1);
		if (spbind->protoid == SP_BIND) {
			if (spbind->family != AF_ENET) {
				freemsg (mp);
				return;
			}
			if (mp->b_wptr > (unsigned char *) addr) {
				QETH->myaddr = * addr ++;
				set_addr (1);
			}
			freemsg (mp);
			mp = allocb (sizeof (struct sp_bind) + sizeof (Eth_addr),
				     BPRI_MED);
			if (mp != NULL) {
				spbind = (struct sp_bind *)mp->b_rptr;
				addr = (Eth_addr *)(spbind + 1);
				spbind->protoid = SP_BIND;
				spbind->family = AF_ENET;
				spbind->connect = TPS_BOUND;
				* addr ++ = QETH->myaddr;
				mp->b_wptr = (unsigned char *) addr;
				mp->b_datap->db_type = M_PROTO;
				qreply (q, mp);
			}
			return;
		}
	default:
nogood:
		freemsg (mp);
		break;
	case M_DATA:
		frame = (Framehdr *)mp->b_rptr;
		if ((unsigned char *)(frame + 1) > mp->b_wptr)
			goto nogood;
		SCREEN (20) ++;
		frame->f_srcaddr = QETH->myaddr;
		queue_packet (q, mp);
		break;
	case M_FLUSH:
		/* canonical flush processing */
		if ((* mp->b_rptr & FLUSHW) != 0)
			flushq (q, FLUSHDATA);
		if ((* mp->b_rptr & FLUSHR) != 0) {
			flushq (RD(q), FLUSHDATA);
			* mp->b_rptr &= ~FLUSHW;
			qreply (q, mp);
		} else
			freemsg (mp);
		break;
	case M_IOCTL:
		mp->b_datap->db_type = M_IOCNAK;
		qreply (q, mp);
		break;
	}
}

/* read service routine */
static void ethrsrv (queue_t * q)
{
	mblk_t * mp;

	while ((mp = getq (q)) != NULL)
		if (canput (q->q_next))
			putnext (q, mp);
		else {
			putbq (q, mp);
			break;
		}
	SCREEN (24) ++;
}

/* close procedure - called on last close of stream */
static int ethclose(queue_t *q)
{
	int i;

	if (QETH->netok == 1)
		chipoff ();
	QETH->netok --;
	for (i = 0;i < MAXMINOR;i++)
		if (minortab[i] == q)
			break;
	minortab[i] = NULL;
	return(0);
}

static struct module_info ethrinfo = {
	0, "ether", 0, 1024, 1024, 0
};
static struct qinit ethread = {
	NULLFUNC, ethrsrv, ethopen, ethclose, NULLFUNC,
	&ethrinfo, NULLSTAT
}, ethwrite = {
	ethwput, NULLFUNC, NULLFUNC, NULLFUNC, NULLFUNC,
	&ethrinfo, NULLSTAT
};
struct streamtab ethinfo = {
	&ethread, &ethwrite, NULLINIT, NULLINIT
};
