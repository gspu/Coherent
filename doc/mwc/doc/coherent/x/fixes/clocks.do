
From VMA.CC.ND.EDU!owner-coherent  Wed Jan 12 05:04:06 1994 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA00678; 12 Jan 94 05:04:06 GMT (Wed)
Received: by mwc.com (smail2.5.3-coh) id AA07518; 12 Jan 94 04:32:21 GMT (Wed)
Received: from VMA.CC.ND.EDU by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AA22921; Tue, 11 Jan 94 23:17:58 -0500
Message-Id: <9401120417.AA22921@relay1.UU.NET>
Received: from VMA.CC.ND.EDU by VMA.CC.ND.EDU (IBM VM SMTP V2R2)
   with BSMTP id 3870; Tue, 11 Jan 94 23:17:36 EST
Received: from VMA.CC.ND.EDU (NJE origin LISTSERV@IRISHVMA) by VMA.CC.ND.EDU (LMail V1.1d/1.7f) with BSMTP id 9520; Tue, 11 Jan 1994 23:17:36 -0500
Date:         Tue, 11 Jan 1994 14:01:31 GMT
Reply-To: bbigb.roch817@xerox.com
Sender: Coherent operating system <COHERENT@VMA.CC.ND.EDU>
Comments:     Warning -- original Sender: tag was news@SPECTRUM.XEROX.COM
From: Bruce Bigby <bbigb.roch817@XEROX.COM>
Subject:      Re: X-Windows and ATI ultra plus
Comments: To: coherent@vma.cc.nd.edu
To: Multiple recipients of list COHERENT <COHERENT@VMA.CC.ND.EDU>
Status: RO

In article mu6@fido.asd.sgi.com, bill@iris12.houst.sgi.com (Bill Richardson) writes:
>
>In article <2ggd9j$jf4@fido.asd.sgi.com>, bill@iris12.houst.sgi.com writes:
>>
>> In article <1994Jan5.142853.11710@spectrum.xerox.com>, bbigb.roch817@xerox.com writes:
>> > In article LHv@undergrad.math.uwaterloo.ca, pkilam@undergrad.math.uwaterloo.ca () writes:
>> > >
>> > >I have an ATI graphics ultra plus (ISA) with
>> > >2 MB of RAM. I am able to run X-windows for
>> > >coherent under 800x600 mode, but unable to run
>> > >under 1024x768 or 1024x768i modes. Did anyone know
>> > >what is the clock settings for the ultra card?
>> > >Or did I do something wrong?
>> > >
>> > >pkilam
>> > >
>> > >thanks
>> > >
>> >
>>      Here is part of my Xconfig file.  Im runnig Xfree 2.0 but this should
>> work fine i have not yet had a chance to try it on CohX.  It is "800x600" and
>> 8415/a ie., 1024x768 interlaced.
>>
>> Make sure your clock chip is the ati 18801-1 ,i think.  If you have any trouble
>> let me know.
>>
>>   Virtual       1024 768
>>   Viewport      0 0
>>   Modes         "1024x768" "800x600" # "640x480"
>>
>> Clocks 100 126 92 36 51 57 0 44 135 32 110 80 39 45 75 65 50 63 46 18 25 28 0 22
>>  67 16 55 40 19 23 37 33
>>
>> ModeDB
>> # name        clock   horizontal timing     vertical timing      flags
>>
>>   "1024x768"    44.9  1024 1048 1208 1264   768  776  784  817   Interlace
>>   "800x600"     32     776  776  896  965   585  634  637  643
>>
>> Bill
>
>Here is the real clocks for the Ultra Card running under CohX i used these so
>i know they work for me!
>Good luck!
>
>Clocks  18 22 35 28 36 44 50 56
>        30 32 37 39 40  0 75 65
>
>
>These clocks are for the ati wonder compatibility mode, ie the same clocks used
>for the wonder card.  These should work if not email me.!
>
>Bill    bill@houst.sgi.com
>       brichard@neosoft.com
>

For the ATI Graphics Ultra Plus/Pro, the default Coherent/X timings for
"800x600" and "640x480" already work.  However, the timings for "1024x768"
do not.  Here is the information that you need in your Xconfig for Coherent/X
to work with your ATI Graphics Ultra in "1024x768" mode as well.


        chipset "ati"
        videoRAM 1024k
        Modes "1024x768" "800x600" "640x480"
        Clocks 18 22 25 28 36 44 50 56 30 32 37 39 40 0 75 65
        Virtual 1024 768

        # ATI Graphics Ultra Plus/Pro/60 Hz/Non-Interlaced

        #Mode           horizontal timings      vertical timings
        "1024x768"      65 1024 1120 1264 1360  768 771 778 825

I prefer non-interlaced at 60 Hz over Interlaced at 87 Hz.  I have an NEC monitor
and it looks good at 60 Hz.

Also, you must set the "VGA Memory Size" of your Graphics Ultra Plus/Pro to
"1.0MB".  To do this, run the ATI "Install" utility and select the
"Set Power-Up Configuration" option; then select the "VGA Memory Size" option
and choose "1.0MB".  Escape from the menus and save the configuration; then
reboot into Coherent.  Good luck!

Bruce Bigby
bbigb.roch817@xerox.com
73514.727@compuserve.com



From VMA.CC.ND.EDU!owner-coherent  Mon Jan 31 18:24:44 1994 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA02355; 31 Jan 94 18:24:44 GMT (Mon)
Received: by mwc.com (smail2.5.3-coh) id AA08624; 31 Jan 94 16:53:52 GMT (Mon)
Received: from VMA.CC.ND.EDU by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwbac03561; Sat, 29 Jan 94 20:37:24 -0500
Message-Id: <9401300137.AAwbac03561@relay1.UU.NET>
Received: from VMA.CC.ND.EDU by VMA.CC.ND.EDU (IBM VM SMTP V2R2)
   with BSMTP id 8964; Sat, 29 Jan 94 20:36:52 EST
Received: from VMA.CC.ND.EDU (NJE origin LISTSERV@IRISHVMA) by VMA.CC.ND.EDU (LMail V1.1d/1.7f) with BSMTP id 2756; Sat, 29 Jan 1994 20:36:52 -0500
Date:         Sat, 22 Jan 1994 22:13:36 GMT
Reply-To: Coherent operating system <COHERENT@VMA.CC.ND.EDU>
Sender: Coherent operating system <COHERENT@VMA.CC.ND.EDU>
From: David Chapman <dchapman@NETCOM.COM>
Subject:      Re: a no-name ET4000 VGA card
Comments: To: coherent@vma.cc.nd.edu
To: Multiple recipients of list COHERENT <COHERENT@VMA.CC.ND.EDU>
Status: RO

In article <dchapmanCJyqE6.JB5@netcom.com> I wrote:
>Well, my Oak 077 chipset VGA card didn't run X, of course, so I ran
>down to my friendly local Fry's Electronics and picked up an ET4000
>card for $80.  (Nice to live in Silicon Valley... :-)
>
>This is truly a no-name card; the box and manual have no company
>name whatsoever.  The box just says "True Color Tseng Labs ET4000".

John K. Milstead (u77576@fmms.orl.mmc.com) replied via E-mail:
>Just a quick note, if you use the X386color -probeonly command, make
>sure your clock
>settings within the /usr/X11/lib/Xconfig file is commented out, then
>you should see
>calculated clocks displayed on your monitor.. hope this helps.

I did, and that got me partway there.  Thanks a lot for the tip!

Alas, here are the only numbers that came out:

25.2 28.3 31.5 36.0

Although X386color -probeonly reports 16 clocks, usually the other
12 are 0.0; on the rare occasion when I see something else it's
always one of these four numbers (plus or minus some roundoff error).

36.0 MHz corresponds to 800x600, so I re-computed the timing numbers
and now I have an 800x600 display that works well but flickers so badly
(56 Hz) I can't use it (I paid $1000 for the monitor because I'm so
sensitive to flicker).

So now what?  I *know* the card is capable of 1024x768x256; I can
bring it up that way in Windoze using that "other" operating system
that no one talks about.  I tried using the 1024x768 timing numbers
I'd already created (now that I have confidence in my calculations)
by replacing each of the 0.0 numbers with 72.0.  All made the screen
go bonkers.

So it looks like the MWC driver isn't capable of invoking the 72 MHz
clock.  The only clue I have is in the instructions for the "vmode"
program that came with the card:

    The following graphic (not text) modes can be displayed after
    setting the video clock to 72 MHz.  (Use "vmode 72m" and you
    can) set the display to any of the graphic modes listed below.

    VMODE     Modes     Vertical      Horizontal
    x Value   Affected  Refresh Rate  Frequency   Resolution
    72M       37        69.80 Hz      56.30 KHz   1024x768
    72M       38        69.80 Hz      56.30 KHz   1024x768

Then at the back of the manual it has a table with *three* entries
for modes 37 and 38:  37i, 37n, and 37 72M, 38i, 38n, and 38 72M.

Does this mean anything to anybody?  I haven't done any low-level
programming of VGA cards, always content to let someone else write
those libraries for me.  Certainly the Windows driver is capable
of invoking 1024x768 non-interlaced without running the "vmode"
program, so I'm still confused...
--
                David Chapman
                Chapman Consulting
                dchapman@netcom.com



From VMA.CC.ND.EDU!owner-coherent  Wed Feb 16 23:20:11 1994 remote from mwc
Received: by lepanto.mwc.com (smail2.5.3-coh) id AA01478; 16 Feb 94 23:20:11 GMT (Wed)
Received: by mwc.com (smail2.5.3-coh) id AA08618; 16 Feb 94 17:27:14 GMT (Wed)
Received: from VMA.CC.ND.EDU by relay1.UU.NET with SMTP 
	(5.61/UUNET-internet-primary) id AAwdna13522; Wed, 16 Feb 94 10:37:58 -0500
Message-Id: <9402161537.AAwdna13522@relay1.UU.NET>
Received: from VMA.CC.ND.EDU by VMA.CC.ND.EDU (IBM VM SMTP V2R2)
   with BSMTP id 3493; Wed, 16 Feb 94 10:37:00 EST
Received: from VMA.CC.ND.EDU (NJE origin LISTSERV@IRISHVMA) by VMA.CC.ND.EDU (LMail V1.1d/1.7f) with BSMTP id 5045; Wed, 16 Feb 1994 10:37:00 -0500
Date:         Wed, 16 Feb 1994 15:10:56 GMT
Reply-To: Coherent operating system <COHERENT@VMA.CC.ND.EDU>
Sender: Coherent operating system <COHERENT@VMA.CC.ND.EDU>
Comments:     Warning -- original Sender: tag was news@NEWS.YCC.YALE.EDU
From: Richard Nolde <NOLDE@MRCBS.MED.YALE.EDU>
Subject:      ATI Graphics Ultra and X386color
Comments: To: coherent@vma.cc.nd.edu
To: Multiple recipients of list COHERENT <COHERENT@VMA.CC.ND.EDU>
Status: RO

Current user of Coherent 4.2 and X:
        I thought it might be useful to add the results of my experimentation with
Coherent 4.2 and X386 to the body of knowledge being compiled about the ATI
video cards.   My specific hardware is a Mylex 386-33 DX with 20 MB of physical
ram, 2 Conner IDE harddisks (120 and 200 MB, the second being given over to Coherent)
and a 1.3 GIG Conner Fast SCSI II disk inhabitated by DOS /Windows.  I have the
older ATI Graphics Ultra, (NOT the pro or plus versions, ie MACH 8, not MACH 32)
which has the 28800 VGA chipset.  I ALSO have a Hercules compatible monochrome
card installed and a second monitor for programming debugging.  This works nicely
for additional virtual consoles by the way.  The main color monitor is an IDEK
5117 Multisync with 30.0 to 57.0 KHz horizontal and 50 to 90Hz vertical.
      If you want to use as much as possible of the addressable video memory
and still retain a correct aspect ratio, you can set a virtual display size and
or resolution of 832 x 624 to squeeze a few more pixels into the address space.
With my particular hardware, the 832x624 resolution is very nice but the card
generates too much flicker with the necessary timing parameters to center and
size it to fill the full display area.  I have optimized each resolution that
I can call up to fill all but a .25 to .5" border on all four sides of my 17"
monitor.  Also, I have very carefully measured the horizontal and vertical
dimensions of the XClock to make sure that a circle is really a circle, or is
at least as close as possible to one given the modulo 8 horizontal settings
requirement.

  Here are my timings etc from Xconfig:
___________________________________________________________________________
#Mouse configuration
# N.B. 9600 will cause X to dump out as soon as the mouse is moved
mouseman  "/dev/com2l"  BaudRate 1200

#Monochrome server
vga2
Modes "640x480" "800x600"
# Monochrome server probe returns the following but I use intergers to distinguish
# the mono clocks from the color clocks since there is a problem with the mono
# server and some of the clocks reported by the color server
# Clocks 25.2 28.3 0.4 22.4
Clocks 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
Virtual 832 624
Viewport 0 0
chipset "ati"

#Color server
vga256
Clocks  18 22 25 28 36 44 50 56 30 32 37 39 40 0 75 65
Virtual 832 624
Viewport 0 0
chipset "ati"

ModeDB
# mode id      DCF     HR  HS1   HS2   HFL       VR    VS1   VS2   VFL
#              clock    horizontal timing           vertical timing      flags
# DCF 4 is for the monochrome server, 28 for the color server
 "640x480"      4     640  696   792   800       480   490   492   525
 "640x480"     28     640  704   800   864       480   480   483   485

# DCF 10 is for the monochrome server, 39 for the color server
 "800x600"     10     800  832   896   965       600   603   637   645
 "800x600"     39     800  856   888  1095       600   603   605   608

# This only seems to work with the color server, mono rolls uncontrollably
# and unfortunately color is too flickerish to be desireable
# Using a higher dot clock doesn't seem to help either
 "832x634"     39     832  904  1096  1124       624   624   629   724
____________________________________________________________________________
    I have spent a fair amount of time trying higher dot clocks with no luck.
Can anyone explain what the higher clocks are good for and why using a higher
clock seems to create more flicker instead of less?  There is also a major
problem related to the mouse behavior that I can't figure out. ie.  When using
the color server, if you move the mouse the bitblt operation that draws the
X cursor blocks out a square behind the X that is not repainted when the cursor
moves on to the next location. Therefore, the C cursor acts like an eraser and
it is necessary to refresh the screen after every mouse operation.  In another
possibly related behavior, doing a directory listing in an xvt window causes
the screen to display the directory and then flash the background color over
the just displayed listing, rendering it next to useless.  The same color,
blue in my case, results from mouse movements.  Does anyone have any clues as
to what is happening here and how to correct this wierdness.  Also, am I correct
in assuming that any other virtual console sessions become unavailable once you
start up X.  I can't switch to a session on my monochrome monitor after I start
X like I can before starting X.  Is this the expected behavior?
   I have not heard from MWC after nearly a week since giving a detailed listing
of problems with my Conner FAST SCSI II CP31370 disk and the Adaptec 1542CF that
I use to drive it.  Does anyone know of further developments in the area of the
Adaptec driver, including SCSI tape support??
  Finally, who ever I spoke to at MWC suggested that more recent versions of the
OS2 boot manager can actually allow you to boot Coherent as one of its choices.
With my current version of Boot Manager, I can see the Coherent partitions but
attempting to boot them results in the ? prompt and no way to execute the
tertiary boot for the Coherent partition.  The MWC master boot manager does do
OK, but it would be nice not to have 2 boot managers.  Has anyone been able to
make this work?
  Richard Nolde

Nolde@mrcbs.med.yale.edu


