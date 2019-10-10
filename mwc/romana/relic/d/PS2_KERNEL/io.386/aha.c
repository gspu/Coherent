/*
 * This is the host adaptor specific portion of the
 * Adaptec AHA154x driver.
 *
 * $Log:	aha.c,v $
 * Revision 1.2  92/08/04  12:51:18  bin
 * update for ker59
 * 
 * Revision 1.2	91/05/01  04:54:43	root
 * Debug code and kalloc arg fixes.
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

#if	0
static
OUTB( port, value )
short port;
{	printf( "<O(%x,%x)>", port, value );
	outb( port, value );	}
INB( port )
short port;
{	register int i = inb(port);
	printf( "<I(%x)=%x>", port, i );
	return i;	}
#else
#define	OUTB( port, value )	outb( port, value )
#define	INB(port)	inb(port)
#endif

#ifdef TRACER
#define	SETMSG(msg)	aha_err_msg = msg

static char *
aha_last_msg()
{
	T_PIGGY( 0x8000, return aha_err_msg; );

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
aha_set_base( base )
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

aha_process( ccb )
	ccb_t *ccb;
{
	register scsi_work_t *sw = ccb->ccb_sw;
	register BUF *bp;

	T_PIGGY( 0x8000, {
		printf( "aha_process: ccb %x ", ccb );
		printf("sw=%x", ccb->ccb_sw);
		if (0 != ccb->ccb_sw) {
			printf(" bp=%x", ccb->ccb_sw->sw_bp);
		}
		printf("\n");
		aha_ccb_print( ccb );
	} );
	if( ccb->ccb_sw == 0 ) {
		T_PIGGY( 0x8000, 
			printf( "process: ccb %x with NULL sw\n", ccb );
		);
		ccb->opcode = AHA_OP_INVALID;
		wakeup( ccb );
		return;
	}

	bp = sw->sw_bp;

	T_PIGGY( 0x8000, printf( "bp = %x\n", bp ); );

	if( (ccb->hoststatus != 0) || (ccb->targetstatus != 0) ) {
		if( --sw->sw_retry > 0
		   || (ccb->targetstatus == CHECK_TARGET_STATUS
		   && ccb->cmd_status[12] == SENSE_UNIT_ATTENTION)) {
			int s = sphi();
			if( scsi_work_queue->sw_actf == NULL ) {
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
	T_PIGGY( 0x8000, printf( "bp flag = %x\n", bp->b_flag ); );

	bdone( bp );
#ifdef _I386
	T_PIGGY( 0x80000, printf( "pf(sw)" ); );
	pfree( sw );
	T_PIGGY( 0x80000, printf( "df(d)" ); );
	dsl_free( ccb->dataptr );
	T_PIGGY( 0x80000, printf( "pf(ccb)" ); );
	pfree( ccb );
	ccb_forget( ccb );
#else /* _I386 */
	kfree( sw );
	kfree( ccb );
#endif /* _I386 */
}

static
int	aha_1out( value )
{
	register int i;

	while( (i = INB(aha_i_o_base + AHA_STATUS) & AHA_CDOPFULL) != 0 )
		if( (i & AHA_INVDCMD)
		 || (INB(aha_i_o_base+AHA_INTERRUPT) & AHA_CMD_DONE) )
			return -1;
	OUTB( aha_i_o_base + AHA_WRITE, value );
	return 0;
}

static
int	aha_1in()
{
	register int i;

	while( (i = INB(aha_i_o_base + AHA_STATUS) & AHA_DIPFULL) == 0 )
		if( (i & AHA_INVDCMD)
		 || (INB(aha_i_o_base+AHA_INTERRUPT) & AHA_CMD_DONE) )
			return -1;
	return INB( aha_i_o_base + AHA_READ ) & 0xFF;
}

static
void	aha_cmd_out( value )
{
	register long l;
	register int i;

	for( l = aha_timeout[TIMEOUT_SENDCMD]; --l > 0; ) {
		if( ((i=INB(aha_i_o_base + AHA_STATUS))
		    & AHA_SCSIIDLE ) != 0 ) {
			aha_1out( value );
			return;
		} else {
			T_PIGGY( 0x8000, 
				printf( "aha: cmd_out status = %x\r", i );
			);
		}
	}
	SETMSG( "timeout sending cmd byte" );
	printf( "aha154x: timeout sending cmd byte\n" );
}

static
int	aha_poll()
{
	register int i;
	register int l = aha_timeout[TIMEOUT_POLL];
	while( (--l > 0 )
	  &&  ((i = INB(aha_i_o_base + AHA_INTERRUPT)) & AHA_CMD_DONE) == 0 )
		;
	if( l == 0 )
		printf( "aha154x: aha_poll timed out\n" );

	i = INB(aha_i_o_base + AHA_STATUS);
	OUTB( aha_i_o_base + AHA_CONTROL, AHA_INTRRESET );
	return i;
}

static
void	aha_get_data( vec, cnt )
char	*vec;
int	cnt;
{
	while( --cnt >= 0 )
		*vec++ = aha_1in();
	aha_poll();
}

static
int	aha_present()
{
	long	l;

	if( INB(aha_i_o_base) == 0xFF ) {
		SETMSG( "no adapter found at io base" );
		return -3;
	}
	for( l = aha_timeout[TIMEOUT_PRESENT];
		(--l > 0) && (INB(aha_i_o_base + AHA_STATUS) & AHA_SELFTEST); )
			;
	if( l == 0 ) {
		SETMSG( "selftest not completed" );
		return -1;
	}
	if( INB(aha_i_o_base + AHA_STATUS) & AHA_DIAGFAIL ) {
		SETMSG( "diagnostics failed" );
		return -2;
	}
	if( INB(aha_i_o_base + AHA_STATUS) & AHA_INITMAIL ) {
		SETMSG( "mailbox initialization needed" );
		return 1;
	}
	if( INB(aha_i_o_base + AHA_STATUS) & AHA_SCSIIDLE ) {
		SETMSG( "adaptor okay, idle" );
		return 0;
	}
	SETMSG( "unknown status at start" );
	return -4;
}

void
aha_l_to_p3( value, vec )
	paddr_t	value;
	P3 vec;
{
	register int i;

	for( i = 3; --i >= 0; ) {
		vec[i] = value & 0xFF;
		value >>= 8;
	}
}

long
aha_p3_to_l( vec )
	P3 vec;
{
	register int i;
	register long retval;

	retval = 0;
	for( i = 0; i < 3; ++i) {
		retval <<= 8;
		retval |= vec[i];
	}

	return( retval );
} /* aha_p3_to_l() */

#ifndef _I386 /* All of aha_p3_to_v().  */
static char *
aha_p3_to_v( vec )
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

	aha_cmd_out( AHA_DO_GET_DEVICES );
	aha_get_data( &buf[0], 8 );
	for( i = 0; i < 8; ++i )
		if( buf[i] != 0 )
			printf( "[%d] %x ", i, buf[i] );
	printf( "\n" );
}

int	aha_unload( ireq )
{
	T_PIGGY( 0x8000, printf( "aha_unload: %x\n", ireq ); );
	/*
	 *	we should really verify that everything
	 *	out there gets flushed.
	 */
	if (!aha_loaded)
		return;
	if( mailbox_out ) {
#ifdef _I386
		T_PIGGY( 0x80000, printf( "pf(mbo)" ); );
		pfree( mailbox_out );
#else /* _I386 */
		kfree( mailbox_out );
#endif /* _I386 */
		mailbox_out = 0;
	}
	clrivec( ireq );
}

int	aha_load( dma, ireq, base, head )
scsi_work_t *head;
{
	register int	i;
	unsigned char	adr[4];

	T_PIGGY( 0x8000,
		printf( "aha_load( %d, %d, 0x%x );\n", dma, ireq, base );
	);
	aha_set_base(base);
	if( mailbox_out == 0 ) {
		if( (mailbox_out = 
#ifdef _I386
		     palloc(2 * MAX_MAILBOX * sizeof(mailentry))) == 0 ) {
#else /* _I386 */
		     kalloc(2 * MAX_MAILBOX * sizeof(mailentry))) == 0 ) {
#endif /* _I386 */
			no_mem();
			return -1;
		} else {
			mailbox_in = &mailbox_out[MAX_MAILBOX];
		}
	}

	for( i = 0; i < MAX_MAILBOX; ++i )
		mailbox_out[i].cmd = mailbox_in[i].cmd = 0;

#ifdef _I386
	aha_l_to_p3( vtop( mailbox_out ), &adr[1] );
#else /* _I386 */
	sds_physical = VTOP2( 0, sds );
	aha_l_to_p3( VTOP2( mailbox_out, sds ), &adr[1] );
#endif /* _I386 */

	adr[0] = MAX_MAILBOX;

	/*
	 * setup HW
	 */
	setivec( ireq, aha_intr );

	outb( 0xD6, 0xC1 );		/* DMA is currently hard coded for */
	outb( 0xD4, 0x01 );		/* DMA channel 5 */


	OUTB( aha_i_o_base+AHA_CONTROL, AHA_HARDRESET );
	if (aha_present() < 0) {
		printf("aha154x: initialization error or host adaptor not ");
		printf("found at 0x%x\n", aha_i_o_base);
		return -1;
	}
	aha_cmd_out( AHA_DO_MAILBOX_INIT );
	for( i = 0; i < 4; ++i )
		aha_1out( adr[i] );
	scsi_work_queue = head;
	++aha_loaded;
	return MAX_MAILBOX;
}

aha_command( sc )
register scsi_cmd_t *sc;
{
	register int i;
	/* register */ ccb_t *ccb;

	short	count = sc->blklen;
	long	block = sc->block;

#ifdef _I386
	T_PIGGY(0x100000, printf("pa(ac)"); );
	ccb = (ccb_t *) palloc(sizeof (ccb_t));
#else /* _I386 */
	ccb = (ccb_t *) kalloc(sizeof (ccb_t));
#endif /* _I386 */
	if (ccb == (ccb_t *) 0) {
		no_mem();
		return -1;
	}
	T_PIGGY( 0x8000, {
		printf( "aha_command( SCSI ID %d, LUN %d, c %x, b %x",
			sc->unit >> 2, sc->unit & 0x3, sc->cmd, sc->block ); 
		printf( " [%d] @%x:%x )\n",
			sc->buflen, sc->buffer );
	} );

	ccb->ccb_sw = 0;
#ifdef _I386
	ccb->opcode = AHA_OP_SIC_SG;		/* SCSI_INITIATOR*/
#else /* _I386 */
	ccb->opcode = AHA_OP_SIC;		/* SCSI_INITIATOR*/
#endif /* _I386 */
	ccb->target = (sc->unit & 0x1C) << 3;	/* SCSI ID */
	ccb->target |= sc->unit & 0x3;		/* LUN */
	if( (ccb->cmd_status[0] = sc->cmd) == ScmdWRITEXTENDED ) {
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
	dsl_gen( ccb->dataptr, ccb->datalen, sc->buffer, (long)sc->buflen );
	aha_l_to_p3( vtop( ccb ), mailbox_out[0].adr );
	ccb_remember( ccb, mailbox_out[0].adr );
#else /* _I386 */
	aha_l_to_p3( (long)sc->buflen, ccb->datalen );
	aha_l_to_p3( sc->buffer, ccb->dataptr );
	aha_l_to_p3( VTOP2( ccb, sds ), mailbox_out[0].adr );
#endif /* _I386 */

	T_PIGGY( 0x8000, aha_ccb_print( ccb ); );
	mailbox_out[0].cmd = MBO_TO_START;

	/* Start the AHA-154x scanning the mailboxes.  */
	aha_1out( AHA_DO_SCSI_START );

	/* Wait for this ccb to finish.  */
	while( ccb->opcode != AHA_OP_INVALID ) {
		v_sleep( ccb, CVBLKIO, IVBLKIO, SVBLKIO, "aha:ccb" );
		/* The AHA-154x driver is waiting for a ccb to complete.  */
	}
	
	T_PIGGY( 0x8000,
		printf( "done with status = %d, %d\n\n",
			ccb->hoststatus, ccb->targetstatus );
	);

	if( (ccb->targetstatus == CHECK_TARGET_STATUS)
	   && (ccb->cmd_status[12] != SENSE_UNIT_ATTENTION) ) {
		printf( "aha: SCSI ID %d LUN %d. SCSI sense =",
		(sc->unit >> 2), sc->unit & 0x3 );
		for( i = 0; i < ccb->senselen; ++i )
			printf( " %x", ccb->cmd_status[10+i] );
		printf( "\n" );
	}
	i = ccb->hoststatus | ccb->targetstatus;
#ifdef _I386
	T_PIGGY( 0x80000, printf( "df(d2)" ); );
	dsl_free( ccb->dataptr );
	T_PIGGY( 0x80000, printf( "pf(ccb2)" ); );
	pfree( ccb );
	ccb_forget( ccb );
#else /* _I386 */
	kfree( ccb );
#endif /* _I386 */
	return i;
}

ccb_t	*buildccb( sw )
register scsi_work_t *sw;
{
	register ccb_t *ccb;
#ifdef _I386
	T_PIGGY(0x100000, printf("pa(bld)"); );
	ccb = (ccb_t *)palloc(sizeof(ccb_t));
#else /* _I386 */
	ccb = (ccb_t *)kalloc(sizeof(ccb_t));
#endif /* _I386 */

	T_PIGGY( 0x8000,
		printf( "build: sw:%x, drv:%x, bno:%D  ",
			sw, sw->sw_drv, sw->sw_bno );
	);

	ccb->ccb_sw = sw;
#ifdef _I386
	ccb->opcode = AHA_OP_SIC_SG;		/* SCSI_INITIATOR*/
#else /* _I386 */
	ccb->opcode = AHA_OP_SIC;		/* SCSI_INITIATOR*/
#endif /* _I386 */

	ccb->target = (sw->sw_drv & 0x1C) << 3;	/* SCSI ID */
	ccb->target |= (sw->sw_drv) & 0x3;	/* LUN */
	if( sw->sw_bp->b_req == BREAD ) {
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
	dsl_gen( ccb->dataptr, ccb->datalen,
		 sw->sw_bp->b_paddr, (long)sw->sw_bp->b_count);
#else /* _I386 */
	aha_l_to_p3( (long)sw->sw_bp->b_count, ccb->datalen );
	aha_l_to_p3( vtop(sw->sw_bp->b_faddr), ccb->dataptr );
#endif /* _I386 */
	return ccb;
#if	0
/* start of ioctl code */
	if( f == SASI_CMD_IN )
		ccb->target |= AHA_CCB_DATA_IN;
	else if( f == SASI_CMD_OUT )
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
	if( locked ) {
		spl(s);
		return;
	}
	++locked;
	spl(s);

	while( (sw = scsi_work_queue->sw_actf) != NULL ) {
		for( i = MIN_MAILBOX; i < MAX_MAILBOX; ++i )
			if( mailbox_out[i].cmd == MBO_IS_FREE ) {
				register ccb_t *ccb;
				int s;

				++n;
				ccb = buildccb( sw );
				T_PIGGY( 0x8000, aha_ccb_print( ccb ); );
#ifdef _I386
				aha_l_to_p3( vtop( ccb ),
						mailbox_out[i].adr );
				ccb_remember( ccb, mailbox_out[i].adr );
#else /* _I386 */
				aha_l_to_p3( VTOP2( ccb, sds ),
						mailbox_out[i].adr );
#endif /* _I386 */
				mailbox_out[i].cmd = MBO_TO_START;

				T_PIGGY( 0x8000, {
				printf( "MBO[%d] = %x:%x:%x:%x, ccb = %x ",
					i, mailbox_out[i].cmd,
					mailbox_out[i].adr[0],
					mailbox_out[i].adr[1],
					mailbox_out[i].adr[2], ccb );
	printf("sw=%x bp=%x\n", ccb->ccb_sw, ccb->ccb_sw->sw_bp);
				} ); /* T_PIGGY() */

				aha_1out( AHA_DO_SCSI_START );

				s = sphi();
				sw = scsi_work_queue->sw_actf = sw->sw_actf;
				if( sw == NULL )
					scsi_work_queue->sw_actl = NULL;
				spl(s);

				if( sw == NULL )
					break;
			}
		if( i == MAX_MAILBOX )
			break;
	}
	--locked;
	return n;
}

int	aha_completed()
{
	register int i, n;

	for( n = 0, i = 0; i < MAX_MAILBOX; ++i )
		if( mailbox_in[i].cmd != MBI_IS_FREE ) {
		T_PIGGY( 0x8000 ,
			printf( "aha: mail[%d] = %x:%x:%x:%x\n",
				i, mailbox_in[i].cmd,
				mailbox_in[i].adr[0],
				mailbox_in[i].adr[1],
				mailbox_in[i].adr[2] );
		);
#ifdef _I386
			aha_process ( ccb_recall( mailbox_in[i].adr ) );
#else /* _I386 */
			defer( aha_process,
				aha_p3_to_v( mailbox_in[i].adr ) );
#endif /* _I386 */
			mailbox_in[i].cmd = MBI_IS_FREE;
			++n;
		}
	return n;
}

void	aha_intr()
{
	register int i;

	T_PIGGY( 0x8000, printf( "aha_interrupt routine\n" ); );

	if( ((i = INB(aha_i_o_base+AHA_INTERRUPT)) & AHA_ANY_INTER) == 0 )
		printf( "aha: spurious interrupt %x\n", i );

	T_PIGGY( 0x8000, printf( "aha_interrupt: %x\n", i ); );

	switch( i & AHA_ALL_INTERRUPTS ) {
	case AHA_RESETED:
		T_PIGGY( 0x8000, printf( "aha: reseted\n" ); );
		break;
	case AHA_CMD_DONE:
		T_PIGGY( 0x8000, printf( "aha: adapter command completed\n" ); );
		break;
	case AHA_MBO_EMPTY:
		T_PIGGY( 0x8000, printf( "aha: MAILBOX emptied\n" ); );
		defer( aha_start, (char *)0 );
		break;
	case AHA_MBI_STORED:
		T_PIGGY( 0x8000, printf( "aha: MAILBOX in stored\n" ); );
		aha_completed();
		break;
	default:
		printf( "aha: multiple interrupts not yet handled\n" );
	}
	outb( aha_i_o_base+AHA_CONTROL, AHA_INTRRESET );
}

aha_ioctl()
{
	printf( "aha_ioctl: Not implemented\n" );
}

#ifdef TRACER
static	unsigned char vec[256];

static	aha_ports_are() {
	printf( "aha_ports_are: %x %x %x\n",
		INB(aha_i_o_base+0),
		INB(aha_i_o_base+1),
		INB(aha_i_o_base+2) );
}

static	aha_inquiry_is() {
	printf( "aha_inquiry:" );
	printf( "... aha_present = %d, ", aha_present() );
	printf( "%s\n", aha_last_msg() );
	aha_cmd_out( AHA_DO_INQUIRY );

	aha_get_data( &vec[0], 4 );
	printf( " board id '%c'", vec[0] );
	printf( ", options '%c'", vec[1] );
	printf( ", HW '%c'", vec[2] );
	printf( ", FW '%c'\n", vec[3] );
}

void	aha_setup_is() {
	register int i;

	printf( "Setup and Data:\n" );
	aha_cmd_out( AHA_DO_GET_SETUP );
	aha_cmd_out( 16 );
	aha_get_data( &vec[0], 16 );
	printf( "  Data Xfer %s Sync (J1)\n", (vec[0]&1) ? "is" : "not" );
	printf( "  Parity %s Enabled (J1)\n", (vec[0]&2) ? "is" : "not" );
	switch( vec[1] ) {
	case AHA_SPEED_5_0_MB:
		printf( "  5.0 Mb/sec.\n" );	break;
	case AHA_SPEED_6_7_MB:
		printf( "  6.7 Mb/sec.\n" );	break;
	case AHA_SPEED_8_0_MB:
		printf( "  8.0 Mb/sec.\n" );	break;
	case AHA_SPEED_10_MB:
		printf( "  10 Mb/sec.\n" );	break;
	case AHA_SPEED_5_7_MB:
		printf( "  5.7 Mb/sec.\n" );	break;
	default:
		if( vec[1] & 0x80 )
			printf( "  Pulse Read %d, Write %d, Strobe off %d\n",
				50*(2+(vec[1]>>4)&0x7), 50*(2+(vec[1]&7)),
				vec[1] & 0x80 ? 150 : 100 );
	}
	printf( "  Bus Time ON %d, OFF %d\n", vec[2], vec[3] );
	printf( "  %d Mailboxes at %x|%x|%x\n", vec[4],
		vec[5], vec[6], vec[7] );
	for( i = 0; i < 8; ++i )
		if( vec[i+8] )
			printf( "  Target [%d] = Sync Neg %x\n", i, vec[i+8] );
}

static	aha_mailboxes_are( n, adr )
mailentry *adr;
{
	register int i;

	printf( "addresses for mailbox is %x:%x\n", (long)adr );
	for( i = 0; i < n; ++i, ++adr )
		printf( "  mbo[%x] = %x %x|%x|%x\n",
			i, adr->cmd, adr->adr[0], adr->adr[1], adr->adr[2] );
	for( i = 0; i < n; ++i, ++adr )
		printf( "  mbi[%x] = %x %x|%x|%x\n",
			i, adr->cmd, adr->adr[0], adr->adr[1], adr->adr[2] );
}

void	aha_status()
{
	aha_ports_are();
	aha_inquiry_is();
/* 	aha_devices_are(); */	/* This appears to have never existed.  */
	aha_setup_is();
	aha_mailboxes_are( MAX_MAILBOX, mailbox_out );
}

aha_ccb_print( ccb )
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
					ccb->ccb_sw->sw_bp->b_flag );
			}
		}
		printf( ", op %d, ", ccb->opcode );
		printf( "target ID=%d, ", (ccb->target>>5) & 0x7 );
		printf( "LUN=%d, ", (ccb->target & 0x7) );
		printf( "dir=%s%s\n",	(ccb->target&AHA_CCB_DATA_IN)?"IN":"",
					(ccb->target&AHA_CCB_DATA_OUT)?"OUT":"" );
		printf( "data len %x|%x|%x, adr %x|%x|%x\n",
			ccb->datalen[0],ccb->datalen[1],ccb->datalen[2],
			ccb->dataptr[0],ccb->dataptr[1],ccb->dataptr[2] );
		printf( "status host=%x, target=%x\n",
			ccb->hoststatus, ccb->targetstatus );
		printf( "cmddata[%d]:", ccb->cmdlen );
		for( i = 0, cp = ccb->cmd_status; i < ccb->cmdlen; ++i )
			printf( " %x", *cp++ );
		printf( "\nrequest sense[%d]:", ccb->senselen );
		for( i = 0; i < ccb->senselen; ++i )
			printf( " %x", *cp++ );
		if( i = cp[-1] ) {
			printf( "\n   + " );
			while( --i >= 0 )
				printf( " %x", *cp++ );
		}
		printf( "\n" );
	}
}

#endif	/* TRACER */
