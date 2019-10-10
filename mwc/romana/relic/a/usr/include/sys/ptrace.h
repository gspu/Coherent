/*
 * Coherent.
 * Process trace.
 */
#ifndef	 PTRACE_H
#define	 PTRACE_H
#include <sys/types.h>

/*
 * Structure used for communication between parent and child.
 */
struct ptrace {
	int	 pt_req;		/* Request */
	int	 pt_pid;		/* Process id */
	vaddr_t	 pt_addr;		/* Address */
	int	 pt_data;		/* Data */
	int	 pt_errs;		/* Error status */
	int	 pt_rval;		/* Return value */
	int	 pt_busy;		/* In use */
	GATE	 pt_gate;		/* Gate */
};

#ifdef KERNEL
/*
 * Global variables.
 */
extern	struct	ptrace pts;			/* Ptrace structure */

#endif

#endif
