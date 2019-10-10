/*
 * Define useful symbols for use in the ps program.
 */
#ifndef __SYS_COH_PS_H__
#define __SYS_COH_PS_H__

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
	long	 p_utime;		/* User time (HZ) */
	long	 p_stime;		/* System time */
	unsigned char	rrun;		/* Ready to run */
	char	 u_comm[U_COMM_LEN];	/* Command name */
	char	 u_sleep[U_SLEEP_LEN];	/* Sleep on event */
#if _I386
	int	 p_schedPri;		/* will index into table in sys/ts.h */
#endif
/* All new fields should be added here. */
	char	 pr_argv[ARGSZ];	/* Insert the argument vector here. */
} stMonitor;

#endif /* _SYS_COH_PS_H */
