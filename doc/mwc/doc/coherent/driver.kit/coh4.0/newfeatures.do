
From gort!hal  Tue Jun  8 15:02:45 1993 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA01737; 8 Jun 93 15:02:45 GMT (Tue)
Received: by mwc.com (smail2.5.3-coh) id AA10940; 8 Jun 93 09:23:33 
Received: by gort.mwc.com (smail2.5.2-coh) id AA00297; 8 Jun 93 09:13:36 
Subject: added driver kit docs
To: tech@gort.mwc.com
From: Hal Snyder <hal@gort.mwc.com>
Cc: fred@gort.mwc.com
X-Mailer: ELM [version 2.3 PL11]
Message-Id: <9306080913.AA00297@gort.mwc.com>
Date: 8 Jun 93 09:13:36 

The following information MUST go out with the r77 driver kits.  We
already have an add-in sheet for the previous driver kit, titled "Recent
Additions".  I leave it to you two how to update the docs - "More Recent
Additions", revised "Recent Additions" sheet, or updated manual.
--
Supplemental documentation for Device Driver Kit.

The following features are in the COHERENT Device Driver Kit, r77
and later.

1.  Terminal-Device Routines getq() and putq() have been renamed to
cltputq() and cltgetq().  This is in preparation for STREAMS support,
since the older names have a different use under STREAMS.

2.  Accessible Kernel Routine v_sleep() has been replaced with a new
routine, x_sleep().  The argument list and return values for x_sleep()
are given below.  The discussion in the driver kit manual for v_sleep()
is still relevant, except that the arguments supplied to the function
are now different.

#include <sys/sched.h>
int x_sleep(event, schedPri, sleepPri, reason)
char * event;
int schedPri;
int sleepPri;
char * reason;

Surrender CPU while awaiting some event or resource.

Arguments:

event:  usually, this is the address of a data item in static kernel data
	space.  A call to wakeup() with the same event value will end the
	present sleep.
schedPri:  a value used as a hint to the scheduler once the process is
	asleep.  It is one of prilo, primed, prihi, pritape, pritty,
	pridisk, prinet.
sleepPri:  a flag indicating what should happen if a signal is sent to
	the process while it sleeps (or as it is about to sleep).  Only
	signals which are neither ignored nor held can affect a sleeping
	process.  Values for sleepPri are:
	- slpriNoSig - signals will not interrupt sleep
	- slpriSigLjmp - signals cause whatever system call was in
	  progress to end immediately with an error value of EINTR
	  (do not use within device "open" routines)
	- slpriSigCatch - signals cause a return from the call to
	  x_sleep().
reason:  pointer to text (up to U_SLEEP_LEN characters) explaining what
	the sleep is for - this text appears in output of the "ps" command.
	If text is fewer than U_SLEEP_LEN characters in length, it must be
	null-terminated.

Return values:
	0		wakeup received
	1		signal (other than SIGSTOP/SIGCONT) received
	2		SIGSTOP/SIGCONT (unimplemented now)

If a signal occurs and slpriSigLjmp has been specified, there is no
return from x_sleep!
-- 
Hal Snyder, M.D.		hal@mwc.com			itis
Mark Williams Company     	(708)-291-6700 (voice)		foras
60 Revere Drive, Suite 250	(708)-291-6750 (fax)		rixsatis
Northbrook, IL 60062


