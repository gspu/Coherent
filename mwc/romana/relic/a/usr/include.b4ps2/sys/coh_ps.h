/*
 * Define useful symbols for use in the ps program.
 */
#ifndef MONITOR_H	/* Rest of file.  */
#define MONITOR_H

#define	ARGSZ	64	/* size of argument list */

/*
 * This structure contains fields of interest to the ps command.
 */
typedef struct {
	unsigned p_pid;			/* Process id */
	unsigned p_ppid;		/* Process id of parent */
	unsigned p_uid;			/* Effective uid */
	unsigned p_ruid;		/* Real uid */
	unsigned p_rgid;		/* Real gid */
	unsigned p_state;		/* Scheduling state */
	unsigned p_flags;		/* Flags */
	char	 *p_event;		/* Wakeup event channel */
	dev_t	 p_ttdev;		/* Controlling terminal */
	unsigned p_nice;		/* Nice value */
	short	 size;			/* Size of the process */
	short	 rsize;			/* Real size of the process */
	unsigned p_cval;		/* Cpu schedule value */
	unsigned p_sval;		/* Swap schedule value */
	int	 p_ival;		/* Importance value */
	unsigned p_rval;		/* Response value */
	long	 p_utime;		/* User time (HZ) */
	long	 p_stime;		/* System time */
	unsigned char	rrun;		/* Ready to run */
	char	 u_comm[U_COMM_LEN];	/* Command name */
	char	 u_sleep[10];		/* Sleep on event */
	char	 pr_argv[ARGSZ];	/* Insert the argument vector here. */
} stMonitor;

#endif /* MONITOR_H */
