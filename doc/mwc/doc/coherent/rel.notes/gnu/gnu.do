
From mwc!gateway!sdc.cs.boeing.com!tecdah1 Wed Sep  7 01:10:02 1994
Return-Path: <mwc!gateway!sdc.cs.boeing.com!tecdah1>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0qiGCo-0001AxC; Wed, 7 Sep 94 01:10 CDT
Received: by mwc.com (smail2.5.3-coh) id AA16627; 7 Sep 94 05:45:38 GMT (Wed)
Received: from atc.boeing.com by gateway.mwc.com with smtp
	(Smail3.1.28.1 #23) id m0qi7fE-00023LC; Tue, 6 Sep 94 16:02 CDT
Received: by atc.boeing.com (5.57) 
	id AA00463; Tue, 6 Sep 94 14:08:29 -0700
Received: from fury.sdc.cs.boeing.com by splinter.boeing.com with SMTP
	(16.6/16.2) id AA16279; Tue, 6 Sep 94 14:05:51 -0700
Received: from sneffels by sdc.cs.boeing.com (5.0/SMI-SVR4)
	id AA10970; Tue, 6 Sep 1994 14:05:18 +0800
From: tecdah1@sdc.cs.boeing.com (Dave Hough)
Received: by sneffels (4.1/Client-1.0)
	id AA07618; Tue, 6 Sep 94 14:05:18 PDT
Date: Tue, 6 Sep 94 14:05:18 PDT
Message-Id: <9409062105.AA07618@sneffels>
To: support@mwc.com
Subject: MWC gnu tools/compilers installs collide.
Cc: fred@mwc.com
Content-Length: 2447
Status: RO

I ran into a problem when I installed the MWC versions of the
gnu tools and gnu c compilers.  The releases collide in a couple
of places that are not well documented.

1) I installed the gnu tools first because it was smaller.  Once
   I got it copied in, I was able to make the required manual 
   adjustments to get man pages and info working.  And fortunately
   it suggests making copies of the index and info files before
   continuing, and I did.  Note that this install makes allowances
   for not being the first package installed.

2) When I installed the compilers, though, the installation assumed
   it was the one and only gnu product and stomped on the index and
   dir files I had just loaded.  I was able to splice the index file
   to get the combined man pages working (I think), but I haven't
   had time to repair info.

3) The files from the two kits are easy to tell apart because they
   have different dates.  From this I learned that the compilers
   release has replaced a number of programs from the tools package.

From this experience, I think that some changes need to be made:

1) The gnu compiler install procedure should be changed to behave
   more like the tools release, i.e. not depend on being the first
   package installed (check to see if the user already has an
   index file for gnu).

2) If the compiler package must be first, then it needs large, bold
   type somewhere in the release notes saying just that.  And there
   needs to be a workaround described if the user alread has gnu
   products installed.

3) I now have duplicate entries in the gnu.index file for the modules
   that were in both releases.  Will this cause me trouble or does 
   man use the first entry?  The release notes for gnu tools don't
   say, even though they give the procedure for creating the 
   composite file.

4) Because the two software releases have files in common, I now
   have a mixture of files on my system with two different release
   dates.  I have probably lucked out into the safest configuration,
   since the compiler related materials are more likely to be
   interdependent than the other tools.  But what if I had loaded in
   the other order?  The documentation doesn't tell me whether or
   not the combined file mix is usable.

The bottom line is that the release notes need some additional words
and the install scripts need to be made a little smarter.

/dave hough
tecdah1@sdc.cs.boeing.com 


From mwc!gateway!uunet.uu.net!umunk!udo Wed Sep  7 20:12:27 1994
Return-Path: <mwc!gateway!uunet.uu.net!umunk!udo>
Received: from mwc by lepanto.mwc.com with uucp
	(Smail3.1.28.1 #3) id m0qiY2M-0000GDC; Wed, 7 Sep 94 20:12 CDT
Received: by mwc.com (smail2.5.3-coh) id AA04147; 8 Sep 94 01:01:05 GMT (Thu)
Received: from rci.ripco.com by gateway.mwc.com with smtp
	(Smail3.1.28.1 #23) id m0qiX8K-0001zwC; Wed, 7 Sep 94 19:14 CDT
Received: from nanolon.gun.de by rci.ripco.com with smtp
	(Smail3.1.28.1 #9) id m0qiWxW-000JoXB; Wed, 7 Sep 94 19:03 CDT
Received: from umunk.UUCP (uucp@localhost) by nanolon.gun.de (8.6.8.1/8.6.6) with UUCP id CAA15213; Thu, 8 Sep 1994 02:00:40 +0200
Received: by umunk.uucp (Smail3.1.28.1 #2)
	id m0qiYIN-0000eJC; Thu, 8 Sep 94 01:28 +0200
Message-Id: <m0qiYIN-0000eJC@umunk.uucp>
Subject: Re: MWC gnu tools/compilers installs collide. (fwd)
To: fred@lepanto.mwc.com (Fred Butzen)
Date: Thu, 8 Sep 1994 01:28:58 +0100 (+0200)
From: "Udo Munk" <udo@umunk.GUN.de>
Cc: jrd@mwc.com
In-Reply-To: <m0qiMsV-0001AxC@lepanto.mwc.com> from "Fred Butzen" at Sep 7, 94 08:17:30 am
Organization: U. Munk's private BBS
X-Mailer: ELM [version 2.4 PL23]
MIME-Version: 1.0
Content-Type: text/plain; charset=US-ASCII
Content-Transfer-Encoding: 7bit
Content-Length: 5341      
Status: RO

> John, Udo - This user makes some good points about the release notes
> for these products.  I'd like to work with you to clarify the release
> notes on these points.  I think we can solve these problems through
> better documentation, without having to touch the bits.

I wouldn't suggest to touch the current bits as long as it isn't really
necessary. I'll definitely support the stuff, upgrade my releases herem
test them and make new distributions available. The upgrade to compiler
release 2.5.8 doesn't give much for Intel CPU's, I did it because the
X Consortium used this compiler release and I wanted to be as close as
possible to their development environment to avoid problems with that.

> Dave Hough writes:
> > From mwc!gateway!sdc.cs.boeing.com!tecdah1 Wed Sep  7 01:10:02 1994
> > From: tecdah1@sdc.cs.boeing.com (Dave Hough)
> > Date: Tue, 6 Sep 94 14:05:18 PDT
> > Message-Id: <9409062105.AA07618@sneffels>
> > To: support@mwc.com
> > Subject: MWC gnu tools/compilers installs collide.
> > Cc: fred@mwc.com
> > Content-Length: 2447
> > 
> > I ran into a problem when I installed the MWC versions of the
> > gnu tools and gnu c compilers.  The releases collide in a couple
> > of places that are not well documented.
> > 
> > 1) I installed the gnu tools first because it was smaller.  Once
> >    I got it copied in, I was able to make the required manual 
> >    adjustments to get man pages and info working.  And fortunately
> >    it suggests making copies of the index and info files before
> >    continuing, and I did.  Note that this install makes allowances
> >    for not being the first package installed.

Yes I know, that was the secound package I made and I learned of course
from the mistakes I made with the first one.

> > 2) When I installed the compilers, though, the installation assumed
> >    it was the one and only gnu product and stomped on the index and
> >    dir files I had just loaded.  I was able to splice the index file
> >    to get the combined man pages working (I think), but I haven't
> >    had time to repair info.

Yes, that really is a problem and it should be mentioned in the release
notes, that the GNU compiler package should be installed first. I'll
change it in the next release, so that it doesn't asume anymore, to be
the first package installed.

> > 3) The files from the two kits are easy to tell apart because they
> >    have different dates.  From this I learned that the compilers
> >    release has replaced a number of programs from the tools package.

The only common things in both packages are bison and flex, both are
the same release, so the system still is up to date. The different
dates simply comes from the fact that I made the packages at different
dates.

> > From this experience, I think that some changes need to be made:
> > 
> > 1) The gnu compiler install procedure should be changed to behave
> >    more like the tools release, i.e. not depend on being the first
> >    package installed (check to see if the user already has an
> >    index file for gnu).

Definitely, that will be different in the next release.

> > 2) If the compiler package must be first, then it needs large, bold
> >    type somewhere in the release notes saying just that.  And there
> >    needs to be a workaround described if the user alread has gnu
> >    products installed.

Right, do that for now, the big letters in the release notes. There is
no workaround other needed, as one always should have a secound copy
of the dir file in the GNU info directory and for the gnu.index file.

> > 3) I now have duplicate entries in the gnu.index file for the modules
> >    that were in both releases.  Will this cause me trouble or does 
> >    man use the first entry?  The release notes for gnu tools don't
> >    say, even though they give the procedure for creating the 
> >    composite file.

The duplicate entries doesn't cause trouble, I have them my self after
every installation of a device driver kit too. One realy should check
for the existence of such a file in the install script and remove the
old one first. Problem is that all GNU manual pages go into /usr/man/GNU
and there is one merged index for them all. I don't want to have a
different dir for every GNU package, this can become confusing with the
time, but probably this must be changed. Or can two index files exist,
which both points so files in one dir? Hm, probably the man command can
handle this, this would be a solution then, what do you think?

> > 4) Because the two software releases have files in common, I now
> >    have a mixture of files on my system with two different release
> >    dates.  I have probably lucked out into the safest configuration,
> >    since the compiler related materials are more likely to be
> >    interdependent than the other tools.  But what if I had loaded in
> >    the other order?  The documentation doesn't tell me whether or
> >    not the combined file mix is usable.

Yes, the mix is usable, no need to worry about that, the bits would be
the same when installed in the other order, only timestamps would be
different.

> > The bottom line is that the release notes need some additional words
> > and the install scripts need to be made a little smarter.

Yes, that is right.
-- 
Udo Munk  udo@mwc.com or udo@umunk.GUN.de, CIS: 100021,2515

