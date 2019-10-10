
From uu5.psi.com!pudding!mousse.rtr.com!gail  Tue Apr  5 20:17:53 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA00125; 5 Apr 94 20:17:53 GMT (Tue)
Received: by mwc.com (smail2.5.3-coh) id AA20907; 5 Apr 94 16:29:56 GMT (Tue)
Received: from uu5.psi.com by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwkig15890; Tue, 5 Apr 94 11:33:36 -0400
Received: from pudding.UUCP by uu5.psi.com (5.65b/4.0.071791-PSI/PSINet) via UUCP;
        id AA10854 for ; Tue, 5 Apr 94 11:18:27 -0400
Received: from mousse.rtr.com by rtr.com (4.1/SMI-4.1)
	id AA09897; Tue, 5 Apr 94 11:09:46 EDT
Received: by mousse.rtr.com (4.1/SMI-4.1)
	id AA02226; Tue, 5 Apr 94 11:08:14 EDT
Date: Tue, 5 Apr 94 11:08:14 EDT
From: gail@mousse.rtr.com (Gail Mignard)
Message-Id: <9404051508.AA02226@mousse.rtr.com>
Subject: Re:  Questions
To: fred@lepanto.mwc.com
Cc: gail@mousse.rtr.com
Reply-To: gail@rtr.com
Return-Receipt-To: gail@rtr.com
Organization: Ready-to-Run Software, Inc.
Status: RO

Morning,
Took yesterday off, It was great.

Here are some of the answers.  

gail


>> 
>> Server Problems
>> ===============
>> 
>> 1.  Communication between clients and servers doesn't work.  Programs that
>> use client/server structure don't work, period.  The programs don't run,
>> or even lock up the server; COHERENT is alive, and activity resumes if one
>> kills the server from a serial port.  Udo has test programs that demonstrate
>> this.

**	Fred, 
	since every XWindow client uses a client/server structure and
clearly, many clients are working.  If someone were to use Nigel's socket
implementation to connect to the current server, that would fail.  Could you
ask Udo to send copies of the test programs that demonstrate this?

>> 
>> 2.  The server will lock up without apparent reason.  Killing the server from
>> a serial port frees the console.

**	How often? When?  We've run servers for up to 4 weeks without locking
up.  Type of system? 

> 
>> 3.  Lack of driver support.  Users want Hercules support and support for a
>> wider range of VGA chip sets.

**	Version 2.1 will have more supported chip sets including Hercules.

>> 
>> 4.  The server does not perform video timing correctly in 1024x768 mode.
>> Most users have great difficulty in coaxing the server to run in 1024x768;
>> and even then, it flickers so much as to be unusable.  Udo has details
>> on bogus video timings.

**	We've been running 1024x768 here.  If you could ask Udo to forward the
details.  If you'd like a copy of our config file I can send it out to you.

>> 
>> 5.  All libraries should be compiled with the -VNDP option, so we can link
>> them with objects compiled with GNU C.  Udo can give Jeff a copy of our
>> new GNU development system.

**	This can be done for the next release.

>> 
>> 6.  On average, the executables are about 100K larger than Answer's.  Some
>> binaries are incredibly large, e.g., xlogo is > 500K.

**	?Answer == it is possible that they may have used a different compiler.

>> 
>> 7.  The server should use Berkeley-compatible socket emulation, as described
>> in the O'Reilly documents, instead of the cobbled-together emulation we use.
>> Our current COHERENT socket-emulation library will do the trick.  Udo has
>> details; he will work with Jeff to get this to work.

**	This can be done for the next release.  There was a problem when we 
last tried it.  Please note that ALL existing clients will have to be relinked
to work with the new server and new clients will not work with the old server.

> 
>> 8.  The server should support SIGWIN.  COHERENT now supports this.  Many
>> applications do not work - or do not work correctly - because the server
>> does not support this.

**	Another piece for the next release.  COHERENT did not support SIGWIN
last May/June when we finished the port.  

>> 
>> 9.  Users want to use a bus mouse.  Udo asks what is needed to get the
>> server to work with a bus mouse.

**	Bus Mouse support is available in 2.1

>> 
>> 10. Applications do not free memory properly.  When an application is killed
>> with SIGKILL, it does not free its memory at all.  When an application is
>> killed with SIGHUP or some other benign signal, it often does not free all
>> of its memory.  As X is used, memory leaks away and eventually is exhausted.

**	Is it the application or the server that does not free all of its memory?
It should be impossible for a dead process to hold onto memory, so I assume
that it is the server or OS that is having trouble.  This sounds a lot like
the bug that existed in the OS late last year, are you sure that it is in 
the X code?

	>> 
>> 11.  The server sometimes leaves garbage characters in the terminal's input
>> queue when it exits.  The user must type <ctrl-U> to erase them before he
>> continues.

**	This needs to be addressed in the keyboard driver.  Perhaps Louis can implement a quick clearing of the keyboard buffer when switching keyboard modes.

	>> 
>> Application Problems
>> ====================
>> 
>> 1.  Resizing xcalc crashes the X server.

**	We will make sure that this is fixed in the next release.

>> 
>> 2.  xbitmap does not work correctly.  In particular, loading a bit-mapped
>> image from within the application can lock up the server.
>> 
**	Could you send us an example?


>> 
>> fred@lepanto.chi.il.us      -- Fred Butzen --     Mark Williams Company
>> =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
>> If you are not confused, you do not understand the situation.
>> 						-- graffito seen in Belfast
>> 
>> 

--
===============================================================================
                  Ready-to-Run Software, Inc.   
                 Software Porting Specialists.

email: gail@rtr.com                     Gail Mignard 
fax  : +1 508 692 9990                  Ready-to-Run Software, Inc.
voice: +1 508 692 9922                  4 Pleasant Street
                                        Forge Village, MA.  01886
                                        USA


