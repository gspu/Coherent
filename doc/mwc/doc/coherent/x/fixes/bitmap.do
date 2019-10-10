
From VMA.CC.ND.EDU!owner-coherent  Fri Jul 22 18:29:16 1994 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA02236; 22 Jul 94 18:29:16 GMT (Fri)
Received: by mwc.com (smail2.5.3-coh) id AA09343; 22 Jul 94 16:34:12 GMT (Fri)
Received: from uga.cc.uga.edu by relay2.UU.NET with SMTP 
	id QQwzqa06141; Fri, 22 Jul 1994 05:13:48 -0400
Message-Id: <QQwzqa06141.199407220913@relay2.UU.NET>
Received: from UGA.CC.UGA.EDU by uga.cc.uga.edu (IBM VM SMTP V2R2)
   with BSMTP id 6734; Fri, 22 Jul 94 05:15:27 EDT
Received: from UGA.CC.UGA.EDU (NJE origin LISTSERV@UGA) by UGA.CC.UGA.EDU (LMail V1.1d/1.7f) with BSMTP id 4223; Fri, 22 Jul 1994 05:15:27 -0400
Date:         Fri, 22 Jul 1994 09:13:07 GMT
Reply-To: Coherent operating system <COHERENT%IRISHVMA.BITNET@uga.cc.uga.edu>
Sender: Coherent operating system <COHERENT%IRISHVMA.BITNET@uga.cc.uga.edu>
From: Todd Ruth <truth@WEBER.UCSD.EDU>
Subject:      bitmap in 640x480
Comments: To: coherent@vma.cc.nd.edu
To: Multiple recipients of list COHERENT <COHERENT%IRISHVMA.BITNET@uga.cc.uga.edu>
Status: RO

I tend to have more problems than solutions, but here's something that
might save some people some time in editres:

*form.point.vertDistance: -287
*form.point.horizDistance: 130
*form.curve.horizDistance: 130
*form.line.horizDistance: 130
*form.rectangle.horizDistance: 130
*form.filledRectangle.horizDistance: 130
*form.circle.horizDistance: 130
*form.filledCircle.horizDistance: 130
*form.floodFill.horizDistance: 130
*form.setHotSpot.horizDistance: 130
*form.clearHotSpot.horizDistance: 130
*form.undo.horizDistance: 130

Add that stuff to your Bitmap file in /usr/X11/lib/app* if you're using
X in 640x480.  It changes bitmap from being tall and narrow to being
short and wide.  My card is an old Paradise 1024, so I can't go higher
than 640x480 in color.  I added those lines to my Bitmap-color file.
I had to manually tell it to include the color file ifdef COLOR though
because xrdb always tells me "-:cannot open" or some such.  Anybody
else ever gotten that?  How can something not be able to open standard
input?  Oh, well.  Hope you find it useful.

                        - Todd Ruth
                        truth@ucsd.edu


