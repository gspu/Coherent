
From kcbbs.gen.nz!nbree  Fri Dec 17 05:03:37 1993 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA01665; 17 Dec 93 05:03:37 GMT (Fri)
Received: by mwc.com (smail2.5.3-coh) id AA11177; 17 Dec 93 04:43:45 GMT (Fri)
Received: from kcbbs.gen.nz (via [202.14.102.1]) by relay2.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AA06162; Thu, 16 Dec 93 18:41:21 -0500
Received: from localhost (nbree@localhost) by kcbbs.gen.nz (8.5/8.5) id MAA01421; Fri, 17 Dec 1993 12:19:49 +1300
From: Nigel Bree <nbree@kcbbs.gen.nz>
Message-Id: <199312162319.MAA01421@kcbbs.gen.nz>
Subject: Re: STREAMS
To: fred@lepanto.mwc.com (Fred Butzen)
Date: Fri, 17 Dec 93 12:19:43 GMT+12:00
Cc: hal@gort.mwc.com (Hal Snyder)
In-Reply-To: <9312160956.AA228.V1.5@lepanto.mwc.com>; from "Fred Butzen" at Dec 16, 93 9:56 am
X-Mailer: ELM [version 2.3 PL0]
Status: RO

Fred Butzen sez:
> Does COHERENT STREAMS support memory-mapped I/O?
				^^^^^^^^^^^^^^^^^^
By this I assume you mean, do drivers have the ability to move data from
the kernel-supplied buffers into random places in physical memory? There
are some other possible spins I could put on the highlighted phrase
depending on whether this is being asked purely from a driver-level or
from some other perspective.

Assuming that is the question, the answer (for now) depends on the details
of what is required; access to exotic memory locations such as VLB video
memory requires that the whole DDI/DKI configuration mechanism be in place
with kernel support, something that can't really be done until we have an
overhauled memory manager.

For simple stuff like accessing VGA memory, there is a macro that I use
in one of the private kernel headers called __PTOV () that can take a
physical address within the low RAM address range (which on the PC happens
to include some memory-mapped peripheral address space) and supply a
virtual address that can be used to access that physical memory.

However, I don't want to document this interface because I will probably
flesh it out somewhat as I develop demand paging, and this facility is
not one that has a portable equivalent. Furthermore, there may well be
something I can do with the mdevice/sdevice information that will provide
a proper framework for this problem in the general case.

I don't mind giving out the information on a need-to-know basis since
people may well need it, but the caveat is that the macro simply will
not be in the next release of Coherent, it is an interim 4.2 measure.

Were you asking the question in general terms or is this based on a
customer enquiry?





From kcbbs.gen.nz!nbree  Sat Dec 18 14:05:02 1993 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA05817; 18 Dec 93 14:05:02 GMT (Sat)
Received: by mwc.com (smail2.5.3-coh) id AA20746; 18 Dec 93 13:29:26 GMT (Sat)
Received: from kcbbs.gen.nz (via [202.14.102.1]) by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AA20647; Sat, 18 Dec 93 08:10:48 -0500
Received: from localhost (nbree@localhost) by kcbbs.gen.nz (8.5/8.5) id BAA14991; Sun, 19 Dec 1993 01:46:36 +1300
From: Nigel Bree <nbree@kcbbs.gen.nz>
Message-Id: <199312181246.BAA14991@kcbbs.gen.nz>
Subject: Re: STREAMS
To: fred@lepanto.mwc.com (Fred Butzen)
Date: Sun, 19 Dec 93 1:46:27 GMT+12:00
In-Reply-To: <9312170722.AA03278@lepanto.mwc.com>; from "Fred Butzen" at Dec 17, 93 7:22 am
X-Mailer: ELM [version 2.3 PL0]
Status: RO

Fred Butzen sez:
> > Fred Butzen sez:
> > > Does COHERENT STREAMS support memory-mapped I/O?
> > 				^^^^^^^^^^^^^^^^^^
> > By this I assume you mean, do drivers have the ability to move data from
> > the kernel-supplied buffers into random places in physical memory? There
> > are some other possible spins I could put on the highlighted phrase
> > depending on whether this is being asked purely from a driver-level or
> > from some other perspective.

> The scope of my question is considerably narrower.  In the DDI/DKI manual,
> it says that functions outb(), outl(), and outw() are meaningful only on
> implementations that support memory-mapped I/O.

No, that isn't what they say. I quote:
	This function may not be supported on all implementations because
	some implementations may not support I/O-mapped I/O.

See the difference?

I/O-mapped I/O means having a logically separate address space for "I/O
ports", something that these days very few architectures support, the
x86 being (as usual) the exception.  The main thing with I/O-mapped I/O
is that separate, special machine instructions are used for performing
I/O, and that the address space for I/O ports is not subject to any of
the advanced mechanisms that regular memory space is (caching, address
protection, address translation). 

> In our manual, we should
> say whether our implementation is such an implementation, rather than
> have the user grope for an answer.  Hence my question.

Since the functions have the identical names to the special x86 machine
opcodes that they allow access to, you have a big hint.  If the device
hardware manual for some item says that some control port is "addressed
at I/O address 0x330", then that is the I/O address for I/O-mapped I/O
using the in...  () and out...  () functions.  If it says "addressed at
memory location 0xFFFF4500", then the things I alluded to in my previous
message come into play, because someone then has to get a pointer to
that physical memory address.



