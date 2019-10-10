
From uu5.psi.com!pudding!mousse.rtr.com!jeff  Tue Apr 26 19:12:05 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA11042; 26 Apr 94 19:12:05 GMT (Tue)
Received: by mwc.com (smail2.5.3-coh) id AA17430; 26 Apr 94 19:08:01 GMT (Tue)
Received: from uu5.psi.com by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwnic16033; Tue, 26 Apr 94 13:39:05 -0400
Received: from pudding.UUCP by uu5.psi.com (5.65b/4.0.071791-PSI/PSINet) via UUCP;
        id AA12508 for ; Tue, 26 Apr 94 13:31:25 -0400
Received: from mousse.rtr.com by rtr.com (4.1/SMI-4.1)
	id AA21860; Tue, 26 Apr 94 13:04:03 EDT
Received: by mousse.rtr.com (4.1/SMI-4.1)
	id AA19266; Tue, 26 Apr 94 13:01:32 EDT
Date: Tue, 26 Apr 94 13:01:32 EDT
From: jeff@mousse.rtr.com (Jeff Moskow)
Message-Id: <9404261701.AA19266@mousse.rtr.com>
Subject: Re: networking
To: fred@lepanto.mwc.com
Cc: mike@mousse.rtr.com
Reply-To: jeff@rtr.com
Return-Receipt-To: jeff@rtr.com
Organization: Ready-to-Run Software, Inc.
Status: RO

>> Message 170:
>> From mwc!lepanto!fred@uunet.UU.NET Tue Apr 26 12:36:23 1994
>> Return-Path: <mwc!lepanto!fred@uunet.UU.NET>
>> Received: from mousse.rtr.com by rtr.com (4.1/SMI-4.1)
>> 	id AA21730; Tue, 26 Apr 94 12:36:21 EDT
>> Received: from rtr.com (pudding) by mousse.rtr.com (4.1/SMI-4.1)
>> 	id AA19044; Tue, 26 Apr 94 12:33:52 EDT
>> Received: from relay1.UU.NET by uu5.psi.com (5.65b/4.0.071791-PSI/PSINet) via SMTP;
>>         id AA02135 for jeff; Tue, 26 Apr 94 12:34:03 -0400
>> Received: from uucp3.uu.net by relay1.UU.NET with SMTP 
>> 	(5.61/UUNET-internet-primary) id AAwnhy22012; Tue, 26 Apr 94 12:33:56 -0400
>> Received: from mwc.UUCP by uucp3.uu.net with UUCP/RMAIL
>>         ; Tue, 26 Apr 1994 12:34:01 -0400
>> Received: by mwc.com (smail2.5.3-coh) id AA16306; 26 Apr 94 15:13:13 GMT (Tue)
>> Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA10206; 26 Apr 94 09:26:14 CDT (Tue)
>> Message-Id: <9404260926.AA10200.V1.5@lepanto.chi.il.us>
>> Date: 26 Apr 94 09:26:13
>> From: fred@lepanto.mwc.com (Fred Butzen)
>> To: jeff@rtr.com
>> Subject: networking
>> Status: RO
>> 
>> I'm giving some thought to assembling the manual for the networking package
>> that you are going to be putting together.  I realize that this matter is
>> still under negotiation between yourself and Bob; but I was wondering if you
>> could give me some idea of what's going to be in the package.  In particular,
>> I would want to know:
>> 
>> - the clients that will be supported (e.g., ftp, telnet, rlogin)

We're still working on a final list, but a good start would be

	ftp
	telnet
	rlogin
	ping

>> - functions that need to be documented (e.g., the sockets routines)

	mostly socket routines, most TCP/IP comms will be through sockets

>> - the structure of the networking package.  This will be used in a technical
>>   document to help users debug and tune their packages.
>> 

	This is still under evolution as we figure out the best way to
mate the code we have with the driver/kernel scheme in coherent.

>> One last question:  will your next release of X, should there be one, work
>> over your network?
>> 

A subsequent release of X will work over the network, if it was up to me, it
would be the next release, but Bob/timing/Udo/market forces, etc. will actually
determine the order in which things happen.

>> Thanks for your time in this matter.
>> 
>> Regards ...
>> 
>> fred@lepanto.chi.il.us      -- Fred Butzen --     Mark Williams Company

Regards,
	Jeff


PS:  There are a bunch of typo's in the Driver manual, after a few days I
started circling them in red/orange as I find them.  In a few weeks, I'll trade
you myu marked up copy for a clean one if you'd like.

>> =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
>> "In writing the biography of great men and women, printing the DNA sequence
>>  of their genome would not be a good place to start." -- Robert Cook-Degan
>> 
>> 

--
===============================================================================
                  Ready-to-Run Software, Inc.   
                 Software Porting Specialists.
                                                                             
email: jeff@rtr.com                     Jeff Moskow  
fax  : +1 508 692 9990                  Ready-to-Run Software, Inc.
voice: +1 508 692 9922                  4 Pleasant Street
                                        Forge Village, MA.  01886
                                        USA


Guys - Someone on the net posted a rumor that RTR's networking would
support only the NE2000.  I didn't know if it were true or not, so I
wrote to Jeff asking what physical devices would be supported.  He
replies as follows:

> Fred,
> 	We still don't know for sure, but we'll be doing some/all of the 
> following:
> 
> 	3c501.c
> 	3c503.c
> 	3c507.c
> 	3c509.c
> 	8390.c
> 	at1700.c
> 	depca.c
> 	eexpress.c
> 	hp.c
> 	lance.c
> 	ne.c
> 	smc-ultra.c
> 	wd.c
> 
> We'll actually be testing:
> 	one 3com (probably 3c501)
> 	one ne2000
> 	one WD8013
> 
> and possibly more.
> 

From drktwr2!louis  Sat May 28 00:21:07 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA00333; 28 May 94 00:21:07 GMT (Sat)
Received: by mwc.com (smail2.5.3-coh) id AA13855; 27 May 94 19:04:20 GMT (Fri)
Received: by drktwr2.mwc.com (smail2.5.3-coh) id AA00893; 27 May 94 12:08:09 CDT (Fri)
Subject: Re: your mail
To: development@mwc.com
Date: Fri, 27 May 1994 12:08:09 +0200 (CDT)
From: "Louis J. Giliberto" <louis@drktwr2.mwc.com>
Cc: rs@mwc.com
In-Reply-To: <9405270806.AA10891.V1.5@lepanto.chi.il.us> from "Fred Butzen" at May 27, 94 08:06:40 am
X-Mailer: ELM [version 2.4 PL23]
Content-Type: text
Content-Length: 538       
Message-Id: <9405271208.AA00893@drktwr2.mwc.com>
Status: RO

The following is ridiculous.  Does this mean he will be shipping
us drivers he hasn't tested?!?  What the fuck is that?!?!?

I sincerely hope I misunderstood this message.

-Louis

> > 	We still don't know for sure, but we'll be doing some/all of the 
> > following:
> > 
> > 	3c501.c
> > 	3c503.c
> > 	3c507.c
> > 	3c509.c
> > 	8390.c
> > 	at1700.c
> > 	depca.c
> > 	eexpress.c
> > 	hp.c
> > 	lance.c
> > 	ne.c
> > 	smc-ultra.c
> > 	wd.c
> > 
> > We'll actually be testing:
> > 	one 3com (probably 3c501)
> > 	one ne2000
> > 	one WD8013


