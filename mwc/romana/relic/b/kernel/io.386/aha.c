/*
 * This is the host adaptor specific portion of the
 * Adaptec AHA154x driver.
 *
 * $Log:	aha.c,v $
 * Revision 1.9  93/04/16  06:57:55  bin
 * Hal: kernel 76 update
 * 
 * Revision 1.1  93/03/18  10:31:13  root
 * r74
 * 
 * Revision 1.1	91/04/30  11:01:41	root
 * Shipped with COH 3.1.0
 * 
 */
#include <sys/coherent.h>
#include <sys/buf.h>
#include <sys/sched.h>

#include <sys/scsiwork.h>
#include <sys/aha154x.h>

#ifdef _I386
extern char *palloc();
#endif /* _I386 */

#ifndef _I386
extern	saddr_t	sds;		/* System Data Selector */
static	paddr_t	sds_physical;	/* as physical address */
#endif /* _I386 */
static	short	aha_i_o_base;
static	char	aha_loaded;	/* did load() find a host adaptor? */
static	char	dev_bit_map[8];	/* one byte per SCSI-ID; one bit per LUN */
char	drive_info[MAX_SCSI_ID * MAX_LUN]; /* "per drive" info/flags */

void	aha_intr();		/* interrupt service routine */

#define	MIN_MAILBOX	1
int	MAX_MAILBOX = { 8 };	/* tunable value */

static	scsi_work_t	*scsi_work_queue;
static	mailentry	*mailbox_in, *mailbox_out;
static	char		*aha_err_msg = { "no message" };

static	long	aha_timeout[] = { 
#define	TIMEOUT_PRESENT	0
	0x30000L,
#define	TIMEOUT_SENDCMD	1
	0x10000L,
#define	TIMEOUT_POLL	2
	0x100L
};

/*
 * NIGEL: The 'ccb_t' structure defined in <sys/aha154x.h> has data for the
 * SCSI controller, at least at the base. Presumably the 'ccb_sw' member is
 * not for the SCSI controller, so data after the 'cmd_status' member is
 * for the driver. The 'buffer' member is not used....
 *
 * Below I create a wrapper structure that separates driver-private data from
 * the Adaptec 'ccb' like the original author should have written. This allows
 * the 'ccb' to be threaded on a work list, and would obviate the need for the
 * stupid machinery in "i386/mem_cache.c" (although not even that would be
 * needed had page management been done right...)
 *
 * The primary motivation for this is to allow ccb's to be deallocated safely.
 * The 286 version of the driver deallocated the structures in aha_process (),
 * which was deferred from aha_intr (). The 4.0 version can not do that because
 * due to the large size of the buffer cache a defer-table overflow might
 * result. We compromise by creating a deferred work list and threading the
 * ccb's on that and only deferring the 'start work' operation. Since
 * aha_process () does no actual work but merely calls bdone () then frees the
 * data, we create a 'free list' of ccb's and a deferred routine to free them,
 * and leave aha_process () to call bdone () at interrupt level to give maximum
 * throughput.
 *
 * You are now passing the Hack City limits. You are now in the Interdicted
 * Zone.
 */

#include <stddef.h>			/* import offsetof () */

typedef struct driver_ccb	drv_ccb_t;

struct driver_ccb {
	ccb_t		aha_ccb;	/* the driver CCB */
	drv_ccb_t     *	next;		/* for threading on work list */
};
	
static	drv_ccb_t     *	free_list;	/* ccb's to free */
static	int		free_active;	/* nonzero if cleanup routine active */

/*
 * There are two low-level allocators in use, one for 3.2 and one for 4.0,
 * and rather than #ifdef the usage we call them via a macro below.
 */

#ifdef	_I386
#define	AHA_ALLOC(size)		palloc (size)
#define	AHA_FREE(size)		pfree (size)
#else
#define	AHA_ALLOC(size)		kalloc (size)
#define	AHA_FREE(size)		kfree (size)
#endif


/*
 * Cleanup routine for free list, called via defer () from ccb_free ().
 *
 * Many fields of active ccb's are filled in with other dynamically allocated
 * structures; this code has responsibility for freeing all of them.
 */
void
ccb_cleanup ()
{
	drv_ccb_t     *	work;
	int		s;

	s = sphi ();
	while ((work = free_list) != NULL) {
		free_list = work->next;
		spl (s);

#ifdef	_I386
		/*
		 * The following code is insane; the DSL stuff should be part
		 * of this ccb system. This will be fixed when a total revamp
		 * of memory management occurs, and the "mem_cache.c" stuff
		 * gets spaced.
		 *
		 * In the original code, the ccb_forget () was done after the
		 * ccb was totally freed. I put it here to save conditionals.
		 */
		dsl_free (work->aha_ccb.dataptr);
		ccb_forget (& work->aha_ccb);
#endif

		if (work->aha_ccb.ccb_sw != NULL)
			AHA_FREE (work->aha_ccb.ccb_sw);
		AHA_FREE (work);

		s = sphi ();
	}

	free_active = 0;		/* defer () needed to reschedule */
	spl (s);
}


/*
 * Allocate a ccb and return a pointer to it. Call from base level only.
 * The scsi_work_t * value passed here is remembered so that when the ccb
 * is deallocated it will be too. In addition, code in aha_process () uses
 * this stored value to map back from a ccb to a buffer-cache entry.
 */
ccb_t *
ccb_alloc (sw)
scsi_work_t   *	sw;
{
	drv_ccb_t     *	drvccb;

	if ((drvccb = (drv_ccb_t *) AHA_ALLOC (sizeof (* drvccb))) == NULL)
		return NULL;

	/*
	 * Remember the 'sw' value. Note that this really should be put in
	 * our wrapper structure, but that is to be fixed later.
	 */

	drvccb->aha_ccb.ccb_sw = sw;

	/*
	 * Return a pointer to the inner adaptec ccb.
	 */

	return & drvccb->aha_ccb;
}


/*
 * Return a ccb to the free pool. Callable from base or interrupt level. All
 * the dynamically allocated member data of the ccb should be freed by this
 * routine.
 */
void
ccb_free (ccb)
ccb_t	      *	ccb;
{
	int		s;
	drv_ccb_t     *	work;

	/*
	 * Perform a portable downcast from the aha_ccb to the base structure.
	 */

	work = (drv_ccb_t *) ((char *) ccb - offsetof (drv_ccb_t, aha_ccb));

	s = sphi ();
	work->next = free_list;
	free_list = work;

	if (free_active == 0) {
		defer (ccb_cleanup, 0);
		free_active = 1;
	}
	spl (s);
}


/*
 * Map from a ccb to the 'scsi_work_t' that was memoized when the ccb was
 * allocated.
 */

#define	ccb_to_scsiwork(ccb)	(ccb->ccb_sw)

/*
 * NIGEL: Welcome back to Hack City. Beware of mutant code!
 */

#if	0
static
OUTB(port, value)
short port;
{	printf("<O(%x,%x)>", port, value);
	outb(port, value);	}
INB(port)
short port;
{	register int i = inb(port);
	printf("<I(%x)=%x>", port, i);
	return i;	}
#else
#define	OUTB(port, value)	outb(port, value)
#define	INB(port)	inb(port)
#endif

#ifdef TRACER
#define	SETMSG(msg)	aha_err_msg = msg

static char *
aha_last_msg()
{
	T_PIGGY(0x8000, return aha_err_msg;);

	return "error messages not verbose";
}

#else /* TRACER */

#define	SETMSG(msg)
static char *
aha_last_msg()
{
	return "error messages not verbose";
} 
#endif /* TRACER */

static
int
no_mem()
{
	printf("aha154x: out of kernel memory\n");
}

int
aha_set_base(base)
{
	register int i;

	i = aha_i_o_base;
	aha_i_o_base = base;
	return i;
}

int
aha_get_base()
{
	return aha_i_o_base;
}

aha_process(ccb)
	ccb_t *ccb;
{
	register scsi_work_t *sw;
	register BUF *bp;

	if ((sw = ccb_to_scsiwork (ccb)) == NULL) {
		ccb->opcode = AHA_OP_INVALID;
		wakeup(ccb);
		return;
	}

	bp = sw->sw_bp;

	if((ccb->hoststatus != 0) || (ccb->targetstatus != 0)) {
		if(--sw->sw_retry > 0
		   || (ccb->targetstatus == CHECK_TARGET_STATUS
		   && ccb->cmd_status[12] == SENSE_UNIT_ATTENTION)) {
			int s = sphi();
			if(scsi_work_queue->sw_actf == NULL) {
				scsi_work_queue->sw_actf = sw;
			} else {
				scsi_work_queue->sw_actl->sw_actf = sw;
			}
			scsi_work_queue->sw_actl = sw;
			spl(s);
			aha_start();
			return;
		}
		bp->b_flag |= BFERR;
	} else {
		bp->b_resid = 0;
	}

	bdone(bp);
	ccb_free(ccb);
}

static
int	aha_1out(value)
{
	register int i;

	while((i = INB(aha_i_o_base + AHA_STATUS) & AHA_CDOPFULL) != 0)
		if((i & AHA_INVDCMD)
		 || (INB(aha_i_o_base+AHA_INTERRUPT) & AHA_CMD_DONE))
			return -1;
	OUTB(aha_i_o_base + AHA_WRITE, value);
	return 0;
}

static
int	aha_1in()
{
	register int i;

	while((i = INB(aha_i_o_base + AHA_STATUS) & AHA_DIPFULL) == 0)
		if((i & AHA_INVDCMD)
		 || (INB(aha_i_o_base+AHA_INTERRUPT) & AHA_CMD_DONE))
			return -1;
	return INB(aha_i_o_base + AHA_READ) & 0xFF;
}

static
void	aha_cmd_out(value)
{
	register long l;
	register int i;

	for(l = aha_timeout[TIMEOUT_SENDCMD]; --l > 0;) {
		if(((i=INB(aha_i_o_base + AHA_STATUS))
		    & AHA_SCSIIDLE) != 0) {
			aha_1out(value);
			return;
		}
	}
	SETMSG("timeout sending cmd byte");
	printf("aha154x: timeout sending cmd byte\n");
}

static
int	aha_poll()
{
	register int i;
	register int l = aha_timeout[TIMEOUT_POLL];
	while((--l > 0)
	  &&  ((i = INB(aha_i_o_base + AHA_INTERRUPT)) & AHA_CMD_DONE) == 0)
		;
	if(l == 0)
		printf("aha154x: aha_poll timed out\n");

	i = INB(aha_i_o_base + AHA_STATUS);
	OUTB(aha_i_o_base + AHA_CONTROL, AHA_INTRRESET);
	return i;
}

static
void	aha_get_data(vec, cnt)
char	*vec;
int	cnt;
{
	while(--cnt >= 0)
		*vec++ = aha_1in();
	aha_poll();
}

static
int	aha_present()
{
	long	l;

	if(INB(aha_i_o_base) == 0xFF) {
		SETMSG("no adapter found at io base");
		return -3;
	}
	for(l = aha_timeout[TIMEOUT_PRESENT];
		(--l > 0) && (INB(aha_i_o_base + AHA_STATUS) & AHA_SELFTEST);)
			;
	if(l == 0) {
		SETMSG("selftest not completed");
		return -1;
	}
	if(INB(aha_i_o_base + AHA_STATUS) & AHA_DIAGFAIL) {
		SETMSG("diagnostics failed");
		return -2;
	}
	if(INB(aha_i_o_base + AHA_STATUS) & AHA_INITMAIL) {
		SETMSG("mailbox initialization needed");
		return 1;
	}
	if(INB(aha_i_o_base + AHA_STATUS) & AHA_SCSIIDLE) {
		SETMSG("adaptor okay, idle");
		return 0;
	}
	SETMSG("unknown status at start");
	return -4;
}

void
aha_l_to_p3(value, vec)
	paddr_t	value;
	P3 vec;
{
	register int i;

	for(i = 3; --i >= 0;) {
		vec[i] = value & 0xFF;
		value >>= 8;
	}
}

long
aha_p3_to_l(vec)
	P3 vec;
{
	register int i;
	register long retval;

	retval = 0;
	for(i = 0; i < 3; ++i) {
		retval <<= 8;
		retval |= vec[i];
	}

	return(retval);
} /* aha_p3_to_l() */

#ifndef _I386 /* All of aha_p3_to_v().  */
static char *
aha_p3_to_v(vec)
	P3 vec;
{
	paddr_t adr;

	adr = vec[0];
	adr <<= 16;
	adr |= (vec[1]<<8) | vec[2];
	adr -= sds_physical;
	return (char *)adr;
}
#endif /* _I386 */

aha_device_info()
{
	register int i;
	static char buf[256];

	aha_cmd_out(AHA_DO_GET_DEVICES);
	aha_get_data(&buf[0], 8);
	for(i = 0; i < 8; ++i)
		if(buf[i] != 0)
			printf("[%d] %x ", i, buf[i]);
	printf("\n");
}

int	aha_unload(ireq)
{
	/*
	 *	we should really verify that everything
	 *	out there gets flushed.
	 */
	if (!aha_loaded)
		return;
	if(mailbox_out) {
		AHA_FREE (mailbox_out);
		mailbox_out = 0;
	}
	clrivec(ireq);
}

int	aha_load(dma, ireq, base, head)
scsi_work_t *head;
{
	register int	i;
	unsigned char	adr[4];

	aha_set_base(base);
	if(mailbox_out == 0) {
		if ((mailbox_out = 
		     AHA_ALLOC (2 * MAX_MAILBOX * sizeof(mailentry))) == 0) {
			no_mem();
			return -1;
		} else {
			mailbox_in = &mailbox_out[MAX_MAILBOX];
		}
	}

	for(i = 0; i < MAX_MAILBOX; ++i)
		mailbox_out[i].cmd = mailbox_in[i].cmd = 0;

#ifdef _I386
	aha_l_to_p3(vtop(mailbox_out), &adr[1]);
#else /* _I386 */
	sds_physical = VTOP2(0, sds);
	aha_l_to_p3(VTOP2(mailbox_out, sds), &adr[1]);
#endif /* _I386 */

	adr[0] = MAX_MAILBOX;

	/*
	 * setup HW
	 */
	setivec(ireq, aha_intr);

	outb(0xD6, 0xC1);		/* DMA is currently hard coded for */
	outb(0xD4, 0x01);		/* DMA channel 5 */


	OUTB(aha_i_o_base+AHA_CONTROL, AHA_HARDRESET);
	if (aha_present() < 0) {
		printf("aha154x: initialization error or host adaptor not ");
		printf("found at 0x%x\n", aha_i_o_base);
		return -1;
	}
	aha_cmd_out(AHA_DO_MAILBOX_INIT);
	for(i = 0; i < 4; ++i)
		aha_1out(adr[i]);
	scsi_work_queue = head;
	++aha_loaded;
	return MAX_MAILBOX;
}

aha_command(sc)
register scsi_cmd_t *sc;
{
	register int i;
	/* register */ ccb_t *ccb;

	short	count = sc->blklen;
	long	block = sc->block;

	if ((ccb = ccb_alloc (NULL)) == NULL) {
		no_mem();
		return -1;
	}

#ifdef _I386
	ccb->opcode = AHA_OP_SIC_SG;		/* SCSI_INITIATOR*/
#else /* _I386 */
	ccb->opcode = AHA_OP_SIC;		/* SCSI_INITIATOR*/
#endif /* _I386 */
	ccb->target = (sc->unit & 0x1C) << 3;	/* SCSI ID */
	ccb->target |= sc->unit & 0x3;		/* LUN */
	if((ccb->cmd_status[0] = sc->cmd) == ScmdWRITEXTENDED) {
		ccb->target |= AHA_CCB_DATA_OUT;
	} else { /* READEXT, READCAP, INQUIRY */
		ccb->target |= AHA_CCB_DATA_IN;
	}
	ccb->cmd_status[1] = 0;
	ccb->cmd_status[2] = block;
	ccb->cmd_status[3] = block >>16;
	ccb->cmd_status[4] = block >> 8;
	ccb->cmd_status[5] = block;
	ccb->cmd_status[6] = 0;
	ccb->cmd_status[7] = count / 512;
	ccb->cmd_status[8] = count;
	ccb->cmd_status[9] = 0;
	ccb->cmdlen = 10;
	ccb->senselen = MAX_SENSEDATA;

#ifdef _I386
	dsl_gen(ccb->dataptr, ccb->datalen, sc->buffer, (long)sc->buflen);
	aha_l_to_p3(vtop(ccb), mailbox_out[0].adr);
	ccb_remember(ccb, mailbox_out[0].adr);
#else /* _I386 */
	aha_l_to_p3((long)sc->buflen, ccb->datalen);
	aha_l_to_p3(sc->buffer, ccb->dataptr);
	aha_l_to_p3(VTOP2(ccb, sds), mailbox_out[0].adr);
#endif /* _I386 */

	mailbox_out[0].cmd = MBO_TO_START;

	/* Start the AHA-154x scanning the mailboxes.  */
	aha_1out(AHA_DO_SCSI_START);

	/* Wait for this ccb to finish.  */
	while(ccb->opcode != AHA_OP_INVALID) {
#ifdef _I386
		x_sleep(ccb, pridisk, slpriNoSig, "aha:ccb");
#else
		v_sleep(ccb, CVBLKIO, IVBLKIO, SVBLKIO, "aha:ccb");
#endif
		/* The AHA-154x driver is waiting for a ccb to complete.  */
	}
	
	if((ccb->targetstatus == CHECK_TARGET_STATUS)
	   && (ccb->cmd_status[12] != SENSE_UNIT_ATTENTION)) {
		printf("aha: SCSI ID %d LUN %d. SCSI sense =",
		(sc->unit >> 2), sc->unit & 0x3);
		for(i = 0; i < ccb->senselen; ++i)
			printf(" %x", ccb->cmd_status[10+i]);
		printf("\n");
	}
	i = ccb->hoststatus | ccb->targetstatus;

	/*
	 * NIGEL: If you are worried that the ccb memory is not getting freed
	 * soon enough, add a parameter to ccb_cleanup () to flag whether it
	 * should clear the active flag and call it directly here.
	 */
	ccb_free(ccb);

	return i;
}

ccb_t	*buildccb(sw)
register scsi_work_t *sw;
{
	register ccb_t *ccb;

	if ((ccb = ccb_alloc(sw)) == NULL)
		return NULL;

#ifdef _I386
	ccb->opcode = AHA_OP_SIC_SG;		/* SCSI_INITIATOR*/
#else /* _I386 */
	ccb->opcode = AHA_OP_SIC;		/* SCSI_INITIATOR*/
#endif /* _I386 */

	ccb->target = (sw->sw_drv & 0x1C) << 3;	/* SCSI ID */
	ccb->target |= (sw->sw_drv) & 0x3;	/* LUN */
	if(sw->sw_bp->b_req == BREAD) {
		ccb->target |= AHA_CCB_DATA_IN;
		ccb->cmd_status[0] = ScmdREADEXTENDED;
	} else {
		ccb->target |= AHA_CCB_DATA_OUT;
		ccb->cmd_status[0] = ScmdWRITEXTENDED;
	}
	ccb->cmd_status[2] = 0;
	ccb->cmd_status[3] = sw->sw_bno >>16;
	ccb->cmd_status[4] = sw->sw_bno >> 8;
	ccb->cmd_status[5] = sw->sw_bno;
	ccb->cmd_status[6] = 0;
	ccb->cmd_status[7] = sw->sw_bp->b_count / (512*256L);
	ccb->cmd_status[8] = sw->sw_bp->b_count / 512;
	ccb->cmd_status[9] = 0;
	ccb->cmdlen = 10;
	ccb->senselen = MAX_SENSEDATA;

#ifdef _I386
	dsl_gen(ccb->dataptr, ccb->datalen,
		 sw->sw_bp->b_paddr, (long)sw->sw_bp->b_count);
#else /* _I386 */
	aha_l_to_p3((long)sw->sw_bp->b_count, ccb->datalen);
	aha_l_to_p3(vtop(sw->sw_bp->b_faddr), ccb->dataptr);
#endif /* _I386 */
	/*
	 * The ccb's returned here are going to be freed by aha_process ().
	 */
	return ccb;
#if	0
/* start of ioctl code */
	if(f == SASI_CMD_IN)
		ccb->target |= AHA_CCB_DATA_IN;
	else if(f == SASI_CMD_OUT)
		ccb->target |= AHA_CCB_DATA_OUT;
	else
		ccb->target |=	 AHA_CCB_DATA_IN
				|AHA_CCB_DATA_OUT;
#endif
}

aha_start()
{
	register int i, s, n = 0;
	scsi_work_t *sw;
	static char locked;

	s = sphi();
	if(locked) {
		spl(s);
		return;
	}
	++locked;
	spl(s);

	while((sw = scsi_work_queue->sw_actf) != NULL) {
		for(i = MIN_MAILBOX; i < MAX_MAILBOX; ++i)
			if(mailbox_out[i].cmd == MBO_IS_FREE) {
				register ccb_t *ccb;
				int s;

				++n;
				if ((ccb = buildccb (sw)) == NULL) {
					/*
					 * NIGEL: Earlier kernels did not
					 * diagnose this! 
					 */
					goto out_of_mem;
				}
#ifdef _I386
				aha_l_to_p3(vtop(ccb),
						mailbox_out[i].adr);
				ccb_remember(ccb, mailbox_out[i].adr);
#else /* _I386 */
				aha_l_to_p3(VTOP2(ccb, sds),
						mailbox_out[i].adr);
#endif /* _I386 */
				mailbox_out[i].cmd = MBO_TO_START;

				aha_1out(AHA_DO_SCSI_START);

				s = sphi();
				sw = scsi_work_queue->sw_actf = sw->sw_actf;
				if(sw == NULL)
					scsi_work_queue->sw_actl = NULL;
				spl(s);

				if(sw == NULL)
					break;
			}
		if(i == MAX_MAILBOX)
			break;
	}
out_of_mem:
	--locked;
	return n;
}

int
aha_completed()
{
	register int i, n;

	for(n = 0, i = 0; i < MAX_MAILBOX; ++i)
		if(mailbox_in[i].cmd != MBI_IS_FREE) {
			/*
			 * NIGEL: Earlier kernels deferred these, but with the
			 * ccb_free ()/ccb_alloc () system that is no longer
			 * necessary in either 4.x or 3.x systems.
			 */
#ifdef _I386
			aha_process(ccb_recall(mailbox_in[i].adr));
#else /* _I386 */
			aha_process(aha_p3_to_v(mailbox_in[i].adr));
#endif /* _I386 */
			mailbox_in[i].cmd = MBI_IS_FREE;
			++n;
		}
	return n;
}

void
aha_intr()
{
	register int i;

	if(((i = INB(aha_i_o_base+AHA_INTERRUPT)) & AHA_ANY_INTER) == 0)
		printf("aha: spurious interrupt %x\n", i);

	switch(i & AHA_ALL_INTERRUPTS) {
	case AHA_RESETED:
		break;
	case AHA_CMD_DONE:
		break;
	case AHA_MBO_EMPTY:
		defer(aha_start, (char *)0);
		break;
	case AHA_MBI_STORED:
		aha_completed();
		break;
	default:
		printf("aha: multiple interrupts not yet handled\n");
	}
	outb(aha_i_o_base+AHA_CONTROL, AHA_INTRRESET);
}

aha_ioctl()
{
	printf("aha_ioctl: Not implemented\n");
}

#ifdef TRACER
static	unsigned char vec[256];

static	aha_ports_are() {
	printf("aha_ports_are: %x %x %x\n",
		INB(aha_i_o_base+0),
		INB(aha_i_o_base+1),
		INB(aha_i_o_base+2));
}

static	aha_inquiry_is() {
	printf("aha_inquiry:");
	printf("... aha_present = %d, ", aha_present());
	printf("%s\n", aha_last_msg());
	aha_cmd_out(AHA_DO_INQUIRY);

	aha_get_data(&vec[0], 4);
	printf(" board id '%c'", vec[0]);
	printf(", options '%c'", vec[1]);
	printf(", HW '%c'", vec[2]);
	printf(", FW '%c'\n", vec[3]);
}

void	aha_setup_is() {
	register int i;

	printf("Setup and Data:\n");
	aha_cmd_out(AHA_DO_GET_SETUP);
	aha_cmd_out(16);
	aha_get_data(&vec[0], 16);
	printf("  Data Xfer %s Sync (J1)\n", (vec[0]&1) ? "is" : "not");
	printf("  Parity %s Enabled (J1)\n", (vec[0]&2) ? "is" : "not");
	switch(vec[1]) {
	case AHA_SPEED_5_0_MB:
		printf("  5.0 Mb/sec.\n");	break;
	case AHA_SPEED_6_7_MB:
		printf("  6.7 Mb/sec.\n");	break;
	case AHA_SPEED_8_0_MB:
		printf("  8.0 Mb/sec.\n");	break;
	case AHA_SPEED_10_MB:
		printf("  10 Mb/sec.\n");	break;
	case AHA_SPEED_5_7_MB:
		printf("  5.7 Mb/sec.\n");	break;
	default:
		if(vec[1] & 0x80)
			printf("  Pulse Read %d, Write %d, Strobe off %d\n",
				50*(2+(vec[1]>>4)&0x7), 50*(2+(vec[1]&7)),
				vec[1] & 0x80 ? 150 : 100);
	}
	printf("  Bus Time ON %d, OFF %d\n", vec[2], vec[3]);
	printf("  %d Mailboxes at %x|%x|%x\n", vec[4],
		vec[5], vec[6], vec[7]);
	for(i = 0; i < 8; ++i)
		if(vec[i+8])
			printf("  Target [%d] = Sync Neg %x\n", i, vec[i+8]);
}

static	aha_mailboxes_are(n, adr)
mailentry *adr;
{
	register int i;

	printf("addresses for mailbox is %x:%x\n", (long)adr);
	for(i = 0; i < n; ++i, ++adr)
		printf("  mbo[%x] = %x %x|%x|%x\n",
			i, adr->cmd, adr->adr[0], adr->adr[1], adr->adr[2]);
	for(i = 0; i < n; ++i, ++adr)
		printf("  mbi[%x] = %x %x|%x|%x\n",
			i, adr->cmd, adr->adr[0], adr->adr[1], adr->adr[2]);
}

void	aha_status()
{
	aha_ports_are();
	aha_inquiry_is();
/* 	aha_devices_are(); */	/* This appears to have never existed.  */
	aha_setup_is();
	aha_mailboxes_are(MAX_MAILBOX, mailbox_out);
}

aha_ccb_print(ccb)
ccb_t	*ccb;
{
	register int i;
	register unsigned char *cp;

	printf("aha_ccb_print(ccb: %x)", ccb);
	if (0 != ccb) {
		printf(", sw: %x", ccb->ccb_sw);
		if (0 != ccb->ccb_sw) {
			printf(", bp: %x", ccb->ccb_sw->sw_bp);
			if (0 != ccb->ccb_sw->sw_bp) {
				printf(", flag: %x",
					ccb->ccb_sw->sw_bp->b_flag);
			}
		}
		printf(", op %d, ", ccb->opcode);
		printf("target ID=%d, ", (ccb->target>>5) & 0x7);
		printf("LUN=%d, ", (ccb->target & 0x7));
		printf("dir=%s%s\n",	(ccb->target&AHA_CCB_DATA_IN)?"IN":"",
					(ccb->target&AHA_CCB_DATA_OUT)?"OUT":"");
		printf("data len %x|%x|%x, adr %x|%x|%x\n",
			ccb->datalen[0],ccb->datalen[1],ccb->datalen[2],
			ccb->dataptr[0],ccb->dataptr[1],ccb->dataptr[2]);
		printf("status host=%x, target=%x\n",
			ccb->hoststatus, ccb->targetstatus);
		printf("cmddata[%d]:", ccb->cmdlen);
		for(i = 0, cp = ccb->cmd_status; i < ccb->cmdlen; ++i)
			printf(" %x", *cp++);
		printf("\nrequest sense[%d]:", ccb->senselen);
		for(i = 0; i < ccb->senselen; ++i)
			printf(" %x", *cp++);
		if(i = cp[-1]) {
			printf("\n   + ");
			while(--i >= 0)
				printf(" %x", *cp++);
		}
		printf("\n");
	}
}

#endif	/* TRACER */
