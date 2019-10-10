
From uu5.psi.com!pudding!mousse.rtr.com!jeff  Tue May 10 18:21:00 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA02377; 10 May 94 18:21:00 GMT (Tue)
Received: by mwc.com (smail2.5.3-coh) id AA00561; 10 May 94 16:37:55 GMT (Tue)
Received: from uu5.psi.com by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwphp29065; Tue, 10 May 94 12:27:03 -0400
Received: from pudding.UUCP by uu5.psi.com (5.65b/4.0.071791-PSI/PSINet) via UUCP;
        id AA22135 for ; Tue, 10 May 94 12:07:47 -0400
Received: from mousse.rtr.com by rtr.com (4.1/SMI-4.1)
	id AA09365; Tue, 10 May 94 12:03:19 EDT
Received: by mousse.rtr.com (4.1/SMI-4.1)
	id AA07543; Tue, 10 May 94 12:02:25 EDT
Date: Tue, 10 May 94 12:02:25 EDT
From: jeff@mousse.rtr.com (Jeff Moskow)
Message-Id: <9405101602.AA07543@mousse.rtr.com>
Subject: Re: socket routines
To: fred@lepanto.mwc.com
Reply-To: jeff@rtr.com
Organization: Ready-to-Run Software, Inc.
Return-Receipt-To: jeff@rtr.com
Status: RO

>> Message 258:
>> From mwc!lepanto!fred@uunet.UU.NET Wed May  4 19:44:28 1994
>> Return-Path: <mwc!lepanto!fred@uunet.UU.NET>
>> Received: from mousse.rtr.com by rtr.com (4.1/SMI-4.1)
>> 	id AA14260; Wed, 4 May 94 19:44:22 EDT
>> Received: from rtr.com (pudding) by mousse.rtr.com (4.1/SMI-4.1)
>> 	id AA05197; Wed, 4 May 94 19:43:42 EDT
>> Received: from relay1.UU.NET by uu5.psi.com (5.65b/4.0.071791-PSI/PSINet) via SMTP;
>>         id AA29860 for jeff; Wed, 4 May 94 19:37:23 -0400
>> Received: from uucp6.UU.NET by relay1.UU.NET with SMTP 
>> 	(5.61/UUNET-internet-primary) id AAwomo24290; Wed, 4 May 94 19:37:21 -0400
>> Received: from mwc.UUCP by uucp6.UU.NET with UUCP/RMAIL
>>         ; Wed, 4 May 1994 19:37:26 -0400
>> Received: by mwc.com (smail2.5.3-coh) id AA05759; 4 May 94 23:11:55 GMT (Wed)
>> Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA04747; 4 May 94 17:12:23 CDT (Wed)
>> Message-Id: <9405041712.AA4744.V1.5@lepanto.chi.il.us>
>> Date: 4 May 94 17:12:22
>> From: fred@lepanto.mwc.com (Fred Butzen)
>> To: moskow@uunet.UU.NET
>> Status: RO
>> 
>> Jeff -
>> 
>> Our documentation currently includes descriptions of the following socket
>> routines:
>> 
>> 	accept()
>> 	bind()
>> 	connect()
>> 	endnetent()		-
>> 	endprotoent()		-
>> 	endservent()		-
>> 	getnetbyaddr()		-
>> 	getnetbyname()		-
>> 	getnetent()		-
>> 	getpeername()
>> 	getprotobyname()	-
>> 	getprotobynumb()	-
>> 	getprotoent()		-
>> 	getservbyname()		-
>> 	getservbyport()		-
>> 	getservent()		-
>> 	getsockname()
>> 	getsockopt()
>> 	listen()	
	recv()			+
	recvfrom()		+
>> 	select()		-
	send()			+
	sendto()		+
>> 	setnetent()		-
>> 	setprotoent()		-
>> 	setservent()		-
>> 	setsockopt()		
	shutdown()		+
>> 	socket()	
>> 
>> Please let me know which ones, if any, do not apply to your new library,
>> and which are missing.
>> 
  
Fred,

I've marked differences with a '+', (in my routines, not in your list) or
a '-' (in your list, not in my routines).

What I've been working with so far seems to be missing many of the get/set
routines, I'll look into it in the next few days (or early next week).

Regards,
	Jeff
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
