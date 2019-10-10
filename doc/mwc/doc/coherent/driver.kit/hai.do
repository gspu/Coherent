
From gort!hal  Thu Dec 23 18:00:20 1993 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA02379; 23 Dec 93 18:00:20 GMT (Thu)
Received: by mwc.com (smail2.5.3-coh) id AA25127; 23 Dec 93 17:56:57 GMT (Thu)
Received: by gort.mwc.com (smail2.5.3-coh) id AA05242; 23 Dec 93 11:48:16 CST (Thu)
Subject: Re: HAI_TAPE_SPEC, HAI_DISK_SPEC
To: chris@yavin.mwc.com (Christopher Sean Hilton)
From: Hal Snyder <hal@gort.mwc.com>
Cc: tech@gort.mwc.com
In-Reply-To: <9312211812.AA01755@yavin.mwc.com>; from "Christopher Sean Hilton" at Dec 21, 93 6:12 pm
X-Mailer: ELM [version 2.3 PL11]
Message-Id: <9312231148.AA05242@gort.mwc.com>
Date: 23 Dec 93 11:48:16 CST (Thu)
Status: RO

Chris - I am copying your message and my response to tech to see if
there is a consensus at MWC one way or the other.

Text from Christopher Sean Hilton:

> In the future I think I will be replacing the HAI_DISK_SPEC,
> HAI_TAPE_SPEC, etc variables with an array (32 elements maximum) of
> unsigned chars corresponding to the individual device type indexes
> returned by each SCSI devices inquiry command. Eventually, when a user
> adds a tape drive or a CD-ROM drive to his computer he won't have to
> rebuild the kernel to add support for it.
> 
> The process would go somthing like this:
> 
> Adapter init routine:
> 	do normal adapter init stuff.
> 	for id from 0 to 7
> 		do an inquiry command for this id.
> 		case inquiry results
> 		disk drive)
> 			install support for disk drive at this id.
> 		tape drive)
> 			install support for tape drive at this id.
> 		cd-rom drive)
> 			install support for cd-rom drive at this id.
> 		default)
> 			ask user to write support of widget at this id.
> 		end case
> 	end for

> In the /etc/brc script

> do an ioctl to get results of init's bus scan.
> 	mknod for all devices found in bus scan.

> This is what UnixWare does. We added a nine track reel to reel tape
> drive to our UnixWare machine and all we had to do was plug it in.

In the past we have got into no end of difficulty trying to auto-sense
customer equipment.  COHERENT is targeted for lots more platforms than
any of the other commercially available Unix-type OS's, and one sure way
to hose up hardware is to send it signals intended for some other kind
of hardware.

Just look at recent reports of standard sound board sensing utilities
(present in the install software for new PC games coming out) which
destroy your DOS partition if they find a Roland SCC-1 and your hard
disk is controlled by an AHA-1542x.

I guess things are a little safer if you're limiting you adventuring to
*inquiry* commands on a SCSI bus, but I'm still gun shy.
-- 
Hal Snyder, M.D.		hal@mwc.com			itis
Mark Williams Company     	(708)-291-6700 (voice)		foras
60 Revere Drive, Suite 250	(708)-291-6750 (fax)		rixsatis
Northbrook, IL 60062


