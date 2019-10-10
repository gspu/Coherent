
From VMA.CC.ND.EDU!owner-coherent  Mon May 16 18:03:30 1994 remote from mwc
Received: by lepanto.chi.il.us (smail2.5.3-coh) id AA15451; 16 May 94 18:03:30 GMT (Mon)
Received: by mwc.com (smail2.5.3-coh) id AA14438; 16 May 94 16:25:40 GMT (Mon)
Received: from uga.cc.uga.edu by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwqdq17825; Mon, 16 May 94 11:33:41 -0400
Message-Id: <9405161533.AAwqdq17825@relay1.UU.NET>
Received: from UGA.CC.UGA.EDU by uga.cc.uga.edu (IBM VM SMTP V2R2)
   with BSMTP id 9576; Mon, 16 May 94 11:35:03 EDT
Received: from UGA.CC.UGA.EDU (NJE origin LISTSERV@UGA) by UGA.CC.UGA.EDU (LMail V1.1d/1.7f) with BSMTP id 5143; Mon, 16 May 1994 11:22:47 -0400
Date:         Mon, 16 May 1994 11:11:44 -0400
Reply-To: Greg Comeau <comeau@csanta.attmail.com>
Sender: Coherent operating system <COHERENT%IRISHVMA.BITNET@uga.cc.uga.edu>
Comments:     Warning -- original Sender: tag was comeau@PANIX.COM
From: Greg Comeau <comeau@CSANTA.ATTMAIL.COM>
Subject:      Comeau C++ modification
Comments: To: coherent@vma.cc.nd.edu
To: Multiple recipients of list COHERENT <COHERENT%IRISHVMA.BITNET@uga.cc.uga.edu>
Status: RO

For those using Comeau C++ and variadic functions, please
create /usr/include/CC3.0/stdarg.h and insert the following into it:
#ifndef va_start
#include "/usr/include/stdarg.h"
#undef va_start
#define va_start(ap, pN) (ap = (va_list)((char *) & pN + __size_to_int (pN)))
#endif
--
       Comeau Computing, 91-34 120th Street, Richmond Hill, NY, 11418-3214
 Here:comeau@csanta.attmail.com / BIX:comeau or comeau@bix.com / CIS:72331,3421
    Voice:718-945-0009 / Fax:718-441-2310 / Prodigy: tshp50a / WELL: comeau


