
From kcbbs.gen.nz!nbree  Fri Nov 26 03:03:58 1993 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA06050; 26 Nov 93 03:03:58 GMT (Fri)
Received: by mwc.com (smail2.5.3-coh) id AA07408; 25 Nov 93 17:30:28 GMT (Thu)
Received: from kcbbs.gen.nz (via [202.14.102.1]) by relay2.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AB16526; Thu, 25 Nov 93 05:07:53 -0500
Received: from localhost (nbree@localhost) by kcbbs.gen.nz (8.5/8.5) id WAA02528; Thu, 25 Nov 1993 22:57:30 +1300
From: Nigel Bree <nbree@kcbbs.gen.nz>
Message-Id: <199311250957.WAA02528@kcbbs.gen.nz>
Subject: Re: locks & Net stuff
To: rw@rwsys.wimsey.bc.ca (Randy Wright)
Date: Thu, 25 Nov 93 22:57:28 GMT+12:00
Cc: fred@mwc.com
In-Reply-To: <9311240821.AA3312.V1.5@rwsys.wimsey.bc.ca>; from "Randy Wright" at Nov 24, 93 8:21 am
X-Mailer: ELM [version 2.3 PL0]
Status: RO

>        A few weeks back, I was discussing the topic of
>locks with you in regards to Coherent. After reading the
>the Coherent dox on the matter, I saw that a sometimes
>a lock can be "held across calls" to various functions.
>I interpreted "held across calls" to mean that attempting
>said calls would fail or I'd have to sleep until the lock
>is released. That sounds like a terrible thing to do in
>an interrupt.

If you can explain the misinterpretation to Fred, I'm sure he could
come up with some more explicit way of saying this. Alternatively,
perhaps some introductory material on what locks are may help.

>        Since then, I've read some other literature on the
>matter. Now it seems more like "held across calls" means
>that if I aquire this lock, I can keep it in spite of
>making said calls.

Here's a shot at a general overview:

A context is holding a basic lock from the time that it successfully
calls LOCK () until the time that it calls UNLOCK (); similarly, for
read/write locks and sleep locks, we say that a context is holding
the lock from the time the lock is acquired until the time that the
lock is released.

Each type of synchronization object is applicable to a different
situation (and subject to a variety of constraints) depending on the
situations in which the data being locked are accessed. When
considering the styles of locks, one should bear in mind that we are
assuming a multi-processor model of operation where many contexts may
be active simultaneously.

A section of code may be executing at either Base level or Interrupt
level. Code at Base level is executing within the kernel under the
auspices of a process as a result of a system call; because there is
a process running, such contexts may "sleep". That is, the system can
set aside execution of the context and start another one. Code at
Interrupt level has been set running by the receipt of an interrupt
by a CPU, and cannot be set aside; it has temporarily usurped control
of the CPU and must run until completion.

Sleep locks exist to control interactions between code sections
running at Base level. When a Base-level context attempts to acquire a
lock and finds that it is already held, it suspends execution until
the lock in question is released; while it is suspended (i.e.,
sleeping), other useful work can be performed. Contexts running at
Interrupt level cannot attempt to use sleep locks or call other
DDI/DKI functions that may sleep.

Basic locks and read/write locks exist to control interactions between
code sections running at Interrupt level and other code sections
running at Interrupt or Base level. Contexts that attempt to access
such a lock do not sleep, but simply stop and wait until the lock is
freed, wasting CPU time. Basic and read/write locks should therefore
be held for as short a time as possible to reduce the likelihood that
another context will be blocked waiting for the lock to become free.

Note that contexts running at Base level can acquire basic locks and
read/write locks; however, while they hold such locks they must
behave as if they are running at Interrupt level and obey the same
restrictions (that is, they must not attempt to call any DDI/DKI
function which sleeps). If a Base-level context goes to sleep holding
a basic lock, an Interrupt-level context which tries to acquire that
same lock will then be forced to halt all activity on that CPU until
the lock becomes free; at best, this will cause a massive slowdown,
and on a uniprocessor system will cause total deadlock.

The above general rules are reflected in the usage notes for each
DDI/DKI function; the description of an individual function will
indicate what kinds of locks may or may not be held when the function
is called. Thus, the SLEEP_LOCK () function contains a note reminding
you that it may sleep, and so should never be called by an Interrupt-
level context, or by a Base-level context while it is holding a basic
lock.

>----- Net stuff
>
>1.        I'm hosting an 'on-line' conference Nov 26 via
>telnet at 192.197.249.158 (rwsyss). Please join in. I want to
>let users have a chance to chat with Coherent 'wizards'
>and demonstrate existing Coherent networking. I don't
>have the time do a lot of 'events' like this.

Sadly, I do not yet have Internet access; I only got a machine
together here yesterday, and am still arranging things with the
sysadmin of Kappa Crucis. I'll let you know if I can get on the air
at all in time.

>2.        I've set accounts on rwsyss for Udo, Fred and Harry
>Pulley to make it easier to collaborate on projects. [...]

I'm not quite sure how things will pan out with net.access here,
so once I know what the final arrangements are and how much it will
cost we can arrange such things. From my point of view the ideal
arrangement would be for MWC to be available at regular intervals
around the clock and useable as a central repository.




From alien!udo  Mon May 16 18:03:32 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA15454; 16 May 94 18:03:32 GMT (Mon)
Received: by mwc.com (smail2.5.3-coh) id AA14649; 16 May 94 16:57:30 GMT (Mon)
Received: by alien.mwc.com (smail2.5.3-coh) id AA04175; 16 May 94 11:59:27 CDT (Mon)
Subject: Re: Locks and Links
To: trixter@trxhome.mwc.com (Trixter / Independent)
Date: Mon, 16 May 1994 11:59:26 +0100 (CDT)
From: "Udo Munk" <udo@alien.mwc.com>
Cc: tech@mwc.com
In-Reply-To: <9405151327.AA00675@trxhome.mwc.com> from "Trixter / Independent" at May 15, 94 01:27:44 pm
X-Mailer: ELM [version 2.4 PL23]
Content-Type: text
Content-Length: 974       
Message-Id: <9405161159.AA04173@alien.mwc.com>
Status: RO

> In k09 (and possibly others as well), I was using ckermit with
> /dev/modem, which is a link to /dev/com2l.  In the middle of my
> connection, my uucico fired, calling to /dev/com2l.  Since the lock file
> was LCK..modem, uucico apparently didn't think there was a problem, not
> knowing that com2l and modem are links on my machine.  It output its
> init string, and then its hangup string, into my connection, therefore
> disconnecting me.
> 
> Needless to say, I deleted /dev/modem and do everything through
> /dev/com2l.  My question is:  Is this a (nonintentional) bug, or a
> (intentional) feature?  Are locks/lock files supposed to figure out
> links, or is this up to the user?

Ahhhrg, Coherent's lock files usually looks like LCK..2.0, they use
the major and minor device numbers instead of com?l or modem. Here
you see the reason why this is done, so whoever ported ckermit did
a lousy job.
-- 
Udo Munk  udo@mwc.com
Mark Williams Company
Technical Development


