
From easix.gun.de!umunk!udo  Fri Jan 28 20:15:25 1994 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA02994; 28 Jan 94 20:15:25 GMT (Fri)
Received: by mwc.com (smail2.5.3-coh) id AA01334; 28 Jan 94 19:35:20 GMT (Fri)
Received: from easix.gun.de by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwatw22738; Fri, 28 Jan 94 04:01:50 -0500
Received: by easix.gun.de (Smail3.1.28.1)
	  from umunk with uucp
	  id <m0pPpyG-00015IA>; Fri, 28 Jan 94 09:58 GMT
Received: by umunk.GUN.de (smail2.5.3-coh) id AA13078; 28 Jan 94 09:44:3800
Message-Id: <9401280944.AA13075.V1.4@umunk.GUN.de>
Date: 28 Jan 94 09:44:34
From: udo@umunk.GUN.de (Udo Munk)
To: tech@mwc.com
To: tech@mwc.com
Subject: (fwd) BEWARE: 4.2 update hazardous to your sanity!
Newsgroups: comp.os.coherent
Organization: Udo Munk's private BBS
Status: RO

Path: umunk.GUN.de!easix!teralon!rrz.uni-koeln.de!news.dfn.de!news.dfn.de!zib-berlin.de!netmbx.de!Germany.EU.net!EU.net!uunet!news.crd.ge.com!agena!kranitz
From: kranitz@sol.med.ge.com (Wesley Kranitz  x7-4409)
Sender: usenet@crdnns.crd.ge.com (USENET News System)
Newsgroups: comp.os.coherent
Distribution: world
Subject: BEWARE: 4.2 update hazardous to your sanity!
Nntp-Posting-Host: agena.med.ge.com
X-Newsreader: TIN [version 1.2 PL1]
Message-ID: <CK7B4u.D51@crdnns.crd.ge.com>
Organization: Nowhere, Inc.
Date: Tue, 25 Jan 1994 19:50:54 GMT
Lines: 105


I just installed the 4.2 update over the weekend. I thought I would
relate some of the problems I had, so others could be warned. This all
happened late Friday night and Saturday morning, so mwc technical support
wasn't available.

   The documentation say atleast 10,000 blocks are needed. This is 
   only true if you have the man pages currently installed on your
   your system. This update will install the man pages whether you
   want them or not. You'll need 16 or 17 thousand blocks if don't
   currently have the man pages installed.


Anyways, I started the update and selected Normal polling mode. After
disk 1 completed and the system rebooted, I found myself in a reboot
loop. The last letter in the upper right hand corner being "I". Searched
all over my office and finally found my 4.0 notes. I needed alternate
polling, so I rebooted disk 1. Selected Alternate polling. Here is
where the trouble began. I get a message something to the effect that
it found a copy of old_coh.taz and do I want to overwrite it. I
enter "n" for no. It then askes "Do you want to save system files". I
type "n" for no and it responds "Not saving system files". The system
sits there for a bit and then says "Saving system Files" and after
a few minutes, tells me that I can find the saved files in old_coh.taz .
Sure enough, it has overwritten old_coh.taz anyways. It then goes on
and overwrites coh.B4.420 (My original 4.0 kernel) with the bad one
that the first install created effectively destroying any possibility
of returning to 4.0.

So I reboot the system and it starts reading the rest of the installation
disks. This is where I discover that it's going to read in the man pages
whether you currently have them installed or not. Run out of space reading
disk 3 (or 4, I'm at work and can't remember which one it was). Blow it
off when it asks for the next disk (4 or 5). Delete the man pages. Reboot.
Stick the last disk in. Reads it, wants more. Stick previous disk back in,
no dice, says it's already read. Nothing I can do to get it to go on. Blow
it off again. Find files Coh_420.1 to .5 . Delete software that I had giving
an extra 5000 blocks. Remove .2 to .5. Reboot and start reading in 
disk 2 to 5. Still wants more. Can't get it to go on. Blow off again and
and remove Coh_420.1 to .5 and start over by booting disk 1 (What the hell,
it's already creamed my old_coh.taz and my coh.B4.420). Go through whole
procedure again, gets to disk five, still wants more!!. Blow it off and
edit /etc/ brc so update only wants 1 disk. Reboot. Says it's done and 
goes into post processing.

When It's done, I find that out of 17000 blocks I started with, I have
about 1000 left. I Deleted the man pages and ended up with about 6000
blocks left. As it is, If I reinstall the software I deleted, I will 
have about 1000 blocks left. Certainly not enough to run my news system.

So be aware that this update will tie up about 10 or 11000 block of your
disk space after it's done (Not including the man pages).


Some addition note:

According the the Lexicon entry for header files you will find a list of
header files that are no longer in Coherent as of 4.2. However, the update
procedure doesn't delete them or even notify you that they should be
deleted. You wouldn't know this unless you happen to read the Lexicon
entry.

I also found that my printer no longer works under Coherent (Works under
DOS just fine and also worked under 4.0 just fine). The ready light
doesn't even come on. Page 31 of the release notes says this might be 
due to a missing lp driver. It says, and I quote, "For details, see the
Lexicon entries for idenable and idmkcoh in these notes". However, there
are no Lexicon entires  for these commands either in the notes or the
Lexicon itself. Must be in the man pages that I don't have space for!!!!

While the system seems to work fine now, I think MWC could have done
a better job on the update procedure:

  1) Tell around how much space will be left after the update. As it is, 
     I can no longer do development or download news at the same time.

  2) DON'T overwrite old_coh.taz anyways when I tell it NOT to and DON'T 
     save system files anyways when I tell it NOT to.

  3) DON'T overwrite coh.B4.420 without asking!!!!!

  4) If it doesn't find a /usr/man/COHERENT directory, ask if it should
     install the man pages.

  5) Describe EXACTLY how to restart the Installation, if the need arises!!

  6) FIX the second portion of the update (first reboot after disk 1)
     to correctly stop after disk 5. I think the fact that the brc
     script calls update and tells it there are 5 more disks might be the
     problem (Although I'm not entirely sure about that). The Release notes
     say (in the first boot section of Installation (The update section
     refers you there for information on that phase)) that the 5.25 inch
     distribution has 5 remaining disks after disk 1. The kit I received
     only had 4 remaining (marked 2 of 5 to 5 of 5). Maybe that's the
     problem.


I hope these comment will help others avoid the problems and
frustrations I had installing 4.2. What should have been easy, took
hours upon hours to complete.

Good Luck!


Wes


--
Udo Munk, Oberstr. 21, 41460 Neuss, Germany
privat: udo@umunk.GUN.de   CIS: 100021,2515
work:   udo@mwc.com


