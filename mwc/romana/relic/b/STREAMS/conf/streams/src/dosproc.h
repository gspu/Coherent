#ifndef	__SYS_DOSPROC_H__
#define	__SYS_DOSPROC_H__

/*
 * Definitions for MS-DOS to emulate the Coherent process table and U area for
 * STREAMS testing.
 */

typedef enum {
	PS_RUN,
	PS_SIGNALLED,
	PS_SLEEP,
	PS_SLEEP_NO_SIG
} pstate_t;


typedef struct {
	pstate_t	p_state;
	struct {
		char		_space [16];
	} p_ddi_space;
} proc_t;


extern	proc_t		_dos_proc_;


#define	CURRENT_PROCESS()	((volatile proc_t *) & _dos_proc_)



typedef struct {
	char		u_nigel [16];
} uarea_t;


extern	uarea_t		_dos_uarea_;


#define	U_AREA()		(& _dos_uarea_)


#endif	/* ! defined (__SYS_DOSPROC_H__) */
