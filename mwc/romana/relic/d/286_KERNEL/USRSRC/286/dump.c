/* $Header: /usr/src/sys/i8086/ibm_at/RCS/dump.c,v 1.1 88/03/24 17:33:35 src Exp $
 *
 * Function to dump Kernel data inforation.
 *
 * $Log:	/usr/src/sys/i8086/ibm_at/RCS/dump.c,v $
 * Revision 1.1	88/03/24  17:33:35	src
 * Initial revision
 * 
 */
#include <ascii.h>
#include <coherent.h>
#include <sys/i8086.h>
#include <sys/proc.h>
#include <sys/uproc.h>
#include <sys/seg.h>

int nodump = 1;

/*
 * Note: ip1 is a dummy parameter facilitating access to calling stack frame.
 */
dumpall( ip1 )
int ip1;
{
	register PROC * pp;
	register SEG  * sp;
	faddr_t gp;
	faddr_t fp;
	int s;
	extern saddr_t gdtsel;
	extern saddr_t uasa, ucs, uds, scs, sds;
	extern int nesterr;

	if ( nodump )
		return;

	s = sphi();

	printf("\nuasa=%x ucs=%x uds=%x scs=%x sds=%x nesterr=%u\n",
		uasa, ucs, uds, scs, sds, nesterr );


	printf("\nGlobal Descriptor Table:\n");
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = 5;	/* offset of flags byte */
	FP_SEL(fp) = 0;
	FP_OFF(fp) = 0;
	do {
		if ( ffbyte(gp) )
			vprint(fp);
		FP_OFF(gp) += 8;
		FP_SEL(fp) += 8;
	} while ( FP_SEL(fp) != 0 );

	showevq();
	showtim();
	putchar( A_FF );
	
	printf("\nProcesses:\n");
	for ( pp = procq.p_nforw; pp != &procq; pp = pp->p_nforw )
		showproc( pp );
	putchar( A_FF );

	printf( "\nMemory Segments:\n\n" );
	for ( sp = segmq.s_forw; sp != &segmq; sp = sp->s_forw )
		showseg( sp, "" );
	putchar( A_FF );

	if ( segdq.s_forw != &segdq ) {
		printf( "\nDisk Segments:\n\n");
		for ( sp = segdq.s_forw; sp != &segdq; sp = sp->s_forw )
			showseg( sp, "" );
		putchar( A_FF );
	}

	printf("\nLocal Stack: sp=%x\n", (&ip1)-1 );
	FP_SEL(fp) = sds;
	FP_OFF(fp) = (char*)(&u) + 1022;
	do {
		printf("        %x: %x\n", FP_OFF(fp), ffword(fp) );
		FP_OFF(fp)  -= 2;
	} while ( FP_OFF(fp) >= (&ip1)-1 );
	putchar( A_FF );

	printf("\nUser Stack: ss:sp = %x:%x\n", regl[OSS], regl[OSP] );
	FP_SEL(gp) = gdtsel;
	FP_OFF(gp) = regl[OSS] & ~7;
	if ( ((regl[OSS]&7) == 0) && regl[OSS] && regl[OSP] && ffbyte(gp+5) ) {
		FP_SEL(fp) = regl[OSS];
		FP_OFF(fp) = regl[OSP] + 120;
		if ( FP_OFF(fp) > udl )
			FP_OFF(fp) = udl & ~1;
		do {
			printf("        %x: %x\n", FP_OFF(fp), ffword(fp) );
			FP_OFF(fp)  -= 2;
		} while ( FP_OFF(fp) >= regl[OSP] );
	}
	putchar( A_FF );

	putchar( A_LF );
	spl( s );
}

static
showevq()
{
	register PROC * pp;
	register int i;

	printf("\nEvent Queues:\n");

	for ( i = 0; i < nel(linkq); i++ ) {

		if ( linkq[i].p_lforw == linkq[i].p_lback )
			continue;

		pp = linkq[i].p_lforw;
		printf("%x:", i );

		do {
			printf("\t%x: lforw=%x lback=%x pid=%d event=%x\n",
				pp,
				pp->p_lforw,
				pp->p_lback,
				pp->p_pid,
				pp->p_event
			);
		} while ((pp = pp->p_lforw) != (PROC *) &linkq[i] );
	}
}

static
showtim()
{
	register TIM * tp;
	register int i;

	printf("\nTiming Queues: lbolt=%lx\n", lbolt );

	for ( i = 0; i < nel(timq); i++ ) {

		if ( ! (tp = timq[i]) )
			continue;

		printf("%x:", i );

		do {
		    printf("\t%x: next=%x last=%x func=%x farg=%x lbolt=%lx\n",
			tp,
			tp->t_next,
			tp->t_last,
			tp->t_func,
			tp->t_farg,
			tp->t_lbolt
		    );
		} while ( tp = tp->t_next );
	}
}

static
showproc( pp )
register PROC * pp;
{
	register int i;

	printf("\n%x:\tnforw=%x nback=%x pid=%d ppid=%d state=%x flags=%x\n",
		pp,
		pp->p_nforw,		/* Forward pointer */
		pp->p_nback,		/* Backward pointer */
		pp->p_pid,		/* Process id */
		pp->p_ppid,		/* Process id of parent */
		pp->p_state,
		pp->p_flags
	);

	if ( pp->p_polltim.t_last )
	printf("\tpolltim@%x: next=%x last=%x func=%x farg=%x lbolt=%lx\n",
		&pp->p_polltim,
		pp->p_polltim.t_next,
		pp->p_polltim.t_last,
		pp->p_polltim.t_func,
		pp->p_polltim.t_farg,
		pp->p_polltim.t_lbolt
	);

	if ( pp->p_alrmtim.t_last )
	printf("\talrmtim@%x: next=%x last=%x func=%x farg=%x lbolt=%lx\n",
		&pp->p_alrmtim,
		pp->p_alrmtim.t_next,
		pp->p_alrmtim.t_last,
		pp->p_alrmtim.t_func,
		pp->p_alrmtim.t_farg,
		pp->p_alrmtim.t_lbolt
	);

	for ( i = 0; i <= NUSEG; i++ ) {
	static char * nam[] = {  "UA", "SS", "SI", "PI", "SD",  "PD", "UX" };
		if ( pp->p_segp[i] ) {
			printf("\n    %s: ", nam[i] );
			showseg( pp->p_segp[i], "        " );
		}
	}
}

static
showseg( sp, prefix )
register SEG * sp;
char * prefix;
{
	printf("%x: forw=%x back=%x ip=%x flags=%x urefc=%d lrefc=%d\n",
		sp,
		sp->s_forw,		/* Forward pointer */
		sp->s_back,		/* Backward pointer */
		sp->s_ip,		/* Inode pointer for shared text */
		sp->s_flags,		/* Flags */
		sp->s_urefc,		/* Reference count of segment */
		sp->s_lrefc		/* Lock reference count */
	);

	printf( "%s    faddr=%X size=%X paddr=%X\n",
		prefix,
		sp->s_faddr,		/* Memory access selector */
		sp->s_size,		/* Size in bytes */
		sp->s_paddr		/* Physical base address */
	);

	printf("%s    ", prefix);
	vprint( sp->s_faddr );
}
